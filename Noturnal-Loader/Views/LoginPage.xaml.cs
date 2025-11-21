using System.Windows.Controls;
using Noturnal.Loader.ViewModels;

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
