// common tests part
#include <cassert>
#include <nt/new.hxx>

#define __attribute__(x)
#pragma warning(disable:4101 4189 4800)
#define VERIFY(e) assert(e)
#define sizeofa(a) (sizeof(a) / sizeof(*a))

#include <stlx/mutex.hxx>

#include <memory>
#include <vector>

#include <atomic.hxx>
#include <nt/thread.hxx>
#include <nt/debug.hxx>

namespace dbg = ntl::nt::dbg;

namespace std
{
  template class lock_guard<mutex>;
  template class unique_lock<mutex>;
}

namespace
{
  void test00()
  {
    typedef std::mutex mutex_type;
    mutex_type m1;
    // ensure no exceptions
  }

  void test01()
  {
    // assign
    typedef std::mutex mutex_type;
    mutex_type m1;
    mutex_type m2;
    //m1 = m2; // fail
  }

  void test02()
  {
    // assign
    typedef std::mutex mutex_type;
    mutex_type m1;
    //mutex_type m2(m1); // fail
  }

  void test03()
  {
    typedef std::mutex mutex_type;

    mutex_type m;
    m.lock();
  }

  void test04()
  {
    typedef std::mutex mutex_type;

      mutex_type m;
      m.lock();
      m.unlock();
  }

  void test05()
  {
    bool test __attribute__((unused)) = true;
    typedef std::mutex mutex_type;

      mutex_type m;
      bool b = m.try_lock();
      VERIFY( b );
      m.unlock();
  }

  void test06()
  {
    bool test __attribute__((unused)) = true;
    typedef std::mutex mutex_type;

    __ntl_try 
    {
      mutex_type m;
      m.lock();
      bool b;

      __ntl_try
      {
        b = m.try_lock();
        VERIFY( !b );
      }
      __ntl_catch (const std::system_error& e)
      {
        VERIFY( false );
      }

      m.unlock();
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }
  }

  void test07()
  {
    bool test __attribute__((unused)) = true;
    typedef std::mutex mutex_type;

    // Unlock mutex that hasn't been locked.
    mutex_type m;
    m.unlock();
  }

  void test08()
  {
      // Check for required typedefs
      typedef std::lock_guard<std::mutex> test_type;
      typedef test_type::mutex_type mutex_type;

  }

  void test09()
  {
    typedef std::mutex mutex_type;
    typedef std::unique_lock<mutex_type> lock_type;

    mutex_type m1, m2, m3;
    lock_type l1(m1, std::defer_lock), 
      l2(m2, std::defer_lock),
      l3(m3, std::defer_lock);

    int result = std::try_lock(l1, l2, l3);
    VERIFY( result == -1 );
  }

  void test10()
  {
    __ntl_try
    {
      std::mutex m1, m2, m3;
      m1.lock();
      int result = std::try_lock(m1, m2, m3);
      VERIFY( result == 0 );
      //m1.lock(); // fail due to m1 already locked upper
      m2.lock();
      m3.lock();
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }
  }

  void test11()
  {
    bool test __attribute__((unused)) = true;

    __ntl_try
    {
      std::mutex m1, m2, m3;
      m2.lock();
      int result = std::try_lock(m1, m2, m3);
      VERIFY( result == 1 );
      m1.lock();
      //m2.lock();  // fail due to m2 already locked upper
      m3.lock();
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }
  }

  void test12()
  {
    bool test __attribute__((unused)) = true;

    __ntl_try
    {
      std::mutex m1, m2, m3;
      m3.lock();
      int result = std::try_lock(m1, m2, m3);
      VERIFY( result == 2 );
      m1.lock();
      m2.lock();
      //m3.lock();  // fail due to m3 already locked upper
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }
  }

  struct user_lock
  {
    user_lock() : is_locked(false) { }

    void lock()
    {
      bool test __attribute__((unused)) = true;
      VERIFY( !is_locked );
      is_locked = true;
    }

    bool try_lock() 
    { return is_locked ? false : (is_locked = true); }

    void unlock()
    {
      bool test __attribute__((unused)) = true;
      VERIFY( is_locked );
      is_locked = false;
    }

  private:
    bool is_locked;
  };

  void test14()
  {
    bool test __attribute__((unused)) = true;

    __ntl_try
    {
      std::mutex m1;
      std::recursive_mutex m2;
      user_lock m3;

      __ntl_try
      {
        //heterogeneous types
        int result = std::try_lock(m1, m2, m3);
        VERIFY( result == -1 );
        m1.unlock();
        m2.unlock();
        m3.unlock();
      }
      __ntl_catch (const std::system_error& e)
      {
        VERIFY( false );
      }
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }
  }

  void test15()
  {
      // Check for required typedefs
      typedef std::unique_lock<std::mutex> test_type;
      typedef test_type::mutex_type mutex_type;
  }

  int test16()
  {
    bool test __attribute__((unused)) = true;
    typedef std::mutex mutex_type;
    typedef std::unique_lock<mutex_type> lock_type;

    __ntl_try 
    {
      lock_type lock;

      VERIFY( !lock.owns_lock() );
      VERIFY( !(bool)lock );
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }

  int test17()
  {
    bool test __attribute__((unused)) = true;
    typedef std::mutex mutex_type;
    typedef std::unique_lock<mutex_type> lock_type;

    __ntl_try 
    {
      mutex_type m;
      lock_type lock(m);

      VERIFY( lock.owns_lock() );
      VERIFY( (bool)lock );
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }

  int test18()
  {
    bool test __attribute__((unused)) = true;
    typedef std::mutex mutex_type;
    typedef std::unique_lock<mutex_type> lock_type;

    __ntl_try 
    {
      mutex_type m;
      lock_type lock(m, std::defer_lock);

      VERIFY( !lock.owns_lock() );
      VERIFY( !(bool)lock );
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }

  int test19()
  {
    bool test __attribute__((unused)) = true;
    typedef std::mutex mutex_type;
    typedef std::unique_lock<mutex_type> lock_type;

    __ntl_try 
    {
      mutex_type m;
      lock_type lock(m, std::try_to_lock);

      VERIFY( lock.owns_lock() );
      VERIFY( (bool)lock );
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }

  int test20()
  {
    bool test __attribute__((unused)) = true;
    typedef std::timed_mutex mutex_type;
    typedef std::unique_lock<mutex_type> lock_type;
    typedef std::chrono::system_clock clock_type;

    __ntl_try 
    {
      clock_type::time_point t = clock_type::now() + std::chrono::seconds(5);

      mutex_type m;
      lock_type lock(m, t);

      VERIFY( lock.owns_lock() );
      VERIFY( (bool)lock );
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }

  int test21()
  {
    bool test __attribute__((unused)) = true;
    typedef std::timed_mutex mutex_type;
    typedef std::unique_lock<mutex_type> lock_type;
    typedef std::chrono::system_clock clock_type;

    __ntl_try 
    {
      clock_type::duration d = std::chrono::seconds(5);

      mutex_type m;
      lock_type lock(m, d);

      VERIFY( lock.owns_lock() );
      VERIFY( (bool)lock );
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }

  int test22()
  {
    bool test __attribute__((unused)) = true;
    typedef std::mutex mutex_type;
    typedef std::unique_lock<mutex_type> lock_type;

    __ntl_try 
    {
      mutex_type m;
      lock_type l(m, std::defer_lock);

      l.lock();

      VERIFY( (bool)l );
      VERIFY( l.owns_lock() );

      l.unlock();

      VERIFY( !(bool)l );
      VERIFY( !l.owns_lock() );
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }

  void test23()
  {
    bool test __attribute__((unused)) = true;
    typedef std::mutex mutex_type;
    typedef std::unique_lock<mutex_type> lock_type;

    __ntl_try
    {
      lock_type l;

      // Lock unique_lock w/o mutex
      __ntl_try
      {
        l.lock();
      }
      __ntl_catch (const std::system_error& ex)
      {
        // TODO: exception code
        VERIFY( 0 && "ex.code() == std::error_code(std::errc::operation_not_permitted)" );
      }
      __ntl_catch (...)
      {
        VERIFY( false );
      }
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }
  }


  void test24()
  {
    bool test __attribute__((unused)) = true;
    typedef std::mutex mutex_type;
    typedef std::unique_lock<mutex_type> lock_type;

    __ntl_try 
    {
      mutex_type m;
      lock_type l(m);

      // Lock already locked unique_lock.
      __ntl_try
      {
        l.lock();
      }
      __ntl_catch (const std::system_error& ex)
      {
        // TODO: exception code
        VERIFY( 0 && "ex.code() == std::error_code(std::errc::resource_deadlock_would_occur)" );
      }
      __ntl_catch (...)
      {
        VERIFY( false );
      }
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }
  }


  int test25()
  {
    bool test __attribute__((unused)) = true;
    typedef std::timed_mutex mutex_type;
    typedef std::unique_lock<mutex_type> lock_type;

    __ntl_try 
    {
      mutex_type m;
      lock_type l(m, std::defer_lock);

      __ntl_try
      {
        l.try_lock_for(std::chrono::milliseconds(100));
      }
      __ntl_catch(const std::system_error&)
      {
        VERIFY( false );
      }
      __ntl_catch (...)
      {
        VERIFY( false );
      }

      VERIFY( l.owns_lock() );
    }
    __ntl_catch (const std::system_error&)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }


  int test26()
  {
    bool test __attribute__((unused)) = true;
    typedef std::timed_mutex mutex_type;
    typedef std::unique_lock<mutex_type> lock_type;
    typedef std::chrono::system_clock clock_type;

    __ntl_try 
    {
      mutex_type m;
      lock_type l(m, std::defer_lock);
      clock_type::time_point t = clock_type::now() + std::chrono::seconds(1);

      __ntl_try
      {
        l.try_lock_until(t);
      }
      __ntl_catch(const std::system_error&)
      {
        VERIFY( false );
      }
      __ntl_catch (...)
      {
        VERIFY( false );
      }

      VERIFY( l.owns_lock() );
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }

  int test27()
  {
    bool test __attribute__((unused)) = true;
    typedef std::mutex mutex_type;
    typedef std::unique_lock<mutex_type> lock_type;

    __ntl_try 
    {
      mutex_type m;
      lock_type l1(m);
      lock_type l2;

      __ntl_try
      {
        l1.swap(l2);
      }
      __ntl_catch (const std::system_error&)
      {
        VERIFY( false );
      }
      __ntl_catch(...)
      {
        VERIFY( false );
      }

      VERIFY( !(bool)l1 );
      VERIFY( (bool)l2 );
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }

  int test28()
  {
    bool test __attribute__((unused)) = true;
    typedef std::mutex mutex_type;
    typedef std::unique_lock<mutex_type> lock_type;

    __ntl_try 
    {
      mutex_type m;
      lock_type l1(m);
      lock_type l2;

      __ntl_try
      {
        l1.swap(l2);
      }
      __ntl_catch (const std::system_error&)
      {
        VERIFY( false );
      }
      __ntl_catch(...)
      {
        VERIFY( false );
      }

      VERIFY( !(bool)l1 );
      VERIFY( (bool)l2 );
    }
    __ntl_catch (const std::system_error&)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }

  void test29()
  {
    // Check for required typedefs
    typedef std::recursive_mutex test_type;
    typedef test_type::native_handle_type type;
  }

  int test30()
  {
    bool test __attribute__((unused)) = true;
    typedef std::recursive_mutex mutex_type;

    __ntl_try 
    {
      mutex_type m1;
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }

  void test31()
  {
    // assign
    typedef std::recursive_mutex mutex_type;
    mutex_type m1;
    mutex_type m2;
    // should fail to compile
    //m1 = m2;
    //mutex_type m3(m1);

    std::recursive_timed_mutex m4;
  }


  int test32()
  {
    bool test __attribute__((unused)) = true;
    typedef std::recursive_mutex mutex_type;

    __ntl_try 
    {
      mutex_type m;
      m.lock();
    }
    __ntl_catch (const std::system_error& e)
    {
      // Destroying locked mutex raises system error, or undefined.
      // POSIX == may fail with EBUSY.
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }


  int test33()
  {
    bool test __attribute__((unused)) = true;
    typedef std::recursive_mutex mutex_type;

    __ntl_try 
    {
      mutex_type m;
      m.lock();

      // Lock already locked recursive mutex.
      __ntl_try
      {
        // XXX Will not block.
        m.lock();
      }
      __ntl_catch (const std::system_error& e)
      {
        VERIFY( false );
      }

      m.unlock();
      m.unlock();
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }

  int test34()
  {
    bool test __attribute__((unused)) = true;
    typedef std::recursive_mutex mutex_type;

    __ntl_try 
    {
      mutex_type m;
      bool b = m.try_lock();
      VERIFY( b );
      m.unlock();
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }

  int test35()
  {
    bool test __attribute__((unused)) = true;
    typedef std::recursive_mutex mutex_type;

    __ntl_try 
    {
      mutex_type m;
      m.lock();
      bool b;

      __ntl_try
      {
        b = m.try_lock();
        VERIFY( b );
        m.unlock();
      }
      __ntl_catch (const std::system_error& e)
      {
        VERIFY( false );
      }
      __ntl_catch(...)
      {
        VERIFY( false );
      }

      m.unlock();
    }
    __ntl_catch (const std::system_error& e)
    {
      VERIFY( false );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }

  int test36()
  {
    bool test __attribute__((unused)) = true;
    typedef std::recursive_mutex mutex_type;

    __ntl_try 
    {
      // Unlock mutex that hasn't been locked.
      mutex_type m;
      m.unlock();
    }
    __ntl_catch (const std::system_error& e)
    {
      // POSIX == EPERM
      VERIFY( true );
    }
    __ntl_catch (...)
    {
      VERIFY( false );
    }

    return 0;
  }

  struct once_flag
  {
    constexpr once_flag()
      :inited(false), locked(false)
    {}

  private:
#if 1
    void enter() 
    {
      dbg::trace.printf("[%04d] -> " __func__ "\n", ntl::nt::teb::instance().ClientId.UniqueThread);
      
      // compare_exchange:
      // tmp = *dest
      // if(*dest == comperand)
      //  *dest = exchange
      // return tmp
      while(ntl::atomic::compare_exchange(locked, true, false) == 1){
        //ntl::cpu::pause();
        ntl::nt::NtDelayExecution(false, -1); // 870-986M
        //ntl::nt::sleep(1); // 1077M
        // CS lock: 14M
      }

      dbg::trace.printf("[%04d] <- " __func__ "\n", ntl::nt::teb::instance().ClientId.UniqueThread);
    }

    void exit()
    {
      ntl::atomic::exchange(locked, false);
      dbg::trace.printf("[%04d] <- " __func__ "\n", ntl::nt::teb::instance().ClientId.UniqueThread);
    }
#else
    void enter()
    {
      dbg::trace.printf("[%04d] -> " __func__ "\n", ntl::nt::teb::instance().ClientId.UniqueThread);
      m_lock.lock();
    }

    void exit()
    {
      dbg::trace.printf("[%04d] -> " __func__ "\n", ntl::nt::teb::instance().ClientId.UniqueThread);
      m_lock.unlock();
    }
  private:
    std::mutex m_lock;

#endif
  private:
    volatile bool inited;
    volatile long locked;

    once_flag(const once_flag&);
    once_flag& operator=(const once_flag&);

    template<class Callable>
    friend void call_once(once_flag& flag, Callable func);
    template<class Callable, class Arg1>
    friend void call_once(once_flag& flag, Callable func, Arg1 arg1);
  };

  namespace __
  {
    template<bool member_function>
    struct call_once_impl
    {
      template<class Callable, class Arg1>
      static inline void call_once(Callable func, Arg1 arg1)
      {
        func(arg1);
      }
    };
    template<>
    struct call_once_impl<true>
    {
      template<class Callable, class T>
      static inline void call_once(Callable mem_fun, T* obj)
      {
        (obj->*mem_fun)();
      }
      template<class Callable, class T>
      static inline void call_once(Callable mem_fun, T obj)
      {
        (obj.*mem_fun)();
      }
    };
  }

  template<class Callable>
  void call_once(once_flag& flag, Callable func)
  {
    static_assert(std::is_member_function_pointer<Callable>::value == false, "must not be a member pointer function");
    if(flag.inited)
      return;

    flag.enter();
    if(!flag.inited){
      func();
      flag.inited = true;
    }
    flag.exit();
  }

  template<class Callable, class Arg1>
  void call_once(once_flag& flag, Callable func, Arg1 arg1)
  {
    if(flag.inited)
      return;

    flag.enter();
    if(!flag.inited){
      __::call_once_impl<std::is_member_function_pointer<Callable>::value>::call_once(func, arg1);
      flag.inited = true;
    }
    flag.exit();
  }

  class information
  {
    std::once_flag flag;

    void verifier()
    {
      ok = true;
    }
    bool ok;

  public:
    void verify()
    {
      std::call_once(flag, &information::verifier, this);
    }
  };

  struct inform
  {
    inform()
    {

    }
    ~inform()
    {

    }

    void info()
    {

    }

    void call_dtor()
    {
      this->inform::~inform();
    }
  };

  std::once_flag g_flag;
  void test37_init()
  {

  }

  void test37()
  {
    std::call_once(g_flag, test37_init);
  }

  struct initializer
  {
    void operator()()
    {

    }
  };

  void test38()
  {
    static std::once_flag flag;
    std::call_once(flag, initializer());
  }

  void test39()
  {
    information info;
    info.verify();
  }

  void test40()
  {
    inform info;
    std::once_flag flag;
    std::call_once(flag, &inform::info, info);
  }


  void test41_functor()
  {
    dbg::trace.printf("[%04d] -> " __func__ "\n", ntl::nt::teb::instance().ClientId.UniqueThread);
    ntl::nt::sleep(1000*30);
    dbg::trace.printf("[%04d] <- " __func__ "\n", ntl::nt::teb::instance().ClientId.UniqueThread);
  }

  std::once_flag g_flag41;
  static uint32_t __stdcall test41_thread_proc(void* arg)
  {
    dbg::trace.printf("[%04d] -> " __func__ "\n", ntl::nt::teb::instance().ClientId.UniqueThread);

    std::call_once(g_flag41, test41_functor);

    dbg::trace.printf("[%04d] <- " __func__ "\n", ntl::nt::teb::instance().ClientId.UniqueThread);
    return 0;
  }

#if defined NTL__CXX_LAMBDA && defined NTL__CXX_RVx
  void test41()
  {
    using namespace ntl::nt;

    const peb51* p51 = static_cast<const peb51*>(&peb::instance());
    const uint32_t hardware_concurrency = p51->NumberOfProcessors;

    std::vector<user_thread> threads;//(hardware_concurrency, empty_thread);
    threads.reserve(hardware_concurrency);

    for(uint32_t i = 0; i < hardware_concurrency; i++)
      threads.push_back(user_thread(test41_thread_proc, (void*)i, true));

    legacy_handle handles[64];
    std::transform(threads.cbegin(), threads.cend(), handles, [](const user_thread& thread) { return thread.get(); } );
    std::for_each(threads.cbegin(), threads.cend(), [](const user_thread& thread){ thread.resume(); } );
    ntstatus wait = NtWaitForMultipleObjects(hardware_concurrency, handles, wait_type::WaitAll, false, system_time::infinite());
    bool ok = success(wait);
  }
#endif
}

namespace mutex_test
{
  void main()
  {
    test00();
    test01();
    test02();
    test03();
    test04();
    test05();
    test06();
    test07();
    test08();
    test09();
    test10();
    test11();
    test12();
    test14();
    test15();
    test16();
    test17();
    test18();
    test19();
    test20();
    test21();
    test22();
    test23();
    test24();
    test25();
    test26();
    test27();
    test28();
    test29();
    test30();
    test31();
    test32();
    test33();
    test34();
    test35();
    test36();
    test37();
    test38();
    test39();
    test40();
  }
}