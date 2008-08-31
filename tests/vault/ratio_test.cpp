/// this tests are taked from the GCC
//#define SMALL_RATIO
#include <ratio>

namespace {

using namespace std;

#pragma warning(disable:4101) // unreferenced local variable
#define __attribute__(x)


template<ratio_t N, ratio_t D>
struct xshow_ratio
{
  char _[0];
};

template<typename T>
struct xshow_type
{
  char _[0];
};

template<ratio_t N, ratio_t D>
struct xshow_type<std::ratio<N,D>>
{
  typedef std::ratio<N,D> ratio;
  xshow_ratio<ratio::num, ratio::den> _;
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

#define SHOWR(T) xshow_type<T> _Join(__x_show_type, __COUNTER__);
#define SHOWT(T) xshow_type<T> _Join(__x_show_type, __COUNTER__);
#define SHOWV(V) xshow_value<V> _Join(__x_show_value, __COUNTER__);
#define SHOWLV(V) xshow_value64<V> _Join(__x_show_value64, __COUNTER__);

typedef ratio<5,- 3>   five_thirds;       // five_thirds::num == 5, five_thirds::den == 3
typedef ratio<25, 15> also_five_thirds;  // also_five_thirds::num == 5, also_five_thirds::den == 3


//SHOWT(one);
//SHOWT(five_thirds);

#define VERIFY(e) STATIC_ASSERT(e)

typedef ratio_divide<five_thirds, also_five_thirds>::type one;  // one::num == 1, one::den == 1

typedef std::ratio<1920,1200> hd;
SHOWR(hd);

std::ratio<1,3> r0;
std::ratio<2,6> r1;  
std::ratio<2,-6> r2;
std::ratio<-2,6> r3;

typedef std::ratio<1,3> one_third;
typedef std::ratio<2,-6> also_one_third;

//SHOWT(one_third);

//SHOWT(also_one_third);

VERIFY( r0.num == 1 );
VERIFY( r0.den == 3 );
VERIFY( r1.num == 1 );
VERIFY( r1.den == 3 );
VERIFY( r2.num == -1 );
VERIFY( r2.den == 3 );

VERIFY( r1.num == r0.num );
VERIFY( r1.den == r0.den );  
VERIFY( r2.num == -r0.num );
VERIFY( r2.den == r0.den ); 
VERIFY( r3.num == -r0.num );
VERIFY( r3.den == r0.den );   

VERIFY((std::ratio_equal<std::ratio<2,6>, std::ratio<1,3>>::value == 1));
VERIFY(( std::ratio_equal<std::ratio<2,6>, std::ratio<1,4>>::value == 0 ));

VERIFY( (std::ratio_not_equal<std::ratio<2,6>, 
       std::ratio<1,3>>::value == 0) );
VERIFY( (std::ratio_not_equal<std::ratio<2,6>, 
       std::ratio<1,4>>::value == 1) ); 

VERIFY( (std::ratio_less<std::ratio<1,3>, std::ratio<1,5>>::value == 0) );
VERIFY( (std::ratio_less<std::ratio<1,4>, std::ratio<1,3>>::value == 1) );
VERIFY( (std::ratio_less<std::ratio<-1,3>, std::ratio<1,3>>::value == 1) );

VERIFY( (std::ratio_less<std::ratio<1,3>, std::ratio<1,4>>::value == 0) );
VERIFY( (std::ratio_less<std::ratio<1,3>, std::ratio<-1,3>>::value == 0) );

VERIFY( (std::ratio_less_equal<std::ratio<-1,3>, 
       std::ratio<-1,3>>::value == 1) );
VERIFY( ( std::ratio_less_equal<std::ratio<1,4>, 
       std::ratio<1,3>>::value == 1) );

VERIFY( (std::ratio_less_equal<std::ratio<1,4>, 
       std::ratio<-1,3>>::value == 0) );
VERIFY( (std::ratio_less_equal<std::ratio<1,3>, 
       std::ratio<-1,3>>::value == 0) );

VERIFY( (std::ratio_greater<std::ratio<1,3>, std::ratio<1,4>>::value == 1) );
VERIFY( (std::ratio_greater<std::ratio<1,3>, std::ratio<-1,3>>::value == 1) );

VERIFY( (std::ratio_greater<std::ratio<1,4>, std::ratio<1,3>>::value == 0) );
VERIFY( (std::ratio_greater<std::ratio<-1,3>, std::ratio<1,3>>::value == 0) );

VERIFY( (std::ratio_greater_equal<std::ratio<1,3>, 
       std::ratio<1,3>>::value == 1) );
VERIFY( (std::ratio_greater_equal<std::ratio<1,3>, 
       std::ratio<-1,3>>::value == 1) );

VERIFY( (std::ratio_greater_equal<std::ratio<-1,3>, 
       std::ratio<1,3>>::value == 0) );
VERIFY( (std::ratio_greater_equal<std::ratio<1,4>, 
       std::ratio<1,3>>::value == 0) );


static const std::intmax_t M = INTMAX_MAX;

// No overflow with same denominator
VERIFY( (std::ratio_less<std::ratio<M - 2, M>,
       std::ratio<M - 1, M>>::value == 1) );

VERIFY( (std::ratio_less<std::ratio<M - 1, M>,
       std::ratio<M - 2, M>>::value == 0) );

// No overflow if signs differ
VERIFY( (std::ratio_less<std::ratio<-M, M - 2>,
       std::ratio<M - 1, M - 2>>::value == 1) );

VERIFY( (std::ratio_less<std::ratio<M - 1, M - 2>,
       std::ratio<-M, M - 2>>::value == 0) ); 

typedef std::ratio<-1, 3> rs13;
typedef std::ratio_subtract<std::ratio<2,3>, std::ratio<1,3> >::type rs11;
//SHOWT(rs11);

typedef std::ratio_add<std::ratio<-2,3>, std::ratio<1,3> >::type rs12;
//SHOWT(rs12);


typedef std::ratio<3,8> r38;
typedef std::ratio<-5,12>r512;

typedef
std::ratio_add<r38, r512>::type rs_t; rs_t rs;
//SHOWT(r38);
//SHOWT(r512);
//SHOWT(rs_t);
VERIFY( rs.num == -1 );
VERIFY( rs.den == 24 ); 

std::ratio_add<std::ratio<3,8>, std::ratio<5,12>>::type r;

VERIFY( r.num == 19 );
VERIFY( r.den == 24 ); 


//SHOWT(r512);

std::ratio_subtract<std::ratio<3,8>, std::ratio<5,12>>::type rx;

VERIFY( rx.num == -1 );
VERIFY( rx.den == 24 ); 

std::ratio_subtract<std::ratio<3,8>, std::ratio<5,12>>::type r22;

VERIFY( r22.num == -1 );
VERIFY( r22.den == 24 ); 

std::ratio_multiply<std::ratio<3,8>, std::ratio<5,12>>::type r4;

VERIFY( r4.num == 5 );
VERIFY( r4.den == 32 ); 


std::ratio_divide<std::ratio<3,8>, std::ratio<5,12>>::type r5;

VERIFY( r5.num == 9 );
VERIFY( r5.den == 10 ); 

typedef std::ratio<1, INTMAX_MAX> one_over_max;
typedef std::ratio<2, INTMAX_MAX> two_over_max;
typedef std::ratio<INTMAX_MAX, 1> max_over_one;
typedef std::ratio<INTMAX_MAX, 2> max_over_two; 

void x1(){

std::ratio_add<one_over_max, one_over_max>::type r1;

VERIFY( r1.num == two_over_max::num);
VERIFY( r1.den == two_over_max::den);

std::ratio_add<
std::ratio<INTMAX_MAX / 2, INTMAX_MAX / 2>,
std::ratio<INTMAX_MAX / 2 , INTMAX_MAX / 2 + 1>>::type r2;

VERIFY( r2.num == INTMAX_MAX );
VERIFY( r2.den == (INTMAX_MAX / 2) + 1 ); 
}

void
test02()
{  
  bool test __attribute__((unused)) = true;

  std::ratio_subtract<one_over_max, one_over_max>::type r1;

  VERIFY( r1.num == 0);
  VERIFY( r1.den == 1);

  std::ratio_subtract<
    std::ratio<INTMAX_MAX / 2, INTMAX_MAX / 2>,
    std::ratio<INTMAX_MAX / 2 , INTMAX_MAX / 2 + 1>>::type r2;

  VERIFY( r2.num == 1 );
  VERIFY( r2.den == (INTMAX_MAX / 2) + 1 );
} 

void
test03()
{
  bool test __attribute__((unused)) = true;

  std::ratio_multiply<
    std::ratio<2, INTMAX_MAX>,
    std::ratio<INTMAX_MAX, 2>>::type r1;

  VERIFY( r1.num == 1 );
  VERIFY( r1.den == 1 );

  std::ratio_multiply<
    std::ratio<INTMAX_MAX, 2>,
    std::ratio<2 , INTMAX_MAX - 1>>::type r2;

  VERIFY( r2.num == INTMAX_MAX );
  VERIFY( r2.den == INTMAX_MAX - 1 );
}

void
test04()
{  
  bool test __attribute__((unused)) = true;

  std::ratio_divide<
    std::ratio<INTMAX_MAX, 2>,
    std::ratio<INTMAX_MAX, 2>>::type r1;

  VERIFY( r1.num == 1 );
  VERIFY( r1.den == 1 );

  std::ratio_divide<
    std::ratio<INTMAX_MAX-1, 2>,
    std::ratio<INTMAX_MAX, 2>>::type r2;

  VERIFY( r2.num == INTMAX_MAX - 1 );
  VERIFY( r2.den == INTMAX_MAX );
}
} // namespace