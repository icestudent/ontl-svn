/**\file*********************************************************************
 *                                                                     \brief
 *  20.6 Function objects [function.objects]
 *  Implementation for legacy compilers
 ****************************************************************************
 */
#ifndef STLX_FUNCTION
#define STLX_FUNCTION

#include "exception.hxx"
#include "ext/pp/macro_params.hxx"
#include "ext/ttl/typelist.hxx"

#define TTL_MAX_FUNCTION_PARAMS 15

namespace stlx
{
  // 20.6.15 polymorphic function wrappers:
  class bad_function_call:
    public exception
  {
  public:
    bad_function_call() __ntl_nothrow
    {}
    const char* what() const __ntl_nothrow { return "bad_function_call"; }
  };

#if 0
  template<class> class function;

  template<class R, class... ArgTypes> class function<R(ArgTypes...)>;
  
  template<class R, class... ArgTypes>
  void swap(function<R(ArgTypes...)>&, function<R(ArgTypes...)>&);
  template<class R, class... ArgTypes>
  
  void swap(function<R(ArgTypes...)>&&, function<R(ArgTypes...)>&);
  template<class R, class... ArgTypes>
  
  void swap(function<R(ArgTypes...)>&, function<R(ArgTypes...)&&);
  
  template<class R, class... ArgTypes>
  bool operator==(const function<R(ArgTypes...)>&, unspecified-null-pointer-type);
  
  template<class R, class... ArgTypes>
  bool operator==(unspecified-null-pointer-type , const function<R(ArgTypes...)>&);
  
  template<class R, class... ArgTypes>
  bool operator!=(const function<R(ArgTypes...)>&, unspecified-null-pointer-type);
  
  template<class R, class... ArgTypes>
  bool operator!=(unspecified-null-pointer-type , const function<R(ArgTypes...)>&);

  template<class R, class... ArgTypes>
  class function<R(ArgTypes...)>
    : public unary_function<T1, R> // if sizeof...(ArgTypes) == 1 and ArgTypes contains T1
    : public binary_function<T1, T2, R> // if sizeof...(ArgTypes) == 2 and ArgTypes contains T1 and
    T2
  {
  public:
    typedef R result_type;

    // 20.6.15.2.1, construct/copy/destroy:
    explicit function();
    function(unspecified-null-pointer-type );
    function(const function&);
    function(function&&);
    
    template<class F> function(F);
    template<class F> function(F&&);
    
    template<class A> function(allocator_arg_t, const A&);
    template<class A> function(allocator_arg_t, const A&, unspecified-null-pointer-type );
    template<class A> function(allocator_arg_t, const A&, const function&);
    template<class A> function(allocator_arg_t, const A&, function&&);

    template<class F, class A> function(allocator_arg_t, const A&, F);
    template<class F, class A> function(allocator_arg_t, const A&, F&&);
    
    function& operator=(const function&);
    function& operator=(function&&);
    function& operator=(unspecified-null-pointer-type );
    
    template<class F> function& operator=(F);
    template<class F> function& operator=(F&&);
    template<class F> function& operator=(reference_wrapper<F>);
    
    ~function();
    
    void swap(function&&);
    
    template<class F, class A> void assign(F, const A&);

    // 20.6.15.2.3, function capacity:
    explicit operator bool() const;
    
    // deleted overloads close possible hole in the type system
    template<class R2, class... ArgTypes2>
    bool operator==(const function<R2(ArgTypes2...)>&) = delete;
    template<class R2, class... ArgTypes2>
    bool operator!=(const function<R2(ArgTypes2...)>&) = delete;
    
    // 20.6.15.2.4, function invocation:
    R operator()(ArgTypes...) const;
    
    // 20.6.15.2.5, function target access:
    const stlx::type_info& target_type() const;
    
    template <typename T> T* target();
    template <typename T> const T* target() const;
  };
#endif

  namespace __ 
  {
    namespace func 
    {
      using namespace ttl;
      using ttl::meta::empty_type;


      template<typename T>
      struct memfunobj: 
        stlx::is_member_function_pointer<T>
      {};

      template<typename RT, class T>
      struct memfunobj<RT (T::*)>
      {
        typedef RT return_type;
        typedef T  object_type;
      };

      //////////////////////////////////////////////////////////////////////////
      template<typename R>
      struct functor_caller_base0
      {
        typedef functor_caller_base0 this_t;
        typedef R return_type;
        virtual return_type operator()() = 0;
        virtual ~functor_caller_base0() {}
        virtual this_t* clone() const = 0;
      };

      template<typename F, typename R>
      struct functor_caller0 : functor_caller_base0<R>
      {
        typedef functor_caller0 this_t;
        typedef functor_caller_base0<R> base_t;
        typedef R return_type;
        F f_;
        
        functor_caller0(F f) : f_(f) {}
        virtual return_type operator()() { return f_(); }
        virtual base_t* clone() const { return new this_t(f_); } 
      };

      //////////////////////////////////////////////////////////////////////////

#define TTL_FUNC_BUILD_FUNCTOR_CALLER(n)	 \
  template<typename R, TTL_TPARAMS(n)> \
      struct functor_caller_base##n \
      { \
      typedef functor_caller_base##n this_t; \
      typedef R return_type; \
      virtual ~functor_caller_base##n() {} \
      virtual return_type operator()( TTL_FUNC_PARAMS(n,p) ) = 0; \
      virtual this_t* clone() const = 0;  \
      }; \
      template<typename F=ttl::meta::empty_type, typename R=ttl::meta::empty_type, TTL_TPARAMS_DEF(n,ttl::meta::empty_type)> struct functor_caller##n;  \
      template<typename F, typename R, TTL_TPARAMS(n)> \
      struct functor_caller##n<F, R (TTL_ARGS(n))> : functor_caller_base##n<R, TTL_ARGS(n)> \
      { \
      typedef functor_caller##n this_t; \
      typedef functor_caller_base##n<R, TTL_ARGS(n)> base_t; \
      typedef R return_type; \
      F f_; \
      functor_caller##n(F f) : f_(f) {} \
      virtual return_type operator()(TTL_FUNC_PARAMS(n,p)) { return f_(TTL_ENUM_ITEMS(n,p)); } \
      virtual base_t* clone() const { return new this_t(f_); }  \
      }; 

#if 0
      TTL_FUNC_BUILD_FUNCTOR_CALLER(1)
#else
      template<typename R, typename T1>
      struct functor_caller_base1
      {
      typedef functor_caller_base1 this_t;
      typedef R return_type;
      virtual ~functor_caller_base1() {}
      virtual return_type operator()(T1 p1) = 0;
      virtual this_t* clone() const = 0; 
      };

      template<typename F = ttl::meta::empty_type, typename R = ttl::meta::empty_type, typename T1 = ttl::meta::empty_type>
      struct functor_caller1;

      template<typename F, typename R, typename T1>
      struct functor_caller1<F, R (T1)>:
        functor_caller_base1<R, T1>
      {
      typedef functor_caller1 this_t;
      typedef functor_caller_base1<R, T1> base_t;
      typedef R return_type;
      F f_;
      functor_caller1(F f) : f_(f) {}
      virtual return_type operator()(T1 p1) 
      {
        return f_(p1); 
      }
      virtual base_t* clone() const { return new this_t(f_); } 
      }; 
#endif

      template<typename R, typename MemFn, class Obj>
      struct memfun_caller0
        :functor_caller_base1<R, Obj>
      {
        typedef memfun_caller0<R, MemFn, Obj> this_t;
        typedef functor_caller_base1<R, Obj> base_t;
        typedef R return_type;

        MemFn f_;

        explicit memfun_caller0(MemFn f)
          :f_(f)
        {}

        return_type operator()(Obj obj)
        {
          return ((*obj).*f_)();
        }

        return_type operator()(const Obj& obj)
        {
          return ((*obj).*f_)();
        }

        base_t* clone() const { return new this_t(f_); }
      };


      //the parameter defines the arity
        TTL_FUNC_BUILD_FUNCTOR_CALLER(2)
        TTL_FUNC_BUILD_FUNCTOR_CALLER(3)
        TTL_FUNC_BUILD_FUNCTOR_CALLER(4)
        TTL_FUNC_BUILD_FUNCTOR_CALLER(5)
        TTL_FUNC_BUILD_FUNCTOR_CALLER(6)
        TTL_FUNC_BUILD_FUNCTOR_CALLER(7)
        TTL_FUNC_BUILD_FUNCTOR_CALLER(8)
        TTL_FUNC_BUILD_FUNCTOR_CALLER(9)
        TTL_FUNC_BUILD_FUNCTOR_CALLER(10)
        TTL_FUNC_BUILD_FUNCTOR_CALLER(11)
        TTL_FUNC_BUILD_FUNCTOR_CALLER(12)
        TTL_FUNC_BUILD_FUNCTOR_CALLER(13)
        TTL_FUNC_BUILD_FUNCTOR_CALLER(14)
        TTL_FUNC_BUILD_FUNCTOR_CALLER(15)

#undef TTL_FUNC_BUILD_FUNCTOR_CALLER
    };
  };

  template<typename R = ttl::meta::empty_type, TTL_TPARAMS_DEF(TTL_MAX_FUNCTION_PARAMS, ttl::meta::empty_type)> struct function;

  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function
  {
    typedef function this_t;
    typedef __::func::functor_caller_base0<R> caller;
    typedef R return_type;
    typedef R result_type; 
    enum { arity = 0 };

    function() : fc_(0) {}
    virtual ~function() { destroy(); }

    template<typename F>
    function(F f, typename enable_if<!is_member_function_pointer<F>::value>::type* = 0)
      :fc_()
    {
      typedef __::func::functor_caller0<F, R> caller_spec;
      fc_ = new caller_spec(f);
    }

    function(const this_t& r) : fc_(0)
    {
      operator=(r);
    }

    this_t& operator=(const this_t& r)
    {
      if(this == &r) return *this;
      destroy();
      fc_ = r.fc_->clone();
      return *this;
    }

    return_type operator()() 
    { 
      if(!fc_) __ntl_throw(bad_function_call());
      return (*fc_)(); 
    }

    inline bool is_valid() const { return fc_ != 0; }

  protected:
    caller *fc_;
    void destroy()
    {
      if(!is_valid()) return;
      delete fc_;
      fc_ = 0;
    }
  };

#define TTL_FUNC_BUILD_FUNCTION(n) \
  typedef function this_t; \
  typedef __::func::functor_caller_base##n<R, TTL_ARGS(n)> caller; \
  typedef R return_type; \
  typedef R result_type; \
  typedef T1 argument_type; \
  typedef T1 first_argument_type; \
  typedef T2 second_argument_type; \
  enum { arity = n };  \
  function() : fc_(0) {} \
  virtual ~function() { destroy(); } \
  template<typename F> \
  function(F f) : fc_(0) \
  { \
  typedef __::func::functor_caller##n<F, R (TTL_ARGS(n))> caller_spec; \
  fc_ = new caller_spec(f); \
  } \
  function(const this_t& r) : fc_(0) \
  { \
  operator=(r); \
  } \
  this_t& operator=(const this_t& r) \
  { \
  if(this == &r) return *this; \
  destroy(); \
  fc_ = r.fc_->clone(); \
  return *this; \
  } \
  return_type operator()(TTL_FUNC_PARAMS(n,p)) \
  { \
  if(!fc_) __ntl_throw(bad_function_call()); \
  return (*fc_)(TTL_ENUM_ITEMS(n,p));  \
  } \
  inline bool is_valid() const { return fc_ != 0; } \
protected: \
  caller *fc_; \
  void destroy() \
  { \
  if(!is_valid()) return; \
  delete fc_;  \
  fc_ = 0; \
  }

  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function<R (TTL_ARGS(1)), T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>
  {
#if 0
    TTL_FUNC_BUILD_FUNCTION(1)
#else
    typedef function this_t;
    typedef __::func::functor_caller_base1<R, T1> caller;
    typedef R return_type;
    typedef R result_type;
    typedef T1 argument_type;
    typedef T1 first_argument_type;
    typedef T2 second_argument_type;
    enum { arity = 1 }; 

    function() : fc_(0) {}
    virtual ~function() { destroy(); }

    template<typename F>
    function(F f, typename enable_if<!is_member_function_pointer<F>::value>::type* = 0) : fc_(0)
    {
      typedef __::func::functor_caller1<F, R (T1)> caller_spec;
      fc_ = new caller_spec(f);
    }

    template<typename MemFn>
    function(MemFn f, typename enable_if<is_member_function_pointer<MemFn>::value>::type* = 0)
    {
      typedef __::func::memfunobj<MemFn> funspec;
      typedef __::func::memfun_caller0<R,MemFn,typename funspec::object_type*> caller_spec;
      fc_ = new caller_spec(f);
    }


    function(const this_t& r) : fc_(0)
    {
      operator=(r);
    }
    this_t& operator=(const this_t& r)
    {
      if(this == &r) return *this;
      destroy();
      fc_ = r.fc_->clone();
      return *this;
    }
    return_type operator()(T1 p1)
    {
      if(!fc_) __ntl_throw(bad_function_call());
      return (*fc_)(p1); 
    }
    inline bool is_valid() const { return fc_ != 0; }
  protected:
    caller *fc_;
    void destroy()
    {
      if(!is_valid()) return;
      delete fc_; 
      fc_ = 0;
    }
#endif
  };
  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function<R (TTL_ARGS(2)), T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>
  {
    TTL_FUNC_BUILD_FUNCTION(2)
  };
  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function<R (TTL_ARGS(3)), T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>
  {
    TTL_FUNC_BUILD_FUNCTION(3)
  };
  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function<R (TTL_ARGS(4)), T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>
  {
    TTL_FUNC_BUILD_FUNCTION(4)
  };
  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function<R (TTL_ARGS(5)), T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>
  {
    TTL_FUNC_BUILD_FUNCTION(5)
  };
  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function<R (TTL_ARGS(6)), T7, T8, T9, T10, T11, T12, T13, T14, T15>
  {
    TTL_FUNC_BUILD_FUNCTION(6)
  };
  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function<R (TTL_ARGS(7)), T8, T9, T10, T11, T12, T13, T14, T15>
  {
    TTL_FUNC_BUILD_FUNCTION(7)
  };
  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function<R (TTL_ARGS(8)), T9, T10, T11, T12, T13, T14, T15>
  {
    TTL_FUNC_BUILD_FUNCTION(8)
  };
  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function<R (TTL_ARGS(9)), T10, T11, T12, T13, T14, T15>
  {
    TTL_FUNC_BUILD_FUNCTION(9)
  };
  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function<R (TTL_ARGS(10)), T11, T12, T13, T14, T15>
  {
    TTL_FUNC_BUILD_FUNCTION(10)
  };
  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function<R (TTL_ARGS(11)), T12, T13, T14, T15>
  {
    TTL_FUNC_BUILD_FUNCTION(11)
  };
  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function<R (TTL_ARGS(12)), T13, T14, T15>
  {
    TTL_FUNC_BUILD_FUNCTION(12)
  };
  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function<R (TTL_ARGS(13)), T14, T15>
  {
    TTL_FUNC_BUILD_FUNCTION(13)
  };
  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function<R (TTL_ARGS(14)), T15>
  {
    TTL_FUNC_BUILD_FUNCTION(14)
  };
  template<typename R, TTL_TPARAMS(TTL_MAX_FUNCTION_PARAMS)>
  struct function<R (TTL_ARGS(15))>
  {
    TTL_FUNC_BUILD_FUNCTION(15)
  };


#undef TTL_FUNC_BUILD_FUNCTION
}

#endif // STLX_FUNCTION
