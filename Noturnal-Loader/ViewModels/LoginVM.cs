using System.ComponentModel;
using System.Runtime.CompilerServices;
using Noturnal.Loader.Infra;
using Noturnal.Loader.Models;
using Noturnal.Loader.Services;

namespace Noturnal.Loader.ViewModels
{
    public class LoginVM : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler? PropertyChanged;
        private string _username = "";
        private string _password = "";
        public string Username { get => _username; set { _username = value; OnPropertyChanged(); } }
        public string Password { get => _password; set { _password = value; OnPropertyChanged(); } }
        public RelayCommand LoginCommand { get; }
        public RelayCommand OpenSiteCommand { get; }
        private readonly System.Action<User> _onLogin;
        public LoginVM(System.Action<User> onLogin)
        {
            LoginCommand = new RelayCommand(Login);
            OpenSiteCommand = new RelayCommand(() => System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo("https://noturnal-loader.com") { UseShellExecute = true }));
            _onLogin = onLogin;
        }
        private void Login()
        {
            try
            {
                var user = AuthService.Instance.LoginAsync(Username, Password).GetAwaiter().GetResult();
                if (user != null) _onLogin(user);
                else ToastService.Instance.Show("Falha no login.");
            }
            catch
            {
                ToastService.Instance.Show("Erro de rede ao autenticar.");
            }
        }
        private void OnPropertyChanged([CallerMemberName] string? n = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(n));
    }
}
