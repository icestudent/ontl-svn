#ifndef NTL__STLX_ESAFETY
#define NTL__STLX_ESAFETY

#include <type_traits>
#include <algorithm>
#include <cassert>

namespace std {
namespace detail
{

template <class Allocator>
struct destructor_a
{
  typedef Allocator allocator_type;
  typedef typename Allocator::value_type value_type;

  destructor_a(allocator_type& alloc) __ntl_nothrow
  : array_allocator_(alloc) {}

  void operator()(value_type& val) __ntl_nothrow
  {
    array_allocator_.destroy(&val);
  }

private:
  allocator_type& array_allocator_;
};

template <class Allocator>
destructor_a<Allocator>
  destructor(Allocator& alloc) __ntl_nothrow
{
  return destructor_a<Allocator>(alloc);
}

/// The template function evaluates alloc.destroy(&(*(first + N))) once for each N in the range [0, count).
template <class InputIterator, class Allocator>
void destroy(InputIterator first,
             InputIterator last,
             Allocator& alloc) __ntl_nothrow
{
  for_each(first, last, destructor(alloc));
}

template <class ForwardIterator, class Allocator>
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

    destroy(first_, current_, array_allocator_);
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

template <class ForwardIterator, class T, class Allocator>
void uninitailized_fill_a(ForwardIterator first,
                          ForwardIterator last,
                          const T& value,
                          Allocator& alloc)
{
  guarded_range_constructor<ForwardIterator, Allocator> ctor(first, alloc);
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
  guarded_range_constructor<ForwardIterator, Allocator> ctor(first, alloc);
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
  guarded_range_constructor<ForwardIterator, Allocator> ctor(dst, alloc);
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
  guarded_range_constructor<ForwardIterator, Allocator> ctor(dst, alloc);
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
void relocate(
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
    memcpy(&(*dst), &(*first), sizeof(src_type));
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
    dst_last_(dst),
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

    for(; first != last; ++first, ++dst_last_)
      memcpy(array_allocator_.address(*dst), array_allocator_.address(*first), sizeof(value_type));
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

class no_throw_guard
{
public:
  no_throw_guard() __ntl_nothrow
#if !defined(NDEBUG)
  : dismissed_(false)
#endif
  {}
  ~no_throw_guard() __ntl_nothrow
  {
#if !defined(NDEBUG)
    assert(dismissed_ == true);
#endif
  }
  void dismiss() __ntl_nothrow
  {
#if !defined(NDEBUG)
    dismissed_ = true;
#endif
  }

private:
  // No value semantics
  no_throw_guard(const no_throw_guard&);
  no_throw_guard& operator=(const no_throw_guard&);

private:
#if !defined(NDEBUG)
  bool dismissed_;
#endif
};

} // namespace detail
}//namespace std

#endif//#ifndef NTL__STLX_ESAFETY
