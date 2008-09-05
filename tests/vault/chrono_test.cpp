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

#include <stlx/chrono.hxx>

typedef std::common_type<std::time_t, int64_t>::type ct;
typedef std::common_type<std::time_t, std::int64_t, std::intmax_t>::type ctt;

#include <cassert>


using namespace ntl;
using namespace ntl::nt;
using namespace std;


#include <ctime>

void test2()
{
  clock_t program_time = clock();
  assert(program_time > 0);
}

namespace chrono_test {
void main()
{
  test2();
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

  int $ = 1, $$ = 2;
  int $$$ = $ + $$;

}

}