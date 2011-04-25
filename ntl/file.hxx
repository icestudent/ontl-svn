/**\file*********************************************************************
 *                                                                     \brief
 *  files support
 *
 ****************************************************************************
 */

#ifndef NTL__FILE
#define NTL__FILE

#include "device_traits.hxx"
#include "handle.hxx"


namespace ntl {


template<class FileDevice, class traits = device_traits<FileDevice> >
class basic_file : public traits
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef typename traits::size_type            size_type;
    typedef typename traits::access_mask          access_mask;
    typedef typename traits::creation_disposition creation_disposition;
    typedef typename traits::share_mode           share_mode;
    typedef typename traits::creation_options     creation_options;
    typedef typename traits::attributes           attributes;

    inline basic_file() : f() {}

    typedef typename traits::unspecified_bool_type unspecified_bool_type; 
    operator unspecified_bool_type() const { return f.operator unspecified_bool_type(); }

    template<typename objectT>
    explicit __forceinline
      basic_file(
        const objectT &             object, 
        const creation_disposition  cd              = traits::creation_disposition_default,
        const access_mask           desired_access  = traits::access_mask_default,
        const share_mode            share_access    = traits::share_mode_default, 
        const creation_options      co              = traits::creation_options_default,
        const attributes            attr            = traits::attribute_default
        ) /* nothrow */
    {
      f.create(object, cd, desired_access, share_access, co, attr);
    }

    //bool open(const char* filename, const char* mode);
    template<typename objectT>
    bool
      open(
        const objectT &             object, 
        const access_mask           desired_access  = traits::access_mask_default,
        const share_mode            share           = traits::share_mode_default,
        const creation_options      co              = traits::creation_options_default
        ) /* nothrow */
    {
      return FileDevice::success(f.open(object, desired_access, share, co));
    }

    template<typename objectT>
    bool
      create(
        const objectT &             object, 
        const creation_disposition  cd              = traits::creation_disposition_default,
        const access_mask           desired_access  = traits::access_mask_default,
        const share_mode            share_access    = traits::share_mode_default, 
        const creation_options      co              = traits::creation_options_default,
        const attributes            attr            = traits::attribute_default
        ) /* nothrow */
    {
      return FileDevice::success(f.create(object, cd, desired_access, share_access, co, attr));
    }

    /*
    bool
      operator()(
        ) __ntl_nothrow
    {
    }
    */

    void close() /* nothrow */ { f.close(); }
    bool flush() /* nothrow */ { return FileDevice::success(f.flush()); }

    bool read(void * out, const size_t & out_size) /* nothrow */
    {
      return FileDevice::success(f.read(out, out_size));
    }

    bool read(void * out, size_t & out_size) /* nothrow */
    {
      bool const r = FileDevice::success(f.read(out, out_size));
      out_size = f.read_write_count();
      return r;
    }

    bool write(const void * in, const size_t in_size) /* nothrow */
    {
      return FileDevice::success(f.write(in, in_size));
    }

    bool write(void * out, size_t & out_size) /* nothrow */
    {
      bool const r = FileDevice::success(f.write(out, out_size));
      out_size = f.read_write_count();
      return r;
    }

    size_type size() const /* nothrow */
    {
      return f.size();
    }

    bool size(const size_type & new_size) /* nothrow */
    {
      return FileDevice::success(f.size(new_size));
    }

    bool remove() /* nothrow */
    {
      return FileDevice::success(f.remove());
    }

    template<class NameType>
    bool rename(
      const NameType &  new_name,
      bool              replace_if_exists = true) /* nothrow */
    {
      return FileDevice::success(f.rename(new_name, replace_if_exists));
    }

    bool getpos(long long & pos) /* nothrow */
    {
      return FileDevice::success(f.getpos(pos));
    }

    bool setpos(const long long & pos) /* nothrow */
    {
      return FileDevice::success(f.setpos(pos));
    }

    __forceinline
    raw_data get_data() __ntl_throws(std::bad_alloc)
    {
      const size_type & file_size = size();
      raw_data file_content;
      if ( ! (file_size & ~static_cast<size_type>(uint32_t(-1))) )
      {
        const uint32_t & data_size = static_cast<uint32_t>(file_size);
        file_content.resize(data_size);
        if ( ! read(file_content.begin(), data_size) )
          file_content.clear();
      }
      return file_content;
    }

    const FileDevice & handler() const /* nothrow */
    { 
      return f;
    }

  ///////////////////////////////////////////////////////////////////////////
  private:

    FileDevice f;

}; // class basic_file


}//namespace ntl

#endif//#ifndef NTL__FILE
