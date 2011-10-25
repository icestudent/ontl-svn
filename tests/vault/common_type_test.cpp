#include <cstddef>

template<typename T, typename _ = int>
struct xshow_type
{
  char __[sizeof(_) == sizeof(char)];
};


template<int value, typename _ = int>
struct xshow_value
{
  char __[sizeof(_) == sizeof(char)];
};
template<__int64 value, typename _ = int>
struct xshow_value64
{
  char _[sizeof(_) == sizeof(char)];
};

template<typename Type, size_t Size, size_t Align, typename _ = int>
struct ShowTypeInfo 
{
  char __[sizeof(_) == sizeof(char)];
};

#define SHOW_TYPE(T) ShowTypeInfo<T,sizeof(T),__alignof(T)> _Join(__x_show_type_info, __COUNTER__);

#define SHOWT(T) xshow_type<T> _Join(__x_show_type, __COUNTER__);
#define SHOWV(V) xshow_value<V> _Join(__x_show_value, __COUNTER__);
#define SHOWLV(V) xshow_value64<V> _Join(__x_show_value64, __COUNTER__);
#define SIZEOF(T) SHOWV(sizeof(T));

template<typename T>
inline void __show_type(T)
{
  using namespace std;
  SHOWT(T);
}
template<typename T>
inline void __show_type()
{
  using namespace std;
  SHOWT(T);
}


#define VERIFY(e) static_assert(e, #e)


#pragma warning(disable:4101 4189)

#include <type_traits>
using namespace std;

namespace {

#ifdef _MSC_VER
# define __attribute__(x)
#endif

typedef int test_type1;
typedef int& test_type2;
typedef double test_type3;
typedef float test_type4;

template struct common_type<test_type1>;
template struct common_type<test_type1, test_type2>;
template struct common_type<test_type1, test_type2, test_type3>;
template struct common_type<test_type1, test_type2, test_type3, test_type4>;


#define JOIN( X, Y ) DO_JOIN( X, Y )
#define DO_JOIN( X, Y ) DO_JOIN2(X,Y)
#define DO_JOIN2( X, Y ) X##Y

#define COMMON_TYPE_TEST_1(type1, uid) \
  typedef common_type<type1>::type JOIN(test_t,uid); \
  VERIFY( (is_same<JOIN(test_t,uid), JOIN(test_t,uid)>::value) ); \
  typedef common_type<const type1>::type JOIN(test_t,JOIN(uid,c)); \
  VERIFY( (is_same<JOIN(test_t,JOIN(uid,c)), \
  JOIN(test_t,JOIN(uid,c))>::value) ); \
  typedef common_type<volatile type1>::type JOIN(test_t,JOIN(uid,v)); \
  VERIFY( (is_same<JOIN(test_t,JOIN(uid,v)), \
  JOIN(test_t,JOIN(uid,v))>::value) ); \
  typedef common_type<const volatile type1>::type JOIN(test_t,JOIN(uid,cv)); \
  VERIFY( (is_same<JOIN(test_t,JOIN(uid,cv)), \
  JOIN(test_t,JOIN(uid,cv))>::value) ); \
  typedef common_type<type1 &>::type JOIN(test_t,JOIN(uid,l)); \
  VERIFY( (is_same<JOIN(test_t,JOIN(uid,l)), \
  JOIN(test_t,JOIN(uid,l))>::value) ); \
  typedef common_type<const type1 &>::type JOIN(test_t,JOIN(uid,lc)); \
  VERIFY( (is_same<JOIN(test_t,JOIN(uid,lc)), \
  JOIN(test_t,JOIN(uid,lc))>::value) ); \
  typedef common_type<volatile type1 &>::type JOIN(test_t,JOIN(uid,lv)); \
  VERIFY( (is_same<JOIN(test_t,JOIN(uid,lv)), \
  JOIN(test_t,JOIN(uid,lv))>::value) ); \
  typedef common_type<const volatile type1 &>::type JOIN(test_t,JOIN(uid,lcv)); \
  VERIFY( (is_same<JOIN(test_t,JOIN(uid,lcv)), \
  JOIN(test_t,JOIN(uid,lcv))>::value) );

#ifdef NTL__CXX_RV_WHATIS
  typedef common_type<type1 &&>::type JOIN(test_t,JOIN(uid,r)); \
  VERIFY( (is_same<JOIN(test_t,JOIN(uid,r)), \
  JOIN(test_t,JOIN(uid,r))>::value) ); \
  typedef common_type<const type1 &&>::type JOIN(test_t,JOIN(uid,rc)); \
  VERIFY( (is_same<JOIN(test_t,JOIN(uid,rc)), \
  JOIN(test_t,JOIN(uid,rc))>::value) ); \
  typedef common_type<volatile type1 &&>::type JOIN(test_t,JOIN(uid,rv)); \
  VERIFY( (is_same<JOIN(test_t,JOIN(uid,rv)), \
  JOIN(test_t,JOIN(uid,rv))>::value) ); \
  typedef common_type<const volatile type1 &&>::type JOIN(test_t,JOIN(uid,rcv)); \
  VERIFY( (is_same<JOIN(test_t,JOIN(uid,rcv)), \
  JOIN(test_t,JOIN(uid,rcv))>::value) )
#endif

struct A { };
struct B : A { };

void test01()
{
  bool test __attribute__((unused)) = true;
  using std::common_type;
  using std::is_same;

  // Positive tests.
  COMMON_TYPE_TEST_1(int, 1);
  COMMON_TYPE_TEST_1(double, 2);
  COMMON_TYPE_TEST_1(A, 3);
  COMMON_TYPE_TEST_1(B, 4);
}

#define COMMON_TYPE_TEST_2_IMPL(type1, type2, expected, uid) \
  typedef common_type<type1, type2>::type  	JOIN(JOIN(test, uid),_t1); \
  typedef common_type<type2, type1>::type  	JOIN(JOIN(test, uid),_t2); \
  static_assert( (is_same<JOIN(JOIN(test, uid),_t1), expected>::value), "must be \"" #expected "\" when common_type<" #type1 ", " #type2 ">"); \
  static_assert( (is_same<JOIN(JOIN(test, uid),_t2), expected>::value), "must be \"" #expected "\" when common_type<" #type1 ", " #type2 ">" );

#define NO_CV

#ifdef NTL__CXX_RV

#define COMMON_TYPE_TEST_2(cv_qual, type1, type2, expected, uid) \
  COMMON_TYPE_TEST_2_IMPL(cv_qual type1, type2, expected, uid) \
  COMMON_TYPE_TEST_2_IMPL(cv_qual type1 &, type2, expected, JOIN(uid,l))\
  COMMON_TYPE_TEST_2_IMPL(cv_qual type1 &&, type2, expected, JOIN(uid,r))

#else

#define COMMON_TYPE_TEST_2(cv_qual, type1, type2, expected, uid) \
  COMMON_TYPE_TEST_2_IMPL(cv_qual type1, type2, expected, uid) \
  COMMON_TYPE_TEST_2_IMPL(cv_qual type1 &, type2, expected, JOIN(uid,l))
  //COMMON_TYPE_TEST_2_IMPL(cv_qual type1 &&, type2, expected, JOIN(uid,r))

#endif

#define COMMON_TYPE_TEST_ALL_2(type1, type2, expected, uid) \
  COMMON_TYPE_TEST_2(NO_CV, type1, type2, expected, JOIN(uid,0)) \
  COMMON_TYPE_TEST_2(const, type1, type2, expected, JOIN(uid,1)) \
  COMMON_TYPE_TEST_2(volatile, type1, type2, expected, JOIN(uid,2)) \
  COMMON_TYPE_TEST_2(const volatile, type1, type2, expected, JOIN(uid,3))

//template<class T, class U>
//struct xcommon_type
//{
//  //typedef T type;
//  typedef typename remove_reference<typename remove_cv<
//    typename conditional<(sizeof(T) < sizeof(U)), T, U>::type
//    >::type>::type type;
//};
#ifdef NTL__CXX_TYPEOF
template<class T, class U = void>
struct xcommon_type
{
  typedef decltype(true ? declval<T>() : declval<U>()) type;
};

template<class T>
struct xcommon_type<T, void> { typedef T type; };
#endif

template<class T> T rv() { return T(); }

template<class T, class U>
struct ycommon_type
{
  //template<class T1>
  //static char test(T1);
  //template<class T2>
  //static int  test(T2);
  //static char test(T);
  //static int  test(U);

  //static typename add_rvalue_reference<T>::type __t();
  //static typename add_rvalue_reference<U>::type __u();

  //static T __t();
  //static U __u();

  //static const size_t __r = sizeof(
  //    //test(true ? declval<T>() : declval<U>())
  //    test(true ? __t() : __u())
  //  );
  //typedef typename conditional<__r == sizeof(char), T, U>::type type;
  typedef typename std::__::select_type<T,U>::type type;

};

template<class T, class U, class R>
static void yselect(R, const char* t, const char* u) {
  SHOWT(R);
  //cout << "select<" << t << ',' << u << ">::" << (is_same<R,T>::value ? t : u) << endl;
}

#define CT(T,U) yselect<T,U>(true ? rv<T>() : rv<U>(), #T, #U)


#define STATIC_ASSERT2(e) typedef char _Join(_STATIC_ASSERT_, __COUNTER__) [(e)?1:-1]
void test02()
{
  bool test __attribute__((unused)) = true;
  using std::common_type;
  using std::is_same;
  using std::is_const;

  //typedef xcommon_type<int&>::type ct0;
  //typedef xcommon_type<int&, const int>::type ct1;
  //typedef xcommon_type<int, int&>::type ct2;

  //__show_type<xcommon_type<int, int&>::type>();

  //CT(const int, int);

  //VERIFY((is_same< xcommon_type<const int, int>::type, int >::value));
  //VERIFY((is_same< ycommon_type<const int, int>::type, int >::value));
  
  //COMMON_TYPE_TEST_2_IMPL(const int,int,int,1);

#if 0
  VERIFY((is_same<ct1, int>::value));
  VERIFY((is_same<ct2, int>::value));

  typedef xcommon_type<int, int>::type   xct0;
  typedef xcommon_type<int &, int>::type xct1;
  typedef xcommon_type<int, int&>::type  xct2;
  VERIFY((is_same<xct0, int>::value));
  VERIFY((is_same<xct1, int>::value));
  VERIFY((is_same<xct2, int>::value));

  typedef common_type<const int, int>::type xct4;
  VERIFY((is_same<xct4, int>::value));
#endif
#if 0
  //COMMON_TYPE_TEST_2(const , int, int, int, 0);
  COMMON_TYPE_TEST_ALL_2(int, int, int, 1);
  COMMON_TYPE_TEST_ALL_2(int, double, double, 2);
  COMMON_TYPE_TEST_2(NO_CV, int, double, double, 6);
  COMMON_TYPE_TEST_2(NO_CV, A, A, A, 3);
  COMMON_TYPE_TEST_2(const, A, A, A, 4);
  COMMON_TYPE_TEST_2(NO_CV, B, A, A, 5);
#endif

}


template<typename T, typename U>
class promote
{
  static bool true_or_false();
  static T makeT();
  static U makeU();

  template<typename Result>
  static void test(Result)
  {
    //typedef Result result_type;
    xshow_type<Result, 10 + is_const<Result>::value> x;
  }

  typedef typename remove_reference<typename remove_cv<T>::type>::type rawT;
  typedef typename remove_reference<typename remove_cv<U>::type>::type rawU;

public:
  /**
   *	@brief правила продвижения
   *  1) если типы равны (без квалификаторов), то выбирается один из них (также без квалификатора [см. show()])
   *  2) если один тип конвертируется в другой, то выбираем тот, в который можно сконвертировать
   *  3) иначе результат - void
   **/
  typedef typename common_type<T,U>::type type;

  static void show()
  {
    test(true_or_false() ? makeT() : makeU());
    //test(makeT() + makeU());
  }

  static void select()
  {
    SHOWT(type);
  }

};

template<class T, class U>
struct xselect
{
  typedef typename remove_cv<typename remove_reference<T>::type>::type rawT;
  typedef typename remove_cv<typename remove_reference<U>::type>::type rawU;
  
  typedef 
    typename conditional<is_same<rawT, rawU>::value, rawT,
      typename conditional<is_arithmetic<rawT>::value && is_arithmetic<rawU>::value, typename conditional<(sizeof(T) < sizeof(U)), rawU, rawT>::type,
        typename conditional<is_convertible<rawT,rawU>::value, rawU,
          typename conditional<is_convertible<rawU,rawT>::value, rawT, void
                              >::type
                            >::type
                          >::type
                        >::type type;
};

void promo_test()
{
  //promote<int, int>::select();                // int
  //promote<const int, int>::select();          // const int
  //promote<int, const int>::select();          // const int
  //promote<volatile int, int>::select();       // int
  //promote<const volatile int, int>::select(); // int
  //promote<int&, int>::select();               // int
  //promote<int&, int&>::select();              // int
  //promote<int, int*>::select();               // void

  //xshow_type<const int> z;

  //promote<const int, int >::select();              // int
  typedef xselect<const volatile int&, int>::type sic;
  typedef xselect<short, int>::type sic2;
  //xshow_type<xselect<const volatile int&, int>::type, sizeof(sic)> x;
  //xshow_type<sic2, sizeof(sic2)> x2;
  //promote<int, short>::select();              // int
  //promote<int, double>::select();              // int
  //promote<short int, int>::select();              // int
  //promote<double, int>::select();              // int
  //promote<const A, const A>::select();              // int

  //promote<const int, short>::show();                // int
  //promote<int, int>::show();                // int
  //promote<const int, int>::show();          // const int
  //promote<volatile int, int>::show();       // int
  //promote<const volatile int, int>::show(); // int
  //promote<int&, int>::show();               // int
  //promote<int&, int&>::show();              // int
  //promote<int, int*>::show();               // void

  //promote<short, int>::show();              // int
  //promote<A, const A>::show();              // int
  //xshow_type<char, __is_convertible_to(int, short)> x;
}


} // namespace