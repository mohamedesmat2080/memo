#region

using System.IO;
using JTGuard.SettingManager;
using Newtonsoft.Json;
using Formatting = Newtonsoft.Json.Formatting;

#endregion

namespace JTGuard.SettingManager;

public class SettingsManager : ISettingsManager
{
    public SettingsManager()
    {
        if (!File.Exists(@"./Settings.json"))
        {
            Settings = new Settings().Init();
            File.WriteAllText(@"./Settings.json",
                JsonConvert.SerializeObject(Settings, Formatting.Indented));
            Console.WriteLine("Settings.json created. Please stop the Proxy and configure the file.");
        }

        // if a config file exists deserialize and initialize it
        Settings = JsonConvert.DeserializeObject<Settings>(File.ReadAllText(@"./Settings.json"));
    }

    public ISettings Settings { get; private set; }

    public void Dispose()
    {
        Settings = null;
    }
}