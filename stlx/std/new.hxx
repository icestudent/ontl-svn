/**\file*********************************************************************
 *                                                                     \brief
 *  Dynamic memory management [18.5 support.dynamic]
 *
 ****************************************************************************
 */

#ifndef STLX_NEW
#define STLX_NEW

#include "../cstddef.hxx"
#include "exception.hxx"

namespace stlx
{

  /**\addtogroup  lib_language_support *** 18 Language support library [language.support] ******
 *@{*/

/**\defgroup  lib_support_dynamic ****** 18.5 Dynamic memory management [support.dynamic] ***
 *@{*/

/**\defgroup  lib_alloc_errors ********* 18.5.2 Storage allocation errors [alloc.errors] *
 *@{*/

/// Class bad_alloc [18.5.2.1 lib.bad.alloc]
class bad_alloc : public exception
{
  public:
    bad_alloc() __ntl_nothrow {}
    bad_alloc(const bad_alloc&) __ntl_nothrow {}
    bad_alloc& operator=(const bad_alloc&) __ntl_nothrow { return *this; }
    virtual ~bad_alloc() __ntl_nothrow {}
    virtual const char* what() const __ntl_nothrow { return "bad_alloc"; }
};

struct  nothrow_t {};

#if defined(_MSC_VER)
__declspec(selectany) extern const nothrow_t nothrow = {};
#elif defined(__BCPLUSPLUS__)
__declspec(selectany) extern const nothrow_t nothrow;
#elif defined(__GNUC__)
 extern const nothrow_t nothrow;
#endif

/// Type new_handler [18.5.2.2 lib.new.handler]
typedef void (*new_handler)();

/// set_new_handler [18.5.2.3 lib.set.new.handler]
new_handler set_new_handler(new_handler new_p) __ntl_nothrow;

/**@} lib_alloc_errors */
/**@} lib_support_dynamic */
/**@} lib_language_support */

}//namespace stlx

/**\addtogroup  lib_language_support
 *@{*/

/**\addtogroup  lib_support_dynamic
 *@{*/

// Storage allocation and deallocation [18.5.1 lib.new.delete]
#ifdef _MSC_VER
#define _newdecl __cdecl
#else
#define _newdecl 
#endif

///\name  Single-object forms [18.5.1.1 lib.new.delete.single]

void* _newdecl operator new      (stlx::size_t size) __ntl_throws(stlx::bad_alloc);
void  _newdecl operator delete   (void* ptr) __ntl_nothrow;

void* _newdecl operator new      (stlx::size_t size, const stlx::nothrow_t&) __ntl_nothrow;
void  _newdecl operator delete   (void* ptr, const stlx::nothrow_t&) __ntl_nothrow;

///\name  Array forms [18.5.1.2 lib.new.delete.array]

void* _newdecl operator new[]    (stlx::size_t size) __ntl_throws(stlx::bad_alloc);
void  _newdecl operator delete[] (void* ptr) __ntl_nothrow;

void* _newdecl operator new[]    (stlx::size_t size, const stlx::nothrow_t&) __ntl_nothrow;
void  _newdecl operator delete[] (void* ptr, const stlx::nothrow_t&) __ntl_nothrow;

///\name  Placement forms [18.5.1.3 lib.new.delete.placement]
///\note  Standard says nothing about inline nor static, but we're avoiding LNK2005

__forceinline
void* _newdecl operator new      (stlx::size_t, void* ptr) __ntl_nothrow  { return ptr; }

__forceinline
void  _newdecl operator delete   (void*, void*) __ntl_nothrow            {}

__forceinline
void* _newdecl operator new[]    (stlx::size_t, void* ptr) __ntl_nothrow  { return ptr; }

__forceinline
void  _newdecl operator delete[] (void*, void*) __ntl_nothrow            {}

#undef operator


///\name Variable-size structures support
struct varsize_tag {};

#if defined(_MSC_VER)
__declspec(selectany) extern const varsize_tag varsize = {};
#elif defined(__BCPLUSPLUS__)
__declspec(selectany) extern const varsize_tag varsize;
#elif defined(__GNUC__)
 extern const varsize_tag varsize;
#endif

__forceinline
void* operator new(stlx::size_t size, const varsize_tag&, stlx::size_t aux_size)
{
  return ::operator new(size+aux_size);
}

__forceinline
void operator delete(void* ptr, const varsize_tag&, stlx::size_t)
{
  return ::operator delete(ptr);
}
///@}

/**@} lib_support_dynamic */
/**@} lib_language_support */

#endif//#ifndef STLX_NEW
