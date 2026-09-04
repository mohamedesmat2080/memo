

namespace JTGuard.SettingManager;

public interface ISettingsManager : IDisposable
{
    ISettings Settings { get; }
}