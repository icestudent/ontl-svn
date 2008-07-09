/**\file*********************************************************************
 *                                                                     \brief
 *  Assertions [lib.assertions 19.2]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CASSERT
#define NTL__STLX_CASSERT

//namespace std {

#ifdef _DEBUG
#define NTL__DEBUG
#endif


#if defined NTL__DEBUG || !defined NDEBUG
#define __ntl_assert(__msg, __line)\
  { const char * volatile __assertion_failure; __assertion_failure = (__msg);\
    unsigned volatile __assertion_failure_at_line; __assertion_failure_at_line = (__line);\
    __debugbreak(); }
#endif


#ifdef NTL__DEBUG
  #define _Assert(expr) \
    if ( !!(expr) ); else \
    __ntl_assert("NTL Assertion ("#expr") failed in "__FUNCSIG__" //"__FILE__,__LINE__);\
    ((void)0)
#else
  #define _Assert(expr)
#endif

//#undef assert
#ifndef assert
  #ifdef NDEBUG
    #define assert(expr) ((void)0)
  #else
    #define assert(expr) \
      if ( !!(expr) ); else \
      __ntl_assert("Assertion ("#expr") failed in "__FUNCSIG__" //"__FILE__,__LINE__);\
      ((void)0)
  #endif
#endif

//}//namespace std

#endif//#ifndef NTL__STLX_CASSERT
