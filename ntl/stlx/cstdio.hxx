/**\file*********************************************************************
 *                                                                     \brief
 *  C Library files utilities [c.files]
 *
 *\note There should be no sprintf and other buffer-overflowing stuff.
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CSTDIO
#define NTL__STLX_CSTDIO

#include "array.hxx"
#include "cstddef.hxx"
#include "cstdarg.hxx"
#include "memory.hxx"
#include "string.hxx"
#include "nt/peb.hxx"

#ifndef _INC_STDLIB// MSVC compatibility

using std::va_list;

NTL__EXTERNAPI size_t __cdecl
  _snprintf(char *buffer, size_t count, const char *format, ...);

NTL__EXTERNAPI size_t __cdecl
  _vsnprintf(char *buffer, size_t count, const char *format, va_list argptr);

#endif


namespace std {

/**\addtogroup  lib_language_support *** Language support library [18] ******
 *@{*/

/**\addtogroup  lib_general_utilities ** C Library filees [c.files  ] *******
 *@{*/

#ifndef _INC_STDLIB// MSVC compatibility

  using ::_snprintf; using ::_vsnprintf;

#else

  using ::va_list;

#endif

#ifndef MAX_PATH
  #define MAX_PATH 260
#endif

#ifndef L_tmpnam
  #define L_tmpnam (MAX_PATH-14)
#endif

namespace __subsystem {

/// C (rulez forever) file interface
template<class F>
struct file_api : private F
{
  typedef file_api<F> this_type;
  typedef F file_type;

  typedef typename F::unspecified_bool_type unspecified_bool_type;
  operator unspecified_bool_type() const { return *reinterpret_cast<const file_type*>(this); };

  using F::access_mask;
  using F::read_access;
  using F::write_access;
  using F::append;

  using F::creation_disposition;
  using F::supersede;
  using F::open_if;

  /// mode: "rb" open binary file for reading
  ///       "wb" truncate to zero length or create binary file for writing
  ///       "ab" append; open or create binary file for writing at end-of-file
  ///       "r+b" or "rb+" open binary file for update (reading and writing)
  ///       "w+b" or "wb+" truncate to zero length or create binary file for update
  ///       "a+b" or "ab+" append; open or create binary file for update, writing at end-of-file
  static inline
  typename F::access_mask translate_access_mask(const char * __restrict mode)
  {
    bool const plus = (mode[1] == '+') ? true
                    : (mode[1] == 'b' && mode[2] == '+') ? true
                    : false;
    switch ( mode[0] )
    {
      case 'r': return plus ? F::read_access | F::write_access : F::read_access;
      case 'w': return plus ? F::read_access | F::write_access : F::write_access;
      case 'a': return plus ? F::read_access | F::append : F::append;
      default: return F::access_mask();
    }
  }
  
  static inline
  typename F::creation_disposition translate_creation_disposition(const char * __restrict mode)
  {
    return mode[0] == 'w' ? F::supersede : F::open_if;
  }

  using F::convert_filename;

  using F::close;
  using F::flush;
  using F::create;
  using F::open;
  using F::rename;
  using F::remove;
  using F::read;
  using F::write;
  using F::getpos;
  using F::setpos;
  using F::size;
  
  ///}
};

}//namespace __subsystem {

using namespace __subsystem;
typedef file_api<ntl::file> FILE;

#define SEEK_CUR  1
#define SEEK_END  2
#define SEEK_SET  0

#define stderr reinterpret_cast<FILE*>(&ntl::nt::peb::instance().ProcessParameters->StandardError)
#define stdin  reinterpret_cast<FILE*>(&ntl::nt::peb::instance().ProcessParameters->StandardInput)
#define stdout reinterpret_cast<FILE*>(&ntl::nt::peb::instance().ProcessParameters->StandardOutput)

///\name 7.19.4 Operations on files

/// 7.19.4.1 The remove function
template<typename StringT>
inline
 int remove(const StringT& filename)
{
  FILE f;
  f.open(filename);
  return f.remove() ? 0 : -1;;
}

int remove(const char* filename)
{ 
  return remove(FILE::convert_filename(filename));
}

/// 7.19.4.2 The rename function
template<typename StringT>
inline
int rename(const StringT& oldname, const StringT& newname)
{
  FILE f;
  f.open(oldname);
  return f.rename(newname) ? 0 : -1;;
}

inline
int rename(const char* oldname, const char* newname)
{
  return rename(FILE::convert_filename(oldname), FILE::convert_filename(newname));
}

/// 7.19.4.4 The tmpnam function
char* tmpnam(char* /*s*/)
{
  //  return F::tmpnam(s ? s : array<char, L_tmpnam>().begin());
  return 0;
}

/// 7.19.4.3 The tmpfile function
FILE* tmpfile();// { return new (nothrow) FILE(tmpnam()); }


///\name 7.19.5 File access functions

/// 7.19.5.1 The fclose function
inline int fclose(FILE* stream)
{
  delete stream;
  return 0;
}

/// 7.19.5.2 The fflush function
inline int fflush(FILE* stream) { return stream->flush() ? 0 : EOF; }

/// 7.19.5.3 The fopen function
inline
FILE*
  fopen(const char* __restrict filename, const char* __restrict mode)
{
  auto_ptr<FILE> f ( new (nothrow) FILE );
  return f->create( FILE::convert_filename(filename),
                    FILE::translate_creation_disposition(mode),
                    FILE::translate_access_mask(mode) )
                  ? f.release() : nullptr;
}

/// 7.19.5.4 The freopen function
inline
FILE*
  freopen(const char * __restrict filename, const char * __restrict mode, FILE* __restrict stream)
{
  stream->close();
  ///\note filename == 0  It is implementation-defined which changes of mode are
  ///                     permitted (if any), and under what circumstances.
  return stream->create( FILE::convert_filename(filename),
                         FILE::translate_creation_disposition(mode),
                         FILE::translate_access_mask(mode) )
                       ? stream : nullptr;
}

/// 7.19.5.5 The setbuf function
inline void setbuf(FILE* __restrict /*stream*/, char * __restrict /*buf*/)
{
 ;
}

/// 7.19.5.6 The setvbuf function
inline int setvbuf(FILE* __restrict /*stream*/, char * __restrict /*buf*/, int /*mode*/, size_t /*size*/)
{
  return -1;
}


///\name 7.19.6 Formatted input/output functions

/// 7.19.6.1 The fprintf function
int fprintf(FILE* __restrict stream, const char * __restrict format, ...);

/// 7.19.6.2 The fscanf function
int fscanf(FILE* __restrict stream, const char * __restrict format, ...);

/// 7.19.6.3 The printf function
int printf(const char * __restrict format, ...);

/// 7.19.6.5 The snprintf function
//int snprintf(char * __restrict s, size_t n, const char * __restrict format, ...);

/// 7.19.6.6 The sprintf function
//int sprintf(char * __restrict s, const char * __restrict format, ...);

/// 7.19.6.7 The sscanf function
NTL__EXTERNAPI int __cdecl
  sscanf(const char * __restrict s, const char * __restrict format, ...);


///\name 7.19.7 Character input/output functions

/// 7.19.7.1 The fgetc function
inline int fgetc(FILE *stream);

/// 7.19.7.2 The fgets function
//inline char *fgets(char * __restrict s, int n, FILE * __restrict stream);

/// 7.19.7.3 The fputc function
inline int fputc(int c, FILE *stream);

/// 7.19.7.4 The fputs function
inline int fputs(const char * __restrict s, FILE * __restrict stream);

/// 7.19.7.5 The getc function
inline int getc(FILE *stream);

/// 7.19.7.6 The getchar function
inline int getchar(void)
{
  return getc(stdin);
}

/// 7.19.7.7 The gets function
//inline char *gets(char *s)

/// 7.19.7.8 The putc function
inline int putc(int c, FILE *stream);

/// 7.19.7.9 The putchar function
inline int putchar(int c)
{
  return putc(c, stdout);
}

/// 7.19.7.10 The puts function
inline int puts(const char *s)
{
  return fputs(s, stdout) | fputs("\n", stdout);
}
  
/// 7.19.7.11 The ungetc function
inline int ungetc(int c, FILE *stream);


///\name 7.19.8 Direct input/output functions

/// 7.19.8.1 The fread function
inline size_t
  fread(void * __restrict ptr, size_t size, size_t nmemb, FILE * __restrict stream)
{
  size_t bytes = size * nmemb;
  stream->read(ptr, bytes);
  return bytes / size;
}

/// 7.19.8.2 The fwrite function
inline size_t
  fwrite(const void * __restrict ptr, size_t size, size_t nmemb, FILE * __restrict stream)
{
  size_t bytes = size * nmemb;
  stream->write(ptr, bytes);
  return bytes / size;
}


///\name 7.19.9 File positioning functions

typedef long long fpos_t;

/// 7.19.9.1 The fgetpos function
inline int fgetpos(FILE * __restrict stream, fpos_t * __restrict pos)
{
  return stream->getpos(*pos) ? 0 : EOF;
}


/// 7.19.9.2 The fseek function
inline int fseek(FILE *stream, long long offset, int whence)
{
  fpos_t oldoff;
  switch ( whence )
  {
    case SEEK_SET: oldoff = 0; break;
    case SEEK_END: oldoff = stream->size(); break;
    case SEEK_CUR: if ( stream->getpos(oldoff) ) break;
    default:  return EOF;
  }
  return stream->setpos(oldoff + offset) ? 0 : EOF;
}

/// 7.19.9.3 The fsetpos function
inline int fsetpos(FILE *stream, const fpos_t *pos)
{
  return stream->setpos(*pos) ? 0 : EOF;
}

/// 7.19.9.4 The ftell function
inline long long ftell(FILE *stream)
{
  long long off;
  return stream->getpos(off) ? off : EOF;
}

/// 7.19.9.5 The rewind function
inline void rewind(FILE *stream)
{
  fseek(stream, 0L, SEEK_SET);
}


///\name 7.19.10 Error-handling functions

/// 7.19.10.1 The clearerr function
inline void clearerr(FILE * /*stream*/) {;}

/// 7.19.10.2 The feof function
inline int feof(FILE *stream)
{
  long long off;
  return stream->getpos(off) && off != stream->size() ? 0 : EOF;
}

/// 7.19.10.3 The ferror function
inline int ferror(FILE *stream);

///\}

//#define EOF in string.hxx


/**@} lib_general_utilities
 */
/**@} lib_language_support */

}//namespace std

#endif //#ifndef NTL__STLX_CSTDIO
