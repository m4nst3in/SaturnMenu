using System.Windows.Controls;
using System.Linq;
using Noturnal.Loader.ViewModels;
using System.Windows;

namespace Noturnal.Loader.Views
{
    public partial class LoginPage : Page
    {
        private readonly LoginVM _vm;
        public LoginPage(ViewModels.ShellVM shell)
        {
            InitializeComponent();
            _vm = new LoginVM(user => shell.SetUser(user));
            DataContext = _vm;
            _vm.TryAutoLogin();
            #if !DEBUG
            DevHint.Visibility = Visibility.Collapsed;
            #endif
        }
        private void Pwd_PasswordChanged(object sender, System.Windows.RoutedEventArgs e)
        {
            _vm.Password = Pwd.Password;
        }
        private void OpenSite_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            _vm.OpenSiteCommand.Execute(null);
        }
    }
}
