/**\file*********************************************************************
 *                                                                     \brief
 *  STLx global configuration
 *
 ****************************************************************************
 */

#ifndef STLX__STLXDEF
#define STLX__STLXDEF

// supported compilers
#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
// MS VC++
  #if _MSC_VER < 1400
  # error upgrade your compiler at least to VC8 (14.00)
  #endif

  #include "msvc.hxx"

#elif defined(__BCPLUSPLUS__)
// Borland C++
  #if __BCPLUSPLUS__ < 0x600
  # error upgrade your compiler at least to BCB 2009 (12.00)
  #endif

  #include "bcb.hxx"

#elif defined(__INTEL_COMPILER)
  #ifndef __ICL
  # define __ICL __INTEL_COMPILER
  #endif

  #include "icc.hxx"

#elif defined(__GNUC__)
// GCC
  #if !(__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 2)
  # error upgrade GCC at least to the GCC 4.2
  #endif

  #include "gcc.hxx"

#else
# pragma message("Unknown compiler, it is unsupported probably. Sorry, mate")
#endif

#define __optional
#define _PasteToken(x,y) x##y
#define _Join(x,y) _PasteToken(x,y)


#if __cplusplus > 199711L
  /// new C++ Standard
  #define NTL__CXX
#endif

#if __cplusplus <= 199711L

  // C++0x support for legacy compilers

  #ifndef NTL__CXX_ASSERT
    #define static_assert(e, Msg) STATIC_ASSERT(e)
  #else
    #undef STATIC_ASSERT
    #define STATIC_ASSERT(e) static_assert(e, #e)
  #endif

  #ifndef NTL__CXX_ALIGNAS
    #ifdef _MSC_VER
      #define alignas(X) __declspec(align(X))
    #else
      #define alignas(X)
    #endif
  #endif

  #ifndef NTL__CXX_ALIGNOF
    #if defined(__ICL)
      #define alignof(X) __ALIGNOF__(X)
    #elif _MSC_VER <= 1600
      #define alignof(X) __alignof(X)
    #endif
  #endif
  static_assert(alignof(int)==alignof(unsigned int), "wierd platform");

  #ifndef __func__
    #ifdef _MSC_VER
      #define __func__ __FUNCSIG__
    #else
      #define __func__ __FUNC__
    #endif
  #endif

  #ifndef __align
    #ifdef _MSC_VER
    # define __align(X) __declspec(align(X))
    #else
    # define __align(X)
    #endif
  #endif

  #ifndef NTL_CXX_CONSTEXPR
    #define constexpr
  #endif

#endif//__cplusplus <= 199711L


// explicit function definition
#ifdef NTL__CXX_EF
  #define __deleted = delete
  #define __default = default
#else
  #define __deleted
  #define __default
#endif


// language support headers
#if !defined(STLX_EXTERNAL_RUNTIME) && !defined(STLX_INTERNAL_RUNTIME)
# define STLX_EXTERNAL_RUNTIME
#elif defined(STLX_INTERNAL_RUNTIME)
# undef  STLX_EXTERNAL_RUNTIME

  // stlx forward declaration
  namespace stlx {}
  // declare std namespace
  namespace std = stlx;
#endif

#endif // STLX__STLXDEF
