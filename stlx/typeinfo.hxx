#ifndef STLX_CSTDDEF
#include "cstddef.hxx"
#endif
#ifdef STLX_EXTERNAL_RUNTIME
#include <typeinfo>
#else
#include "std/typeinfo.hxx"
namespace stlx { using std::typeinfo; using std::bad_cast; using std::bad_typeid; }
#endif
