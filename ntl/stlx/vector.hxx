/**\file*********************************************************************
 *                                                                     \brief
 *  Class template vector [23.2.6 lib.vector]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_VECTOR
#define NTL__STLX_VECTOR

#include "cstring.hxx"
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

// There should be a better place for names in this (detail) namespace.
// !fr3@K!


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
    committed_(false)
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
    if(committed_)
      return;

    destroy(first_, current_, array_allocator_);
  }

  void operator()(const value_type& value)
  {
    array_allocator_.construct(array_allocator_.address(*current_), value);
    ++current_;
  }

  iterator commit() __ntl_nothrow
  {
    committed_ = true;
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
  bool committed_;
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
  ctor.commit();
  assert(first == ctor.commit());
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
  ctor.commit();
  assert(first == ctor.commit());
}

template <class InputIterator, class ForwardIterator, class Allocator>
InputIterator uninitailized_copy_a(InputIterator first,
                                   InputIterator last,
                                   ForwardIterator dst,
                                   Allocator& alloc)
{
  guarded_range_constructor<ForwardIterator, Allocator> ctor(dst, alloc);
  for(; first != last; ++first, ++dst)
  {
    ctor(*first);
  }
  return ctor.commit();
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
  for(; n; ++first, ++dst)
  {
    ctor(*first);
  }
  return ctor.commit();
}

// Is Power Of 2, optionally used in
// vector_allocation_policy().
// Ref: http://www.aggregate.org/MAGIC/#Is%20Power%20of%202
inline bool ispo2(uint64_t n) __ntl_nothrow
{
  return ((n&(n-1)) == 0) ? true : false;
}

// Next Largest Power of 2, optionally used in
// vector_allocation_policy().
// Ref: http://www.aggregate.org/MAGIC/#Next%20Largest%20Power%20of%202
uint64_t nlpo2(uint64_t n) __ntl_nothrow
{
  n |= (n >> 1);
  n |= (n >> 2);
  n |= (n >> 4);
  n |= (n >> 8);
  n |= (n >> 16);
  n |= (n >> 32);
  return(++n);
}

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
# define NTL__VECTOR_ALLOCATION_POLICY__AGGRESSIVE
#endif

// Interface to vector allocation policy.
size_t vector_allocation_policy(size_t min)
{
#if defined(NTL__VECTOR_ALLOCATION_POLICY__USER_DEFINED)
  // user with user-defined policy must declare their policy as:
  return ::ntl_user_defined::vector_allocation_policy(min);

#elif defined(NTL__VECTOR_ALLOCATION_POLICY__AGGRESSIVE)
  // returns 0, if min is 0
  // returns 4, if min is 1, 2, 3, 4
  // returns min, if min is power of 2
  // returns 8, if min is 5, 6, 7, 8
  // returns 16, if min is 9, 10, 11..., 16
  // and so on...
  if(min <= 4)
    return min;
  return ispo2(min) ? min : size_t(nlpo2(min));

#else // NTL__VECTOR_ALLOCATION_POLICY__CONSERVATIVE
  return min;
#endif
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
    committed_(false)
  {
  }
  ~guarded_allocation() __ntl_nothrow
  {
    if(committed_ == true)
      return;

    assert(p_ != nullptr);
    array_allocator_.deallocate(p_, n_);
  }

  pointer get() const __ntl_nothrow
  {
    assert(p_ != nullptr);
    return p_;
  }

  void commit() __ntl_nothrow
  {
    committed_ = true;
  }

  private:
    allocator_type& array_allocator_;
    size_t n_;
    pointer p_;
    bool committed_;
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
    committed_(false)
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
    if(committed_)
      return;

    destroy(dst_, dst_last_, array_allocator_);
  }

  void commit() __ntl_nothrow
  {
    committed_ = true;
  }

private:
  ForwardIterator dst_;
  ForwardIterator dst_last_;
  Allocator& array_allocator_;
  bool committed_;
};

template <class T>
bool overlapping(const T* first, const T* last, const T* p) __ntl_nothrow
{
  return (p >= first && p < last) ? true : false;
}

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
    typedef detail::guarded_allocation<allocator_type> guarded_allocation;
    typedef detail::guarded_relocation<iterator, iterator, allocator_type> guarded_relocation;

  public:

    ///\name construct/copy/destroy [23.2.6.1]

    explicit vector(const Allocator& a = Allocator())
    : array_allocator_(a), capacity_(0), begin_(nullptr), end_(nullptr) {}

    explicit vector(size_type n,
                    const T& value      = T(),
                    const Allocator& a  = Allocator())
    : array_allocator_(a),
      capacity_(detail::vector_allocation_policy(n)),
      begin_(array_allocator_.allocate(capacity_)),
      end_(begin_)
    {
      detail::uninitailized_fill_n_a(begin_, n, value, array_allocator_);
      end_ += n;
    }

    template <class InputIterator>
    vector(InputIterator first,
           InputIterator last,
           const Allocator& a = Allocator())
    : array_allocator_(a),
      begin_(nullptr),
      end_(nullptr),
      capacity_(0)
    {
      // It's typical to implement assignment via a tmp obj and swap.
      // But in this case it's much easier to implement a
      // exception-safe (basic guaranty) version of range assign for
      // InputIterator.
      // Hint, with help from dtor of tmp obj.
      // Therefore I choose to implement this constructor as such.
      // !fr3@K!
      vector tmp(array_allocator_);
      tmp.assign(first, last);
      this->swap(other);
    }

    __forceinline
    vector(const vector<T, Allocator>& x)
    : array_allocator_(x.array_allocator_),
      capacity_(detail::vector_allocation_policy(x.size())),
      begin_(nullptr),
      end_(nullptr)
    {
      if ( !capacity_ )
        return;

      guarded_allocation new_begin(array_allocator_, capacity_);
      detail::uninitailized_copy_a(x.begin(), x.end(), new_begin.get(), array_allocator_);

      // no-throw begin
      new_begin.commit();
      begin_ = end_ = new_begin.get();
      end_ += x.size();
    }

    __forceinline
    ~vector() __ntl_nothrow
    {
      if(capacity_ == 0)
        return;

      assert(begin_ != nullptr);
      clear();
      array_allocator_.deallocate(begin_, capacity_);
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
        is_integral<InputIterator>());
    }

    void assign(size_type n, const T& u)
    {
      if(detail::overlapping(begin_, end_, &u) == false)
      {
        assign__n(n, u);
        return;
      }

      // Make a copy of `u`, before clear()ing.
      // Checking for self referencing.
      // !fr3@K!
      const T tmp(u);
      assign__n(n, tmp);
    }

    allocator_type get_allocator() const  { return array_allocator_; }

    ///@}

  private:

    template <class InputIterator>
    void assign__disp(InputIterator first, InputIterator last,
                      const false_type& /*is_integral*/)
    {
      this->assign__range(
        first,
        last,
        typename iterator_traits<InputIterator>::iterator_category());
    }
    template <class InputIterator>
    void assign__disp(InputIterator first, InputIterator last,
                      const true_type& /*is_integral*/)
    {
      this->assign(size_type(first), value_type(last));
    }

    template <class InputIterator>
    void assign__range(InputIterator first, InputIterator last,
                      const input_iterator_tag&)
    {
      clear();
      for(; first != last; ++first)
        push_back(*first);
    }

    template <class ForwardIterator>
    void assign__range(ForwardIterator first, ForwardIterator last,
                      const forward_iterator_tag&)
    {
      const size_type n = distance(first, last);
      const size_type old_size = distance(begin_, end_);
      if(capacity_ >= n)
      {
        // No reallocation needed.
        if(old_size >= n)
        {
          // `this ` has at least `n` managed elements.
          pointer new_end = copy(first, last, begin_);

          { // No-throw code block
            detail::destroy(new_end, end_, array_allocator_);
            end_ = new_end;
          }
          return;
        }

        // `this` has less than `n` managed elements.
        ForwardIterator anchor = first;
        advance(anchor, old_size);
        copy(first, anchor, begin_);
        for(size_type idx = old_size; idx != n; ++idx, ++anchor)
        {
          array_allocator_.construct(&begin_[idx], *anchor);
          ++end_;
        }
        return;
      }

      // Reallocation required.
      const size_type new_cap = detail::vector_allocation_policy(n);
      pointer new_begin = array_allocator_.allocate(new_cap);

      if(old_size >= n)
      {
        // Has at least `n` managed elements.
        { // No-throw code block
          detail::relocate(begin_, begin_ + n, new_begin);
          detail::destroy(begin_ + n, end_, array_allocator_);
          begin_ = end_ = new_begin;
          end_ += n;
          capacity_ = new_cap;
        }
        copy_n(first, old_size, begin_);
        return;
      }

      // Has less than `n` managed elements.
      { // No-throw code block
        detail::relocate(begin_, end_, new_begin);
        begin_ = end_ = new_begin;
        end_ += old_size;
        capacity_ = new_cap;
      }
      ForwardIterator anchor(first);
      advance(anchor, old_size);
      copy(first, anchor, begin_);
      for(size_type idx = old_size; idx != n; ++idx, ++anchor)
      {
        array_allocator_.construct(&begin_[idx], *anchor);
        ++end_;
      }
    }

    void assign__n(const size_type n, const T& u)
    {
      const size_type old_size = distance(begin_, end_);
      if(capacity_ >= n)
      {
        // No reallocation needed.
        if(old_size >= n)
        {
          // `this ` has at least `n` managed elements.
          pointer new_end = begin_ + n;
          fill(begin_, new_end, u);

          { // No-throw code block
            detail::destroy(new_end, end_, array_allocator_);
            end_ = new_end;
          }
          return;
        }

        // `this` has less than `n` managed elements.
        fill_n(begin_, old_size, u);
        for(size_type idx = old_size; idx != n; ++idx)
        {
          array_allocator_.construct(&begin_[idx], u);
          ++end_;
        }
        return;
      }

      // Reallocation required.
      const size_type new_cap = detail::vector_allocation_policy(n);
      pointer new_begin = array_allocator_.allocate(new_cap);

      if(old_size >= n)
      {
        // Has at least `n` managed elements.
        { // No-throw code block
          detail::relocate(begin_, begin_ + n, new_begin);
          detail::destroy(begin_ + n, end_, array_allocator_);
          begin_ = end_ = new_begin;
          end_ += n;
          capacity_ = new_cap;
        }
        fill_n(begin_, old_size, u);
        return;
      }

      // Has less than `n` managed elements.
      { // No-throw code block
        detail::relocate(begin_, end_, new_begin);
        begin_ = end_ = new_begin;
        end_ += old_size;
        capacity_ = new_cap;
      }
      fill_n(begin_, old_size, u);
      for(size_type idx = old_size; idx != n; ++idx)
      {
        array_allocator_.construct(&begin_[idx], u);
        ++end_;
      }
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
    size_type max_size()  const { return array_allocator_.max_size(); }
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
      if (capacity_ >= n)
        return;

      size_type new_cap = detail::vector_allocation_policy(n);
      pointer new_begin = array_allocator_.allocate(new_cap);

      // no-throw begin
      detail::relocate(begin_, end_, new_begin);
      array_allocator_.deallocate(begin_, capacity_);
      end_ = new_begin + distance(begin_, end_);
      begin_ = new_begin;
      capacity_ = new_cap;
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

    pointer insert__n(pointer position, size_type n, const T& x)
    {
      if(capacity_ < this->size() + n)
      {
        if(detail::overlapping(begin_, end_, &x) == true)
        {
          // Self referencing protection.
          // `x` is going to be relocate()ed, make a copy of it before
          // it's relocated.
          const T tmp(x);
          return this->insert__n_realloc(position, n, tmp);
        }
        return this->insert__n_realloc(position, n, x);
      }

      if(detail::overlapping(position, position + n, &x) == true)
      {
        // Self referencing protection.
        // `x` is going to be relocate()ed, make a copy of it before
        // it's relocated.
        const T tmp(x);
        return this->insert__n_no_realloc(position, n, tmp);
      }

      return this->insert__n_no_realloc(position, n, x);
    }

    pointer insert__n_realloc(pointer position, size_type n, const T& x)
    {
      assert(capacity_ < this->size() + n);

      const size_type old_size = this->size();
      const size_type new_size = old_size + n;
      const size_type new_cap = detail::vector_allocation_policy(old_size + n);
      guarded_allocation new_begin(array_allocator_, new_cap);
      pointer new_pos = new_begin.get() + distance(begin_, position);
      detail::uninitailized_fill_n_a(new_pos, n, x, array_allocator_);

      // no-throw begin
      new_begin.commit();
      detail::relocate(begin_, position, new_begin.get());
      detail::relocate(position, end_, new_pos + n);
      array_allocator_.deallocate(begin_, capacity_);
      begin_ = end_ = new_begin.get();
      end_ += (old_size + n);
      capacity_ = new_cap;
      return new_pos;
    }

    pointer insert__n_no_realloc(pointer position, size_type n, const T& x)
    {
      assert((capacity_ < this->size() + n) == false);

      const size_type old_size = this->size();
      const size_type new_size = old_size + n;
      if(position + n < end_)
      {
        // Relocating in overlapping ranges.
        // Relocates reversely to avoid race condition.
        detail::guarded_relocation<reverse_iterator, reverse_iterator, allocator_type> reloc(
          reverse_iterator(end_),
          reverse_iterator(position),
          reverse_iterator(position + n),
          array_allocator_);
        end_ = begin_ + distance(begin_, position);
        detail::uninitailized_fill_n_a(position, n, x, array_allocator_);

        // no-throw begin
        reloc.commit();
        end_ += n;
        return position;
      }

      // Relocating in non-overlapping ranges.
      // Relocates normally.
      guarded_relocation reloc(
        position,
        end_,
        position + n,
        array_allocator_);

      end_ = begin_ + distance(begin_, position);
      detail::uninitailized_fill_n_a(position, n, x, array_allocator_);

      // no-throw begin
      reloc.commit();
      end_ += n;
      return position;
    }

    template <class InputIterator>
    void insert__dispatch(const_iterator position,
                          InputIterator first,
                          InputIterator last,
                          const false_type& /*is_integral*/)
    {
      pointer p = const_cast<pointer>(&(*position));
      this->insert__range(
        p,
        first,
        last,
        typename iterator_traits<InputIterator>::iterator_category());
    }
    template <class InputIterator>
    void insert__dispatch(const_iterator position,
                          InputIterator first,
                          InputIterator last,
                          const true_type& /*is_integral*/)
    {
      this->insert(
        position,
        size_type(first),
        value_type(last));
    }

    template <class InputIterator>
    void insert__range(pointer position,
                       InputIterator first,
                       InputIterator last,
                       const input_iterator_tag&)
    {
      size_type offset = distance(begin_, position);
      for(; first != last; ++first, ++offset)
      {
        this->insert__n(begin_ + offset, 1, *first);
      }
    }

    template <class ForwardIterator>
    void insert__range(pointer position,
                       ForwardIterator first,
                       ForwardIterator last,
                       const forward_iterator_tag&)
    {
      const size_type n = distance(first, last);
      const size_type old_size = this->size();
      const size_type new_size = old_size + n;
      if(capacity_ >= new_size)
      {
        // No reallocation needed.

        // Though I don't know where it states this in the standard,
        // STLport says the standard forbids checking for self
        // referencing here.
        // !fr3@K!

        if(position + n < end_)
        {
          // Relocating in overlapping ranges.
          // Relocates reversely to avoid race condition.
          detail::guarded_relocation<reverse_iterator, reverse_iterator, allocator_type> reloc(
            reverse_iterator(end_),
            reverse_iterator(position),
            reverse_iterator(position + n),
            array_allocator_);
          end_ = begin_ + distance(begin_, position);
          detail::uninitailized_copy_a(first, last, position, array_allocator_);

          // no-throw begin
          reloc.commit();
          end_ += n;
          return;
        }

        // Relocating in non-overlapping ranges.
        // Relocates normally.
        guarded_relocation reloc(
          position,
          end_,
          position + n,
          array_allocator_);

        end_ = begin_ + distance(begin_, position);
        detail::uninitailized_copy_a(first, last, position, array_allocator_);

        // no-throw begin
        reloc.commit();
        end_ += n;
        return;
      }

      // Reallocation required.
      const size_type new_cap = detail::vector_allocation_policy(old_size + n);
      guarded_allocation new_begin(array_allocator_, new_cap);
      pointer new_pos = new_begin.get() + distance(begin_, position);
      detail::uninitailized_copy_a(first, last, new_pos, array_allocator_);

      // no-throw begin
      new_begin.commit();
      detail::relocate(begin_, position, new_begin.get());
      detail::relocate(position, end_, new_pos + n);
      array_allocator_.deallocate(begin_, capacity_);
      begin_ = end_ = new_begin.get();
      end_ += (old_size + n);
      capacity_ = new_cap;
      return;
    }

    void push_back__impl(const T& x)
    {
      reserve(size() + 1);
      array_allocator_.construct(end_, x);
      ++end_;
    }

    pointer erase__impl(pointer first, pointer last) __ntl_nothrow
    {
      detail::destroy(first, last, array_allocator_);
      detail::relocate(last, end_, first);
      end_ -= distance(first, last);
      return first;
    }

  public:

    ///\name  modifiers [23.2.6.3]

    __forceinline
    void push_back(const T& x)
    {
      if(capacity_ == size() && detail::overlapping(begin_, end_, &x))
      {
        // Relocation required.
        value_type tmp(x);
        this->push_back__impl(tmp);
      }

      this->push_back__impl(x);
    }

    void pop_back() __ntl_nothrow { array_allocator_.destroy(--end_); }

    iterator insert(const_iterator position, const T& x)
    {
      pointer p = const_cast<pointer>(&(*position));
      return insert__n(p, 1, x);
    }

    void insert(const_iterator position, size_type n, const T& x)
    {
      pointer p = const_cast<pointer>(&(*position));
      insert__n(p, n, x);
    }

    template <class InputIterator>
    void insert(const_iterator position, InputIterator first, InputIterator last)
    {
      this->insert__dispatch(
        position,
        first,
        last,
        is_integral<InputIterator>());
    }

    __forceinline
    iterator erase(const_iterator position) __ntl_nothrow
    {
      pointer p = const_cast<pointer>(position);
      return this->erase__impl(position, position + 1);
    }

    __forceinline
    iterator erase(const_iterator first, const_iterator last) __ntl_nothrow
    {
      pointer f = const_cast<pointer>(first);
      pointer l = const_cast<pointer>(last);
      return this->erase__impl(f, l);
    }

    void swap(vector<T, Allocator>& x) __ntl_nothrow
    {
      std::swap(array_allocator_, x.array_allocator_);
      std::swap(capacity_, x.capacity_);
      std::swap(begin_, x.begin_);
      std::swap(end_, x.end_);
    }

    __forceinline
    void clear() __ntl_nothrow
    {
      detail::destroy(begin_, end_, array_allocator_);
      end_ = begin_;
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
    : array_allocator_(a), begin_(first), end_(last), capacity(last - first) {}
  //end extension
  #endif

    // Why mutable?
    // !fr3@K!
    mutable allocator_type  array_allocator_;

    // 0 ~ 3: managed elements
    //   x  : allocated but unused memory blocks
    // + - + - + - + - + - + - + - + - + . .
    // | 0 | 1 | 2 | 3 | x | x | x | x |   :
    // + - + - + - + - + - + - + - + - + . .
    //   ^               ^               ^
    //   |               |               |
    // begin_           end_      begin_ + capacity_

    // Size, in elements, of the memory buffer `begin_` points to.
    size_type       capacity_;
    // Pointer to the begining of memory buffer.
    pointer         begin_;
    // Pointer to one pass the last managed elements in buffer.
    pointer         end_;

    // "stdexcept.hxx" includes this header
    // hack: MSVC doesn't look inside function body
    void check_bounds(size_type n) const //__ntl_throws(out_of_range)
    {
      if ( n > size() ) __ntl_throw (out_of_range(__FUNCTION__));
    }

    void move(const iterator  to, const iterator from) const
    {
      array_allocator_.construct(to, *from);
      array_allocator_.destroy(from);
    }
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
