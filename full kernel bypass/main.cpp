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
#include "server_shared.h"

using namespace driver;

extern void server_thread();

enum DBG_LEVEL { INF = 0x0, WRN, ERR };



#define	DUMP(level, lpszFormat, ...);
void driver_thread( void* context )
{
	// allow five seconds for driver to finish entry
	//utils::sleep(5000);
	log( "[START] :驱动开始监听 \n");

	// debug text
	log( "cleaning status -> %i \n", cleaning::clean_traces());
	log( "tid -> %i \n", PsGetCurrentThreadId( ));

	
	// user extersize
	bool status = thread::unlink( );

	log( "unlinked thread -> %i \n", status);

	server_thread();
	//      thread 方式读取内存。 在这里可以 建立 SOCKET 进行内存交互 、 Callback 交互等 （ TODO ）

	// sleep for 15 seconds to allow game to get started and prevent us from getting false info
	
	//utils::sleep(7000);
	//HANDLE processId;
	//utils::GetProcessInfo( &processId );

	//PsLookupProcessByProcessId(processId, &process::process);

	//utils::process_by_name( process::process_name, &process::process );
	//log( "peprocess -> 0x%llx", process::process );

	//process::pid = reinterpret_cast< uint32 >( PsGetProcessId( process::process ) );
	//log( "pid -> %i", process::pid);

	//uint64 varInt = memory::read< uint64 >(0x53FB94);
	//memory::read_virtual_memory(process::pid, process::process, (void*)0x53FB94, (void*)packet.dest_address, packet.size);
	/*
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
	
	
	PsTerminateSystemThread( STATUS_SUCCESS );
	*/
}
extern "C"
NTSTATUS KnoveEntry( PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path ) {

	KeEnterGuardedRegion();


	UNREFERENCED_PARAMETER( driver_object );
	UNREFERENCED_PARAMETER( registry_path );
	log("[Knove] :driver 开始初始化.  ———— START \n");

	// change this per mapper; debug prints the entire mmu
	cleaning::debug = false;
	cleaning::driver_timestamp = 0x5284EAC3;
	cleaning::driver_name = RTL_CONSTANT_STRING(L"iqvw64e.sys");

	// PsCreateSystemThread 方式新建系统线程

	//HANDLE thread_handle = nullptr;
	//OBJECT_ATTRIBUTES object_attribues{ };
	//InitializeObjectAttributes( &object_attribues, nullptr, OBJ_KERNEL_HANDLE, nullptr, nullptr );

	//NTSTATUS status = PsCreateSystemThread( &thread_handle, 0, &object_attribues, nullptr, nullptr, reinterpret_cast< PKSTART_ROUTINE >( &driver_thread ), nullptr );

	//log("thread status -> 0x%llx \n", status);

	//ZwClose(thread_handle);



	// ExInitializeWorkItem 方式新建系统线程

	PWORK_QUEUE_ITEM WorkItem = (PWORK_QUEUE_ITEM)ExAllocatePool(NonPagedPool, sizeof(WORK_QUEUE_ITEM));
	if (!WorkItem)
	{
		log("Failed to allocate memory for work item");
	}
	ExInitializeWorkItem(WorkItem, driver_thread, WorkItem);
	ExQueueWorkItem(WorkItem, DelayedWorkQueue);

	KeLeaveGuardedRegion();
	log("[Knove] 完成驱动的 Entry， 关闭驱动 ———— END  \n");
	return STATUS_SUCCESS;
}

