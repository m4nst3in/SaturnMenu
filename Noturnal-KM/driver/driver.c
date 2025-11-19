#include "driver.h"

// Debug print implementation
void debug_print(PCSTR text) {
#ifndef DEBUG
    UNREFERENCED_PARAMETER(text);
#endif // DEBUG
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, text));
}

// Kernel-mode safe wide char to ANSI conversion
NTSTATUS WideCharToAnsi(PWCHAR WideString, PCHAR AnsiString, ULONG AnsiStringLength)
{
    UNICODE_STRING unicodeString;
    ANSI_STRING ansiString;
    NTSTATUS status;

    RtlInitUnicodeString(&unicodeString, WideString);
    ansiString.Buffer = AnsiString;
    ansiString.Length = 0;
    ansiString.MaximumLength = (USHORT)AnsiStringLength;

    status = RtlUnicodeStringToAnsiString(&ansiString, &unicodeString, FALSE);
    if (NT_SUCCESS(status)) {
        // Null-terminate the string
        if (ansiString.Length < AnsiStringLength) {
            AnsiString[ansiString.Length] = '\0';
        }
    }
    return status;
}

// Get process ID by name
HANDLE GetPID(const wchar_t* process_name)
{
    ULONG length = 0;
    NTSTATUS status = ZwQuerySystemInformation(SystemProcessInformation, NULL, 0, &length);
    if (status != STATUS_INFO_LENGTH_MISMATCH)
        return NULL;

    // Use ExAllocatePoolWithTag for compatibility with older WDK versions
    PVOID buffer = ExAllocatePoolWithTag(NonPagedPool, length, POOL_TAG);
    if (!buffer)
        return NULL;

    status = ZwQuerySystemInformation(SystemProcessInformation, buffer, length, &length);
    if (!NT_SUCCESS(status)) {
        ExFreePoolWithTag(buffer, POOL_TAG);
        return NULL;
    }

    PSYSTEM_PROCESS_INFORMATION process_info = (PSYSTEM_PROCESS_INFORMATION)buffer;

    while (TRUE)
    {
        if (process_info->ImageName.Buffer && process_info->ImageName.Length > 0) {
            // Compare using RtlCompareUnicodeString (kernel-safe)
            UNICODE_STRING processNameUnicode;
            UNICODE_STRING targetNameUnicode;
            RtlInitUnicodeString(&targetNameUnicode, process_name);
            
            if (RtlCompareUnicodeString(&process_info->ImageName, &targetNameUnicode, TRUE) == 0) {
                HANDLE pid = process_info->UniqueProcessId;
                ExFreePoolWithTag(buffer, POOL_TAG);
                return pid;
            }
        }

        if (process_info->NextEntryOffset == 0)
            break;

        process_info = (PSYSTEM_PROCESS_INFORMATION)((PUCHAR)process_info + process_info->NextEntryOffset);
    }

    ExFreePoolWithTag(buffer, POOL_TAG);
    return NULL;
}

// Get module base address
PVOID GetProcessModuleBase(HANDLE processId, const char* moduleName)
{
    PEPROCESS targetProcess = NULL;
    NTSTATUS status = PsLookupProcessByProcessId(processId, &targetProcess);
    if (!NT_SUCCESS(status) || !targetProcess) {
        return NULL;
    }

    // Attach to the target process
    KAPC_STATE apcState;
    KeStackAttachProcess(targetProcess, &apcState);

    PVOID moduleBase = NULL;

    __try {
        // Get PEB
        PPEB peb = PsGetProcessPeb(targetProcess);
        if (!peb) {
            __leave;
        }

        // Get loader data
        PPEB_LDR_DATA ldrData = peb->Ldr;
        if (!ldrData) {
            __leave;
        }

        // Walk the module list
        PLIST_ENTRY listHead = &ldrData->InLoadOrderModuleList;
        PLIST_ENTRY listEntry = listHead->Flink;

        while (listEntry != listHead) {
            PLDR_DATA_TABLE_ENTRY ldrEntry = CONTAINING_RECORD(listEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

            if (ldrEntry->BaseDllName.Buffer && ldrEntry->BaseDllName.Length > 0) {
                // Convert Unicode to ANSI for comparison
                char ansiName[256] = { 0 };
                UNICODE_STRING unicodeName = ldrEntry->BaseDllName;
                ANSI_STRING ansiString;
                ANSI_STRING moduleNameAnsi;

                ansiString.Buffer = ansiName;
                ansiString.Length = 0;
                ansiString.MaximumLength = sizeof(ansiName);

                RtlInitAnsiString(&moduleNameAnsi, moduleName);

                if (NT_SUCCESS(RtlUnicodeStringToAnsiString(&ansiString, &unicodeName, FALSE))) {
                    if (RtlCompareString(&ansiString, &moduleNameAnsi, TRUE) == 0) {
                        moduleBase = ldrEntry->DllBase;
                        break;
                    }
                }
            }

            listEntry = listEntry->Flink;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        moduleBase = NULL;
    }

    KeUnstackDetachProcess(&apcState);
    ObDereferenceObject(targetProcess);

    return moduleBase;
}

// Handle attach request
NTSTATUS handle_attach(PIRP irp, SIZE_T* information) {
    Request* request = (Request*)irp->AssociatedIrp.SystemBuffer;
    if (!request) {
        *information = 0;
        return STATUS_INVALID_PARAMETER;
    }

    PEPROCESS temp_process = NULL;
    NTSTATUS status = PsLookupProcessByProcessId(request->process_id, &temp_process);

    if (NT_SUCCESS(status)) {
        ExAcquireFastMutex(&g_cache_mutex);
        if (cached_process && cached_pid != request->process_id) {
            ObDereferenceObject(cached_process);
            cached_process = NULL;
        }
        cached_process = temp_process;
        cached_pid = request->process_id;
        ExReleaseFastMutex(&g_cache_mutex);
    }

    *information = (NT_SUCCESS(status)) ? sizeof(Request) : 0;
    return status;
}

// Handle get PID request
NTSTATUS handle_get_pid(PIRP irp, SIZE_T* information) {
    P_PID_PACK pidPack = (P_PID_PACK)irp->AssociatedIrp.SystemBuffer;
    if (!pidPack || pidPack->name[0] == 0) {
        *information = 0;
        return STATUS_INVALID_PARAMETER;
    }

    HANDLE processId = GetPID(pidPack->name);
    pidPack->pid = HandleToULong(processId);

    NTSTATUS status = (processId != NULL) ? STATUS_SUCCESS : STATUS_NOT_FOUND;
    *information = sizeof(PID_PACK);
    return status;
}

// Handle get module request
NTSTATUS handle_get_module(PIRP irp, SIZE_T* information) {
    PMODULE_PACK modulePack = (PMODULE_PACK)irp->AssociatedIrp.SystemBuffer;
    if (!modulePack || modulePack->moduleName[0] == 0 || modulePack->pid == 0) {
        *information = 0;
        return STATUS_INVALID_PARAMETER;
    }
    HANDLE processId = ULongToHandle(modulePack->pid);
    SIZE_T imageSize = 0;
    PVOID baseAddr = GetProcessModuleInfo(processId, modulePack->moduleName, &imageSize);
    modulePack->baseAddress = (UINT64)baseAddr;
    modulePack->size = imageSize;
    NTSTATUS status = (baseAddr != NULL) ? STATUS_SUCCESS : STATUS_NOT_FOUND;
    *information = sizeof(MODULE_PACK);
    return status;
}

// Handle read request
NTSTATUS handle_read(PIRP irp, SIZE_T* information) {
    Request* request = (Request*)irp->AssociatedIrp.SystemBuffer;
    if (!request) {
        *information = 0;
        return STATUS_INVALID_PARAMETER;
    }

    PEPROCESS target_process;
    NTSTATUS status;

    // Fast path: use cached process (likely case)
    if (cached_process && cached_pid == request->process_id) {
        ExAcquireFastMutex(&g_cache_mutex);
        target_process = cached_process;
        ObReferenceObject(target_process);
        ExReleaseFastMutex(&g_cache_mutex);
    }
    else {
        // Slow path: lookup and cache
        status = PsLookupProcessByProcessId(request->process_id, &target_process);
        if (!NT_SUCCESS(status)) {
            *information = 0;
            return status;
        }

        ExAcquireFastMutex(&g_cache_mutex);
        if (cached_process) {
            ObDereferenceObject(cached_process);
        }
        cached_process = target_process;
        cached_pid = request->process_id;
        ObReferenceObject(target_process);
        ExReleaseFastMutex(&g_cache_mutex);
    }

    // Single validation block with early exit on most restrictive checks first
    ULONG_PTR target_addr = (ULONG_PTR)request->target;

    if (request->size > 0x1000 ||                              // Most likely to fail
        request->size == 0 ||
        !request->buffer ||
        target_addr < 0x10000 ||                               // Quick range checks
        target_addr >= 0x7FFFFFFFFFFF ||
        (target_addr + request->size) <= target_addr) {        // Overflow check last
        ObDereferenceObject(target_process);
        *information = 0;
        return STATUS_INVALID_PARAMETER;
    }

    // Direct memory copy with minimal overhead
    SIZE_T return_size;
    status = MmCopyVirtualMemory(target_process, request->target,
        PsGetCurrentProcess(), request->buffer,
        request->size, KernelMode, &return_size);

    ObDereferenceObject(target_process);
    *information = NT_SUCCESS(status) ? sizeof(Request) : 0;
    return status;
}

// Handle batch read request
NTSTATUS handle_batch_read(PIRP irp, PIO_STACK_LOCATION stack_irp, SIZE_T* information) {
    PBatchReadHeader header = (PBatchReadHeader)irp->AssociatedIrp.SystemBuffer;
    if (!header) {
        *information = 0;
        return STATUS_INVALID_PARAMETER;
    }

    // Fast validation with most restrictive first
    UINT32 num_requests = header->num_requests;
    if (num_requests > 0x100000 || num_requests == 0) {        // Most likely failures first
        *information = 0;
        return STATUS_INVALID_PARAMETER;
    }

    // Size validation (compiler will optimize this multiply)
    SIZE_T expected_size = sizeof(BatchReadHeader) + (num_requests * sizeof(BatchReadRequest));
    if (stack_irp->Parameters.DeviceIoControl.InputBufferLength < expected_size) {
        *information = 0;
        return STATUS_BUFFER_TOO_SMALL;
    }

    // Process lookup (same fast path as single read)
    PEPROCESS target_process;
    NTSTATUS status;

    if (cached_process && cached_pid == header->process_id) {
        ExAcquireFastMutex(&g_cache_mutex);
        target_process = cached_process;
        ObReferenceObject(target_process);
        ExReleaseFastMutex(&g_cache_mutex);
    }
    else {
        status = PsLookupProcessByProcessId(header->process_id, &target_process);
        if (!NT_SUCCESS(status)) {
            *information = 0;
            return status;
        }

        ExAcquireFastMutex(&g_cache_mutex);
        if (cached_process) {
            ObDereferenceObject(cached_process);
        }
        cached_process = target_process;
        cached_pid = header->process_id;
        ObReferenceObject(target_process);
        ExReleaseFastMutex(&g_cache_mutex);
    }

    // Pre-calculate pointers once
    PBatchReadRequest requests = (PBatchReadRequest)(header + 1);
    BYTE* output_buffer = (BYTE*)requests + (num_requests * sizeof(BatchReadRequest));
    SIZE_T total_buffer_size = header->total_buffer_size;

    UINT32 successful_reads = 0;

    // Main processing loop - optimized for common case (valid requests)
    for (UINT32 i = 0; i < num_requests; ++i) {
        ULONG_PTR addr = (ULONG_PTR)requests[i].address;
        SIZE_T size = requests[i].size;
        SIZE_T offset = requests[i].offset_in_buffer;

        if (size > 0x10000 || size == 0 || addr < 0x10000 || addr >= 0x7FFFFFFFFFFF ||
            (addr + size) <= addr || (offset + size) > total_buffer_size) {
            if ((offset + size) <= total_buffer_size) {
                RtlZeroMemory(output_buffer + offset, size);
            }
            continue;
        }

        SIZE_T run_size = size;
        UINT32 j = i + 1;
        while (j < num_requests) {
            ULONG_PTR next_addr = (ULONG_PTR)requests[j].address;
            SIZE_T next_size = requests[j].size;
            SIZE_T next_offset = requests[j].offset_in_buffer;
            if (next_size == 0 || next_addr != (addr + run_size) || next_offset != (offset + run_size)) break;
            if (next_addr >= 0x7FFFFFFFFFFF || (next_addr + next_size) <= next_addr || (next_offset + next_size) > total_buffer_size) break;
            run_size += next_size;
            ++j;
        }

        SIZE_T bytes_read;
        if (NT_SUCCESS(MmCopyVirtualMemory(target_process, (PVOID)addr,
            PsGetCurrentProcess(), output_buffer + offset,
            run_size, KernelMode, &bytes_read))) {
            ++successful_reads;
        } else {
            RtlZeroMemory(output_buffer + offset, run_size);
        }

        i = j - 1;
    }

    ObDereferenceObject(target_process);

    // Simple success determination
    status = successful_reads ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
    *information = NT_SUCCESS(status) ?
        stack_irp->Parameters.DeviceIoControl.InputBufferLength : 0;

    return status;
}

// Create/Close handler
NTSTATUS create(PDEVICE_OBJECT device_object, PIRP irp) {
    UNREFERENCED_PARAMETER(device_object);

    irp->IoStatus.Status = STATUS_SUCCESS;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

NTSTATUS close(PDEVICE_OBJECT device_object, PIRP irp) {
    UNREFERENCED_PARAMETER(device_object);

    irp->IoStatus.Status = STATUS_SUCCESS;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

// Device control handler
NTSTATUS device_control(PDEVICE_OBJECT device_object, PIRP irp) {
    UNREFERENCED_PARAMETER(device_object);

    debug_print("Device control called\n");

    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
    SIZE_T information = 0;

    PIO_STACK_LOCATION stack_irp = IoGetCurrentIrpStackLocation(irp);

    if (!stack_irp || !irp->AssociatedIrp.SystemBuffer) {
        irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
        irp->IoStatus.Information = 0;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        return STATUS_INVALID_PARAMETER;
    }

    switch (stack_irp->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_ATTACH:
        if (stack_irp->Parameters.DeviceIoControl.InputBufferLength < sizeof(Request)) {
            status = STATUS_BUFFER_TOO_SMALL;
            information = 0;
            break;
        }
        status = handle_attach(irp, &information);
        break;

    case IOCTL_GET_PID:
        if (stack_irp->Parameters.DeviceIoControl.InputBufferLength < sizeof(PID_PACK)) {
            status = STATUS_BUFFER_TOO_SMALL;
            information = 0;
            break;
        }
        status = handle_get_pid(irp, &information);
        break;

    case IOCTL_GET_MODULE_BASE:
        if (stack_irp->Parameters.DeviceIoControl.InputBufferLength < sizeof(MODULE_PACK)) {
            status = STATUS_BUFFER_TOO_SMALL;
            information = 0;
            break;
        }
        status = handle_get_module(irp, &information);
        break;

    case IOCTL_READ:
        if (stack_irp->Parameters.DeviceIoControl.InputBufferLength < sizeof(Request)) {
            status = STATUS_BUFFER_TOO_SMALL;
            information = 0;
            break;
        }
        status = handle_read(irp, &information);
        break;

    case IOCTL_BATCH_READ:
        status = handle_batch_read(irp, stack_irp, &information);
        break;

    case IOCTL_BATCH_READ_DIRECT:
        status = handle_batch_read_direct(device_object, irp, stack_irp, &information);
        break;

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        information = 0;
        break;
    }

    irp->IoStatus.Status = status;
    irp->IoStatus.Information = information;
    IoCompleteRequest(irp, IO_NO_INCREMENT);

    return status;
}

// Real entry point
NTSTATUS driver_main(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path) {
    UNREFERENCED_PARAMETER(registry_path);

    UNICODE_STRING device_name = {};
    RtlInitUnicodeString(&device_name, DEVICE_NAME);

    // Create driver device obj
    PDEVICE_OBJECT device_object = NULL;
    NTSTATUS status = IoCreateDevice(driver_object, 0, &device_name, FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN, FALSE, &device_object);

    if (status != STATUS_SUCCESS) {
        debug_print("Failed to create driver device.\n");
        return status;
    }

    debug_print("Driver device successfully created.\n");

    UNICODE_STRING symbolic_link = {};
    RtlInitUnicodeString(&symbolic_link, SYMBOLIC_LINK);

    status = IoCreateSymbolicLink(&symbolic_link, &device_name);
    if (status != STATUS_SUCCESS) {
        debug_print("Failed to establish symbolic link.\n");
        return status;
    }

    debug_print("Driver symbolic link successfully established.\n");

    // Flag for sending small amounts of data between um/km
    SetFlag(device_object->Flags, DO_BUFFERED_IO);
    SetFlag(device_object->Flags, DO_DIRECT_IO);

    driver_object->MajorFunction[IRP_MJ_CREATE] = create;
    driver_object->MajorFunction[IRP_MJ_CLOSE] = close;
    driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = device_control;
    driver_object->DriverUnload = unload;

    // Device has been initialized final step
    ClearFlag(device_object->Flags, DO_DEVICE_INITIALIZING);

    debug_print("Driver initialized successfully.\n");

    return status;
}

VOID unload(PDRIVER_OBJECT driver_object) {
    UNICODE_STRING symbolic_link = {};
    RtlInitUnicodeString(&symbolic_link, SYMBOLIC_LINK);
    IoDeleteSymbolicLink(&symbolic_link);

    if (driver_object && driver_object->DeviceObject) {
        IoDeleteDevice(driver_object->DeviceObject);
    }

    if (cached_process) {
        ObDereferenceObject(cached_process);
        cached_process = NULL;
        cached_pid = NULL;
    }
}

// kdmapper calls this entry point 
NTSTATUS DriverEntry() {
    debug_print("Hello, World from the Noturnal-KM kernel driver!\n");

    UNICODE_STRING driver_name = {};
    RtlInitUnicodeString(&driver_name, L"\\Driver\\Noturnal-KM");

    return IoCreateDriver(&driver_name, &driver_main);
}

PVOID GetProcessModuleInfo(HANDLE processId, const wchar_t* moduleName, SIZE_T* sizeOut)
{
    PEPROCESS targetProcess = NULL;
    NTSTATUS status = PsLookupProcessByProcessId(processId, &targetProcess);
    if (!NT_SUCCESS(status) || !targetProcess) {
        return NULL;
    }

    KAPC_STATE apcState;
    KeStackAttachProcess(targetProcess, &apcState);

    PVOID moduleBase = NULL;
    SIZE_T imageSize = 0;

    __try {
        PPEB peb = PsGetProcessPeb(targetProcess);
        if (!peb) {
            __leave;
        }
        PPEB_LDR_DATA ldrData = peb->Ldr;
        if (!ldrData) {
            __leave;
        }
        UNICODE_STRING targetNameUnicode;
        RtlInitUnicodeString(&targetNameUnicode, moduleName);

        PLIST_ENTRY listHead = &ldrData->InLoadOrderModuleList;
        PLIST_ENTRY listEntry = listHead->Flink;
        while (listEntry != listHead) {
            PLDR_DATA_TABLE_ENTRY ldrEntry = CONTAINING_RECORD(listEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
            if (ldrEntry->BaseDllName.Buffer && ldrEntry->BaseDllName.Length > 0) {
                if (RtlCompareUnicodeString(&ldrEntry->BaseDllName, &targetNameUnicode, TRUE) == 0) {
                    moduleBase = ldrEntry->DllBase;
                    imageSize = ldrEntry->SizeOfImage;
                    break;
                }
            }
            listEntry = listEntry->Flink;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        moduleBase = NULL;
        imageSize = 0;
    }

    KeUnstackDetachProcess(&apcState);
    ObDereferenceObject(targetProcess);

    if (sizeOut) *sizeOut = imageSize;
    return moduleBase;
}
NTSTATUS handle_batch_read_direct(PDEVICE_OBJECT device_object, PIRP irp, PIO_STACK_LOCATION stack_irp, SIZE_T* information) {
    UNREFERENCED_PARAMETER(device_object);
    PBatchReadHeader header = (PBatchReadHeader)irp->AssociatedIrp.SystemBuffer;
    if (!header) {
        *information = 0;
        return STATUS_INVALID_PARAMETER;
    }

    UINT32 num_requests = header->num_requests;
    if (num_requests > 0x100000 || num_requests == 0) {
        *information = 0;
        return STATUS_INVALID_PARAMETER;
    }

    SIZE_T request_struct_size = sizeof(BatchReadHeader) + (num_requests * sizeof(BatchReadRequest));
    if (stack_irp->Parameters.DeviceIoControl.InputBufferLength < request_struct_size) {
        *information = 0;
        return STATUS_BUFFER_TOO_SMALL;
    }

    if (!irp->MdlAddress) {
        *information = 0;
        return STATUS_INVALID_PARAMETER;
    }

    BYTE* output_buffer = (BYTE*)MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
    if (!output_buffer) {
        *information = 0;
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    SIZE_T total_buffer_size = header->total_buffer_size;
    if (stack_irp->Parameters.DeviceIoControl.OutputBufferLength < total_buffer_size) {
        *information = 0;
        return STATUS_BUFFER_TOO_SMALL;
    }

    PEPROCESS target_process;
    NTSTATUS status;
    if (cached_process && cached_pid == header->process_id) {
        ExAcquireFastMutex(&g_cache_mutex);
        target_process = cached_process;
        ObReferenceObject(target_process);
        ExReleaseFastMutex(&g_cache_mutex);
    } else {
        status = PsLookupProcessByProcessId(header->process_id, &target_process);
        if (!NT_SUCCESS(status)) {
            *information = 0;
            return status;
        }
        ExAcquireFastMutex(&g_cache_mutex);
        if (cached_process) {
            ObDereferenceObject(cached_process);
        }
        cached_process = target_process;
        cached_pid = header->process_id;
        ObReferenceObject(target_process);
        ExReleaseFastMutex(&g_cache_mutex);
    }

    PBatchReadRequest requests = (PBatchReadRequest)(header + 1);
    UINT32 successful_reads = 0;

    for (UINT32 i = 0; i < num_requests; ++i) {
        ULONG_PTR addr = (ULONG_PTR)requests[i].address;
        SIZE_T size = requests[i].size;
        SIZE_T offset = requests[i].offset_in_buffer;

        if (size > 0x20000 || size == 0 || addr < 0x10000 || addr >= 0x7FFFFFFFFFFF ||
            (addr + size) <= addr || (offset + size) > total_buffer_size) {
            if ((offset + size) <= total_buffer_size) {
                RtlZeroMemory(output_buffer + offset, size);
            }
            continue;
        }

        SIZE_T run_size = size;
        UINT32 j = i + 1;
        while (j < num_requests) {
            ULONG_PTR next_addr = (ULONG_PTR)requests[j].address;
            SIZE_T next_size = requests[j].size;
            SIZE_T next_offset = requests[j].offset_in_buffer;
            if (next_size == 0 || next_addr != (addr + run_size) || next_offset != (offset + run_size)) break;
            if (next_addr >= 0x7FFFFFFFFFFF || (next_addr + next_size) <= next_addr || (next_offset + next_size) > total_buffer_size) break;
            run_size += next_size;
            ++j;
        }

        SIZE_T bytes_read;
        if (NT_SUCCESS(MmCopyVirtualMemory(target_process, (PVOID)addr,
            PsGetCurrentProcess(), output_buffer + offset,
            run_size, KernelMode, &bytes_read))) {
            ++successful_reads;
        } else {
            RtlZeroMemory(output_buffer + offset, run_size);
        }

        i = j - 1;
    }

    ObDereferenceObject(target_process);
    status = successful_reads ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
    *information = NT_SUCCESS(status) ? stack_irp->Parameters.DeviceIoControl.OutputBufferLength : 0;
    return status;
}
    ExInitializeFastMutex(&g_cache_mutex);
