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
        private bool _isLoading;
        private string _status = "";
        private bool _rememberMe;
        public string Username { get => _username; set { _username = value; OnPropertyChanged(); LoginCommand.RaiseCanExecuteChanged(); } }
        public string Password { get => _password; set { _password = value; OnPropertyChanged(); LoginCommand.RaiseCanExecuteChanged(); } }
        public bool IsLoading { get => _isLoading; private set { _isLoading = value; OnPropertyChanged(nameof(IsLoading)); LoginCommand.RaiseCanExecuteChanged(); } }
        public string Status { get => _status; private set { _status = value; OnPropertyChanged(nameof(Status)); } }
        public bool RememberMe { get => _rememberMe; set { _rememberMe = value; OnPropertyChanged(); } }
        public RelayCommand LoginCommand { get; }
        public RelayCommand OpenSiteCommand { get; }
        private readonly System.Action<User> _onLogin;
        public LoginVM(System.Action<User> onLogin)
        {
            LoginCommand = new RelayCommand(Login, () => !_isLoading && !string.IsNullOrWhiteSpace(Username) && !string.IsNullOrWhiteSpace(Password));
            OpenSiteCommand = new RelayCommand(() => System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo("https://noturnal-loader.com") { UseShellExecute = true }));
            _onLogin = onLogin;
        }
        private async void Login()
        {
            try
            {
                IsLoading = true; Status = "Autenticando...";
                var res = await AuthService.Instance.LoginAsync(Username, Password);
                if (res.user != null)
                {
                    if (RememberMe) CredentialStore.Save(Username, Password); else CredentialStore.Clear();
                    System.Windows.Application.Current.Dispatcher.Invoke(() => _onLogin(res.user));
                }
                else { Status = res.error ?? "Falha no login."; ToastService.Instance.ShowError(Status); }
            }
            catch
            {
                Status = "Erro de rede ao autenticar."; ToastService.Instance.Show("Erro de rede ao autenticar.");
            }
            finally { IsLoading = false; }
        }
        private void OnPropertyChanged([CallerMemberName] string? n = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(n));
        public async void TryAutoLogin()
        {
            if (IsLoading) return;
            if (CredentialStore.TryGet(out var u, out var p))
            {
                try
                {
                    IsLoading = true; Status = "Autenticando...";
                    Username = u; Password = p; RememberMe = true;
                    var res = await AuthService.Instance.LoginAsync(u, p);
                    if (res.user != null) System.Windows.Application.Current.Dispatcher.Invoke(() => _onLogin(res.user));
                    else { Status = res.error ?? "Falha no login."; ToastService.Instance.ShowError(Status); }
                }
                catch { Status = "Erro de rede ao autenticar."; ToastService.Instance.Show("Erro de rede ao autenticar."); }
                finally { IsLoading = false; }
            }
        }
    }
}
