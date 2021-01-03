#pragma warning (disable : 4047)

#include "event.h"
#include "data.h"
#include "log.hpp"

PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
	//DebugMessage("ImageLoaded: %ls \n", FullImageName->Buffer);

	if (wcsstr(FullImageName->Buffer, L"\\readm.exe"))
	{
		log("Black Mesa Client.DLL found!\n");
		BlackMesaClientDLLAddress = (ULONG)ImageInfo->ImageBase;

		log("ProcessID: %d \n", ProcessId);
	}
	return STATUS_SUCCESS;
}