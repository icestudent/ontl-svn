/**\file*********************************************************************
 *                                                                     \brief
 *  N2315 27.6.2 Output streams [output.streams]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_SSTREAM
#define NTL__STLX_SSTREAM

#include "ios.hxx"
#include "iosfwd.hxx"

namespace std {

/**\addtogroup  lib_input_output ******* [27] Input/output library **********
 *@{*/

/**\addtogroup  lib_string_streams ***** [27.7] String-based streams ********
 *@{*/


/// 27.7.1 Class template basic_stringbuf [stringbuf]
/// 1 The class basic_stringbuf is derived from basic_streambuf to associate
///   possibly the input sequence and possibly the output sequence with a sequence
///   of arbitrary characters. The sequence can be initialized from, or made
///   available as, an object of class basic_string.
template <class charT, class traits, class Allocator>
class basic_stringbuf : public basic_streambuf<charT, traits>
{
  public:
    typedef charT                     char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits                    traits_type;
    typedef Allocator                 allocator_type;

    /// 27.7.1.1 basic_stringbuf constructors [stringbuf.cons]
    explicit basic_stringbuf(ios_base::openmode which = ios_base::in | ios_base::out)
    : mode(which)
    {}

    /// 5 Effects: Constructs an object of class basic_stringbuf, initializing
    ///   the base class with basic_streambuf() (27.5.2.1), and initializing
    ///   mode with which. Then calls str(s).
    explicit basic_stringbuf(const basic_string<charT,traits,Allocator>& str,
                            ios_base::openmode which = ios_base::in | ios_base::out)
    : mode(which), str_(str)
    {
      set_ptrs();
    }

#ifdef NTL__CXX
    basic_stringbuf(basic_stringbuf&& rhs);
#endif

#ifdef NTL__CXX
    ///\name 27.7.1.2 Assign and swap [stringbuf.assign]
    basic_stringbuf& operator=(basic_stringbuf&& rhs);
    void swap(basic_stringbuf&& rhs);
#else
    void swap(basic_stringbuf& rhs);
#endif

    ///\name 27.7.1.3 Member functions [stringbuf.members]

    /// 12 Returns: A basic_string object whose content is equal to the basic_stringbuf
    ///   underlying character sequence. If the basic_stringbuf was created only
    ///   in input mode, the resultant basic_string contains the character sequence
    ///   in the range [eback(),egptr()). If the basic_stringbuf was created
    ///   with which & ios_base::out being true then the resultant basic_string
    ///   contains the character sequence in the range [pbase(),high_mark),
    ///   where high_mark represents the position one past the highest initialized
    ///   character in the buffer. Characters can be initialized by writing to
    ///   the stream, by constructing the basic_stringbuf with a basic_string,
    ///   or by calling the str(basic_string) member function. In the case of
    ///   calling the str(basic_string) member function, all characters
    ///   initialized prior to the call are now considered uninitialized
    ///   (except for those characters re-initialized by the new basic_string).
    ///   Otherwise the basic_stringbuf has been created in neither input nor
    ///   output mode and a zero length basic_string is returned.
    basic_string<charT,traits,Allocator> str() const
    {
      const char_type * const beg = mode & ios_base::out ? this->pbase()
                                  : mode & ios_base::in ? this->eback() : 0;
      const char_type * const end = mode & ios_base::out ? this->pptr()
                                  : mode & ios_base::in ? this->egptr() : 0;
      return basic_string<charT,traits,Allocator>(beg, end);
    }

    /// 13 Effects: Copies the content of s into the basic_stringbuf underlying
    ///   character sequence and initializes the input and output sequences according to mode.
    /// 14 Postconditions: If mode & ios_base::out is true, pbase() points to
    ///   the first underlying character and epptr() >= pbase() + s.size() holds;
    ///   in addition, if mode & ios_base::in is true, pptr() == pbase() + s.data()
    ///   holds, otherwise pptr() == pbase() is true.
    ///   If mode & ios_base::in is true, eback() points to the first underlying
    ///   character, and both gptr() == eback() and egptr() == eback() + s.size() hold.
    void str(const basic_string<charT,traits,Allocator>& s)
    { 
      str_.assign(s);
      set_ptrs();
    }

    ///\}

  protected:

    ///\name 27.7.1.4 Overridden virtual functions [stringbuf.virtuals]

    /// 15 Returns: If the input sequence has a read position available,
    ///   returns traits::to_int_type(*gptr()). Otherwise, returns traits::eof().
    ///   Any character in the underlying buffer which has been initialized
    ///   is considered to be part of the input sequence.
    virtual int_type underflow()
    {
      return this->gptr() < this->egptr()
              ? traits::to_int_type(*this->gptr()) : traits::eof();
    }

    /// 16 Effects: Puts back the character designated by c to the input sequence,
    ///   if possible, in one of three ways:
    ///   — If traits::eq_int_type(c,traits::eof()) returns false and if
    ///     the input sequence has a putback position available, and if
    ///     traits::eq(to_char_type(c),gptr()[-1]) returns true, assigns
    ///     gptr() - 1 to gptr().
    ///     Returns: c.
    ///   — If traits::eq_int_type(c,traits::eof()) returns false and if
    ///     the input sequence has a putback position available, and if
    ///     mode & ios_base::out is nonzero, assigns c to *--gptr().
    ///     Returns: c.
    ///   — If traits::eq_int_type(c,traits::eof()) returns true and if
    ///     the input sequence has a putback position available, assigns gptr() - 1 to gptr().
    ///     Returns: traits::not_eof(c).
    /// 17 Returns: traits::eof() to indicate failure.
    /// 18 Remarks: If the function can succeed in more than one of these ways,
    ///    it is unspecified which way is chosen.
    virtual int_type pbackfail(int_type c = traits::eof())
    {
      // backup sequence
      if ( this->eback() < this->gptr() )
      {
        if ( !traits::eq_int_type(c, traits::eof()) )
        {
          if ( !traits::eq(traits::to_char_type(c), this->gptr()[-1])
            && mode & ios_base::out )
            this->gptr()[-1] = traits::to_char_type(c);
        }
        this->gbump(-1);
        return traits::not_eof(c);
      }
      return traits::eof();
    }

    virtual int_type overflow (int_type c = traits::eof());

    virtual pos_type seekoff(off_type off, ios_base::seekdir way, ios_base::openmode which = ios_base::in | ios_base::out);
    virtual pos_type seekpos(pos_type sp, ios_base::openmode which = ios_base::in | ios_base::out);

    /// 29 Effects: implementation-defined, except that setbuf(0,0) has no effect.
    virtual basic_streambuf<charT,traits>* setbuf(charT*, streamsize)
    {
      return this;
    }

  private:

    /// set stream poiners according to mode. \see str.
    void set_ptrs()
    {
      if ( mode & ios_base::out )
      {
        this->setp(s.begin(), s.end());
        //if ( mode & ios_base::in )
        //  this->gbump(s.size());
      }
      if ( mode & ios_base::in )
        this->setg(s.begin(), s.begin(), s.end());      
    }

    basic_string<charT,traits,Allocator>  str_;
    ios_base::openmode mode;

};// template class basic_stringbuf

template <class charT, class traits, class Allocator>
void swap(basic_stringbuf<charT, traits, Allocator>& x, basic_stringbuf<charT, traits, Allocator>& y)
{
  x.swap(y);
}

#ifdef NTL__CXX

template <class charT, class traits, class Allocator>
void swap(basic_stringbuf<charT, traits, Allocator>&& x,
  basic_stringbuf<charT, traits, Allocator>& y);

template <class charT, class traits, class Allocator>
void swap(basic_stringbuf<charT, traits, Allocator>& x,
  basic_stringbuf<charT, traits, Allocator>&& y);

#endif


/// 27.7.2 Class template basic_istringstream [istringstream]
/// 1 The class basic_istringstream<charT, traits, Allocator> supports reading
///   objects of class basic_string<charT, traits, Allocator>.
///   It uses a basic_stringbuf object to control the associated storage.
template <class charT, class traits, class Allocator>
class basic_istringstream : public basic_istream<charT,traits>
{
  public:

    ///\name Types
    typedef charT                     char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits                    traits_type;
    typedef Allocator                 allocator_type;

    ///\name 27.7.2.1 basic_istringstream constructors [istringstream.cons]

    /// 2 Effects: Constructs an object of class basic_istringstream<charT, traits>,
    ///   initializing the base class with basic_istream(&sb) and initializing
    ///   sb with basic_stringbuf<charT, traits, Allocator>(which|ios_base::in))
    explicit basic_istringstream(ios_base::openmode which = ios_base::in)
    : basic_istream<charT,traits>(&sb), sb(which | ios_base::in)
    {;}

    /// 3 Effects: Constructs an object of class basic_istringstream<charT, traits>,
    ///   initializing the base class with basic_istream(&sb) and initializing
    ///   sb with basic_stringbuf<charT, traits, Allocator>(str, which|ios_base::in))
    explicit basic_istringstream(const basic_string<charT,traits,Allocator>& str,
                                 ios_base::openmode which = ios_base::in)
    : basic_istream<charT,traits>(&sb), sb(which | ios_base::in)
    {;}

#ifdef NTL__CXX
    /// 4 Effects: Move constructs from the rvalue rhs. This is accomplished by
    ///   move constructing the base class, and the contained basic_stringbuf.
    ///   Next basic_istream<charT,traits>::set_rdbuf(&sb) is called to install
    ///   the contained basic_stringbuf.
    basic_istringstream(basic_istringstream&& rhs);
#endif

    /// 27.7.2.2 Assign and swap [istringstream.assign]

#ifdef NTL__CXX
    basic_istringstream& operator=(basic_istringstream&& rhs);
    void swap(basic_istringstream&& rhs);
#else
    void swap(basic_istringstream& rhs);
#endif

    /// 27.7.2.3 Member functions [istringstream.members]

    basic_stringbuf<charT,traits,Allocator>* rdbuf() const
    {
      return const_cast<basic_stringbuf<charT,traits,Allocator>*>(&sb);
    }

    basic_string<charT,traits,Allocator> str() const
    {
      return sb.str();
    }

    void str(const basic_string<charT,traits,Allocator>& s)
    {
      sb.str(s);
    }

    ///\}

  private:
    basic_stringbuf<charT,traits,Allocator> sb;

};// template class basic_istringstream


/// 27.7.3 Class basic_ostringstream [ostringstream]
/// 1 The class basic_ostringstream<charT, traits, Allocator> supports writing
///   objects of class basic_string<charT, traits, Allocator>.
///   It uses a basic_stringbuf object to control the associated storage.
template <class charT, class traits, class Allocator>
class basic_ostringstream : public basic_ostream<charT,traits>
{
  public:

    ///\name Types
    typedef charT                     char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits                    traits_type;
    typedef Allocator                 allocator_type;

    ///\name 27.7.3.1 basic_ostringstream constructors [ostringstream.cons]

    /// 2 Effects: Constructs an object of class basic_ostringstream, initializing
    ///   the base class with basic_ostream(&sb) and initializing sb with
    ///   basic_stringbuf<charT, traits, Allocator>(which | ios_base::out))
    explicit basic_ostringstream(ios_base::openmode which = ios_base::out)
    : basic_ostream<charT,traits>(&sb), sb(which | ios_base::out)
    {;}
    
    /// 3 Effects: Constructs an object of class basic_ostringstream<charT, traits>,
    ///   initializing the base class with basic_ostream(&sb) and initializing
    ///   sb with basic_stringbuf<charT, traits, Allocator>(str, which|ios_base::out))
    explicit basic_ostringstream(const basic_string<charT,traits,Allocator>& str,
                                 ios_base::openmode which = ios_base::out)
    : basic_ostream<charT,traits>(&sb), sb(str, which | ios_base::out)
    {;}

#ifdef NTL__CXX
    /// 4 Effects: Move constructs from the rvalue rhs. This is accomplished 
    ///   move constructing the base class, and the contained basic_stringbuf.
    ///   Next basic_ostream<charT,traits>::set_rdbuf(&sb) is called to install
    ///  the contained basic_stringbuf.
    basic_ostringstream(basic_ostringstream&& rhs);
#endif

    ///\name 27.7.3.2 Assign and swap [ostringstream.assign]

#ifdef NTL__CXX
    basic_ostringstream& operator=(basic_ostringstream&& rhs);
    void swap(basic_ostringstream&& rhs);
#else
    void swap(basic_ostringstream& rhs);
#endif

    ///\name 27.7.3.3 Member functions [ostringstream.members]

    basic_stringbuf<charT,traits,Allocator>* rdbuf() const
    {
      return const_cast<basic_stringbuf<charT,traits,Allocator>*>(&sb);
    }

    basic_string<charT,traits,Allocator> str() const
    { 
      return sb.str();
    }

    void str(const basic_string<charT,traits,Allocator>& s)
    { 
      sb.str(s);
    }

    ///\}

  private:
     basic_stringbuf<charT,traits,Allocator> sb;

};// template class basic_ostringstream


/// 27.7.4 Class template basic_stringstream [stringstream]
/// 1 The class template basic_stringstream<charT, traits> supports reading and
///   writing from objects of class basic_string<charT, traits, Allocator>.
///   It uses a basic_stringbuf<charT, traits, Allocator> object to control
///   the associated sequence.
template <class charT, class traits, class Allocator>
class basic_stringstream : public basic_iostream<charT, traits> 
{
  public:

    ///\name Types
    typedef charT                     char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits                    traits_type;
    typedef Allocator                 allocator_type;

    ///\name 27.7.1.1 basic_stringbuf constructors [stringbuf.cons]

    /// 1 Effects: Constructs an object of class basic_stringstream<charT,traits>,
    ///   initializing the base class with basic_iostream(&sb) and initializing
    ///   sb with basic_stringbuf<charT,traits,Allocator>(which).
    explicit basic_stringstream(ios_base::openmode which = ios_base::out|ios_base::in)
    : basic_iostream<charT, traits>(&sb), sb(which)
    {}

    /// 2 Effects: Constructs an object of class basic_stringstream<charT, traits>,
    ///   initializing the base class with basic_iostream(&sb) and initializing
    ///   sb with basic_stringbuf<charT, traits, Allocator>(str,which).
    explicit basic_stringstream(const basic_string<charT,traits,Allocator>& str,
                                ios_base::openmode which = ios_base::out|ios_base::in)
    : basic_iostream<charT, traits>(&sb), sb(str, which)
    {}

  #ifdef NTL__CXX
    /// 3 Effects: Move constructs from the rvalue rhs. This is accomplished by
    ///   move constructing the base class, and the contained basic_stringbuf.
    ///   Next basic_istream<charT,traits>::set_rdbuf(&sb) is called to install
    ///   the contained basic_stringbuf.
    basic_stringstream(basic_stringstream&& rhs);
  #endif

    ///\name 27.7.5.1 Assign and swap [stringstream.assign]

  #ifdef NTL__CXX
    basic_stringstream& operator=(basic_stringstream&& rhs);
    void swap(basic_stringstream&& rhs);
  #else
    void swap(basic_stringstream& rhs);
  #endif

    ///\name 27.7.6 Member functions [stringstream.members]

    basic_stringbuf<charT,traits,Allocator>* rdbuf() const
    {
      return const_cast<basic_stringbuf<charT,traits,Allocator>*>(&sb);
    }

    basic_string<charT,traits,Allocator> str() const
    { 
      return sb.str();
    }

    void str(const basic_string<charT,traits,Allocator>& str)
    { 
      sb.str(str);
    }

    ///\}

  private:
     basic_stringbuf<charT, traits> sb;

};// template class basic_stringstream


template <class charT, class traits, class Allocator>
void swap(basic_stringstream<charT, traits, Allocator>& x, basic_stringstream<charT, traits, Allocator>& y)
{
  x.swap(y);
}

#ifdef NTL__CXX

template <class charT, class traits, class Allocator>
void swap(basic_stringstream<charT, traits, Allocator>&& x,
  basic_stringstream<charT, traits, Allocator>& y);

template <class charT, class traits, class Allocator>
void swap(basic_stringstream<charT, traits, Allocator>& x,
  basic_stringstream<charT, traits, Allocator>&& y);

#endif

/**@} lib_string_streams */
/**@} lib_input_output */

} // namespace std

#endif // NTL__STLX_SSTREAM
