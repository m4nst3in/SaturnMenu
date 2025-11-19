using System.Windows.Controls;
using Noturnal.Loader.ViewModels;

namespace Noturnal.Loader.Views
{
    public partial class SupportPage : Page
    {
        private readonly SupportVM _vm = new SupportVM();
        public SupportPage()
        {
            InitializeComponent();
        }
        private void OpenFaq_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo("https://example.com/faq") { UseShellExecute = true });
        }
        private void OpenDiscord_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            _vm.OpenDiscord();
        }
        private void OpenEmail_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            _vm.OpenEmail();
        }
        private void SendLogs_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            _vm.SendLogs();
        }
    }
}
