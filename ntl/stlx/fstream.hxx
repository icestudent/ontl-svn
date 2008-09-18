/**\file*********************************************************************
 *                                                                     \brief
 *  27.8.1 File streams [lib.fstreams]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_FSTREAM
#define NTL__STLX_FSTREAM

#include "cstdio.hxx"
#include "streambuf.hxx"
#include "ostream.hxx"

namespace std {

/// 27.8.1.1 Class template basic_filebuf [lib.filebuf]
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_filebuf
: public basic_streambuf<charT, traits>
{
    FILE filebuf;
    static const size_t hdd_sector_size = 512; 
    array<charT, hdd_sector_size/sizeof(charT)> buf;
  
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef charT                     char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits                    traits_type;

    ///\name 27.8.1.2 basic_filebuf constructors [lib.filebuf.cons]

    basic_filebuf() { this->setp(buf.begin(), buf.end()); }

    virtual ~basic_filebuf() { close(); }

    ///\name 27.8.1.4 Member functions [filebuf.members]

    bool is_open() const { return filebuf; }

    basic_filebuf<charT,traits>* open(const char* s, ios_base::openmode mode)
    {
      if ( is_open() != false ) return 0;
      FILE::access_mask m = (FILE::access_mask)0;
      if ( mode & ios_base::in ) m = m | FILE::read_access;
      if ( mode & ios_base::out ) m = m | FILE::write_access;
      if ( mode & ios_base::app ) m = m | FILE::append;
      FILE::creation_disposition const cd = mode & ios_base::trunc ? FILE::supersede : FILE::open_if;
      return filebuf.create(FILE::convert_filename(s), cd, m) ? this : 0;
    }

    basic_filebuf<charT,traits>* close()
    {
      if ( is_open() == false ) return 0;
      if ( this->pptr() != this->pbase() ) this->overflow();
      filebuf.close();
      return this;
    }

    ///\}

  ///////////////////////////////////////////////////////////////////////////
  protected:

    using basic_streambuf::eback;
    using basic_streambuf::gptr;
    using basic_streambuf::egptr;

    /// 27.8.1.5 Overridden virtual functions [filebuf.virtuals]

    virtual streamsize showmanyc()
    {
      long long pos;
      filebuf.getpos(pos);
      long long const r = filebuf.size() - pos;
      return sizeof(streamsize) >= sizeof(long long) 
            || 0 == (r & 0xFFFFFFFF00000000ui64)
            ? static_cast<streamsize>(filebuf.size() - pos)
            : static_cast<streamsize>(-1) >> 1;
    }

    virtual int_type underflow()
    {
      _Assert(!gptr() || gptr() >= egptr());
      if ( eback() < gptr() )
      {
        // backup sequence
      }
      while ( gptr() >= egptr() )
      {
        size_t size = buf.size();
        if ( !filebuf.read(buf.begin(), size) /* || !size*/ )
          return traits::eof();
        this->setg(buf.begin(), buf.begin(), buf.begin() + size);
      }
      return traits::to_int_type(*gptr());
    }

    //inherited: virtual int_type uflow();
    
    //virtual int_type pbackfail(int_type c = traits::eof());
    //virtual int_type overflow (int_type c = traits::eof());

    virtual basic_streambuf<charT,traits>* setbuf(char_type* s, streamsize n)
    {
      this->setp(s, s + n);
      return this;
    }

    //virtual pos_type seekoff(off_type off, ios_base::seekdir way, ios_base::openmode which = ios_base::in | ios_base::out);
    //virtual pos_type seekpos(pos_type sp, ios_base::openmode which = ios_base::in | ios_base::out);

    //virtual int sync();
    //virtual void imbue(const locale& loc);

  ///////////////////////////////////////////////////////////////////////////
  private:

};


/// 27.8.1.6 Class template basic_ifstream [ifstream]
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_ifstream
: public basic_istream<charT, traits>
{
  public:

    typedef charT                     char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits                    traits_type;

    ///\name 27.8.1.7 basic_ifstream constructors [ifstream.cons]

    /// 48 Effects: Constructs an object of class basic_ifstream<charT,traits>,
    ///   initializing the base class with basic_istream(&sb) and initializing
    ///   sb with basic_filebuf<charT,traits>()) (27.6.1.1.1, 27.8.1.2).
    basic_ifstream() : basic_istream<charT, traits>(&sb), sb() {;}

    /// 49 Effects: Constructs an object of class basic_ifstream,
    ///   initializing the base class with basic_istream(&sb) and initializing
    ///   sb with basic_filebuf<charT, traits>()) (27.6.1.1.1, 27.8.1.2),
    ///   then calls rdbuf()->open(s, mode | ios_base::in).
    ///   If that function returns a null pointer, calls setstate(failbit).
    explicit basic_ifstream(const char* s, ios_base::openmode mode = ios_base::in)
    : basic_istream<charT, traits>(&sb), sb()
    {
      if ( !sb.open(s, mode | ios_base::in) )
        basic_ios::setstate(ios_base::failbit);
    }

    ///\name 27.8.1.8 Assign and swap [ifstream.assign]

    ///\name 27.8.1.9 Member functions [ifstream.members]

    basic_filebuf<charT,traits>* rdbuf() const
    {
      return const_cast<basic_filebuf<charT,traits>*>(&sb);
    }
    
    bool is_open() const { return sb.is_open(); }

    void open(const char* s, ios_base::openmode mode = ios_base::in)
    {
      if ( sb.open(s, mode | ios_base::in) )
        basic_ios::clear();
      else
        basic_ios::setstate(ios_base::failbit);
    }
    
    void close()
    {
      if ( !sb.close() )
        basic_ios::setstate(ios_base::failbit);
    }

    ///\}

  private:
     basic_filebuf<charT,traits> sb;
};


/// 27.8.1.10 Class template basic_ofstream [ofstream]
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_ofstream
: public basic_ostream<charT, traits>
{
  public:

    typedef charT char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits traits_type;

    ///\name 27.8.1.11 basic_ofstream constructors [ofstream.cons]

    /// 62 Effects: Constructs an object of class basic_ofstream<charT,traits>,
    ///   initializing the base class with basic_ostream(&sb) and initializing
    ///   sb with basic_filebuf<charT,traits>()) (27.6.2.2, 27.8.1.2).
    basic_ofstream() : basic_ostream<charT, traits>(&sb), sb() {;}

    /// 63 Effects: Constructs an object of class basic_ofstream<charT,traits>,
    ///   initializing the base class with basic_ostream(&sb) and initializing
    ///   sb with basic_filebuf<charT,traits>()) (27.6.2.2, 27.8.1.2),
    ///   then calls rdbuf()->open(s, mode|ios_base::out).
    ///   If that function returns a null pointer, calls setstate(failbit).
    explicit basic_ofstream(const char* s, ios_base::openmode mode = ios_base::out)
    : basic_ostream<charT, traits>(&sb), sb()
    {
      if ( !sb.open(s, mode | ios_base::out) )
        basic_ios::setstate(ios_base::failbit);
    }

    ///\name 27.8.1.12 Assign and swap [ofstream.assign]

    ///\name 27.8.1.13 Member functions [ofstream.members]

    basic_filebuf<charT,traits>* rdbuf() const
    {
      return const_cast<basic_filebuf<charT,traits>*>(&sb);
    }
    
    bool is_open() const { return sb.is_open(); }

    void open(const char* s, ios_base::openmode mode = ios_base::out)
    {
      if ( sb.open(s, mode | ios_base::out) )
        basic_ios::clear();
      else
        basic_ios::setstate(ios_base::failbit);
    }
    
    void close()
    {
      if ( !sb.close() )
        basic_ios::setstate(ios_base::failbit);
    }

    ///\}

  ///////////////////////////////////////////////////////////////////////////
  private:
     basic_filebuf<charT,traits> sb;

  private:
    basic_ofstream(const basic_ofstream&); // not defined
    basic_ofstream& operator=(const basic_ofstream&); // not defined
};


/// 27.8.1.14 Class template basic_fstream [fstream]
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_fstream
: public basic_iostream<charT, traits>
{
  public:

    typedef charT char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits traits_type;

    ///\name 27.8.1.15 basic_fstream constructors [fstream.cons]

    /// 76 Effects: Constructs an object of class basic_fstream<charT,traits>,
    ///   initializing the base class with basic_iostream(&sb) and initializing
    ///   sb with basic_filebuf<charT,traits>().
    basic_fstream() : basic_iostream<charT, traits>(&sb), sb() {;}

    /// 77 Effects: Constructs an object of class basic_fstream<charT, traits>,
    ///   initializing the base class with basic_iostream(&sb) and initializing
    ///   sb with basic_filebuf<charT, traits>(). Then calls rdbuf()->open(s, mode).
    ///   If that function returns a null pointer, calls setstate(failbit).
    explicit basic_fstream(const char* s, ios_base::openmode mode = ios_base::in|ios_base::out)
    : basic_iostream<charT, traits>(&sb), sb() 
    {
      if ( !sb.open(s, mode | ios_base::in|ios_base::out) )
        basic_ios::setstate(ios_base::failbit);
    }

    ///\name 27.8.1.16 Assign and swap [fstream.assign]

    ///\name 27.8.1.17 Member functions [fstream.members]

    basic_filebuf<charT,traits>* rdbuf() const
    {
      return const_cast<basic_filebuf<charT,traits>*>(&sb);
    }
    
    bool is_open() const { return sb.is_open(); }

    void open(const char* s, ios_base::openmode mode = ios_base::in|ios_base::out)
    {
      if ( sb.open(s, mode | ios_base::in|ios_base::out) )
        basic_ios::clear();
      else
        basic_ios::setstate(ios_base::failbit);
    }
    
    void close()
    {
      if ( !sb.close() )
        basic_ios::setstate(ios_base::failbit);
    }

    ///\}

  private:
     basic_filebuf<charT,traits> sb;
};


}//namespace std

#endif//#ifndef NTL__STLX_FSTREAM
