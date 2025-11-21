namespace Noturnal.Loader.Services
{
    public static class SecurityConfig
    {
        public static readonly string PinnedPublicKeyHash = ""; // SHA-256 hex da chave pública do servidor
        public static readonly string ServerHost = "localhost";
        public static readonly string PublicKeyPem = ""; // PEM para verificar assinaturas
    }
}