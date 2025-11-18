using System;
using System.Collections.Generic;

namespace SaturnLoader
{
    public class User
    {
        public string Username { get; set; } = string.Empty;
        public string Email { get; set; } = string.Empty;
        public string AvatarUrl { get; set; } = string.Empty;
        public List<Subscription> Subscriptions { get; set; } = new();
        public DateTime CreatedAt { get; set; }
        public bool IsAdmin { get; set; }
    }

    public class Subscription
    {
        public string Id { get; set; } = Guid.NewGuid().ToString();
        public string Name { get; set; } = string.Empty;
        public string Game { get; set; } = string.Empty;
        public DateTime ExpiresAt { get; set; }
        public bool IsActive => ExpiresAt > DateTime.Now;
        public string Status => IsActive ? "Active" : "Expired";
        public string TimeRemaining
        {
            get
            {
                if (!IsActive) return "Expired";
                var timeLeft = ExpiresAt - DateTime.Now;
                if (timeLeft.Days > 0) return $"Expires in {timeLeft.Days} days";
                if (timeLeft.Hours > 0) return $"Expires in {timeLeft.Hours} hours";
                return $"Expires in {timeLeft.Minutes} minutes";
            }
        }
        public string IconColor { get; set; } = "#FF2A4BFF";
    }

    public class CheatDetails
    {
        public string Name { get; set; } = string.Empty;
        public string Version { get; set; } = string.Empty;
        public string Description { get; set; } = string.Empty;
        public DateTime LastUpdate { get; set; }
        public string Status { get; set; } = string.Empty;
        public bool IsDetected { get; set; }
        public string[] Features { get; set; } = Array.Empty<string>();
    }

    public class LogEntry
    {
        public DateTime Timestamp { get; set; }
        public string Level { get; set; } = string.Empty;
        public string Message { get; set; } = string.Empty;
        public string Category { get; set; } = string.Empty;
    }
}