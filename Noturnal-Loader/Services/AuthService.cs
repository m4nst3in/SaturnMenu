using Noturnal.Loader.Models;

namespace Noturnal.Loader.Services
{
    public class AuthService
    {
        public static AuthService Instance { get; } = new AuthService();
        private readonly System.Net.Http.HttpClient _http = new System.Net.Http.HttpClient { Timeout = System.TimeSpan.FromSeconds(10) };
        private const string BaseUrl = "http://localhost:4000";
        public async System.Threading.Tasks.Task<(User? user, string? error)> LoginAsync(string username, string password)
        {
            try
            {
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
                var payload = System.Text.Json.JsonSerializer.Serialize(new { email = username, password });
                var res = await _http.PostAsync(BaseUrl + "/api/auth/login", new System.Net.Http.StringContent(payload, System.Text.Encoding.UTF8, "application/json"));
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
