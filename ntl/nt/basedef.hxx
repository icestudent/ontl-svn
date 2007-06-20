/**\file*********************************************************************
 *                                                                     \brief
 *  NT Types support library
 *
 ****************************************************************************
 */

#ifndef NTL__NT_BASEDEF
#define NTL__NT_BASEDEF

#include "status.hxx"
#include "../basedef.hxx"
#include "../stdlib.hxx"

namespace ntl {
namespace nt {

/**\addtogroup  native_types_support *** NT Types support library ***********
 *@{*/


struct list_entry
{
  list_entry * Flink;
  list_entry * Blink;

  list_entry * next() const { return Flink; }
  list_entry * prev() const { return Blink; }

  void next(list_entry * p)  { Flink = p; }
  void prev(list_entry * p)  { Blink = p; }

  void link(list_entry * prev, list_entry * next)
  {
    this->prev(prev); this->next(next);
    prev->next(this); next->prev(this);
  }

  void unlink()
  {
    next()->prev(prev());
    prev()->next(next());
  }
};


struct list_head : public list_entry
{
  list_head()
  {
    next(this);
    prev(this);
  }

  bool empty() const { return next() == this; }

  void insert(list_entry * position, list_entry * entry)
  {
    entry->link(position, position->next());
  }

  list_entry * erase(list_entry * position)
  { 
    list_entry * const next = position->next();
    position->unlink();
    return next;
  }

  void insert_tail(list_entry * entry)
  {
    entry->link(prev(), this);
  }

  list_entry * begin() { return next(); }
  list_entry * end()   { return this; }
  
};


struct single_list_entry
{
  single_list_entry * Next;

  single_list_entry * next() const { return Next; }
  void next(single_list_entry * p)  { Next = p; }
};

typedef single_list_entry slist_entry;

__align(8)
struct slist_header : public slist_entry
{
  uint16_t    Depth;
  uint16_t    Sequence;
};


/// Masks for the predefined standard access types
enum access_mask
{
  no_access = 0,
  delete_access             = 0x00010000L,
  read_control              = 0x00020000L,
  write_dac                 = 0x00040000L,
  write_owner               = 0x00080000L,
  synchronize               = 0x00100000L,
  standard_rights_required  = delete_access | read_control | write_dac | write_owner,
  standard_rights_read      = read_control,
  standard_rights_write     = read_control,
  standard_rights_execute   = read_control,
  standard_rights_all       = standard_rights_required | synchronize,
  specific_rights_all       = 0x0000FFFFL,
  access_system_security    = 0x01000000L,
  maximum_allowed           = 0x02000000L,
  generic_read              = 0x80000000L,
  generic_write             = 0x40000000L,
  generic_execute           = 0x20000000L,
  generic_all               = 0x10000000L
};

static inline 
access_mask  operator | (access_mask m, access_mask m2) 
{ 
  return bitwise_or(m, m2);
}

static inline 
access_mask  operator & (access_mask m, access_mask m2) 
{ 
  return bitwise_and(m, m2);
}


struct generic_mapping
{
  access_mask generic_read;
  access_mask generic_write;
  access_mask generic_execute;
  access_mask generic_all;
};


struct io_status_block 
{
  union { ntstatus  Status; void * Pointer; };
  uintptr_t Information;
};


typedef
void __stdcall
  io_apc_routine(
    const void *            ApcContext,
    const io_status_block * IoStatusBlock,
    uint32_t                Reserved
    );


/**@} native_types_support */

enum system_power_state 
{
  PowerSystemUnspecified,
  PowerSystemWorking,
  PowerSystemSleeping1,
  PowerSystemSleeping2,
  PowerSystemSleeping3,
  PowerSystemHibernate,
  PowerSystemShutdown,
  PowerSystemMaximum
};


enum power_action 
{
  PowerActionNone,
  PowerActionReserved,
  PowerActionSleep,
  PowerActionHibernate,
  PowerActionShutdown,
  PowerActionShutdownReset,
  PowerActionShutdownOff,
  PowerActionWarmEject
};

enum device_power_state
{
  PowerDeviceUnspecified,
  PowerDeviceD0,
  PowerDeviceD1,
  PowerDeviceD2,
  PowerDeviceD3,
  PowerDeviceMaximum
};


}//namespace ntl
}//namespace nt

#endif//#ifndef NTL__NT_BASEDEF
