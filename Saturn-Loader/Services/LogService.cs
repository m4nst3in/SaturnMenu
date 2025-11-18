using System.Collections.ObjectModel;
using System.IO;
using Saturn.Loader.Models;

namespace Saturn.Loader.Services
{
    public class LogService
    {
        public static LogService Instance { get; } = new LogService();
        public ObservableCollection<LogEntry> Entries { get; } = new ObservableCollection<LogEntry>();
        public void Add(string message)
        {
            Entries.Add(new LogEntry { Message = message });
        }
        public void ExportToFile()
        {
            var path = Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.Desktop), "Saturn-Loader-logs.txt");
            using var sw = new StreamWriter(path, false);
            foreach (var e in Entries) sw.WriteLine($"[{e.Timestamp:HH:mm:ss}] {e.Message}");
            ToastService.Instance.Show("Logs exported.");
        }
    }
}