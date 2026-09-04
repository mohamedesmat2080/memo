<<<<<<< ours
﻿namespace PacketLibrary.Enums.Agent.Logout;
=======
namespace PacketLibrary.Enums.Agent.Logout;
>>>>>>> theirs

// https://github.com/DummkopfOfHachtenduden/SilkroadDoc/wiki/LogoutMode
public enum LogoutMode : byte
{
    /// <summary>
    /// Go to Process.CPSQuit
    /// </summary>
    Exit = 1, 
    
    /// <summary>
    /// Go to Process.CPSRestart
    /// </summary>
    Restart = 2,
}