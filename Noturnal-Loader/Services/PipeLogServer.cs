using System;
using System.IO;
using System.IO.Pipes;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Noturnal.Loader.Services
{
    public static class PipeLogServer
    {
        private const string PipeName = "NoturnalLoaderLog";
        private static CancellationTokenSource _cts = new CancellationTokenSource();

        public static void Start()
        {
            Task.Run(async () =>
            {
                while (!_cts.IsCancellationRequested)
                {
                    try
                    {
                        using var server = new NamedPipeServerStream(PipeName, PipeDirection.In, 4, PipeTransmissionMode.Byte, PipeOptions.Asynchronous);
                        await server.WaitForConnectionAsync(_cts.Token).ConfigureAwait(false);
                        using var reader = new StreamReader(server, Encoding.UTF8);
                        while (server.IsConnected && !_cts.IsCancellationRequested)
                        {
                            var line = await reader.ReadLineAsync().ConfigureAwait(false);
                            if (line == null) break;
                            LogService.Instance.Add(line);
                        }
                    }
                    catch { }
                }
            }, _cts.Token);
        }

        public static void Stop()
        {
            try { _cts.Cancel(); } catch { }
        }
    }
}
