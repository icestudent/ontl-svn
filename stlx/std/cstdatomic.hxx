/**\file*********************************************************************
 *                                                                     \brief
 *  Atomic operations library [29 atomics]
 *
 ****************************************************************************
 */

#ifndef STLX_ATOMIC
#define STLX_ATOMIC

namespace stlx
{
/**\defgroup  lib_atomic *** 29 Atomic operations library [atomics] *****
 *
 *    Components for fine-grained atomic access.
 * @{
 **/


  /// Order and consistency [29.1 atomics.order]
  enum memory_order
  {
    /** the operation does not order memory */
    memory_order_relaxed,

    /** the operation performs a release operation on the affected memory location
    thus making regular memory writes visible to other threads through the
    atomic variable to which it is applied */
    memory_order_consume,
    
    /** the operation performs an acquire operation on the affected memory lo
    tion, thus making regular memory writes in other threads released throu
    the atomic variable to which it is applied visible to the current thread */
    memory_order_acquire,
    
    /** the operation performs a consume operation on the affected memory loca-
    tion, thus making regular memory writes in other threads released through
    the atomic variable to which it is applied visible to the regular memory
    reads that are dependencies of this consume operation */
    memory_order_release,
    
    /** the operation has both acquire and release semantics */
    memory_order_acq_rel,
    
    /** the operation has both acquire and release semantics, and, in addition, has
    sequentially-consistent operation ordering */
    memory_order_seq_cst
  };

  /**
   *	The argument does not carry a dependency to the return value
   *  @return y
   **/
  template <class T>
  T kill_dependency(T y);



#pragma region atomic implementation

  namespace __
  {
    namespace atomic_v1
    {
       template<typename T, bool fundamental>
       struct atomic_base;

       template<typename T>
       struct atomic_base<T, true>
       {
         typedef T type;
         static const bool lock_free = true;

         void store(type value, memory_order mo = memory_order_seq_cst) volatile
         {

         }

         type load(memory_order mo = memory_order_seq_cst) const volatile
         {

         }
         type exchange(type value, memory_order mo = memory_order_seq_cst) volatile
         {

         }

         type compare_exchange(type&, type, memory_order, memory_order) volatile;
         type compare_exchange(type&, type, memory_order = memory_order_seq_cst) volatile;
         void fence(memory_order) const volatile;
       };
    }
  }

#pragma endregion



  // 29.2, lock-free property
  // 0 - never lock free, 1 - sometimes lock free, 2 - always lock-free
#define ATOMIC_INTEGRAL_LOCK_FREE 2
#define ATOMIC_ADDRESS_LOCK_FREE 1


  /// Flag Type and Operations [29.5 atomics.flag]
  struct atomic_flag
  {
    bool test_and_set(memory_order = memory_order_seq_cst) volatile;
    void clear(memory_order = memory_order_seq_cst) volatile;
    void fence(memory_order) const volatile;

#ifdef NTL__CXX_EF
    atomic_flag() = default;
#endif
  private:
    atomic_flag(const atomic_flag&) __deleted;
    atomic_flag& operator=(const atomic_flag&) __deleted;
  };

  bool atomic_flag_test_and_set(volatile atomic_flag* flag)
  {
    return flag->test_and_set();
  }
  bool atomic_flag_test_and_set_explicit(volatile atomic_flag* flag, memory_order mo)
  {
    return flag->test_and_set(mo);
  }
  void atomic_flag_clear(volatile atomic_flag* flag)
  {
    flag->clear();
  }
  void atomic_flag_clear_explicit(volatile atomic_flag* flag, memory_order mo)
  {
    flag->clear(mo);
  }
  void atomic_flag_fence(const volatile atomic_flag* flag, memory_order mo)
  {
    flag->fence(mo);
  }


#define ATOMIC_FLAG_INIT unspecified
  extern const atomic_flag atomic_global_fence_compatibility;


  // 29.3.1, integral types
  struct atomic_bool
  {
    bool is_lock_free() const volatile;
    void store(bool, memory_order = memory_order_seq_cst) volatile;
    bool load(memory_order = memory_order_seq_cst) const volatile;
    operator bool() const volatile;
    bool exchange(bool, memory_order = memory_order_seq_cst) volatile;
    bool compare_exchange(bool&, bool, memory_order, memory_order) volatile;
    bool compare_exchange(bool&, bool, memory_order = memory_order_seq_cst) volatile;
    void fence(memory_order) const volatile;
    atomic_bool() = default;
    constexpr explicit atomic_bool(bool);
    atomic_bool(const atomic_bool&) = delete;
    atomic_bool& operator=(const atomic_bool&) = delete;
    bool operator=(bool) volatile;
  };
  bool atomic_is_lock_free(const volatile atomic_bool*);
  void atomic_store(volatile atomic_bool*, bool);
  void atomic_store_explicit(volatile atomic_bool*, bool, memory_order);
  bool atomic_load(const volatile atomic_bool*);
  bool atomic_load_explicit(const volatile atomic_bool*, memory_order);
  bool atomic_exchange(volatile atomic_bool*);
  bool atomic_exchange_explicit(volatile atomic_bool*, bool);
  bool atomic_compare_exchange(volatile atomic_bool*, bool*, bool);
  bool atomic_compare_exchange_explicit(volatile atomic_bool*, bool*, bool, memory_order, memory_order);
  void atomic_fence(const volatile atomic_bool*, memory_order);


  // For each of the integral types:
  struct atomic_itype
  {
    bool is_lock_free() const volatile;
    void store(integral, memory_order = memory_order_seq_cst) volatile;
    integral load(memory_order = memory_order_seq_cst) const volatile;
    operator integral() const volatile;
    integral exchange(integral,
      memory_order = memory_order_seq_cst) volatile;
    bool compare_exchange(integral&, integral,
      memory_order, memory_order) volatile;
    bool compare_exchange(integral&, integral,
      memory_order = memory_order_seq_cst) volatile;
    void fence(memory_order) const volatile;
    integral fetch_add(integral,
      memory_order = memory_order_seq_cst) volatile;
    integral fetch_sub(integral,
      memory_order = memory_order_seq_cst) volatile;
    integral fetch_and(integral,
      memory_order = memory_order_seq_cst) volatile;
    integral fetch_or(integral,
      memory_order = memory_order_seq_cst) volatile;
    integral fetch_xor(integral,
      memory_order = memory_order_seq_cst) volatile;
    atomic_itype() = default;
    constexpr explicit atomic_itype(integral);
    atomic_itype(const atomic_itype&) = delete;
    atomic_itype& operator=(const atomic_itype &) = delete;
    integral operator=(integral) volatile;
    integral operator++(int) volatile;
    integral operator--(int) volatile;
    integral operator++() volatile;
    integral operator--() volatile;
    integral operator+=(integral) volatile;
    integral operator-=(integral) volatile;
    integral operator&=(integral) volatile;
    integral operator|=(integral) volatile;
    integral operator^=(integral) volatile;
  };
  bool atomic_is_lock_free(const volatile atomic_itype*);
  void atomic_store(volatile atomic_itype*, integral);
  void atomic_store_explicit(volatile atomic_itype*, integral, memory_order);
  integral atomic_load(const volatile atomic_itype*);
  integral atomic_load_explicit(const volatile atomic_itype*, memory_order);
  integral atomic_exchange(volatile atomic_itype*, integral);
  integral atomic_exchange_explicit(volatile atomic_itype*, integral, memory_order);
  bool atomic_compare_exchange(volatile atomic_itype*, integral*, integral);
  bool atomic_compare_exchange_explicit(volatile atomic_itype*, integral*, integral, memory_order, memory_order);
  void atomic_fence(const volatile atomic_itype*, memory_order);
  integral atomic_fetch_add(volatile atomic_itype*, integral);
  integral atomic_fetch_add_explicit(volatile atomic_itype*, integral, memory_order);
  integral atomic_fetch_sub(volatile atomic_itype*, integral);
  integral atomic_fetch_sub_explicit(volatile atomic_itype*, integral, memory_order);
  integral atomic_fetch_and(volatile atomic_itype*, integral);
  integral atomic_fetch_and_explicit(volatile atomic_itype*, integral, memory_order);
  integral atomic_fetch_or(volatile atomic_itype*, integral);
  integral atomic_fetch_or_explicit(volatile atomic_itype*, integral, memory_order);
  integral atomic_fetch_xor(volatile atomic_itype*, integral);
  integral atomic_fetch_xor_explicit(volatile atomic_itype*, integral, memory_order);


  // 29.3.2, address types
  struct atomic_address
  {
    bool is_lock_free() const volatile;
    void store(void*, memory_order = memory_order_seq_cst) volatile;
    void* load(memory_order = memory_order_seq_cst) const volatile;
    operator void*() const volatile;
    void* exchange(void*, memory_order = memory_order_seq_cst) volatile;
    bool compare_exchange(void*&, void*,
      memory_order, memory_order) volatile;
    bool compare_exchange(void*&, void*,
      memory_order = memory_order_seq_cst) volatile;
    void fence(memory_order) const volatile;
    void* fetch_add(ptrdiff_t,
      memory_order = memory_order_seq_cst) volatile;
    void* fetch_sub(ptrdiff_t,
      memory_order = memory_order_seq_cst) volatile;
    atomic_address() = default;
    constexpr explicit atomic_address(void*);
    atomic_address(const atomic_address&) = delete;
    atomic_address& operator=(const atomic_address&) = delete;
    void* operator=(void*) volatile;
    void* operator+=(ptrdiff_t) volatile;
    void* operator-=(ptrdiff_t) volatile;
  };
  bool atomic_is_lock_free(const volatile atomic_address*);
  void atomic_store(volatile atomic_address*, void*);
  void atomic_store_explicit(volatile atomic_address*, void*, memory_order);
  void* atomic_load(const volatile atomic_address*);
  void* atomic_load_explicit(const volatile atomic_address*, memory_order);
  void* atomic_exchange(volatile atomic_address*);
  void* atomic_exchange_explicit(volatile atomic_address*, void*, memory_order);
  bool atomic_compare_exchange(volatile atomic_address*, void**, void*);
  bool atomic_compare_exchange_explicit(volatile atomic_address*, void**, void*, memory_order, memory_order);
  void atomic_fence(const volatile atomic_address*, memory_order);
  void* atomic_fetch_add(volatile atomic_address*, ptrdiff_t);
  void* atomic_fetch_add_explicit(volatile atomic_address*, ptrdiff_t, memory_order);
  void* atomic_fetch_sub(volatile atomic_address*, ptrdiff_t);
  void* atomic_fetch_sub_explicit(volatile atomic_address*, ptrdiff_t,
    memory_order);


  // 29.3.3, generic types
  template<class T> 
  struct atomic
  {
    bool is_lock_free() const volatile;

    void store(T, memory_order = memory_order_seq_cst) volatile;
    T load(memory_order = memory_order_seq_cst) const volatile;
    operator T() const volatile;
    
    T exchange(T, memory_order = memory_order_seq_cst) volatile;
    bool compare_exchange(T&, T, memory_order, memory_order) volatile;
    bool compare_exchange(T&, T, memory_order = memory_order_seq_cst) volatile;
    
    void fence(memory_order) const volatile;

    atomic() = default;
    constexpr explicit atomic(T);
    atomic(const atomic&) = delete;
    atomic& operator=(const atomic&) = delete;

    T operator=(T) volatile;
  };

  template<class T>
  struct atomic<T*>
  {
    void store(T*, memory_order = memory_order_seq_cst) volatile;
    T* load(memory_order = memory_order_seq_cst) const volatile;
    operator T*() const volatile;
    
    T* exchange(T*, memory_order = memory_order_seq_cst) volatile;
    bool compare_exchange(T*&, T*, memory_order, memory_order) volatile;
    bool compare_exchange(T*&, T*, memory_order = memory_order_seq_cst) volatile;
    
    T* fetch_add(ptrdiff_t, memory_order = memory_order_seq_cst) volatile;
    T* fetch_sub(ptrdiff_t, memory_order = memory_order_seq_cst) volatile;
    
    atomic() = default;
    constexpr explicit atomic(T*);
    atomic(const atomic&) = delete;
    atomic& operator=(const atomic&) = delete;
    T* operator=(T*) volatile;

    T* operator++(int) volatile;
    T* operator--(int) volatile;
    T* operator++() volatile;
    T* operator--() volatile;
    T* operator+=(ptrdiff_t) volatile;
    T* operator-=(ptrdiff_t) volatile;
  };

  template<> struct atomic<integral>;

  /**@} lib_utilities */
} // namespace stlx

#include "memory.hxx"

namespace stlx 
{
  /// 20.7.12.5 shared_ptr atomic access [util.smartptr.shared.atomic]
  template<class T>
  inline
  bool atomic_is_lock_free(const shared_ptr<T> *p) __ntl_nothrow
  {
    return *p ? atomic_is_lock_free(p->get()) : false;
  }

  template<class T>
  inline
  shared_ptr<T> atomic_load(const shared_ptr<T> *p) __ntl_nothrow
  {
    atomic_load_explicit(p, memory_order_seq_cst);
  }
  
  template<class T>
  shared_ptr<T> atomic_load_explicit(const shared_ptr<T> *p, memory_order mo) __ntl_nothrow;
  
  template<class T>
  inline
  void atomic_store(shared_ptr<T> *p, shared_ptr<T> r) __ntl_nothrow
  {
    atomic_store_explicit(p, r, memory_order_acq_rel);
  }
  
  template<class T>
  void atomic_store_explicit(shared_ptr<T> *p, shared_ptr<T> r, memory_order mo) __ntl_nothrow;
  
  template<class T>
  inline
  shared_ptr<T> atomic_exchange(shared_ptr<T> *p, shared_ptr<T> r) __ntl_nothrow
  {
    atomic_exchange_explicit(p, r, memory_order_seq_cst);
  }
  
  template<class T>
  shared_ptr<T> atomic_exchange_explicit(shared_ptr<T> *p, shared_ptr<T> r, memory_order mo) __ntl_nothrow;

  template<class T>
  inline
  bool atomic_compare_exchange(shared_ptr<T> *p, shared_ptr<T> *v, shared_ptr<T> w) __ntl_nothrow
  {
    return atomic_compare_exchange_explicit(p, v, w, memory_order_seq_cst);
  }
  
  template<class T>
  bool atomic_compare_exchange_explicit(shared_ptr<T> *p, shared_ptr<T> *v, shared_ptr<T> w, memory_order success, memory_order failure) __ntl_nothrow;

} // namespace stlx

#endif // STLX_ATOMIC
