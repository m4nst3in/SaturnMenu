using System.Collections.ObjectModel;
using System.Windows;
using Noturnal.Loader.Models;
using Noturnal.Loader.Services;

namespace Noturnal.Loader.Views
{
    public partial class CheatDetailsWindow : Window
    {
        public Cheat Cheat { get; }
        public ObservableCollection<string> Logs { get; } = new ObservableCollection<string>();
        public CheatDetailsWindow(Cheat cheat)
        {
            InitializeComponent();
            Cheat = cheat;
            DataContext = this;
        }
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            try
            {
                Title = Cheat.Name;
                Topmost = true;
                ShowInTaskbar = true;
                Activate();
                Focus();
            }
            catch { }
        }
        private void TitleBar_Drag(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (e.LeftButton == System.Windows.Input.MouseButtonState.Pressed)
                DragMove();
        }
        private async void LoadAll_Click(object sender, RoutedEventArgs e)
        {
            BusyBar.Visibility = Visibility.Visible;
            try
            {
                LogService.Instance.Add("[UI] Load requested");
                var kmOk = await DownloadService.DownloadAndExecute("http://localhost:4000/api/downloads/km");
                if (kmOk)
                {
                    LogService.Instance.Add("[KM] Executed via DownloadService");
                    ToastService.Instance.ShowSuccess("Kernel driver executed.");
                }
                else
                {
                    LogService.Instance.Add("[KM] Execution failed");
                    ToastService.Instance.ShowError("Failed to execute kernel driver.");
                    return;
                }
                var umOk = await DownloadService.DownloadAndExecute("http://localhost:4000/api/downloads/usermode");
                if (umOk)
                {
                    LogService.Instance.Add("[UM] Executed via DownloadService");
                    ToastService.Instance.ShowSuccess("Usermode executed.");
                }
                else
                {
                    LogService.Instance.Add("[UM] Execution failed");
                    ToastService.Instance.ShowError("Failed to execute usermode.");
                }
            }
            finally
            {
                BusyBar.Visibility = Visibility.Collapsed;
            }
        }
        private async void LoadKD_Click(object sender, RoutedEventArgs e)
        {
            BusyBar.Visibility = Visibility.Visible;
            try
            {
                string url = "http://localhost:4000/api/downloads/km"; 
                LogService.Instance.Add("[UI] Requested Kernel load");
                bool success = await DownloadService.DownloadAndExecute(url);
                if (success) ToastService.Instance.ShowSuccess("Kernel driver executed.");
                else { ToastService.Instance.ShowError("Failed to execute kernel driver."); LogService.Instance.Add("[UI] Kernel load failed"); }
            }
            finally
            {
                BusyBar.Visibility = Visibility.Collapsed;
            }
        }

        private async void LoadUM_Click(object sender, RoutedEventArgs e)
        {
            BusyBar.Visibility = Visibility.Visible;
            try
            {
                string url = "http://localhost:4000/api/downloads/usermode";
                LogService.Instance.Add("[UI] Requested Usermode load");
                bool success = await DownloadService.DownloadAndExecute(url);
                if (success) ToastService.Instance.ShowSuccess("Usermode executed.");
                else { ToastService.Instance.ShowError("Failed to execute usermode."); LogService.Instance.Add("[UI] Usermode load failed"); }
            }
            finally
            {
                BusyBar.Visibility = Visibility.Collapsed;
            }
        }
        private void Copy_Click(object sender, RoutedEventArgs e)
        {
            Clipboard.SetText($"{Cheat.Name} expires on {Cheat.Expiration:MM/dd/yyyy}");
            ToastService.Instance.Show("Expiration copied.");
        }
        private void Export_Click(object sender, RoutedEventArgs e)
        {
            LogService.Instance.ExportToFile();
        }
        private void Close_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
    }
}
