/**\file*********************************************************************
 *                                                                     \brief
 *  Kernel Threads
 *
 ****************************************************************************
 */

#ifndef NTL__KM_PROCESS
#define NTL__KM_PROCESS

#include "basedef.hxx"
#include "apc.hxx"
#include "handle.hxx"
#include "object.hxx"
#include "thread.hxx"
#include "system_information.hxx"
#include "../pe/image.hxx"


namespace ntl {
namespace km {


struct kprocess
{

  typedef kprocess * get_current_t();

  static 
  get_current_t * get_current_fn(get_current_t * ptr = 0)
  {
    static get_current_t * get_current_fn_;
    if ( ptr ) get_current_fn_ = ptr;
    return get_current_fn_ ;
  }

  static 
  kprocess * get_current()
  {
#ifndef NTL_SUPPRESS_IMPORT
    if ( get_current_fn() ) return (*get_current_fn())();
#endif
    return kthread::get_current()->ApcState.Process;    
  }

};
//struct eprocess;


NTL__EXTERNAPI
kprocess * __stdcall
  PsGetCurrentProcess();


NTL__EXTERNAPI
char * __stdcall
  PsGetProcessImageFileName(kprocess *);


NTL__EXTERNAPI
ntstatus __stdcall
  PsLookupProcessByProcessId(
    legacy_handle ProcessId,
    kprocess **   Process
    );


static inline
kprocess * lookup_process(legacy_handle process_id)
{
  kprocess * process;
  return nt::success(PsLookupProcessByProcessId(process_id, &process))
    ? process : 0;
}


static __forceinline
void dereference_object(kprocess * process)
{
  ObfDereferenceObject(process);
}



NTL__EXTERNAPI
void __stdcall
  KeStackAttachProcess(
    kprocess *    Process,
    kapc_state *  ApcState
    );

NTL__EXTERNAPI
void __stdcall
  KeUnstackDetachProcess(
    kapc_state *  ApcState
    );


struct nt::peb;

typedef
nt::peb * __stdcall
  get_process_peb_t(kprocess * Process);

// XP+ only
NTL__EXTERNAPI
get_process_peb_t PsGetProcessPeb;


}//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_PROCESS
