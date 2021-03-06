/**\file*********************************************************************
 *                                                                     \brief
 *  27.5 Stream buffers [stream.buffers]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_STREAMBUF
#define NTL__STLX_STREAMBUF
#pragma once

#include "locale.hxx"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4820) // X bytes padding added...
#endif

namespace std
{

/**\addtogroup  lib_input_output ******* 27 Input/output library [input.output]
 *@{*/
/**\addtogroup  lib_stream_buffers ***** 27.5 Stream buffers
 *@{*/

/**
 *	@brief 27.5.2 Class template basic_streambuf<charT,traits> [streambuf]
 *
 *  The class template basic_streambuf<charT,traits> serves as an abstract base class for deriving various
 *  stream buffers whose objects each control two character sequences:
 *  - a character input sequence;
 *  - a character output sequence.
 **/
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_streambuf
{
  ///////////////////////////////////////////////////////////////////////////
  public:
    ///\name Types:
    typedef charT                     char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits                    traits_type;

  protected:

    /// 27.5.2.1 basic_streambuf constructors [streambuf.cons]
    __forceinline
    basic_streambuf()
    : gbeg(0), gend(0), gnext(0),
      pbeg(0), pend(0), pnext(0)/*, l(locale())*/
    {/**/}

    basic_streambuf(const basic_streambuf& r)
    : gbeg(r.gbeg), gend(r.gend), gnext(r.gnext),
      pbeg(r.pbeg), pend(r.pend), pnext(r.pnext)
  #if STLX__CONFORMING_LOCALE
      , l(r.l)
  #endif
    {/**/}
  
    basic_streambuf& operator=(const basic_streambuf& r)
    {
      if(this == &r)
        return *this;
      gbeg = r.gbeg; gend = r.gend; gnext = r.gnext;
      pbeg = r.pbeg; pend = r.pend; pnext = r.pnext;
    #if STLX__CONFORMING_LOCALE
      l = r.l;
    #endif
      return *this;
    }

    void swap(basic_streambuf& rhs)
    {
      if(this == &rhs)
        return;
      std::swap(gbeg,  rhs.gbeg);
      std::swap(gend,  rhs.gend);
      std::swap(gnext, rhs.gnext);
      std::swap(pbeg,  rhs.pbeg);
      std::swap(pend,  rhs.pend);
      std::swap(pnext, rhs.pnext);
    }

  public:

    virtual ~basic_streambuf() {}

    //  27.5.2.2 basic_streambuf public member functions [streambuf.members]

    ///\name 27.5.2.2.1 Locales [streambuf.locales]

    locale pubimbue(const locale& loc)
    {
      locale prev = getloc();
      imbue(loc);
      return prev;
    }

    locale getloc() const
    {
  #if !STLX__CONFORMING_LOCALE
      locale  l;
  #endif
      return l;
    }

    ///\name 27.5.2.2.2 Buffer management and positioning [streambuf.buffer]

    basic_streambuf<char_type,traits>* pubsetbuf(char_type* s, streamsize n)
    {
      return setbuf(s, n);
    }

    pos_type pubseekoff(off_type           off,
                        ios_base::seekdir  way,
                        ios_base::openmode which = ios_base::in | ios_base::out)
    {
      return seekoff(off, way, which);
    }

    pos_type pubseekpos(pos_type sp,
                        ios_base::openmode which = ios_base::in | ios_base::out)
    {
      return seekpos(sp, which);
    }

    int pubsync()
    {
      return sync();
    }

    // Get and put areas:

    ///\name  27.5.2.2.3 Get area [streambuf.pub.get]

    streamsize in_avail()
    {
      // If xnext is not a null pointer and xnext < xend for an input sequence,
      // then a read position is available.
      const streamsize ravail = gend - gnext;
      return 0 < ravail ? ravail : showmanyc();
    }

    int_type snextc()
    {
      const int_type c = sbumpc();
      return c == traits::eof() ? c : sgetc();
    }

    __forceinline
    int_type sbumpc()
    {
      const streamsize ravail = gend - gnext;
      return !(0 < ravail) ? uflow() : traits_type::to_int_type(*gnext++);
    }

    int_type sgetc()
    {
      const streamsize ravail = gend - gnext;
      return !(0 < ravail) ? underflow() : traits_type::to_int_type(*gnext);
    }

    streamsize sgetn(char_type* s, streamsize n)
    {
      return xsgetn(s, n);
    }

    ///\name  27.5.2.2.4 Putback [streambuf.pub.pback]

    int_type sputbackc(char_type c)
    {
      const streamsize pbavail = gnext - gbeg;
      return !(0 < pbavail) || !traits_type::eq(c ,gnext[-1])
        ? pbackfail(traits_type::to_int_type(c)) : traits_type::to_int_type(*--gnext);
    }

    int_type sungetc()
    {
      const streamsize pbavail = gnext - gbeg;
      return !(0 < pbavail) ? pbackfail() : traits_type::to_int_type(*--gnext);
    }

    ///\name  27.5.2.2.5 Put area [streambuf.pub.put]

    int_type sputc(char_type c)
    {
      const int_type ic = traits_type::to_int_type(c);
      const streamsize wavail = pend - pnext;
      return !(0 < wavail) ? overflow(ic) : *pnext++ = c, ic;
    }

    streamsize sputn(const char_type* s, streamsize n)
    {
      return xsputn(s, n);
    }

    ///@}

  // 27.5.2.3 basic_streambuf protected member functions [streambuf.protected]
  protected:

    ///\name  27.5.2.3.2 Get area access [streambuf.get.area]

    char_type* eback() const  { return gbeg;  }
    char_type* gptr()  const  { return gnext; }
    char_type* egptr() const  { return gend;  }
    void gbump(int n)         { gnext += n;   }

    void setg(char_type* gbeg, char_type* gnext, char_type* gend)
    {
      this->gbeg = gbeg; this->gnext = gnext; this->gend = gend;
    }

    ///\name  27.5.2.3.3 Put area access [streambuf.put.area]

    char_type* pbase() const { return pbeg;   }
    char_type* pptr()  const { return pnext;  }
    char_type* epptr() const { return pend;   }
    void pbump(int n)        { pnext += n;    }

    void setp(char_type* pbeg, char_type* pend)
    {
      this->pbeg = this->pnext = pbeg; this->pend = pend;
    }

    // 27.5.2.4 basic_streambuf virtual functions [streambuf.virtuals]

    ///\name  27.5.2.4.1 Locales [streambuf.virt.locales]
    virtual void imbue(const locale&)
    {
      // Default behavior: Does nothing.
    }

    ///\name  27.5.2.4.2 Buffer management and positioning [streambuf.virt.buffer]

    virtual basic_streambuf<char_type,traits>* setbuf(char_type*, streamsize)
    {
      // Default behavior: Does nothing.
      return this;
    }

    virtual pos_type
      seekoff(off_type,
              ios_base::seekdir,
              ios_base::openmode = ios_base::in | ios_base::out)
    {
      // Default behavior: Does nothing.
      return pos_type(off_type(-1));
    }

    virtual pos_type
      seekpos(pos_type,
              ios_base::openmode = ios_base::in | ios_base::out)
    {
      // Default behavior: Does nothing.
      return pos_type(off_type(-1));
    }

    virtual int sync()
    {
      // Default behavior: Returns zero.
      return 0;
    }

    ///\name  27.5.2.4.3 Get area [streambuf.virt.get]

    virtual streamsize showmanyc()
    {
      // Default behavior: Returns zero.
      return 0;
    }

    virtual streamsize xsgetn(char_type* s, streamsize n)
    {
      for ( streamsize copied = 0; ; )
      {
        if ( !(0 < n) ) return copied;
        const streamsize ravail = gend - gnext;
        if ( !(0 < ravail) )
        {
          if ( traits_type::eq_int_type(traits_type::eof(), underflow()) )
            return copied;
          continue;
        }
        //__assume(n >= 0 && ravail >= 0);
        const size_t chunksize = static_cast<size_t>(min(n, ravail));
        traits_type::copy(s, gnext, chunksize);
        s += chunksize;
        copied += chunksize;
        gnext += chunksize;
        n -= chunksize;
      }
    }

    virtual int_type underflow()
    {
      ///\note The public members of basic_streambuf call this virtual function
      ///      only if gptr() is null or gptr() >= egptr()
      assert(!gptr() || gptr() >= egptr());
      //  Default behavior: Returns traits::eof().
      return traits_type::eof();
    }

    virtual int_type uflow()
    {
      //  Default behavior: Calls underflow().
      //  If underflow() returns traits::eof(), returns traits::eof(). Otherwise,
      //  returns the value of traits::to_int_type(*gptr()) and increment the value
      //  of the next pointer for the input sequence.
      return traits_type::eq_int_type(underflow(), traits_type::eof())
           ? traits_type::eof() : traits_type::to_int_type(*gnext++);
    }

    ///\name  27.5.2.4.4 Putback [streambuf.virt.pback]
    virtual int_type pbackfail(int_type c = traits_type::eof())
    {
      ///\note The public functions of basic_streambuf call this virtual function
      ///      only when gptr() is null, gptr() == eback(),
      ///      or traits::eq(traits::to_char_type(c ),gptr()[-1]) returns false.
      ///      Other calls shall also satisfy that constraint.
      assert(!gptr() || gptr() >= eback()
          || !traits_type::eq(traits_type::to_char_type(c),gnext[-1]));
      (void)(c);
      //  Default behavior: Returns traits::eof().
      return traits_type::eof();
    }

    ///\name  27.5.2.4.5 Put area [streambuf.virt.put]

    virtual streamsize xsputn(const char_type* s, streamsize n)
    {
      const int_type eof = traits_type::eof();
      for ( streamsize copied = 0; ; )
      {
        if ( !(0 < n) ) return copied;
        const streamsize wavail = pend - pnext;
        if ( !(0 < wavail) )
        {
          const char_type c = *s;
          if ( traits_type::eq_int_type(eof, overflow(traits_type::to_int_type(c))) )
            return copied;
          //if(pbeg) // overflow already puts character into sequence
          //  *pnext++ = c;
          s++;
          copied++;
          n--;
          continue;
        }
        //__assume(n >= 0 && ravail >= 0);
        const size_t chunksize = static_cast<size_t>(min(n, wavail));
        traits_type::copy(pnext, s, chunksize);
        s += chunksize;
        copied += chunksize;
        pnext += chunksize;
        n -= chunksize;
      }
    }

    virtual int_type overflow(int_type = traits_type::eof())
    {
      //  Default behavior: Returns traits::eof().
      return traits_type::eof();
    }

    ///@}

  ///////////////////////////////////////////////////////////////////////////
  //private:

    char_type * gbeg; // read:  start
    char_type * gend; //        end
    char_type * gnext;//        pos

    char_type * pbeg; // write: start
    char_type * pend; //        end
    char_type * pnext;//        pos

  #if STLX__CONFORMING_LOCALE
    locale      l;
  #endif
};

/**@} lib_stream_buffers */
/**@} lib_input_output */

}//namespace std

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif//#ifndef NTL__STLX_STREAMBUF
