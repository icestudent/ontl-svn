/**\file*********************************************************************
 *                                                                     \brief
 *  NT time
 *
 ****************************************************************************
 */

#ifndef NTL__NT_TIME
#define NTL__NT_TIME

#include "../stdint.h"
#include "shared_data.hxx"

namespace ntl {
namespace nt {

typedef system_time::type systime_t;
typedef systime_t file_time;

static const systime_t system_time_resolution = system_time::resolution;

systime_t inline query_system_time()
{
  return user_shared_data::instance().SystemTime.get();
}

}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_TIME
