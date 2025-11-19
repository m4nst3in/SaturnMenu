using System.Windows.Controls;

namespace NoturnalLoader.Views
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
                FileName = "https://noturnal-loader.com",
                UseShellExecute = true
            });
        }
    }
}