using System.Windows;
using System.Windows.Navigation;

namespace Noturnal.Loader
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            var vm = new ViewModels.ShellVM(MainFrame);
            DataContext = vm;
            InputBindings.Add(new System.Windows.Input.KeyBinding(vm.NavigateInjectorCommand, new System.Windows.Input.KeyGesture(System.Windows.Input.Key.D1, System.Windows.Input.ModifierKeys.Control)));
            InputBindings.Add(new System.Windows.Input.KeyBinding(vm.NavigateWebsiteCommand, new System.Windows.Input.KeyGesture(System.Windows.Input.Key.D2, System.Windows.Input.ModifierKeys.Control)));
            InputBindings.Add(new System.Windows.Input.KeyBinding(vm.NavigateConfigCommand, new System.Windows.Input.KeyGesture(System.Windows.Input.Key.D3, System.Windows.Input.ModifierKeys.Control)));
            InputBindings.Add(new System.Windows.Input.KeyBinding(vm.NavigateSupportCommand, new System.Windows.Input.KeyGesture(System.Windows.Input.Key.D4, System.Windows.Input.ModifierKeys.Control)));
            TitleBar.MouseLeftButtonDown += (_, __) => DragMove();
        }
        private void Minimize_Click(object sender, System.Windows.RoutedEventArgs e) => WindowState = WindowState.Minimized;
        private void Close_Click(object sender, System.Windows.RoutedEventArgs e) => Close();
    }
}
