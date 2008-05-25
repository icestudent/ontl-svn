/**\file*********************************************************************
*                                                                     \brief
*  Fast I/O procedure prototypes.
*
****************************************************************************
*/
#pragma once

#include "basedef.hxx"
#include "file_information.hxx"

namespace ntl {
	namespace km {

struct eprocess;
struct eresource;

struct compressed_data_info
{
	/*<+0x0>*/ /*|0x2|*/ uint16_t CompressionFormatAndEngine;
	/*<+0x2>*/ /*|0x1|*/ uint8_t  CompressionUnitShift;
	/*<+0x3>*/ /*|0x1|*/ uint8_t  ChunkShift;
	/*<+0x4>*/ /*|0x1|*/ uint8_t  ClusterShift;
	/*<+0x5>*/ /*|0x1|*/ uint8_t  Reserved;
	/*<+0x6>*/ /*|0x2|*/ int16_t  NumberOfChunks;
	/*<+0x8>*/ /*|0x4|*/ int32_t  CompressedChunkSizes[1];
};
STATIC_ASSERT(sizeof(compressed_data_info) == 0xc);

///\name fast I/O read and write procedures.

typedef
bool __stdcall
  fast_io_check_if_possible_t(
						   const file_object *    FileObject,
						   const int64_t *        FileOffset,
						   const uint32_t         Length,
						   const bool             Wait,
						   const uint32_t         LockKey,
						   const bool             CheckForReadOperation,
						         io_status_block* IoStatus,
						   const device_object *  DeviceObject
						   );

typedef
bool __stdcall
fast_io_read_t (
			  const file_object *FileObject,
			  const int64_t* FileOffset,
			  const uint32_t Length,
			  const bool Wait,
			  const uint32_t LockKey,
			        void* Buffer,
			        io_status_block* IoStatus,
			  const device_object *DeviceObject
			  );

typedef
bool __stdcall
fast_io_write_t (
			   const file_object *FileObject,
			   const int64_t* FileOffset,
			   const uint32_t Length,
			   const bool Wait,
			   const uint32_t LockKey,
			   const void* Buffer,
			         io_status_block* IoStatus,
			   const device_object *DeviceObject
			   );

///\name fast I/O query basic and standard information procedures.

typedef
bool __stdcall
fast_io_query_basic_info_t (
						  const file_object *FileObject,
						  const bool Wait,
						        file_basic_information* Buffer,
						        io_status_block* IoStatus,
						  const device_object *DeviceObject
						  );

typedef
bool __stdcall
fast_io_query_standard_info_t (
							 const file_object *FileObject,
							 const bool Wait,
							       file_standard_information* Buffer,
							       io_status_block* IoStatus,
							 const device_object *DeviceObject
							 );

///\name fast I/O lock and unlock procedures.

typedef
bool __stdcall
fast_io_lock_t (
			  const file_object *FileObject,
			  const int64_t* FileOffset,
			  const int64_t* Length,
			  const eprocess* ProcessId,
			  const uint32_t Key,
			  const bool FailImmediately,
			  const bool ExclusiveLock,
			        io_status_block* IoStatus,
			  const device_object *DeviceObject
			  );

typedef
bool __stdcall
fast_io_unlock_single_t (
					   const file_object *FileObject,
					   const int64_t* FileOffset,
					   const int64_t* Length,
					   const eprocess* ProcessId,
					   const uint32_t Key,
					         io_status_block* IoStatus,
					   const device_object *DeviceObject
					   );

typedef
bool __stdcall
fast_io_unlock_all_t (
					const file_object *FileObject,
					const eprocess* ProcessId,
					      io_status_block* IoStatus,
					const device_object *DeviceObject
					);

typedef
bool __stdcall
fast_io_unlock_all_by_key_t (
						   const file_object *FileObject,
						   const void* ProcessId,
						   const uint32_t Key,
						         io_status_block* IoStatus,
						   const device_object *DeviceObject
						   );

///\name fast I/O device control procedure.

typedef
bool __stdcall
fast_io_device_control_t (
						const file_object *FileObject,
						const bool Wait,
						const void* InputBuffer,          __optional
						const uint32_t InputBufferLength,
					        void* OutputBuffer,         __optional
						const uint32_t OutputBufferLength,
						const uint32_t IoControlCode,
						      io_status_block* IoStatus,
						const device_object *DeviceObject
						);

///\name callbacks for NtCreateSection to synchronize correctly with the file system

typedef
void __stdcall
fast_io_acquire_file_t (
					  const file_object *FileObject
					  );

typedef
void __stdcall
fast_io_release_file_t (
					  const file_object *FileObject
					  );

///\name callback for drivers that have device objects attached to lower-level drivers' device objects.

typedef
void __stdcall
fast_io_detach_device_t (
					   const device_object *SourceDevice,
					   const device_object *TargetDevice
					   );

///\name

typedef
bool __stdcall
fast_io_query_network_open_info_t (
								 const file_object *FileObject,
								 const bool Wait,
								       file_network_open_information *Buffer,
								       io_status_block *IoStatus,
								 const device_object *DeviceObject
								 );

///\name  Mdl-based routines for the server to call

typedef
bool __stdcall
fast_io_mdl_read_t (
				  const file_object *FileObject,
				  const int64_t* FileOffset,
				  const uint32_t Length,
				  const uint32_t LockKey,
				        mdl* *MdlChain,
				        io_status_block* IoStatus,
				  const device_object *DeviceObject
				  );

typedef
bool __stdcall
fast_io_mdl_read_complete_t (
						   const file_object *FileObject,
						   const mdl* MdlChain,
						   const device_object *DeviceObject
						   );

typedef
bool __stdcall
fast_io_prepare_mdl_write_t (
						   const file_object *FileObject,
						   const int64_t* FileOffset,
						   const uint32_t Length,
						   const uint32_t LockKey,
						         mdl* *MdlChain,
						         io_status_block* IoStatus,
						   const device_object *DeviceObject
						   );

typedef
bool __stdcall
fast_io_mdl_write_complete_t (
							const file_object *FileObject,
							const int64_t* FileOffset,
							const mdl* MdlChain,
							const device_object *DeviceObject
							);

///\name acquire the file for the mapped page writer.

typedef
ntstatus __stdcall
fast_io_acquire_for_mod_write_t (
							   const file_object *FileObject,
							   const int64_t* EndingOffset,
							         eresource **ResourceToRelease,
							   const device_object *DeviceObject
							   );

typedef
ntstatus __stdcall
fast_io_release_for_mod_write_t (
							   const file_object *FileObject,
							   const eresource *ResourceToRelease,
							   const device_object *DeviceObject
							   );

///\name

typedef
ntstatus __stdcall
fast_io_acquire_for_ccflush_t (
							 const file_object *FileObject,
							 const device_object *DeviceObject
							 );

typedef
ntstatus __stdcall
fast_io_release_for_ccflush_t (
							 const file_object *FileObject,
							 const device_object *DeviceObject
							 );

typedef
bool __stdcall
fast_io_read_compressed_t (
						 const file_object *FileObject,
						 const int64_t* FileOffset,
						 const uint32_t Length,
						 const uint32_t LockKey,
						       void* Buffer,
						       mdl* *MdlChain,
						       io_status_block* IoStatus,
						       compressed_data_info *CompressedDataInfo,
						 const uint32_t CompressedDataInfoLength,
						 const device_object *DeviceObject
						 );

typedef
bool __stdcall
fast_io_write_compressed_t (
						  const file_object *FileObject,
						  const int64_t* FileOffset,
						  const uint32_t Length,
						  const uint32_t LockKey,
						  const void* Buffer,
						        mdl* *MdlChain,
						        io_status_block* IoStatus,
						  const compressed_data_info *CompressedDataInfo,
						  const uint32_t CompressedDataInfoLength,
						  const device_object *DeviceObject
						  );

typedef
bool __stdcall
fast_io_mdl_read_complete_compressed_t (
									  const file_object *FileObject,
									  const mdl* MdlChain,
									  const device_object *DeviceObject
									  );

typedef
bool __stdcall
fast_io_mdl_write_complete_compressed_t (
									   const file_object *FileObject,
									   const int64_t* FileOffset,
									   const mdl* MdlChain,
									   const device_object *DeviceObject
									   );

typedef
bool __stdcall
fast_io_query_open_t (
					const irp *Irp,
					      file_network_open_information NetworkInformation,
					const device_object *DeviceObject
					);

/// structure to describe the Fast I/O dispatch routines.
struct fast_io_dispatch
{
	uint32_t                                  SizeOfFastIoDispatch;
	fast_io_check_if_possible_t *             FastIoCheckIfPossible;
	fast_io_read_t *                          FastIoRead;
	fast_io_write_t *                         FastIoWrite;
	fast_io_query_basic_info_t *              FastIoQueryBasicInfo;
	fast_io_query_standard_info_t *           FastIoQueryStandardInfo;
	fast_io_lock_t *                          FastIoLock;
	fast_io_unlock_single_t *                 FastIoUnlockSingle;
	fast_io_unlock_all_t *                    FastIoUnlockAll;
	fast_io_unlock_all_by_key_t *             FastIoUnlockAllByKey;
	fast_io_device_control_t *                FastIoDeviceControl;
	fast_io_acquire_file_t *                  AcquireFileForNtCreateSection;
	fast_io_release_file_t *                  ReleaseFileForNtCreateSection;
	fast_io_detach_device_t *                 FastIoDetachDevice;
	fast_io_query_network_open_info_t *       FastIoQueryNetworkOpenInfo;
	fast_io_acquire_for_mod_write_t *         AcquireForModWrite;
	fast_io_mdl_read_t *                      MdlRead;
	fast_io_mdl_read_complete_t *             MdlReadComplete;
	fast_io_prepare_mdl_write_t *             PrepareMdlWrite;
	fast_io_mdl_write_complete_t *            MdlWriteComplete;
	fast_io_read_compressed_t *               FastIoReadCompressed;
	fast_io_write_compressed_t *              FastIoWriteCompressed;
	fast_io_mdl_read_complete_compressed_t *  MdlReadCompleteCompressed;
	fast_io_mdl_write_complete_compressed_t * MdlWriteCompleteCompressed;
	fast_io_query_open_t *                    FastIoQueryOpen;
	fast_io_release_for_mod_write_t *         ReleaseForModWrite;
	fast_io_acquire_for_ccflush_t *           AcquireForCcFlush;
	fast_io_release_for_ccflush_t *           ReleaseForCcFlush;
};

STATIC_ASSERT(sizeof(fast_io_dispatch) == 0x70 || sizeof(fast_io_dispatch) == 0xE0);

///\}

	}//namespace km
}//namespace ntl
