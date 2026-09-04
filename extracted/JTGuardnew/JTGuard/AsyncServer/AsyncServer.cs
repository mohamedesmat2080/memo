using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using static System.Collections.Specialized.BitVector32;
using JTGuard.PacketHandlerManager;
using JTGuard.Helpers;
using JTGuard.SessionManager;
using JTGuard.SettingManager;
using JTGuard.Database.Model;
using JTGuard.Database.Models;
using JTGuard.ServerManagers.Events;

namespace JTGuard.AsyncServerManager
{
    public class AsyncServer : IAsyncServer
    {
        public bool Exit { get; set; }
        public int RealPort { get; set; }
        public int FakePort { get; set; }
        public bool Started { get; set; } = false;
        public TcpListener _tcpServer { get; set; }
        public IPEndPoint RemoteEndPoint { get; set; }
        public IPacketHandler PacketHandler { get; set; }
        public HashSet<ISession> AgentSessions { get; set; }
        public HashSet<ISession> DownloadSessions { get; set; }
        public HashSet<ISession> GatewaySessions { get; set; }
       
        public ServerType SessionServerType { get; set; }

        public _ServerSettings Service { get; init; }
        public int LastPartyMatchingID { get; set; } = 0;
        public Dictionary<byte, _RefHWANLevel> RefHwan { get; set; }

        public EventManager eventManager { get; set; }
        protected AsyncServer(_ServerSettings service)
        {
            Service = service;
        }
        private bool IsPortAvailable(int port)
        {
            try
            {
                using (var socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp))
                {
                    socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
                    socket.Bind(new IPEndPoint(IPAddress.Loopback, port));
                    return true; // Port kullanılabilir
                }
            }
            catch (SocketException)
            {
                return false; // Port kullanımda
            }
        }

        public async Task Start()
        {
            try
            {

                _tcpServer?.Stop();
                AgentSessions?.Clear();
                DownloadSessions?.Clear();
                GatewaySessions?.Clear();


                AgentSessions = new HashSet<ISession>();
                DownloadSessions = new HashSet<ISession>();
                GatewaySessions = new HashSet<ISession>();
                // defines a binding endpoint
                var bindEndPoint = new IPEndPoint(IPAddress.Parse(Program.MainMachineIP), FakePort);
                RemoteEndPoint = new IPEndPoint(IPAddress.Parse(Service.ServerIP), RealPort);

                // starts the listener socket for the incoming connections
                _tcpServer = new TcpListener(bindEndPoint);
                _tcpServer.Start();

                Program.PrintInColor($"{SessionServerType} : [F:{Program.MainMachineIP}] => [R:{Service.ServerIP}] : [F:{RealPort}] => [R:{FakePort}]", ConsoleColor.Red);
                Started = true;
                // accepts client connections
                while (!Exit)
                {
                    await _tcpServer.AcceptTcpClientAsync().ContinueWith(OnAccept, TaskScheduler.Default);
                    await Task.Delay(3);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Asyncserver + {ex.Message.ToString()}");
            }

        }
        public void Stop()
        {
            try
            {
                // Duran TcpListener'ı kapat


                // Bağlı olan tüm TcpClient nesnelerini kapat
                if (AgentSessions.Any())
                {
                    foreach (var client in AgentSessions)
                    {
                        client.Stop(); // Asenkron işlemi bekleyerek durdur
                    }
                }

                if (DownloadSessions.Any())
                {
                    foreach (var client in DownloadSessions)
                    {
                        client.Stop(); // Asenkron işlemi bekleyerek durdur
                    }
                }

                if (GatewaySessions.Any())
                {
                    foreach (var client in GatewaySessions)
                    {
                        client.Stop(); // Asenkron işlemi bekleyerek durdur
                    }
                }
                _tcpServer?.Stop();
                Started = false;
                Exit = true;

                // Diğer temizleme işlemleri...

                //Console.WriteLine($"UserID = {Service.UserID} is stopped");
            }
            catch (Exception ex)
            {
                //Console.WriteLine($"Error stopping server for UserID = {Service.UserID}: {ex.Message}");
            }
        }
        public virtual void Dispose()
        {
            Exit = true;
            _tcpServer?.Stop();
        }
        public async Task OnAccept(Task<TcpClient> task)
        {

            var tcpClient = await task;
            var clientSession = new Session(tcpClient, this, SessionServerType);
            AddSession(clientSession);
            await clientSession.Start();
        }

        public virtual void RemoveSession(ISession session)
        {
        }

        public virtual void AddSession(ISession session)
        {

        }


        public async Task<int> GetHWIDCount(string hwid)
        {
            int count = 0;
            foreach (var x in AgentSessions)
            {
                if (x.SessionData.Hwid == hwid)
                    count++;
            }
            return count;
        }

    }
}
