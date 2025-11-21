using System.Collections.ObjectModel;
using System.Windows;
using Noturnal.Loader.Models;
using Noturnal.Loader.Services;
using System.Diagnostics;
using System.Security.Principal;
using System.Linq;
using System.Windows.Media;
using System.Windows.Threading;

namespace Noturnal.Loader.Views
{
    public partial class CheatDetailsWindow : Window
    {
        public Cheat Cheat { get; }
        public ObservableCollection<string> Logs { get; } = new ObservableCollection<string>();
        private static bool KernelLoadedFlag = false;
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
                var timer = new DispatcherTimer { Interval = System.TimeSpan.FromSeconds(1) };
                timer.Tick += (s, _) => UpdateDriverStatus();
                timer.Start();
                UpdateDriverStatus();
            }
            catch { }
        }
        private void TitleBar_Drag(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (e.LeftButton == System.Windows.Input.MouseButtonState.Pressed)
                DragMove();
        }

        private static bool IsAdministrator()
        {
            try
            {
                var wi = WindowsIdentity.GetCurrent();
                var wp = new WindowsPrincipal(wi);
                return wp.IsInRole(WindowsBuiltInRole.Administrator);
            }
            catch { return false; }
        }

        private static bool IsKernelReady()
        {
            try
            {
                NativeLoader.Initialize();
                if (KernelLoadedFlag) return true;
                var ready = NativeInterop.QueryDriverStatus() == 1 || NativeInterop.IsDriverReady() == 1;
                if (ready) KernelLoadedFlag = true;
                return ready;
            }
            catch { return false; }
        }

        private static bool IsKernelProcessRunning()
        {
            try
            {
                var procs = Process.GetProcesses();
                return procs.Any(p =>
                {
                    var name = "";
                    try { name = p.ProcessName; } catch { }
                    if (string.IsNullOrEmpty(name)) return false;
                    name = name.ToLowerInvariant();
                    return name.Contains("noturnal-km") || name.Contains("kernel") || name.Contains("noturnal_kernel");
                });
            }
            catch { return false; }
        }
        private static bool IsUsermodeRunning()
        {
            try
            {
                var procs = Process.GetProcesses();
                return procs.Any(p =>
                    {
                        var name = "";
                        try { name = p.ProcessName; } catch { }
                        if (string.IsNullOrEmpty(name)) return false;
                        name = name.ToLowerInvariant();
                        return name.Contains("noturnal-usermode") || name.Contains("noturnal-um") || name.Contains("usermode");
                    });
            }
            catch { return false; }
        }
        private async void LoadAll_Click(object sender, RoutedEventArgs e)
        {
            BusyBar.Visibility = Visibility.Visible;
            try
            {
                LogService.Instance.Add("[UI] Load requested");
                var kmUrl = "http://localhost:4000/api/downloads/km";
                var umUrl = "http://localhost:4000/api/downloads/usermode";

                var kernelReady = IsKernelReady() || IsKernelProcessRunning();
                if (kernelReady)
                {
                    LogService.Instance.Add("[KM] Kernel already ready, skipping KM start");
                    ToastService.Instance.Show("Kernel já carregado. Pulando KM.");
                }
                else
                {
                    if (!IsAdministrator())
                        LogService.Instance.Add("[KM] Not elevated; DownloadService may request UAC");
                    var kmOk = await DownloadService.DownloadAndExecute(kmUrl);
                    if (!kmOk)
                    {
                        LogService.Instance.Add("[KM] Execution failed");
                        ToastService.Instance.ShowError("Falha ao executar kernel driver.");
                        return;
                    }
                    LogService.Instance.Add("[KM] Executed via DownloadService");
                    ToastService.Instance.ShowSuccess("Kernel driver executado.");
                    KernelLoadedFlag = true;
                    UpdateDriverStatus();
                }

                var umRunning = IsUsermodeRunning();
                if (umRunning)
                {
                    LogService.Instance.Add("[UM] Usermode already running, skipping UM start");
                    ToastService.Instance.Show("Usermode já está em execução. Pulando UM.");
                }
                else
                {
                    var umOk = await DownloadService.DownloadAndExecute(umUrl);
                    if (umOk)
                    {
                        LogService.Instance.Add("[UM] Executed via DownloadService");
                        ToastService.Instance.ShowSuccess("Usermode executado.");
                    }
                    else
                    {
                        LogService.Instance.Add("[UM] Execution failed");
                        ToastService.Instance.ShowError("Falha ao executar usermode.");
                    }
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
                if (IsKernelReady() || IsKernelProcessRunning())
                {
                    LogService.Instance.Add("[KM] Kernel already ready, skipping KM start");
                    ToastService.Instance.Show("Kernel já carregado. Pulando KM.");
                }
                else
                {
                    if (!IsAdministrator())
                        LogService.Instance.Add("[KM] Not elevated; DownloadService may request UAC");
                    bool success = await DownloadService.DownloadAndExecute(url);
                    if (success) ToastService.Instance.ShowSuccess("Kernel driver executado.");
                    else { ToastService.Instance.ShowError("Falha ao executar kernel driver."); LogService.Instance.Add("[UI] Kernel load failed"); }
                    if (success) KernelLoadedFlag = true;
                    if (success) UpdateDriverStatus();
                }
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
                if (IsUsermodeRunning())
                {
                    LogService.Instance.Add("[UM] Usermode already running, skipping UM start");
                    ToastService.Instance.Show("Usermode já está em execução. Pulando UM.");
                }
                else
                {
                    bool success = await DownloadService.DownloadAndExecute(url);
                    if (success) ToastService.Instance.ShowSuccess("Usermode executado.");
                    else { ToastService.Instance.ShowError("Falha ao executar usermode."); LogService.Instance.Add("[UI] Usermode load failed"); }
                }
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

        private void UpdateDriverStatus()
        {
            try
            {
                bool on = IsKernelReady() || IsKernelProcessRunning();
                DriverStatus.Text = on ? "Driver: ON" : "Driver: OFF";
                DriverStatus.Foreground = on ? new SolidColorBrush(Color.FromRgb(121, 192, 255)) : new SolidColorBrush(Color.FromRgb(139, 148, 158));
            }
            catch { }
        }
    }
}
