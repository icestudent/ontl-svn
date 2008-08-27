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

#define VERIFY(e) STATIC_ASSERT(e)

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

#ifdef NTL__CXX
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

#define COMMON_TYPE_TEST_2_IMPL(type1, type2, type3, uid) \
  typedef common_type<type1, type2>::type  	JOIN(JOIN(test, uid),_t1); \
  typedef common_type<type2, type1>::type  	JOIN(JOIN(test, uid),_t2); \
  VERIFY( (is_same<JOIN(JOIN(test, uid),_t1), type3>::value) ); \
  VERIFY( (is_same<JOIN(JOIN(test, uid),_t2), type3>::value) )

#define NO_CV

#define COMMON_TYPE_TEST_2(cv_qual, type1, type2, type3, uid) \
  COMMON_TYPE_TEST_2_IMPL(cv_qual type1, type2, type3, uid); \
  COMMON_TYPE_TEST_2_IMPL(cv_qual type1 &, type2, type3, JOIN(uid,l)); 
  //COMMON_TYPE_TEST_2_IMPL(cv_qual type1 &&, type2, type3, JOIN(uid,r))

#define COMMON_TYPE_TEST_ALL_2(type1, type2, type3, uid) \
  COMMON_TYPE_TEST_2(NO_CV, type1, type2, type3, uid); \
  COMMON_TYPE_TEST_2(const, type1, type2, type3, uid); \
  COMMON_TYPE_TEST_2(volatile, type1, type2, type3, uid); \
  COMMON_TYPE_TEST_2(const volatile, type1, type2, type3, uid)

template<class T, class U>
struct xcommon_type
{
  //typedef T type;
  typedef typename remove_reference<typename remove_cv<
    typename conditional<(sizeof(T) < sizeof(U)), T, U>::type
    >::type>::type type;
};

void test02()
{
  bool test __attribute__((unused)) = true;
  using std::common_type;
  using std::is_same;

  typedef common_type<int&>::type ct0;
  typedef common_type<int&, int>::type ct1;
  typedef common_type<int, int&>::type ct2;
  VERIFY((is_same<ct0, int>::value));
  VERIFY((is_same<ct1, int>::value));
  VERIFY((is_same<ct2, int>::value));

  typedef xcommon_type<int, int>::type   xct0;
  typedef xcommon_type<int &, int>::type xct1;
  typedef xcommon_type<int, int&>::type  xct2;
  VERIFY((is_same<xct0, int>::value));
  VERIFY((is_same<xct1, int>::value));
  VERIFY((is_same<xct2, int>::value));

#if 0
  COMMON_TYPE_TEST_2(NO_CV, int, int, int, 1);
  COMMON_TYPE_TEST_ALL_2(int, int, int, 1);
  COMMON_TYPE_TEST_ALL_2(int, double, double, 2);
  COMMON_TYPE_TEST_2(NO_CV, A, A, A, 3);
  COMMON_TYPE_TEST_2(const, A, A, const A, 4);
  COMMON_TYPE_TEST_2(NO_CV, B, A, A, 5);  
#endif
}

} // namespace