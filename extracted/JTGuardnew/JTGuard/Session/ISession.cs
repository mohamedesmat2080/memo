using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using JTGuard.AsyncServerManager;
using SilkroadSecurityAPI;

namespace JTGuard.SessionManager
{
    public interface ISession
    {
        IAsyncServer AsyncServer { get; init; }
        Packet TempPacket { get; set; }
        Guid ClientGuid { get; set; }
        string ClientIp { get; set; }
        Task SendToClient(Packet packet);
        Task SendToServer(Packet packet);
        Task SendNotice(string message);
        Task Start();
        void Stop();
        void Stop(string reason);
        #region Features
        string PlayerUserID { get; set; }
        bool CharacterGameReady { get; set; }
        ISessionData SessionData { get; init; }
        #endregion

        #region Protection

        // Packet Modification
        int PacketLength { get; set; }
        // False Packets
        bool CharnameSent { get; set; }
        bool CharScreen { get; set; }
        bool UserLoggedIn { get; set; }
        DateTime LastPing { get; set; }
        #endregion
    }
}
