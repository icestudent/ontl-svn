#ifndef STLX_CONFIG_GCC
#define STLX_CONFIG_GCC

# define __forceinline inline
# define __assume(X)
# define __restrict
# define __thiscall
# define __noalias
# define __w64

#define _cdecl    __attribute__((cdecl))
#define _stdcall  __attribute__((stdcall))
#define _fastcall __attribute__((fastcall))
#define __cdecl    _cdecl
#define __stdcall  _stdcall
#define __fastcall _fastcall

#ifndef NTL__CRTCALL
# ifdef NTL__STLX_FORCE_CDECL
#define NTL__CRTCALL __cdecl
# else
#define NTL__CRTCALL
# endif
#endif

#define NTL__EXTERNAPI extern "C"
#define NTL__EXTERNVAR extern "C"

#define NTL__NORETURN __attribute__((noreturn))

#ifndef STATIC_ASSERT
# if (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 3)
  // !fr3@K!
  // GCC specific note: __COUNTER__ is not supported prior to 4.3
  #define STATIC_ASSERT(e) typedef char _Join(_STATIC_ASSERT_, __COUNTER__) [(e)?1:-1]
# else
  #define STATIC_ASSERT(e) typedef char _Join(_STATIC_ASSERT_, __LINE__) [(e)?1:-1]
# endif
#endif


#ifdef __GXX_EXPERIMENTAL_CXX0X__

/** GCC's partial C++0x support */

// keywords:
// align (N2798+)
//#define NTL__CXX_ALIGN
// alignas( <= N2723)
//#define NTL__CXX_ALIGNAS
// alignof
//#define NTL__CXX_ALIGNOF
// auto
//#define NTL__CXX_AUTO
// char16_t, char32_t
#define NTL__CXX_CHARS
// concepts, concept_map, requires
//#define NTL__CXX_CONCEPT
// constexpr
//#define NTL__CXX_CONSTEXPR
// decltype (typeof)
#define NTL__CXX_TYPEOF
// nullptr
//#define NTL__CXX_NULLPTR
// static assert
#define NTL__CXX_ASSERT
// thread_local
//#define NTL__CXX_THREADL

// syntax:
// explicit conversion operators
//#define NTL__CXX_EXPLICITOP
// extern templates
#define NTL__CXX_EXTPL
// lambda
//#define NTL__CXX_LAMBDA
// rvalues
#define NTL__CXX_RV
// template typedef
//#define NTL__CXX_TT
// variadic templates (implies rvalue references support)
//#define NTL__CXX_VT

#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 3))

// initializer lists
#define NTL__CXX_IL
// class enum
#define NTL__CXX_ENUM
// explicit delete/default function definition
#define NTL__CXX_EF

#endif // gcc > 4.3
#endif // C++0x

#define STLX_CSTD_INT_EXISTS

#endif // STLX_CONFIG_GCC
