
///\file C++ exception support runtime
/// This must be a separate compilation unit to support /GL for other ones.

#define NTL__OTHEREHMAGICS

#include <exception>
#include <nt/exception.hxx>

/// RTL poiner to the current terminate_handler
///\todo shall be an atomic type for safe multithreading
std::terminate_handler __ntl_std_terminate_handler;

/// Sets the current handler function for terminating exception processing.
///\warning f shall not be a null pointer.
///\return  The previous terminate_handler.
std::terminate_handler
  std::set_terminate(std::terminate_handler f ///< new handler
                    ) __ntl_nothrow
{
  const terminate_handler old = __ntl_std_terminate_handler;
  __ntl_std_terminate_handler = f;
  return old;
}

void std::terminate()
{
  __ntl_std_terminate_handler();
}


using namespace ntl;

static __forceinline
uint32_t get_eax()
{
  __asm xchg eax, eax
}

/// throw T; implementation
///\ note both pointers are null if re-throw (throw;)
extern "C"
void __stdcall _CxxThrowException(void * object, _s__ThrowInfo const * info)
{
  uintptr_t args[] = { _EH_MAGIC, (uintptr_t)object, (uintptr_t)info };
  RaiseException(exception_record::cxxmagic, nt::exception::noncontinuable, sizeof(args)/sizeof(*args), args);
}

/// indirectly called by OS' dispatcher
extern "C"
exception_disposition __cdecl /*actually not, the first ehfuncinfo* arg is passed in EAX*/
__CxxFrameHandler3 (const exception_record *  er,
                          cxxregistration *   frame,
                    const nt::context *       ectx,
                    ehdispatchercontext *     dispatch)
{
  const ehfuncinfo* const ehfi = reinterpret_cast<const ehfuncinfo*>(get_eax());
  return cxxframehandler(er, frame, ectx, dispatch, ehfi);
}

