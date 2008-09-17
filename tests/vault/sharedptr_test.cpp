// common tests part
#include <cassert>

#define __attribute__(x)
#pragma warning(disable:4101 4189)
#define VERIFY(e) assert(e)


#include <memory>
#include <nt/new.hxx>

class ClassType { };
class IncompleteClass;

template class std::shared_ptr<int>;
//template class std::shared_ptr<void>;           // is it right?
template class std::shared_ptr<ClassType>;
template class std::shared_ptr<IncompleteClass>;  // warning C4150 at std.shared_ptr.free(): deletion of pointer to incomplete type 'IncompleteClass'; no destructor called

namespace sharedptr_test
{
  namespace a1 {
  struct A
  {
    A() { ++ctor_count; }
    virtual ~A() { ++dtor_count; }
    static long ctor_count;
    static long dtor_count;
  };
  long A::ctor_count = 0;
  long A::dtor_count = 0;

  struct reset_count_struct
  {
    ~reset_count_struct()
    {
      A::ctor_count = 0;
      A::dtor_count = 0;
    }
  };
  }

  // 20.6.6.2.3 shared_ptr assignment [util.smartptr.shared.assign]

  // Assignment from shared_ptr<Y>
  void
    test01()
  {
    using namespace a1;
    reset_count_struct __attribute__((unused)) reset;
    bool test __attribute__((unused)) = true;

    std::shared_ptr<A> a;

    a = std::shared_ptr<A>(new A);
    VERIFY( a.get() != 0 );
    VERIFY( A::ctor_count == 1 );
    VERIFY( A::dtor_count == 0 );

    a = std::shared_ptr<A>();
    VERIFY( a.get() == 0 );
    VERIFY( A::ctor_count == 1 );
    VERIFY( A::dtor_count == 1 );
  }

  namespace a2
  {
    struct A
    {
      A() { ++ctor_count; }
      virtual ~A() { ++dtor_count; }
      static long ctor_count;
      static long dtor_count;
    };
    long A::ctor_count = 0;
    long A::dtor_count = 0;

    struct B : A
    {
      B() { ++ctor_count; }
      virtual ~B() { ++dtor_count; }
      static long ctor_count;
      static long dtor_count;
    };
    long B::ctor_count = 0;
    long B::dtor_count = 0;

void deleter(A* p) { delete p; }
    struct reset_count_struct
    {
      ~reset_count_struct()
      {
        A::ctor_count = 0;
        A::dtor_count = 0;
        B::ctor_count = 0;
        B::dtor_count = 0;
      }
    };

  }

  // 20.6.6.2.3 shared_ptr assignment [util.smartptr.shared.assign]

  // Assignment from auto_ptr<Y>
  int
    test02()
  {
    using namespace a2;
    reset_count_struct __attribute__((unused)) reset;
    bool test __attribute__((unused)) = true;

    std::shared_ptr<A> a(new A);
    std::auto_ptr<B> b(new B);
    a = b;
    VERIFY( a.get() != 0 );
    VERIFY( b.get() == 0 );
    VERIFY( A::ctor_count == 2 );
    VERIFY( A::dtor_count == 1 );
    VERIFY( B::ctor_count == 1 );
    VERIFY( B::dtor_count == 0 );

    return 0;
  }

  namespace a3{
  struct A { };
  struct B { };
  std::auto_ptr<A> source() { return std::auto_ptr<A>(); }
  }

  // 20.6.6.2.3 shared_ptr assignment [util.smartptr.shared.assign]

  // Assignment from incompatible auto_ptr<Y>
  int
    test03()
  {
    using namespace a3;
    bool test __attribute__((unused)) = true;

    std::shared_ptr<A> a;
    std::auto_ptr<B> b;
    //a = b;                      // { dg-error "here" }
    return 0;
  }


  // 20.7.12.2.3 shared_ptr assignment [util.smartptr.shared.assign]

  // Assignment from rvalue auto_ptr
  int
    test04()
  {
    using namespace a3;
    bool test __attribute__((unused)) = true;

    std::shared_ptr<A> a;
    a = source();

    return 0;
  }

  
  #ifdef NTL__CXX
  int
    test05()
  {
    bool test __attribute__((unused)) = true;

    std::shared_ptr<A> a;
    std::auto_ptr<A> au;
    a = std::move(au);

    return 0;
  }
  #endif

  // DR 541. shared_ptr template assignment and void
#if 0
  void test06()
  {
    std::shared_ptr<void> p;
    p.operator=<void>(p);
  }
#endif

  // 20.6.6.2.3 shared_ptr assignment [util.smartptr.shared.assign]

  // Assignment from shared_ptr<Y>
  void
    test07()
  {
    using namespace a2;
    reset_count_struct __attribute__((unused)) reset;
    bool test __attribute__((unused)) = true;

    std::shared_ptr<A> a;

    a = std::shared_ptr<A>();
    VERIFY( a.get() == 0 );
    VERIFY( A::ctor_count == 0 );
    VERIFY( A::dtor_count == 0 );
    VERIFY( B::ctor_count == 0 );
    VERIFY( B::dtor_count == 0 );

    //a = std::shared_ptr<A>(new A);
    //VERIFY( a.get() != 0 );
    //VERIFY( A::ctor_count == 1 );
    //VERIFY( A::dtor_count == 0 );
    //VERIFY( B::ctor_count == 0 );
    //VERIFY( B::dtor_count == 0 );

    //a = std::shared_ptr<B>(new B);
    //VERIFY( a.get() != 0 );
    //VERIFY( A::ctor_count == 2 );
    //VERIFY( A::dtor_count == 1 );
    //VERIFY( B::ctor_count == 1 );
    //VERIFY( B::dtor_count == 0 );
  }

  // 20.6.6.2.3 shared_ptr assignment [util.smartptr.shared.assign]

  // Assignment from incompatible shared_ptr<Y>
  int
    test08()
  {
    using namespace a3;
    bool test __attribute__((unused)) = true;

    std::shared_ptr<A> a;
    std::shared_ptr<B> b;
    //a = b;                      // { dg-error "here" }

    return 0;
  }

  int
    test09()
  {
    using namespace a3;
    bool test __attribute__((unused)) = true;

    std::shared_ptr<A> a;
    std::unique_ptr<A> u;
    a = u; // { dg-error "used here" }

    return 0;
  }

  namespace a4{
  struct A
  {
    virtual ~A() { }
  };

  struct B : A
  {
  };
  }

  // 20.6.6.2.6 shared_ptr comparison [util.smartptr.shared.cmp]

  int
    test10()
  {
    using namespace a4;
    // test empty shared_ptrs compare equivalent
    std::shared_ptr<A> p1;
    std::shared_ptr<B> p2;
    VERIFY( p1 == p2 );
    VERIFY( !(p1 != p2) );
    VERIFY( !(p1 < p2) && !(p2 < p1) );
    return 0;
  }


  // Construction from pointer
  int
    test11()
  {
    using namespace a4;
    std::shared_ptr<A> A_default;

    std::shared_ptr<A> A_from_A(new A);
    VERIFY( A_default != A_from_A );
    VERIFY( !(A_default == A_from_A) );
    VERIFY( (A_default < A_from_A) || (A_from_A < A_default) );

    std::shared_ptr<B> B_from_B(new B);
    VERIFY( B_from_B != A_from_A );
    VERIFY( !(B_from_B == A_from_A) );
    VERIFY( (B_from_B < A_from_A) || (A_from_A < B_from_B) );

    A_from_A.reset();
    VERIFY( A_default == A_from_A );
    VERIFY( !(A_default != A_from_A) );
    VERIFY( !(A_default < A_from_A) && !(A_from_A < A_default) );

    B_from_B.reset();
    VERIFY( B_from_B == A_from_A );
    VERIFY( !(B_from_B != A_from_A) );
    VERIFY( !(B_from_B < A_from_A) && !(A_from_A < B_from_B) );

    return 0;
  }

  namespace a5 {
  struct A
  {
    A() : i() { }
    virtual ~A() { }
    int i;
  };

  struct B : A
  {
    B() : A(), a() { }
    virtual ~B() { }
    A a;
  };

  void deletefunc(A* p) { delete p; }
  }
  // 20.6.6.2.1 shared_ptr constructors [util.smartptr.shared.const]

  // Aliasing constructors

  int test12()
  {
    using namespace a5;
    bool test __attribute__((unused)) = true;

#if 0
    std::shared_ptr<A> a;
    std::shared_ptr<bool> b1(a, &test);
    VERIFY( b1.use_count() == 0 );
    VERIFY( a.get() == 0 );
    VERIFY( b1.get() == &test );

    std::shared_ptr<bool> b2(b1);
    VERIFY( b2.use_count() == 0 );
    VERIFY( b1.get() == b2.get() );
#endif
    return 0;
  }

  // 20.6.6.2.1 shared_ptr constructors [util.smartptr.shared.const]

  // Construction from auto_ptr
  int
    test14()
  {
    using namespace a3;
    bool test __attribute__((unused)) = true;

    std::auto_ptr<A> a(new A);
    std::shared_ptr<A> a2(a);
    VERIFY( a.get() == 0 );
    VERIFY( a2.get() != 0 );
    VERIFY( a2.use_count() == 1 );

    return 0;
  }

  // 20.6.6.2.3 shared_ptr assignment [util.smartptr.shared.const]

  // Construction from const auto_ptr
  int
    test15()
  {
    using namespace a3;
    bool test __attribute__((unused)) = true;

    const std::auto_ptr<A> a;
    //std::shared_ptr<A> p(a); // { dg-error "no match" }

    return 0;
  }

  // 20.6.6.2.1 shared_ptr constructors [util.smartptr.shared.const]

  // Copy construction
  int test16()
  {
    using namespace a2;
    reset_count_struct __attribute__((unused)) reset;
    bool test __attribute__((unused)) = true;

    std::shared_ptr<A> a1;
    std::shared_ptr<A> a2(a1);
    VERIFY( a2.use_count() == 0 );
    VERIFY( A::ctor_count == 0 );
    VERIFY( A::dtor_count == 0 );
    VERIFY( B::ctor_count == 0 );
    VERIFY( B::dtor_count == 0 );

    return 0;
  }

  int
    test17()
  {
    using namespace a2;
    reset_count_struct __attribute__((unused)) reset;
    bool test __attribute__((unused)) = true;

    std::shared_ptr<A> a1(new A);
    std::shared_ptr<A> a2(a1);
    VERIFY( a2.use_count() == 2 );
    VERIFY( A::ctor_count == 1 );
    VERIFY( A::dtor_count == 0 );
    VERIFY( B::ctor_count == 0 );
    VERIFY( B::dtor_count == 0 );

    return 0;
  }

  int
    test18()
  {
    using namespace a2;
    reset_count_struct __attribute__((unused)) reset;
    bool test __attribute__((unused)) = true;

    std::shared_ptr<B> b(new B);
    //std::shared_ptr<A> a(b);
    //VERIFY( a.use_count() == 2 );
    //VERIFY( A::ctor_count == 1 );
    //VERIFY( A::dtor_count == 0 );
    //VERIFY( B::ctor_count == 1 );
    //VERIFY( B::dtor_count == 0 );

    return 0;
  }

  int
    test19()
  {
    using namespace a2;
    reset_count_struct __attribute__((unused)) reset;
    bool test __attribute__((unused)) = true;

    //std::shared_ptr<B> b(new B, &deleter);
    //std::shared_ptr<A> a(b);
    //VERIFY( a.use_count() == 2 );
    //VERIFY( A::ctor_count == 1 );
    //VERIFY( A::dtor_count == 0 );
    //VERIFY( B::ctor_count == 1 );
    //VERIFY( B::dtor_count == 0 );

    return 0;
  }

  // 20.6.6.2.1 shared_ptr constructors [util.smartptr.shared.const]

  // Default construction
  int
    test20()
  {
    bool test __attribute__((unused)) = true;

    std::shared_ptr<a3::A> a;
    VERIFY( a.get() == 0 );

    return 0;
  }

  // 20.6.6.2.1 shared_ptr constructors [util.smartptr.shared.const]

  // Construction from pointer

  int
    test21()
  {
    using namespace a4;
    bool test __attribute__((unused)) = true;

    A * const a = 0;
    std::shared_ptr<A> p(a);
    VERIFY( p.get() == 0 );
    VERIFY( p.use_count() == 1 );

    return 0;
  }

  int
    test22()
  {
    using namespace a4;
    bool test __attribute__((unused)) = true;

    A * const a = new A;
    std::shared_ptr<A> p(a);
    VERIFY( p.get() == a );
    VERIFY( p.use_count() == 1 );

    return 0;
  }


  int
    test23()
  {
    using namespace a4;
    bool test __attribute__((unused)) = true;

    B * const b = new B;
    std::shared_ptr<A> p(b);
    VERIFY( p.get() == b );
    VERIFY( p.use_count() == 1 );

    return 0;
  }

  // 20.7.12.2.1 shared_ptr constructors [util.smartptr.shared.const]

  // Construction from lvalue unique_ptr
  int
    test24()
  {
    using namespace a3;
    bool test __attribute__((unused)) = true;

    std::unique_ptr<A> a;
    std::shared_ptr<A> p(a); // { dg-error "used here" }

    return 0;
  }

  // 20.6.6.2.1 shared_ptr constructors [util.smartptr.shared.const]

  // Construction from weak_ptr
  int
    test25()
  {
    using namespace a3;
    bool test __attribute__((unused)) = true;

#if 0
    A * const a = new A;
    std::shared_ptr<A> a1(a);
    std::weak_ptr<A> wa(a1);
    std::shared_ptr<A> a2(wa);
    VERIFY( a2.get() == a );
    VERIFY( a2.use_count() == wa.use_count() );
#endif
    return 0;
  }

#ifdef _CPPUNWIND
  // 20.6.6.2.1 shared_ptr constructors [util.smartptr.shared.const]

  // Construction from expired weak_ptr
  int
    test26()
  {
    using namespace a3;
    bool test __attribute__((unused)) = true;

    std::shared_ptr<A> a1(new A);
    std::weak_ptr<A> wa(a1);
    a1.reset();
    VERIFY( wa.expired() );
    try
    {
      std::shared_ptr<A> a2(wa);
    }
    catch (const std::bad_weak_ptr&)
    {
      // Expected.
      __throw_exception_again;
    }
    catch (...)
    {
      // Failed.
    }

    return 0;
  }
#endif



  void main()
  {
    test01();
    test02(); // fail
    test03();
    test04();
    
    #ifdef NTL__CXX
    test05();
    #endif
    //test06(); // shared_ptr<void>
    test07();
    test08();
    test09();
    test10();
    test11();
    test12();
    test14();
    test15();
    test16();
    //test17(); // access violation
    test18();
    test19();
    test20();
    test21(); // fail
    test22();
    test23();
    test24();
    test25();
#ifdef _CPPUNWIND
    test26();
#endif
    // TODO: "libstdc++-v3\testsuite\20_util\shared_ptr\creation" и далее

  }
}