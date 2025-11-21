using System.Windows;
using System.Windows.Controls;
using Noturnal.Loader.ViewModels;
using Noturnal.Loader.Models;
using System.Diagnostics;
using Noturnal.Loader.Services;

namespace Noturnal.Loader.Views
{
    public partial class InjectorPage : Page
    {
        private readonly InjectorVM _vm = new InjectorVM();
        public InjectorPage()
        {
            InitializeComponent();
            DataContext = _vm;
            Loaded += async (_, __) => await _vm.SimulateLoadAsync();
        }
        private void Inject_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            if ((sender as Button)?.DataContext is Cheat cheat) _vm.Inject(cheat);
        }
        private void Details_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            if ((sender as Button)?.DataContext is Cheat cheat)
            {
                var win = new CheatDetailsWindow(cheat);
                win.ShowDialog();
            }
        }
        private void Card_Click(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            var cheat = (sender as FrameworkElement)?.DataContext as Cheat
                        ?? (e.Source as FrameworkElement)?.DataContext as Cheat;
            LogService.Instance.Add("[UI] Card_Click");
            if (cheat == null)
            {
                ToastService.Instance.Show("Seleção inválida.");
                return;
            }
            try
            {
                var win = new CheatDetailsWindow(cheat);
                win.ShowDialog();
            }
            catch (System.Exception ex)
            {
                ToastService.Instance.ShowError("Falha ao abrir detalhes: " + ex.Message);
                LogService.Instance.Add("[UI] Open details error: " + ex.Message);
            }
        }
        private void OpenSite_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            Process.Start(new ProcessStartInfo("https://example.com") { UseShellExecute = true });
        }

        

        private void CardButton_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            var cheat = (sender as System.Windows.FrameworkElement)?.DataContext as Cheat;
            LogService.Instance.Add("[UI] CardButton_Click");
            if (cheat == null)
            {
                ToastService.Instance.Show("Seleção inválida.");
                return;
            }
            try
            {
                OpenDetails(cheat);
            }
            catch (System.Exception ex)
            {
                ToastService.Instance.ShowError("Falha ao abrir detalhes: " + ex.Message);
                LogService.Instance.Add("[UI] Open details error: " + ex.Message);
            }
            e.Handled = true;
        }

        private Cheat? FindCheatFrom(System.Windows.DependencyObject? d)
        {
            while (d != null)
            {
                if (d is FrameworkElement fe && fe.DataContext is Cheat c) return c;
                d = System.Windows.Media.VisualTreeHelper.GetParent(d);
            }
            return null;
        }


        private void OpenDetails(Cheat cheat)
        {
            try
            {
                var win = new CheatDetailsWindow(cheat)
                {
                    ShowActivated = true,
                    ShowInTaskbar = true,
                    WindowStartupLocation = System.Windows.WindowStartupLocation.Manual
                };
                var owner = System.Windows.Application.Current?.MainWindow;
                if (owner != null) win.Owner = owner;
                var sw = System.Windows.SystemParameters.PrimaryScreenWidth;
                var sh = System.Windows.SystemParameters.PrimaryScreenHeight;
                win.Left = (sw - win.Width) / 2;
                win.Top = (sh - win.Height) / 2;
                win.ShowDialog();
            }
            catch (System.Exception ex)
            {
                System.Windows.MessageBox.Show("Erro ao abrir detalhes: " + ex.ToString());
                var fb = new System.Windows.Window
                {
                    Title = cheat.Name,
                    Width = 560,
                    Height = 400,
                    WindowStartupLocation = System.Windows.WindowStartupLocation.CenterScreen,
                    Content = new System.Windows.Controls.ScrollViewer { Content = new System.Windows.Controls.TextBlock { Text = "Detalhes indisponíveis.\n\n" + ex.ToString(), Margin = new System.Windows.Thickness(20), TextWrapping = System.Windows.TextWrapping.Wrap } }
                };
                fb.ShowDialog();
            }
        }
    }
}
