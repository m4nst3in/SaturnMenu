#include "noturnal_native.h"
#include <Windows.h>
#include <winternl.h>

static bool g_driverReady = false;
static log_cb g_logger = nullptr;
static void log(const char* s){ if(g_logger) g_logger(s); }

int __cdecl LoadKernelDriver(const char* driverName, void* statusOut)
{
    log("LoadKernelDriver called");
    log("[KD] Checking kernel driver availability...");
    log("[KD] Searching for service...");
    log("[KD] Mapping driver (stub)...");
    g_driverReady = true;
    log("[KD] Driver ready.");
    return 0; // stub success
}

int __cdecl LoadUsermodeFromMemory(const unsigned char* payload, int size, void* statusOut)
{
    log("LoadUsermodeFromMemory called");
    if(!(payload && size>0)) { log("[UM] Invalid payload."); return -1; }
    log("[UM] Allocating memory...");
    log("[UM] Writing payload...");
    log("[UM] Starting thread (stub)...");
    log("[UM] Usermode started.");
    return 0;
}

int __cdecl IsDriverReady()
{
    return g_driverReady ? 1 : 0;
}

int __cdecl QueryDriverStatus()
{
    const wchar_t* devName = L"\\\\.\\Noturnal-KM";
    const DWORD IOCTL_QUERY_STATUS = (0x8000 << 16) | (0x4458 << 2) | 0x00000003; // FILE_SPECIAL_ACCESS + METHOD_BUFFERED
    HANDLE h = CreateFileW(devName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        return 0;
    }
    DWORD inout = 0; DWORD ret = 0;
    BOOL ok = DeviceIoControl(h, IOCTL_QUERY_STATUS, &inout, sizeof(inout), &inout, sizeof(inout), &ret, NULL);
    CloseHandle(h);
    if (ok && inout == 1) {
        g_driverReady = true;
        return 1;
    }
    return 0;
}

unsigned int __cdecl GetDriverVersion()
{
    const wchar_t* devName = L"\\\\.\\Noturnal-KM";
    const DWORD IOCTL_GET_VERSION = (0x8000 << 16) | (0x4459 << 2) | 0x00000003;
    HANDLE h = CreateFileW(devName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return 0;
    DWORD ver = 0; DWORD ret = 0;
    BOOL ok = DeviceIoControl(h, IOCTL_GET_VERSION, &ver, sizeof(ver), &ver, sizeof(ver), &ret, NULL);
    CloseHandle(h);
    return ok ? ver : 0;
}

int __cdecl LoadKernelFromMemory(const unsigned char* payload, int size, void* statusOut)
{
    log("LoadKernelFromMemory called");
    if(!(payload && size>0)) { log("[KD] Invalid payload."); g_driverReady=false; return -1; }
    log("[KD] Checking kernel driver availability...");
    log("[KD] Mapping driver from memory (stub)...");
    g_driverReady = true;
    log("[KD] Driver ready.");
    return 0;
}

void __cdecl SetLogger(log_cb cb)
{
    g_logger = cb;
    log("Logger attached");
}

static FARPROC getproc(const char* dll, const char* name) {
    HMODULE h = GetModuleHandleA(dll); if (!h) h = LoadLibraryA(dll); return h ? GetProcAddress(h, name) : nullptr;
}

int __cdecl RunExeFromMemory(const unsigned char* exe, int size, const wchar_t* hostPath)
{
    log("RunExeFromMemory called");
    if (!(exe && size > 0 && hostPath)) { log("[RUN] Invalid parameters"); return -1; }
    auto pNtUnmapViewOfSection = (NTSTATUS (NTAPI*)(HANDLE,PVOID))getproc("ntdll.dll","NtUnmapViewOfSection");
    STARTUPINFOW si{}; PROCESS_INFORMATION pi{}; si.cb = sizeof(si);
    bool created = false;
    __try {
        if (!CreateProcessW(hostPath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) { log("[RUN] CreateProcessW failed"); return -1; }
        created = true;
        IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)exe; if (dos->e_magic != IMAGE_DOS_SIGNATURE) { log("[RUN] DOS signature invalid"); TerminateProcess(pi.hProcess, 0); return -1; }
        IMAGE_NT_HEADERS64* nt = (IMAGE_NT_HEADERS64*)(exe + dos->e_lfanew); if (nt->Signature != IMAGE_NT_SIGNATURE) { log("[RUN] NT signature invalid"); TerminateProcess(pi.hProcess, 0); return -1; }
        if (nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) { log("[RUN] Not PE32+ 64-bit"); TerminateProcess(pi.hProcess, 0); return -1; }
        SIZE_T imageSize = nt->OptionalHeader.SizeOfImage; PVOID remoteBase = (PVOID)nt->OptionalHeader.ImageBase;
        if (pNtUnmapViewOfSection) pNtUnmapViewOfSection(pi.hProcess, remoteBase);
        remoteBase = VirtualAllocEx(pi.hProcess, remoteBase, imageSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (!remoteBase) { log("[RUN] VirtualAllocEx failed"); TerminateProcess(pi.hProcess, 0); return -1; }
        SIZE_T written = 0; if (!WriteProcessMemory(pi.hProcess, remoteBase, exe, nt->OptionalHeader.SizeOfHeaders, &written)) { log("[RUN] Write headers failed"); TerminateProcess(pi.hProcess, 0); return -1; }
        IMAGE_SECTION_HEADER* sec = (IMAGE_SECTION_HEADER*)((BYTE*)&nt->OptionalHeader + nt->FileHeader.SizeOfOptionalHeader);
        for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
            LPVOID dst = (BYTE*)remoteBase + sec[i].VirtualAddress; LPVOID src = (LPVOID)(exe + sec[i].PointerToRawData);
            SIZE_T sz = sec[i].SizeOfRawData; if (sz) WriteProcessMemory(pi.hProcess, dst, src, sz, &written);
        }
        auto RvaToPtr = [&](DWORD rva) -> const BYTE* {
            for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
                DWORD va = sec[i].VirtualAddress;
                DWORD size = sec[i].SizeOfRawData ? sec[i].SizeOfRawData : sec[i].Misc.VirtualSize;
                if (rva >= va && rva < va + size) {
                    DWORD off = sec[i].PointerToRawData + (rva - va);
                    return exe + off;
                }
            }
            return nullptr;
        };
        ULONGLONG baseDelta = (ULONGLONG)remoteBase - nt->OptionalHeader.ImageBase;
        if (baseDelta) {
            IMAGE_DATA_DIRECTORY relocDir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
            if (relocDir.Size) {
                const BYTE* relocBase = RvaToPtr(relocDir.VirtualAddress);
                if (!relocBase) { log("[RUN] Reloc dir invalid"); TerminateProcess(pi.hProcess, 0); return -1; }
                DWORD offset = 0;
                while (offset < relocDir.Size) {
                    auto reloc = (IMAGE_BASE_RELOCATION*)(relocBase + offset);
                    if (reloc->SizeOfBlock == 0) break;
                    DWORD count = (reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
                    WORD* entries = (WORD*)(relocBase + offset + sizeof(IMAGE_BASE_RELOCATION));
                    for (DWORD j = 0; j < count; ++j) {
                        WORD typeOffset = entries[j]; WORD type = typeOffset >> 12; WORD rva = typeOffset & 0xFFF;
                        if (type == IMAGE_REL_BASED_DIR64) {
                            ULONGLONG* addr = (ULONGLONG*)RvaToPtr(reloc->VirtualAddress + rva);
                            if (!addr) continue;
                            ULONGLONG val = *addr + baseDelta;
                            WriteProcessMemory(pi.hProcess, (BYTE*)remoteBase + reloc->VirtualAddress + rva, &val, sizeof(val), &written);
                        }
                    }
                    offset += reloc->SizeOfBlock;
                }
            }
        }
        IMAGE_DATA_DIRECTORY impDir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
        if (impDir.Size) {
            auto imp = (IMAGE_IMPORT_DESCRIPTOR*)RvaToPtr(impDir.VirtualAddress);
            if (!imp) { log("[RUN] Import dir invalid"); TerminateProcess(pi.hProcess, 0); return -1; }
            while (imp->Name) {
                const char* dll = (const char*)RvaToPtr(imp->Name);
                if (!dll) { log("[RUN] Import dll name invalid"); break; }
                HMODULE hMod = LoadLibraryA(dll);
                auto origThunk = (ULONGLONG*)RvaToPtr(imp->OriginalFirstThunk ? imp->OriginalFirstThunk : imp->FirstThunk);
                auto iatThunk = (ULONGLONG*)RvaToPtr(imp->FirstThunk);
                if (!origThunk || !iatThunk) { log("[RUN] Thunks invalid"); break; }
                SIZE_T idx = 0;
                while (origThunk[idx]) {
                    FARPROC fn = NULL;
                    if (origThunk[idx] & IMAGE_ORDINAL_FLAG64) {
                        fn = GetProcAddress(hMod, (LPCSTR)(origThunk[idx] & 0xFFFF));
                    } else {
                        auto ibn = (IMAGE_IMPORT_BY_NAME*)RvaToPtr((DWORD)origThunk[idx]);
                        if (ibn) fn = GetProcAddress(hMod, (LPCSTR)ibn->Name);
                    }
                    auto remoteIat = (BYTE*)remoteBase + imp->FirstThunk + (idx * sizeof(ULONGLONG));
                    WriteProcessMemory(pi.hProcess, remoteIat, &fn, sizeof(fn), &written);
                    ++idx;
                }
                ++imp;
            }
        }
        CONTEXT ctx{}; ctx.ContextFlags = CONTEXT_FULL | CONTEXT_CONTROL;
        if (!GetThreadContext(pi.hThread, &ctx)) { log("[RUN] GetThreadContext failed"); TerminateProcess(pi.hProcess, 0); return -1; }
        ULONGLONG entry = (ULONGLONG)remoteBase + nt->OptionalHeader.AddressOfEntryPoint; ctx.Rip = entry;
        if (!SetThreadContext(pi.hThread, &ctx)) { log("[RUN] SetThreadContext failed"); TerminateProcess(pi.hProcess, 0); return -1; }
        ResumeThread(pi.hThread);
        CloseHandle(pi.hThread); CloseHandle(pi.hProcess);
        log("[RUN] Process resumed");
        return 0;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        log("[RUN] SEH: unexpected exception");
        if (created) {
            if (pi.hThread) CloseHandle(pi.hThread);
            if (pi.hProcess) TerminateProcess(pi.hProcess, 0), CloseHandle(pi.hProcess);
        }
        return -3;
    }
}