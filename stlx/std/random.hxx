/**\file*********************************************************************
 *                                                                     \brief
 *  26.4 Random number generation [rand]
 *  This subclause defines a facility for generating (pseudo-)random numbers.
 ****************************************************************************
 */
#ifndef NTL__STLX_RANDOM
#define NTL__STLX_RANDOM

#include "cstdint.hxx"
#include "type_traits.hxx"

namespace stlx
{
 /**\addtogroup  lib_numeric ************ 26 Numerics library [numerics]
  *@{
  **/
 /**\addtogroup  lib_numeric_rand ******* 26.4 Random number generation [rand]
  *@{
  **/


  // 26.4.3.1 Class template linear_congruential_engine
  template <class UIntType, UIntType a, UIntType c, UIntType m>
  class linear_congruential_engine;

  // 26.4.3.2 Class template mersenne_twister_engine
  template <class UIntType, size_t w, size_t n, size_t m, size_t r,
    UIntType a, size_t u, UIntType d, size_t s,
    UIntType b, size_t t,
    UIntType c, size_t l, UIntType f>
  class mersenne_twister_engine;

  // 26.4.3.3 Class template subtract_with_carry_engine
  template <class UIntType, size_t w, size_t s, size_t r>
  class subtract_with_carry_engine;

  // 26.4.4.1 Class template discard_block_engine
  template <class Engine, size_t p, size_t r>
  class discard_block_engine;

  // 26.4.4.2 Class template independent_bits_engine
  template <class Engine, size_t w, class UIntType>
  class independent_bits_engine;

  // 26.4.4.3 Class template shuffle_order_engine
  template <class Engine, size_t k>
  class shuffle_order_engine;

  // 26.4.5 Engines and engine adaptors with predefined parameters
  typedef linear_congruential_engine<uint_fast32_t, 16807, 0, 2147483647> minstd_rand0;
  typedef linear_congruential_engine<uint_fast32_t, 48271, 0, 2147483647> minstd_rand;
  typedef mersenne_twister_engine<uint_fast32_t, 32,624,397,31,0x9908b0df,11,0xffffffff,7,0x9d2c5680,15,0xefc60000,18,1812433253> mt19937;
  typedef mersenne_twister_engine<uint_fast64_t, 64,312,156,31,0xb5026f5aa96619e9,29,
                                  0x5555555555555555,17, 0x71d67fffeda60000,37, 0xff7eee000000000,43, 6364136223846793005> mt19937_64;
  typedef subtract_with_carry_engine<uint_fast32_t, 24, 10, 24> ranlux24_base;
  typedef subtract_with_carry_engine<uint_fast64_t, 48, 5, 12> ranlux48_base;
  typedef discard_block_engine<ranlux24_base, 223, 23> ranlux24;
  typedef discard_block_engine<ranlux48_base, 389, 11> ranlux48;
  typedef shuffle_order_engine<minstd_rand0,256> knuth_b;

  typedef minstd_rand default_random_engine;

  // 26.4.6 Class random_device
  class random_device;

  // 26.4.7.1 Class seed_seq
  class seed_seq;

  // 26.4.7.2 Function template generate_canonical
  template<class RealType, size_t bits, class UniformRandomNumberGenerator>
  RealType generate_canonical(UniformRandomNumberGenerator& g);

  // 26.4.8.1.1 Class template uniform_int_distribution
  template <class IntType = int>
  class uniform_int_distribution;

  // 26.4.8.1.2 Class template uniform_real_distribution
  template <class RealType = double>
  class uniform_real_distribution;

  // 26.4.8.2.1 Class bernoulli_distribution
  class bernoulli_distribution;
  
  // 26.4.8.2.2 Class template binomial_distribution
  template <class IntType = int>
  class binomial_distribution;
  
  // 26.4.8.2.3 Class template geometric_distribution
  template <class IntType = int>
  class geometric_distribution;
  
  // 26.4.8.2.4 Class template negative_binomial_distribution
  template <class IntType = int>
  class negative_binomial_distribution;
  
  // 26.4.8.3.1 Class template poisson_distribution
  template <class IntType = int>
  class poisson_distribution;
  
  // 26.4.8.3.2 Class template exponential_distribution
  template <class RealType = double>
  class exponential_distribution;
  
  // 26.4.8.3.3 Class template gamma_distribution
  template <class RealType = double>
  class gamma_distribution;
  
  // 26.4.8.3.4 Class template weibull_distribution
  template <class RealType = double>
  class weibull_distribution;
  
  // 26.4.8.3.5 Class template extreme_value_distribution
  template <class RealType = double>
  class extreme_value_distribution;
  
  // 26.4.8.4.1 Class template normal_distribution
  template <class RealType = double>
  class normal_distribution;
  
  // 26.4.8.4.2 Class template lognormal_distribution
  template <class RealType = double>
  class lognormal_distribution;
  
  // 26.4.8.4.3 Class template chi_squared_distribution
  template <class RealType = double>
  class chi_squared_distribution;
  
  // 26.4.8.4.4 Class template cauchy_distribution
  template <class RealType = double>
  class cauchy_distribution;

  // 26.4.8.4.5 Class template fisher_f_distribution
  template <class RealType = double>
  class fisher_f_distribution;
  
  // 26.4.8.4.6 Class template student_t_distribution
  template <class RealType = double>
  class student_t_distribution;
  
  // 26.4.8.5.1 Class template discrete_distribution
  template <class IntType = int>
  class discrete_distribution;
  
  // 26.4.8.5.2 Class template piecewise_constant_distribution
  template <class RealType = double>
  class piecewise_constant_distribution;
  
  // 26.4.8.5.3 Class template general_pdf_distribution
  template <class RealType = double>
  class general_pdf_distribution;




  /************************************************************************/
  /* Implementation                                                       */
  /************************************************************************/

  /**
   *	@brief 26.4.3.1 Class template linear_congruential_engine [rand.eng.lcong]
   *  @details A linear_congruential engine produces random numbers using a linear function <tt>x(i+1) := (a * x(i) + c) mod m</tt>.
   **/
  template <class UIntType, UIntType a, UIntType c, UIntType m>
  class linear_congruential_engine
  {
    static_assert(is_integral<UIntType>::value && is_unsigned<UIntType>::value, "Template parameter UIntType shall denote an unsigned integral type.");
    typedef integral_constant<bool, m == 0> zerom;
  public:
    // types
    typedef UIntType result_type;

    ///\name engine characteristics
    static const result_type multiplier = a;
    static const result_type increment = c;
    static const result_type modulus = m;   // max+1 == 0
    static const result_type default_seed = 1u;
    static_assert(m == 0 || (a < m && c < m), "following relations shall hold: a < m and c < m");

    static constexpr result_type min() { return increment == 0u ? 1u : 0u; }
    static constexpr result_type max() { return modulus - 1u; }

    
    ///\name constructors and seeding functions
    /** Constructs a linear_congruential_engine object. */
    explicit linear_congruential_engine(result_type s = default_seed)
    {
      seed(s, zerom());
    }

    explicit linear_congruential_engine(seed_seq& q);

    /** If <tt>c mod m</tt> is 0 and <tt>s mod m</tt> is 0, 
      sets the engine's state to 1, otherwise sets the engine's state to <tt>s mod m</tt>.  */
    void seed(result_type s = default_seed)
    {
      seed(s, zerom());
    }

    void seed(seed_seq& q);
    
    ///\name generating functions
    result_type operator()()
    {
      return produce(zerom());
    }
    void discard(unsigned long long z)
    {
      while(z--)
        produce(zerom());
    }
    ///\}

    friend bool operator==(const linear_congruential_engine& x, const linear_congruential_engine& y) { return x.state_ == y.state_; }

  private:
    // DivByZero prevention
    result_type produce(false_type) { return state_ = (multiplier * state_ + increment) % modulus; }
    result_type produce( true_type) { return state_ =  multiplier * state_ + increment; }

    void seed(result_type s, false_type) { state_ = (increment % modulus == 0 && s % modulus == 0) ? 1 : s % modulus; }
    void seed(result_type, true_type) { state_ = 1; }

  private:
    result_type state_;
  };


  /**
   *	@brief 26.4.4.1 Class template discard_block_engine [rand.adapt.disc]
   *  @details A discard_block engine produces random numbers from some base engine by discarding blocks of data.
   **/
  template <class Engine, size_t p, size_t r>
  class discard_block_engine
  {
  public:
    ///\name types
    typedef Engine base_type;
    typedef typename base_type::result_type result_type;

    ///\name engine characteristics
    static const size_t block_size = p;
    static const size_t used_block = r;
    static constexpr result_type min() { return base_type::min(); }
    static constexpr result_type max() { return base_type::max(); }

    ///\name constructors and seeding functions
    discard_block_engine()
      :n(0)
    {}
    explicit discard_block_engine(const base_type& urng)
      :e(urng), n(0)
    {}
    explicit discard_block_engine(result_type s)
      :e(s), n(0)
    {}
    explicit discard_block_engine(seed_seq& q)
      :e(q), n(0)
    {}
    void seed()
    {
      e.seed(), n = 0;
    }
    void seed(result_type s)
    {
      e.seed(s), n = 0;
    }
    void seed(seed_seq& q)
    {
      e.seed(q), n = 0;
    }

    ///\name generating functions
    result_type operator()()
    {
      if(n >= used_block){
        size_t i = block_size - used_block;
        while(i--)
          e();
        n = 0;
      }
      ++n;
      return e();
    }

    void discard(unsigned long long z)
    {
      while(z--)
        this->operator ()();
    }

    ///\name property functions
    const base_type& base() const { return e; }
    ///\}
  private:
    base_type e;
    size_t n;
  };


  /**
   *	@brief    26.4.7.1 Class seed_seq [rand.util.seedseq]
   *  @details  An object of type seed_seq consumes a sequence of integer-valued data and produces a fixed number of
   *  unsigned integer values, 0 <= i < 232, based on the consumed data.
   *  @note   Such an object provides a mechanism to avoid replication of streams of random variates.
   *  This can be useful in applications requiring large numbers of random number engines.
   **/
  class seed_seq
  {
  public:
    ///\name types
    typedef uint_least32_t result_type;

    ///\name constructors
    /** Constructs a seed_seq object */
    seed_seq()
    {}

    /** Constructs a seed_seq object by rearranging some or all of the bits of the supplied sequence <tt>[begin,end)</tt> of w-bit quantities into 32-bit units */
    template<class InputIterator>
    seed_seq(InputIterator begin, InputIterator end, size_t u = numeric_limits<typename iterator_traits<InputIterator>::value_type>::digits);

    ///\name generating functions
    template<class RandomAccessIterator>
    void generate(RandomAccessIterator begin, RandomAccessIterator end) const;

    ///\name property functions
    /** Returns the number of 32-bit units that would be returned by a call to param(). */
    size_t size() const { return v.size(); }

    /** Copies the sequence of prepared 32-bit units to the given destination. */
    template<class OutputIterator>
    void param(OutputIterator dest) const { copy(v.cbegin(), v.cend(), dest); }

    ///\}
private:
  vector<result_type> v;
};

  /**@} lib_numeric_rand */
  /**@} lib_numeric */
}
#endif // NTL__STLX_RANDOM
