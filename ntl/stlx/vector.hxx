/**\file*********************************************************************
 *                                                                     \brief
 *  Class template vector [23.2.6 lib.vector]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_VECTOR
#define NTL__STLX_VECTOR

#include "algorithm.hxx"
#include "iterator.hxx"
#include "memory.hxx"
//#include "stdexcept.hxx"
#include "type_traits.hxx"

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:4127) // conditional expression is constant - clear()
# pragma warning(disable:4820) // X bytes padding added...
# pragma warning(disable:4710) // insert__blank_space(char * const,unsigned int)' : function not inlined
#endif

namespace std {

/**\defgroup  lib_containers *********** Containers library [23] ************
 *@{ *    Components to organize collections of information
 */

/**\defgroup  lib_sequence ************* Sequences [23.2] *******************
 *@{ *    Containers that organizes a finite set of objects,
 *    all of the same type, into a strictly linear arrangement.
 */

namespace detail
{

// There should be a better place for things in the detail namespace.
// !fr3@K!

template <class ForwardIterator, class Allocator>
struct range_construct_guard
{
  typedef ForwardIterator iterator;
  typedef Allocator allocator_type;
  typedef typename iterator_traits<ForwardIterator>::value_type value_type;

  range_construct_guard(iterator first, allocator_type& alloc) __ntl_nothrow
  : first_(first),
    current_(first),
    alloc_(alloc),
    committed_(false)
  {
    // Making sure users don't get the wrong combinations of iterator
    // and allocator.
    // Consider using concepts instead of static assertion in the
    // future.
    // !fr3@K!
    typedef value_type iterator_value_type;
    typedef typename allocator_type::value_type allocator_value_type;
    STATIC_ASSERT((is_same<iterator_value_type, allocator_value_type>::value));
  }
  ~range_construct_guard() __ntl_nothrow
  {
    if(committed_)
      return;

    for(; first_ != current_; ++first_)
      alloc_.destroy(first_);
  }

  void construct_from(const value_type& value)
  {
    alloc_.construct(current_, value);
    ++current_;
  }

  iterator commit()
  {
    committed_ = true;
    return current_;
  }

private:
  // no value semantics
  range_construct_guard(const range_construct_guard&);
  range_construct_guard& operator=(const range_construct_guard&);

private:
  iterator first_;
  iterator current_;
  allocator_type& alloc_;
  bool committed_;
};

template <class ForwardIterator, class T, class Allocator>
void uninitailized_fill_a(ForwardIterator first,
                          ForwardIterator last,
                          const T& value,
                          Allocator& alloc)
{
  range_construct_guard<ForwardIterator, Allocator> g(first, alloc);
  for(; first != last; ++first)
  {
    g.construct_from(value);
  }
  g.commit();
  assert(first == g.commit());
}

template <class ForwardIterator, class Size, class T, class Allocator>
void uninitailized_fill_n_a(ForwardIterator first,
                            Size n,
                            const T& value,
                            Allocator& alloc)
{
  range_construct_guard<ForwardIterator, Allocator> g(first, alloc);
  for(; n; --n, ++first)
  {
    g.construct_from(value);
  }
  g.commit();
  assert(first == g.commit());
}

template <class InputIterator, class ForwardIterator, class Allocator>
InputIterator uninitailized_copy_a(InputIterator first,
                                   InputIterator last,
                                   ForwardIterator dst,
                                   Allocator& alloc)
{
  range_construct_guard<ForwardIterator, Allocator> g(dst, alloc);
  for(; first != last; ++first, ++dst)
  {
    g.construct_from(*first);
  }
  return g.commit();
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
  range_construct_guard<ForwardIterator, Allocator> g(dst, alloc);
  for(; n; ++first, ++dst)
  {
    g.construct_from(*first);
  }
  return g.commit();
}

// Next Largest Power of 2, optionally used in
// vector_allocation_policy().
// Ref: http://www.aggregate.org/MAGIC/#Next%20Largest%20Power%20of%202
template <size_t Size>
struct nlpo2_impl
{
};

template <>
struct nlpo2_impl<32>
{
  static uint32_t get(uint32_t n)
  {
    n |= (n >> 1);
    n |= (n >> 2);
    n |= (n >> 4);
    n |= (n >> 8);
    n |= (n >> 16);
    return(++n);
  }
};
template <>
struct nlpo2_impl<64>
{
  static uint64_t get(uint64_t n)
  {
    n |= (n >> 1);
    n |= (n >> 2);
    n |= (n >> 4);
    n |= (n >> 8);
    n |= (n >> 16);
    n |= (n >> 32);
    return(++n);
  }
};

size_t nlpo2(size_t n)
{
  return nlpo2_impl<sizeof(size_t) * 8>::get(n);
}

//#define NTL__VECTOR_ALLOCATION_POLICY__USER_DEFINED
//#define NTL__VECTOR_ALLOCATION_POLICY__AGGRESSIVE
//#define NTL__VECTOR_ALLOCATION_POLICY__CONSERVATIVE

// Makes sure no more than one vector allocation policy is defined.
#if (defined(NTL__VECTOR_ALLOCATION_POLICY__USER_DEFINED) && \
    (defined(NTL__VECTOR_ALLOCATION_POLICY__AGGRESSIVE) || \
     defined(NTL__VECTOR_ALLOCATION_POLICY__CONSERVATIVE))) || \
     (defined(NTL__VECTOR_ALLOCATION_POLICY__AGGRESSIVE) && \
      defined(NTL__VECTOR_ALLOCATION_POLICY__CONSERVATIVE))
# error Only one vector capacity policy is allowed!
#endif

// Defines default vector allocation behavior, if none defined.
#if !defined(NTL__VECTOR_ALLOCATION_POLICY__USER_DEFINED) && \
    !defined(NTL__VECTOR_ALLOCATION_POLICY__AGGRESSIVE) && \
    !defined(NTL__VECTOR_ALLOCATION_POLICY__CONSERVATIVE)
# define NTL__VECTOR_ALLOCATION_POLICY__CONSERVATIVE
#endif

// Interface to vector allocation policy.
size_t vector_allocation_policy(size_t min)
{
#if defined(NTL__VECTOR_ALLOCATION_POLICY__USER_DEFINED)
  // user with user-defined policy must declare their policy as:
  return ::ntl_user_defined::vector_allocation_policy(min);

#elif defined(NTL__VECTOR_ALLOCATION_POLICY__AGGRESSIVE)
  return nlpo2(min);

#else // NTL__VECTOR_ALLOCATION_POLICY__CONSERVATIVE
  return min;
#endif
}

template <bool>
struct make_bool
{
  typedef false_type value;
};
template <>
struct make_bool<true>
{
  typedef true_type value;
};

template <class Allocator>
struct allocation_guard
{
  typedef Allocator allocator_type;
  typedef typename Allocator::pointer pointer;

  allocation_guard(allocator_type& alloc, size_t n)
  : alloc_(alloc),
    n_(n),
    p_(alloc.allocate(n))
  {
  }
  ~allocation_guard()
  {
    if(p_ == nullptr)
      return;

    alloc_.deallocate(p_, n_);
  }

  pointer get() const
  {
    assert(p_ != nullptr);
    return p_;
  }

  void commit()
  {
    p_ = nullptr;
  }

  private:
    allocator_type& alloc_;
    size_t n_;
    pointer p_;
};

} // namespace detail

/// Class template vector [23.2.6]
template <class T, class Allocator = allocator<T> >
class vector
{
    template <class T, class traits, class Allocator> friend class basic_string;

  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef           T                           value_type;
    typedef           Allocator                   allocator_type;
    typedef typename  Allocator::pointer          pointer;
    typedef typename  Allocator::const_pointer    const_pointer;
    typedef typename  Allocator::reference        reference;
    typedef typename  Allocator::const_reference  const_reference;
    typedef typename  Allocator::size_type        size_type;
    typedef typename  Allocator::difference_type  difference_type;

    typedef pointer                               iterator;
    typedef const_pointer                         const_iterator;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  private:
    typedef detail::allocation_guard<allocator_type> allocation_guard;

    void construct(size_type n, const T& value)
    {
      iterator i = begin_;
      while ( n-- ) alloc_.construct(i++, value);
      end_ = i;
    }

    template <class InputIterator>
    __forceinline
    void construct(InputIterator first, size_type n)
    {
      iterator i = begin_;
      for ( ; n--; ++first, ++i )
        alloc_.construct(i, *first);
      end_ = i;
    }

    template <class InputIterator>
    void vector__disp(InputIterator first, InputIterator last,
                      const input_iterator_tag)
    {
      capacity_ = 0;
      begin_ = end_ = nullptr;

      for(; first != last; ++first)
        this->push_back(*first);
    }

    template <class ForwardIterator, class IteratorCategory>
    void vector__disp(ForwardIterator first, ForwardIterator last,
                      const IteratorCategory)
    {
      size_type n = distance(first, last);
      capacity_ = detail::vector_allocation_policy(n);
      allocation_guard g(alloc_, capacity_);
      begin_ = end_ = g.get();
      detail::uninitailized_copy_a(first, last, begin_, alloc_);
      g.commit();
      end_ += n;
    }

  public:

    ///\name construct/copy/destroy [23.2.6.1]

    explicit vector(const Allocator& a = Allocator())
    : alloc_(a), capacity_(0), begin_(nullptr), end_(nullptr) {}

    explicit vector(size_type n,
                    const T& value      = T(),
                    const Allocator& a  = Allocator())
    : alloc_(a),
      capacity_(detail::vector_allocation_policy(n)),
      begin_(alloc_.allocate(capacity_)),
      end_(begin_)
    {
      detail::uninitailized_fill_n_a(begin_, n, value, alloc_);
      end_ += n;
    }

    template <class InputIterator>
    vector(InputIterator first,
           InputIterator last,
           const Allocator& a = Allocator())
    : alloc_(a)
    {
      vector__disp(
        first,
        last,
        typename iterator_traits<InputIterator>::iterator_category());
    }

    __forceinline
    vector(const vector<T, Allocator>& x)
    : alloc_(x.alloc_),
      capacity_(detail::vector_allocation_policy(x.size()))
    {
      if ( !capacity_ )
      {
        begin_ = end_ = nullptr;
        return;
      }

      allocation_guard g(alloc_, capacity_);
      begin_ = end_ = g.get();
      detail::uninitailized_copy_a(x.begin(), x.end(), begin_, alloc_);
      g.commit();
      end_ += x.size();
    }

    __forceinline
    ~vector() __ntl_nothrow
    {
      if(capacity_ == 0)
        return;

      clear();
      alloc_.deallocate(begin_, capacity_);
    }

    vector<T, Allocator>& operator=(const vector<T, Allocator>& x)
    {
      assign(x.begin(), x.end());
      return *this;
    }

    template <class InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
      assign__disp(
        first,
        last,
        typename iterator_traits<InputIterator>::iterator_category());
    }

    void assign(size_type n, const T& u)
    {
      clear();
      if ( capacity() < n )
      {
        alloc_.deallocate(begin_, capacity_);
        capacity_ = 0;
        begin_ = end_ = nullptr;
        size_type new_capacity = detail::vector_allocation_policy(n);
        allocation_guard g(alloc_, new_capacity);
        capacity_ = new_capacity;
        begin_ = end_ = g.get();
        detail::uninitailized_fill_n_a(begin_, n, u, alloc_);
        g.commit();
        end_ += n;
        return;
      }

      detail::uninitailized_fill_n_a(begin_, n, u, alloc_);
      end_ = begin_ + n;
    }

    allocator_type get_allocator() const  { return alloc_; }

    ///@}

  private:

    template <class InputIterator>
    void assign__disp(InputIterator first, InputIterator last,
                      const input_iterator_tag)
    {
      clear();
      for(; first != last; ++first)
        push_back(*first);
    }

    template <class ForwardIterator, class IteratorCategory>
    void assign__disp(ForwardIterator first, ForwardIterator last,
                      const IteratorCategory)
    {
      clear();
      // distance SHOULD always be positive
      size_type n = static_cast<size_type>(distance(first, last));
      if ( capacity() < n )
      {
        alloc_.deallocate(begin_, capacity_);
        capacity_ = 0;
        begin_ = end_ = nullptr;
        size_type new_capacity = detail::vector_allocation_policy(n);
        allocation_guard g(alloc_, new_capacity);
        capacity_ = new_capacity;
        begin_ = end_ = g.get();
        detail::uninitailized_copy_a(first, last, begin_, alloc_);
        g.commit();
        end_ += n;
        return;
      }

      detail::uninitailized_copy_a(first, last, begin_, alloc_);
      end_ = begin_ + n;
    }

  public:

    ///\name  iterators

    iterator                begin()       { return begin_; }
    const_iterator          begin() const { return begin_; }
    iterator                end()         { return end_; }
    const_iterator          end()   const { return end_; }

    reverse_iterator        rbegin()      { return reverse_iterator(end_); }
    const_reverse_iterator  rbegin() const
      { return const_reverse_iterator(end_); }
    reverse_iterator        rend()       { return reverse_iterator(begin_); }
    const_reverse_iterator  rend() const
      { return const_reverse_iterator(begin_); }

    const_iterator          cbegin() const { return begin(); }
    const_iterator          cend()   const { return end(); }
    const_reverse_iterator  crbegin()const { return rbegin(); }
    const_reverse_iterator  crend()  const { return rend(); }

    ///\name  capacity [23.2.6.2]

    size_type size()      const { return static_cast<size_type>(end_- begin_); }
    size_type max_size()  const { return alloc_.max_size(); }
    size_type capacity()  const { return capacity_; }
    bool      empty()     const { return begin_ == end_; }

    void resize(size_type sz, const T& c = T())
    {
      iterator new_end = begin_ + sz;
      while ( new_end < end_ ) pop_back();
      if    ( new_end > end_ ) insert(end_, new_end - end_, c);
    }

    void reserve(size_type n) __ntl_throws(bad_alloc) //throw(length_error)
    {
      if ( capacity() < n ) realloc(n);
    }

    ///\name  element access

    reference       operator[](size_type n)       { return *(begin_ + n); }
    const_reference operator[](size_type n) const { return *(begin_ + n); }

    const_reference at(size_type n) const
    {
      check_bounds(n);
      return operator[](n);
    }

    reference at(size_type n)
    {
      check_bounds(n);
      return operator[](n);
    }

    reference       front()       __ntl_nothrow { return *begin(); }
    const_reference front() const __ntl_nothrow { return *begin(); }
    reference       back()        __ntl_nothrow { return *(end() - 1); }
    const_reference back()  const __ntl_nothrow { return *(end() - 1); }

    ///@}

  private:

    iterator insert__blank_space(const iterator position, const size_type n)
    {
      const size_type old_capacity = capacity_;
      iterator old_mem = 0;
      iterator new_end = end_ + n;
      // a hint to compiler - may be it'll eliminate  while ( tail_size-- ) ...
      difference_type tail_size = end_ - position;
      // realloc the first part if needed
      if ( capacity_ < end_- begin_ + n )
      {
        old_mem = begin_;
        capacity_ = n + capacity_factor();
        const iterator new_mem = alloc_.allocate(capacity_);
        new_end = new_mem + difference_type(new_end - old_mem);
        //new_end += difference_type(new_mem - old_mem);        // dangerous alignment
        iterator dest = begin_ = new_mem;
        // this is safe for begin_ == 0 && end_ == 0, but keep vector() intact
        for ( iterator src = old_mem; src != position; ++src, ++dest )
          move(dest, src);
      }
      // move the tail. iterators are reverse - may be no realloc
      iterator r_src = end();
      iterator r_dest = end_ = new_end;
      while ( tail_size-- )
        move(--r_dest, --r_src);
      if ( old_mem ) alloc_.deallocate(old_mem, old_capacity);
      return r_dest;
    }

    iterator insert__impl(iterator position, size_type n, const T& x)
    {
      iterator r_dest = insert__blank_space(position, n);
      while ( n-- ) alloc_.construct(--r_dest, x);
      return r_dest;
    }

    template <class InputIterator>
    void insert__disp_it(iterator position, InputIterator first,
                         InputIterator last, const input_iterator_tag&)
    {
      while ( first != last ) position = insert__impl(position, 1, *first++) + 1;
    }

    template <class InputIterator>
   void insert__disp_it(iterator position, InputIterator first,
                         InputIterator last, const random_access_iterator_tag &)
    {
      position = insert__blank_space(position, static_cast<size_type>(last - first));
      while ( first != last ) alloc_.construct(--position, *--last);
    }

    template <class InputIterator>
    void insert__disp(iterator position, InputIterator first, InputIterator last,
                      const false_type&)
    {
      insert__disp_it(position, first, last,
                      iterator_traits<InputIterator>::iterator_category());
    }

    template <class IntegralType>
    void insert__disp(iterator position, IntegralType n, IntegralType x,
                      const true_type&)
    {
      insert__impl(position, static_cast<size_type>(n),
                  static_cast<value_type>(x));
    }

  public:

    ///\name  modifiers [23.2.6.3]

    __forceinline
    void push_back(const T& x)
    {
      if ( size() == capacity() ) realloc(capacity_factor());
      alloc_.construct(end_++, x);
    }

    void pop_back() __ntl_nothrow { alloc_.destroy(--end_); }

    iterator insert(iterator position, const T& x)
    {
      return insert__impl(position, 1, x);
    }

    void insert(iterator position, size_type n, const T& x)
    {
      insert__impl(position, n, x);
    }

    template <class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last)
    {
      insert__disp(position, first, last, is_integral<InputIterator>::type());
    }

    __forceinline
    iterator erase(iterator position) __ntl_nothrow
    {
      // return erase(position, position + 1);
      alloc_.destroy(position);
      --end_;
      iterator i = position;
      do move(i, i + 1); while ( ++i != end_ );
      return position;
    }

    __forceinline
    iterator erase(iterator first, iterator last) __ntl_nothrow
    {
      for ( iterator i = last; i != first;  ) alloc_.destroy(--i);
      iterator const tail = first;
      for ( ; last != end_; ++first, ++last ) move(first, last);
      end_ = first;
      return tail;
    }

    void swap(vector<T, Allocator>& x) __ntl_nothrow
    {
      std::swap(begin_, x.begin_);
      std::swap(end_, x.end_);
      std::swap(capacity_, x.capacity_);
    }

    __forceinline
    void clear() __ntl_nothrow
    {
      difference_type n = end_ - begin_;
      end_ = begin_;
      while ( n ) alloc_.destroy(begin_ + --n);
    }

    ///@}

  ///////////////////////////////////////////////////////////////////////////
  private:

  #if 0
  // non-standard extension
  friend
    const vector<T, Allocator>
      make_vector(pointer first, pointer last, const Allocator& a = Allocator())
    {
      return const vector(first, last, true, a);
    }

  friend
    const vector<T, Allocator>
      make_vector(const_pointer first, const_pointer last,
                  const Allocator& a = Allocator())
    {
      return const vector(first, last, true, a);
    }

    vector(pointer first, size_type n, bool, const Allocator& a = Allocator())
    : alloc_(a), begin_(first), end_(last), capacity(last - first) {}
  //end extension
  #endif

    mutable allocator_type  alloc_;

    size_type       capacity_;
    pointer         begin_;
    pointer         end_;

    // "stdexcept.hxx" includes this header
    // hack: MSVC doesn't look inside function body
    void check_bounds(size_type n) const //__ntl_throws(out_of_range)
    {
      if ( n > size() ) __ntl_throw (out_of_range(__FUNCTION__));
    }

    void move(const iterator  to, const iterator from) const
    {
      alloc_.construct(to, *from);
      alloc_.destroy(from);
    }


    void realloc(size_type n) __ntl_throws(bad_alloc)
    {
      const iterator new_mem = alloc_.allocate(n);
      const size_type old_capacity = capacity_;
      capacity_ = n;
      iterator dest = new_mem;
      // this is safe for begin_ == 0 && end_ == 0, but keep vector() coherent
      for ( iterator src = begin_; src != end_; ++src, ++dest )
        move(dest, src);
      if ( begin_ ) alloc_.deallocate(begin_, old_capacity);
      begin_ = new_mem;
      end_ = dest;
    }

    //  + 8/2 serves two purposes:
    //    capacity_ may be 0;
    //    for smal capacity_ values reallocation will be more efficient
    //      2,  4,  8,  16,  32,  64, 128, 256, 512
    //      8, 24, 56, 120, 248, 504,
    size_type capacity_factor() const { return (capacity_ + 4) * 2; }

};//class vector

///\name  Vector comparisons

template <class T, class Allocator>
inline
bool operator==(const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  __ntl_nothrow
{
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T, class Allocator>
inline
bool operator< (const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  __ntl_nothrow
{
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <class T, class Allocator>
inline
bool operator!=(const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  __ntl_nothrow
{
  return ! (x == y);
}

template <class T, class Allocator>
inline
bool operator> (const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  __ntl_nothrow
{
  return y < x;
}

template <class T, class Allocator>
inline
bool operator>=(const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  __ntl_nothrow
{
  return ! (x < y);
}

template <class T, class Allocator>
inline
bool operator<=(const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  __ntl_nothrow
{
  return ! (y < x);
}

///\name  Vector specialized algorithms
template <class T, class Allocator>
inline
void swap(vector<T, Allocator>& x, vector<T, Allocator>& y) __ntl_nothrow
{
  x.swap(y);
}

///@}
/**@} lib_sequence */
/**@} lib_containers */

}//namespace std

#ifdef _MSC_VER
# pragma warning(pop)//disable:4820
#endif

#endif//#ifndef NTL__STLX_VECTOR
