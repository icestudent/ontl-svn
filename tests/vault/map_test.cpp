// common tests part
#include <cassert>

#define __attribute__(x)
#pragma warning(disable:4101 4189)
#define VERIFY(e) assert(e)

#include <nt/debug.hxx>
namespace dbg = ntl::nt::dbg;

// tests

#include <map>

template class std::map<int, int>;
template class std::map<int, double>;
template class std::map<int, double, std::less<int>, std::allocator<char> >;


struct user_type {};

namespace std 
{
  template<typename Data, typename Allocator>
  class map<user_type, Data, Allocator> {};
}

namespace {

void test01()
{
  bool test __attribute__((unused)) = true;

  std::map<int, int> m;
  m[1] = 1;
#if 1
  VERIFY( m.cbegin() == m.begin() );
  VERIFY( m.cend() == m.end() );
  VERIFY( m.crbegin() == m.rbegin() );
  VERIFY( m.crend() == m.rend() );
  VERIFY( m.cbegin() != m.cend() );
  VERIFY( m.crbegin() != m.crend() );  
#endif
}

void test02()
{
  bool test __attribute__((unused)) = true;

  std::map<int, int> m;

  VERIFY( m.max_size() == m.get_allocator().max_size() );
}

#ifdef _CPPUNWIND
void test03() 
{ 
  bool test __attribute__((unused)) = true;
  typedef std::map<int, double> map_type;

  {
    map_type m;
    m[0] = 1.5;

    double& rd = m.at(0);
    VERIFY( rd == 1.5 );
    try
    {
      m.at(1);
    }
    catch(std::out_of_range& obj)
    {
      // Expected.
    }
    catch(...)
    {
      // Failed.
      throw;
    }    
  }

  {
    map_type m;
    m[1] = 2.5;
    const map_type cm(m);

    const double& crd = cm.at(1);
    VERIFY( crd == 2.5 );
    try
    {
      cm.at(0);
    }
    catch(std::out_of_range& obj)
    {
      // Expected.
    }
    catch(...)
    {
      // Failed.
      throw;
    }    
  }
}
#endif

void test04()
{
  bool test __attribute__((unused)) = true;
  typedef std::map<int, int>   Map;
  Map             M;
  Map::iterator   hint;

  hint = M.insert(Map::value_type(7, 0)).first;

  M.insert(hint, Map::value_type(8, 1));
  M.insert(M.begin(), Map::value_type(9, 2));

#if 0
  // The tree's __rb_verify() member must be exposed in map<> before this
  // will even compile.  It's good test to see that "missing" entries are
  // in fact present in the {map,tree}, but in the wrong place.
  if (0)
  {
    Map::iterator  i = M.begin();
    while (i != M.end()) {
      std::cerr << '(' << i->first << ',' << i->second << ")\n";
      ++i;
    }
    std::cerr << "tree internal verify: "
      << std::boolalpha << M.__rb_verify() << "\n";
  }
#endif

  VERIFY ( M.find(7) != M.end() );
  VERIFY ( M.find(8) != M.end() );
  VERIFY ( M.find(9) != M.end() );
}

void test05()
{
  using std::map;
  bool test __attribute__((unused)) = true;

  map<int, float> m1, m2;

  m1[3] = 3.0f;
  m1[11] = -67.0f;

  m2.insert(m1.begin(), m1.end());

  VERIFY( m1 == m2 );
}
#if 0

struct T { int i; };

// T must be LessThanComparable to pass concept-checks
bool operator<(T l, T r) { return l.i < r.i; }

int swap_calls;

namespace std
{
  template<> 
  void 
    map<T, int>::swap(map<T, int>&) 
  { ++swap_calls; }
}

// Should use map specialization for swap.
void test06()
{
  bool test __attribute__((unused)) = true;
  std::map<T, int> A;
  std::map<T, int> B;
  swap_calls = 0;
  std::swap(A, B);
  VERIFY(1 == swap_calls);
}

// Should use map specialization for swap.
void test07()
{
  bool test __attribute__((unused)) = true;
  using namespace std;
  map<T, int> A;
  map<T, int> B;
  swap_calls = 0;
  swap(A, B);
  VERIFY(1 == swap_calls);
}
#endif
// uneq_allocator as a non-empty allocator.
void test08()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

  typedef pair<const char, int> my_pair;
  //typedef __gnu_test::uneq_allocator<my_pair> my_alloc;
  typedef std::allocator<my_pair> my_alloc;
  typedef map<char, int, less<char>, my_alloc> my_map;

  const char title01[] = "Rivers of sand";
  const char title02[] = "Concret PH";
  const char title03[] = "Sonatas and Interludes for Prepared Piano";
  const char title04[] = "never as tired as when i'm waking up";

  const size_t N1 = sizeof(title01);
  const size_t N2 = sizeof(title02);
  const size_t N3 = sizeof(title03);
  const size_t N4 = sizeof(title04);

  map<char, int> map01_ref;
  for (size_t i = 0; i < N1; ++i)
    map01_ref.insert(my_pair(title01[i], i));
  map<char, int> map02_ref;
  for (size_t i = 0; i < N2; ++i)
    map02_ref.insert(my_pair(title02[i], i));
  map<char, int> map03_ref;
  for (size_t i = 0; i < N3; ++i)
    map03_ref.insert(my_pair(title03[i], i));
  map<char, int> map04_ref;
  for (size_t i = 0; i < N4; ++i)
    map04_ref.insert(my_pair(title04[i], i));

  my_map::size_type size01, size02;

  my_alloc alloc01/*(1)*/;

  my_map map01(less<char>(), alloc01);
  size01 = map01.size();
  my_map map02(less<char>(), alloc01);
  size02 = map02.size();

  map01.swap(map02);
  VERIFY( map01.size() == size02 );
  VERIFY( map01.empty() );
  VERIFY( map02.size() == size01 );
  VERIFY( map02.empty() );

  my_map map03(less<char>(), alloc01);
  size01 = map03.size();
  my_map map04(map02_ref.begin(), map02_ref.end(), less<char>(), alloc01);
  size02 = map04.size();

  map03.swap(map04);
  VERIFY( map03.size() == size02 );
  VERIFY( equal(map03.begin(), map03.end(), map02_ref.begin()) );
  VERIFY( map04.size() == size01 );
  VERIFY( map04.empty() );

  my_map map05(map01_ref.begin(), map01_ref.end(), less<char>(), alloc01);
  size01 = map05.size();
  my_map map06(map02_ref.begin(), map02_ref.end(), less<char>(), alloc01);
  size02 = map06.size();

  map05.swap(map06);
  VERIFY( map05.size() == size02 );
  VERIFY( equal(map05.begin(), map05.end(), map02_ref.begin()) );
  VERIFY( map06.size() == size01 );
  VERIFY( equal(map06.begin(), map06.end(), map01_ref.begin()) );

  my_map map07(map01_ref.begin(), map01_ref.end(), less<char>(), alloc01);
  size01 = map07.size();
  my_map map08(map03_ref.begin(), map03_ref.end(), less<char>(), alloc01);
  size02 = map08.size();

  map07.swap(map08);
  VERIFY( map07.size() == size02 );
  VERIFY( equal(map07.begin(), map07.end(), map03_ref.begin()) );
  VERIFY( map08.size() == size01 );
  VERIFY( equal(map08.begin(), map08.end(), map01_ref.begin()) );

  my_map map09(map03_ref.begin(), map03_ref.end(), less<char>(), alloc01);
  size01 = map09.size();
  my_map map10(map04_ref.begin(), map04_ref.end(), less<char>(), alloc01);
  size02 = map10.size();

  map09.swap(map10);
  VERIFY( map09.size() == size02 );
  VERIFY( equal(map09.begin(), map09.end(), map04_ref.begin()) );
  VERIFY( map10.size() == size01 );
  VERIFY( equal(map10.begin(), map10.end(), map03_ref.begin()) );

  my_map map11(map04_ref.begin(), map04_ref.end(), less<char>(), alloc01);
  size01 = map11.size();
  my_map map12(map01_ref.begin(), map01_ref.end(), less<char>(), alloc01);
  size02 = map12.size();

  map11.swap(map12);
  VERIFY( map11.size() == size02 );
  VERIFY( equal(map11.begin(), map11.end(), map01_ref.begin()) );
  VERIFY( map12.size() == size01 );
  VERIFY( equal(map12.begin(), map12.end(), map04_ref.begin()) );

  my_map map13(map03_ref.begin(), map03_ref.end(), less<char>(), alloc01);
  size01 = map13.size();
  my_map map14(map03_ref.begin(), map03_ref.end(), less<char>(), alloc01);
  size02 = map14.size();

  map13.swap(map14);
  VERIFY( map13.size() == size02 );
  VERIFY( equal(map13.begin(), map13.end(), map03_ref.begin()) );
  VERIFY( map14.size() == size01 );
  VERIFY( equal(map14.begin(), map14.end(), map03_ref.begin()) );
}

void test09()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

  map<int, int> m0;
  typedef map<int, int>::iterator iterator;
  typedef pair<iterator, bool> insert_return_type;
  pair<iterator, iterator> pp0;
  typedef map<int, int>::value_type value_type;

  pp0 = m0.equal_range(1);
  VERIFY( m0.count(1) == 0 );
  VERIFY( pp0.first == m0.end() );
  VERIFY( pp0.second == m0.end() );

  insert_return_type irt0 = m0.insert(value_type(1, 1));
  insert_return_type irt1 = m0.insert(value_type(2, 2));
  insert_return_type irt2 = m0.insert(value_type(3, 3));

  pp0 = m0.equal_range(2);
  VERIFY( m0.count(2) == 1 );
  VERIFY( *pp0.first == value_type(2, 2) );
  VERIFY( *pp0.second == value_type(3, 3) );
  VERIFY( pp0.first == irt1.first );
  VERIFY( --pp0.first == irt0.first );  
  VERIFY( pp0.second == irt2.first );

  m0.insert(value_type(3, 4));
  insert_return_type irt3 = m0.insert(value_type(3, 5));
  insert_return_type irt4 = m0.insert(value_type(4, 6));

  pp0 = m0.equal_range(3);
  VERIFY( m0.count(3) == 1 );
  VERIFY( *pp0.first == value_type(3, 3) );
  VERIFY( *pp0.second == value_type(4, 6) );
  VERIFY( pp0.first == irt2.first );
  VERIFY( --pp0.first == irt1.first );  
  VERIFY( pp0.second == irt4.first );

  insert_return_type irt5 = m0.insert(value_type(0, 7));
  m0.insert(value_type(1, 8));
  m0.insert(value_type(1, 9));
  m0.insert(value_type(1, 10));

  pp0 = m0.equal_range(1);
  VERIFY( m0.count(1) == 1 );
  VERIFY( *pp0.first == value_type(1, 1) );
  VERIFY( *pp0.second == value_type(2, 2) );
  VERIFY( pp0.first == irt0.first );
  VERIFY( --pp0.first == irt5.first );  
  VERIFY( pp0.second == irt1.first );

  insert_return_type irt6 = m0.insert(value_type(5, 11));
  m0.insert(value_type(5, 12));
  m0.insert(value_type(5, 13));

  pp0 = m0.equal_range(5);
  VERIFY( m0.count(5) == 1 );
  VERIFY( *pp0.first == value_type(5, 11) );
  VERIFY( pp0.first == irt6.first );
  VERIFY( --pp0.first == irt4.first );  
  VERIFY( pp0.second == m0.end() );

  m0.insert(value_type(4, 14));
  m0.insert(value_type(4, 15));
  m0.insert(value_type(4, 16));

  pp0 = m0.equal_range(4);
  VERIFY( m0.count(4) == 1 );  
  VERIFY( *pp0.first == value_type(4, 6) );
  VERIFY( *pp0.second == value_type(5, 11) );  
  VERIFY( pp0.first == irt4.first );
  VERIFY( --pp0.first == irt3.first );  
  VERIFY( pp0.second == irt6.first );

  m0.insert(value_type(0, 17));
  insert_return_type irt7 = m0.insert(value_type(0, 18));
  m0.insert(value_type(1, 19));

  pp0 = m0.equal_range(0);
  VERIFY( m0.count(0) == 1 );  
  VERIFY( *pp0.first == value_type(0, 7) );
  VERIFY( *pp0.second == value_type(1, 1) );  
  VERIFY( pp0.first == irt5.first );
  VERIFY( pp0.first == m0.begin() );
  VERIFY( pp0.second == irt0.first );

  pp0 = m0.equal_range(1);
  VERIFY( m0.count(1) == 1 );  
  VERIFY( *pp0.first == value_type(1, 1) );
  VERIFY( *pp0.second == value_type(2, 2) );  
  VERIFY( pp0.first == irt0.first );
  VERIFY( --pp0.first == irt7.first);
  VERIFY( pp0.second == irt1.first );
}

// libstdc++/31440
struct DagNode
{ };

class MemoTable
{
public:
  void memoRewrite();

private:
  struct dagNodeLt;
  class MemoMap;

  MemoMap* memoMap;
};

struct MemoTable::dagNodeLt
{
  bool operator()(const DagNode*, const DagNode*) const
  {
    return false;
  }
};

class MemoTable::MemoMap
  : public std::map<DagNode*, DagNode*, MemoTable::dagNodeLt>
{ };

void
MemoTable::memoRewrite()
{
  memoMap->find(0);
}

// libstdc++/737
// http://gcc.gnu.org/ml/libstdc++/2000-11/msg00093.html

void test10()
{
  typedef std::map<int, int> MapInt;

  MapInt m;

  for (unsigned i = 0; i < 10; ++i)
    m.insert(MapInt::value_type(i,i));

  for (MapInt::const_iterator i = m.begin(); i != m.end(); ++i)
    //std::cerr << i->second << ' ';
    dbg::info.printf("%d ", i->second);
    

  for (MapInt::const_iterator i = m.begin(); m.end() != i; ++i)
    //std::cerr << i->second << ' ';
    dbg::info.printf("%d ", i->second);
}

void test11()
{
  bool test __attribute__((unused)) = true;
  std::map<unsigned int, int> mapByIndex;
  std::map<std::string, unsigned> mapByName;

  mapByIndex.insert(mapByIndex.begin(), std::pair<unsigned, int>(0, 1));
  mapByIndex.insert(std::pair<unsigned, int>(6, 5));

  std::map<unsigned, int>::iterator itr(mapByIndex.begin());

  // NB: notice, it's not mapByIndex!!
  //test &= itr != mapByName.end(); // { dg-error "no" } 
  //test &= itr == mapByName.end(); // { dg-error "no" } 
}
// { dg-error "candidates are" "" { target *-*-* } 210 }
// { dg-error "candidates are" "" { target *-*-* } 214 }

template<typename T>
struct less_than: std::unary_function<std::map<int, int>::value_type, bool>
{
  less_than(T then)
    :value(then)
  {}

  result_type operator()(const argument_type& arg)
  {
    return arg.second < value;
  }
private:
  T value;
};

}

void map_test()
{
  std::map<int,int> x1, x2;
  x1[0] = 1;
  x1[1] = 2;

  x2 = x1;
  assert(x2.size() == x1.size());
  assert(x2 == x1);

  std::map<int,int> x3(x1);
  assert(x3.size() == x1.size());
  assert(x3 == x1);
  assert(!(x3 != x1));


  test01();
//  test02(); // fail
 #ifdef _CPPUNWIND
  test03();
#endif
  test04();
  test05();
#if 0
  // class members specialization
  test06();
  test07();
#endif
  test08();
  test09(); // fail
  test10();
  test11();

}
