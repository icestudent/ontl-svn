// common tests part
#include <cassert>

#define __attribute__(x)
#pragma warning(disable:4101 4189)
#define VERIFY(e) assert(e)

#ifdef __ICL
# pragma warning(disable:383)
#endif

#include <deque>
#include <queue>
#include <list>
#include <stack>
#include <vector>

#pragma warning(disable:4661) // no definition for allocator-suffix construction

template class std::vector<int>;
template class std::queue<int>;
template class std::deque<int>;
template class std::deque<int, std::allocator<char> >;
template class std::queue<int, std::list<int> >;

struct user_type {};

namespace std 
{
  template<typename Allocator>
  class deque<user_type, Allocator> {};
}

struct copycounter
{
  static int copycount;
  copycounter(){copycount++;}
  copycounter(int c){copycount = c;}
  copycounter(const copycounter&){copycount++;}
  copycounter& operator=(const copycounter&){copycount++; return *this;}
  ~copycounter(){copycount--;}

  static void reset() { copycount = 0; }
};

int copycounter::copycount = 0;

#define assert_count(n) assert(copycounter::copycount == n)

#include <cstdlib>
#include <ctime>
#include <nt/basedef.hxx>
#include <nt/debug.hxx>
namespace dbg = ntl::nt::dbg;

#if 0
#include <nt/heap.hxx>

template<class T> 
class checked_allocator:
  public std::allocator<T>
{
  typedef ntl::nt::heap heap;
  heap h;
  static const size_t sig_size = 32;
public:
  template<class U> struct rebind { typedef checked_allocator<U> other; };

  checked_allocator()
    :h(heap::no_serialize|heap::create_align_16|heap::tail_checking_enabled|heap::free_checking_enabled,heap::generate_exceptions)
  {}
  checked_allocator(const checked_allocator&)
    :h(heap::no_serialize|heap::create_align_16|heap::tail_checking_enabled|heap::free_checking_enabled,heap::generate_exceptions)
  {}

  T* allocate(size_type n)
  {
    h.validate(heap::no_serialize);
    n *= sizeof(T);
    void* p = h.alloc(sig_size*2 + n, heap::no_serialize|heap::zero_memory);
    h.validate(heap::no_serialize);
    std::memset(p, 0xBB, sig_size);
    std::memset((char*)p + sig_size + n, 0xEE, sig_size);
    return reinterpret_cast<T*>( (char*)p + sig_size );
  }
  void deallocate(pointer p, size_type n)
  {
    const char* pc = (char*)p;
    n*= sizeof(T);
    static uint8_t sig1[sig_size], sig2[sig_size];
    if(!sig1[0]){
      std::memset(sig1, 0xbb, sig_size);
      std::memset(sig2, 0xee, sig_size);
    }
    assert(std::memcmp(pc-sig_size, sig1, sig_size) == 0);
    assert(std::memcmp(pc+n, sig2, sig_size) == 0);
    h.validate(heap::no_serialize);
    h.free(pc-sig_size, heap::no_serialize);
    h.validate(heap::no_serialize);
  }
  void construct(pointer p)
  {
    h.validate(heap::no_serialize);
    __super::construct(p);
    h.validate(heap::no_serialize);
  }
#ifdef NTL__CXX_RV
  template<class Arg>
  void construct(pointer p, Arg&& val)
  {
    h.validate(heap::no_serialize);
    __super::construct(p, std::forward<Arg>(val));
    h.validate(heap::no_serialize);
  }
#else
  template<class Arg>
  void construct(pointer p, const Arg& val)
  {
    h.validate(heap::no_serialize);
    __super::construct(p, val);
    h.validate(heap::no_serialize);
  }
  template<class Arg>
  void construct(pointer p, Arg& val)
  {
    h.validate(heap::no_serialize);
    __super::construct(p, val);
    h.validate(heap::no_serialize);
  }
#endif
  void destroy(const pointer p)
  {
    h.validate(heap::no_serialize);
    __super::destroy(p);
    h.validate(heap::no_serialize);
  }
};
#else

template<class T> 
class checked_allocator: public std::allocator<T>
{

};
#endif

namespace
{
  using namespace std;

  typedef std::deque<int, checked_allocator<int> > ideque;
  void test01()
  {
    {
      ideque d0, d3(3), d7(15);
      d7.assign(3,13);
      d0.resize(2);
      d0.resize(8, 4);
      d0.resize(4);
      d0.clear();
      d0.shrink_to_fit();
    }

    ideque d1(1);
    assert(d1.size() == 1 && !d1.empty());
    ideque d2;
    d2.push_back(1);
    d2.push_back(2);
    d2.push_front(0);
    d2.push_front(-1);
    while(!d2.empty())
      d2.pop_back();
    assert(d2.size() == 0);

    {
      ideque d1(12), d2(1);
      d1 = d2;
      d2.clear();
    }
  }

  struct A
  {
    explicit A(int) { }
  };

  void requirements()
  {
#ifdef CT_FAIL
    std::deque<A> d;
    std::deque<std::deque<int> > d2(10, 1);
    std::deque<int> v(10, 1);
    std::deque<std::deque<std::pair<char, char> > > d3('a', 'b');
    std::deque<A> d4;
    d.assign(10, 1);
    d4.insert(d.begin(), 10, 1);
#else
    std::deque<int> v(10, 1);
    std::vector<int> vv(10,1);

    std::deque<A> d;
    std::deque<std::deque<int> > d2(10, deque<int>(1));
    std::deque<A> d4;
    d.assign(10, A(1));
    d4.insert(d.begin(), 10, A(1));
#endif
  }

  void
    test02()
  {
    bool test __attribute__((unused)) = true;

    std::deque<int> d(7);
    VERIFY( d.cbegin() == d.begin() );
    VERIFY( d.cend() == d.end() );
    VERIFY( d.crbegin() == d.rbegin() );
    VERIFY( d.crend() == d.rend() );
    VERIFY( d.cbegin() != d.cend() );
    VERIFY( d.crbegin() != d.crend() );  
  }

  namespace N
  {
    struct X { };

    template<typename T>
    X operator+(T, std::size_t)
    { return X(); }

    template<typename T>
    X operator-(T, T)
    { return X(); }
  }


  int test03()
  {
    std::deque<N::X> d(5);
    const std::deque<N::X> e(1);

    {
      ideque ide(10,1);
      ideque ide2(ide.cbegin(), ide.cend());
      std::deque<int> di(5), de(1);
#ifdef NTL__CXX_LAMBDA
      int _cnt = 0;
      std::generate(di.begin(), di.end(), [&_cnt](){ return ++_cnt;} );
#endif
      di.erase(di.begin());
      di.resize(1);
      di.assign(1, -1);
      di.insert(di.begin(), 1);
      di.insert(di.begin(), 1, 2);
      di.insert(di.begin(), de.cbegin(), de.cend());
      di = de;
      de.empty();
    }
    

    d[0];
    e[0];
    d.size();
    d.erase(d.begin());
    d.resize(1);
    d.assign(1, N::X());
    d.insert(d.begin(), N::X());
    d.insert(d.begin(), 1, N::X());
    d.insert(d.begin(), e.begin(), e.end());
    d = e;

    return 0;
  }

  void test04()
  {
    bool test __attribute__((unused)) = true;

    std::deque<int> d;

    VERIFY( d.max_size() == d.get_allocator().max_size() );
  }

  void test05()
  {
#if STLX__USE_EXCEPTIONS
    bool test __attribute__((unused)) = true;
    using namespace std;

    deque<int> d;

    try
    {
      d.resize(size_t(-1));
    }
    catch(const std::length_error&)
    {
      VERIFY( true );
    }
    catch(...)
    {
      VERIFY( false );
    }
#endif
  }


  void
    test06()
  {
    bool test __attribute__((unused)) = true;

#if 0
    std::deque<copycounter> a;
    copycounter::copycount = 0;
    a.resize(10);
    a.resize(98);
    a.resize(99);
    a.resize(100);
#ifndef _GLIBCXX_DEBUG
    VERIFY( copycounter::copycount == 100 );
#else
    VERIFY( copycounter::copycount == 100 + 4 );
#endif
    a.resize(99);
    a.resize(0);
#ifndef _GLIBCXX_DEBUG
    VERIFY( copycounter::copycount == 100 );
#else
    VERIFY( copycounter::copycount == 100 + 6 );
#endif
    a.resize(100);
#ifndef _GLIBCXX_DEBUG
    VERIFY( copycounter::copycount == 200 );
#else
    VERIFY( copycounter::copycount == 200 + 7 );
#endif
    a.clear();
#ifndef _GLIBCXX_DEBUG
    VERIFY( copycounter::copycount == 200 );
#else
    VERIFY( copycounter::copycount == 200 + 7 );
#endif
#endif
  }

  void test07()
  {
    bool test __attribute__((unused)) = true;
    using namespace std;

    int data3[1000];
    fill(data3, data3 + 1000, 3);

    int data5[1000];
    fill(data5, data5 + 1000, 5);

    for (deque<int>::size_type i = 0; i < 1000; ++i)
    {
      deque<int> d(rand() % 500, 1);
      d.assign(i, i % 2 ? 3 : 5);

      VERIFY( d.size() == i );
      VERIFY( equal(d.begin(), d.end(), i % 2 ? data3 : data5) );
    }
  }


  void
    test08()
  {
    typedef std::deque<copycounter>   gdeque;
    copycounter::copycount = 0;
    assert_count (0);
    {
      gdeque   d(10);
      assert_count (10);
    }
    assert_count (0);
  }


  namespace T1 {
  const int  A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  const int A1[] = {0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  const int A2[] = {0, 2, 3, 4, 10, 11, 12, 13, 14, 15};
  const int A3[] = {0, 2, 3, 4, 10, 11};
  const int A4[] = {4, 10, 11};
  const int A5[] = {4, 10};
  const unsigned  N = sizeof(A)  / sizeof(int);
  const unsigned N1 = sizeof(A1) / sizeof(int);
  const unsigned N2 = sizeof(A2) / sizeof(int);
  const unsigned N3 = sizeof(A3) / sizeof(int);
  const unsigned N4 = sizeof(A4) / sizeof(int);
  const unsigned N5 = sizeof(A5) / sizeof(int);

  void
    test01()
  {
    bool test __attribute__((unused)) = true;

    typedef std::deque<int>                deque_type;
    typedef deque_type::iterator        iterator_type;

    deque_type v(A, A + N);

    iterator_type it1 = v.erase(v.begin() + 1);
    VERIFY( it1 == v.begin() + 1 );
    VERIFY( v.size() == N1 );
    VERIFY( std::equal(v.begin(), v.end(), A1) );

    iterator_type it2 = v.erase(v.begin() + 4, v.begin() + 9);
    VERIFY( it2 == v.begin() + 4 );
    VERIFY( v.size() == N2 );
    VERIFY( std::equal(v.begin(), v.end(), A2) );

    iterator_type it3 = v.erase(v.begin() + 6, v.end());
    VERIFY( it3 == v.begin() + 6 );
    VERIFY( v.size() == N3 );
    VERIFY( std::equal(v.begin(), v.end(), A3) );

    iterator_type it4 = v.erase(v.begin(), v.begin() + 3);
    VERIFY( it4 == v.begin() );
    VERIFY( v.size() == N4 );
    VERIFY( std::equal(v.begin(), v.end(), A4) );

    iterator_type it5 = v.erase(v.begin() + 2);
    VERIFY( it5 == v.begin() + 2 );
    VERIFY( v.size() == N5 );
    VERIFY( std::equal(v.begin(), v.end(), A5) );

    iterator_type it6 = v.erase(v.begin(), v.end());
    VERIFY( it6 == v.begin() );
    VERIFY( v.empty() );
  }

  void
    test02()
  {
    bool test __attribute__((unused)) = true;

    typedef std::deque<int> inner_deque;
    typedef std::deque<inner_deque, checked_allocator<inner_deque> > deque_type;
    typedef deque_type::iterator        iterator_type;

    deque_type v, v1, v2, v3, v4, v5;
    for (unsigned i = 0; i < N; ++i)
      v.push_back(std::deque<int>(1, A[i]));
    for (unsigned i = 0; i < N1; ++i)
      v1.push_back(std::deque<int>(1, A1[i]));
    for (unsigned i = 0; i < N2; ++i)
      v2.push_back(std::deque<int>(1, A2[i]));
    for (unsigned i = 0; i < N3; ++i)
      v3.push_back(std::deque<int>(1, A3[i]));
    for (unsigned i = 0; i < N4; ++i)
      v4.push_back(std::deque<int>(1, A4[i]));
    for (unsigned i = 0; i < N5; ++i)
      v5.push_back(std::deque<int>(1, A5[i]));

    iterator_type it1 = v.erase(v.begin() + 1);
    VERIFY( it1 == v.begin() + 1 );
    VERIFY( v.size() == N1 );
    VERIFY( std::equal(v.begin(), v.end(), v1.begin()) );

    iterator_type it2 = v.erase(v.begin() + 4, v.begin() + 9);
    VERIFY( it2 == v.begin() + 4 );
    VERIFY( v.size() == N2 );
    VERIFY( std::equal(v.begin(), v.end(), v2.begin()) );

    iterator_type it3 = v.erase(v.begin() + 6, v.end());
    VERIFY( it3 == v.begin() + 6 );
    VERIFY( v.size() == N3 );
    VERIFY( std::equal(v.begin(), v.end(), v3.begin()) );

    iterator_type it4 = v.erase(v.begin(), v.begin() + 3);
    VERIFY( it4 == v.begin() );
    VERIFY( v.size() == N4 );
    VERIFY( std::equal(v.begin(), v.end(), v4.begin()) );

    iterator_type it5 = v.erase(v.begin() + 2);
    VERIFY( it5 == v.begin() + 2 );
    VERIFY( v.size() == N5 );
    VERIFY( std::equal(v.begin(), v.end(), v5.begin()) );

    iterator_type it6 = v.erase(v.begin(), v.end());
    VERIFY( it6 == v.begin() );
    VERIFY( v.empty() );
  }
  }

  namespace T2 {
    const int  A[] = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
      13, 14, 15};
    const int A0[] = {-5, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    const int A1[] = {-5, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    const int A2[] = {-5, 0, 1, 2, 8, 9, 10, 11, 12, 13, 14, 15};
    const int A3[] = {-5, 0, 1, 2, 8, 9, 10, 11};
    const int A4[] = {2, 8, 9, 10, 11};
    const int A5[] = {2, 8, 10, 11};
    const int A6[] = {2, 8, 10};
    const unsigned  N = sizeof(A)  / sizeof(int);
    const unsigned N0 = sizeof(A0) / sizeof(int);
    const unsigned N1 = sizeof(A1) / sizeof(int);
    const unsigned N2 = sizeof(A2) / sizeof(int);
    const unsigned N3 = sizeof(A3) / sizeof(int);
    const unsigned N4 = sizeof(A4) / sizeof(int);
    const unsigned N5 = sizeof(A5) / sizeof(int);
    const unsigned N6 = sizeof(A6) / sizeof(int);

    template<int Size>
    class My_class
    {
      int data;
      double dummy[Size];

    public:
      My_class(int num)
        : data(num) { }

      operator int() const
      { return data; }
    };

    template<typename T>
    void
      test01()
    {
      bool test __attribute__((unused)) = true;

      typedef std::deque<T, checked_allocator<T>> deque_type;
      typedef typename deque_type::iterator      iterator_type;

      deque_type v(A, A + N);

      iterator_type it0 = v.erase(v.begin() + 1, v.begin() + 4);
      VERIFY( it0 == v.begin() + 1 );
      VERIFY( v.size() == N0 );
      VERIFY( std::equal(v.begin(), v.end(), A0) );

      iterator_type it1 = v.erase(v.begin() + 1);
      VERIFY( it1 == v.begin() + 1 );
      VERIFY( v.size() == N1 );
      VERIFY( std::equal(v.begin(), v.end(), A1) );

      iterator_type it2 = v.erase(v.begin() + 4, v.begin() + 9);
      VERIFY( it2 == v.begin() + 4 );
      VERIFY( v.size() == N2 );
      VERIFY( std::equal(v.begin(), v.end(), A2) );

      iterator_type it3 = v.erase(v.begin() + 8, v.end());
      VERIFY( it3 == v.begin() + 8 );
      VERIFY( v.size() == N3 );
      VERIFY( std::equal(v.begin(), v.end(), A3) );

      iterator_type it4 = v.erase(v.begin(), v.begin() + 3);
      VERIFY( it4 == v.begin() );
      VERIFY( v.size() == N4 );
      VERIFY( std::equal(v.begin(), v.end(), A4) );

      iterator_type it5 = v.erase(v.begin() + 2);
      VERIFY( it5 == v.begin() + 2 );
      VERIFY( v.size() == N5 );
      VERIFY( std::equal(v.begin(), v.end(), A5) );

      iterator_type it6 = v.erase(v.begin() + 3, v.end());
      VERIFY( it6 == v.begin() + 3 );
      VERIFY( v.size() == N6 );
      VERIFY( std::equal(v.begin(), v.end(), A6) );

      iterator_type it7 = v.erase(v.begin(), v.end());
      VERIFY( it7 == v.begin() );
      VERIFY( v.empty() );
    }

    int main()
    {
      test01<My_class<1> >();
      test01<My_class<8> >();
      test01<My_class<32> >();
      return 0;
    }
  }

  void test09()
  {
    T1::test01();
    T1::test02();
    T2::main();
  }
#if 0
  void erase(size_t num_elm, size_t elm_strt, size_t elm_end)
  {
    bool test __attribute__((unused)) = true;
    using __gnu_test::copy_tracker;
    using __gnu_test::assignment_operator;

    std::deque<copy_tracker> x(num_elm);
    copy_tracker::reset();

    x.erase(x.begin() + elm_strt, x.begin() + elm_end);

    const size_t min_num_cpy = std::min(elm_strt, num_elm - elm_end);
    VERIFY( assignment_operator::count() == min_num_cpy );
  }


  void test10()
  {
    for (size_t num_elm = 0; num_elm <= 10; ++num_elm)
      for (size_t elm_strt = 0; elm_strt <= num_elm; ++elm_strt)
        for (size_t elm_end = elm_strt; elm_end <= num_elm; ++elm_end)
          erase(num_elm, elm_strt, elm_end);
  }
#else
  void test10(){}
#endif

  void
    test11()
  {
    bool test __attribute__((unused)) = true;

#if 0
    std::deque<copycounter> a(40);
    copycounter::copycount = 0;

    a.erase(a.begin() + 20);
    VERIFY( copycounter::copycount == 0 );

    a.erase(a.begin());
    VERIFY( copycounter::copycount == 0 );

    a.erase(a.end() - 1);
    VERIFY( copycounter::copycount == 0 );

    a.erase(a.begin() + 10, a.end() - 10);
    VERIFY( copycounter::copycount == 0 );

    a.erase(a.begin(), a.begin() + 5);
    VERIFY( copycounter::copycount == 0 );

    a.erase(a.end() - 5, a.end());
    VERIFY( copycounter::copycount == 0 );

    a.erase(a.begin(), a.end());
    VERIFY( copycounter::copycount == 0 );
#endif
  }

}

struct TT { int i; };

int swap_calls;


namespace std
{
#ifdef NTL__CXX_RV
  template<> 
  void deque<TT, std::allocator<TT> >::swap(deque<TT, std::allocator<TT> > &&)
  { ++swap_calls; }
#else
  template<> 
  void deque<TT, std::allocator<TT> >::swap(deque<TT, std::allocator<TT> >&)
  { ++swap_calls; }
#endif
}

namespace {
  // Should use deque specialization for swap.
  void test12()
  {
    bool test __attribute__((unused)) = true;
    std::deque<TT> A;
    std::deque<TT> B;
    swap_calls = 0;
    std::swap(A, B);
    VERIFY(1 == swap_calls);
  }

  // Should use deque specialization for swap.
  void test13()
  {
    bool test __attribute__((unused)) = true;
    using namespace std;
    deque<TT> A;
    deque<TT> B;
    swap_calls = 0;
    swap(A, B);
    VERIFY(1 == swap_calls);
  }

  // Test deque::push_back makes no unneeded copies.
  void
    test14()
  {
    bool test __attribute__((unused)) = true;

#if 0
    std::deque<copycounter> a;
    copycounter c(1);
    copycounter::copycount = 0;
    for(int i = 0; i < 1000; ++i)
      a.push_back(c);
    VERIFY(copycounter::copycount == 1000);
#endif
  }

  // Test deque::push_front makes no unneeded copies.
  void
    test15()
  {
    bool test __attribute__((unused)) = true;

#if 0
    std::deque<copycounter> a;
    copycounter c(1);
    copycounter::copycount = 0;
    for(int i = 0; i < 1000; ++i)
      a.push_front(c);
    VERIFY(copycounter::copycount == 1000);
#endif
  }

  // Test deque::insert makes no unneeded copies.
  void
    test16()
  {
    bool test __attribute__((unused)) = true;

    std::deque<copycounter> a(1000);
    copycounter c(1);
    copycounter::copycount = 0;
    a.insert(a.begin(),c);
    a.insert(a.end(),c);
    for(int i = 0; i < 500; ++i)
      a.insert(a.begin() + i, c);
    //VERIFY(copycounter::copycount == 502);
  }

  // Test deque::insert(iterator, count, value) makes no unneeded copies
  // when it has to also reallocate the deque's internal buffer.
  void
    test17()
  {
    bool test __attribute__((unused)) = true;

#if 0
    copycounter c(1);
    std::deque<copycounter> a(10, c);
    copycounter::copycount = 0;
    a.insert(a.begin(), 20, c);
    VERIFY(copycounter::copycount == 20);
    a.insert(a.end(), 50, c);
    VERIFY(copycounter::copycount == 70);
    // NOTE : These values are each one higher than might be expected, as
    // deque::insert(iterator, count, value) copies the value it is given
    // when it has to move elements in the deque in case that value is
    // in the deque.

    // Near the start
    a.insert(a.begin() + 10, 100, c);
    VERIFY(copycounter::copycount == 170 + 1);
    // Near the end
    a.insert(a.end() - 10, 1000, c);
    VERIFY(copycounter::copycount == 1170 + 2);
#endif
  }

  // Test deque::insert(iterator, count, value) makes no unneeded copies
  // when it doesn't have to reallocate the deque's internal buffer.
  void
    test18()
  {
    bool test __attribute__((unused)) = true;

#if 0
    copycounter c(1);
    std::deque<copycounter> a(10, c);
    copycounter::copycount = 0;
    //a.reserve(1000);
    a.insert(a.begin(), 20, c);
    VERIFY(copycounter::copycount == 20 );
    a.insert(a.end(), 50, c);
    VERIFY(copycounter::copycount == 70 );

    // NOTE : These values are each one higher than might be expected, as
    // deque::insert(iterator, count, value) copies the value it is given
    // when it has to move elements in the deque in case that value is
    // in the deque.
    // Near the start
    a.insert(a.begin() + 10, 100, c);
    VERIFY(copycounter::copycount == 170 + 1);
    // Near the end
    a.insert(a.end() - 10, 200, c);
    VERIFY(copycounter::copycount == 370 + 2);
#endif
  }

  void test19()
  {
    bool test __attribute__((unused)) = true;

    std::deque<int> d(2);       
    typedef std::deque<int>::iterator iter;         
    typedef std::deque<int>::const_iterator constiter;

    iter beg = d.begin();               
    iter end = d.end();
    constiter constbeg = d.begin();               
    constiter constend = d.end();

    VERIFY( beg == constbeg );
    VERIFY( constend == end );

    VERIFY( beg != constend );
    VERIFY( constbeg != end );

    VERIFY( beg < constend );
    VERIFY( constbeg < end );

    VERIFY( end > constbeg );
    VERIFY( constend > beg );

    VERIFY( end >= constend );
    VERIFY( constbeg >= beg );

    VERIFY( beg <= constbeg );
    VERIFY( constend <= end );
  }

  // libstdc++/7186
  void test20()
  {
    bool test __attribute__((unused)) = true;

    std::deque<int> d(2);       
    typedef std::deque<int>::iterator iter;         
    typedef std::deque<int>::const_iterator constiter;

    iter beg = d.begin();
    iter end = d.end();
    constiter constbeg = d.begin();
    constiter constend = d.end();

    VERIFY( beg - constbeg == 0 );
    VERIFY( constend - end == 0 );

    VERIFY( end - constbeg > 0 );
    VERIFY( constend - beg > 0 );
  }


  int test21()
  {
    bool test __attribute__((unused)) = true;

    std::deque<int> a,b;
    a.push_back(1);
    b = std::move(a);
    VERIFY( b.size() == 1 && b[0] == 1 && a.size() == 0 );

    std::deque<int> c(std::move(b));
    VERIFY( c.size() == 1 && c[0] == 1 );
    VERIFY( b.size() == 0 );
    return 0;
  }

  template<int sig = 1>
  struct tracer_t
  {
    static size_t& refc()
    {
      static size_t refc_ = 0;
      return refc_;
    }
    tracer_t()
    {
      ref = no = ++refc();
      dbg::trace.printf(__func__" [%d]\n", no);
    }
    ~tracer_t()
    {
      refc()--;
      dbg::trace.printf(__func__" [%d-%d]\n", no,ref);
    }
#if 1
    tracer_t(const tracer_t& r)
    {
      ref = ++refc();
      no = r.no;
      dbg::trace.printf(__func__" [%d-%d]\n", no,ref);
    }
    tracer_t& operator=(const tracer_t& r)
    {
      no = r.no;
      dbg::trace.printf(__func__" [%d-%d]\n", no,ref);
      return *this;
    }
#endif
#ifdef NTL__CXX_RV
    tracer_t(tracer_t&& r)
    {
      ref = ++refc();
      no = r.no;
      r.no = 0;
      dbg::trace.printf(__func__" [%d-%d]\n", no,ref);
    }
    tracer_t& operator=(tracer_t&& r)
    {
      no = r.no;
      r.no = 0;
      dbg::trace.printf(__func__" [%d-%d]\n", no,ref);
      return *this;
    }
#endif

    typedef int result_type;
    int operator()() const
    {
      dbg::trace.printf(__func__" [%d-%d]\n", no,ref);
      return 7;
    }

  private:
    size_t no, ref;
  };

  void test22()
  {
    typedef tracer_t<> tracer;

    std::deque<tracer> d(2);

  }
}

//#include <spp/loop.hxx>
#include <stlx/ext/pp/macro_repeat.hxx>

void deque_test()
{
  test22();
  // fails: 06, 11, 14, 15, 16, 17, 18 (copycount problems)
  //debug_abort();
  #define macro(n, aux) aux ## n();
  TTL_REPEAT(9,macro,macro,test0);
  TTL_REPEAT(9,macro,macro,test1);
  TTL_REPEAT(1,macro,macro,test2);
  test10();
  test20();
}
