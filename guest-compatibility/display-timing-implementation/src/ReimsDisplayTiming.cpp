// Exact Sequoia guest timing-contract experiment; no fixed refresh rate.
#include <Headers/kern_api.hpp>
#include <Headers/plugin_start.hpp>
#include <IOKit/graphics/IOGraphicsTypes.h>
#include <libkern/c++/OSArray.h>
#include <libkern/c++/OSData.h>
#include <libkern/c++/OSDictionary.h>
#include <libkern/c++/OSNumber.h>
#include "exact_code.hpp"
namespace {
using TimingFn = IOReturn (*)(IORegistryEntry *, int, IOTimingInformation *);
static TimingFn Original = nullptr;
bool copyTiming(IORegistryEntry *fb, uint32_t mode, IODetailedTimingInformationV2 &out) {
    OSObject *owned = fb->copyProperty("IOFBConfig");
    auto cfg = OSDynamicCast(OSDictionary, owned);
    auto modes = cfg ? OSDynamicCast(OSArray, cfg->getObject("IOFBModes")) : nullptr;
    bool found = false;
    if (modes && modes->getCount() <= 256) for (unsigned i = 0; i < modes->getCount(); ++i) {
        auto entry = OSDynamicCast(OSDictionary, modes->getObject(i));
        auto id = entry ? OSDynamicCast(OSNumber, entry->getObject("ID")) : nullptr;
        if (!id || id->unsigned32BitValue() != mode) continue;
        auto bytes = OSDynamicCast(OSData, entry->getObject("TM"));
        if (bytes && bytes->getLength() == sizeof(out)) { memcpy(&out, bytes->getBytesNoCopy(), sizeof(out)); found = true; }
        break;
    }
    if (owned) owned->release();
    return found;
}
IOReturn timing(IORegistryEntry *fb, int mode, IOTimingInformation *info) {
    IOReturn result = Original(fb, mode, info);
    if (result != kIOReturnSuccess || !info || (info->flags & kIODetailedTimingValid)) return result;
    IODetailedTimingInformationV2 t {};
    if (!copyTiming(fb, static_cast<uint32_t>(mode), t)) return result;
    uint64_t h = uint64_t(t.horizontalActive) + t.horizontalBlanking;
    uint64_t v = uint64_t(t.verticalActive) + t.verticalBlanking;
    if (!t.horizontalActive || !t.verticalActive || h > 65535 || v > 65535) return result;
    if (!t.pixelClock || t.minPixelClock != t.pixelClock || t.maxPixelClock != t.pixelClock) return result;
    if (t.pixelClock < h * v * 20 || t.pixelClock > h * v * 240 || t.verticalBlankingExtension) return result;
    for (auto &reserved : t.__reservedA) reserved = 0;
    memcpy(&info->detailedInfo.v2, &t, sizeof(t));
    info->flags |= kIODetailedTimingValid;
    fb->setProperty("ReimsFixedTimingMode", static_cast<uint32_t>(mode), 32);
    fb->setProperty("ReimsFixedTimingPeriodNS", (h * v * 1000000000ULL) / t.pixelClock, 64);
    return result;
}
const char *const Symbol = "__ZN24AppleParavirtFramebuffer27getTimingInfoForDisplayModeEiP19IOTimingInformation";
const char *Paths[] = {"/System/Library/Extensions/AppleParavirtGPU.kext/Contents/MacOS/AppleParavirtGPU"};
KernelPatcher::KextInfo Driver {"com.apple.driver.AppleParavirtGPU", Paths, arrsize(Paths), {true}, {}, KernelPatcher::KextInfo::Unloaded};
void loaded(void *, KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
    if (index != Driver.loadIndex || getKernelVersion() != KernelVersion::Sequoia) return;
    auto function = patcher.solveSymbol(index, Symbol);
    if (!function || function < address || size < sizeof(ExactCode) || function - address > size - sizeof(ExactCode)) {
        patcher.clearError(); SYSLOG("timing", "Refused missing or out-of-bounds driver method"); return;
    }
    if (memcmp(reinterpret_cast<const void *>(function), ExactCode, sizeof(ExactCode))) {
        SYSLOG("timing", "Refused unknown AppleParavirtFramebuffer implementation"); return;
    }
    KernelPatcher::RouteRequest request(Symbol, timing, Original);
    bool ok = patcher.routeMultiple(index, &request, 1, address, size);
    SYSLOG("timing", "Exact guest detailed-timing route installed=%d error=%d", ok, patcher.getError());
    patcher.clearError();
}
void start() { lilu.onKextLoadForce(&Driver, 1, loaded); }
const char *Off[] = {"-reimstimingoff"};
const char *Debug[] = {"-reimstimingdbg"};
}
PluginConfiguration ADDPR(config) {
    xStringify(PRODUCT_NAME), parseModuleVersion(xStringify(MODULE_VERSION)), LiluAPI::AllowNormal,
    Off, arrsize(Off), Debug, arrsize(Debug), nullptr, 0, KernelVersion::Sequoia, KernelVersion::Sequoia, start
};
extern "C" kern_return_t ADDPR(kern_start)(kmod_info_t *, void *);
extern "C" kern_return_t ADDPR(kern_stop)(kmod_info_t *, void *);
KMOD_EXPLICIT_DECL(org.reims.fieldnotes.ReimsDisplayTiming, xStringify(MODULE_VERSION), ADDPR(kern_start), ADDPR(kern_stop))
static_assert(sizeof(IODetailedTimingInformationV2) == 160, "Exact timing ABI");
static_assert(sizeof(ExactCode) == 52, "Exact inspected method");
