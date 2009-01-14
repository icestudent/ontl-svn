/**\file*********************************************************************
 *                                                                     \brief
 *  Standard iostream objects [27.3 lib.iostream.objects]
 *
 ****************************************************************************
 */

#ifndef STLX_IOSTREAM
#define STLX_IOSTREAM

#include "iosfwd.hxx"
#if 1
#include <ios>
#include <streambuf>
#include <istream>
#include <ostream>
#endif

namespace stlx {

/**\addtogroup  lib_input_output ******* 27 Input/output library [input.output] **********
 *@{*/

///\name  27.3 Standard iostream objects [lib.iostream.objects]
extern istream  cin;
extern ostream  cout;
extern ostream  cerr;
extern ostream  clog;
extern wistream wcin;
extern wostream wcout;
extern wostream wcerr;
extern wostream wclog;

///@}
/**@} lib_input_output */

}//namespace stlx

#endif//#ifndef STLX_IOSTREAM
