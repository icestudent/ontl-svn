
///\file C++ exception support runtime
/// This must be a separate compilation unit to support /GL for other ones.

#define NTL__OTHEREHMAGICS

#include <exception>
#include <nt/exception.hxx>

void std::terminate()
{
  ///\todo it
}


using namespace ntl;

static __forceinline
uint32_t get_eax()
{
  __asm xchg eax, eax
}

extern "C"
void __stdcall _CxxThrowException(void * object, _s__ThrowInfo const * info)
{
  uintptr_t args[] = { _EH_MAGIC, (uintptr_t)object, (uintptr_t)info };
  RaiseException(exception_record::cxxmagic, nt::exception::noncontinuable, sizeof(args)/sizeof(*args), args);
}

extern "C"
exception_disposition __cdecl /*actually not, the first ehfuncinfo* arg is passed in EAX*/
__CxxFrameHandler3 (const exception_record *            er,
                          cxxregistration *             frame,
                    const nt::context *                 ectx,
                    struct ehdispatchercontext *        dispatch)
{
  const ehfuncinfo* const ehfi = reinterpret_cast<const ehfuncinfo*>(get_eax());
  return cxxframehandler(er, frame, ectx, dispatch, ehfi);
}

