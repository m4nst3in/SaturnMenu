using System.Windows;
using System.Windows.Controls;
using Noturnal.Loader.ViewModels;
using Noturnal.Loader.Models;
using System.Diagnostics;

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
            if ((sender as System.Windows.Controls.Border)?.DataContext is Cheat cheat)
            {
                var win = new CheatDetailsWindow(cheat);
                win.ShowDialog();
            }
        }
        private void OpenSite_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            Process.Start(new ProcessStartInfo("https://example.com") { UseShellExecute = true });
        }
    }
}
