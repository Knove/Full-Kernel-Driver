#include <ntifs.h>
#include "utils.h"
#include "../defs.h"
#include <ntstrsafe.h>



typedef struct _SYSTEM_THREAD_INFORMATION
{
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID ClientId;
    KPRIORITY Priority;
    LONG BasePriority;
    ULONG ContextSwitches;
    ULONG ThreadState;
    KWAIT_REASON WaitReason;
}SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFO
{
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER WorkingSetPrivateSize;
    ULONG HardFaultCount;
    ULONG NumberOfThreadsHighWatermark;
    ULONGLONG CycleTime;
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    ULONG_PTR UniqueProcessKey;
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER ReadOperationCount;
    LARGE_INTEGER WriteOperationCount;
    LARGE_INTEGER OtherOperationCount;
    LARGE_INTEGER ReadTransferCount;
    LARGE_INTEGER WriteTransferCount;
    LARGE_INTEGER OtherTransferCount;
    SYSTEM_THREAD_INFORMATION Threads[1];
}SYSTEM_PROCESS_INFO, * PSYSTEM_PROCESS_INFO;


NTSTATUS driver::utils::process_by_name(CHAR* process_name, PEPROCESS* process)
{
    PEPROCESS sys_process = PsInitialSystemProcess;
    PEPROCESS cur_entry = sys_process;
    CHAR image_name[15];

    do
    {
        // win10 20H2 0x5a8
        RtlCopyMemory( ( PVOID )( &image_name ), ( PVOID )( ( uintptr_t )cur_entry + 0x5a8) /*EPROCESS->ImageFileName*/, sizeof( image_name ) );

        if ( strstr ( image_name, process_name ) )
        {
            ULONG active_threads;
            // win10 20H2 0x5f0
            RtlCopyMemory( ( PVOID ) &active_threads, ( PVOID )( ( uintptr_t )cur_entry + 0x5f0) /*EPROCESS->ActiveThreads*/, sizeof( active_threads ) );
            if ( active_threads )
            {
                *process = cur_entry;
                return STATUS_SUCCESS;
            }
        }
        // win10 20H2 0x448
        PLIST_ENTRY list = (PLIST_ENTRY)((uintptr_t)(cur_entry)+0x448) /*EPROCESS->ActiveProcessLinks*/;
        cur_entry = (PEPROCESS)((uintptr_t)list->Flink - 0x448);

    } while (cur_entry != sys_process);

    return STATUS_NOT_FOUND;
}

int driver::utils::GetProcessInfo(HANDLE* processId) {

	NTSTATUS status = STATUS_SUCCESS;
	PVOID buffer;


	buffer = ExAllocatePoolWithTag(NonPagedPool, 1024 * 1024, 'enoN');

	if (!buffer) {
		DbgPrintEx(0, 0, "couldn't allocate memory \n");
		return 0;
	}

	DbgPrintEx(0, 0, "Process list allocated at address %#x\n", buffer);

	PSYSTEM_PROCESS_INFO pInfo = (PSYSTEM_PROCESS_INFO)buffer;


	status = ZwQuerySystemInformation(SystemProcessInformation, pInfo, 1024 * 1024, NULL);
	if (!NT_SUCCESS(status)) {
		DbgPrintEx(0, 0, "ZwQuerySystemInformation Failed : STATUS CODE : %p\n", status);
		ExFreePoolWithTag(buffer, 'enoN');
		return 0;
	}

	UNICODE_STRING WantedImageName;
	RtlInitUnicodeString(&WantedImageName, L"readm.exe");

	if (NT_SUCCESS(status)) {
		for (;;) {
			// DbgPrintEx(0, 0, "\nProcess name: %ws | Process ID: %d\n", pInfo->ImageName.Buffer, pInfo->UniqueProcessId); // Display process information.
			if (RtlEqualUnicodeString(&pInfo->ImageName, &WantedImageName, TRUE)) {
                *processId = pInfo->UniqueProcessId;
				DbgPrintEx(0, 0, "readm.exe has just started!\n");

				break;
			}
			else if (pInfo->NextEntryOffset)
				pInfo = (PSYSTEM_PROCESS_INFO)((PUCHAR)pInfo + pInfo->NextEntryOffset);
			else
				break;
		}
	}

	ExFreePoolWithTag(buffer, 'enoN');
}


//NTSTATUS inVGetProcessInformationInternal(
//    _In_    void* InputOutputBuffer_,
//    _Inout_ _IRP* IRP_)
//{
//    /* Convenience-conversion */
//    auto InputOutputBuffer = reinterpret_cast<inVGetProcessIdBuffer*>(InputOutputBuffer_);
//
//    /* Convenience-reference */
//    auto& StatusCode = InputOutputBuffer->StatusCode;
//
//    /* Will be filled with the size needed to be allocated to fit the entire process list */
//    auto SizeToAllocate = unsigned long{ };
//
//    StatusCode = ZwQuerySystemInformation(SystemProcessInformation,
//        nullptr,
//        NULL,
//        &SizeToAllocate);
//
//    /* We expect the STATUS_INFO_LENGTH_MISMATCH status code, if we don't get that one, something is wrong */
//    if (StatusCode != STATUS_INFO_LENGTH_MISMATCH)
//    {
//        inVPrintEx("[inVc]: inVGetProcessInformationInternal - ZwQuerySystemInformation failed with status code 0x%X\n", StatusCode);
//
//        /* Set the amount of bytes written to zero as we're bailing out without writing anything to the user-supplied buffer */
//        IRP_->IoStatus.Information = NULL;
//
//        return StatusCode;
//    }
//
//    /* Allocate space for the process buffer to be filled by ZwQuerySystemInformation */
//    auto ProcessListBuffer = new(PagedPool | POOL_COLD_ALLOCATION) unsigned char[SizeToAllocate];
//
//    /* Allocation failed, bail out */
//    if (!ProcessListBuffer)
//    {
//        inVPrintEx("[inVc]: inVGetProcessInformationInternal - ZwQuerySystemInformation failed with status code 0x%X\n", StatusCode);
//
//        /* Set the amount of bytes written to zero as we're bailing out without writing anything to the user-supplied buffer */
//        IRP_->IoStatus.Information = NULL;
//
//        return STATUS_INSUFFICIENT_RESOURCES;
//    }
//
//    /* Get the process list */
//    StatusCode = ZwQuerySystemInformation(SystemProcessInformation,
//        ProcessListBuffer,
//        SizeToAllocate,
//        &SizeToAllocate);
//
//    if (!NT_SUCCESS(StatusCode))
//    {
//        /* It could've happened that the process list size changed between the two ZwQuerySystemInformation calls */
//        if (StatusCode == STATUS_INFO_LENGTH_MISMATCH)
//        {
//            /* Make sure to delete the buffer we just allocated since we're going to create a new one */
//            delete ProcessListBuffer;
//
//            /* Recursively call inVGetProcessIdInternal if the length mismatches until the sizes match */
//            return inVGetProcessInformationInternal(InputOutputBuffer_, IRP_);
//        }
//
//        inVPrintEx("[inVc]: inVGetProcessInformationInternal - ZwQuerySystemInformation failed with status code 0x%X\n", StatusCode);
//
//        /* Set the amount of bytes written to zero as we're bailing out without writing anything to the user-supplied buffer */
//        IRP_->IoStatus.Information = NULL;
//
//        /* We're going to return without success, so make sure to free the allocated memory */
//        delete ProcessListBuffer;
//
//        /* Return the error code back to the user mode application via the StatusCode reference */
//        return StatusCode;
//    }
//
//    /* Cast to the correct structure */
//    auto ProcessInformationEntry = reinterpret_cast<_SYSTEM_PROCESS_INFORMATION*>(ProcessListBuffer);
//
//    auto DesiredImageFileName = UNICODE_STRING{ };
//
//    __try
//    {
//        /* Check if the address is page aligned and within user address space ( not within kernel address space ) */
//        ProbeForRead(reinterpret_cast<void*>(InputOutputBuffer->ImageFileName), InputOutputBuffer->ImageFileNameLength, 1);
//
//        /* Validate access to the buffer in usermode to make sure it's not pointing to non-accessible user-mode memory */
//        ValidateAccess(InputOutputBuffer->ImageFileName, InputOutputBuffer->ImageFileNameLength);
//
//        /* Create a UNICODE_STRING from our desired image file name for comparison via RtlEqualUnicodeString */
//        RtlInitUnicodeString(&DesiredImageFileName, InputOutputBuffer->ImageFileName);
//    }
//    __except (EXCEPTION_EXECUTE_HANDLER)
//    {
//        /* Grab the exception code to redirect straight back to the user mode application */
//        StatusCode = _exception_code();
//
//        inVPrintEx("[inVc]: inVGetProcessInformationInternal - Access validation of ImageFileName at 0x%p with length %d failed with status code 0x%d\n", InputOutputBuffer->ImageFileName, InputOutputBuffer->ImageFileNameLength, StatusCode);
//
//        /* Set the amount of bytes written to zero as we're bailing out without writing anything to the user-supplied buffer */
//        IRP_->IoStatus.Information = NULL;
//
//        /* Return the exception code */
//        return StatusCode;
//    }
//
//    do
//    {
//        /* Compare the current process' image name with the desired one ( case-insensitive ) */
//        if (RtlEqualUnicodeString(&ProcessInformationEntry->ImageName, &DesiredImageFileName, TRUE))
//        {
//            /* Due to METHOD_NEITHER, the user-mode application could access the buffers as well, so wrap this in a __try__except block */
//            __try
//            {
//                /* Check if the address is page aligned and within user address space ( not within kernel address space ) */
//                ProbeForRead(InputOutputBuffer->ProcessInformation, sizeof(_SYSTEM_PROCESS_INFORMATION), 1);
//
//                /* Validate access to the buffer in usermode to make sure it's not pointing to non-accessible user-mode memory */
//                ValidateAccess(InputOutputBuffer->ProcessInformation, sizeof(_SYSTEM_PROCESS_INFORMATION));
//
//                /* If validating went fine, fill the process information structure */
//                *InputOutputBuffer->ProcessInformation = *ProcessInformationEntry;
//            }
//            __except (EXCEPTION_EXECUTE_HANDLER)
//            {
//                /* Grab the exception code to redirect straight back to the user mode application */
//                StatusCode = _exception_code();
//
//                inVPrintEx("[inVc]: inVGetProcessInformationInternal - Buffer validation of user-mode _SYSTEM_PROCESS_INFORMATION supplied buffer at 0x%p failed with status code 0x%d\n", InputOutputBuffer->ProcessInformation, StatusCode);
//
//                /* Set the amount of bytes written to zero as we're bailing out without writing anything to the user-supplied buffer */
//                IRP_->IoStatus.Information = NULL;
//
//                /* Return the exception code */
//                return StatusCode;
//            }
//
//            /* Don't forget to free the allocated memory for the process list */
//            delete ProcessListBuffer;
//
//            /* Set the amount of bytes written to zero as we're bailing out without writing anything to the user-supplied buffer */
//            IRP_->IoStatus.Information = sizeof(_SYSTEM_PROCESS_INFORMATION);
//
//            /* Return our success */
//            return StatusCode;
//        }
//
//        /* Check if we are at the end of the list, if so, bail out */
//        if (ProcessInformationEntry->NextEntryOffset == NULL) break;
//
//        /* Get the next entry in the list by adding the offset to the next entry to its own address */
//        ProcessInformationEntry = reinterpret_cast<_SYSTEM_PROCESS_INFORMATION*>(
//            reinterpret_cast<char*>(ProcessInformationEntry) + ProcessInformationEntry->NextEntryOffset);
//
//    } while (true);
//
//    inVPrintEx("[inVc]: inVGetProcessInformationInternal - Desired process %ws could not be found\n", InputOutputBuffer->ImageFileName);
//
//    /* Don't forget to free up the memory for the process list buffer */
//    delete ProcessListBuffer;
//
//    /* Set the amount of bytes written to zero as we're bailing out without writing anything to the user-supplied buffer */
//    IRP_->IoStatus.Information = sizeof(_SYSTEM_PROCESS_INFORMATION);
//
//    /* Process wasn't found, so return that to the user-mode application */
//    return STATUS_NOT_FOUND;
//}