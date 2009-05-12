// common tests part
#include <cassert>

#define __attribute__(x)
#pragma warning(disable:4101 4189)
#define VERIFY(e) assert(e)

#include <unordered_map>

template class std::unordered_map<int, float>;

namespace
{
  void test01()
  {
    bool test __attribute__((unused)) = true;

    std::unordered_map<int, int> um;
    um[1] = 1;
    VERIFY( um.cbegin() == um.begin() );
    VERIFY( um.cend() == um.end() );
    VERIFY( um.cbegin() != um.cend() );
  }

  void test02()
  {
    bool test __attribute__((unused)) = true;

    typedef std::unordered_map<int, int> um_type;
    um_type um;
    um[1] = 1;
    um_type::local_iterator li = um.begin(0);
    um_type::const_local_iterator cli = um.cbegin(0);
    test = li == cli;
    VERIFY( um.cbegin(0) == um.begin(0) );
    VERIFY( um.cend(0) == um.end(0) );
    const um_type::size_type bn = um.bucket(1);
    VERIFY( um.cbegin(bn) != um.cend(bn) );
  }

  void test03()
  {
    // Check for required typedefs
    typedef std::unordered_map<int, int>            test_type;

    typedef test_type::key_type                     key_type;
    typedef test_type::value_type                   value_type;
    typedef test_type::mapped_type                  mapped_type;
    typedef test_type::hasher                       hasher;
    typedef test_type::key_equal                    key_equal;
    typedef test_type::allocator_type               allocator_type;
    typedef test_type::pointer                      pointer;
    typedef test_type::const_pointer                const_pointer;
    typedef test_type::reference                    reference;
    typedef test_type::const_reference              const_reference;
    typedef test_type::size_type                    size_type;
    typedef test_type::difference_type              difference_type;
    typedef test_type::iterator                     iterator;
    typedef test_type::const_iterator               const_iterator;
    typedef test_type::local_iterator               local_iterator;
    typedef test_type::const_local_iterator         const_local_iterator;
  }

  void test04() 
  { 
    bool test __attribute__((unused)) = true;
    typedef std::unordered_map<int, double> map_type;
#if STLX__USE_EXCEPTIONS
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
#endif
  }

  void test05()
  {
#ifdef NTL__CXX_IL
    bool test __attribute__((unused)) = true;
    unordered_map<int,double> m({ { 1, 1.0 }, { 2, 2.0 }, { 42, 237.0 } });

    VERIFY(m.size() == 3);
    VERIFY(m[1] == 1.0);
    VERIFY(m[2] == 2.0);
    VERIFY(m[42] == 237.0);

    m = { {5, 55.0}, { 6, 66.0 } };
    VERIFY(m.size() == 2);
    VERIFY(m[5] == 55.0);
    VERIFY(m[6] == 66.0);

    m.insert({ { 7, 77.0 }, { 8, 88.0 } });
    VERIFY(m.size() == 4);
    VERIFY(m[5] == 55.0);
    VERIFY(m[6] == 66.0);
    VERIFY(m[7] == 77.0);
    VERIFY(m[8] == 88.0);
#endif
  }

  void test06()
  {
#ifdef NTL__CXX_RV
    bool test __attribute__((unused)) = true;

    std::unordered_map<int, int> a,b;
    a[2]=0;
    b[1]=0;
    b = std::move(a);
    VERIFY( b.find(2) != b.end() && a.find(1) == a.end() );

    std::unordered_map<int, int> c(std::move(b));
    VERIFY( c.find(2) != c.end() );
    VERIFY( b.find(2) == b.end() );
#endif
  }
}

void unordered_map_test2()
{
  test01();
  test02();
  test03();
  test04();
  test05();
  test06();
}
