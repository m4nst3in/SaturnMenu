using System;
using System.IO;
using System.Runtime.InteropServices;

namespace Noturnal.Loader.Services
{
    public static class NativeLoader
    {
        private static bool _loaded;
        public static bool IsLoaded => _loaded;
        public static void Initialize()
        {
            if (_loaded) return;
            var baseDir = AppDomain.CurrentDomain.BaseDirectory;
            var dllName = "NoturnalNative.dll";
            var nativeDir = Path.Combine(baseDir, "Native");
            var candidates = new[]
            {
                Path.Combine(baseDir, dllName),
                Path.Combine(nativeDir, dllName)
            };
            foreach (var p in candidates)
            {
                if (File.Exists(p) && TryLoad(p)) { _loaded = true; return; }
            }
        }
        private static bool TryLoad(string path)
        {
            try { return NativeLibrary.TryLoad(path, out _); }
            catch { return false; }
        }
    }
}
