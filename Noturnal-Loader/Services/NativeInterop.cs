using System.Runtime.InteropServices;

namespace Noturnal.Loader.Services
{
    public static class NativeInterop
    {
        [DllImport("NoturnalNative.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int LoadKernelDriver(string driverName, System.IntPtr statusOut);
        [DllImport("NoturnalNative.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int LoadUsermodeFromMemory(byte[] payload, int size, System.IntPtr statusOut);
        [DllImport("NoturnalNative.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int IsDriverReady();
        [DllImport("NoturnalNative.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int LoadKernelFromMemory(byte[] payload, int size, System.IntPtr statusOut);
        [DllImport("NoturnalNative.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern int RunExeFromMemory(byte[] exe, int size, string hostPath);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void LogCallback(string msg);
        [DllImport("NoturnalNative.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetLogger(LogCallback cb);
    }
}
