/**\file*********************************************************************
 *                                                                     \brief
 *  C Library files utilities [c.files]
 *
 *\note There should be no sprintf and other buffer-overflowing stuff.
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CSTDIO
#define NTL__STLX_CSTDIO

#include "array.hxx"
#include "cstddef.hxx"
#include "cstdarg.hxx"
#include "memory.hxx"
#include "string.hxx"
#include "nt/peb.hxx"

#ifndef _INC_STDLIB// MSVC compatibility

using std::va_list;

NTL__EXTERNAPI size_t __cdecl
  _snprintf(char *buffer, size_t count, const char *format, ...);

NTL__EXTERNAPI size_t __cdecl
  _vsnprintf(char *buffer, size_t count, const char *format, va_list argptr);

#endif


namespace std {

/**\addtogroup  lib_language_support *** Language support library [18] ******
 *@{*/

/**\addtogroup  lib_general_utilities ** C Library filees [c.files  ] *******
 *@{*/

#ifndef _INC_STDLIB// MSVC compatibility

  using ::_snprintf; using ::_vsnprintf;

#else

  using ::va_list;

#endif

#ifndef MAX_PATH
  #define MAX_PATH 260
#endif

#ifndef L_tmpnam
  #define L_tmpnam (MAX_PATH-14)
#endif

typedef long long fpos_t;

//#define EOF in string.hxx

#define SEEK_CUR  1
#define SEEK_END  2
#define SEEK_SET  0

#define stderr reinterpret_cast<FILE*>(&ntl::nt::peb::instance().ProcessParameters->StandardError)
#define stdin  reinterpret_cast<FILE*>(&ntl::nt::peb::instance().ProcessParameters->StandardInput)
#define stdout reinterpret_cast<FILE*>(&ntl::nt::peb::instance().ProcessParameters->StandardOutput)


/**@} lib_general_utilities
 */
/**@} lib_language_support */

}//namespace std

#endif //#ifndef NTL__STLX_CSTDIO
