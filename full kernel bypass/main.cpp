#include <ntifs.h>
#include <ntimage.h>
#include <ntddk.h>
#include <wdf.h>
#include "defs.h"
#include "io/io.h"
#include "utils/utils.h"
#include "memory/memory.h"
#include "thread/thread.h"
#include "cleaning/cleaning.h"
#include "log.hpp"
#include "event.h"
#include "common.h"
#include "data.h"

using namespace driver;

enum DBG_LEVEL { INF = 0x0, WRN, ERR };

#define	DUMP(level, lpszFormat, ...);
void driver_thread(void* context)
{
	// allow five seconds for driver to finish entry
	utils::sleep(5000);
	log("[START] :驱动开始监听 \n");

	// debug text
	log("cleaning status -> %i \n", cleaning::clean_traces());
	log("tid -> %i \n", PsGetCurrentThreadId());


	// user extersize
	bool status = thread::unlink();

	log("unlinked thread -> %i \n", status);

	// change your process name here
	process::process_name = "readm.exe";
	log("process name -> %s", process::process_name);


	//      这里是 process_by_name 的使用场景

	// scuff check to check if our peprocess is valid
	//while ( utils::process_by_name( process::process_name, &process::process ) == STATUS_NOT_FOUND)
	//{
	//	log( "waiting for -> %s", process::process_name );
	//	utils::sleep(2000);
	//}
	//log("found process -> %s", process::process_name);


	//      thread 方式读取内存。 在这里可以 建立 SOCKET 进行内存交互 、 Callback 交互等 （ TODO ）

	// sleep for 15 seconds to allow game to get started and prevent us from getting false info
	/*
	utils::sleep(7000);
	HANDLE processId;
	utils::GetProcessInfo( &processId );

	PsLookupProcessByProcessId(processId, &process::process);

	utils::process_by_name( process::process_name, &process::process );
	log( "peprocess -> 0x%llx", process::process );

	process::pid = reinterpret_cast< uint32 >( PsGetProcessId( process::process ) );
	log( "pid -> %i", process::pid);

	process::base_address = reinterpret_cast < uint64 >( PsGetProcessSectionBaseAddress( process::process ) );
	log( "base address -> 0x%llx", process::base_address );


	// main loop
	int times = 0;
	while ( times < 10 )
	{
		log( "———— LOOP START ————", process::base_address);
		//example read
		uint64 round_manager = memory::read< uint64 >( process::base_address + 0x53FBB8);
		uint64 varInt = memory::read< uint64 >( 0x53FB94 );
		//uint32 encrypted_round_state = memory::read< uint32 >( round_manager + 0xC0 );
		//uint32 decrypted_round_state = _rotl64(round_manager - 0x56, 0x1E );

		log( "round_manager -> 0x%llx", varInt);

		utils::sleep(3000);
		times += 5;
	}

	*/
	PsTerminateSystemThread(STATUS_SUCCESS);
}


void UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	log("[END] END dDriver!");

	PsRemoveLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)ImageLoadCallback);

	IoDeleteSymbolicLink(&dos);
	IoDeleteDevice(pDriverObject->DeviceObject);
}


extern "C"
NTSTATUS KnoveEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path) {
	UNREFERENCED_PARAMETER(driver_object);
	UNREFERENCED_PARAMETER(registry_path);
	log("[Knove] :IOCTL driver 开始初始化.  ———— START \n");

	// IOCTL INIT

	// 会蓝屏 艹
	//driver_object->DriverUnload = UnloadDriver;

	PsSetLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)ImageLoadCallback);

	//RtlInitUnicodeString(&dev, L"\\Device\\knove");
	//RtlInitUnicodeString(&dos, L"\\DosDevices\\knove");

	//IoCreateDevice(driver_object, 0, &dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
	//IoCreateSymbolicLink(&dos, &dev);

	//driver_object->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	//driver_object->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	//driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

	//pDeviceObject->Flags |= DO_DIRECT_IO;
	//pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	log("[Knove] :IOCTL 部署完成 CreateCall CloseCall IoControl.  ———— RUNING \n");


	// change this per mapper; debug prints the entire mmu
	cleaning::debug = true;
	cleaning::driver_timestamp = 0x5284EAC3;
	cleaning::driver_name = RTL_CONSTANT_STRING(L"iqvw64e.sys");

	HANDLE thread_handle = nullptr;
	OBJECT_ATTRIBUTES object_attribues{ };
	InitializeObjectAttributes(&object_attribues, nullptr, OBJ_KERNEL_HANDLE, nullptr, nullptr);

	NTSTATUS status = PsCreateSystemThread(&thread_handle, 0, &object_attribues, nullptr, nullptr, reinterpret_cast<PKSTART_ROUTINE>(&driver_thread), nullptr);

	log("thread status -> 0x%llx \n", status);
	/**/
	log("[Knove] 完成驱动的 Entry， 关闭驱动 ———— END  \n");
	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path)
{
	UNREFERENCED_PARAMETER(driver_object);
	UNREFERENCED_PARAMETER(registry_path);

	// This isn't a standard way against better anticheats such as BE, and EAC.
	// Could give you a good example though.
	UNICODE_STRING  drvName;
	NTSTATUS status;
	RtlInitUnicodeString(&drvName, L"\\Driver\\Knove");
	status = IoCreateDriver(&drvName, &KnoveEntry);
	if (NT_SUCCESS(status))
	{
		log("Created driver.\n");
	}
	return status;
