#ifndef STLX_CSTDDEF
#include "cstddef.hxx"
#endif
#if defined(STLX_EXTERNAL_RUNTIME) && defined(STLX_CSTD_UCHAR_EXISTS)
#include <uchar.h>
#else
#include "cstd/cuchar.hxx"
#endif
