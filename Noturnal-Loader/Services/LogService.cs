using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Windows;
using Noturnal.Loader.Models;

namespace Noturnal.Loader.Services
{
    public class LogService
    {
        public static LogService Instance { get; } = new LogService();
        public ObservableCollection<LogEntry> Entries { get; } = new ObservableCollection<LogEntry>();
        public void Add(string message)
        {
            var entry = new LogEntry { Message = message };
            var disp = Application.Current?.Dispatcher;
            if (disp == null || disp.CheckAccess())
                Entries.Add(entry);
            else
                disp.BeginInvoke(new Action(() => Entries.Add(entry)));
        }
        public void ExportToFile()
        {
            var path = Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.Desktop), "Noturnal-Loader-logs.txt");
            using var sw = new StreamWriter(path, false);
            foreach (var e in Entries) sw.WriteLine($"[{e.Timestamp:HH:mm:ss}] {e.Message}");
            ToastService.Instance.Show("Logs exported.");
        }
    }
}
