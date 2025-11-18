using System;
using System.Windows;

namespace SaturnLoader.Views
{
    public partial class CheatDetailsWindow : Window
    {
        private readonly Subscription _subscription;
        private readonly MainWindowViewModel _viewModel;

        public CheatDetailsWindow(Subscription subscription, MainWindowViewModel viewModel)
        {
            InitializeComponent();
            _subscription = subscription;
            _viewModel = viewModel;
            DataContext = subscription;
            
            LoadCheatDetails();
        }

        private void LoadCheatDetails()
        {
            // Mock cheat details
            var cheatDetails = new CheatDetails
            {
                Name = _subscription.Name,
                Version = "1.2.3",
                Description = "Advanced CS2 cheat with aimbot, ESP, and more features",
                LastUpdate = DateTime.Now.AddDays(-2),
                Status = _subscription.IsActive ? "Undetected" : "Expired",
                IsDetected = false,
                Features = new[] { "Aimbot", "ESP", "Triggerbot", "Skin Changer", "Radar" }
            };
            
            FeaturesItemsControl.ItemsSource = cheatDetails.Features;
        }

        private void LoadKD_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                _viewModel.ShowNotification("Loading kernel driver...", NotificationType.Info);
                
                // Simulate kernel driver loading
                System.Threading.Tasks.Task.Delay(2000).ContinueWith(_ =>
                {
                    Dispatcher.Invoke(() =>
                    {
                        _viewModel.ShowNotification("Kernel driver loaded successfully!", NotificationType.Success);
                        Close();
                    });
                });
            }
            catch (Exception ex)
            {
                _viewModel.ShowNotification($"Failed to load kernel driver: {ex.Message}", NotificationType.Error);
            }
        }

        private void LoadUM_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                _viewModel.ShowNotification("Loading user mode cheat...", NotificationType.Info);
                
                // Simulate fileless injection
                System.Threading.Tasks.Task.Delay(3000).ContinueWith(_ =>
                {
                    Dispatcher.Invoke(() =>
                    {
                        _viewModel.ShowNotification("User mode cheat injected successfully!", NotificationType.Success);
                        Close();
                    });
                });
            }
            catch (Exception ex)
            {
                _viewModel.ShowNotification($"Failed to inject user mode: {ex.Message}", NotificationType.Error);
            }
        }

        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
    }
}