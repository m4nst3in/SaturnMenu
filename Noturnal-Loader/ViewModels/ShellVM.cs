using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows.Controls;
using Noturnal.Loader.Infra;
using Noturnal.Loader.Models;
using Noturnal.Loader.Services;

namespace Noturnal.Loader.ViewModels
{
    public class ShellVM : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler? PropertyChanged;
        private readonly Frame _frame;
        public User CurrentUser { get; set; } = new User();
        private string _selectedTab = "Login";
        private bool _isLoggedIn;
        public bool IsLoggedIn { get => _isLoggedIn; private set { _isLoggedIn = value; OnPropertyChanged(nameof(IsLoggedIn)); NavigateInjectorCommand.RaiseCanExecuteChanged(); NavigateWebsiteCommand.RaiseCanExecuteChanged(); NavigateConfigCommand.RaiseCanExecuteChanged(); NavigateSupportCommand.RaiseCanExecuteChanged(); } }
        public string SelectedTab { get => _selectedTab; set { _selectedTab = value; OnPropertyChanged(nameof(SelectedTab)); } }
        public ObservableCollection<Toast> Toasts { get; } = ToastService.Instance.Toasts;
        public RelayCommand NavigateInjectorCommand { get; }
        public RelayCommand NavigateWebsiteCommand { get; }
        public RelayCommand NavigateConfigCommand { get; }
        public RelayCommand NavigateSupportCommand { get; }

        public ShellVM(Frame frame)
        {
            _frame = frame;
            NavigateInjectorCommand = new RelayCommand(() => { SelectedTab = "Injector"; _frame.Navigate(new Views.InjectorPage()); }, () => IsLoggedIn);
            NavigateWebsiteCommand = new RelayCommand(() => { SelectedTab = "Website"; _frame.Navigate(new Views.WebsitePage()); }, () => IsLoggedIn);
            NavigateConfigCommand = new RelayCommand(() => { SelectedTab = "Settings"; _frame.Navigate(new Views.ConfigPage()); }, () => IsLoggedIn);
            NavigateSupportCommand = new RelayCommand(() => { SelectedTab = "Support"; _frame.Navigate(new Views.SupportPage()); }, () => IsLoggedIn);
            _frame.Navigate(new Views.LoginPage(this));
        }
        public void SetUser(User user)
        {
            CurrentUser = user;
            OnPropertyChanged(nameof(CurrentUser));
            ToastService.Instance.ShowSuccess("Welcome, " + user.DisplayName + ".");
            IsLoggedIn = true;
            SelectedTab = "Injector";
            _frame.Navigate(new Views.InjectorPage());
        }
        private void OnPropertyChanged([CallerMemberName] string? n = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(n));
    }
}
