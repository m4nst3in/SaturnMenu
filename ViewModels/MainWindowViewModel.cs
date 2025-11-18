using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Threading;
using SaturnLoader.Services;
using SaturnLoader.Views;

namespace SaturnLoader
{
    public class MainWindowViewModel : INotifyPropertyChanged
    {
        private User? _currentUser;
        private Page _currentPage;
        private string _notificationMessage = string.Empty;
        private Brush _notificationBackground = Brushes.Transparent;
        private bool _isNotificationVisible;
        private readonly IInjectionService _injectionService;
        private readonly LogService _logService;

        public MainWindowViewModel()
        {
            _injectionService = new InjectionService();
            _logService = new LogService();
            CurrentPage = new LoginPage(this);
        }

        public User? CurrentUser
        {
            get => _currentUser;
            set
            {
                _currentUser = value;
                OnPropertyChanged();
            }
        }

        public Page CurrentPage
        {
            get => _currentPage;
            set
            {
                _currentPage = value;
                OnPropertyChanged();
            }
        }

        public string NotificationMessage
        {
            get => _notificationMessage;
            set
            {
                _notificationMessage = value;
                OnPropertyChanged();
            }
        }

        public Brush NotificationBackground
        {
            get => _notificationBackground;
            set
            {
                _notificationBackground = value;
                OnPropertyChanged();
            }
        }

        public bool IsNotificationVisible
        {
            get => _isNotificationVisible;
            set
            {
                _isNotificationVisible = value;
                OnPropertyChanged();
            }
        }

        public IInjectionService InjectionService => _injectionService;
        public LogService LogService => _logService;

        public void NavigateToPage(Page page)
        {
            CurrentPage = page;
        }

        public void ShowNotification(string message, NotificationType type = NotificationType.Info)
        {
            NotificationMessage = message;
            NotificationBackground = type switch
            {
                NotificationType.Success => new SolidColorBrush(Colors.Green),
                NotificationType.Error => new SolidColorBrush(Colors.Red),
                NotificationType.Warning => new SolidColorBrush(Colors.Orange),
                _ => new SolidColorBrush(Colors.Blue)
            };
            IsNotificationVisible = true;

            // Log the notification
            _logService.AddLog(type.ToString().ToUpper(), "NOTIFICATION", message);

            // Hide notification after 3 seconds
            var timer = new DispatcherTimer { Interval = TimeSpan.FromSeconds(3) };
            timer.Tick += (s, e) =>
            {
                IsNotificationVisible = false;
                timer.Stop();
            };
            timer.Start();
        }

        public bool LoadKernelDriver()
        {
            bool success = _injectionService.LoadKernelDriver();
            if (success)
            {
                ShowNotification("Kernel driver loaded successfully!", NotificationType.Success);
                _logService.AddLog("INFO", "KERNEL", "Kernel driver loaded successfully");
            }
            else
            {
                string error = _injectionService.GetLastError();
                ShowNotification($"Failed to load kernel driver: {error}", NotificationType.Error);
                _logService.AddLog("ERROR", "KERNEL", $"Failed to load kernel driver: {error}");
            }
            return success;
        }

        public bool InjectUserMode()
        {
            bool success = _injectionService.InjectUserMode();
            if (success)
            {
                ShowNotification("User mode cheat injected successfully!", NotificationType.Success);
                _logService.AddLog("INFO", "INJECTOR", "User mode cheat injected via fileless method");
            }
            else
            {
                string error = _injectionService.GetLastError();
                ShowNotification($"Failed to inject user mode: {error}", NotificationType.Error);
                _logService.AddLog("ERROR", "INJECTOR", $"Failed to inject user mode: {error}");
            }
            return success;
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    public enum NotificationType
    {
        Info,
        Success,
        Error,
        Warning
    }
}