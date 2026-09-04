using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using JTGuard.Helpers;
using JTGuard.PacketHandlerManager;
using JTGuard.SessionManager;
using JTGuard.Database.Model;
using JTGuard.Database.Models;
using JTGuard.ServerManagers.Events;
namespace JTGuard.AsyncServerManager
{
    public interface IAsyncServer : IDisposable
    {
        _ServerSettings Service { get; init; }
        EventManager eventManager { get; set; }
        Dictionary<byte, _RefHWANLevel> RefHwan { get; set; }

        int LastPartyMatchingID { get; set; }
        bool Exit { get; set; }
        bool Started { get; set; }
        ServerType SessionServerType { get; set; }
        TcpListener _tcpServer { get; set; }
        HashSet<ISession> AgentSessions { get; }
        HashSet<ISession> DownloadSessions { get; }
        HashSet<ISession> GatewaySessions { get; }
        IPacketHandler PacketHandler { get; set; }
        IPEndPoint RemoteEndPoint { get; set; }
        void AddSession(ISession session);
        void RemoveSession(ISession session);
        Task Start();
        void Stop();
        Task OnAccept(Task<TcpClient> task);
        public Task<int> GetHWIDCount(string hwid);
    }
}
