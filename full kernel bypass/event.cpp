#pragma warning (disable : 4047)

#include "event.h"
#include "data.h"

PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
	//DebugMessage("ImageLoaded: %ls \n", FullImageName->Buffer);

	//if (wcsstr(FullImageName->Buffer, L"\\Black Mesa\\bms\\bin\\client.dll"))
	//{
	//	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Black Mesa Client.DLL found!\n");

	//	BlackMesaClientDLLAddress = ImageInfo->ImageBase;

	//	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ProcessID: %d \n", ProcessId);
	//}

	return STATUS_SUCCESS;
}