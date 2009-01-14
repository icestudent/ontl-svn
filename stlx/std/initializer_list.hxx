/**\file*********************************************************************
 *                                                                     \brief
 *  Initializer lists [18.8 support.initlist]
 *
 ****************************************************************************
 */

#ifndef STLX_INITLIST
#define STLX_INITLIST

#ifndef STLX_CSTDDEF
#include "../cstddef.hxx" // for size_t
#endif

namespace stlx {

  /// Initializer lists [18.8 support.initlist]
  template<class E> 
  class initializer_list
  {
  public:
    initializer_list();
    size_t size() const;
    const E* begin() const { return nullptr; }
    const E* end() const { return nullptr; }
  };
}

#endif // STLX_INITLIST
