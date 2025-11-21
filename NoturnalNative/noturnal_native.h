#pragma once

extern "C" __declspec(dllexport) int __cdecl LoadKernelDriver(const char* driverName, void* statusOut);
extern "C" __declspec(dllexport) int __cdecl LoadUsermodeFromMemory(const unsigned char* payload, int size, void* statusOut);
extern "C" __declspec(dllexport) int __cdecl IsDriverReady();
extern "C" __declspec(dllexport) int __cdecl LoadKernelFromMemory(const unsigned char* payload, int size, void* statusOut);
typedef void (__cdecl *log_cb)(const char* msg);
extern "C" __declspec(dllexport) void __cdecl SetLogger(log_cb cb);
extern "C" __declspec(dllexport) int __cdecl RunExeFromMemory(const unsigned char* exe, int size, const wchar_t* hostPath);
extern "C" __declspec(dllexport) int __cdecl QueryDriverStatus();
extern "C" __declspec(dllexport) unsigned int __cdecl GetDriverVersion();