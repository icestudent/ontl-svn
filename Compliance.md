

# 17.6.1.3 Freestanding implementations #

2 A freestanding implementation has an implementation-defined set of headers. This set shall include at least the following
headers:
|18.2 Types |`<cstddef>`|
|:----------|:----------|
|18.3 Implementation properties |`<cfloat><limits><climits>`|
|18.4 Integer types |`<cstdint>`|
|18.5 Start and termination |`<cstdlib>`|
|18.6 Dynamic memory management |`<new>`|
|18.7 Type identification |`<typeinfo>`|
|18.8 Exception handling |`<exception>`|
|18.9 Initializer lists |`<initializer_list>`|
|18.10 Other runtime support |`<cstdalign> <cstdarg> <cstdbool>`|
|20.9 Type traits |`<type_traits>`|
|29 Atomics |`<atomic>`|

3 The supplied version of the header 

&lt;cstdlib&gt;

 shall declare at least the functions `abort()`, `atexit()`, `at_quick_exit()`, `exit()` and `quick_exit()`.

---


## C++ Features requiring runtime support ##
  1. `try`/`catch` works even in kernel mode with a few limitations ([ehsup.asm](http://code.google.com/p/ontl/source/browse/branches/x64/ntl/rtl/ehsup.asm), [eh.cpp](http://code.google.com/p/ontl/source/browse/branches/x64/ntl/rtl/eh.cpp)).
  1. Runtime type info and `dynamic_cast<>` ([rtti.cpp](http://code.google.com/p/ontl/source/browse/branches/x64/ntl/rtl/rtti.cpp)).
  1. Static object ctors/dtors support ([crt.cpp](http://code.google.com/p/ontl/source/browse/branches/x64/ntl/rtl/crt.cpp)).
  1. I/O Streams support (crt.cpp, [iostream.cpp](http://code.google.com/p/ontl/source/browse/branches/x64/ntl/rtl/iostream.cpp); [wchar\_mask\_data.cpp](http://code.google.com/p/ontl/source/browse/branches/x64/ntl/rtl/wchar_mask_data.cpp) for `wchar_t` specializations).
  1. `thread_local` not supported yet.
  1. floats - FPU flags not compatible to Standard (`<cmath>` runtime in [math.cpp](http://code.google.com/p/ontl/source/browse/branches/x64/ntl/rtl/math.cpp)).

To enable various runtime features, include corresponding [ntl/rtl/\*](http://code.google.com/p/ontl/source/browse/branches/x64/ntl/rtl/) files in the build list or project file.



## C++ Library Headers (§17.6.1.2/2) ##
Note that **[trunk/ntl](http://code.google.com/p/ontl/source/browse/trunk/ntl/)** codebase is **not supported** and deprecated, it will be **replaced with the [branches/x64](http://code.google.com/p/ontl/source/browse/branches/x64/) branch**.

| # | 

&lt;algorithm&gt;

             | About 45%.|
|:--|:----------------------------------|:----------|
| # | 

&lt;array&gt;

                 | Complete. |
| # | 

&lt;atomic&gt;

                | No 64bit types support in x86, not conforming the N3000 |
| # | 

&lt;bitset&gt;

                | Complete. |
| # | 

&lt;chrono&gt;

                | Complete. |
| # | 

&lt;codecvt&gt;

               | Work in progress |
| # | 

&lt;complex&gt;

               | Not planned in the near future. |
| # | 

<condition\_variable>

    |  |
| # | 

&lt;deque&gt;

                 | Complete. |
| # | 

&lt;exception&gt;

             | Complete. |
| # | 

<forward\_list>

          | tested; incomplete (no `sort()`) |
| # | 

&lt;fstream&gt;

               | Almost complete (no seekpos) |
| # | 

&lt;functional&gt;

            | almost complete, but arguments count are limited (in development) |
| # | 

&lt;future&gt;

                | Not conforming the N3000 |
| # | 

<initializer\_list>

      | No compilers to support it (except GCC 4.4+). |
| # | 

&lt;iomanip&gt;

               | Complete. |
| # | 

&lt;ios&gt;

                   | No `ios_base` callbacks. |
| # | 

&lt;iosfwd&gt;

                | Complete. |
| # | 

&lt;iostream&gt;

              | Complete for console io. |
| # | 

&lt;istream&gt;

               | Almost complete (FP uses `msvcrt`). |
| # | 

&lt;iterator&gt;

              | Complete. |
| # | 

&lt;limits&gt;

                | Complete except of `numeric_limits<long double>::infinity()` & Co for ICC (10 bytes in size). |
| # | 

&lt;list&gt;

                  | No `merge` yet. |
| # | 

&lt;locale&gt;

                | Only "C" locale (no time, collate, messages and w/o locale mixins) |
| # | 

&lt;map&gt;

                   | `std::map` only |
| # | 

&lt;memory&gt;

                | Incomplete (no `scoped_allocator`). |
| # | 

&lt;mutex&gt;

                 | Complete (see `functional` for `call_once`) |
| # | 

&lt;new&gt;

                   | Complete. |
| # | 

&lt;numeric&gt;

               | Only `std::accumulate`. |
| # | 

&lt;ostream&gt;

               | Almost complete (FP uses `msvcrt`). |
| # | 

&lt;queue&gt;

                 | No `priority_queue`. |
| # | 

&lt;random&gt;

                | Incomplete |
| # | 

&lt;ratio&gt;

                 | Complete. |
| # | 

&lt;regex&gt;

                 |  |
| # | 

&lt;set&gt;

                   | Complete. |
| # | 

&lt;sstream&gt;

               | Complete. |
| # | 

&lt;stack&gt;

                 | Complete, but on top of `vector`. |
| # | 

&lt;stdexcept&gt;

             | Complete. |
| # | 

&lt;streambuf&gt;

             | Complete. |
| # | 

&lt;string&gt;

                | Complete. |
| # | 

&lt;strstream&gt;

             | Complete. |
| # | 

<system\_error>

          | Complete. |
| # | 

&lt;thread&gt;

                | Complete (see `functional`) |
| # | 

&lt;tuple&gt;

                 | Complete; limited type count. |
| # | 

&lt;typeinfo&gt;

              | Complete. |
| # | 

&lt;typeindex&gt;

             | Complete. |
| # | 

<type\_traits>

           | Based on old traits (N2960). |
| # | 

<unordered\_map>

         | Complete, untested. |
| # | 

<unordered\_set>

         | Complete, untested. |
| # | 

&lt;utility&gt;

               | Complete. |
| # | 

&lt;valarray&gt;

              | Not planned in near future. |
| # | 

&lt;vector&gt;

                | Without `emplace`. |

## C++ Headers for C Library Facilities (§17.6.1.2/3) ##
| # | 

&lt;cassert&gt;

   | Complete. |
|:--|:----------------------|:----------|
| # | 

&lt;ccomplex&gt;

  | Not planned. |
| # | 

&lt;cctype&gt;

    |  |
| # | 

&lt;cerrno&gt;

    | Not planned. |
| # | 

&lt;cfenv&gt;

     |  |
| # | 

&lt;cfloat&gt;

    | Complete. But no runtime support for floats yet. |
| # | 

&lt;cinttypes&gt;

 | Complete. |
| # | 

&lt;ciso646&gt;

   | Dummy header. |
| # | 

&lt;climits&gt;

   | Complete. |
| # | 

&lt;clocale&gt;

   | Not planned. |
| # | 

&lt;cmath&gt;

     | Not planned. |
| # | 

&lt;csetjmp&gt;

   | Not planned. |
| # | 

&lt;csignal&gt;

   | Not planned. |
| # | 

&lt;cstdalign&gt;

 | Dummy header. |
| # | 

&lt;cstdarg&gt;

   | Complete. |
| # | 

&lt;cstdbool&gt;

  | Dummy header. |
| # | 

&lt;cstddef&gt;

   | No `NULL` macro, use `nullptr` instead. |
| # | 

&lt;cstdint&gt;

   | Complete. |
| # | 

&lt;cstdio&gt;

    | Incomplete. |
| # | 

&lt;cstdlib&gt;

   | `FILE` supports only binary mode. `atexit` in x64 branch only. No buffer-overflowing stuff included. |
| # | 

&lt;cstring&gt;

   | No `strdup`. |
| # | 

&lt;ctgmath&gt;

   | None as ccomplex. |
| # | 

&lt;ctime&gt;

     | Incomplete. |
| # | 

&lt;cuchar&gt;

    | Incomplete. |
| # | 

&lt;cwchar&gt;

    | Incomplete. |
| # | 

&lt;cwctype&gt;

   | Compliance |


# Technical Report on C++ Library extensions #

## Technical Report 1 ##

[Draft](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1836.pdf)

The technical report describes extensions to the C++ standard library that is described in the International Standard for the C++ programming language. Almost all library components from it are moved to the C++0x Standard.

NTL contains following separate components, which are based on TR1:
| # | 2.2 Smart pointers | [<tr1/smart\_ptr.hxx>](http://code.google.com/p/ontl/source/browse/branches/x64/ntl/stlx/ext/tr1/smart_ptr_tr1.hxx) |
|:--|:-------------------|:--------------------------------------------------------------------------------------------------------------------|


## Technical Report 2 ##

[C++0x Standard Library wishlist](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n2034.htm)

The TR2 was cancelled in '08 and not published, but you can find C++0x Library extensions in the wishlist.

NTL contains following separate components, which are based on TR2:
| # | [Filesystem Library Proposal](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n1975.html) | [<tr2/filesystem.hxx>](http://code.google.com/p/ontl/source/browse/branches/x64/ntl/stlx/ext/tr2/filesystem.hxx) (old), [<tr2/files.hxx>](http://code.google.com/p/ontl/source/browse/branches/devel/ntl/stlx/ext/tr2/files.hxx) |
|:--|:--------------------------------------------------------------------------------------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| # | [Hierarchical Data Structures](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n2101.html) | planned |
| # | [Networking Library Proposal](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n2054.pdf) | Works with winsock, planned for KM. |