/**\file*********************************************************************
 *                                                                     \brief
 *  Utility components [20.2 lib.utility]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_UTILITY
#define NTL__STLX_UTILITY

/// Standard Template Library
namespace std {

/**\defgroup  lib_utilities *** 20 General utilities library [utilities] *****
 *
 *    Components used by other elements of the Standard C + + library.
 *@{
 */

/**\defgroup  lib_utility ************** Utility components [20.2] **********
 *
 *    basic function and class templates that are used throughout
 *    the rest of the library.
 *@{
 */

/// 20.2.1 Operators [operators]
/// 1 To avoid redundant definitions of operator!= out of operator== and
///   operators >, <=, and >= out of operator<, the library provides the following:
namespace rel_ops {

template<class T> inline
bool operator!=(const T & x, const T & y) { return !(x == y); }

template<class T> inline 
bool operator> (const T & x, const T & y) { return y < x; }

template<class T> inline 
bool operator<=(const T & x, const T & y) { return !(y < x); }

template<class T> inline 
bool operator>=(const T & x, const T & y) { return !(x < y); }

}//namespace rel_ops


/// Pairs [20.2.2 lib.pairs]
#pragma warning(push)
// assignment operator could not be generated if either T is const
#pragma warning(disable:4512)
template<class T1, class T2>
struct pair
{
    typedef T1  first_type;
    typedef T2  second_type;

    T1  first;
    T2  second;

    pair() : first(T1()), second(T2()) {}
    pair(const T1 & x, const T2 & y) : first(x), second(y) {}

    template<class U, class V>
    pair(const pair<U, V> & p) : first(p.first), second(p.second) {}
};
#pragma warning(pop)

///\name  Comparisons
///@{

template<class T1, class T2> 
inline
bool
  operator==(const pair<T1, T2> & x, const pair<T1, T2> & y)
{ 
  return x.first == y.first && x.second == y.second;
}

template<class T1, class T2> 
inline
bool
  operator<(const pair<T1, T2> & x, const pair<T1, T2> & y)
{ 
  const bool less = x.first < y.first;
  return less || (!less && x.second < y.second);
}

template<class T1, class T2> 
inline
bool
  operator!=(const pair<T1, T2> & x, const pair<T1, T2> & y)
{ 
  return !(x == y);
}

template<class T1, class T2>
inline
bool
  operator> (const pair<T1, T2> & x, const pair<T1, T2> & y)
{ 
  return y < x;
}

template<class T1, class T2>
inline
bool
  operator>=(const pair<T1, T2> & x, const pair<T1, T2> & y)
{ 
  return !(y < x);
}

template<class T1, class T2>
inline
bool
  operator<=(const pair<T1, T2> & x, const pair<T1, T2> & y)
{ 
  return !(x < y);
}

///@}

template<class T1, class T2>
inline
pair<T1, T2>
  make_pair(T1 x, T2 y)
{ 
  return pair<T1, T2>( x, y );
}

/**@} lib_utility */
/**@} lib_utilities */

}//namespace std

#endif//#ifndef NTL__STLX_UTILITY
