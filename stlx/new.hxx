#ifndef STLX_CSTDDEF
#include "cstddef.hxx"
#endif
#ifdef STLX_EXTERNAL_RUNTIME
#include <new>
namespace stlx { using std::bad_alloc; using std::nothrow_t; using std::nothrow; }
#else
#include "std/new.hxx"
#endif
