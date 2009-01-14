/**\file*********************************************************************
 *                                                                     \brief
 *  30.4 Condition variables [thread.condition]
 *
 ****************************************************************************
 */

#ifndef STLX_CONDVARIABLE
#define STLX_CONDVARIABLE

#include "chrono.hxx"
#include "mutex.hxx"

namespace stlx
{
  /**
   *	@addtogroup thread Thread support library [thread]
   *  @{
   **/

  /// Class condition_variable [30.4.1 thread.condition.condvar]
  class condition_variable 
  {
  public:
    condition_variable();
    ~condition_variable();
    

    void notify_one();
    void notify_all();
    
    void wait(unique_lock<mutex>& lock);
    
    template <class Predicate>
    void wait(unique_lock<mutex>& lock, Predicate pred);
    
    template <class Clock, class Duration>
    bool wait_until(unique_lock<mutex>& lock, const chrono::time_point<Clock, Duration>& abs_time);

    template <class Clock, class Duration, class Predicate>
    bool wait_until(unique_lock<mutex>& lock, const chrono::time_point<Clock, Duration>& abs_time, Predicate pred);

    template <class Rep, class Period>
    bool wait_for(unique_lock<mutex>& lock, const chrono::duration<Rep, Period>& rel_time);

    template <class Rep, class Period, class Predicate>
    bool wait_for(unique_lock<mutex>& lock, const chrono::duration<Rep, Period>& rel_time, Predicate pred);

    typedef uintptr_t native_handle_type; // See 30.1.3
    native_handle_type native_handle(); // See 30.1.3

  private:
    condition_variable(const condition_variable&) __deleted;
    condition_variable& operator=(const condition_variable&) __deleted;
  };

  class condition_variable_any
  {
  public:
    condition_variable_any();
    ~condition_variable_any();

    void notify_one();
    void notify_all();
    
    template <class Lock>
    void wait(Lock& lock);
    
    template <class Lock, class Predicate>
    void wait(Lock& lock, Predicate pred);
    
    template <class Lock, class Clock, class Duration>
    bool wait_until(Lock& lock, const chrono::time_point<Clock, Duration>& abs_time);
    
    template <class Lock, class Clock, class Duration, class Predicate>
    bool wait_until(Lock& lock, const chrono::time_point<Clock, Duration>& abs_time, Predicate pred);
    
    template <class Lock, class Rep, class Period>
    bool wait_for(Lock& lock, const chrono::duration<Rep, Period>& rel_time);
    
    template <class Lock, class Rep, class Period, class Predicate>
    bool wait_for(Lock& lock, const chrono::duration<Rep, Period>& rel_time, Predicate pred);
    
    typedef uintptr_t native_handle_type; // See 30.1.3
    native_handle_type native_handle(); // See 30.1.3

  private:
    condition_variable_any(const condition_variable_any&) __deleted;
    condition_variable_any& operator=(const condition_variable_any&) __deleted;
  };

  /** @} thread */
} // stlx

#endif // STLX_CONDVARIABLE
