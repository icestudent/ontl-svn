

# Supported compilers #
  * MSVC 2005 SP1 (v14.00.50727) or newer
  * MSVC 2010 (v16), 2011 (v17), 2013 (v18)
  * Intel C++ 10.0 (or older?), 11.0, 12.0
  * Codegear C++ 2009 (v6.16) or newer
  * GCC 4.2 or newer
  * Clang 2.9 or newer



## MSVC 2005 ##

Fully supported, but no C++0x specific features available (however some of them are emulated, such as `static_assert`, `nullptr`, alignment).

## MSVC 2010 and higher ##

Provides [part of C++0x Core Language features](http://blogs.msdn.com/b/vcblog/archive/2010/04/06/c-0x-core-language-features-in-vc10-the-table.aspx).

2012 have [more C++11 features](http://blogs.msdn.com/b/vcblog/archive/2011/09/12/10209291.aspx).

2013 [preview](http://blogs.msdn.com/b/vcblog/archive/2013/06/27/what-s-new-for-visual-c-developers-in-vs2013-preview.aspx), [RC](http://msdn.microsoft.com/en-us/library/vstudio/hh409293(v=vs.120).aspx), [RTM and CTP](http://blogs.msdn.com/b/vcblog/archive/2013/12/02/c-11-14-core-language-features-in-vs-2013-and-the-nov-2013-ctp.aspx) features.

2014 [CTP1](http://blogs.msdn.com/b/vcblog/archive/2014/06/11/c-11-14-feature-tables-for-visual-studio-14-ctp1.aspx)

Future versions [roadmap](https://udta1g.blu.livefilestore.com/y2pMXBJL7l2a5UOf_pXnLXghSUhPWK8w5skFyc50SVFcMjVwa1guQnM6R0NNLN1buBUNPGbLBejpYXXBXSbqshQKKWVfQxvJjk2jGRPPbL-UBu7gaao4RxifZgPXY5ksdei/image1.png?psid=1).

Fully supported and recommended compiler.

## Intel C++ 10.0 ##

It is compatible with the MSVC2005 and 2008, so it is fully supported except of 10 bytes size `double` type (disabled by default).

## Intel C++ 11.0 and higher ##

11.0: it is the first C++ compiler version from Intel which has some C++0x [specific features](http://software.intel.com/en-us/articles/c0x-features-supported-by-intel-c-compiler/) (e.g. `static_assert`, `auto`, lambdas, extern templates, `decltype`), but no rvalue references, so it is supported as MSVC2005/2008.

12.0 and higher: newest C++ compiler with lot of [C++0x features](http://software.intel.com/en-us/articles/c0x-features-supported-by-intel-c-compiler/).

## Codegear C++ Builder 2009 ##

New compiler of the C++ Builder has a [lot of C++0x features](http://docwiki.embarcadero.com/RADStudio/en/C%2B%2B0x_Features_Index) ([extended unicode character types](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2249.html) (UTF-16 and UTF-32), `alignof`, `static_assert`, `decltype`, strongly typed enums, extern templates and rvalues).

It id supported only in [STLx](http://ontl.googlecode.com/svn/branches/stlx) branch with the external runtime.

## GCC ##

GCC's C++ Frontend has an [experimental C++0x support](http://gcc.gnu.org/projects/cxx0x.html) (in 4.3+ version). Compilers prior to 4.3 are not fully supported due to absence of the special built-in type traits.

It is fully supported only in [STLx](http://ontl.googlecode.com/svn/branches/stlx) branch with the external C runtime library.


## Clang ##

It is a compiler front end for the C, C++, Objective-C, and Objective-C++ programming languages, which uses LLVM as its back end.

Provides support for a [number of features](http://clang.llvm.org/cxx_status.html) of C++11.

Planned to use as NTL compiler.