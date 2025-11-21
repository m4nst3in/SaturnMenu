using Noturnal.Loader.Models;

namespace Noturnal.Loader.Services
{
    public class AuthService
    {
        public static AuthService Instance { get; } = new AuthService();
        private readonly System.Net.Http.HttpClient _http;
        private const string BaseUrl = "http://localhost:4000";
        public AuthService()
        {
            var handler = new System.Net.Http.HttpClientHandler();
            handler.ServerCertificateCustomValidationCallback = (req, cert, chain, errors) => {
                try {
                    if (cert == null) return false;
                    var pub = cert.GetPublicKey();
                    using var sha = System.Security.Cryptography.SHA256.Create();
                    var hash = System.BitConverter.ToString(sha.ComputeHash(pub)).Replace("-", "").ToLowerInvariant();
                    if (!string.IsNullOrEmpty(SecurityConfig.PinnedPublicKeyHash))
                        return hash == SecurityConfig.PinnedPublicKeyHash;
                    return true; // se não configurado, aceitar (dev)
                } catch { return false; }
            };
            _http = new System.Net.Http.HttpClient(handler) { Timeout = System.TimeSpan.FromSeconds(10) };
        }

        public async System.Threading.Tasks.Task<(User? user, string? error)> LoginAsync(string username, string password)
        {
            try
            {
                // Health check antes do login
                try
                {
                    var health = await _http.GetAsync(BaseUrl + "/api/health");
                    if (!health.IsSuccessStatusCode)
                        return (null, "Servidor indisponível (health check falhou)");
                }
                catch (System.Exception)
                {
                    return (null, "Servidor indisponível (sem resposta)");
                }

                #if DEBUG
                if (string.Equals(username, "admin", System.StringComparison.OrdinalIgnoreCase) && string.Equals(password, "admin", System.StringComparison.Ordinal))
                {
                    return (new User
                    {
                        Id = "dev_admin",
                        Username = "admin",
                        DisplayName = "Admin",
                        Email = "admin@local",
                        Hwid = null,
                        Subscription = new Subscription { Active = true, Plan = "DEV", ExpiresAt = null }
                    }, null);
                }
                #endif
                var cleanUser = (username ?? string.Empty).Trim();
                var cleanPass = (password ?? string.Empty).Trim();
                // Challenge de sessão
                var chalRes = await _http.GetAsync(BaseUrl + "/api/auth/challenge");
                string? nonce = null;
                if (chalRes.IsSuccessStatusCode)
                {
                    var cj = await chalRes.Content.ReadAsStringAsync();
                    using var cdoc = System.Text.Json.JsonDocument.Parse(cj);
                    if (cdoc.RootElement.TryGetProperty("nonce", out var n)) nonce = n.GetString();
                }
                var payload = System.Text.Json.JsonSerializer.Serialize(new { email = cleanUser, username = cleanUser, password = cleanPass });
                var req = new System.Net.Http.HttpRequestMessage(System.Net.Http.HttpMethod.Post, BaseUrl + "/api/auth/login");
                if (!string.IsNullOrEmpty(nonce)) req.Headers.Add("X-Client-Nonce", nonce);
                req.Content = new System.Net.Http.StringContent(payload, System.Text.Encoding.UTF8, "application/json");
                var res = await _http.SendAsync(req);
                if (!res.IsSuccessStatusCode)
                {
                    string? msg = null;
                    try
                    {
                        var j = await res.Content.ReadAsStringAsync();
                        using var d = System.Text.Json.JsonDocument.Parse(j);
                        if (d.RootElement.TryGetProperty("error", out var e)) msg = e.GetString();
                    }
                    catch { }
                    return (null, msg ?? ($"Falha no login: código {(int)res.StatusCode}"));
                }
                var json = await res.Content.ReadAsStringAsync();
                using var doc = System.Text.Json.JsonDocument.Parse(json);
                var userElem = doc.RootElement.GetProperty("user");
            
            var u = new User 
            { 
                Id = userElem.GetProperty("id").GetString() ?? "",
                Username = userElem.GetProperty("username").GetString() ?? "", 
                DisplayName = userElem.GetProperty("username").GetString() ?? "",
                Email = userElem.GetProperty("email").GetString() ?? "",
                Hwid = userElem.TryGetProperty("hwid", out var hwidProp) ? hwidProp.GetString() : null
            };

            // Parse subscription
            if (userElem.TryGetProperty("subscription", out var subElem))
            {
                u.Subscription = new Subscription
                {
                    Active = subElem.GetProperty("active").GetBoolean(),
                    Plan = subElem.TryGetProperty("plan", out var planProp) ? planProp.GetString() : null,
                    ExpiresAt = subElem.TryGetProperty("expiresAt", out var expProp) ? expProp.GetString() : null
                };
            }

                return (u, null);
            }
            catch (System.OperationCanceledException)
            {
                return (null, "Tempo de espera excedido (timeout)");
            }
            catch (System.Net.Http.HttpRequestException ex)
            {
                return (null, "Erro de rede: " + ex.Message);
            }
            catch (System.Exception ex)
            {
                return (null, "Erro: " + ex.Message);
            }
        }
    }
}
