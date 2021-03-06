/**\file*********************************************************************
 *                                                                     \brief
 *  18.10 Other runtime support [lib.support.runtime]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_CSTARG
#define NTL__STLX_CSTARG
#pragma once

//namespace std {

/**\addtogroup  lib_language_support *** 18 Language support library [language.support]
 *@{*/
/**\addtogroup  lib_support_runtime **** 18.10 Other runtime support [support.runtime]
 *@{*/

#ifndef va_start

// MSVC stdlib compatibility
#ifndef _VA_LIST_DEFINED
typedef struct { } * va_list;
#define _VA_LIST_DEFINED
#endif

#ifdef _MSC_VER

#if defined(_M_IX86)

//  stack parameters are aligned by 4 bytes

#define va_start(__argptr, __last_param)\
      (__argptr = (va_list)&__last_param + (sizeof(__last_param) + 3 & ~3))

#define va_arg(__argptr, __type)(*(__type*) __argptr += __type + 3 & ~3)

#ifdef NTL__DEBUG
#   define va_end(__argptr)(__argptr = (va_list)0)
#else
#   define va_end(__argptr)
#endif

#elif defined(_M_X64)

//  stack parameters are aligned by 8 bytes

#define va_start(__argptr, __last_param)\
  (__argptr = (va_list)&__last_param + (sizeof(__last_param) + 7 & ~7))

#define va_arg(__argptr, __type)(*(__type*) __argptr += __type + 7 & ~7)

#ifdef NTL__DEBUG
#   define va_end(__argptr)(__argptr = (va_list)0)
#else
#   define va_end(__argptr)()
#endif


#else//CPU type
#   error unsupported CPU type
#endif

#endif//!_MSC_VER

#endif//! va_start

#ifndef va_start
#   error define va_start & Co
#endif

/**@} lib_support_runtime */
/**@} lib_language_support */

//}//namespace std

//using std::va_list;

#endif//#ifndef NTL__STLX_CSTARG
