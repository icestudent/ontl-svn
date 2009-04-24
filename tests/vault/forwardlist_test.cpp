// common tests part
#include <cassert>
//#include <nt/new.hxx>

#define __attribute__(x)
#pragma warning(disable:4101 4189)
#define VERIFY(e) assert(e)
#define sizeofa(a) (sizeof(a) / sizeof(*a))

#include <forward_list>
#include <array>

template class std::forward_list<int>;
template class std::forward_list<int, std::allocator<char> >;

namespace 
{
  namespace a1 {
  // A nontrivial type.
  template<typename T>
  struct A 
  {
    friend bool operator==(const A&, const A&){ return false; }
    friend bool operator< (const A&, const A&){ return false; }
  };

  // Another nontrivial type
  struct B { };

  // A nontrivial type convertible from an int
  struct C
  {
    C(int i) : i_(i) { }
    bool operator==(const C& rhs) { return i_ == rhs.i_; }
    int i_;
  };
  } // a1

  // This test verifies the following.
  //
  void
    test01()
  {
    using namespace a1;
    std::forward_list< A<B> > lst;
    VERIFY(lst.begin() == lst.end());
    VERIFY(std::distance(lst.begin(), lst.end()) == 0);

    // check type definitions -- will fail compile if missing
    typedef std::forward_list< A<B> >::reference              reference;
    typedef std::forward_list< A<B> >::const_reference        const_reference;
    typedef std::forward_list< A<B> >::iterator               iterator;
    typedef std::forward_list< A<B> >::const_iterator         const_iterator;
    typedef std::forward_list< A<B> >::size_type              size_type;
    typedef std::forward_list< A<B> >::difference_type        difference_type;
    typedef std::forward_list< A<B> >::value_type             value_type;
    typedef std::forward_list< A<B> >::allocator_type         allocator_type;
    typedef std::forward_list< A<B> >::pointer                pointer;
    typedef std::forward_list< A<B> >::const_pointer          const_pointer;
  }

#ifdef NTL__CXX_IL
  void test02()
  {
    std::forward_list<double> a = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::forward_list<double> b = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};

    VERIFY((a == b) == false);
    VERIFY((a <  b) == true);
    VERIFY((a != b) == true);
    VERIFY((a >  b) == false);
    VERIFY((a >= b) == false);
    VERIFY((a <= b) == true);

    VERIFY((b == a) == false);
    VERIFY((b <  a) == false);
    VERIFY((b != a) == true);
    VERIFY((b >  a) == true);
    VERIFY((b >= a) == true);
    VERIFY((b <= a) == false);
  }
#endif

  void test03()
  {
    std::forward_list<int> a, b;
    a.push_front(1);

    b = std::move(a);
    VERIFY(b.empty() == false);
    VERIFY(*b.begin() == 1);
    VERIFY(a.empty() == true);

    std::forward_list<int> c(std::move(b));
    VERIFY(c.empty() == false);
    (*c.begin() == 1 );
    VERIFY( b.empty() == true );
  }

  void test04()
  {
    std::forward_list<double> fld;
    VERIFY(fld.empty() == true);

    fld.push_front(1.0);
    VERIFY(fld.empty() == false);

    double front = fld.front();

    fld.resize(0);
    VERIFY(fld.empty() == true);

    //VERIFY( fld.max_size()
    //  == std::allocator<std::_Fwd_list_node<double> >().max_size() );
  }

  void test05()
  {
    std::forward_list<double> fld;
    fld.push_front(1.0);
    std::forward_list<double>::const_iterator ci1 = fld.cbefore_begin(), ci2 = fld.cbegin();
    VERIFY(fld.empty() == false);
    fld.clear();
    VERIFY(fld.empty() == true);

    fld.push_front(1.0);
    fld.push_front(2.0);
    fld.pop_front();
    double tmpp = fld.front();
    fld.push_front(2.0);
    ci1 = fld.cbegin();
    std::forward_list<double>::iterator i1 = fld.erase_after(ci1);
    VERIFY(fld.empty() == false);
    tmpp = fld.front();
    if(i1 != fld.end())
      tmpp = *i1;
    VERIFY(fld.front() == 2.0);
  }

  void test06()
  {
    const int ni = 10;
    int i[ni] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::forward_list<int> flccin(i, i+ni);
    std::forward_list<int> flcc(flccin/*, std::allocator<int>()*/);
  }

  void test07()
  {
    const int ni = 10;
    int i[ni] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::forward_list<int> flmc(std::forward_list<int>(i, i+ni)/*, std::allocator<int>()*/);
  }

  void test08()
  {
    std::forward_list<double> flvd(10);
  }

  void test09()
  {
     std::forward_list<float> flv(10, 5.0F);
  }

  void test10()
  {
    const int ni = 10;
    int i[ni] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::forward_list<int> fl(i, i+ni);
  }

  void test11()
  {
    const int ni = 10;
    int i[ni] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::forward_list<int> fl(i, i+ni);
    std::forward_list<int> flc(fl);
  }

  void test12()
  {
    const int ni = 10;
    int i[ni] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::forward_list<int> fl(i, i+ni);
    std::forward_list<int> flm(std::move(fl));
  }

#ifdef NTL__CXX_IL
  void test13()
  {
     std::forward_list<double> flil({1.0, 2.0, 3.0, 4.0, 5.0});
  }
#endif

  class PathPoint
  {
  public:
    PathPoint(char t, std::array<double, 3> & c)
      : type(t), coord(c) { }
    char getType() const { return type; }
  private:
    char type;
    std::array<double, 3> coord;
  };

  // This test verifies the following.
  //   emplace_front
  //   pop_front
  //   emplace_after
#ifdef NTL__CXX_RV
  void
    test13()
  {
    std::forward_list<PathPoint> path;
    std::array<double, 3> coord1 = { { 0.0, 1.0, 2.0 } };
    path.emplace_front('a', coord1);

    std::forward_list<PathPoint>::const_iterator pos = path.cbegin();

    std::array<double, 3> coord2 = { { 3.0, 4.0, 5.0 } };
    path.emplace_after(pos, 'b', coord2);

    VERIFY(path.front().getType() == 'a');

    path.pop_front();

    VERIFY(path.front().getType() == 'b');

    path.pop_front();

    VERIFY(path.empty() == true);
  }
#endif
  // This test verifies the following:
  //   insert_after single item
  //   before_begin iterator
  void
    test14()
  {
#ifdef NTL__CXX_IL
    std::forward_list<int> fl({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
#else
    const int ni = 10;
    int i[ni] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::forward_list<int> fl(i, i+ni);
#endif

    fl.insert_after(fl.before_begin(), 42);
    VERIFY(fl.front() == 42);
  }

  // This test verifies the following:
  void
    test15()
  {
#ifdef NTL__CXX_IL
    std::forward_list<int> fl({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
#else
    const int ni = 10;
    int i[ni] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::forward_list<int> fl(i, i+ni);
#endif
    std::forward_list<int>::const_iterator pos = fl.cbegin();
    ++pos;
    VERIFY(*pos == 1);

    // Note: Calling l.insert_after(pos, 5, 42); without the long five
    // gets resolved to the iterator range version and fails to compile!
    fl.insert_after(pos, 5, 42);
    VERIFY(*pos == 1);

    ++pos;
    VERIFY(*pos == 42);
    ++pos;
    ++pos;
    ++pos;
    ++pos;
    VERIFY(*pos == 42);
  }

  // This test verifies the following:
  void
    test16()
  {
#ifdef NTL__CXX_IL
    std::forward_list<int> fl({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
#else
    const int ni = 10;
    int ix[ni] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::forward_list<int> fl(ix, ix+ni);
#endif
    std::forward_list<int>::const_iterator pos = fl.cbegin();
    ++pos;
    VERIFY(*pos == 1);

    int i[3] = {666, 777, 888};
    fl.insert_after(pos, i, i+3);
    VERIFY(*pos == 1);

    ++pos;
    ++pos;
    ++pos;
    VERIFY(*pos == 888);
    ++pos;
    VERIFY(*pos == 2);
  }

  // This test verifies the following:
#ifdef NTL__CXX_IL
  void
    test17()
  {
    std::forward_list<int> fl({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

    std::forward_list<int>::const_iterator pos = fl.cbegin();
    ++pos;
    VERIFY(*pos == 1);

    fl.insert_after(pos, {-1, -2, -3, -4, -5});
    VERIFY(*pos == 1);

    ++pos;
    ++pos;
    ++pos;
    VERIFY(*pos == -3);
  }
#endif
  // This test verifies the following:
  void
    test18()
  {
#ifdef NTL__CXX_IL
    std::forward_list<std::string> fl({"AAA", "BBB", "CCC"});
#else
    std::string s[] = {"AAA", "BBB", "CCC"};
    std::forward_list<std::string> fl(s, s+sizeofa(s));
#endif


    std::forward_list<std::string>::const_iterator pos = fl.cbegin();
    ++pos;
    VERIFY(*pos == "BBB");

    std::string x( "XXX" );
    fl.insert_after(pos, std::move(x));
    VERIFY(*pos == "BBB");
    ++pos;
    VERIFY(*pos == "XXX");
    ++pos;
    VERIFY(*pos == "CCC");
  }

  // This test verifies the following:
  //   cbegin
  //   erase_after one iterator
  //   pos is useable and points to current element
  void
    test19()
  {
#ifdef NTL__CXX_IL
    std::forward_list<int> fl({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
#else
    const int ni = 10;
    int i[ni] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::forward_list<int> fl(i, i+ni);
#endif

    std::forward_list<int>::const_iterator pos = fl.cbegin();
    ++pos;
    VERIFY(*pos == 1);

    std::forward_list<int>::iterator next = fl.erase_after(pos);
    VERIFY(*next == 1);

    VERIFY(*pos == 1);
    ++pos;
    VERIFY(*pos == 3);
  }

  // This test verifies the following:
  //   cbegin
  //   erase_after iterator range
  //   pos is useable and points to current element
  void
    test20()
  {
#ifdef NTL__CXX_IL
    std::forward_list<int> fl({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
#else
    const int ni = 10;
    int i[ni] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::forward_list<int> fl(i, i+ni);
#endif
    std::forward_list<int>::const_iterator pos = fl.cbegin();
    ++pos;
    VERIFY(*pos == 1);

    std::forward_list<int>::iterator stop = fl.begin();
    ++stop;
    ++stop;
    ++stop;
    ++stop;
    VERIFY(*stop == 4);

    std::forward_list<int>::iterator next = fl.erase_after(pos, stop);
    VERIFY(*next == 1);

    VERIFY(*pos == 1);
    ++pos;
    VERIFY(*pos == 5);
  }

  // This test verifies the following:
  //   swap
  void
    test21()
  {
#ifdef NTL__CXX_IL
    std::forward_list<int> fl1({0, 1, 2, 3, 4, 5});
    std::forward_list<int> fl2({666, 777, 888});
#else
    int i1[] = {0,1,2,3,4,5};
    int i2[] = {666,777,888};
    std::forward_list<int> fl1(i1, i1+sizeofa(i1));
    std::forward_list<int> fl2(i2, i2+sizeofa(i2));
#endif
    fl1.swap(fl2);

    VERIFY(fl1.front() == 666);
    VERIFY(fl2.front() == 0);
  }

  // This test verifies the following:
  //   clear
  void
    test22()
  {
#ifdef NTL__CXX_IL
    std::forward_list<int> fl({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
#else
    const int ni = 10;
    int i[ni] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::forward_list<int> fl(i, i+ni);
#endif
    VERIFY(fl.empty() == false);

    fl.clear();

    VERIFY(fl.empty() == true);
  }

  // This test verifies the following:
  //   
  void
    test23()
  {
#ifdef NTL__CXX_IL
    std::forward_list<double> a = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::forward_list<double> x = {666.0, 777.0, 888.0};
#else
    double d1[] = {0.0, 1.0, 2.0, 3.0, 4.0};
    double d2[] = {666.0, 777.0, 888.0};
    std::forward_list<double> a(d1, d1+sizeofa(d1));
    std::forward_list<double> x(d2, d2+sizeofa(d2));
#endif
    std::forward_list<double>::const_iterator posa = a.cbefore_begin();


    a.splice_after(posa, x);

    ++posa;
    VERIFY(*posa == 666.0);

    VERIFY(x.empty() == true);
  }

  // This test verifies the following:
  //   
  void
    test24()
  {
#ifdef NTL__CXX_IL
    std::forward_list<double> a = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::forward_list<double> y = {10.0, 11.0, 12.0, 13.0, 14.0, 15.0};
#else
    double d1[] = {0.0, 1.0, 2.0, 3.0, 4.0};
    double d2[]  = {10.0, 11.0, 12.0, 13.0, 14.0, 15.0};
    std::forward_list<double> a(d1, d1+sizeofa(d1));
    std::forward_list<double> y(d2, d2+sizeofa(d2));
#endif
    std::forward_list<double>::const_iterator posa = a.cbefore_begin();
    ++posa;
    VERIFY(*posa == 0.0);

    std::forward_list<double>::const_iterator befy = y.cbefore_begin();
    ++befy;
    VERIFY(*befy == 10.0);
    std::forward_list<double>::const_iterator endy = befy;
    ++endy;
    ++endy;
    ++endy;
    ++endy;
    VERIFY(*endy == 14.0);

    a.splice_after(posa, y, befy, endy);
    VERIFY(*posa == 0.0);

    VERIFY(*befy == 10.0);
    ++befy;
    VERIFY(*befy == 15.0);
  }

  // This test verifies the following:
  //   
  void
    test25()
  {
#ifdef NTL__CXX_IL
    std::forward_list<double> a = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::forward_list<double> z = {42.0, 43.0, 44.0};
#else
    double d1[] = {0.0, 1.0, 2.0, 3.0, 4.0};
    double d2[]  = {42.0, 43.0, 44.0};
    std::forward_list<double> a(d1, d1+sizeofa(d1));
    std::forward_list<double> z(d2, d2+sizeofa(d2));
#endif

    std::forward_list<double>::const_iterator posa = a.cbefore_begin();
    ++posa;
    ++posa;
    VERIFY(*posa == 1.0);

    std::forward_list<double>::const_iterator posz = z.begin();
    VERIFY(*posz == 42.0);

    a.splice_after(posa, z, posz);
    VERIFY(*posa == 1.0);
    ++posa;
    VERIFY(*posa == 43.0);

    VERIFY(*posz == 42.0);
    ++posz;
    VERIFY(*posz == 44.0);
  }

  // This test verifies the following:
  //   remove
  void
    test26()
  {
#ifdef NTL__CXX_IL
    std::forward_list<int> fl({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
#else
    const int ni = 10;
    int i[ni] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::forward_list<int> fl(i, i+ni);
#endif
    fl.remove(7);

    std::forward_list<int>::const_iterator pos = fl.cbefore_begin();
    for (std::size_t i = 0; i < 7; ++i)
      ++pos;
    VERIFY(*pos == 6);

    ++pos;
    VERIFY(*pos == 8);
  }

  // This test verifies the following:
  //   remove_if
  void
    test27()
  {
#ifdef NTL__CXX_IL
    std::forward_list<int> fl({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
#else
    const int ni = 10;
    int i[ni] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::forward_list<int> fl(i, i+ni);
#endif


    fl.remove_if(std::bind2nd(std::less<int>(),5));

    std::forward_list<int>::const_iterator pos = fl.cbegin();
    VERIFY(*pos == 5);
  }


  // This test verifies the following:
  //   unique
  void
    test28()
  {
#ifdef NTL__CXX_IL
    std::forward_list<int> fl = {99, 5, 99, 6, -5, 666, 777, 888,
      42, 42, 42, 42, 42, 7, 0, 0, 0, 9, 9, 9};
    std::forward_list<int> fl2 = {99, 5, 99, 6, -5, 666, 777, 888, 42, 7, 0, 9};
#else
    int i[] = {99, 5, 99, 6, -5, 666, 777, 888, 42, 42, 42, 42, 42, 7, 0, 0, 0, 9, 9, 9};
    int i2[] = {99, 5, 99, 6, -5, 666, 777, 888, 42, 7, 0, 9};

    std::forward_list<int> fl(i, i+sizeofa(i));
    std::forward_list<int> fl2(i2, i2+sizeofa(i2));
#endif

    fl.unique();

    VERIFY(fl == fl2);
  }

  //  Test comparison predicate.
  template<typename Num>
  class Mod
  {
  public:
    Mod(const Num & mod)
    {
      m = mod;
    }
    bool operator()(const Num i, const Num j)
    {
      return i%m == j%m;
    }
  private:
    Num m;
  };

  // This test verifies the following:
  //   unique with predicate
  void
    test29()
  {
#ifdef NTL__CXX_IL
    std::forward_list<int> fl = {99, 5, 99, 6, -5, 666, 777, 888, 42, 7, 0, 9};
    std::forward_list<int> fl2 = {99, 5, 99, 6, -5, 666, 42, 7, 0, 9};
#else
    int i[] = {99, 5, 99, 6, -5, 666, 777, 888, 42, 7, 0, 9};
    int i2[] = {99, 5, 99, 6, -5, 666, 42, 7, 0, 9};

    std::forward_list<int> fl(i, i+sizeofa(i));
    std::forward_list<int> fl2(i2, i2+sizeofa(i2));
#endif
    fl.unique(Mod<int>(111));

    VERIFY(fl == fl2);
  }


  // This test verifies the following:
  //   
  void
    test30()
  {
#ifdef NTL__CXX_IL
    std::forward_list<double> a = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::forward_list<double> b = {1.0, 2.0, 3.0, 4.0, 4.0, 5.0};
    std::forward_list<double> r = {0.0, 1.0, 1.0, 2.0, 2.0, 3.0, 3.0,
      4.0, 4.0, 4.0, 5.0};
#else
    double d1[] = {0.0, 1.0, 2.0, 3.0, 4.0};
    double d2[] = {1.0, 2.0, 3.0, 4.0, 4.0, 5.0};
    double d3[] = {0.0, 1.0, 1.0, 2.0, 2.0, 3.0, 3.0, 4.0, 4.0, 4.0, 5.0};
    std::forward_list<double> a(d1, d1+sizeofa(d1));
    std::forward_list<double> b(d2, d2+sizeofa(d2));
    std::forward_list<double> r(d3, d3+sizeofa(d3));

#endif
    a.merge(b);

    VERIFY(a == r);
  }

  //  Comparison functor.
  template<typename Num>
  class Comp
  {
  public:
    Comp(const Num & num)
    {
      n = num;
    }
    bool operator()(const Num i, const Num j)
    {
      return (n * i) < (n * j);
    }
  private:
    Num n;
  };

  // This test verifies the following:
  //   
#ifdef LIST_WITH_SORT
  void
    test31()
  {
    const unsigned int n = 13;
    int order[][n] = {
      { 0,1,2,3,4,5,6,7,8,9,10,11,12 },
      { 6,2,8,4,11,1,12,7,3,9,5,0,10 },
      { 12,11,10,9,8,7,6,5,4,3,2,1,0 },
    };
    std::forward_list<int> sorted(order[0], order[0] + n);

    for (unsigned int i = 0; i < sizeof(order)/sizeof(*order); ++i)
    {
      std::forward_list<int> head(order[i], order[i] + n);

      head.sort();

      VERIFY(head == sorted);
    }

    std::forward_list<int> reversed(order[2], order[2] + n);
    for (unsigned int i = 0; i < sizeof(order)/sizeof(*order); ++i)
    {
      std::forward_list<int> head(order[i], order[i] + n);

      Comp<int> comp(-1);
      head.sort( comp );

      VERIFY(head == reversed);
    }
  }
#endif

  // This test verifies the following:
  //   
  void
    test32()
  {
    const unsigned int n = 13;
    int order[n] = {0,1,2,3,4,5,6,7,8,9,10,11,12};

    std::forward_list<int> fl(order, order + n);

    std::forward_list<int> fl2;
    for (std::size_t i = 0; i < n; ++i)
      fl2.push_front(order[i]);

    fl.reverse();

    VERIFY(std::lexicographical_compare(fl.begin(), fl.end(),
      fl2.begin(), fl2.end()) == false);
  }

} // namespace


namespace forwardlist_test {

  void main()
  {
    test01();
#ifdef NTL__CXX_IL
    test02();
#endif
#ifdef NTL__CXX_RV
    test03();
#endif
    test04();
    test05();
    test06();
    test07();
    test08();
    test09();
    test10();
    test11();
    test12();
#ifdef NTL__CXX_RV
    test13();
#endif
    test14();
    test15();
    test16();
#ifdef NTL__CXX_IL
    test17();
#endif
    test18();
    test19();
    test20();
    test21();
    test22();
    test23();
    test24();
    test25();
    test26();
    test27();
    test28();
    test29();
    test30();
#ifdef LIST_WITH_SORT
    test31();
#endif
    test32();
  }
} // test