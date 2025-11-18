using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Media;
using Saturn.Loader.Services;

namespace Saturn.Loader.Converters
{
    public class SeverityToBrush : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var sev = value as ToastSeverity? ?? ToastSeverity.Info;
            switch (sev)
            {
                case ToastSeverity.Success: return System.Windows.Application.Current.Resources["Brush.Success"] as Brush;
                case ToastSeverity.Error: return System.Windows.Application.Current.Resources["Brush.Error"] as Brush;
                default: return System.Windows.Application.Current.Resources["Brush.Accent"] as Brush;
            }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) => ToastSeverity.Info;
    }
}