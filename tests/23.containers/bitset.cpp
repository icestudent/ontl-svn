#include "../tests.hxx"

#include <stlx/bitset.hxx>

// explicit instantiation
template class stlx::bitset<10>;

// tests
namespace tut
{
  STLX_DEFAULT_TESTGROUP();
  
  testgroup tg_bitset("stlx::bitset");

  // DR 693. stlx::bitset::all() missing.
  template<> template<> void to::test<1>()
  {
    bool test __attribute__((unused)) = true;

#if CT_FAIL
    stlx::bitset<0> z1;
    quick_ensure( !z1.all() );
    z1.set();
    quick_ensure( z1.all() );
#endif

    stlx::bitset<8> z2;
    quick_ensure( !z2.all() );
    z2.set();
    quick_ensure( z2.all() );

    stlx::bitset<16> z3;
    quick_ensure( !z3.all() );
    z3.set();
    quick_ensure( z3.all() );

    stlx::bitset<32> z4;
    quick_ensure( !z4.all() );
    z4.set();
    quick_ensure( z4.all() );

    stlx::bitset<64> z5;
    quick_ensure( !z5.all() );
    z5.set();
    quick_ensure( z5.all() );

    stlx::bitset<96> z6;
    quick_ensure( !z6.all() );
    z6.set();
    quick_ensure( z6.all() );

    stlx::bitset<128> z7;
    quick_ensure( !z7.all() );
    z7.set();
    quick_ensure( z7.all() );

    stlx::bitset<192> z8;
    quick_ensure( !z8.all() );
    z8.set();
    quick_ensure( z8.all() );

    stlx::bitset<1024> z9;
    quick_ensure( !z9.all() );
    z9.set();
    quick_ensure( z9.all() );

    stlx::bitset<1> z10;
    quick_ensure( !z10.all() );
    z10.set();
    quick_ensure( z10.all() );

    stlx::bitset<1025> z11;
    quick_ensure( !z11.all() );
    z11.set();
    quick_ensure( z11.all() );

  }

  // libstdc++/16020
  template<> template<> void to::test<2>()
  {
    using stlx::bitset;
    bool test __attribute__((unused)) = true;

    bitset<5> b(7);
    bitset<5> c;

    bitset<5> bb(b);
    c = bb;

    quick_ensure( bb == b );
    quick_ensure( c == bb );
  }
  // DR 778. stlx::bitset does not have any constructor taking a string literal.
  template<> template<> void to::test<3>()
  {
#if CT_FAIL
    bool test __attribute__((unused)) = true;

    stlx::bitset<4> z1("1101"); // should fail
    stlx::bitset<4> z1_ref(stlx::string("1101"));
    quick_ensure( z1.to_string() == "1101" );
    quick_ensure( z1 == z1_ref );

    stlx::bitset<8> z2("1011"); // should fail (bitset from string ctor is explicit)
    stlx::bitset<8> z2_ref(stlx::string("1011"));
    quick_ensure( z2.to_string() == "00001011" );
    quick_ensure( z2 == z2_ref );

    stlx::bitset<2> z3("1101");
    stlx::bitset<2> z3_ref(stlx::string("1101"));
    quick_ensure( z3.to_string() == "11" );
    quick_ensure( z3 == z3_ref );
#endif
  }

  // libstdc++/6124
  template<> template<> void to::test<4>()
  {
    stlx::bitset<1> bs;
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

      stlx::bitset<size> shifted;
      stlx::bitset<size> correct;

      initialise(size);

      //stlx::bitset<size> original = stlx::string(original_bits); 
      stlx::bitset<size> original = stlx::bitset<size> (stlx::string(original_bits)); 

      for (size_t shift_step=0; shift_step==0 || shift_step<size; shift_step++) {
        shift_arrays(shift_step, size);

        shifted = original;
        shifted <<= shift_step;
        //correct = stlx::string(left_shifted);
        correct = stlx::bitset<size> (stlx::string(left_shifted));

        stlx::string shifted_str = shifted.to_string(); shifted_str.c_str();
        stlx::string correct_str(left_shifted);
        bool ok = shifted_str == correct_str;
        quick_ensure( shifted == correct );

        shifted = original;
        shifted >>= shift_step;
        correct = stlx::bitset<size> (stlx::string(right_shifted));
        shifted_str = shifted.to_string(); shifted_str.c_str();
        correct_str = right_shifted;
        ok = shifted_str == correct_str;


        quick_ensure( shifted == correct );
      }

      return test;
  }

  template<> template<> void to::test<5>()
  {
      bool test __attribute__((unused)) = true;

      quick_ensure( do_test<32>() );
      quick_ensure( do_test<48>() );
      quick_ensure( do_test<63>() );
      quick_ensure( do_test<64>() );
      quick_ensure( do_test<65>() );
      quick_ensure( do_test<66>() );
      quick_ensure( do_test<127>() );
      quick_ensure( do_test<128>() );
      quick_ensure( do_test<129>() );

      quick_ensure( do_test<511>() );
      quick_ensure( do_test<513>() );
      quick_ensure( do_test<997>() );
  }

  // libstdc++/13838
  template<> template<> void to::test<6>()
  {
    using stlx::bitset;
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

    quick_ensure( b0 == br );
  }

  template<> template<> void to::test<7>()
  {
    bool test __attribute__((unused)) = true;

    stlx::bitset<66>  b;
    b <<= 400;
    quick_ensure( b.count() == 0 );
  }

  template<> template<> void to::test<8>()
  {
    using namespace stlx;
    bool test __attribute__((unused)) = true;

    bitset<5> b5;
    string s0 = b5.to_string<char, char_traits<char>, allocator<char> >();
    quick_ensure( s0 == "00000" );

    // DR 434. bitset::to_string() hard to use.
    b5.set(0);
    string s1 = b5.to_string<char, char_traits<char> >();
    quick_ensure( s1 == "00001" );

    b5.set(2);
    string s2 = b5.to_string<char>();
    quick_ensure( s2 == "00101" );

    b5.set(4);
    string s3 = b5.to_string();
    quick_ensure( s3 == "10101" );
  }

#ifdef _CPPUNWINDx
  template<> template<> void to::test<9>()
  {
    bool test __attribute__((unused)) = true;
    stlx::bitset<5>      b;
    std::stringstream   ss("101");
    ss.exceptions(std::ios_base::eofbit);

    try
    {
      ss >> b;
    }
    catch (std::exception&) { }

    quick_ensure( b.to_ulong() == 5 );
  }
#endif
 }