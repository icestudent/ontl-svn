/**\file*********************************************************************
 *                                                                     \brief
 *  C Library files utilities [c.files]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CSTDIO
#define NTL__STLX_CSTDIO

#include "cstddef.hxx"
#include "cstdarg.hxx"

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

  typedef long long fpos_t;

#ifndef _INC_STDLIB// MSVC compatibility

  using ::_snprintf; using ::_vsnprintf;

#else

using ::va_list;

#endif

/**@} lib_general_utilities
 */
/**@} lib_language_support */

}//namespace std

#endif //#ifndef NTL__STLX_CSTDIO
