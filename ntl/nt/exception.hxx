/**\file*********************************************************************
 *                                                                     \brief
 *  NT Exceptions
 *
 ****************************************************************************
 */

#ifndef NTL__NT_EXCEPTION
#define NTL__NT_EXCEPTION

#include "basedef.hxx"
#include "teb.hxx"
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
      /* 0x00 */  registration *  next;
      /* 0x04 */  handler_t *     handler;

      static registration * end_of_chain()
      { 
        return reinterpret_cast<registration*>(-1);
      }

      inline void unwind(const record * = 0) const;

      //_UnwindNestedFrames
      #pragma warning(push)
      #pragma warning(disable:4733)//Inline asm assigning to 'FS:0' : handler not registered as safe handler
      void unwindnestedframes(const record * ehrec) const
      {
        // same cast
        registration * const top = reinterpret_cast<registration *>(teb::get(&teb::ExceptionList));
        unwind(ehrec);
        top->next = reinterpret_cast<registration *>(teb::get(&teb::ExceptionList));
        teb::set(&teb::ExceptionList, top);
      }
      #pragma warning(push)

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

/// the functopn is exported by kernel32.dll ntdll.dll & nt.exe
NTL__EXTERNAPI
void __stdcall
RtlUnwind(
    const exception::registration * TargetFrame,    __optional
    const void *                    TargetIp,       __optional ///< not used on x86 
    const exception::record *       ExceptionRecord,__optional
    const void *                    ReturnValue
    );


void exception::record::raise() const
{
  RtlRaiseException(this);
}

void exception::registration::unwind(const exception::record * er)
const
{
  RtlUnwind(this, std::nullptr, er,std::nullptr);
}


}//namespace nt

typedef nt::exception::filter       exception_filter;
typedef nt::exception::disposition  exception_disposition;
typedef nt::exception::record       exception_record;
typedef nt::exception::pointers     exception_pointers;

//typedef nt::exception::registration exception_registration; see nt::tib::ExceptionList
struct exception_registration : public nt::exception::registration {};

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
  type_info *       typeinfo;
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
  struct eobject {/**/};
  /* 0x00 */  uint32_t  econst    : 1;
  /* 0x00 */  uint32_t  evolatile : 1;
  /* 0x04 */  void (__thiscall eobject::* exception_dtor)(); // avoid ugly __asm for thiscall
  /* 0x08 */  exception_disposition (__cdecl * forwardcompathandler)(...);///\todo safe proto
  /* 0x0C */  ntl::catchabletypearray *  catchabletypearray;
};
STATIC_ASSERT(sizeof(throwinfo) == 0x10); // STATIC_ASSERT(sizeof(exception_dtor) == sizeof(void*));

struct ehandler
{
//  union { uint32_t  adjectives; void * ptr; };
  uint32_t isconst      : 1;
  uint32_t isvolatile   : 1;
  uint32_t isunaligned  : 1;// guess it is not used on x86
  uint32_t isreference  : 1;

  const type_info * typeinfo;
  ptrdiff_t eobject_bpoffset; // 0 = no object (catch by type)
  void *  handler;

  bool type_match(const catchabletype * ct, const throwinfo * ti) const
  {
    // catch(...) ?
    if ( !ct->typeinfo || !*ct->typeinfo->name() )
      return true;
    // different TI record with different name?
    if ( this->typeinfo != ct->typeinfo
      && std::strcmp(this->typeinfo->name(), ct->typeinfo->name()) )
      return false;
    // reference?
    if ( ct->refonly && !isreference ) return false;
    // check CV cvalifiers
    if ( ti->econst && !isconst ) return false;
    if ( ti->evolatile && !isvolatile ) return false;
    return true;
  }
};

struct unwindtable
{
  int     state;
  void (* unwindfunclet)(); 
};

typedef int ehstate_t; ///< unwind map index

struct tryblock
{
  ehstate_t   trylow;
  ehstate_t   tryhigh;
  ehstate_t   catchhigh;
  int         ncatches;
  ehandler *  catchsym;

  typedef std::pair<const tryblock*, const tryblock*> ranges;
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
  /* 0x10 */  tryblock *      tryblocktable;
  /* 0x14 */  size_t          _size;
  /* 0x18 */  void *          _;

  tryblock::ranges
    get_try_ranges(int depth, ehstate_t state) const
  {
    const tryblock * end = &tryblocktable[tryblocktable_size];
    tryblock::ranges r(end, end);
    while ( depth >= 0 )
    {
      if ( --r.first < &tryblocktable[0]
        || r.first->tryhigh < state && state <= r.first->catchhigh )
      {
        --depth;
        r.second = end;
        end = r.first;
      }
    }
    ++r.first;
    return r;
  }
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
  /* 0x08 */  ehstate_t state;
  /* 0x0C */  uint32_t  stackbaseptr;

  // __CallSettingFrame 
  #pragma warning(push)
  #pragma warning(disable:4731)//frame pointer register 'ebp' modified by inline assembly code
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

  ehstate_t current_state(const ehfuncinfo * const  ehfi) const
  {
    const ehstate_t cs = ehfi->unwindtable_size > 80 ?
                            this->state : static_cast<int8_t>(this->state);
    _Assert( cs > -1 );
    _Assert( cs < ehfi->unwindtable_size );
    return cs;
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
    for ( ehstate_t cs = current_state(ehfi); cs != to_state; cs = ehfi->unwindtable[cs].state )
    {
      __try
      {
        if ( ehfi->unwindtable[cs].unwindfunclet )
        {
          this->state = ehfi->unwindtable[cs].state;
          callsettingframe(ehfi->unwindtable[cs].unwindfunclet);
        }
      }
      __except(unwindfilter(_exception_status()/*_exception_info()*/))
      { 
      }
    }
  }

};//struct cxxregistration


struct cxxrecord : public nt::exception::record
{

  uint32_t get_ehmagic() const
  { 
    /* 0x14 */  return ExceptionInformation[0];
  }

  void * get_object() const
  { 
    /* 0x18 */  return reinterpret_cast<void*>(ExceptionInformation[1]);
  }

  const throwinfo * get_throwinfo() const
  { 
    /* 0x1C */  return reinterpret_cast<const throwinfo*>(ExceptionInformation[2]);
  }

  void destruct_eobject(bool cannotthrow = true) const
  {
    _Assert(ExceptionCode == cxxmagic);
    _Assert(get_throwinfo());
    void (throwinfo::eobject::* const exception_dtor)() = get_throwinfo()->exception_dtor;
    if ( exception_dtor )
    {
      __try
      {
        (reinterpret_cast<throwinfo::eobject*>(get_object())->*exception_dtor)();
      }
      __except(cannotthrow ? exception_execute_handler : exception_continue_search)
      {
        std::terminate();
      }
    }
  }

  void * call_catchblock(
  //     EHExceptionRecord  *pExcept, 
    cxxregistration * const cxxreg,
    const nt::context * const ctx,
    const ehfuncinfo *  const funcinfo,
        void               *handler,
    int               const catchdepth,
        unsigned       nlg_code = 0x100
        ) const ;

  #pragma warning(push)
  #pragma warning(disable:4731)//frame pointer register 'ebp' modified by inline assembly code
  #pragma warning(disable:4733)//Inline asm assigning to 'FS:0' : handler not registered as safe handler
  __declspec(noreturn)
  static void jumptocontinuation(void * funclet, cxxregistration *cxxreg)
  {
    using namespace nt;
    // unlink SE handler
    exception_registration * const top = teb::get(&teb::ExceptionList);
    teb::set(&teb::ExceptionList, top->next);

    const uint32_t _ebp = cxxreg->stackbaseptr;
  	__asm
  	{
  		mov		ebp, cxxreg
  		mov		esp, [ebp-4]
  		mov		ebp, _ebp
  		// work arround C4740: flow in or out of inline asm code suppresses global optimization
  		//jmp		funclet
  		push  funclet
  		ret
		}
	}
  #pragma warning(pop)

  void catchit(
    cxxregistration *     const cxxreg,
    const nt::context *   const ctx,
    ehdispatchercontext * const dispatch,
    const ehfuncinfo *    const funcinfo,
    ehandler *            const catchblock,
    catchabletype *       const convertable,
    const tryblock *      const tb,
    int                   const catchdepth,
    const cxxregistration * const nested) const
  {
    if ( convertable )
    {
      // ___BuildCatchObject(this, , , convertable);
    }
    if ( nested ) nested->unwindnestedframes(this);
    else          cxxreg->unwindnestedframes(this);
    cxxreg->unwind(dispatch, funcinfo, tb->trylow);
    cxxreg->state = tb->tryhigh + 1;
    void * const continuation =
        call_catchblock(cxxreg, ctx, funcinfo, catchblock->handler, catchdepth);
    if ( continuation )
     jumptocontinuation(continuation, cxxreg);
  }

  // does not return on handled exceptions
  void find_handler(cxxregistration * const ereg,
                    const nt::context *  const ctx,
                    ehdispatchercontext * const dispatch,
                    const ehfuncinfo *      const ehfi,
                    const bool                    destruct,
                    const ehstate_t               trylevel,
                    const cxxregistration * const nested_eframe)
  const
  {
    ehstate_t cs = ereg->current_state(ehfi);
    if ( ExceptionCode == cxxmagic )
    {
      const uint32_t   ehmagic = ExceptionInformation[0];
      if ( NumberParameters == 3
#ifdef NTL__OTHEREHMAGICS
        && ehmagic >= ehmagic1200 && ehmagic <= ehmagic1400
#else
        && ehmagic == _EH_MAGIC
#endif
        )
      {
        const throwinfo * const ti = get_throwinfo();
        if ( !ti ) // rethrow?
        {
          // MSVC keeps previous throw object in the per-thread data...
          return;
        }
        for ( tryblock::ranges tr = ehfi->get_try_ranges(trylevel, cs);
              tr.first < tr.second;
              ++tr.first )
        {
          const tryblock* const tb = tr.first;
          if ( tb->trylow <= cs && cs <= tb->tryhigh )
            for ( int c = 0; c < tb->catchhigh; ++c )
              for ( unsigned i = 0; i < ti->catchabletypearray->size; ++i )
                if ( tb->catchsym[c].type_match(ti->catchabletypearray->type[i], ti) )
                {
                  catchit(ereg, ctx, dispatch, ehfi, &tb->catchsym[c],
                          ti->catchabletypearray->type[i], tb, trylevel, nested_eframe);
                  // new or rethrown excetion
                  goto next_try;
                }
          next_try: ;
        }
        if ( destruct )
          destruct_eobject();
      }
      //return;
    }
    else
    if ( ehfi->tryblocktable_size > 0 && !destruct )
    {
      //FindHandlerForForeignException(EHExceptionRecord *,EHRegistrationNode *,_CONTEXT *,void *,_s_FuncInfo const *,int,int,EHRegistrationNode *)
    }
    else std::terminate();
  }
};


exception_disposition
static __forceinline
  cxxframehandler (const exception_record * const er,//
                         cxxregistration *  const eframe, //
                   const nt::context *      const ectx, //
                   ehdispatchercontext *    const dispatch, //
                   const ehfuncinfo *       const ehfi,
                   int                      const trylevel = 0,
                   const cxxregistration *  const nested_eframe = 0, //
                   bool                     const destruct = false)
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
        const cxxrecord * const cxxer = static_cast<const cxxrecord*>(er);
#ifdef NTL__OTHEREHMAGICS
        if ( er->ExceptionCode == exception_record::cxxmagic
          && er->NumberParameters >= 3 /**\see _CxxThrowException args*/
          && er->ExceptionInformation[0] > ehmagic1400
          && cxxer->get_throwinfo()->forwardcompathandler )
        {
          return cxxer->get_throwinfo()->forwardcompathandler
            (er, eframe, ectx, dispatch, ehfi, trylevel, nested_eframe, destruct);
        }
        else
#endif
        {
          cxxer->find_handler(eframe, ectx, dispatch, ehfi, destruct, trylevel, nested_eframe);
        }
      }
    }
  }
  return ExceptionContinueSearch;
}

#endif//#ifdef _MSC_VER

}//namespace ntl

#endif//#ifndef NTL__NT_EXCEPTION
