using Saturn.Loader.Models;

namespace Saturn.Loader.Services
{
    public class AuthService
    {
        public static AuthService Instance { get; } = new AuthService();
        public bool DevBuild { get; set; } = true;
        public User? Login(string username, string password)
        {
            if (DevBuild && username == "admin" && password == "admin")
                return new User { Username = username, DisplayName = "Admin" };
            return null;
        }
    }
}