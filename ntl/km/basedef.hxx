/**\file*********************************************************************
 *                                                                     \brief
 *  NT Kernel Types support library
 *
 ****************************************************************************
 */

#ifndef NTL__KM_BASEDEF
#define NTL__KM_BASEDEF

#include "../nt/basedef.hxx"
#include "../stdlib.hxx"

namespace ntl {
namespace km {

#ifndef SYSTEM_CACHE_ALIGNMENT_SIZE
#define SYSTEM_CACHE_ALIGNMENT_SIZE 64
#endif

using nt::slist_header;
using nt::single_list_entry;
using nt::slist_entry;
using nt::list_entry;
using nt::list_head;
using nt::status;

using nt::access_mask;
using nt::synchronize;
using nt::generic_read;

using nt::io_status_block;
using nt::io_apc_routine;

using nt::device_power_state;
using nt::system_power_state;

struct kthread;

//enum mode { KernelMode, UserMode };
struct kprocessor_mode { uint8_t mode; };

static const kprocessor_mode KernelMode = { 0 };
static const kprocessor_mode UserMode = { 1 };

typedef long kpriority;

#if 0
struct kirql{ uint8_t _; };
#else
typedef uint8_t kirql;
#endif

struct kspin_lock
{ 
    kspin_lock() : _() {} // no need for KeInitializeSpinLock call

    __forceinline kirql acquire();
    __forceinline void release(const kirql);

  private:
    /*volatile*/ uintptr_t _;
};


NTL__EXTERNAPI
bool __stdcall
  PsGetVersion(
    uint32_t *        MajorVersion,
    uint32_t *        MinorVersion,
    uint32_t *        BuildNumber,
    void/*unicode_string*/ *  CSDVersion
    );

static inline
bool get_version(uint32_t & major_version, uint32_t & minor_version)
{
  return PsGetVersion(&major_version, &minor_version, 0, 0);
}


NTL__EXTERNAPI
ntstatus __stdcall
  KeDelayExecutionThread(
    kprocessor_mode WaitMode,
    bool            Alertable,
    const int64_t * Interval
    );


template<times TimeResolution>
static inline
ntstatus sleep(
  uint32_t        time_resolution,
  bool            alertable = false,
  kprocessor_mode wait_mode = KernelMode)
{
  const int64_t interval = int64_t(-1) * TimeResolution * time_resolution;
  return KeDelayExecutionThread(wait_mode, alertable, &interval);
}

/// default milliseconds
static inline
ntstatus sleep(
  uint32_t        ms,
  bool            alertable = false,
  kprocessor_mode wait_mode = KernelMode)
{
  const int64_t interval = int64_t(-1) * milliseconds * ms;
  return KeDelayExecutionThread(wait_mode, alertable, &interval);
}


NTL__EXTERNAPI
void __fastcall
KfLowerIrql(
    const kirql NewIrql
    );

NTL__EXTERNAPI
kirql __fastcall
KfRaiseIrql (
    const kirql NewIrql
    );

NTL__EXTERNAPI
kirql __stdcall
KeRaiseIrqlToDpcLevel();


NTL__EXTERNAPI
kirql __fastcall
  KfAcquireSpinLock(kspin_lock * SpinLock);

NTL__EXTERNAPI
void __fastcall
  KfReleaseSpinLock(
    kspin_lock *  SpinLock,
    const kirql   NewIrql
    );


kirql kspin_lock::acquire()
{
  return KfAcquireSpinLock(this);
}

void kspin_lock::release(const kirql new_irql)
{
  KfReleaseSpinLock(this, new_irql);
}


struct kspin_lock_at_dpc : public kspin_lock
{
    __forceinline void acquire();
    __forceinline void release();
};

NTL__EXTERNAPI
void __fastcall
  KefAcquireSpinLockAtDpcLevel(kspin_lock * SpinLock);

NTL__EXTERNAPI
void __fastcall
  KefReleaseSpinLockFromDpcLevel(kspin_lock * SpinLock);


void kspin_lock_at_dpc::acquire()
{
  KefAcquireSpinLockAtDpcLevel(this);
}

void kspin_lock_at_dpc::release()
{
  KefReleaseSpinLockFromDpcLevel(this);
}


typedef ntl::atomic_exec<kspin_lock, kirql> synchronize_access;
typedef ntl::atomic_exec<kspin_lock_at_dpc, void> synchronize_access_at_dpc_level;


struct dispatcher_header
{
  uint8_t     Type;
  uint8_t     Absolute;
  uint8_t     Size;
  uint8_t     Inserted;
  long        SignalState;
  list_entry  WaitListHead;
};

struct kgate  { dispatcher_header Header; };

struct kevent { dispatcher_header Header; };

enum event_type { NotificationEvent, SynchronizationEvent };

NTL__EXTERNAPI
void __stdcall
  KeInitializeEvent(
    kevent *    Event,
    event_type  Type,
    bool        State
    );

NTL__EXTERNAPI
long __stdcall
  KeSetEvent(
    kevent *  Event,
    kpriority Increment,
    bool      Wait
    );

template <event_type Type>
struct event : kevent
{
  event(bool Signaled = false)
  {
    KeInitializeEvent(this, Type, Signaled);
  }

  long set(
    kpriority Increment = 0,
    bool      Wait      = false)
  {
    return KeSetEvent(this, Increment, Wait);
  }
};

typedef event<NotificationEvent> notification_event;
typedef event<SynchronizationEvent> synchronization_event;


enum kwait_reason
{
  Executive,
  FreePage,
  PageIn,
  PoolAllocation,
  DelayExecution,
  Suspended,
  UserRequest,
  WrExecutive,
  WrFreePage,
  WrPageIn,
  WrPoolAllocation,
  WrDelayExecution,
  WrSuspended,
  WrUserRequest,
  WrEventPair,
  WrQueue,
  WrLpcReceive,
  WrLpcReply,
  WrVirtualMemory,
  WrPageOut,
  WrRendezvous,
  Spare2,
  Spare3,
  Spare4,
  Spare5,
  Spare6,
  WrKernel,
  WrResource,
  WrPushLock,
  WrMutex,
  WrQuantumEnd,
  WrDispatchInt,
  WrPreempted,
  WrYieldExecution,
  WrFastMutex,
  WrGuardedMutex,
  WrRundown,
  MaximumWaitReason
}; // enum kwait_reason

NTL__EXTERNAPI
ntstatus __stdcall
  KeWaitForSingleObject(
    void *          Object,
    kwait_reason    WaitReason,
    kprocessor_mode WaitMode,
    bool            Alertable,
    int64_t *       Timeout  __optional
    );

static inline
ntstatus
  wait_for_single_object(
    void *          Object,
    kwait_reason    WaitReason  = Executive,
    kprocessor_mode WaitMode    = KernelMode,
    bool            Alertable   = false,
    int64_t *       Timeout     = 0
    )
{
  return KeWaitForSingleObject(Object, WaitReason, WaitMode, Alertable, Timeout);
}


struct kdpc;

typedef
void __stdcall
  kdeferred_routine_t(
    const kdpc *  Dpc,
    const void *  DeferredContext,
    const void *  SystemArgument1,
    const void *  SystemArgument2
    );

struct kdpc
{
  uint8_t               Type;
  uint8_t               Importance;
  uint8_t               Number;
  uint8_t               Expedite;
  list_entry            DpcListEntry;
  kdeferred_routine_t * DeferredRoutine;
  void *                DeferredContext;
  void *                SystemArgument1;
  void *                SystemArgument2;
  void *                DpcData;
};


struct ksemaphore
{
  dispatcher_header Header;
  long  Limit;
};


struct ktimer
{
  dispatcher_header Header;
  uint64_t          DueTime;
  list_entry        TimerListEntry;
  kdpc *            Dpc;
  long              Period;
};


struct work_queue_item 
{
  list_entry  List;
  void     (* WorkerRoutine)(void*);
  void      * Parameter;
};


struct fast_mutex
{
  static const int32_t  lock_bit_v        = 0;
  static const int32_t  lock_bit          = 1;
  static const int32_t  lock_waiter_woken = 2;
  static const int32_t  lock_waiter_inc   = 0;

  /*volatile*/ int32_t      Count;
  kthread *             Owner;
  uint32_t              Contention;
//  synchronization_event Gate;
  kevent                Gate;
  uint32_t              OldIrql;
};

struct kmutant 
{
  dispatcher_header   Header;
  list_entry          MutantListEntry;
  struct _KTHREAD   * OwnerThread;
  unsigned char       Abandoned;
  unsigned char       ApcDisable;
};


struct kguarded_mutex 
{
  static const int32_t  lock_bit_v        = 0;
  static const int32_t  lock_bit          = 1;
  static const int32_t  lock_waiter_woken = 2;
  static const int32_t  lock_waiter_inc   = 0;

  int32_t   Count;
  kthread*  Owner;
  uint32_t  Contention;
  kgate     Gate;
  int16_t   KernelApcDisable;
  int16_t   SpecialApcDisable;
};

typedef uintptr_t eresource_thread_t;

struct owner_entry
{
  eresource_thread_t OwnerThread;
  union { int32_t OwnerCount; uint32_t TableSize; };
};


typedef uint16_t eresource_flag_t;
static const eresource_flag_t
  ResourceNeverExclusive        = 0x10,
  ResourceReleaseByOtherThread  = 0x20,
  ResourceOwnedExclusive        = 0x80;

struct eresource
{
  list_entry        SystemResourcesList;
  owner_entry *     OwnerTable;
  int16_t           ActiveCount;
  eresource_flag_t  Flag;
  volatile ksemaphore * SharedWaiters;
  volatile kevent *     ExclusiveWaiters;
  owner_entry       OwnerEntry[2];
  uint32_t          ContentionCount;
  int16_t           NumberOfSharedWaiters;
  int16_t           NumberOfExclusiveWaiters;
  union
  {
    void *    Address;
    uintptr_t CreatorBackTraceIndex;
  };
  kspin_lock SpinLock;
};
STATIC_ASSERT(sizeof(eresource)==0x38);


union ex_rundown_ref
{
  uint32_t  Count;
  void *    Ptr;
};

union ex_fast_ref
{
  void *    Object;
  uint32_t  RefCnt:3;
  uint32_t  Value;
};

struct ex_push_lock
{
  union
  {
    struct flags
    {
      /*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Waiting    :1;
      /*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Exclusive  :1;
      /*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Shared     :0x1E;
    };
    /*<thisrel this+0x0>*/ /*|0x4|*/ uint32_t Value;
    /*<thisrel this+0x0>*/ /*|0x4|*/ void* Ptr;
  };
};// <size 0x4>



}//namespace ntl
}//namespace nt

#endif//#ifndef NTL__KM_BASEDEF
