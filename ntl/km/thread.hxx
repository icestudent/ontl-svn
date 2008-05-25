/**\file*********************************************************************
 *                                                                     \brief
 *  Kernel Threads
 *
 ****************************************************************************
 */

#ifndef NTL__KM_THREAD
#define NTL__KM_THREAD

#include "basedef.hxx"
#include "object.hxx"
#include "apc.hxx"
#include "handle.hxx"
#include "../nt/teb.hxx"
#include "../device_traits.hxx"

namespace ntl {
namespace km {

using nt::client_id;

struct ktrap_frame;
struct kqueue;
struct kthread;


NTL__EXTERNAPI
kthread * __stdcall
  KeGetCurrentThread();

///\note XP+ only, use KeGetCurrentThread instead
NTL__EXTERNAPI
kthread * __stdcall
  PsGetCurrentThread();

NTL__EXTERNAPI
legacy_handle __stdcall
  PsGetCurrentThreadId();

NTL__EXTERNAPI
ntstatus __stdcall
  PsLookupThreadByThreadId(
    legacy_handle ThreadId,
    kthread * *   Thread
    );

static inline
kthread * lookup_tread(legacy_handle threads_id)
{
  kthread * thread;
  return nt::success(PsLookupThreadByThreadId(threads_id, &thread))
    ? thread : 0;
}

static __forceinline
void dereference_object(kthread * thread)
{
  ObfDereferenceObject(thread);
}


typedef
nt::tib * __stdcall
  get_thread_teb_t(const kthread * Thread);
  
NTL__EXTERNAPI
get_thread_teb_t PsGetThreadTeb;


struct kwait_block
{
  list_entry    WaitListEntry;
  kthread *     Thread;
  void *        Object;
  kwait_block * NextWaitBlock;
  uint16_t      WaitKey;
  uint16_t      WaitType;
};


struct kaffinity
{
  kaffinity() {}
  kaffinity(uintptr_t affinity) : affinity(affinity) { }
  uintptr_t affinity;
};


struct kpcr : public nt::tib
{
  //nt::tib NtTib;
  struct kpcr * SelfPcr;
  struct kprcb* Prcb;
  kirql         Irql;
  uint32_t      IRR;
  uint32_t      IrrActive;
  uint32_t      IDR;
  void *        KdVersionBlock;
  struct kidtentry* IDT;
  struct kgdtentry* GDT;
  struct ktss*      TSS;
  uint16_t    MajorVersion;
  uint16_t    MinorVersion;
  kaffinity   SetMember;
  uint32_t    StallScaleFactor;
  uint8_t     SpareUnused;
  uint8_t     Number;
  uint8_t     Spare0;
  uint8_t     SecondLevelCacheAssociativity;
  uint32_t    VdmAlert;
  uint32_t    KernelReserved[14];
  uint32_t    SecondLevelCacheSize;
  uint32_t    HalReserved[16];
  
  uint32_t    InterruptMode;
  char        Spare1;
  uint32_t    KernelReserved2[17];
//  struct _KPRCB PrcbData;
  struct kprcb* prcb_data() { return reinterpret_cast<struct kprcb*>(this+1); }
};
STATIC_ASSERT(sizeof(kpcr)==(0xd70-0xc50));


/// Common KTHREAD region
struct kthread
{
  /* 0x00 */  dispatcher_header Header;
  /* 0x10 */  list_entry        MutantListHead;
  /* 0x18 */  void *            InitialStack;
  /* 0x1c */  void *            StackLimit;
  /* 0x20 */  nt::tib *         Teb;
  /* 0x24 */  void *            TlsArray;
  /* 0x28 */  void *            KernelStack;
  /* 0x2c */  bool              DebugActive;
  /* 0x2d */  uint8_t           State;
  /* 0x2e */  bool              Alerted[2/*UserMode+1*/];
  /* 0x30 */  uint8_t           Iopl;
  /* 0x31 */  uint8_t           NpxState;
  /* 0x32 */  char              Saturation;
  /* 0x33 */  int8_t            Priority;
  /* 0x34 */  kapc_state        ApcState;
  /* 0x4c */  uint32_t          ContextSwitches;

  static 
  kthread * get_current()
  {
#ifndef NTL_SUPPRESS_IMPORT
    return KeGetCurrentThread();
#else
    return *reinterpret_cast<kthread**>(/*__readfsdword(0x124)*/0xFFDFF124);    
#endif
  }

};
STATIC_ASSERT(sizeof(kthread) == 0x50);


/// 2K
struct kthread50 : kthread
{
  /* 0x54 */  intptr_t          WaitStatus;
  /* 0x58 */  kirql             WaitIrql;
  /* 0x59 */  kprocessor_mode   WaitMode;
  /* 0x5a */  bool              WaitNext;
  /* 0x5b */  unsigned char     WaitReason;
  /* 0x5c */  kwait_block *     WaitBlockList;
  /* 0x60 */  list_entry        WaitListEntry;
  /* 0x60 */  single_list_entry SwapListEntry;
  /* 0x68 */  uint32_t          WaitTime;
  /* 0x6c */  int8_t            BasePriority;
  /* 0x6d */  uint8_t           DecrementCount;
  /* 0x6e */  int8_t            PriorityDecrement;
  /* 0x6f */  int8_t            Quantum;
  /* 0x70 */  kwait_block       WaitBlock[4];
  /* 0xd0 */  void *            LegoData;
  /* 0xd4 */  unsigned long     KernelApcDisable;
  /* 0xd8 */  kaffinity         UserAffinity;
  /* 0xdc */  bool              SystemAffinityActive;
  /* 0xdd */  int8_t            PowerState;
  /* 0xde */  kirql             NpxIrql;
  /* 0xdf */  int8_t            InitialNode;
  /* 0xe0 */  void *            ServiceTable;
  /* 0xe4 */  kqueue *          Queue;
  /* 0xe8 */  kspin_lock        ApcQueueLock;
  /* 0xf0 */  ktimer            Timer;
  /* 0x118 */ list_entry        QueueListEntry;
  /* 0x124 */ kaffinity         Affinity;
  /* 0x128 */ bool              Preempted;
  /* 0x129 */ bool              ProcessReadyQueue;
  /* 0x12a */ bool              KernelStackResident;
  /* 0x12b */ int8_t            NextProcessor;
  /* 0x12c */ void *            CallbackStack;
  /* 0x130 */ void *            Win32Thread;
  /* 0x134 */ ktrap_frame *     TrapFrame;
  /* 0x138 */ kapc_state *      ApcStatePointer[2];
  /* 0x140 */ kprocessor_mode   PreviousMode;
  /* 0x141 */ bool              EnableStackSwap;
  /* 0x142 */ bool              LargeStack;
  /* 0x143 */ unsigned char     ResourceIndex;
  /* 0x144 */ uint32_t          KernelTime;
  /* 0x148 */ uint32_t          UserTime;
  /* 0x14c */ kapc_state        SavedApcState;
  /* 0x164 */ bool              Alertable;
  /* 0x165 */ uint8_t           ApcStateIndex;
  /* 0x166 */ bool              ApcQueueable;
  /* 0x167 */ bool              AutoAlignment;
  /* 0x168 */ void *            StackBase;
  /* 0x16c */ kapc              SuspendApc;
  /* 0x19c */ ksemaphore        SuspendSemaphore;
  /* 0x1b0 */ list_entry        ThreadListEntry;
  /* 0x1b8 */ char              FreezeCount;
  /* 0x1b9 */ char              SuspendCount;
  /* 0x1ba */ uint8_t           IdealProcessor;
  /* 0x1bb */ bool              DisableBoost;
}; // struct ktread50
STATIC_ASSERT(sizeof(kthread50) == 0x1b8);


/// XP SP2
struct kthread51 : kthread
{
  /* 0x50 */  unsigned char     IdleSwapBlock;  /// new to XP
  /* 0x51 */  unsigned char     Spare0[3];      /// new to XP
  /* 0x54 */  intptr_t          WaitStatus;
  /* 0x58 */  kirql             WaitIrql;
  /* 0x59 */  kprocessor_mode   WaitMode;
  /* 0x5a */  bool              WaitNext;
  /* 0x5b */  unsigned char     WaitReason;
  /* 0x5c */  kwait_block *     WaitBlockList;
  /* 0x60 */  list_entry        WaitListEntry;
  /* 0x60 */  single_list_entry SwapListEntry;
  /* 0x68 */  uint32_t          WaitTime;
  /* 0x6c */  int8_t            BasePriority;
  /* 0x6d */  uint8_t           DecrementCount;
  /* 0x6e */  int8_t            PriorityDecrement;
  /* 0x6f */  int8_t            Quantum;
  /* 0x70 */  kwait_block       WaitBlock[4];
  /* 0xd0 */  void *            LegoData;
  /* 0xd4 */  unsigned long     KernelApcDisable;
  /* 0xd8 */  kaffinity         UserAffinity;
  /* 0xdc */  bool              SystemAffinityActive;
  /* 0xdd */  int8_t            PowerState;
  /* 0xde */  kirql             NpxIrql;
  /* 0xdf */  int8_t            InitialNode;
  /* 0xe0 */  void *            ServiceTable;
  /* 0xe4 */  kqueue *          Queue;
  /* 0xe8 */  kspin_lock        ApcQueueLock;
  /* 0xf0 */  ktimer            Timer;
  /* 0x118 */ list_entry        QueueListEntry;
  /* 0x120 */ kaffinity         SoftAffinity; /// new to XP
  /* 0x124 */ kaffinity         Affinity;
  /* 0x128 */ bool              Preempted;
  /* 0x129 */ bool              ProcessReadyQueue;
  /* 0x12a */ bool              KernelStackResident;
  /* 0x12b */ int8_t            NextProcessor;
  /* 0x12c */ void *            CallbackStack;
  /* 0x130 */ void *            Win32Thread;
  /* 0x134 */ ktrap_frame *     TrapFrame;
  /* 0x138 */ kapc_state *      ApcStatePointer[2];
  /* 0x140 */ kprocessor_mode   PreviousMode;
  /* 0x141 */ bool              EnableStackSwap;
  /* 0x142 */ bool              LargeStack;
  /* 0x143 */ unsigned char     ResourceIndex;
  /* 0x144 */ uint32_t          KernelTime;
  /* 0x148 */ uint32_t          UserTime;
  /* 0x14c */ kapc_state        SavedApcState;
  /* 0x164 */ bool              Alertable;
  /* 0x165 */ uint8_t           ApcStateIndex;
  /* 0x166 */ bool              ApcQueueable;
  /* 0x167 */ bool              AutoAlignment;
  /* 0x168 */ void *            StackBase;
  /* 0x16c */ kapc              SuspendApc;
  /* 0x19c */ ksemaphore        SuspendSemaphore;
  /* 0x1b0 */ list_entry        ThreadListEntry;
  /* 0x1b8 */ char              FreezeCount;
  /* 0x1b9 */ char              SuspendCount;
  /* 0x1ba */ uint8_t           IdealProcessor;
  /* 0x1bb */ bool              DisableBoost;
}; // struct ktread51
STATIC_ASSERT(sizeof(kthread51) == 0x1c0);


typedef
void __stdcall
  kstart_routine_t(
    void * StartContext
    );


class system_tread;

}//namspace km


template<>
struct device_traits<km::system_tread> : private device_traits<>
{
  enum access_mask
  {
    terminate                 = 0x0001,
    suspend_resume            = 0x0002,
    alert                     = 0x0004,
    get_context               = 0x0008,
    set_context               = 0x0010,
    set_information           = 0x0020,
    set_limited_information   = 0x0400,
    query_limited_information = 0x0800,
  #if 0//(NTDDI_VERSION >= NTDDI_LONGHORN)
    all_access                = standard_rights_required | synchronize | 0xFFFF,
  #else                                   
    all_access                = standard_rights_required | synchronize | 0x3FF,
  #endif
  };
};


namespace km {

NTL__EXTERNAPI
ntstatus __stdcall
  PsCreateSystemThread(
    handle *            ThreadHandle,
    uint32_t            DesiredAccess,
    object_attributes * ObjectAttributes,
    legacy_handle       ProcessHandle,
    client_id *         ClientId,
    kstart_routine_t *  StartRoutine,
    void *              StartContext
    );

NTL__EXTERNAPI
ntstatus __stdcall
  PsTerminateSystemThread(
    ntstatus  ExitStatus
    );


class system_tread : public handle, public device_traits<system_tread>
{
  ////////////////////////////////////////////////////////////////////////////
  public:

    typedef kstart_routine_t start_routine_t;

    explicit
    system_tread(
        start_routine_t *   start_routine,
        void *              start_context   = 0,
        object_attributes * oa              = 0,
        access_mask         desired_access  = all_access,
        legacy_handle       process_handle  = 0,
        client_id *         client          = 0)
    {
      create(this, start_routine, start_context, oa, desired_access,
              process_handle, client);
    }
      
    static
    ntstatus
      create(
        handle *            thread_handle,
        start_routine_t *   start_routine,
        void *              start_context   = 0,
        object_attributes * oa              = 0,
        access_mask         desired_access  = all_access,
        legacy_handle       process_handle  = 0,
        client_id *         client          = 0)
    {
      return PsCreateSystemThread(thread_handle, desired_access, oa, process_handle,
                                  client, start_routine, start_context);
    }

    static
    void exit(ntstatus st)
    {
      PsTerminateSystemThread(st);
    }

};//

NTL__EXTERNAPI
ntstatus __stdcall
  ZwYieldExecution();

static inline
ntstatus yield_execution()
{
  return ZwYieldExecution();
}

namespace this_thread {

using km::system_tread;
  
void yield() { km::yield_execution(); }

}//namespace this_thread

}//namspace km

using namespace km::this_thread;

}//namespace ntl


#endif//#ifndef NTL__KM_THREAD
