#ifndef NTL__STLX_ESAFETY
#define NTL__STLX_ESAFETY

#include <type_traits> // is_same
#include <iterator> // iterator_traits
#include <cstddef> // STATIC_ASSERT
#include <cassert> // assert
#include <cstring> // memcpy

namespace std {
namespace ext {

/// The template function evaluates alloc.destroy(&(*(first + N))) once for each N in the range [0, count).
template <class InputIterator, class Allocator>
void destroy(InputIterator first,
             InputIterator last,
             Allocator& alloc) __ntl_nothrow
{
  {
    // Making sure users don't get the wrong combinations of iterators
    // and allocator.
    // !fr3@K!
    typedef typename iterator_traits<InputIterator>::value_type iter_value_type;
    typedef typename Allocator::value_type alloc_value_type;
    STATIC_ASSERT((is_same<alloc_value_type, iter_value_type>::value));
  }

  for(; first != last; ++first)
    alloc.destroy(alloc.address(*first));
}

template <class InputIterator>
void destroy(InputIterator first,
             InputIterator last) __ntl_nothrow
{
  typedef typename iterator_traits<InputIterator>::value_type value_type;
  for(; first != last; ++first)
    first->~value_type();
}

} // namespace ext

namespace detail {

template <class ForwardIterator, class Allocator = void>
struct guarded_range_constructor
{
  typedef ForwardIterator iterator;
  typedef Allocator allocator_type;
  typedef typename iterator_traits<ForwardIterator>::value_type value_type;

  guarded_range_constructor(iterator first, allocator_type& alloc) __ntl_nothrow
  : first_(first),
    current_(first),
    array_allocator_(alloc),
    dismissed_(false)
  {
    // Making sure users don't get the wrong combinations of iterator
    // and allocator.
    // Consider using concepts instead of static assertion in the
    // future.
    // !fr3@K!
    typedef value_type iterator_value_type;
    typedef typename allocator_type::value_type array_allocator_value_type;
    STATIC_ASSERT((is_same<iterator_value_type, array_allocator_value_type>::value));
  }
  ~guarded_range_constructor() __ntl_nothrow
  {
    if(dismissed_)
      return;

    std::ext::destroy(first_, current_, array_allocator_);
  }

  void operator()(const value_type& value)
  {
    array_allocator_.construct(array_allocator_.address(*current_), value);
    ++current_;
  }

  iterator dismiss() __ntl_nothrow
  {
    dismissed_ = true;
    return current_;
  }

private:
  // no value semantics
  guarded_range_constructor(const guarded_range_constructor&);
  guarded_range_constructor& operator=(const guarded_range_constructor&);

private:
  iterator first_;
  iterator current_;
  allocator_type& array_allocator_;
  bool dismissed_;
};

template <class ForwardIterator>
struct guarded_range_constructor<ForwardIterator, void>
{
  typedef ForwardIterator iterator;
  typedef typename iterator_traits<ForwardIterator>::value_type value_type;

  guarded_range_constructor(iterator first) __ntl_nothrow
  : first_(first),
    current_(first),
    dismissed_(false)
  {
  }
  ~guarded_range_constructor() __ntl_nothrow
  {
    if(dismissed_)
      return;

    std::ext::destroy(first_, current_);
  }

  void operator()(const value_type& value)
  {
    new (current_) value_type(value);
    ++current_;
  }

  iterator dismiss() __ntl_nothrow
  {
    dismissed_ = true;
    return current_;
  }

private:
  // no value semantics
  guarded_range_constructor(const guarded_range_constructor&);
  guarded_range_constructor& operator=(const guarded_range_constructor&);

private:
  iterator first_;
  iterator current_;
  bool dismissed_;
};

} // namespace detail

namespace ext {

template <class ForwardIterator, class T, class Allocator>
void uninitailized_fill_a(ForwardIterator first,
                          ForwardIterator last,
                          const T& value,
                          Allocator& alloc)
{
  std::detail::guarded_range_constructor<ForwardIterator, Allocator> ctor(first, alloc);
  for(; first != last; ++first)
  {
    ctor(value);
  }
  ctor.dismiss();
  assert(first == ctor.dismiss());
}

template <class ForwardIterator, class Size, class T, class Allocator>
void uninitailized_fill_n_a(ForwardIterator first,
                            Size n,
                            const T& value,
                            Allocator& alloc)
{
  std::detail::guarded_range_constructor<ForwardIterator, Allocator> ctor(first, alloc);
  for(; n; --n, ++first)
  {
    ctor(value);
  }
  ctor.dismiss();
  assert(first == ctor.dismiss());
}

template <class InputIterator, class ForwardIterator, class Allocator>
InputIterator uninitailized_copy_a(InputIterator first,
                                   InputIterator last,
                                   ForwardIterator dst,
                                   Allocator& alloc)
{
  std::detail::guarded_range_constructor<ForwardIterator, Allocator> ctor(dst, alloc);
  for(; first != last; ++first)
  {
    ctor(*first);
  }
  return ctor.dismiss();
}

template <class InputIterator,
          class Size,
          class ForwardIterator,
          class Allocator>
InputIterator uninitailized_copy_n_a(InputIterator first,
                                     Size n,
                                     ForwardIterator dst,
                                     Allocator& alloc)
{
  std::detail::guarded_range_constructor<ForwardIterator, Allocator> ctor(dst, alloc);
  for(; n; --n, ++first)
  {
    ctor(*first);
  }
  return ctor.dismiss();
}

template <class Allocator>
struct guarded_allocation
{
  typedef Allocator allocator_type;
  typedef typename Allocator::pointer pointer;

  guarded_allocation(allocator_type& alloc, size_t n)
  : array_allocator_(alloc),
    n_(n),
    p_(alloc.allocate(n)),
    dismissed_(false)
  {
  }
  ~guarded_allocation() __ntl_nothrow
  {
    if(dismissed_ == true)
      return;

    assert(p_ != nullptr);
    array_allocator_.deallocate(p_, n_);
  }

  pointer get() const __ntl_nothrow
  {
    assert(p_ != nullptr);
    return p_;
  }

  void dismiss() __ntl_nothrow
  {
    dismissed_ = true;
  }

  private:
    allocator_type& array_allocator_;
    size_t n_;
    pointer p_;
    bool dismissed_;
};

template <class InputIterator, class ForwardIterator>
ForwardIterator relocate(
  InputIterator first,
  InputIterator last,
  ForwardIterator dst) __ntl_nothrow
{
  // Making sure users don't get the wrong combinations of source
  // iterators and destination iterators.
  // Consider using concepts instead of static assertion in the
  // future.
  // !fr3@K!
  typedef typename iterator_traits<InputIterator>::value_type src_type;
  typedef typename iterator_traits<ForwardIterator>::value_type dst_type;
  STATIC_ASSERT((is_same<src_type, dst_type>::value));

  for(; first != last; ++first, ++dst)
    std::memcpy(&(*dst), &(*first), sizeof(src_type));

  return dst;
}

template <class InputIterator, class ForwardIterator, class Allocator>
struct guarded_relocation
{
  typedef typename iterator_traits<InputIterator>::value_type value_type;

  guarded_relocation(InputIterator first,
                   InputIterator last,
                   ForwardIterator dst,
                   Allocator& alloc) __ntl_nothrow
  : dst_(dst),
    dst_last_(relocate(first, last, dst)),
    array_allocator_(alloc),
    dismissed_(false)
  {
    // Making sure users don't get the wrong combinations of source
    // iterators and destination iterators.
    // Consider using concepts instead of static assertion in the
    // future.
    // !fr3@K!
    typedef typename iterator_traits<InputIterator>::value_type src_type;
    typedef typename iterator_traits<ForwardIterator>::value_type dst_type;
    STATIC_ASSERT((is_same<src_type, dst_type>::value));
  }

  ~guarded_relocation() __ntl_nothrow
  {
    if(dismissed_)
      return;

    destroy(dst_, dst_last_, array_allocator_);
  }

  void dismiss() __ntl_nothrow
  {
    dismissed_ = true;
  }

private:
  ForwardIterator dst_;
  ForwardIterator dst_last_;
  Allocator& array_allocator_;
  bool dismissed_;
};


// I am greedy. This class serves two purposes:
//  1. Explicitly marks the nothrow code path.
//  2. `assert`s the code path really not throwing.
//
// For those who has seen me commenting a code block as following:
//  void bar()
//  {
//    // no-throw begin
//    foo();
//    // no-throw end
//  }
// This is to replace the comment:
//  void bar()
//  {
//    nothrow_guard ntg;
//    foo();
//    ntg.dismiss();
//  }
//
// !fr3@K!
class nothrow_guard
{
public:
  nothrow_guard() __ntl_nothrow
  : dismissed_(false)
  {}
  ~nothrow_guard() __ntl_nothrow
  {
    assert(dismissed_ == true);
  }
  void dismiss() __ntl_nothrow
  {
    dismissed_ = true;
  }

private:
  // No value semantics
  nothrow_guard(const nothrow_guard&);
  nothrow_guard& operator=(const nothrow_guard&);

private:
  bool dismissed_;
};

} // namespace ext
} // namespace std

// These macros are used to mark and `assert` a nothrow code path does
// NOT throw.
// One could use `nothrow_guard` for the same purpose, but I would like
// the nothrow code path to be more VISIBLE in code. This helps me a
// lot when writing exception code.
// Example:
//  void bar0()
//  {
//    nothrow_guard ntg;
//    foo();
//    ntg.dismiss();
//  }
//  void bar1()
//  {
//    NTL__NOTHROW_GUARD_ENGAGE();
//    foo();
//    NTL__NOTHROW_GUARD_DISMISS();
//  }
//
// The no throw code path in bar1() is way more visible than in bar0().
//
// !fr3@K!

#define NTL__NOTHROW_GUARD_PREFIX \
  ntl__nothrow_guard

#if defined(NDEBUG)

  #define NTL__NOTHROW_GUARD_ENGAGE() \
    int ntl__nothrow_guard_default
  #define NTL__NOTHROW_GUARD_DISMISS() \
    ((void)ntl__nothrow_guard_default)

  #define NTL__NOTHROW_GUARD_ENGAGE_NAMED(ntg_name) \
    int NTL__NOTHROW_GUARD_PREFIX##_##ntg_name
  #define NTL__NOTHROW_GUARD_DISMISS_NAMED(ntg_name) \
    ((void)NTL__NOTHROW_GUARD_PREFIX##_##ntg_name)

#else // #if defined(NDEBUG)

  #define NTL__NOTHROW_GUARD_ENGAGE() \
    ::std::ext::nothrow_guard ntl__nothrow_guard_default
  #define NTL__NOTHROW_GUARD_DISMISS() \
    ntl__nothrow_guard_default.dismiss()

  #define NTL__NOTHROW_GUARD_ENGAGE_NAMED(ntg_name) \
    ::std::ext::nothrow_guard NTL__NOTHROW_GUARD_PREFIX##_##ntg_name
  #define NTL__NOTHROW_GUARD_DISMISS_NAMED(ntg_name) \
    NTL__NOTHROW_GUARD_PREFIX##_##ntg_name.dismiss()

#endif // #if defined(NDEBUG)

#endif//#ifndef NTL__STLX_ESAFETY
