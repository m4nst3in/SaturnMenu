using System;
using System.IO;
using System.Security.Cryptography;

namespace Noturnal.Loader.Services
{
    public static class CredentialStore
    {
        private static readonly string Dir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "NoturnalLoader");
        private static readonly string PathFile = Path.Combine(Dir, "creds.dat");
        private static readonly byte[] Entropy = new byte[] { 0x13, 0x71, 0xF7, 0x5A, 0x29, 0xC0, 0xFF, 0x42 };
        public static void Save(string username, string password)
        {
            Directory.CreateDirectory(Dir);
            var plain = System.Text.Encoding.UTF8.GetBytes(username + "\n" + password);
            var protectedBytes = ProtectedData.Protect(plain, Entropy, DataProtectionScope.CurrentUser);
            File.WriteAllBytes(PathFile, protectedBytes);
        }
        public static bool TryGet(out string username, out string password)
        {
            username = ""; password = "";
            if (!File.Exists(PathFile)) return false;
            try
            {
                var bytes = File.ReadAllBytes(PathFile);
                var un = ProtectedData.Unprotect(bytes, Entropy, DataProtectionScope.CurrentUser);
                var s = System.Text.Encoding.UTF8.GetString(un);
                var parts = s.Split('\n');
                if (parts.Length >= 2) { username = parts[0]; password = parts[1]; return true; }
            }
            catch { }
            return false;
        }
        public static void Clear()
        {
            try { if (File.Exists(PathFile)) File.Delete(PathFile); } catch { }
        }
        public static bool Exists() => File.Exists(PathFile);
    }
}