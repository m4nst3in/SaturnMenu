using System.Windows;
using System.Threading.Tasks;
using System;

namespace Noturnal.Loader
{
    public partial class App : Application
    {
        private static Services.NativeInterop.LogCallback _loggerCallback;

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            this.DispatcherUnhandledException += (s, ex) => { Services.LogService.Instance.Add("[FATAL] UI exception: " + ex.Exception.Message); Services.LogService.Instance.Add(ex.Exception.ToString()); ex.Handled = true; };
            AppDomain.CurrentDomain.UnhandledException += (s, ex) => { Services.LogService.Instance.Add("[FATAL] Unhandled exception: " + (ex.ExceptionObject?.ToString() ?? "unknown")); };
            TaskScheduler.UnobservedTaskException += (s, ex) => { Services.LogService.Instance.Add("[FATAL] Task exception: " + ex.Exception.Message); Services.LogService.Instance.Add(ex.Exception.ToString()); ex.SetObserved(); };
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
