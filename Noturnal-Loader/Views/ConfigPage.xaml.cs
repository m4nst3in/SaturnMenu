using System.Windows.Controls;

namespace Noturnal.Loader.Views
{
    public partial class ConfigPage : Page
    {
        public ConfigPage()
        {
            InitializeComponent();
            Loaded += ConfigPage_Loaded;
        }
        private void ConfigPage_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
            var check = this.FindName("DarkCheck") as System.Windows.Controls.CheckBox;
            if (check != null) check.Checked += (_, __) => Services.SettingsService.Instance.ApplyTheme(true);
            if (check != null) check.Unchecked += (_, __) => Services.SettingsService.Instance.ApplyTheme(false);
            var hc = this.FindName("HighContrastCheck") as System.Windows.Controls.CheckBox;
            if (hc != null) hc.Checked += (_, __) => Services.SettingsService.Instance.ApplyHighContrast(true);
            if (hc != null) hc.Unchecked += (_, __) => Services.SettingsService.Instance.ApplyHighContrast(false);
        }
    }
}
