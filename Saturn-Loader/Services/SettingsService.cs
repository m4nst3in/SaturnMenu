using System.Linq;
using System.Windows;

namespace Saturn.Loader.Services
{
    public class SettingsService
    {
        public static SettingsService Instance { get; } = new SettingsService();
        public bool DarkTheme { get; private set; } = true;
        public bool HighContrast { get; private set; } = false;
        public void ApplyTheme(bool dark)
        {
            DarkTheme = dark;
            var dicts = Application.Current.Resources.MergedDictionaries;
            var light = dicts.FirstOrDefault(d => d.Source != null && d.Source.OriginalString.EndsWith("LightTheme.xaml"));
            var darkDict = dicts.FirstOrDefault(d => d.Source != null && d.Source.OriginalString.EndsWith("DarkTheme.xaml"));
            if (dark)
            {
                if (light != null) dicts.Remove(light);
                if (darkDict == null) dicts.Add(new ResourceDictionary { Source = new System.Uri("Themes/DarkTheme.xaml", System.UriKind.Relative) });
            }
            else
            {
                if (darkDict != null) dicts.Remove(darkDict);
                if (light == null) dicts.Add(new ResourceDictionary { Source = new System.Uri("Themes/LightTheme.xaml", System.UriKind.Relative) });
            }
        }
        public void ApplyHighContrast(bool enable)
        {
            HighContrast = enable;
            var dicts = Application.Current.Resources.MergedDictionaries;
            var hc = dicts.FirstOrDefault(d => d.Source != null && d.Source.OriginalString.EndsWith("HighContrast.xaml"));
            if (enable)
            {
                if (hc == null) dicts.Add(new ResourceDictionary { Source = new System.Uri("Themes/HighContrast.xaml", System.UriKind.Relative) });
            }
            else
            {
                if (hc != null) dicts.Remove(hc);
            }
        }
    }
}