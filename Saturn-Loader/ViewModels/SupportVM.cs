using Saturn.Loader.Services;

namespace Saturn.Loader.ViewModels
{
    public class SupportVM
    {
        public void OpenDiscord() => System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo("https://discord.com") { UseShellExecute = true });
        public void OpenEmail() => System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo("mailto:support@example.com") { UseShellExecute = true });
        public void SendLogs() => LogService.Instance.ExportToFile();
    }
}