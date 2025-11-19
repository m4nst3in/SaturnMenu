using System.Text.Json.Serialization;
using System.Text.Json;
using System.Net.Http;

var builder = WebApplication.CreateBuilder(args);
builder.Services.AddCors(o => o.AddDefaultPolicy(p => p.AllowAnyOrigin().AllowAnyHeader().AllowAnyMethod()));

var app = WebApplication.Create();
app.UseCors();

// Simple in-memory dev store
var store = new DevStore(Path.Combine(AppContext.BaseDirectory, "server-data.json"));

app.MapPost("/api/auth/login", async (LoginRequest req) => {
    var user = store.FindByEmail(req.Email);
    if (user == null) return Results.BadRequest(new { error = "Invalid credentials" });
    return Results.Ok(new { user, jwt = "dev-token" });
});

app.MapPost("/api/auth/register", async (RegisterRequest req) => {
    var user = store.CreateUser(req.Username, req.Email);
    return Results.Ok(new { user, jwt = "dev-token" });
});

app.MapPost("/api/subscription/purchase", async (PurchaseRequest req) => {
    var user = store.CurrentUserOrThrow();
    store.UpdateSubscription(user.Id, true, req.Plan, DateTime.UtcNow.AddDays(30));
    return Results.Ok(store.UserById(user.Id));
});

app.MapPost("/api/subscription/reset-hwid", () => {
    var user = store.CurrentUserOrThrow();
    store.UpdateHwid(user.Id, null);
    return Results.Ok(store.UserById(user.Id));
});

app.MapGet("/api/downloads/loader", (string plan) => {
    var url = plan.Equals("PREMIUM", StringComparison.OrdinalIgnoreCase)
        ? "/downloads/noturnal_loader_kernel_v2.exe"
        : "/downloads/noturnal_loader_client.exe";
    return Results.Ok(new { url, method = plan.Equals("PREMIUM", StringComparison.OrdinalIgnoreCase) ? "SECURE_KERNEL" : "STANDARD" });
});

app.MapGet("/api/versions", () => {
    var payload = new {
        record = new {
            **kernel_ver = new[] { "2.4.7.1" }, // from cfg.h
            **usermode_ver = new[] { "3.6.8.1" },
            last_access_time = DateTime.UtcNow.ToString("yyyy-MM-dd HH:mm:ss")
        }
    };
    return Results.Json(payload);
});

app.MapGet("/api/offsets/{name}", async (string name) => {
    var map = new Dictionary<string, string> {
        ["offsets.json"] = "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/offsets.json",
        ["buttons.json"] = "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/buttons.json",
        ["client_dll.json"] = "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/client_dll.json",
    };
    if (!map.TryGetValue(name, out var url)) return Results.NotFound();
    using var http = new HttpClient();
    var text = await http.GetStringAsync(url);
    return Results.Text(text, "application/json");
});

app.Run();

record LoginRequest(string Email, string Password);
record RegisterRequest(string Username, string Email, string Password);
record PurchaseRequest(string Plan);

class DevStore
{
    public DevStore(string path) { _path = path; Load(); }
    string _path;
    DevData _data = new() { Users = new List<User>() };
    public User? FindByEmail(string email) => _data.Users.FirstOrDefault(u => u.Email.Equals(email, StringComparison.OrdinalIgnoreCase));
    public User CreateUser(string username, string email) {
        var u = new User { Id = $"uid_{DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()}", Username = username, Email = email, Hwid = null, Subscription = new Subscription { Active=false, Plan=null, ExpiresAt=null } };
        _data.Users.Add(u); _data.CurrentUserId = u.Id; Save(); return u;
    }
    public User CurrentUserOrThrow() { if (_data.CurrentUserId == null) throw new InvalidOperationException("no current user"); return _data.Users.First(x=>x.Id==_data.CurrentUserId); }
    public void UpdateSubscription(string id, bool active, string? plan, DateTime? expiresAt) { var u = _data.Users.First(x=>x.Id==id); u.Subscription = new Subscription { Active=active, Plan=plan, ExpiresAt=expiresAt?.ToString("o") }; _data.CurrentUserId = id; Save(); }
    public void UpdateHwid(string id, string? hwid) { var u = _data.Users.First(x=>x.Id==id); u.Hwid = hwid; Save(); }
    public User UserById(string id) => _data.Users.First(x=>x.Id==id);
    void Load(){ if (File.Exists(_path)) { _data = JsonSerializer.Deserialize<DevData>(File.ReadAllText(_path)) ?? _data; } }
    void Save(){ File.WriteAllText(_path, JsonSerializer.Serialize(_data, new JsonSerializerOptions{ WriteIndented=true })); }
}

class DevData { public List<User> Users { get; set; } = new(); public string? CurrentUserId { get; set; } }
class User { public string Id { get; set; } = ""; public string Username { get; set; } = ""; public string Email { get; set; } = ""; public string? Hwid { get; set; } = null; public Subscription Subscription { get; set; } = new(); }
class Subscription { public bool Active { get; set; } = false; public string? Plan { get; set; } = null; public string? ExpiresAt { get; set; } = null; }