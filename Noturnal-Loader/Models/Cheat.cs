using System;

namespace Noturnal.Loader.Models
{
    public class Cheat
    {
        public string Name { get; set; } = "";
        public string Icon { get; set; } = "";
        public DateTime Expiration { get; set; } = DateTime.UtcNow.AddDays(7);
        public int RemainingDays => (int)Math.Max(0, (Expiration.Date - DateTime.UtcNow.Date).TotalDays);
    }
}
