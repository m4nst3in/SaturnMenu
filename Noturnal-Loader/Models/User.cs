namespace Noturnal.Loader.Models
{
    public class User
    {
        public string Id { get; set; } = "";
        public string Username { get; set; } = "Guest";
        public string DisplayName { get; set; } = "Guest";
        public string Email { get; set; } = "";
        public string? Hwid { get; set; }
        public Subscription Subscription { get; set; } = new Subscription();
    }

    public class Subscription
    {
        public bool Active { get; set; } = false;
        public string? Plan { get; set; }
        public string? ExpiresAt { get; set; }
    }
}
