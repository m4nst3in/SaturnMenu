using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows.Controls;
using Saturn.Loader.Infra;
using Saturn.Loader.Models;
using Saturn.Loader.Services;

namespace Saturn.Loader.ViewModels
{
    public class ShellVM : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler? PropertyChanged;
        private readonly Frame _frame;
        public User CurrentUser { get; set; } = new User();
        private string _selectedTab = "Injector";
        public string SelectedTab { get => _selectedTab; set { _selectedTab = value; OnPropertyChanged(nameof(SelectedTab)); } }
        public ObservableCollection<Toast> Toasts { get; } = ToastService.Instance.Toasts;
        public RelayCommand NavigateInjectorCommand { get; }
        public RelayCommand NavigateWebsiteCommand { get; }
        public RelayCommand NavigateConfigCommand { get; }
        public RelayCommand NavigateSupportCommand { get; }

        public ShellVM(Frame frame)
        {
            _frame = frame;
            NavigateInjectorCommand = new RelayCommand(() => { SelectedTab = "Injector"; _frame.Navigate(new Views.InjectorPage()); });
            NavigateWebsiteCommand = new RelayCommand(() => { SelectedTab = "Website"; _frame.Navigate(new Views.WebsitePage()); });
            NavigateConfigCommand = new RelayCommand(() => { SelectedTab = "Settings"; _frame.Navigate(new Views.ConfigPage()); });
            NavigateSupportCommand = new RelayCommand(() => { SelectedTab = "Support"; _frame.Navigate(new Views.SupportPage()); });
            _frame.Navigate(new Views.LoginPage(this));
        }
        public void SetUser(User user)
        {
            CurrentUser = user;
            OnPropertyChanged(nameof(CurrentUser));
            ToastService.Instance.ShowSuccess("Welcome, " + user.DisplayName + ".");
            SelectedTab = "Injector";
            _frame.Navigate(new Views.InjectorPage());
        }
        private void OnPropertyChanged([CallerMemberName] string? n = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(n));
    }
}