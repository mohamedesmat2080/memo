<<<<<<< ours
﻿using McMaster.NETCore.Plugins;
=======
using McMaster.NETCore.Plugins;
>>>>>>> theirs

namespace API.Event;

public interface IEventManager : IDisposable
{
    Dictionary<PluginLoader, IEvent> Loaders { get; }

    bool IsLoaded(string name);
    PluginLoader LoadEvent(string file);
    IEvent StartEvent(PluginLoader pluginLoader);
    bool UnloadEvent(string name);
    bool UnloadEvent(IEvent eEvent);
    bool UnloadEvent(PluginLoader pluginLoader);
    List<string> GetFilesInDirectory(string directory);
    string? SearchEvent(string directory, string eventName);
}