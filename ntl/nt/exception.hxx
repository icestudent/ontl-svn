/**\file*********************************************************************
 *                                                                     \brief
 *  NT Exceptions
 *
 ****************************************************************************
 */

#ifndef NTL__NT_EXCEPTION
#define NTL__NT_EXCEPTION

#include "basedef.hxx"
#include <typeinfo>

namespace ntl {
namespace nt {

struct context;

class exception
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    /// Exception filter return values
    enum filter
    {
      continue_execution  = -1,
      continue_search     = 0,
      execute_handler     = 1
    };

    /// Exception disposition return values.
    enum disposition
    {
      ContinueExecution,
      ContinueSearch,
      NestedException,
      CollidedUnwind
    };

    enum flags
    {
      noncontinuable  = 0x01,
      unwinding       = 0x02,
      exit_unwind     = 0x04,
      stack_invalid   = 0x08,
      nested_call     = 0x10,
      target_unwind   = 0x20,
      collied_unwind  = 0x40,
      unwind          = 0x66
    };

    struct record 
    {
      static const int maximum_parameters = 15;
      static const ntstatus cxxmagic = status::cxx_exception;// 0xE0000000|'msc';
      static const ntstatus commagic = status::com_exception;//0xE0000000|'MOC';
      /* 0x00 */  ntstatus  ExceptionCode;
      /* 0x04 */  flags     ExceptionFlags;
      /* 0x08 */  record *  ExceptionRecord;
      /* 0x0C */  void *    ExceptionAddress;
      /* 0x10 */  uint32_t  NumberParameters;
      /* 0x14 */  uintptr_t ExceptionInformation[maximum_parameters];
      inline void raise() const;
    };
    
    /// Exception handler prototype
    typedef
      disposition __stdcall
      handler_t(
        const exception::record * exception_record,
        const void              * establisher_frame,
              context           * context_record,
              void              * dispatcher_context
        );

    /// SEH record
    struct registration
    {
      registration *  next;
      handler_t *     handler;

      static registration * end_of_chain()
      { 
        return reinterpret_cast<registration*>(-1);
      }
    };

    /// exception_info result
    struct pointers
    {
      record *  ExceptionRecord;
      context * ContextRecord;
    };

};//class exception


NTL__EXTERNAPI
__declspec(noreturn)
void __stdcall
RtlRaiseException(
    const exception::record * ExceptionRecord
    );

void exception::record::raise() const
{
  RtlRaiseException(this);
}

}//namespace nt

typedef nt::exception::filter       exception_filter;
typedef nt::exception::disposition  exception_disposition;
typedef nt::exception::record       exception_record;
typedef nt::exception::registration exception_registration;
typedef nt::exception::pointers     exception_pointers;

/// Exception filter return values
static const exception_filter
  exception_continue_execution  = nt::exception::continue_execution,
  exception_continue_search     = nt::exception::continue_search,
  exception_execute_handler     = nt::exception::execute_handler;

static const exception_disposition
      ExceptionContinueExecution  = nt::exception::ContinueExecution,
      ExceptionContinueSearch     = nt::exception::ContinueSearch,
      ExceptionNestedException    = nt::exception::NestedException,
      ExceptionCollidedUnwind     = nt::exception::CollidedUnwind;


extern "C" unsigned long __cdecl  _exception_code();
extern "C" ntl::nt::exception::pointers * __cdecl _exception_info();

#define _exception_status() static_cast<ntstatus>(_exception_code())

extern "C" void * _ReturnAddress();
#pragma intrinsic(_ReturnAddress)

extern "C"//NTL__EXTERNAPI is not an option because of __declspec(dllimport)
__declspec(noreturn)
void __stdcall
RaiseException(ntstatus, uint32_t, uint32_t, const uintptr_t *);

#ifndef NTL__WIN_NEW
__forceinline
void __stdcall
RaiseException(
    ntstatus              ExceptionCode,
    nt::exception::flags  ExceptionFlags,
    uint32_t              NumberOfArguments,
    const uintptr_t *     Arguments)
{
  exception_record er =
  { 
    ExceptionCode, ExceptionFlags, 0, _ReturnAddress(),
    !Arguments ? 0 :
      NumberOfArguments > exception_record::maximum_parameters ?
        exception_record::maximum_parameters : NumberOfArguments
  };
  for ( uint32_t i = 0; i != er.NumberParameters; ++i )
    er.ExceptionInformation[i] = Arguments[i];
  er.raise();
}
#endif

#if defined _MSC_VER && defined _M_IX86

// C++ exception compiler support
// see details at http://www.openrce.org/articles/full_view/21

// wierd names! do not abuse =)

static const uint32_t ehmagic1400 = 0x19930522;
static const uint32_t ehmagic1300 = 0x19930521;
static const uint32_t ehmagic1200 = 0x19930520;

#if _MSC_VER >= 1400
#define _EH_MAGIC ehmagic1400
#elif _MSC_VER>= 1300
#define _EH_MAGIC ehmagic1300
#else
#define _EH_MAGIC ehmagic1200
#endif

struct memptrdescriptor // _PMD
{
  int32_t member_offset;
  int32_t vbtable_offset; // -1 if not a virtual base
  int32_t vdisp_offset;   // offset to the displacement value inside the vbtable
};

struct catchabletype
{
  uint32_t  memmoveable : 1;
  uint32_t  refonly     : 1;
  uint32_t  hasvirtbase : 1;
  ::type_info *     type_info;
  memptrdescriptor  thiscast;
  uint32_t          object_size;
  void (*copyctor)();
};

struct catchabletypearray
{
  uint32_t        size; 
  catchabletype * type[1];
};


struct throwinfo // _s__ThrowInfo
{
  /* 0x00 */  uint32_t  econst    : 1;
  /* 0x00 */  uint32_t  evolatile : 1;
  /* 0x04 */  void (* exception_dtor)();
  /* 0x08 */  exception_disposition (__cdecl * forwardcompathandler)(...);///\todo safe proto
  /* 0x0C */  catchabletypearray *  catchabletypearray;
};

struct ehandler
{
  // 0x01: const, 0x02: volatile, 0x08: reference
  union { uint32_t  adjectives; void * ptr; };
  ::type_info * rtti;
  int32_t eobject_bpoffset; // 0 = no object (catch by type)
  void *  handler;
};

struct unwindtable
{
  int     state;
  void (* unwindfunclet)(); 
};

typedef int ehstate_t; ///< unwind map index

struct tryblocktable
{
  ehstate_t trylow;
  ehstate_t tryhigh;
  ehstate_t catchhigh;
  int       ncatches;
  ehandler *  catchsym;
};

// have you seen /d1ESrt in TFM? so do I...

struct estypeinfo
{
  /* 0x00 */  size_t      size;
  /* 0x04 */  ehandler *  ptr;
};


struct ehfuncinfo1200 //_s_ESTypeList
{
  /* 0x00 */  uint32_t        magic : 30;
  /* 0x04 */  ehstate_t       unwindtable_size;
  /* 0x08 */  unwindtable *   unwindtable;
  /* 0x0C */  size_t          tryblocktable_size;
  /* 0x10 */  tryblocktable * tryblocktable;
  /* 0x14 */  size_t          _size;
  /* 0x18 */  void *          _;
};

struct ehfuncinfo1300 : public ehfuncinfo1200
{
  /* 0x1C */  estypeinfo  *   estypeinfo;
};

struct ehfuncinfo1400 : public ehfuncinfo1300
{
  /* 0x20 */  uint32_t        synchronous : 1;
};

typedef ehfuncinfo1400 ehfuncinfo;

struct ehdispatchercontext {};

/* per-thread data returned by MSVC's _getpt()

    +0x78 - terminate handler
    +0x7C - unexpected handler
    +0x80 - SE translator
 */


struct cxxregistration : public nt::exception::registration
{
  ehstate_t state;
  uint32_t  stackbaseptr;

  // __CallSettingFrame 
  #pragma warning(push)
  #pragma warning(disable:4731)
  void callsettingframe(void (*unwindfunclet)(), int /*nlg_notify_param*/ = 0x103)
  {
    const uint32_t stackbaseptr = this->stackbaseptr;
    __asm mov ebp, stackbaseptr
    unwindfunclet();
  }
  #pragma warning(pop)

  static exception_filter unwindfilter(ntstatus code)
  {
    switch ( code )
    {
      case exception_record::cxxmagic:
        std::terminate();
      case exception_record::commagic:
        /**/
      default:
        return exception_continue_search;
    }
  }

  static exception_filter unwindfilter(exception_pointers * ptrs)
  {
    return unwindfilter(ptrs->ExceptionRecord->ExceptionCode);
  }

  // __FrameUnwindToState
  void// __forceinline 
    unwind(ehdispatchercontext *,
          const ehfuncinfo * const  ehfi,
          ehstate_t const           to_state = -1) ///< defaults to empty state
  {
    ehstate_t currstate = ehfi->unwindtable_size > 80 ?
                            this->state : static_cast<int8_t>(this->state);

    while ( currstate != to_state )
    {
      _Assert( currstate != -1 );
      _Assert( currstate < ehfi->unwindtable_size );
      __try
      {
        if ( ehfi->unwindtable[currstate].unwindfunclet )
        {
          this->state = ehfi->unwindtable[currstate].state;
          callsettingframe(ehfi->unwindtable[currstate].unwindfunclet);
        }
      }
      __except(unwindfilter(_exception_status()/*_exception_info()*/))
      { 
      }
    }
    currstate = this->state;
  }

};

exception_disposition
static __forceinline
  cxxframehandler (const exception_record *       const er,//
                         cxxregistration *        const eframe, //
                   const nt::context *            const ectx, //
                   struct ehdispatchercontext *   const dispatch, //
                   const ehfuncinfo *             const ehfi,
                   int                            const trylevel = 0,
                   const exception_registration * const nested_eframe = 0, //
                   bool                           const destruct = false)
{
  // handle C++ EH or longjump
  if ( er->ExceptionCode == exception_record::cxxmagic ||
#ifdef NTL__LONGJUMP
       er->ExceptionCode == nt::status::longjump ||
#endif
#if _MSC_VER >= 1400
  #ifdef NTL__OTHEREHMAGICS
       ehfi->magic < ehmagic1400 &&
  #endif
        //  skip ordinary SEH frames for functons compiled with /EHs (MSVC2005+)
       !ehfi->synchronous
#endif//MSVC 14
      )
  {
    if ( er->ExceptionFlags & nt::exception::unwind )
    {
      if ( ehfi->unwindtable_size && !trylevel )
      {
        eframe->unwind(dispatch, ehfi);
      }
    }
    else
    {
      if ( ehfi->tryblocktable_size ||
#ifdef NTL__OTHEREHMAGICS
       ehfi->magic >= ehmagic1300 &&
#endif
       ehfi->estypeinfo )
      {
#ifdef NTL__OTHEREHMAGICS
        if ( er->ExceptionCode == exception_record::cxxmagic
          && er->NumberParameters >= 3 /**\see _CxxThrowException args*/
          && er->ExceptionInformation[0] > ehmagic1400
          && reinterpret_cast<const throwinfo*>(er->ExceptionInformation[2])->forwardcompathandler )
        {
          return reinterpret_cast<const throwinfo*>(er->ExceptionInformation[2])->forwardcompathandler
                          (er, eframe, ectx, dispatch, ehfi, trylevel, nested_eframe, destruct);
        }
        else
#endif
        {
          destruct; nested_eframe; ectx;
          // FindHandler(er, eframe, ectx, void *, ehfi, destruct, trylevel, nested_eframe);
        }
      }
    }
  }
  return ExceptionContinueSearch;
}

#endif//#ifdef _MSC_VER

}//namespace ntl

#endif//#ifndef NTL__NT_EXCEPTION
