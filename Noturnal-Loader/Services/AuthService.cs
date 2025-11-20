using Noturnal.Loader.Models;

namespace Noturnal.Loader.Services
{
    public class AuthService
    {
        public static AuthService Instance { get; } = new AuthService();
        private readonly System.Net.Http.HttpClient _http = new System.Net.Http.HttpClient();
        private const string BaseUrl = "http://localhost:4000";
        public async System.Threading.Tasks.Task<User?> LoginAsync(string username, string password)
        {
            var payload = System.Text.Json.JsonSerializer.Serialize(new { email = username, password });
            var res = await _http.PostAsync(BaseUrl + "/api/auth/login", new System.Net.Http.StringContent(payload, System.Text.Encoding.UTF8, "application/json"));
            if (!res.IsSuccessStatusCode) return null;
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

            return u;
        }
    }
}
