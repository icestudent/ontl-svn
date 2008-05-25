/**\file*********************************************************************
 *                                                                     \brief
 *  Driver Object
 *
 ****************************************************************************
 */

#ifndef NTL__KM_DRIVER_OBJECT
#define NTL__KM_DRIVER_OBJECT

#include "basedef.hxx"
#include "string.hxx"
#include "device_object.hxx"
#include "irp.hxx"

namespace ntl {
namespace km {

struct device_object;
struct driver_object;
struct fast_io_dispatch;

typedef
ntstatus __stdcall
  driver_add_device_t(
    driver_object * DriverObject,
    device_object * PhysicalDeviceObject
    );

struct driver_extension
{
  driver_object *       DriverObject;
  driver_add_device_t * AddDevice;
  uint32_t              Count;
  unicode_string        ServiceKeyName;
  struct _IO_CLIENT_EXTENSION* ClientDriverExtension;
  struct _FS_FILTER_CALLBACKS* FsFilterCallbacks;
};

typedef
ntstatus __stdcall
  driver_initialize_t(
    const driver_object *       DriverObject,
    const const_unicode_string  RegistryPath
    );

typedef
void __stdcall
  driver_reinitialize_t(
    driver_object * DriverObject,
    void *          Context,
    uint32_t        Count
    );

typedef
void __stdcall
  driver_unload_t(
    driver_object * DriverObject
    );


NTL__EXTERNAPI
void __stdcall
  IoRegisterBootDriverReinitialization(
    driver_object *         DriverObject,
    driver_reinitialize_t * DriverReinitializationRoutine,
    void *                  Context
    );

NTL__EXTERNAPI
void __stdcall
  IoRegisterDriverReinitialization(
    driver_object *         DriverObject,
    driver_reinitialize_t * DriverReinitializationRoutine,
    void *                  Context
    );


struct driver_object
{
  uint16_t                    Type;
  uint16_t                    Size;
  device_object *             DeviceObject;
  uint32_t                    Flags;
  void *                      DriverStart;
  uint32_t                    DriverSize;
  nt::ldr_data_table_entry *  DriverSection;
  driver_extension *          DriverExtension;
  unicode_string              DriverName;
  unicode_string *            HardwareDatabase;
  fast_io_dispatch *          FastIoDispatch;
  driver_initialize_t *       DriverInit;
  void    (__stdcall *        DriverStartIo)(device_object*, irp*);
  driver_unload_t *           DriverUnload;

  device_object::dispatch_ptr MajorFunction[28];

  void
    register_boot_driver_reinitialization(
      driver_reinitialize_t * DriverReinitializationRoutine,
      void *                  Context)
  {
    IoRegisterBootDriverReinitialization(this, DriverReinitializationRoutine, Context);
  }

  void
    register_driver_reinitialization(
      driver_reinitialize_t * DriverReinitializationRoutine,
      void *                  Context)
  {
    IoRegisterDriverReinitialization(this, DriverReinitializationRoutine, Context);
  }

};

STATIC_ASSERT(sizeof(driver_object) == 0xA8 || sizeof(driver_object) == 0x150);
#ifdef _M_IX86
STATIC_ASSERT(offsetof(driver_object, MajorFunction) == 0x38 || offsetof(driver_object, MajorFunction) == 0x70);
#endif

__forceinline
ntstatus device_object::call(irp * pirp)
{
//    pirp->CurrentLocation--;
//    io_stack_location * const stack = pirp->get_next_stack_location();
//    pirp->Tail.Overlay.CurrentStackLocation = stack;
  pirp->set_next_stack_location();
#if 0//defined(_DEBUG)
  if( pirp->CurrentLocation <= 0 )
      KeBugCheckEx(NO_MORE_IRP_STACK_LOCATIONS, pirp, 0, 0, 0);
#endif
  io_stack_location * const stack = pirp->get_current_stack_location();
  stack->DeviceObject = this;
  return DriverObject->MajorFunction[stack->MajorFunction](this, pirp);
}
  
}//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_DRIVER_OBJECT
