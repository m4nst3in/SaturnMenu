using System.Windows.Controls;

namespace SaturnLoader.Views
{
    public partial class WebsitePage : Page
    {
        public WebsitePage()
        {
            InitializeComponent();
        }

        private void OpenWebsite_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo
            {
                FileName = "https://saturn-loader.com",
                UseShellExecute = true
            });
        }
    }
}