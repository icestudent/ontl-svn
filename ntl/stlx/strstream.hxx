/**\file*********************************************************************
 *                                                                     \brief
 *  N2315 D.7 char* streams [depr.str.strstreams]
 *
 *  ///\note deprecated by Standard
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_STRSTREAM
#define NTL__STLX_STRSTREAM

#include "string.hxx"
#include "iosfwd.hxx"
#include "streambuf.hxx"


namespace std {


/**\defgroup  str_strstreams ********** Strings streams [D.7] ***************
 *@{*/

///D.7.1 Class strstreambuf [depr.strstreambuf]
class strstreambuf : public basic_streambuf<char>
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    ///\name D.7.1.1 strstreambuf constructors [depr.strstreambuf.cons]
 
    explicit strstreambuf(streamsize alsize_arg = 0)
    : basic_streambuf(),
      strmode(dynamic), alsize(alsize_arg), palloc(0), pfree(0)
    {/**/}
    
    strstreambuf(void* (*palloc_arg)(size_t), void (*pfree_arg)(void*))
    : basic_streambuf(),/*alsize(alsize_arg), an unspecified value*/
      strmode(dynamic), palloc(palloc_arg), pfree(pfree_arg)
    {/**/}

    strstreambuf(char* gnext_arg, streamsize n, char* pbeg_arg = 0)
    : basic_streambuf(),/*alsize(alsize_arg), an unspecified value*/
      strmode(), palloc(0), pfree(0)
    {
      _construct(gnext_arg, n, pbeg_arg);
    }
    
    strstreambuf(const char* gnext_arg, streamsize n)
    : basic_streambuf(),/*alsize(alsize_arg), an unspecified value*/
      strmode(constant), palloc(0), pfree(0)
    {
      _construct(gnext_arg, n);
    }

    strstreambuf(signed char* gnext_arg, streamsize n, signed char* pbeg_arg = 0)
    : basic_streambuf(),/*alsize(alsize_arg), an unspecified value*/
      strmode(), palloc(0), pfree(0)
    {
      _construct(reinterpret_cast<char*>(gnext_arg), n, reinterpret_cast<char*>(pbeg_arg));
    }
    
    strstreambuf(const signed char* gnext_arg, streamsize n)
    : basic_streambuf(),/*alsize(alsize_arg), an unspecified value*/
      strmode(constant), palloc(0), pfree(0)
    {
      _construct(reinterpret_cast<const char*>(gnext_arg), n);
    }

    strstreambuf(unsigned char* gnext_arg, streamsize n, unsigned char* pbeg_arg = 0)
    : basic_streambuf(),/*alsize(alsize_arg), an unspecified value*/
      strmode(), palloc(0), pfree(0)
    {
      _construct(reinterpret_cast<char*>(gnext_arg), n, reinterpret_cast<char*>(pbeg_arg));
    }

    strstreambuf(const unsigned char* gnext_arg , streamsize n )
    : basic_streambuf(),/*alsize(alsize_arg), an unspecified value*/
      strmode(constant), palloc(0), pfree(0)
    {
      _construct(reinterpret_cast<const char*>(gnext_arg), n);
    }

    virtual ~strstreambuf()
    {
      if ( (strmode & allocated) && !(strmode & frozen) )
      {
        _free(eback());
      }
    }

    ///\name  D.7.1.2 Member functions [depr.strstreambuf.members]

    void freeze(bool freezefl = true)
    {
    //  if ( strmode & dynamic )
      strmode = strmode | frozen ^ (frozen & strstate(freezefl-1));
    }

    char* str()
    {
      freeze();
      return gptr();
    }
    
    streamsize pcount()
    {
      return !pptr() ? reinterpret_cast<streamsize>(pptr()) : pptr() - pbase();
    }

  ///////////////////////////////////////////////////////////////////////////
  protected:

    ///\name D.7.1.3 strstreambuf overridden virtual functions [depr.strstreambuf.virtuals]

    /// 1 Effects: Appends the character designated by c to the output sequence,
    ///   if possible, in one of two ways:
    /// — If c != EOF and if either the output sequence has a write position
    ///   available or the function makes a write position available (as described below),
    ///   assigns c to *pnext++.
    /// 2 Returns (unsigned char)c.
    /// — If c == EOF, there is no character to append.
    /// 3 Returns a value other than EOF.
    /// 4 Returns EOF to indicate failure.
    /// 5 Remarks: The function can alter the number of write positions
    ///   available as a result of any call.
    /// 6 To make a write position available, the function reallocates
    ///   (or initially allocates) an array object with a sufficient number
    ///   of elements n to hold the current array object (if any), plus at least
    ///   one additional write position. How many additional write positions are
    ///   made available is otherwise unspecified. [...]
    ///   If the allocation fails, the function returns EOF.
    ///   Otherwise, it sets allocated in strmode.
    /// 8 If strmode & dynamic == 0, or if strmode & frozen != 0, the function
    ///   cannot extend the array (reallocate it with greater length)
    ///   to make a write position available.
    virtual int_type overflow(int_type c = EOF)
    {
      if ( c == EOF )
        return c + 1;
      if ( pptr() >= epptr()
        && strmode & dynamic
        && !(strmode & frozen) )
      {
        const streamsize newsize = __grow_heap_block_size(alsize);
        char * p = _alloc(newsize);
        if ( p )
        {
          alsize = newsize;
          strmode |= allocated;
          if ( pbase() )
          {
            std::copy(pbase(), pptr()/*epptr()*/, p);
            _free(pbase());
          }
          const int pnextdiff = pptr() - pbase();
          setp(p, p + alsize);
          pbump(pnextdiff);
          setg(p, p + (gnext - gbeg), pnext + 1/*note pnext++ below*/);
        }
      }
      if ( pptr() >= epptr() )
        return EOF;
      *pnext++ = traits_type::to_char_type(c);
      return (unsigned char)c;
    }

    /// 9 Puts back the character designated by c to the input sequence,
    ///   if possible, in one of three ways:
    /// — If c != EOF, if the input sequence has a putback position available,
    ///   and if (char)c == gnext[-1], assigns gnext - 1 to gnext.
    /// 10 Returns c.
    /// — If c != EOF, if the input sequence has a putback position available,
    ///   and if strmode & constant is zero, assigns c to *--gnext.
    ///   11 Returns c.
    /// — If c == EOF and if the input sequence has a putback position available,
    ///   assigns gnext - 1 to gnext.
    /// 12 Returns a value other than EOF.
    /// 13 Returns EOF to indicate failure.
    /// 14 Remarks: If the function can succeed in more than one of these ways,
    ///    it is unspecified which way is chosen. The function can alter
    ///    the number of putback positions available as a result of any call.
    virtual int_type pbackfail(int_type c = EOF)
    {
      if ( gnext <= gbeg )
        return EOF;
      if ( c == EOF )
      {
        --gnext;
        return c + 1;
      }
      if ( strmode & constant )
      {
        if ( (char)c != gnext[-1] )
          return EOF;
        --gnext;
        return c;
      }
      *--gnext = traits_type::to_char_type(c);
      return c;
    }

    /// 15 Effects: Reads a character from the input sequence, if possible,
    ///    without moving the stream position past it, as follows:
    /// — If the input sequence has a read position available, the function
    ///   signals success by returning (unsigned char)*gnext.
    /// — Otherwise, if the current write next pointer pnext is not a null pointer
    ///   and is greater than the current read end pointer gend, makes a read
    ///   position available by assigning to gend a value greater than gnext and
    ///   no greater than pnext.
    /// 16 Returns (unsigned char*)gnext.
    /// 17 Returns EOF to indicate failure.
    /// 18 Remarks: The function can alter the number of read positions available
    ///    as a result of any call.
    virtual int_type underflow()
    {
      if ( gnext < gend )
        return (unsigned char)*gnext;
      if ( pnext && pnext > gend )
      {
        gend = pnext;
        return (unsigned char)*gnext;
      }
      return EOF;
    }

    virtual
    pos_type
      seekoff(off_type            off,
              ios_base::seekdir   way,
              ios_base::openmode  which = ios_base::in | ios_base::out)
    {
      const pos_type failed = pos_type(off_type(-1));
      if ( (which & (ios::in | ios::out)) == (ios::in | ios::out)
        && way == ios::cur
        || !(which & (ios::in | ios::out)) )
        return failed;
      char_type * const seekhigh = gnext ? (pend ? pend : gend) : 0;
      if ( !seekhigh )
        return failed;
      char_type * const seeklow  = gbeg;
      if ( which & ios::in )
      {
        const off_type newoff = off + way == ios::beg ? 0 
                                    : way == ios::cur ? gnext - gbeg
                                    : /*way == ios::end ?*/ seekhigh - gbeg;
        if ( newoff < (seeklow - gbeg) || (seekhigh - gbeg) < newoff )
          return failed;
        gnext = gbeg + newoff;
        if ( !(which & ios::out) )
          return pos_type(newoff);
      }
      // ( which & ios::out )
      if ( !pnext )
        return failed;
      const off_type newoff = off + way == ios::beg ? 0 
                                  : way == ios::cur ? pnext - pbeg
                                  : /*way == ios::end ?*/ seekhigh - pbeg;
      if ( newoff < (seeklow - pbeg) || (seekhigh - pbeg) < newoff )
        return failed;
      pnext = pbeg + newoff;
      return pos_type(newoff);
    }

    virtual
    pos_type
      seekpos(pos_type sp,
              ios_base::openmode which = ios_base::in | ios_base::out)
    {
      const pos_type failed = pos_type(off_type(-1));
      if ( !(which & (ios::in | ios::out)) )
        return failed;
      const off_type newoff = sp;
      char_type * const seekhigh = gnext ? (pend ? pend : gend) : 0;
      char_type * const seeklow  = gbeg;
      if ( newoff < 0 || newoff > seekhigh - seeklow )
        return failed;
      if ( which & ios::in )
      {
        if ( !gnext ) return failed;
        gnext = gbeg + newoff;
      }
      if ( which & ios::out )
      {
        if ( !pnext ) return failed;
        pnext = pbeg + newoff;
      }
      return pos_type(newoff);
    }

    /// 26 Effects: Implementation defined, except that setbuf(0, 0) has no effect.
    //virtual streambuf* setbuf(char* s , streamsize n);

  ///////////////////////////////////////////////////////////////////////////
  private:

    enum strstate
    { 
      /// set when a dynamic array object has been allocated, and
      /// hence should be freed by the destructor for the strstreambuf object;
      allocated = 1 << 0,
      /// set when the array object has const elements,
      /// so the output sequence cannot be written;
      constant  = 1 << 1,
      /// set when the array object is allocated (or reallocated) as necessary
      /// to hold a character sequence that can change in length;
      dynamic   = 1 << 2,
      /// set when the program has requested that the array object
      /// not be altered, reallocated, or freed.
      frozen    = 1 << 3
    };

    __ntl_bitmask_type(strstate, friend);

    strstate    strmode;
    streamsize  alsize;
    void*     (*palloc)(size_t);
    void      (*pfree)(void*);

    void _construct(const char* gnext_arg, streamsize n)
    {
      const streamsize _n = n > 0 ? n : !n ? static_cast<ssize_t>(strlen(gnext_arg)) : INT_MAX;
      setg(const_cast<char*>(gnext_arg),
           const_cast<char*>(gnext_arg),
           const_cast<char*>(gnext_arg + _n));
    }
    void _construct(char* gnext_arg, streamsize n, char* pbeg_arg = 0)
    {
      const streamsize _n = n > 0 ? n : !n ? static_cast<ssize_t>(strlen(gnext_arg)) : INT_MAX;
      if ( pbeg_arg == 0 )
        setg(gnext_arg, gnext_arg, gnext_arg + _n);
      else
      {
        setg(gnext_arg, gnext_arg, pbeg_arg);
        setp(pbeg_arg, pbeg_arg + _n);
      }
    }

    /// 6 ... If palloc is not a null pointer, the function calls (*palloc)(n)
    ///   to allocate the new dynamic array object.
    ///   Otherwise, it evaluates the expression new charT[n]
    char* _alloc(size_t size)
    {
      return palloc ? (char*)palloc(size) : new (std::nothrow) char[size];
    }

    /// 7 To free a previously existing dynamic array object whose first element
    ///   address is p:
    ///   If pfree is not a null pointer, the function calls (*pfree)(p).
    ///   Otherwise, it evaluates the expression delete[] p.
    void _free(char* p)
    {
      if ( pfree ) pfree(p);
      else delete[] p;
    }
    

};//class strstreambuf

/**@} str_strstreams */

}//namespace std

#endif//#ifndef NTL__STLX_STRSTREAM
