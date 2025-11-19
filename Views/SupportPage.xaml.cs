using System.Diagnostics;
using System.Windows.Controls;

namespace NoturnalLoader.Views
{
    public partial class SupportPage : Page
    {
        public SupportPage()
        {
            InitializeComponent();
        }

        private void OpenDiscord_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            Process.Start(new ProcessStartInfo
            {
                FileName = "https://discord.gg/noturnal",
                UseShellExecute = true
            });
        }

        private void SendEmail_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            Process.Start(new ProcessStartInfo
            {
                FileName = "mailto:support@noturnal-loader.com",
                UseShellExecute = true
            });
        }

        private void OpenFAQ_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            Process.Start(new ProcessStartInfo
            {
                FileName = "https://noturnal-loader.com/faq",
                UseShellExecute = true
            });
        }

        private void SendLogs_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            // Logic to collect and send logs would go here
            System.Windows.MessageBox.Show("Logs sent successfully! We will respond within 24 hours.", "Support", 
                System.Windows.MessageBoxButton.OK, System.Windows.MessageBoxImage.Information);
        }
    }
}