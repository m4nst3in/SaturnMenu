using System.Windows.Controls;
using Saturn.Loader.ViewModels;

namespace Saturn.Loader.Views
{
    public partial class LoginPage : Page
    {
        private readonly LoginVM _vm;
        public LoginPage(ViewModels.ShellVM shell)
        {
            InitializeComponent();
            _vm = new LoginVM(user => shell.SetUser(user));
            DataContext = _vm;
        }
        private void Login_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            _vm.Password = Pwd.Password;
            _vm.LoginCommand.Execute(null);
        }
        private void OpenSite_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            _vm.OpenSiteCommand.Execute(null);
        }
    }
}