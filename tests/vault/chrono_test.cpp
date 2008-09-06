// common tests part
#include <cassert>

#define __attribute__(x)
#pragma warning(disable:4101)
#define VERIFY(e) assert(e)

#include <ratio>

template<typename T>
struct xshow_type
{
  char _[0];
};

template<std::ratio_t N, std::ratio_t D>
struct xshow_type<std::ratio<N,D>>
{
  char _[0];
};

template<int value>
struct xshow_value
{
  char _[0];
};

template<__int64 value>
struct xshow_value64
{
  char _[0];
};

#define SHOWT(T) xshow_type<T> _Join(__x_show_type, __COUNTER__);
#define SHOWV(V) xshow_value<V> _Join(__x_show_value, __COUNTER__);
#define SHOWLV(V) xshow_value64<V> _Join(__x_show_value64, __COUNTER__);

#pragma warning(disable:4101)

// chrono
#include <chrono>

typedef std::common_type<std::time_t, int64_t>::type ct;
typedef std::common_type<std::time_t, std::int64_t, std::intmax_t>::type ctt;

template class std::chrono::time_point<std::chrono::system_clock>;
template class std::chrono::duration<int>;
template class std::chrono::duration<float, std::ratio<2,3>>;

using namespace ntl;
using namespace ntl::nt;
using namespace std;


#include <ctime>

namespace chrono_test {

void test00()
{
  systime_t sysnow = query_system_time();
  time_t tnow = time();

  chrono::system_clock::time_point snow = chrono::system_clock::now();
  time_t stnow = chrono::system_clock::to_time_t(snow);
  chrono::system_clock::time_point snow2 = chrono::system_clock::from_time_t(stnow);

  int64_t x1 = snow2.time_since_epoch().count(),
    x2 = snow.time_since_epoch().count();
  assert(stnow == tnow);

  chrono::system_clock::time_point xtp(snow.time_since_epoch() / chrono::system_clock::period::den * chrono::system_clock::period::den);
  assert(snow2 == xtp);

  using namespace std::chrono;

  system_clock::time_point t1 = system_clock::now();
  bool is_monotonic = system_clock::is_monotonic;
  std::time_t t2 = system_clock::to_time_t(t1);
  system_clock::time_point t3 = system_clock::from_time_t(t2);
}

void test01()
{
  clock_t program_time = clock();
  assert(program_time > 0);
}

// 20.8.4.1 time_point constructors [time.point.cons]
void test02()
{
  bool test __attribute__((unused)) = true;
  using namespace std::chrono;

  time_point<system_clock> t1;
  VERIFY(t1.time_since_epoch() == system_clock::duration::zero());

  time_point<monotonic_clock> t2;
  VERIFY(t2.time_since_epoch() == monotonic_clock::duration::zero());

  //time_point<high_resolution_clock> t3;
  //VERIFY(t3.time_since_epoch() == high_resolution_clock::duration::zero());
}

// 20.8.4.3 time_point arithmetic [time.point.arithmetic]
void
test03()
{
  bool test __attribute__((unused)) = true;
  using namespace std::chrono;

  time_point<system_clock> t1, t2;
  t1 += seconds(1);
  VERIFY(t2.time_since_epoch() + seconds(1) == t1.time_since_epoch());

  t1 -= std::chrono::seconds(1);
  VERIFY(t2.time_since_epoch() == t1.time_since_epoch());
}

// 20.8.4.5 time_point non-member arithmetic [time.point.nonmember]
void
test04()
{
  bool test __attribute__((unused)) = true;
  using namespace std::chrono;

  time_point<system_clock> t1;
  time_point<system_clock> t2(t1 + seconds(1));
  VERIFY(t2.time_since_epoch() == t1.time_since_epoch() + seconds(1));

  time_point<system_clock> t3(seconds(1) + t1);
  VERIFY(t3.time_since_epoch() == t1.time_since_epoch() + seconds(1));

  time_point<system_clock> t4(seconds(1));
  time_point<system_clock> t5(seconds(2));

  time_point<system_clock> t6(t5 - seconds(1));
  VERIFY(t6.time_since_epoch() == t4.time_since_epoch());

  time_point<system_clock> t7(t5 - t4);
  VERIFY(t7.time_since_epoch() == t4.time_since_epoch());
}

// 20.8.4.6 time_point comparisons [time.point.comparisons]
void
test05()
{
  bool test __attribute__((unused)) = true;
  using namespace std::chrono;

  time_point<system_clock> t1(seconds(1));
  time_point<system_clock> t2(seconds(1));
  time_point<system_clock> t3(seconds(2));

  VERIFY(t1 == t2);
  VERIFY(t1 != t3);
  VERIFY(t1 < t3);
  VERIFY(t1 <= t3);
  VERIFY(t1 <= t2);
  VERIFY(t3 > t1);
  VERIFY(t3 >= t1);
  VERIFY(t2 >= t1);  
}

// 20.8.3.3 duration arithmetic [time.duration.arithmetic] (unary member ops)
void
test06()
{
  bool test __attribute__((unused)) = true;
  using namespace std::chrono;

  duration<int> d0(3);
  duration<int> d1 = -d0;
  VERIFY(d0.count() == 3);
  VERIFY(d1.count() == -3);

  duration<int> d2 = (+d0);
  VERIFY(d2.count() == 3);

  duration<int> d3(++d2);
  VERIFY(d2.count() == 4);
  VERIFY(d3.count() == 4);

  int x = 4;
  int y(x++);

  duration<int> d4(d3++);
  VERIFY(d3.count() == 5);
  VERIFY(d4.count() == 4);

  duration<int> d5(--d4);
  VERIFY(d4.count() == 3);
  VERIFY(d5.count() == 3);

  duration<int> d6(d5--);
  VERIFY(d5.count() == 2);
  VERIFY(d6.count() == 3);
}

// 20.8.3.3 duration arithmetic [time.duration.arithmetic] (binary member ops)
void
test07()
{
  bool test __attribute__((unused)) = true;
  using namespace std::chrono;

  duration<int> d7(3);
  duration<int> d8(9);
  d7 += d8;
  VERIFY(d7.count() == 12);
  VERIFY(d8.count() == 9);

  duration<int> d9(3);
  duration<int> d10(9);
  d9 -= d10;
  VERIFY(d9.count() == -6);
  VERIFY(d10.count() == 9);

  duration<int> d11(9);
  int i = 3;
  d11 *= i;
  VERIFY(d11.count() == 27);

  duration<int> d12(12);  
  d12 /= i;
  VERIFY(d12.count() == 4);
}

// 20.8.3.5 duration non-member arithmetic [time.duration.nonmember]
void
test08()
{
  bool test __attribute__((unused)) = true;
  using namespace std::chrono;

  duration<int> d0(12);
  duration<int> d1(3);
  int i = 3;

  duration<int> d2 = d0 + d1;
  VERIFY(d2.count() == 15);

  duration<int> d3 = d0 - d1;
  VERIFY(d3.count() == 9);

  duration<int> d4 = d0 * i;
  VERIFY(d4.count() == 36);

  duration<int> d5 = i * d0;
  VERIFY(d5.count() == 36);

  duration<int> d6 = d0 / i;
  VERIFY(d6.count() == 4);

  int j = d0 / d1;
  VERIFY(j == 4);
}

// 20.8.3.6 duration comparisons [time.duration.comparisons]
void
test09()
{
  bool test __attribute__((unused)) = true;
  using namespace std::chrono;

  duration<int> d0(12);
  duration<int> d1(3);
  duration<int> d2(3);

  VERIFY(d1 < d0);  
  VERIFY(d0 > d1);

  VERIFY(d0 != d1);
  VERIFY(d1 == d2);

  VERIFY(d1 <= d2);
  VERIFY(d1 >= d2);

  VERIFY(d1 <= d0);
  VERIFY(d0 >= d1);  
}
}

template<typename T>
struct type_emulator
{
  type_emulator()
    : i(T(0)) { }

  type_emulator(T j)
    : i(j) { }

  type_emulator(const type_emulator& e)
    : i(e.i) { }

  type_emulator&
    operator*=(type_emulator a)
  {
    i *= a.i;
    return *this;
  }

  type_emulator&
    operator+=(type_emulator a)
  {
    i += a.i;
    return *this;
  }

  operator T ()
  { return i; }

  T i;
};

template<typename T>
bool
operator==(type_emulator<T> a, type_emulator<T> b)
{ return a.i == b.i; }

template<typename T>
bool
operator<(type_emulator<T> a, type_emulator<T> b)
{ return a.i < b.i; }

template<typename T>
type_emulator<T>
operator+(type_emulator<T> a, type_emulator<T> b)
{ return a += b; }

template<typename T>
type_emulator<T>
operator*(type_emulator<T> a, type_emulator<T> b)
{ return a *= b; }

namespace std
{
  template<typename T, typename U>
  struct common_type<type_emulator<T>, U>
  { typedef typename common_type<T,U>::type type; };

  template<typename T, typename U>
  struct common_type<U, type_emulator<T>>
  { typedef typename common_type<U,T>::type type; };

  template<typename T, typename U>
  struct common_type<type_emulator<T>, type_emulator<U>>
  { typedef typename common_type<T,U>::type type; };

  namespace chrono
  {    
    template<typename T>
    struct treat_as_floating_point<type_emulator<T>>
      : is_floating_point<T>
    { };
  }

  // arithmetic
  template<typename T>
  struct is_arithmetic<type_emulator<T>>: true_type
  {};
}

typedef type_emulator<int> int_emulator;
typedef type_emulator<double> dbl_emulator;

namespace chrono_test{  
// 20.8.3.1 duration constructors [time.duration.cons]
void
test10()
{
  bool test __attribute__((unused)) = true;
  using std::chrono::duration;

  duration<int> d0;
  VERIFY(d0.count() == static_cast<duration<int>::rep>(0));

  int r = 3;
  duration<int> d1(r);
  VERIFY(d1.count() == static_cast<duration<int>::rep>(r));

  double s = 8.0;
  duration<double> d2(s);
  VERIFY(d2.count() == static_cast<duration<double>::rep>(s));

  int_emulator ie(3);
  duration<int_emulator> d3(ie);
  VERIFY(d3.count() == static_cast<duration<int_emulator>::rep>(ie));

  dbl_emulator de(4.0);
  duration<dbl_emulator> d4(de);
  VERIFY(d4.count() == static_cast<duration<dbl_emulator>::rep>(de));
}

void
test11()
{
  //std::chrono::duration<int> d1(1.0); // should fail
}

void
test12()
{
  using namespace std::chrono;

  duration<int, std::micro> d2(8);
  duration<int, std::milli> d2_copy(d2);
}

#if 0
template<typename T>
struct type_emulator
{
  type_emulator() : i(T(0)) { }
  type_emulator(T j) : i(j) { }
  type_emulator(const type_emulator& e) : i(e.i) { }

  type_emulator& operator*=(type_emulator a)
  { i *= a.i; return *this; }

  type_emulator& operator+=(type_emulator a)
  { i += a.i; return *this; }

  operator T () { return i; }
  T i;
};

template<typename T>
bool operator==(type_emulator<T> a, type_emulator<T> b)
{ return a.i == b.i; }

template<typename T>
bool operator<(type_emulator<T> a, type_emulator<T> b)
{ return a.i < b.i; }

template<typename T>
type_emulator<T> operator+(type_emulator<T> a, type_emulator<T> b)
{ return a += b; }

template<typename T>
type_emulator<T> operator*(type_emulator<T> a, type_emulator<T> b)
{ return a *= b; }

namespace std
{
  template<typename T, typename U>
  struct common_type<type_emulator<T>, U>
  { typedef typename common_type<T,U>::type type; };

  template<typename T, typename U>
  struct common_type<U, type_emulator<T>>
  { typedef typename common_type<U,T>::type type; };

  template<typename T, typename U>
  struct common_type<type_emulator<T>, type_emulator<U>>
  { typedef typename common_type<T,U>::type type; };

  namespace chrono
  {    
    template<typename T>
    struct treat_as_floating_point<type_emulator<T>>
      : is_floating_point<T>
    { };
  }
}

typedef type_emulator<int> int_emulator;
typedef type_emulator<double> dbl_emulator;
#endif
// 20.8.3.1 duration constructors [time.duration.cons]
void
test13()
{
  bool test __attribute__((unused)) = true;
  using namespace std::chrono;

  duration<int> d0(3);
  duration<int> d0_copy(d0);
  VERIFY(d0_copy.count() == d0.count());

  duration<int, std::milli> d1(5);
  duration<int, std::micro> d1_copy(d1);
  VERIFY(d1.count() * 1000 == d1_copy.count());

  duration<double, std::micro> d2(8.0);
  duration<double, std::milli> d2_copy(d2);
  VERIFY(d2.count() == d2_copy.count() * 1000.0);

  duration<int_emulator, std::milli> d3(5);
  duration<int_emulator, std::micro> d3_copy(d3);
  VERIFY(d3.count() * 1000 == d3_copy.count());

  duration<dbl_emulator, std::micro> d4(5.0);
  duration<dbl_emulator, std::milli> d4_copy(d4);
  VERIFY(d4.count() == d4_copy.count() * dbl_emulator(1000.0));
}

void test14()
{
  // Check if rep is a duration type
  typedef std::chrono::duration<int> rep_type;
  typedef std::chrono::duration<rep_type> test_type;
  //test_type d; // is duration an arithmetic type?
}

void test15()
{
  // Check if period is a ratio
  typedef int rep_type;
  typedef int period_type;
  typedef std::chrono::duration<rep_type, period_type> test_type;
  //test_type d;  // should fail
}

void test16()
{
  // Check if period is positive
  typedef int rep_type;
  typedef std::ratio<-1> period_type;
  typedef std::chrono::duration<rep_type, period_type> test_type;
  //test_type d;  // should fail
}

void main()
{
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
  test13();
  test14();
  test15();
  test16();
}

}