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

using namespace driver;

enum DBG_LEVEL { INF = 0x0, WRN, ERR };



#define	DUMP(level, lpszFormat, ...);
void driver_thread( void* context )
{
	// allow five seconds for driver to finish entry
	utils::sleep(5000);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[START] :Çý¶¯¿ªÊ¼¼àÌý \n");

	// debug text
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "cleaning status -> %i \n", cleaning::clean_traces());
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "tid -> %i \n", PsGetCurrentThreadId( ));

	
	// user extersize
	bool status = thread::unlink( );

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "unlinked thread -> %i \n", status);
	
	// change your process name here
	process::process_name = "readm.exe";
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "process name -> %s", process::process_name );


	// scuff check to check if our peprocess is valid
	//while ( utils::process_by_name( process::process_name, &process::process ) == STATUS_NOT_FOUND)
	//{
	//	io::dbgprint( "waiting for -> %s", process::process_name );
	//	utils::sleep(2000);
	//}
	//io::dbgprint("found process -> %s", process::process_name);

	
	// sleep for 15 seconds to allow game to get started and prevent us from getting false info
	utils::sleep(7000);
	HANDLE processId;
	utils::GetProcessInfo( &processId );

	PsLookupProcessByProcessId(processId, &process::process);
	//utils::process_by_name( process::process_name, &process::process );
	//io::dbgprint( "peprocess -> 0x%llx", process::process );

	process::pid = reinterpret_cast< uint32 >( PsGetProcessId( process::process ) );
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "pid -> %i", process::pid);

	process::base_address = reinterpret_cast < uint64 >( PsGetProcessSectionBaseAddress( process::process ) );
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "base address -> 0x%llx", process::base_address );
	

	// main loop
	int times = 0;
	while ( times < 10 )
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "¡ª¡ª¡ª¡ª LOOP START ¡ª¡ª¡ª¡ª", process::base_address);
		//example read
		uint64 round_manager = memory::read< uint64 >( process::base_address + 0x53FBB8);
		uint32 varInt = memory::read< uint32 >( 0x53FBB8 );
		int varInt1 = memory::read< int >( 0x53FBB8 );
		//uint32 encrypted_round_state = memory::read< uint32 >( round_manager + 0xC0 );
		//uint32 decrypted_round_state = _rotl64(round_manager - 0x56, 0x1E );

		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "round_manager -> 0x%llx", varInt);
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "round_manager -> %d", varInt1);

		utils::sleep(3000);
		times += 5;
		// example write
		//memory::write< uint32 >( round_manager + 0xC0, 0x0 );

		// for testing
		//if ( thread::terminate_thread ) 
		//{
		//	io::dbgprint( "loops -> %i", thread::total_loops );
		//	utils::sleep( 5000 );
		//	thread::total_loops++;

		//	if ( thread::total_loops > thread::loops_before_end )
		//	{
		//		io::dbgprint( "terminating thread" );
		//		PsTerminateSystemThread( STATUS_SUCCESS );
		//	}
		//}
	}
	/*
	*/
	PsTerminateSystemThread( STATUS_SUCCESS );
}
extern "C"
NTSTATUS DriverEntry( PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path ) {
	UNREFERENCED_PARAMETER( driver_object );
	UNREFERENCED_PARAMETER( registry_path );
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[DRV] :driver entry called. \n");
	// change this per mapper; debug prints the entire mmu
	cleaning::debug = false;
	cleaning::driver_timestamp = 0x5284EAC3;
	cleaning::driver_name = RTL_CONSTANT_STRING(L"iqvw64e.sys");

	HANDLE thread_handle = nullptr;
	OBJECT_ATTRIBUTES object_attribues{ };
	InitializeObjectAttributes( &object_attribues, nullptr, OBJ_KERNEL_HANDLE, nullptr, nullptr );

	NTSTATUS status = PsCreateSystemThread( &thread_handle, 0, &object_attribues, nullptr, nullptr, reinterpret_cast< PKSTART_ROUTINE >( &driver_thread ), nullptr );

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "thread status -> 0x%llx \n", status);

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[DRV] :fininshed driver entry... closing.... \n");
	return STATUS_SUCCESS;
}

