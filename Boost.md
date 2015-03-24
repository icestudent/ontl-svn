It is not smooth but still possible to build Boost over NTL.

Here is a quick hack to build boost::asio::

Add the following paths to INCLUDE environment. The order is mandatory.
  * ntl/
  * ntl/stlx/cstd/
  * WDK/7600.16385.1/inc/api
  * WDK/7600.16385.1/inc/crt
  * boost\_1\_42\_0/
(of course these must be full)

Include the following source before anything
```
// Do the following before including any Boost header

//#undef BOOST_ASSERT_CONFIG

//#if defined(NTL__CPPLIB)
//boost/config/ntl.hpp

#define BOOST_STDLIB "NTL standard library"

#define __STDC_WANT_SECURE_LIB__ 0
#define __STRALIGN_H_
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501
#include <Windows.h>

#define BOOST_HAS_STDINT_H
#define BOOST_HAS_TR1
#define BOOST_HAS_THREADS
//#define BOOST_HAS_WINTHREADS
#define BOOST__STDC_CONSTANT_MACROS_DEFINED
#define BOOST_DATE_TIME_HAS_REENTRANT_STD_FUNCTIONS

// I see these have no effect at all:
//#  define BOOST_NO_0X_HDR_ARRAY
//#  define BOOST_NO_0X_HDR_CHRONO
//#  define BOOST_NO_0X_HDR_CODECVT
#  define BOOST_NO_0X_HDR_CONCEPTS
//#  define BOOST_NO_0X_HDR_CONDITION_VARIABLE
#  define BOOST_NO_0X_HDR_CONTAINER_CONCEPTS
#  define BOOST_NO_0X_HDR_FORWARD_LIST
#  define BOOST_NO_0X_HDR_FUTURE
//#  define BOOST_NO_0X_HDR_INITIALIZER_LIST
//#  define BOOST_NO_0X_HDR_ITERATOR_CONCEPTS
//#  define BOOST_NO_0X_HDR_MEMORY_CONCEPTS
#  define BOOST_NO_0X_HDR_MUTEX
#  define BOOST_NO_0X_HDR_RANDOM
#  define BOOST_NO_0X_HDR_RATIO
//#  define BOOST_NO_0X_HDR_REGEX
//#  define BOOST_NO_0X_HDR_SYSTEM_ERROR
#  define BOOST_NO_0X_HDR_THREAD
#  define BOOST_NO_0X_HDR_TUPLE
#  define BOOST_NO_0X_HDR_TYPE_TRAITS
//#  define BOOST_NO_STD_UNORDERED        // deprecated;
//#  define BOOST_NO_0X_HDR_UNORDERED_MAP
//#  define BOOST_NO_0X_HDR_UNORDERED_SET

#pragma warning (disable: 4290)// C++ exception specification ignored except to indicate a function is not __declspec(nothrow)

#define BOOST_DATE_TIME_HAS_HIGH_PRECISION_CLOCK

#define BOOST_NO_STD_MESSAGES
#define BOOST_MULTI_INDEX_DISABLE_SERIALIZATION

#define BOOST_ARRAY_HPP
#define NTL__ARRAY_BOOST_COMPATIBLE
#include <array>
namespace boost { using std::array; }

#define BOOST_THREAD_THREAD_HPP
#include <thread>
#define BOOST_THREAD_MUTEX_HPP
#include <mutex>
#define BOOST_THREAD_CONDITION_HPP
#define BOOST_THREAD_CONDITION_VARIABLE_HPP
//#include <condition_variable>
#define BOOST_THREAD_FUTURE_HPP
#include <future>
namespace boost { 
  using std::thread;
  using std::mutex;
  using std::unique_lock;
}

#define BOOST_ASIO_DETAIL_THREAD_HPP
namespace boost { namespace asio { namespace detail {
  using std::thread;
} } }

//namespace std {
__forceinline
void//errno_t
  strcpy_s(char *Destination, size_t n, const char *Source)
{
  *Destination = '\0';
  strncat(Destination, Source, n);
}

int sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, ...);
//}

#include <iostream>

#define BOOST_SYSTEM_ERROR_HPP
#define BOOST_ERROR_CODE_HPP
#include <system_error>
namespace boost { namespace system {
  using std::system_error;
  using std::error_code;
  using std::error_category;

  // just a dummy to work arround  
  // boost/asio/error.hpp(370): error C2888: 'std::is_error_code_enum<boost::asio::error::basic_errors>' : symbol cannot be defined within namespace 'system'
  template <typename T>struct is_error_code_enum : std::is_error_code_enum<T> {};

  inline const std::error_category& get_system_category() { return std::system_category(); }
}}

#include <boost/asio/error.hpp>

template<> struct std::is_error_code_enum<boost::asio::error::basic_errors>     : std::true_type {};
template<> struct std::is_error_code_enum<boost::asio::error::netdb_errors>     : std::true_type {};
template<> struct std::is_error_code_enum<boost::asio::error::addrinfo_errors>  : std::true_type {};
template<> struct std::is_error_code_enum<boost::asio::error::misc_errors>      : std::true_type {};
template<> struct std::is_error_code_enum<boost::asio::error::ssl_errors>       : std::true_type {};

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

```

Please note Boost at the moment does not officially support NTL. We will work at this later.