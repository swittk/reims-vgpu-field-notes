// Native x86 test of the observed producer conditional-doorbell protocol.
// Finite two-thread atomic experiment; never touches the VM, GPU, or guest RAM.
#include <atomic>
#include <thread>
#include <pthread.h>
#include <sched.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <algorithm>

struct alignas(64) Node {std::atomic<unsigned> written{4};std::atomic<unsigned> read{0};char padding[56];};
struct alignas(64) Gate {std::atomic<unsigned> start{0};std::atomic<unsigned> done{0};};
static Node node;
static Gate guestGate,hostGate;
static std::atomic<unsigned> guestObserved{0},hostObserved{0};
static inline void pause_cpu(){asm volatile("pause" ::: "memory");}
static void pin(int cpu){cpu_set_t cpus;CPU_ZERO(&cpus);CPU_SET(cpu,&cpus);pthread_setaffinity_np(pthread_self(),sizeof(cpus),&cpus);}
int main(int argc,char**argv){
 const unsigned loops=argc>1?std::strtoul(argv[1],nullptr,10):1000000;
 const bool fence=argc>2&&std::strcmp(argv[2],"guest-fence")==0;
 const bool exchange=argc>2&&std::strcmp(argv[2],"guest-exchange")==0;
 const bool doubleCheck=argc>3&&std::strcmp(argv[3],"double-check")==0;
 if(loops>10000000||!loops)return 2;
 cpu_set_t available;CPU_ZERO(&available);sched_getaffinity(0,sizeof(available),&available);
 int chosen[3]={-1,-1,-1},n=0;for(int i=0;i<CPU_SETSIZE&&n<3;i++)if(CPU_ISSET(i,&available))chosen[n++]=i;
 if(n<3)return 3;
 pin(chosen[0]);
 std::atomic<unsigned> hostLock{0};
 auto start=std::chrono::steady_clock::now();
 std::thread producer([&]{pin(chosen[1]);for(unsigned i=1;i<=loops;i++){
  while(guestGate.start.load(std::memory_order_acquire)!=i)pause_cpu();
  if(exchange)node.written.exchange(8,std::memory_order_seq_cst);
  else node.written.store(8,std::memory_order_relaxed);
  if(fence)std::atomic_thread_fence(std::memory_order_seq_cst);
  unsigned r=node.read.load(std::memory_order_relaxed);
  guestObserved.store(r,std::memory_order_relaxed);
  guestGate.done.store(i,std::memory_order_release);
 }});
 std::thread consumer([&]{pin(chosen[2]);for(unsigned i=1;i<=loops;i++){
  while(hostGate.start.load(std::memory_order_acquire)!=i)pause_cpu();
  node.read.store(4,std::memory_order_seq_cst);
  unsigned w=node.written.load(std::memory_order_acquire);
  if(doubleCheck&&w==4){
   unsigned expected=0;while(!hostLock.compare_exchange_weak(expected,1,std::memory_order_acquire)){expected=0;pause_cpu();}
   w=node.written.load(std::memory_order_acquire);
   hostLock.store(0,std::memory_order_release);
  }
  hostObserved.store(w,std::memory_order_relaxed);
  hostGate.done.store(i,std::memory_order_release);
 }});
 unsigned lost=0,notify=0,observed=0;
 for(unsigned i=1;i<=loops;i++){
  node.written.store(4,std::memory_order_relaxed);node.read.store(0,std::memory_order_relaxed);
  if(i&1){guestGate.start.store(i,std::memory_order_release);hostGate.start.store(i,std::memory_order_release);}
  else{hostGate.start.store(i,std::memory_order_release);guestGate.start.store(i,std::memory_order_release);}
  while(guestGate.done.load(std::memory_order_acquire)!=i||hostGate.done.load(std::memory_order_acquire)!=i)pause_cpu();
  unsigned g=guestObserved.load(std::memory_order_relaxed),h=hostObserved.load(std::memory_order_relaxed);
  if(g!=4&&h==4)lost++;
  if(g==4)notify++;if(h==8)observed++;
 }
 producer.join();consumer.join();
 auto seconds=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
 std::printf("{\"guestExchange\":%s,\"loops\":%u,\"guestFence\":%s,\"hostDoubleCheck\":%s,\"lostWakeups\":%u,\"notified\":%u,\"hostSawNewWritten\":%u,\"seconds\":%.3f,\"cpus\":[%d,%d,%d]}\n",exchange?"true":"false",loops,fence?"true":"false",doubleCheck?"true":"false",lost,notify,observed,seconds,chosen[0],chosen[1],chosen[2]);
 return 0;
}
