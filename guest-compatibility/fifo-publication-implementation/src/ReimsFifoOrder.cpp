// Sequoia x86 AppleParavirtGPU producer StoreLoad ordering correction.
// Lilu load-time patch, deliberately independent from GPU capability/userland patches.
// Exact inspected function only; no polling, alternate completions, or runtime service.
#include <Headers/kern_api.hpp>
#include <Headers/plugin_start.hpp>
#include <os/log.h>

namespace {
constexpr size_t FunctionLength = 346;
const char *const Symbol = "__ZN27AppleParavirtVirtualChannel12submitBufferEP24IOAccelCommandDescriptor";
const uint8_t Find[] = {
 0x48,0x8b,0x83,0x20,0x02,0,0,0x43,0x8d,0x0c,0x34,0x89,0x08,
 0x48,0x8b,0x83,0x20,0x02,0,0,0x8b,0x40,0x04,0x89,0x83,0x14,0x01,0,0,
 0x44,0x39,0xe0,0x75,0x17
};
const uint8_t Replace[] = {
 0x48,0x8b,0x83,0x20,0x02,0,0,0x43,0x8d,0x0c,0x34,0x87,0x08,
 0x48,0x8b,0x83,0x20,0x02,0,0,0x8b,0x40,0x04,0x89,0x83,0x14,0x01,0,0,
 0x44,0x39,0xe0,0x75,0x17
};
static_assert(sizeof(Find)==34 && sizeof(Replace)==sizeof(Find),"Patch lengths");
const char *DriverPaths[] = {"/System/Library/Extensions/AppleParavirtGPU.kext/Contents/MacOS/AppleParavirtGPU"};
KernelPatcher::KextInfo Driver {
 "com.apple.driver.AppleParavirtGPU",DriverPaths,arrsize(DriverPaths),{true},{},KernelPatcher::KextInfo::Unloaded
};
// One result snapshot after the guest's read-only telemetry daemon has started.
// No FIFO inspection, completion changes, repeated timer, or repair action.
static uint32_t VerificationState=0;
static uint64_t VerifiedAddress=0;
static thread_call_t ReportCall;
void reportOnce(thread_call_param_t,thread_call_param_t){
 uint32_t state=__atomic_load_n(&VerificationState,__ATOMIC_ACQUIRE);
 uint64_t address=__atomic_load_n(&VerifiedAddress,__ATOMIC_ACQUIRE);
 os_log_error(OS_LOG_DEFAULT,"Paravirt Accelerator FIFO ordering ONE_SHOT state=%{public}u address=%{public}llx (7=verified,8=already-ordered)",state,static_cast<unsigned long long>(address));
 if(auto service=ADDPR(selfInstance)){
  service->setProperty("PublicationVerificationState",state,32);
  service->setProperty("PublicationOrdered",static_cast<uint32_t>(state==7||state==8),32);
 }
}
void loaded(void *,KernelPatcher &patcher,size_t index,mach_vm_address_t address,size_t size){
 if(index!=Driver.loadIndex)return;
 __atomic_store_n(&VerificationState,2,__ATOMIC_RELEASE);
 if(getKernelVersion()!=KernelVersion::Sequoia){
  __atomic_store_n(&VerificationState,3,__ATOMIC_RELEASE);
  SYSLOG("fifo","Paravirt Accelerator FIFO ordering: refused unsupported kernel");return;
 }
 mach_vm_address_t function=patcher.solveSymbol(index,Symbol);
 if(!function || function<address || size<FunctionLength || function-address>size-FunctionLength){
  __atomic_store_n(&VerificationState,4,__ATOMIC_RELEASE);
  SYSLOG("fifo","Paravirt Accelerator FIFO ordering: missing/bounds-invalid submitBuffer err=%d",patcher.getError());
  patcher.clearError();return;
 }
 auto bytes=reinterpret_cast<uint8_t *>(function);
 size_t matches=0,offset=0,already=0;
 for(size_t at=0;at+sizeof(Find)<=FunctionLength;at++){
  if(!memcmp(bytes+at,Find,sizeof(Find))){++matches;offset=at;}
  if(!memcmp(bytes+at,Replace,sizeof(Replace)))++already;
 }
 if(matches==0 && already==1){
  __atomic_store_n(&VerificationState,8,__ATOMIC_RELEASE);
  SYSLOG("fifo","Paravirt Accelerator FIFO ordering: already ordered at %llx",static_cast<unsigned long long>(function));return;
 }
 if(matches!=1 || already!=0 || offset!=0x10d){
  __atomic_store_n(&VerificationState,5,__ATOMIC_RELEASE);
  SYSLOG("fifo","Paravirt Accelerator FIFO ordering: refused unexpected code count=%lu offset=%lu",matches,offset);return;
 }
 KernelPatcher::LookupPatch patch{&Driver,Find,Replace,sizeof(Find),1};
 patcher.applyLookupPatch(&patch,bytes,FunctionLength);
 if(patcher.getError()!=KernelPatcher::Error::NoError || memcmp(bytes+offset,Replace,sizeof(Replace))){
  __atomic_store_n(&VerificationState,6,__ATOMIC_RELEASE);
  SYSLOG("fifo","Paravirt Accelerator FIFO ordering: patch verification FAILED err=%d",patcher.getError());
  patcher.clearError();return;
 }
 __atomic_store_n(&VerifiedAddress,static_cast<uint64_t>(function+offset+11),__ATOMIC_RELEASE);
 __atomic_store_n(&VerificationState,7,__ATOMIC_RELEASE);
 SYSLOG("fifo","Paravirt Accelerator FIFO ordering: VERIFIED mov-to-xchg at %llx",static_cast<unsigned long long>(function+offset+11));
 if(auto service=ADDPR(selfInstance)){
  service->setProperty("PublicationOrdered",static_cast<uint32_t>(1),32);
  service->setProperty("PublicationInstruction",static_cast<uint32_t>(0x0887),32);
 }
}
void start(){
 __atomic_store_n(&VerificationState,1,__ATOMIC_RELEASE);
 lilu.onKextLoadForce(&Driver,1,loaded);
 ReportCall=thread_call_allocate(reportOnce,nullptr);
 if(ReportCall){uint64_t deadline;clock_interval_to_deadline(60,kSecondScale,&deadline);thread_call_enter_delayed(ReportCall,deadline);}
}
const char *Off[]={"-reimsfifooff"};
const char *Debug[]={"-reimsfifodbg"};
}
PluginConfiguration ADDPR(config){
 xStringify(PRODUCT_NAME),parseModuleVersion(xStringify(MODULE_VERSION)),LiluAPI::AllowNormal,
 Off,arrsize(Off),Debug,arrsize(Debug),nullptr,0,KernelVersion::Sequoia,KernelVersion::Sequoia,start
};
extern "C" kern_return_t ADDPR(kern_start)(kmod_info_t *,void *);
extern "C" kern_return_t ADDPR(kern_stop)(kmod_info_t *,void *);
KMOD_EXPLICIT_DECL(org.reims.fieldnotes.ReimsFifoOrder,xStringify(MODULE_VERSION),ADDPR(kern_start),ADDPR(kern_stop))
