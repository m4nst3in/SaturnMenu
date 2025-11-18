using System.Windows.Controls;

namespace SaturnLoader.Views
{
    public partial class ConfigPage : Page
    {
        public ConfigPage()
        {
            InitializeComponent();
        }

        private void SaveSettings_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            // Save settings logic would go here
            System.Windows.MessageBox.Show("Settings saved successfully!", "Saturn Loader", 
                System.Windows.MessageBoxButton.OK, System.Windows.MessageBoxImage.Information);
        }
    }
}