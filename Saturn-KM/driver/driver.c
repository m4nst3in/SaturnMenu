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
        // Clear old cache if different PID
        if (cached_process && cached_pid != request->process_id) {
            ObDereferenceObject(cached_process);
            cached_process = NULL;
        }

        // Cache the new process
        cached_process = temp_process;
        cached_pid = request->process_id;
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

    // Convert wide char to ANSI
    char ansiModuleName[1024] = { 0 };
    NTSTATUS status = WideCharToAnsi(modulePack->moduleName, ansiModuleName, sizeof(ansiModuleName));

    if (NT_SUCCESS(status)) {
        HANDLE processId = ULongToHandle(modulePack->pid);
        // Get process module base address
        PVOID baseAddr = GetProcessModuleBase(processId, ansiModuleName);
        modulePack->baseAddress = (UINT64)baseAddr;
        status = (baseAddr != NULL) ? STATUS_SUCCESS : STATUS_NOT_FOUND;
        *information = sizeof(MODULE_PACK);
        return status;
    }
    else {
        *information = 0;
        return status;
    }
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
        target_process = cached_process;
        ObReferenceObject(target_process);
    }
    else {
        // Slow path: lookup and cache
        status = PsLookupProcessByProcessId(request->process_id, &target_process);
        if (!NT_SUCCESS(status)) {
            *information = 0;
            return status;
        }

        // Update cache atomically
        if (cached_process) {
            ObDereferenceObject(cached_process);
        }

        cached_process = target_process;
        cached_pid = request->process_id;
        ObReferenceObject(target_process);
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
        target_process = cached_process;
        ObReferenceObject(target_process);
    }
    else {
        status = PsLookupProcessByProcessId(header->process_id, &target_process);
        if (!NT_SUCCESS(status)) {
            *information = 0;
            return status;
        }

        if (cached_process) {
            ObDereferenceObject(cached_process);
        }

        cached_process = target_process;
        cached_pid = header->process_id;
        ObReferenceObject(target_process);
    }

    // Pre-calculate pointers once
    PBatchReadRequest requests = (PBatchReadRequest)(header + 1);
    BYTE* output_buffer = (BYTE*)requests + (num_requests * sizeof(BatchReadRequest));
    SIZE_T total_buffer_size = header->total_buffer_size;

    UINT32 successful_reads = 0;

    // Main processing loop - optimized for common case (valid requests)
    for (UINT32 i = 0; i < num_requests; ++i) {
        PBatchReadRequest req = &requests[i];

        // Fast validation path - order by likelihood of failure
        ULONG_PTR addr = (ULONG_PTR)req->address;
        SIZE_T size = req->size;
        SIZE_T offset = req->offset_in_buffer;

        if (size > 0x10000 ||                                  // Size check first (most restrictive)
            size == 0 ||
            addr < 0x10000 ||                                  // Address range checks
            addr >= 0x7FFFFFFFFFFF ||
            (addr + size) <= addr ||                           // Overflow
            (offset + size) > total_buffer_size) {             // Buffer bounds
            // Zero invalid reads (only if bounds allow)
            if ((offset + size) <= total_buffer_size) {
                RtlZeroMemory(output_buffer + offset, size);
            }
            continue;
        }

        // Optimized memory copy - no intermediate variables
        SIZE_T bytes_read;
        if (NT_SUCCESS(MmCopyVirtualMemory(target_process, (PVOID)addr,
            PsGetCurrentProcess(), output_buffer + offset,
            size, KernelMode, &bytes_read))) {
            ++successful_reads;
        }
        else {
            // Zero failed reads
            RtlZeroMemory(output_buffer + offset, size);
        }
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
        status = handle_attach(irp, &information);
        break;

    case IOCTL_GET_PID:
        status = handle_get_pid(irp, &information);
        break;

    case IOCTL_GET_MODULE_BASE:
        status = handle_get_module(irp, &information);
        break;

    case IOCTL_READ:
        status = handle_read(irp, &information);
        break;

    case IOCTL_BATCH_READ:
        status = handle_batch_read(irp, stack_irp, &information);
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

    driver_object->MajorFunction[IRP_MJ_CREATE] = create;
    driver_object->MajorFunction[IRP_MJ_CLOSE] = close;
    driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = device_control;

    // Device has been initialized final step
    ClearFlag(device_object->Flags, DO_DEVICE_INITIALIZING);

    debug_print("Driver initialized successfully.\n");

    return status;
}

// kdmapper calls this entry point 
NTSTATUS DriverEntry() {
    debug_print("Hello, World from the Saturn-KM kernel driver!\n");

    UNICODE_STRING driver_name = {};
    RtlInitUnicodeString(&driver_name, L"\\Driver\\Saturn-KM");

    return IoCreateDriver(&driver_name, &driver_main);
}
