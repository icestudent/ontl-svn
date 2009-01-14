#ifndef STLX_CONFIG_ICC
#define STLX_CONFIG_ICC
#pragma once

#ifdef _MSC_VER
  // MSVC compatible mode
# include "msvc.hxx"
#endif


#if __ICL > 1100

// TODO: detect -stlx=C++0x presence

/** ICL's (East Mill) partial C++0x support */

// keywords:
// align (N2798+)
//#define NTL__CXX_ALIGN
// alignas( <= N2723)
//#define NTL__CXX_ALIGNAS
// alignof
//#define NTL__CXX_ALIGNOF
// auto
#define NTL__CXX_AUTO
// char16_t, char32_t
//#define NTL__CXX_CHARS
// concepts, concept_map, requires
//#define NTL__CXX_CONCEPT
// constexpr
//#define NTL__CXX_CONSTEXPR
// decltype (typeof)
#define NTL__CXX_TYPEOF
// class enum
//#define NTL__CXX_ENUM
// nullptr
//#define NTL__CXX_NULLPTR
// static assert
#define NTL__CXX_ASSERT
// thread_local
//#define NTL__CXX_THREADL

// syntax:
// explicit delete/default function definition
//#define NTL__CXX_EF
// explicit conversion operators
//#define NTL__CXX_EXPLICITOP
// extern templates
#define NTL__CXX_EXTPL
// initializer lists
//#define NTL__CXX_IL
// lambda
#define NTL__CXX_LAMBDA
// rvalues
//#define NTL__CXX_RV
// template typedef
//#define NTL__CXX_TT
// variadic templates (implies rvalue references support)
//#define NTL__CXX_VT

#endif

#endif // STLX_CONFIG_ICC

