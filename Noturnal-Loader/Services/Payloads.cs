using System;
using System.IO;
using System.Reflection;

namespace Noturnal.Loader.Services
{
    public static class Payloads
    {
        public static byte[] GetKernelPayload()
        {
            return ReadResource("Assets/Payloads/km.bin");
        }
        public static byte[] GetUsermodePayload()
        {
            return ReadResource("Assets/Payloads/um.bin");
        }
        private static byte[] ReadResource(string relativePath)
        {
            var asm = Assembly.GetExecutingAssembly();
            var resourcePath = relativePath.Replace("\\", "/");
            // Try WPF pack URI first
            try
            {
                var packUri = new Uri("pack://application:,,,/" + resourcePath, UriKind.Absolute);
                var rs = System.Windows.Application.GetResourceStream(packUri);
                if (rs != null && rs.Stream != null)
                {
                    using var ms = new MemoryStream();
                    rs.Stream.CopyTo(ms);
                    return ms.ToArray();
                }
            }
            catch { }
            // Fallback: embedded resource stream (if marked as EmbeddedResource)
            try
            {
                foreach (var name in asm.GetManifestResourceNames())
                {
                    if (name.EndsWith(Path.GetFileName(resourcePath), StringComparison.OrdinalIgnoreCase))
                    {
                        using var s = asm.GetManifestResourceStream(name);
                        if (s == null) break;
                        using var ms = new MemoryStream();
                        s.CopyTo(ms);
                        return ms.ToArray();
                    }
                }
            }
            catch { }
            return Array.Empty<byte>();
        }
    }
}
