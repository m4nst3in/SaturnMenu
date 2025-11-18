using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace SaturnLoader.Views
{
    public partial class LoginPage : Page
    {
        private readonly MainWindowViewModel _viewModel;

        public LoginPage(MainWindowViewModel viewModel)
        {
            InitializeComponent();
            _viewModel = viewModel;
        }

        private void LoginButton_Click(object sender, RoutedEventArgs e)
        {
            PerformLogin();
        }

        private void PasswordBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                PerformLogin();
            }
        }

        private void PerformLogin()
        {
            string username = UsernameTextBox.Text.Trim();
            string password = PasswordBox.Password;

            // Development mode credentials
            if (username == "admin" && password == "admin")
            {
                var user = new User
                {
                    Username = "admin",
                    Email = "admin@saturn.com",
                    IsAdmin = true,
                    CreatedAt = DateTime.Now,
                    Subscriptions = new()
                    {
                        new Subscription
                        {
                            Name = "CS2 Premium",
                            Game = "Counter-Strike 2",
                            ExpiresAt = DateTime.Now.AddDays(16),
                            IconColor = "#FFFFA500"
                        },
                        new Subscription
                        {
                            Name = "Valorant Pro",
                            Game = "Valorant",
                            ExpiresAt = DateTime.Now.AddDays(30),
                            IconColor = "#FFFF4444"
                        }
                    }
                };

                _viewModel.CurrentUser = user;
                _viewModel.ShowNotification($"Welcome back, {user.Username}!", NotificationType.Success);
                
                // Navigate to main page
                _viewModel.NavigateToPage(new MainPage(_viewModel));
            }
            else
            {
                ErrorTextBlock.Text = "Invalid username or password";
                ErrorTextBlock.Visibility = Visibility.Visible;
                _viewModel.ShowNotification("Login failed: Invalid credentials", NotificationType.Error);
            }
        }

        private void RegisterButton_Click(object sender, RoutedEventArgs e)
        {
            // Open website for registration
            System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo
            {
                FileName = "https://saturn-loader.com/register",
                UseShellExecute = true
            });
        }
    }
}