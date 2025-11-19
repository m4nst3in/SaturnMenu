using System.Windows.Controls;

namespace Noturnal.Loader.Views
{
    public partial class WebsitePage : Page
    {
        public WebsitePage()
        {
            InitializeComponent();
        }
        private void Open_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo("https://example.com") { UseShellExecute = true });
        }
    }
}
