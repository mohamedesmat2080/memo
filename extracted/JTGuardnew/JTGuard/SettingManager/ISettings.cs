namespace JTGuard.SettingManager;

public interface ISettings : IDisposable
{
    string Address { get; set; }
    int? Port { get; set; }
    string Username { get; set; }
    string Password { get; set; }
    string ProxyDb { get; set; }
    int MaximumPool { get; set; }
    int MinimumPool { get; set; }
    int ConnectionLifetime { get; set; }
    string ServerIP { get; set; }
    ISettings Init();
}