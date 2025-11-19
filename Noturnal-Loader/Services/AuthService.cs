using Noturnal.Loader.Models;

namespace Noturnal.Loader.Services
{
    public class AuthService
    {
        public static AuthService Instance { get; } = new AuthService();
        private readonly System.Net.Http.HttpClient _http = new System.Net.Http.HttpClient();
        private const string BaseUrl = "http://localhost:3000";
        public async System.Threading.Tasks.Task<User?> LoginAsync(string username, string password)
        {
            var payload = System.Text.Json.JsonSerializer.Serialize(new { email = username, password });
            var res = await _http.PostAsync(BaseUrl + "/api/auth/login", new System.Net.Http.StringContent(payload, System.Text.Encoding.UTF8, "application/json"));
            if (!res.IsSuccessStatusCode) return null;
            var json = await res.Content.ReadAsStringAsync();
            using var doc = System.Text.Json.JsonDocument.Parse(json);
            var userElem = doc.RootElement.GetProperty("user");
            var u = new User { Username = userElem.GetProperty("username").GetString() ?? "", DisplayName = userElem.GetProperty("username").GetString() ?? "" };
            return u;
        }
    }
}
