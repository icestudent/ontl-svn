// common tests part
#include <cassert>

#define __attribute__(x)
#pragma warning(disable:4101 4189)
#define VERIFY(e) assert(e)

// set to 1 to check compile-time errors
#define CT_FAIL 0

template<typename T, typename _ = int>
struct xshow_type
{
  char _[sizeof(_) == sizeof(char)];
};

template<int value, typename _ = int>
struct xshow_value
{
  char _[sizeof(_) == sizeof(char)];
};

template<__int64 value, typename _ = int>
struct xshow_value64
{
  char _[sizeof(_) == sizeof(char)];
};

#define _PasteToken(x,y) x##y
#define _Join(x,y) _PasteToken(x,y)

#define SHOWR(...) xshow_type<__VA_ARGS__> _Join(__x_show_type, __COUNTER__);
#define SHOWT(...) xshow_type<__VA_ARGS__> _Join(__x_show_type, __COUNTER__);
#define SHOWV(...) xshow_value<__VA_ARGS__> _Join(__x_show_value, __COUNTER__);
#define SHOWLV(...) xshow_value64<__VA_ARGS__> _Join(__x_show_value64, __COUNTER__);

#include <cstdint>

#if 0
struct test {
  typedef std::uintptr_t storage_type; // native platform type

  enum { N = 511 };
  enum { digits = N };
  enum { element_size_ = sizeof(storage_type) * 8 }; // bits count

  static const size_t element_mod_ = element_size_ - 1;
  static const size_t digitd_mod_ = static_cast<size_t>(1 << (N & (element_size_-1))) - 1;
  static const size_t elements_count_ = N / element_size_ + ((N & (element_size_-1)) ? 1 : 0);
  static const storage_type set_bits_ = static_cast<storage_type>(-1);
};

SHOWV(test::digitd_mod_);
#endif

#include <bitset>

template class std::bitset<10>;

namespace bitset_test
{
  // DR 693. std::bitset::all() missing.
  void test01()
  {
    bool test __attribute__((unused)) = true;

#if 0
    std::bitset<0> z1;
    VERIFY( z1.all() );
    z1.set();
    VERIFY( z1.all() );
#endif
    std::bitset<8> z2;
    VERIFY( !z2.all() );
    z2.set();
    VERIFY( z2.all() );

    std::bitset<16> z3;
    VERIFY( !z3.all() );
    z3.set();
    VERIFY( z3.all() );

    std::bitset<32> z4;
    VERIFY( !z4.all() );
    z4.set();
    VERIFY( z4.all() );

    std::bitset<64> z5;
    VERIFY( !z5.all() );
    z5.set();
    VERIFY( z5.all() );

    std::bitset<96> z6;
    VERIFY( !z6.all() );
    z6.set();
    VERIFY( z6.all() );

    std::bitset<128> z7;
    VERIFY( !z7.all() );
    z7.set();
    VERIFY( z7.all() );

    std::bitset<192> z8;
    VERIFY( !z8.all() );
    z8.set();
    VERIFY( z8.all() );

    std::bitset<1024> z9;
    VERIFY( !z9.all() );
    z9.set();
    VERIFY( z9.all() );

    std::bitset<1> z10;
    VERIFY( !z10.all() );
    z10.set();
    VERIFY( z10.all() );

    std::bitset<1025> z11;
    VERIFY( !z11.all() );
    z11.set();
    VERIFY( z11.all() );

  }

  // libstdc++/16020
  void test02()
  {
    using std::bitset;
    bool test __attribute__((unused)) = true;

    bitset<5> b(7);
    bitset<5> c;

    bitset<5> bb(b);
    c = bb;

    VERIFY( bb == b );
    VERIFY( c == bb );
  }
  // DR 778. std::bitset does not have any constructor taking a string literal.
  void test03()
  {
#if CT_FAIL
    bool test __attribute__((unused)) = true;

    std::bitset<4> z1("1101"); // should fail
    std::bitset<4> z1_ref(std::string("1101"));
    VERIFY( z1.to_string() == "1101" );
    VERIFY( z1 == z1_ref );

    std::bitset<8> z2("1011"); // should fail (bitset from string ctor is explicit)
    std::bitset<8> z2_ref(std::string("1011"));
    VERIFY( z2.to_string() == "00001011" );
    VERIFY( z2 == z2_ref );

    std::bitset<2> z3("1101");
    std::bitset<2> z3_ref(std::string("1101"));
    VERIFY( z3.to_string() == "11" );
    VERIFY( z3 == z3_ref );
#endif
  }

  // libstdc++/6124
  void test04()
  {
    std::bitset<1> bs;
    bs.count();
  }

  static char original_bits[1024];
  static char left_shifted[1024];
  static char right_shifted[1024];

  char
    random_bit() {
      static long x = 1;
      return static_cast<char>( ((x = (3432L*x + 6789L) % 9973L) & 1) + '0' );
  }

  void
    initialise(size_t size) {
      for (size_t i=0; i<size; i++)
        original_bits[i] = random_bit();  

      original_bits[size] = '\0';
      left_shifted[size] = '\0';
      right_shifted[size] = '\0';
  }

  void
    shift_arrays(size_t shift_step, size_t size) {
      for (size_t i=shift_step; i<size; i++) {
        right_shifted[i] = original_bits[i-shift_step];
        left_shifted[size-i-1] = original_bits[size+shift_step-i-1];
      }
      for (size_t i=0; i<shift_step && i<size; i++) {
        right_shifted[i] = '0';
        left_shifted[size-i-1] = '0';
      }
  }

  template <size_t size>
  bool
    do_test() {
      const size_t lsize = size;
      bool test __attribute__((unused)) = true;

      std::bitset<size> shifted;
      std::bitset<size> correct;

      initialise(size);

      //std::bitset<size> original = std::string(original_bits); 
      std::bitset<size> original = std::bitset<size> (std::string(original_bits)); 

      for (size_t shift_step=0; shift_step==0 || shift_step<size; shift_step++) {
        shift_arrays(shift_step, size);

        shifted = original;
        shifted <<= shift_step;
        //correct = std::string(left_shifted);
        correct = std::bitset<size> (std::string(left_shifted));

        std::string shifted_str = shifted.to_string(); shifted_str.c_str();
        std::string correct_str(left_shifted);
        bool ok = shifted_str == correct_str;
        VERIFY( shifted == correct );

        shifted = original;
        shifted >>= shift_step;
        correct = std::bitset<size> (std::string(right_shifted));
        shifted_str = shifted.to_string(); shifted_str.c_str();
        correct_str = right_shifted;
        ok = shifted_str == correct_str;


        VERIFY( shifted == correct );
      }

      return test;
  }

  bool
    test05() {
      bool test __attribute__((unused)) = true;

      VERIFY( do_test<32>() );
      VERIFY( do_test<48>() );
      VERIFY( do_test<63>() );
      VERIFY( do_test<64>() );
      VERIFY( do_test<65>() );
      VERIFY( do_test<66>() );
      VERIFY( do_test<127>() );
      VERIFY( do_test<128>() );
      VERIFY( do_test<129>() );

      VERIFY( do_test<511>() );
      VERIFY( do_test<513>() );
      VERIFY( do_test<997>() );
      return test;
  }

  // libstdc++/13838
  void test06()
  {
    using std::bitset;
    bool test __attribute__((unused)) = true;

    bitset<4>  b0, b1;
    b0.set(1);
    b0.set(3);
    b1.set(2);
    b1.set(3);

    b0 |= b1;

    bitset<4>  br;
    br.set(1);
    br.set(2);
    br.set(3);

    VERIFY( b0 == br );
  }

  bool
    test07()
  {
    bool test __attribute__((unused)) = true;

    std::bitset<66>  b;
    b <<= 400;
    VERIFY( b.count() == 0 );
    return test;
  }

  void test08()
  {
    using namespace std;
    bool test __attribute__((unused)) = true;

    bitset<5> b5;
    string s0 = b5.to_string<char, char_traits<char>, allocator<char> >();
    VERIFY( s0 == "00000" );

    // DR 434. bitset::to_string() hard to use.
    b5.set(0);
    string s1 = b5.to_string<char, char_traits<char> >();
    VERIFY( s1 == "00001" );

    b5.set(2);
    string s2 = b5.to_string<char>();
    VERIFY( s2 == "00101" );

    b5.set(4);
    string s3 = b5.to_string();
    VERIFY( s3 == "10101" );
  }

#ifdef _CPPUNWIND
  void test09()
  {
    bool test __attribute__((unused)) = true;
    std::bitset<5>      b;
    std::stringstream   ss("101");
    ss.exceptions(std::ios_base::eofbit);

    try
    {
      ss >> b;
    }
    catch (std::exception&) { }

    VERIFY( b.to_ulong() == 5 );
  }
#endif

  void main()
  {
    typedef std::uint64_t uint64_t;
    std::bitset<32> x(0x0F);
    {
      static const size_t bits = 63;
      static const size_t ival = 0x0D;
      static const int maxi = bits - 4;
      uint64_t cache = ival;
      for(int i = 1; i < maxi; ++i){
        std::bitset<bits> x(ival);
        x <<= i;
        const uint64_t xval = x.to_ullong();
        const uint64_t yval = cache <<= 1;
        bool ok = xval == yval;
        assert(ok);
      }

      // 4 = 2
      static const uint64_t val = 0x3400000000000000I64;
      //SHOWLV(ival);

      for(int i = 1; i < maxi; ++i){
        std::bitset<bits> x(val);
        x >>= i;
        const uint64_t xval = x.to_ullong();
        const uint64_t yval = cache >>= 1;
        bool ok = xval == yval;
        assert(ok);
      }
    }
    test01(); // no bitset<0>
    test02();
    test03();
    test04();
    test05();
    test06();
    test07();
    test08();
#ifdef _CPPUNWIND
    test09();
#endif
  }
}