#pragma once
#pragma once
#include <ntddk.h>
#include <intrin.h>
#include <stdlib.h>
#include <Ntstrsafe.h>
#include <windef.h>



typedef struct _DYNDATA
{
	UINT64 CallbackListOffset;
}DYNDATA, * PDYNDATA;

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
	HANDLE Section;         // Not filled in
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR  FullPathName[MAXIMUM_FILENAME_LENGTH];
} RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, * PRTL_PROCESS_MODULES;

typedef struct _CALLBACK_ENTRY {
	UINT16 Version; // 0x0
	UINT16 OperationRegistrationCount; // 0x2
	UINT32 unk1; // 0x4
	PVOID RegistrationContext; // 0x8
	UNICODE_STRING Altitude; // 0x10
} CALLBACK_ENTRY, * PCALLBACK_ENTRY;

typedef struct _OBJECT_CALLBACK_ENTRY {
	LIST_ENTRY CallbackList;
	OB_OPERATION Operations;
	ULONG Active;
	/*OB_HANDLE*/ PCALLBACK_ENTRY CallbackEntry;
	POBJECT_TYPE ObjectType;
	POB_PRE_OPERATION_CALLBACK  PreOperation;
	POB_POST_OPERATION_CALLBACK PostOperation;
} OBJECT_CALLBACK_ENTRY, * POBJECT_CALLBACK_ENTRY;

struct REQUEST_STRUCT
{
	PIO_COMPLETION_ROUTINE OldRoutine;
	PVOID OldContext;
	ULONG OutputBufferLength;
	PVOID SystemBuffer;
};

typedef struct _PS_CALLBACK_ENTRY
{
	PVOID* Callback;
	LARGE_INTEGER* Fillz;
} PS_CALLBACK_ENTRY, * PPS_CALLBACK_ENTRY;

typedef struct _OB_CALLBACK_ADDRESSES
{
	UINT64* pProcPreCallback, * pProcPostCallback;
	UINT64* pThreadPreCallback, * pThreadPostCallback;
	UINT64 OrigProcPre, OrigProcPost;
	UINT64 OrigThreadPre, OrigThreadPost;
}OB_CALLBACK_ADDRESSES, * POB_CALLBACK_ADDRESSES;

typedef struct _IDINFO
{
	USHORT	wGenConfig;
	USHORT	wNumCyls;
	USHORT	wReserved;
	USHORT	wNumHeads;
	USHORT	wBytesPerTrack;
	USHORT	wBytesPerSector;
	USHORT	wNumSectorsPerTrack;
	USHORT	wVendorUnique[3];
	CHAR	sSerialNumber[20];
	USHORT	wBufferType;
	USHORT	wBufferSize;
	USHORT	wECCSize;
	CHAR	sFirmwareRev[8];
	CHAR	sModelNumber[40];
	USHORT	wMoreVendorUnique;
	USHORT	wDoubleWordIO;
	struct {
		USHORT	Reserved : 8;
		USHORT	DMA : 1;
		USHORT	LBA : 1;
		USHORT	DisIORDY : 1;
		USHORT	IORDY : 1;
		USHORT	SoftReset : 1;
		USHORT	Overlap : 1;
		USHORT	Queue : 1;
		USHORT	InlDMA : 1;
	} wCapabilities;
	USHORT	wReserved1;
	USHORT	wPIOTiming;
	USHORT	wDMATiming;
	struct {
		USHORT	CHSNumber : 1;
		USHORT	CycleNumber : 1;
		USHORT	UnltraDMA : 1;
		USHORT	Reserved : 13;
	} wFieldValidity;
	USHORT	wNumCurCyls;
	USHORT	wNumCurHeads;
	USHORT	wNumCurSectorsPerTrack;
	USHORT	wCurSectorsLow;
	USHORT	wCurSectorsHigh;
	struct {
		USHORT	CurNumber : 8;
		USHORT	Multi : 1;
		USHORT	Reserved : 7;
	} wMultSectorStuff;
	ULONG	dwTotalSectors;
	USHORT	wSingleWordDMA;
	struct {
		USHORT	Mode0 : 1;
		USHORT	Mode1 : 1;
		USHORT	Mode2 : 1;
		USHORT	Reserved1 : 5;
		USHORT	Mode0Sel : 1;
		USHORT	Mode1Sel : 1;
		USHORT	Mode2Sel : 1;
		USHORT	Reserved2 : 5;
	} wMultiWordDMA;
	struct {
		USHORT	AdvPOIModes : 8;
		USHORT	Reserved : 8;
	} wPIOCapacity;
	USHORT	wMinMultiWordDMACycle;
	USHORT	wRecMultiWordDMACycle;
	USHORT	wMinPIONoFlowCycle;
	USHORT	wMinPOIFlowCycle;
	USHORT	wReserved69[11];
	struct {
		USHORT	Reserved1 : 1;
		USHORT	ATA1 : 1;
		USHORT	ATA2 : 1;
		USHORT	ATA3 : 1;
		USHORT	ATA4 : 1;
		USHORT	ATA5 : 1;
		USHORT	ATA6 : 1;
		USHORT	ATA7 : 1;
		USHORT	ATA8 : 1;
		USHORT	ATA9 : 1;
		USHORT	ATA10 : 1;
		USHORT	ATA11 : 1;
		USHORT	ATA12 : 1;
		USHORT	ATA13 : 1;
		USHORT	ATA14 : 1;
		USHORT	Reserved2 : 1;
	} wMajorVersion;
	USHORT	wMinorVersion;
	USHORT	wReserved82[6];
	struct {
		USHORT	Mode0 : 1;
		USHORT	Mode1 : 1;
		USHORT	Mode2 : 1;
		USHORT	Mode3 : 1;
		USHORT	Mode4 : 1;
		USHORT	Mode5 : 1;
		USHORT	Mode6 : 1;
		USHORT	Mode7 : 1;
		USHORT	Mode0Sel : 1;
		USHORT	Mode1Sel : 1;
		USHORT	Mode2Sel : 1;
		USHORT	Mode3Sel : 1;
		USHORT	Mode4Sel : 1;
		USHORT	Mode5Sel : 1;
		USHORT	Mode6Sel : 1;
		USHORT	Mode7Sel : 1;
	} wUltraDMA;
	USHORT	wReserved89[167];
} IDINFO, * PIDINFO;

typedef struct _SYSTEM_MODULE   // Information Class 11
{
	ULONG_PTR Reserved[2];
	PVOID Base;
	ULONG Size;
	ULONG Flags;
	USHORT Index;
	USHORT Unknown;
	USHORT LoadCount;
	USHORT ModuleNameOffset;
	CHAR ImageName[256];
} SYSTEM_MODULE, * PSYSTEM_MODULE;

typedef struct _SYSTEM_MODULE_INFORMATION   // Information Class 11
{
	ULONG_PTR ulModuleCount;
	SYSTEM_MODULE Modules[1];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

struct piddbcache
{
	LIST_ENTRY		List;
	UNICODE_STRING	DriverName;
	ULONG			TimeDateStamp;
	NTSTATUS		LoadStatus;
	char			_0x0028[16]; // data from the shim engine, or uninitialized memory for custom drivers
};


typedef struct _PEB_LDR_DATA {
	ULONG Length;
	BOOLEAN Initialized;
	PVOID SsHandle;
	LIST_ENTRY ModuleListLoadOrder;
	LIST_ENTRY ModuleListMemoryOrder;
	LIST_ENTRY ModuleListInitOrder;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
	BYTE Reserved1[16];
	PVOID Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

typedef void(__stdcall* PPS_POST_PROCESS_INIT_ROUTINE)(void);

typedef struct _PEB {
	BYTE Reserved1[2];
	BYTE BeingDebugged;
	BYTE Reserved2[1];
	PVOID Reserved3[2];
	PPEB_LDR_DATA Ldr;
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
	PVOID Reserved4[3];
	PVOID AtlThunkSListPtr;
	PVOID Reserved5;
	ULONG Reserved6;
	PVOID Reserved7;
	ULONG Reserved8;
	ULONG AtlThunkSListPtr32;
	PVOID Reserved9[45];
	BYTE Reserved10[96];
	PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
	BYTE Reserved11[128];
	PVOID Reserved12[1];
	ULONG SessionId;
} PEB, * PPEB;

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;  // in bytes
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;  // LDR_*
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY HashLinks;
	PVOID SectionPointer;
	ULONG CheckSum;
	ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

extern "C"
{
	NTKERNELAPI PPEB PsGetProcessPeb(IN PEPROCESS Process);


	NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(
		IN HANDLE			ProcessId,
		OUT PEPROCESS* Process
	);

	NTKERNELAPI PVOID PsGetProcessSectionBaseAddress(
		IN PEPROCESS		Process
	);

	NTKERNELAPI NTSTATUS ZwQuerySystemInformation(
		SYSTEM_INFORMATION_CLASS SystemInformationClass,
		PVOID SystemInformation,
		ULONG SystemInformationLength,
		PULONG ReturnLength
	);

	NTSYSAPI ULONG RtlRandomEx(
		PULONG Seed
	);
}
