using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using Noturnal.Loader.Infra;

namespace Noturnal.Loader.ViewModels
{
    public class ConfigVM : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler? PropertyChanged;
        private bool _darkTheme = true;
        private bool _showProfile = true;
        private bool _debugMode = false;
        public bool DarkTheme { get => _darkTheme; set { _darkTheme = value; OnPropertyChanged(); } }
        public bool ShowProfile { get => _showProfile; set { _showProfile = value; OnPropertyChanged(); } }
        public bool DebugMode { get => _debugMode; set { _debugMode = value; OnPropertyChanged(); } }
        private void OnPropertyChanged([CallerMemberName] string? n = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(n));
    }
}
