// Independently removable Sequoia Metal-profile compatibility experiment.
// Does not change queue ordering, protocol version, texture sizes or bounds checks.
#include <Headers/kern_api.hpp>
#include <Headers/plugin_start.hpp>
#include "UserPatch.hpp"
namespace {
const uint8_t Find[]={0x55,0x48,0x89,0xe5,0xb8,0x10,0x27,0,0,0x5d,0xc3};
const uint8_t Replace[]={0x55,0x48,0x89,0xe5,0xb8,0x11,0x27,0,0,0x5d,0xc3};
UserPatch::Patch Profile{
 "/System/Library/Extensions/AppleParavirtGPUMetal.bundle/Contents/MacOS/AppleParavirtGPUMetal",
 Find,Replace,sizeof(Find)
};
const char *DriverPaths[]={"/System/Library/Extensions/AppleParavirtGPU.kext/Contents/MacOS/AppleParavirtGPU"};
KernelPatcher::KextInfo Driver{"com.apple.driver.AppleParavirtGPU",DriverPaths,arrsize(DriverPaths),{true},{},KernelPatcher::KextInfo::Unloaded};
static thread_call_t Report;
void report(thread_call_param_t,thread_call_param_t){
 uint32_t routed=0,located=0,calls=0,patched=0;
 UserPatch::status(routed,located,calls,patched);
 if(auto service=ADDPR(selfInstance)){
  service->setProperty("ProfileRouteResult",routed,32);
  service->setProperty("ProfilePatternsLocated",located,32);
  service->setProperty("ProfilePagesPatched",patched,32);
  service->setProperty("ProfileValidationCalls",calls,32);
  service->setProperty("ExpectedProfile",static_cast<uint32_t>(10001),32);
  service->setProperty("TextureBoundsUnchanged",true);
  service->setProperty("WireVersionUnchanged",true);
 }
}
void patcherLoaded(void *,KernelPatcher &patcher){
 if(getKernelVersion()!=KernelVersion::Sequoia)return;
 UserPatch::route(patcher);
}
void driverLoaded(void *,KernelPatcher &,size_t index,mach_vm_address_t,size_t){
 if(index!=Driver.loadIndex || getKernelVersion()!=KernelVersion::Sequoia)return;
 UserPatch::load(&Profile,1);
 // One diagnostic snapshot only. No repair loop, GPU polling or fake completions.
 Report=thread_call_allocate(report,nullptr);
 if(Report){uint64_t deadline;clock_interval_to_deadline(45,kSecondScale,&deadline);thread_call_enter_delayed(Report,deadline);}
}
void start(){lilu.onPatcherLoadForce(patcherLoaded);lilu.onKextLoadForce(&Driver,1,driverLoaded);}
const char *Off[]={"-reimsmetaloff"};
const char *Debug[]={"-reimsmetaldbg"};
}
PluginConfiguration ADDPR(config){xStringify(PRODUCT_NAME),parseModuleVersion(xStringify(MODULE_VERSION)),LiluAPI::AllowNormal,
 Off,arrsize(Off),Debug,arrsize(Debug),nullptr,0,KernelVersion::Sequoia,KernelVersion::Sequoia,start};
extern "C" kern_return_t ADDPR(kern_start)(kmod_info_t *,void *);
extern "C" kern_return_t ADDPR(kern_stop)(kmod_info_t *,void *);
KMOD_EXPLICIT_DECL(org.reims.fieldnotes.ReimsSequoiaMetal,xStringify(MODULE_VERSION),ADDPR(kern_start),ADDPR(kern_stop))
