using System.Collections.ObjectModel;
using Saturn.Loader.Models;
using Saturn.Loader.Services;

namespace Saturn.Loader.ViewModels
{
    public class InjectorVM : System.ComponentModel.INotifyPropertyChanged
    {
        public event System.ComponentModel.PropertyChangedEventHandler? PropertyChanged;
        public ObservableCollection<Cheat> Cheats { get; } = new ObservableCollection<Cheat>(CheatService.Instance.GetCheats());
        public bool HasCheats => Cheats.Count > 0;
        private bool _isLoading;
        public bool IsLoading { get => _isLoading; private set { _isLoading = value; PropertyChanged?.Invoke(this, new System.ComponentModel.PropertyChangedEventArgs(nameof(IsLoading))); } }
        public async System.Threading.Tasks.Task SimulateLoadAsync()
        {
            IsLoading = true;
            await System.Threading.Tasks.Task.Delay(1200);
            IsLoading = false;
        }
        public void Inject(Cheat cheat)
        {
            var ready = NativeInterop.IsDriverReady();
            if (ready != 1)
            {
                var kdRes = NativeInterop.LoadKernelDriver("SaturnKD", System.IntPtr.Zero);
                if (kdRes == 0) { ToastService.Instance.ShowSuccess("Kernel driver loaded."); LogService.Instance.Add("Kernel driver loaded"); }
                else { ToastService.Instance.ShowError("Failed to load driver."); LogService.Instance.Add("Failed to load driver"); return; }
            }
            var payload = new byte[] { 0x00 };
            var res = NativeInterop.LoadUsermodeFromMemory(payload, payload.Length, System.IntPtr.Zero);
            if (res == 0) { ToastService.Instance.ShowSuccess("Cheat injected."); LogService.Instance.Add("Cheat injected: " + cheat.Name); }
            else { ToastService.Instance.ShowError("Injection error."); LogService.Instance.Add("Injection error"); }
        }
    }
}