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
        private void TitleBar_Drag(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (e.LeftButton == System.Windows.Input.MouseButtonState.Pressed)
                DragMove();
        }
        private async void LoadKD_Click(object sender, RoutedEventArgs e)
        {
            BusyBar.Visibility = Visibility.Visible;
            try
            {
                // Placeholder URL - User needs to update this
                string url = "http://localhost:4000/api/downloads/km"; 
                LogService.Instance.Add("[UI] Requested Kernel load");
                bool success = await DownloadService.DownloadAndExecute(url);
                
                StepKD.Text = success ? "KD ✓" : "KD ✗";
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
                // Placeholder URL - User needs to update this
                string url = "http://localhost:4000/api/downloads/usermode";
                LogService.Instance.Add("[UI] Requested Usermode load");
                bool success = await DownloadService.DownloadAndExecute(url);

                StepUM.Text = success ? "UM ✓" : "UM ✗";
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
