using System;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using System.Windows;

namespace Saturn.Loader.Services
{
    public class Toast
    {
        public string Message { get; set; } = "";
        public ToastSeverity Severity { get; set; } = ToastSeverity.Info;
        public int TimeoutMs { get; set; } = 3000;
        public Guid Id { get; set; } = Guid.NewGuid();
    }
    public enum ToastSeverity { Info, Success, Error }
    public class ToastService
    {
        public static ToastService Instance { get; } = new ToastService();
        public ObservableCollection<Toast> Toasts { get; } = new ObservableCollection<Toast>();
        public void Show(string message, ToastSeverity severity = ToastSeverity.Info, int timeoutMs = 3000)
        {
            var t = new Toast { Message = message, Severity = severity, TimeoutMs = timeoutMs };
            Toasts.Add(t);
            _ = AutoDismissAsync(t);
        }
        public void ShowSuccess(string message, int timeoutMs = 3000) => Show(message, ToastSeverity.Success, timeoutMs);
        public void ShowError(string message, int timeoutMs = 3000) => Show(message, ToastSeverity.Error, timeoutMs);
        private async Task AutoDismissAsync(Toast toast)
        {
            try
            {
                await Task.Delay(toast.TimeoutMs);
                Application.Current?.Dispatcher?.Invoke(() => Toasts.Remove(toast));
            }
            catch { }
        }
    }
}