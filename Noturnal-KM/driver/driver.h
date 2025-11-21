#pragma once

#if defined(_M_AMD64)
#ifndef _AMD64_
#define _AMD64_
#endif
#elif defined(_M_IX86)
#ifndef _X86_
#define _X86_
#endif
#elif defined(_M_ARM64)
#ifndef _ARM64_
#define _ARM64_
#endif
#elif defined(_M_ARM)
#ifndef _ARM_
#define _ARM_
#endif
#endif

#pragma once

#include <ntifs.h>
#include <ntddk.h>
#include <ntintsafe.h>

typedef unsigned char BYTE;

typedef struct _PEB PEB, *PPEB;

// IOCTL definitions (must match MemoryMgr.h)
#define DRAGON_DEVICE 0x8000
#define IOCTL_ATTACH CTL_CODE(DRAGON_DEVICE, 0x4452, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_READ CTL_CODE(DRAGON_DEVICE, 0x4453, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_GET_MODULE_BASE CTL_CODE(DRAGON_DEVICE, 0x4454, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_GET_PID CTL_CODE(DRAGON_DEVICE, 0x4455, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_BATCH_READ CTL_CODE(DRAGON_DEVICE, 0x4456, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_BATCH_READ_DIRECT CTL_CODE(DRAGON_DEVICE, 0x4457, METHOD_OUT_DIRECT, FILE_SPECIAL_ACCESS)
#define IOCTL_QUERY_STATUS CTL_CODE(DRAGON_DEVICE, 0x4458, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_GET_VERSION CTL_CODE(DRAGON_DEVICE, 0x4459, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

// Device names
#define DEVICE_NAME L"\\Device\\Noturnal-KM"
#define SYMBOLIC_LINK L"\\DosDevices\\Noturnal-KM"
#define USER_DEVICE_NAME L"\\\\.\\Noturnal-KM"

#define POOL_TAG 'oamL'

// Process cache for optimization
static PEPROCESS cached_process = NULL;
static HANDLE cached_pid = NULL;
static FAST_MUTEX g_cache_mutex;

#ifdef __cplusplus
extern "C" {
#endif
    NTKERNELAPI NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName,
        PDRIVER_INITIALIZE InitializationFunction);

    NTKERNELAPI NTSTATUS MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress,
        PEPROCESS TargetProcess, PVOID TargetAddress,
        SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode,
        PSIZE_T ReturnSize);

    NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation(ULONG InfoClass, PVOID Buffer, ULONG Length, PULONG ReturnLength);

    NTKERNELAPI PPEB NTAPI PsGetProcessPeb(PEPROCESS Process);

#ifdef __cplusplus
}
#endif

// Debug print macro
void debug_print(PCSTR text);

// Structure for reading memory from a process (matches usermode MemoryMgr.h)
typedef struct _Request
{
    HANDLE process_id;
    PVOID target;
    PVOID buffer;
    SIZE_T size;
} Request, * PRequest;

// Structure for getting pid by name (matches usermode MemoryMgr.h)
typedef struct _PID_PACK
{
    UINT32 pid;
    WCHAR name[1024];
} PID_PACK, * P_PID_PACK;

// Structure for getting module address base (matches usermode MemoryMgr.h)
typedef struct _MODULE_PACK {
    UINT32 pid;
    UINT64 baseAddress;
    SIZE_T size;
    WCHAR moduleName[1024];
} MODULE_PACK, * PMODULE_PACK;

// Batch read structures (matches usermode MemoryMgr.h)
typedef struct _BatchReadRequest {
    DWORD64 address;
    SIZE_T size;
    SIZE_T offset_in_buffer;
} BatchReadRequest, * PBatchReadRequest;

typedef struct _BatchReadHeader {
    HANDLE process_id;
    UINT32 num_requests;
    SIZE_T total_buffer_size;
} BatchReadHeader, * PBatchReadHeader;

// System information structures
typedef struct _SYSTEM_MODULE {
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

typedef struct _SYSTEM_MODULE_INFORMATION {
    ULONG_PTR ulModuleCount;
    SYSTEM_MODULE Modules[1];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS
{
    SystemProcessInformation = 5,
    SystemModuleInformation = 11,
} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_THREAD_INFORMATION {
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
} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFORMATION {
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
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

typedef struct _PEB_LDR_DATA {
    ULONG Length;
    UCHAR Initialized;
    PVOID SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    union {
        LIST_ENTRY HashLinks;
        struct {
            PVOID SectionPointer;
            ULONG CheckSum;
        }Info;
    };
    union {
        ULONG TimeDateStamp;
        PVOID LoadedImports;
    };
    PVOID EntryPointActivationContext;
    PVOID PatchInformation;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef struct _PEB {
    UCHAR InheritedAddressSpace;
    UCHAR ReadImageFileExecOptions;
    UCHAR BeingDebugged;
    UCHAR BitField;
    PVOID Mutant;
    PVOID ImageBaseAddress;
    PPEB_LDR_DATA Ldr;
} PEB, * PPEB;

// Function declarations
NTSTATUS driver_main(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path);
NTSTATUS DriverEntry();
NTSTATUS create(PDEVICE_OBJECT device_object, PIRP irp);
NTSTATUS close(PDEVICE_OBJECT device_object, PIRP irp);
NTSTATUS device_control(PDEVICE_OBJECT device_object, PIRP irp);
NTSTATUS handle_attach(PIRP irp, SIZE_T* information);
NTSTATUS handle_get_pid(PIRP irp, SIZE_T* information);
NTSTATUS handle_get_module(PIRP irp, SIZE_T* information);
NTSTATUS handle_read(PIRP irp, SIZE_T* information);
NTSTATUS handle_batch_read(PIRP irp, PIO_STACK_LOCATION stack_irp, SIZE_T* information);
NTSTATUS handle_batch_read_direct(PDEVICE_OBJECT device_object, PIRP irp, PIO_STACK_LOCATION stack_irp, SIZE_T* information);
HANDLE GetPID(const wchar_t* process_name);
PVOID GetProcessModuleBase(HANDLE processId, const char* moduleName);
NTSTATUS WideCharToAnsi(PWCHAR WideString, PCHAR AnsiString, ULONG AnsiStringLength);
VOID unload(PDRIVER_OBJECT driver_object);
PVOID GetProcessModuleInfo(HANDLE processId, const wchar_t* moduleName, SIZE_T* sizeOut);
