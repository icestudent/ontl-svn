/**\file*********************************************************************
 *                                                                     \brief
 *  30.5 Futures [futures]
 *
 ****************************************************************************
 */

#ifndef STLX_FUTURE
#define STLX_FUTURE

#include "system_error.hxx"
#include "chrono.hxx"

namespace stlx
{
  /**
   *	@addtogroup thread Thread support library [thread]
   *  @{
   **/
#ifdef NTL__CXX_ENUM
  enum class future_errc {
    broken_promise,
    future_already_retrieved,
    promise_already_satisfied
  };
#else
  __class_enum(future_errc)
  {
    broken_promise,
    future_already_retrieved,
    promise_already_satisfied
  }};
#endif

  template <class R> class unique_future;
  template <class R> class unique_future<R&>;
  template <> class unique_future<void>;
  
  template <class R> class shared_future;
  template <class R> class shared_future<R&>;
  template <> class shared_future<void>;
  
  template <class R> class promise;
  template <class R> class promise<R&;
  template <> class promise<void>;
  
  template <class> class packaged_task; // undefined


  template <class R, class Alloc>
  struct uses_allocator<promise<R>, Alloc>: true_type {};
  
  template <class R>
  struct constructible_with_allocator_prefix<promise<R> >: true_type {};

  template <> struct is_error_code_enum<future_errc>: true_type { };


  class future_error: 
    public logic_error
  {
  public:
    future_error(error_code ec);

    const error_code& code() const __ntl_nothrow;
    const char* what*() const __ntl_nothrow;
  };

  constexpr error_code make_error_code(future_errc e);
  constexpr error_condition make_error_condition(future_errc e);

  extern const error_category* const future_category;
  
//////////////////////////////////////////////////////////////////////////

  template <class R>
  class unique_future 
  {
  public:
    unique_future(unique_future &&);
    unique_future(const unique_future& rhs) = delete;
    ~unique_future();
    unique_future& operator=(const unique_future& rhs) = delete;

    // retrieving the value
    see below get();
    
    // functions to check state and wait for ready
    bool is_ready() const;
    bool has_exception() const;
    bool has_value() const;
    
    void wait() const;
    template <class Rep, class Period>>
    bool wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    template <class Clock, class Duration>
    bool wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
  };

  template <class R>
  class shared_future
  {
  public:
    shared_future(const shared_future& rhs);
    shared_future(unique_future<R>);
    ~shared_future();
    shared_future & operator=(const shared_future& rhs) = delete;

    // retrieving the value
    see below get() const;
    
    // functions to check state and wait for ready
    bool is_ready() const;
    bool has_exception() const;
    bool has_value() const;
    
    void wait() const;
    template <class Rep, class Period>>
    bool wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    template <class Clock, class Duration>
    bool wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
  };

  template <class R>
  class promise
  {
  public:
    promise();
    template <class Allocator>
    promise(allocator_arg_t, const Allocator& a);
    promise(promise&& rhs);
    template <class Allocator>
    promise(allocator_arg_t, const Allocator& a, promise& rhs);
    promise(const promise& rhs) = delete;
    ~promise();

    // assignment
    promise & operator=(promise&& rhs);
    promise & operator=(const promise& rhs) = delete;
    void swap(promise& other);
    
    // retrieving the result
    unique_future<R> get_future();
    
    // setting the result
    void set_value(const R& r);
    void set_value(see below);
    void set_exception(exception_ptr p);
  };

#ifdef NTL__CXX_VT
  template<class R, class... ArgTypes>
  class packaged_task<R(ArgTypes...)> {
  public:
    typedef R result_type;
    // construction and destruction
    packaged_task();
    template <class F>
    explicit packaged_task(F f);
    template <class F, class Allocator>
    explicit packaged_task(allocator_arg_t, const Allocator& a, F f);
    explicit packaged_task(R(*f)());
    template <class F>
    explicit packaged_task(F&& f);
    template <class F, class Allocator>
    explicit packaged_task(allocator_arg_t, const Allocator& a, F&& f);
    ~packaged_task();
    // no copy
    packaged_task(packaged_task&) = delete;
    packaged_task& operator=(packaged_task&) = delete;
    // move support
    packaged_task(packaged_task&& other);
    packaged_task& operator=(packaged_task&& other);
    void swap(packaged_task&& other);
    explicit operator bool() const;
    // result retrieval
    unique_future<R> get_future();
    // execution
    void operator()(ArgTypes... );
    void reset();
  };
#else
  template<class R, class Arg1>
  class packaged_task<R(Arg1)> {
  public:
    typedef R result_type;

    // construction and destruction
    packaged_task();
    template <class F>
    explicit packaged_task(F f);
    template <class F, class Allocator>
    explicit packaged_task(allocator_arg_t, const Allocator& a, F f);
    explicit packaged_task(R(*f)());
    template <class F>
    explicit packaged_task(F&& f);
    template <class F, class Allocator>
    explicit packaged_task(allocator_arg_t, const Allocator& a, F&& f);
    ~packaged_task();

    // no copy
    packaged_task(packaged_task&) = delete;
    packaged_task& operator=(packaged_task&) = delete;
    
    // move support
    packaged_task(packaged_task&& other);
    packaged_task& operator=(packaged_task&& other);
    
    void swap(packaged_task&& other);
    
    explicit operator bool() const;
    
    // result retrieval
    unique_future<R> get_future();
    
    // execution
    void operator()(Arg1 arg1);
    void reset();
  };
#endif

  
  /** @} thread */
} // stlx

#endif // STLX_FUTURE
