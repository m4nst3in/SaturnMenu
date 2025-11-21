using System.Windows;
using System.Windows.Input;

namespace Noturnal.Loader
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            var vm = new ViewModels.ShellVM(MainFrame);
            DataContext = vm;

            // Atalhos de Teclado
            InputBindings.Add(new KeyBinding(vm.NavigateInjectorCommand, new KeyGesture(Key.D1, ModifierKeys.Control)));
            InputBindings.Add(new KeyBinding(vm.NavigateWebsiteCommand, new KeyGesture(Key.D2, ModifierKeys.Control)));

            // Lógica para arrastar a janela customizada
            TitleBar.MouseLeftButtonDown += (s, e) => 
            {
                if (e.ButtonState == MouseButtonState.Pressed)
                    DragMove();
            };
        }

        private void Minimize_Click(object sender, RoutedEventArgs e) => WindowState = WindowState.Minimized;
        private void Close_Click(object sender, RoutedEventArgs e) => Application.Current.Shutdown();
    }
}