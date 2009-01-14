/**\file*********************************************************************
 *                                                                     \brief
 *  20.6 Function objects [function.objects]
 *  Implementation for C++0x compilers
 ****************************************************************************
 */
#ifndef STLX_FUNCTION_VT
#define STLX_FUNCTION_VT

namespace stlx
{
  // 20.6.15 polymorphic function wrappers:
  class bad_function_call;
  template<class> class function; // undefined
  template<class R, class... ArgTypes> class function<R(ArgTypes...)>;
  template<class R, class... ArgTypes>
  void swap(function<R(ArgTypes...)>&, function<R(ArgTypes...)>&);
  template<class R, class... ArgTypes>
  void swap(function<R(ArgTypes...)>&&, function<R(ArgTypes...)>&);
  template<class R, class... ArgTypes>
  void swap(function<R(ArgTypes...)>&, function<R(ArgTypes...)&&);
  template<class R, class... ArgTypes>
  bool operator==(const function<R(ArgTypes...)>&, unspecified-null-pointer-type );
  template<class R, class... ArgTypes>
  bool operator==(unspecified-null-pointer-type , const function<R(ArgTypes...)>&);
  template<class R, class... ArgTypes>
  bool operator!=(const function<R(ArgTypes...)>&, unspecified-null-pointer-type );
  template<class R, class... ArgTypes>
  bool operator!=(unspecified-null-pointer-type , const function<R(ArgTypes...)>&);

}

#endif // STLX_FUNCTION_VT
