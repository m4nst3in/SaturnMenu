using System;
using System.Diagnostics;
using System.IO;
using System.Net.Http;
using System.Threading.Tasks;

namespace Noturnal.Loader.Services
{
    public class DownloadService
    {
        public static DownloadService Instance { get; } = new DownloadService();
        private readonly HttpClient _http = new HttpClient();
        private const string BaseUrl = "http://localhost:4000";

        public async Task<string> DownloadKernelAsync(string destinationDirectory)
        {
            Directory.CreateDirectory(destinationDirectory);
            var destPath = Path.Combine(destinationDirectory, "Noturnal-km.exe");
            using var stream = await _http.GetStreamAsync(BaseUrl + "/api/downloads/km");
            using var file = File.Create(destPath);
            await stream.CopyToAsync(file);
            return destPath;
        }

        public async Task<string> DownloadUsermodeAsync(string destinationDirectory)
        {
            Directory.CreateDirectory(destinationDirectory);
            var destPath = Path.Combine(destinationDirectory, "Noturnal-usermode.exe");
            using var stream = await _http.GetStreamAsync(BaseUrl + "/api/downloads/usermode");
            using var file = File.Create(destPath);
            await stream.CopyToAsync(file);
            return destPath;
        }

        public static async Task<bool> DownloadAndExecute(string url)
        {
            try
            {
                var dir = Path.Combine(Path.GetTempPath(), "Noturnal", "bin");
                Directory.CreateDirectory(dir);

                using var handler = new HttpClientHandler();
                handler.ServerCertificateCustomValidationCallback = (req, cert, chain, errors) => {
                    try {
                        if (cert == null) return false;
                        var pub = cert.GetPublicKey();
                        using var sha = System.Security.Cryptography.SHA256.Create();
                        var hash = System.BitConverter.ToString(sha.ComputeHash(pub)).Replace("-", "").ToLowerInvariant();
                        if (!string.IsNullOrEmpty(SecurityConfig.PinnedPublicKeyHash))
                            return hash == SecurityConfig.PinnedPublicKeyHash;
                        return true;
                    } catch { return false; }
                };
                using var client = new HttpClient(handler);
                var resp = await client.GetAsync(url, HttpCompletionOption.ResponseHeadersRead);
                resp.EnsureSuccessStatusCode();

                LogService.Instance.Add($"[Downloader] Requesting: {url}");
                var fileName = "download.exe";
                var cd = resp.Content.Headers.ContentDisposition;
                if (cd?.FileNameStar != null)
                    fileName = cd.FileNameStar;
                else if (cd?.FileName != null)
                    fileName = cd.FileName.Trim('"');
                else
                {
                    var name = Path.GetFileName(new Uri(url).LocalPath);
                    if (string.IsNullOrWhiteSpace(name)) name = fileName;
                    if (!Path.HasExtension(name)) name += ".exe";
                    fileName = name;
                }
                if (!Path.HasExtension(fileName)) fileName += ".exe";

                var dest = Path.Combine(dir, fileName);
                var tmp = dest + ".part";
                LogService.Instance.Add($"[Downloader] Saving to: {tmp}");
                var contentType = resp.Content.Headers.ContentType?.MediaType ?? "unknown";
                var contentLength = resp.Content.Headers.ContentLength?.ToString() ?? "unknown";
                LogService.Instance.Add($"[Downloader] Content-Type: {contentType}, Length: {contentLength}");
                using (var stream = await resp.Content.ReadAsStreamAsync())
                using (var file = File.Create(tmp))
                {
                    await stream.CopyToAsync(file);
                }
                LogService.Instance.Add("[Downloader] Download completed");
                try
                {
                    if (File.Exists(dest)) File.Delete(dest);
                }
                catch (Exception ex)
                {
                    LogService.Instance.Add($"[Downloader] Failed to delete existing: {ex.Message}");
                }
                try
                {
                    File.Move(tmp, dest);
                    LogService.Instance.Add($"[Downloader] Moved into place: {dest}");
                }
                catch (Exception ex)
                {
                    LogService.Instance.Add($"[Downloader] Move failed: {ex.Message}");
                    dest = Path.Combine(dir, Path.GetFileNameWithoutExtension(fileName) + "-copy.exe");
                    File.Copy(tmp, dest, true);
                    try { File.Delete(tmp); } catch {}
                    LogService.Instance.Add($"[Downloader] Copied into place: {dest}");
                }
                var unlocked = false;
                for (int i = 0; i < 10; i++)
                {
                    try
                    {
                        using var probe = new FileStream(dest, FileMode.Open, FileAccess.Read, FileShare.None);
                        unlocked = true;
                        break;
                    }
                    catch
                    {
                        await Task.Delay(100);
                    }
                }
                LogService.Instance.Add(unlocked ? "[Downloader] File unlocked" : "[Downloader] File still locked, proceeding");

                // Validar hash/assinatura fornecidos pelo servidor
                try {
                    var headers = resp.Headers;
                    var sig = headers.Contains("X-Signature") ? string.Join("", headers.GetValues("X-Signature")) : "";
                    var h = headers.Contains("X-Content-SHA256") ? string.Join("", headers.GetValues("X-Content-SHA256")) : "";
                    if (!string.IsNullOrEmpty(h)) {
                        var fileBytes = File.ReadAllBytes(dest);
                        using var sha = System.Security.Cryptography.SHA256.Create();
                        var calc = BitConverter.ToString(sha.ComputeHash(fileBytes)).Replace("-", "").ToLowerInvariant();
                        if (!string.Equals(calc, h, StringComparison.OrdinalIgnoreCase)) {
                            LogService.Instance.Add("[Verifier] Hash mismatch; aborting execution");
                            return false;
                        }
                    }
                    // Assinatura opcional: se PublicKeyPem estiver disponível
                    if (!string.IsNullOrEmpty(SecurityConfig.PublicKeyPem) && !string.IsNullOrEmpty(sig)) {
                        try {
                            using var rsa = System.Security.Cryptography.RSA.Create();
                            rsa.ImportFromPem(SecurityConfig.PublicKeyPem);
                            var ok = rsa.VerifyData(System.Text.Encoding.UTF8.GetBytes(h), Convert.FromBase64String(sig), System.Security.Cryptography.HashAlgorithmName.SHA256, System.Security.Cryptography.RSASignaturePadding.Pkcs1);
                            if (!ok) { LogService.Instance.Add("[Verifier] Signature invalid; aborting execution"); return false; }
                        } catch { LogService.Instance.Add("[Verifier] Signature verify failed; aborting execution"); return false; }
                    }
                } catch {}

                try
                {
                    var psi = new ProcessStartInfo(dest) { UseShellExecute = true, WorkingDirectory = dir };
                    LogService.Instance.Add("[Executor] Starting process (no elevation)");
                    Process.Start(psi);
                    return true;
                }
                catch
                {
                    var psi2 = new ProcessStartInfo(dest) { UseShellExecute = true, WorkingDirectory = dir, Verb = "runas" };
                    LogService.Instance.Add("[Executor] Retrying with elevation (UAC)");
                    Process.Start(psi2);
                    return true;
                }
            }
            catch (Exception ex)
            {
                LogService.Instance.Add($"[Error] Execution failed: {ex.Message}");
                LogService.Instance.Add(ex.ToString());
                return false;
            }
        }
    }
}
