using System;
using System.Collections.Generic;
using Saturn.Loader.Models;

namespace Saturn.Loader.Services
{
    public class CheatService
    {
        public static CheatService Instance { get; } = new CheatService();
        public IEnumerable<Cheat> GetCheats()
        {
            yield return new Cheat { Name = "Counter-Strike 2", Expiration = DateTime.UtcNow.AddDays(16), Icon = "pack://application:,,,/Assets/cs2.png" };
        }
    }
}