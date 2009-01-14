/**\file*********************************************************************
 *                                                                     \brief
 *  Numerics library [25 lib.numerics]
 *
 ****************************************************************************
 */

#ifndef STLX_NUMERICS
#define STLX_NUMERICS

#ifndef STLX_ITERATOR
#include "iterator.hxx"
#endif



namespace stlx {

// 26.6 Generalized numeric operations [numeric.ops]

  // 26.6.1 Accumulate [accumulate]
  template <class InputIterator, class T>
  __forceinline
  T
    accumulate(InputIterator first, InputIterator last, T init)
  {
    while(first != last){
      init = init + *first;
      ++first;
    }
    return init;
  }

  // 26.6.1 Accumulate [accumulate]
  template <class InputIterator, class T, class BinaryOperation>
  __forceinline
  T
    accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op)
  {
    while(first != last){
      init = binary_op(init, *first);
      ++first;
    }
    return init;
  }

} // namespace stlx
#endif // STLX_NUMERICS
