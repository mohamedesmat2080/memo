namespace JTGuard.SettingManager;

public class Settings : ISettings
{
    public string Address { get; set; }
    public int? Port { get; set; }
    public string Username { get; set; }
    public string Password { get; set; }
    public string ProxyDb { get; set; }
    public int MaximumPool { get; set; }
    public int MinimumPool { get; set; }
    public int ConnectionLifetime { get; set; }
    public string ServerIP { get; set; }
    public ISettings Init()
    {
        Address = "0.0.0.0";
        Username = "sa";
        Password = "1234";
        ProxyDb = "VFilterMain";
        MaximumPool = 1500;
        MinimumPool = 50;
        ConnectionLifetime = 0;
        ServerIP = "192.168.1.1";
        return this;
    }

    public void Dispose()
    {
        Address = null;
        Port = null;
        Username = null;
        Password = null;
        ProxyDb = null;
    }
}