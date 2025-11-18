using System.Windows;

namespace Saturn.Loader
{
    public partial class App : Application
    {
        private static Services.NativeInterop.LogCallback _loggerCallback;

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            Services.NativeLoader.Initialize();
            try
            {
                _loggerCallback = new Services.NativeInterop.LogCallback(msg => Services.LogService.Instance.Add(msg));
                Services.NativeInterop.SetLogger(_loggerCallback);
            }
            catch { }
            Services.PipeLogServer.Start();
        }
    }
}