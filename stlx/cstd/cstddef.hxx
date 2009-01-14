/**\file*********************************************************************
 *                                                                     \brief
 *  18.1 Types [lib.support.types]
 *
 ****************************************************************************
 */

#ifndef STLX_CSTD_CSTDDEF
#define STLX_CSTD_CSTDDEF

#include "../../config/stlxdef.hxx"

namespace stlx {

/**\defgroup  lib_language_support ***** 18 Language support library [language.support] ******
 *@{*/

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable:4324)
  #ifndef __ICL
  typedef alignas(8192)  struct {} max_align_t;
  #else
  typedef alignas(8192)  struct {void*_;} max_align_t;
  #endif
  #pragma warning(pop)
#else
  typedef struct {} max_align_t;
#endif

/**\defgroup  lib_support_types ******** 18.1 Types [support.types] ***********************
 *@{*/

#ifdef NTL__CXX_NULLPTR
  typedef decltype(nullptr) nullptr_t;
#else

//based on SC22/WG21/N2431 = J16/07-0301
struct nullptr_t
{
    template<typename any> operator any * () const { return 0; }
    template<class any, typename T> operator T any:: * () const { return 0; }

    #ifdef _MSC_VER
    struct pad {};
    pad __[sizeof(void*)/sizeof(pad)];
    #else
    char __[sizeof(void*)];
    #endif
  private:
  //  nullptr_t();// {}
  //  nullptr_t(const nullptr_t&);
  //  void operator = (const nullptr_t&);
    void operator &() const;
    template<typename any> void operator +(any) const { /*I Love MSVC 2005!*/ }
    template<typename any> void operator -(any) const { /*I Love MSVC 2005!*/ }

};
static const nullptr_t __nullptr = {};

  #ifndef nullptr
    #define nullptr stlx::__nullptr
  #endif
#endif // NTL__CXX
static_assert(sizeof(nullptr)==sizeof(void*), "3.9.1.10: sizeof(stlx::nullptr_t) shall be equal to sizeof(void*)");

#ifndef NULL
#define NULL 0
#endif

#ifdef __GNUC__

  typedef __PTRDIFF_TYPE__  ptrdiff_t;
  typedef __SIZE_TYPE__     size_t;
  typedef __PTRDIFF_TYPE__  ssize_t;

#else

  #ifdef _M_X64
      typedef          long long  ptrdiff_t;
  # ifdef _MSC_VER
      using ::size_t;
  # else
      typedef unsigned long long  size_t;
  # endif
      typedef          long long  ssize_t;
  #else
      typedef __w64    int  ptrdiff_t;
  # ifdef _MSC_VER
      using ::size_t;
  # else
      typedef unsigned int  size_t;
  # endif
      typedef __w64    int  ssize_t;
  #endif

#endif



#ifndef offsetof
  #define offsetof(s,m) (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))
  // from bcb #define offsetof( s_name, m_name )  (size_t)&(((s_name*)0)->m_name)
#endif

#define __ntl_bitmask_type(bitmask, _func_spec)\
  _func_spec bitmask operator&(bitmask x, bitmask y) { return bitwise_and(x, y); }\
  _func_spec bitmask operator|(bitmask x, bitmask y) { return bitwise_or (x, y); }\
  _func_spec bitmask operator^(bitmask x, bitmask y) { return bitwise_xor(x, y); }\
  _func_spec bitmask operator~(bitmask x) { return static_cast<bitmask>(~static_cast<unsigned>(x)); }\
  _func_spec bitmask& operator&=(bitmask& x, bitmask y) { x = x&y ; return x ; }\
  _func_spec bitmask& operator|=(bitmask& x, bitmask y) { x = x|y ; return x ; }\
  _func_spec bitmask& operator^=(bitmask& x, bitmask y) { x = x^y ; return x ; }

/**@} lib_support_types */
/**@} lib_language_support */


  /**\addtogroup  lib_utilities ********** 20 General utilities library [utilities]
  *@{*/
  /**\addtogroup  lib_memory ************* 20.7 Memory [memory]
  *@{*/

  /// 20.7.1 Allocator argument tag [allocator.tag]
  struct allocator_arg_t {/**/};
#ifdef _MSC_VER
  const allocator_arg_t allocator_arg;// = allocator_arg_t();
#else
  const allocator_arg_t allocator_arg = allocator_arg_t();
#endif
  /**@} lib_memory */
  /**@} lib_utilities */


} //namespace stlx

#ifndef _MSC_VER
  using stlx::ptrdiff_t;
  using stlx::size_t;
#endif

/// _countof macro to calculate array length in compile time
template <typename T, size_t N>
char (*__countof_helper(T(&array)[N]))[N];
#define _countof(array) sizeof(*__countof_helper(array))


#endif //#ifndef STLX_CSTD_CSTDDEF
