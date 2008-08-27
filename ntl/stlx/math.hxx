#ifndef NTL__STLX_MATH
#define NTL__STLX_MATH

#include "cstddef.hxx"
#include <type_traits>

namespace std {

namespace ext {

// Is Power Of 2, optionally used in
// vector_allocation_policy().
// Ref: http://www.aggregate.org/MAGIC/#Is%20Power%20of%202
template <class UnsignedIntegerType>
inline bool ispo2(UnsignedIntegerType n) __ntl_nothrow
{
  STATIC_ASSERT(is_integral<UnsignedIntegerType>::value && is_unsigned<UnsignedIntegerType>::value);
  return ((n&(n-1)) == 0) ? true : false;
}

// Next Largest Power of 2, optionally used in
// vector_allocation_policy().
// Ref: http://www.aggregate.org/MAGIC/#Next%20Largest%20Power%20of%202
template <class UnsignedIntegerType>
typename enable_if<sizeof(UnsignedIntegerType) == 1, UnsignedIntegerType>::type
  nlpo2_impl(UnsignedIntegerType n) __ntl_nothrow
{
  n |= (n >> 1);
  n |= (n >> 2);
  n |= (n >> 4);
  return(++n);
}
template <class UnsignedIntegerType>
typename enable_if<sizeof(UnsignedIntegerType) == 2, UnsignedIntegerType>::type
  nlpo2_impl(UnsignedIntegerType n) __ntl_nothrow
{
  n |= (n >> 1);
  n |= (n >> 2);
  n |= (n >> 4);
  n |= (n >> 8);
  return(++n);
}
template <class UnsignedIntegerType>
typename enable_if<sizeof(UnsignedIntegerType) == 4, UnsignedIntegerType>::type
  nlpo2_impl(UnsignedIntegerType n) __ntl_nothrow
{
  n |= (n >> 1);
  n |= (n >> 2);
  n |= (n >> 4);
  n |= (n >> 8);
  n |= (n >> 16);
  return(++n);
}
template <class UnsignedIntegerType>
typename enable_if<sizeof(UnsignedIntegerType) == 8, UnsignedIntegerType>::type
  nlpo2_impl(UnsignedIntegerType n) __ntl_nothrow
{
  n |= (n >> 1);
  n |= (n >> 2);
  n |= (n >> 4);
  n |= (n >> 8);
  n |= (n >> 16);
  n |= (n >> 32);
  return(++n);
}

template <class UnsignedIntegerType>
inline UnsignedIntegerType nlpo2(UnsignedIntegerType n) __ntl_nothrow
{
  STATIC_ASSERT(is_integral<UnsignedIntegerType>::value && is_unsigned<UnsignedIntegerType>::value);
  return nlpo2_impl(n);
}

} // namespace ext
}//namespace std

#endif//#ifndef NTL__STLX_MATH
