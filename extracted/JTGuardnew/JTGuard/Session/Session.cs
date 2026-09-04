using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using JTGuard.AsyncServerManager;
using JTGuard.Helpers;
using JTGuard.PacketHandlerManager;
using SilkroadSecurityAPI;
using System.Net.Sockets;
using System.Net;
using JTGuard.AsyncServerManager;

namespace JTGuard.SessionManager
{
    public sealed class Session : ISession
    {
        private readonly byte[] _clientBuffer = new byte[4096];
        private readonly Security _clientSecurity = new();

        private readonly byte[] _serverBuffer = new byte[4096];
        private readonly Security _serverSecurity = new();
        private TcpClient _clientTcpClient;
        private bool _exit;
        private TcpClient _serverTcpClient;
        private readonly bool _isTryCatchDebug;
        private ServerType SessionServerType { get; }

        public Session(TcpClient clientTcpClient, IAsyncServer asyncServer, ServerType servertype)
        {
            SessionServerType = servertype;
            AsyncServer = asyncServer;
            SessionData = new SessionData();
            _clientTcpClient = clientTcpClient;

            // generates a "unique" id from the address and port hashcode and safes ip
            if (!(_clientTcpClient.Client.RemoteEndPoint is IPEndPoint ep)) return;
            ClientGuid = Guid.NewGuid();
            ClientIp = ep.Address.ToString();
            //Console.WriteLine($"{ClientIp}");

        }

        public IAsyncServer AsyncServer { get; init; }
        public ISessionData SessionData { get; init; }

        public Guid ClientGuid { get; set; }
        public string ClientIp { get; set; }
        public int ConnectedServerID { get; set; }
        public bool SecondaryCodeEntered { get; set; } = false;

        public Packet TempPacket { get; set; }
   
        public void Stop()
        {
            Stop("Unknown reason");
        }

        public void Stop(string reason)
        {

            // double socket close prevention
            if (_exit) return;
            _exit = true;

            // removes the session from the session list - the function has a contains check
            AsyncServer.RemoveSession(this);

            _clientTcpClient?.Close();
            _clientTcpClient = null;
            _serverTcpClient?.Close();
            _serverTcpClient = null;

        }

        public async Task SendToClient(Packet packet)
        {
            if (_clientTcpClient == null || _serverTcpClient == null || _exit)
                return;

            try
            {
                _clientSecurity.Send(packet);
                // probably not needed tho since we're in a permanent circle anyways
                // might be the reason login and so on is FUCKING slow
                await TransferToClient();
            }
            catch (Exception)
            {
                Stop("send to client");
            }
        }

        public async Task SendToServer(Packet packet)
        {
            if (_clientTcpClient == null || _serverTcpClient == null || _exit)
                return;

            try
            {
                _serverSecurity.Send(packet);
                // probably not needed tho since we're in a permanent circle anyways
                // might be the reason login and so on is FUCKING slow
                await TransferToServer();
            }
            catch (Exception)
            {
                Stop("send to server");
            }
        }

        public async Task SendNotice(string message)
        {
            var notice = new Packet(0x3026, false, false);
            notice.WriteByte(7);
            notice.WriteAscii(message);
            await SendToClient(notice);
        }

        public async Task Start()
        {

            _clientSecurity.GenerateSecurity(true, true, true);
            // creates a new server socket and connects it according to the remote addr. and port 
            //_serverTcpClient = new TcpClient(new IPEndPoint(IPAddress.Parse("78.135.85.104"), 0));

            TokenProvider token = TokenProvider.GetToken(ClientIp);
            if (SessionServerType == ServerType.AgentServer)
            {
                if (token != null)
                {
                    _serverTcpClient = new TcpClient(new IPEndPoint(IPAddress.Parse(Program.MainMachineIP), 0));

                    await _serverTcpClient.ConnectAsync(token.AgentIP, token.AgentPort);
                }
                else
                {
                    AsyncServer.RemoveSession(this);
                }
            }
            else
            {
                _serverTcpClient = new TcpClient();

                await _serverTcpClient.ConnectAsync(AsyncServer.RemoteEndPoint.Address, AsyncServer.RemoteEndPoint.Port);

            }

            // Just making sure to disconnect clients that have fucked up
            _ = Task.Factory.StartNew(() =>
            {
                Thread.Sleep(500);
                if (_serverTcpClient is not { Connected: true } || _clientTcpClient is not { Connected: true })
                    Stop("500 ms");

                return Task.CompletedTask;
            });

            // starts receiving loop from server + client - if a destroy was called the loop brakes
            _ = Task.Factory.StartNew(async () =>
            {
                while (!_exit)
                    await DoReceiveFromServer();

                _serverTcpClient?.Close();
                _serverTcpClient = null;
            }, CancellationToken.None, TaskCreationOptions.LongRunning, TaskScheduler.Default);

            _ = Task.Factory.StartNew(async () =>
            {
                while (!_exit)
                    await DoReceiveFromClient();

                _clientTcpClient?.Close();
                _clientTcpClient = null;
            }, CancellationToken.None, TaskCreationOptions.LongRunning, TaskScheduler.Default);
        }

        private async Task DoReceiveFromClient()
        {
            try
            {
                // receive stuff
                var clientBufferMemory = new Memory<byte>(_clientBuffer);
                var recvCount = await _clientTcpClient.GetStream().ReadAsync(clientBufferMemory);

                if (recvCount == 0)
                {
                    Stop("receive count 0");
                    return;
                }

                // starts receiving again
                _clientSecurity.Recv(_clientBuffer, 0, recvCount);

                // transfers the incoming packets to a list
                var receivedPackets = _clientSecurity.TransferIncoming();

                // if there are no packets start receiving again
                if (receivedPackets == null)
                {
                    await TransferToServer();
                    return;
                }


                // loop through all received packets
                foreach (var packet in receivedPackets)
                {
                    //Console.WriteLine(2, $"Client Packets {packet.Opcode}");
                    // ignore handshake
                    if (packet.Opcode == 0x9000 || packet.Opcode == 0x5000 || packet.Opcode == 0x2001)
                        continue;
                    //WriteLine(1, $"Client Packet {packet.Opcode.ToString("X")}");
                    #region Protection

                    // Packet Modifying
                    PacketLength = packet.GetBytes().Length;

                    // Packet Flooding
                    // calc the last checktime
                    var lastCheckDiff = (DateTime.Now - _lastPacketReset).TotalSeconds;
                    // lastcheckdiff * bytelimitation for exact bytes per time - don't need to round it to one second
                    var maxBytesPerTime = lastCheckDiff * /*AsyncServer.Service.ByteLimitation */2048;
                    // if the packetsize exceeded the calculated value and it was measured over 2 second (to prevent super random lag dcs)
                    if (_packetSize > maxBytesPerTime && lastCheckDiff > 2.0)
                    {

                        Stop("byte limit");
                    }
                    else if (lastCheckDiff > 1)
                    {
                        // else reset
                        _lastPacketReset = DateTime.Now;
                        _packetSize = 0;
                    }

                    #endregion

                    PacketResult packetResult;
                    if (_isTryCatchDebug)
                    {
                        try
                        {
                            packetResult = await AsyncServer.PacketHandler.HandleClient(packet, this);
                        }
                        catch (Exception e)
                        {
                            Console.WriteLine(e.Message.ToString());
                            packetResult = new PacketResult(PacketResultType.Block);
                        }
                    }
                    else
                    {
                        packetResult = await AsyncServer.PacketHandler.HandleClient(packet, this);
                    }

                    switch (packetResult.PacketResultType)
                    {
                        case PacketResultType.Override:
                            _serverSecurity.Send(packetResult.OverridePacket);
                            break;
                        case PacketResultType.Block:
                            break;
                        case PacketResultType.Disconnect:
                            Stop("receive from client disconnect");
                            break;
                        case PacketResultType.Nothing:
                            _serverSecurity.Send(packet);
                            break;
                        default:
                            throw new ArgumentOutOfRangeException();
                    }
                }

                // transfers packets to the server and starts receiving from client again
                await TransferToServer();
            }
            catch (Exception e)
            {
                Stop("receive from client catch " + e?.Message + "\n" + e?.InnerException?.Message);
            }
        }

        private async Task DoReceiveFromServer()
        {
            try
            {
                // receive stuff
                var serverBufferMemory = new Memory<byte>(_serverBuffer);
                var recvCount = await _serverTcpClient.GetStream().ReadAsync(serverBufferMemory);
                if (recvCount == 0)
                {
                    Stop("receive from server disconnect");
                    return;
                }

                // starts receiving again
                _serverSecurity.Recv(_serverBuffer, 0, recvCount);

                // transfers the incoming packets to a list
                var receivedPackets = _serverSecurity.TransferIncoming();

                // if there are no packets start receiving again
                if (receivedPackets == null)
                {
                    await TransferToClient();
                    return;
                }

                // loop through all received packets
                foreach (var packet in receivedPackets)
                {
                    //Console.WriteLine(2, "Opcode:{Iwa0x" + packet.Opcode.ToString("X") + "}");
                    // ignore handshake
                    if (packet.Opcode == 0x5000 || packet.Opcode == 0x9000)
                        continue;
                    // debug
                    //WriteLine(1, $"Server Packet {packet.Opcode.ToString("X")}");
                    PacketResult packetResult;
                    if (_isTryCatchDebug)
                    {
                        try
                        {
                            packetResult = await AsyncServer.PacketHandler.HandleServer(packet, this);
                        }
                        catch (Exception e)
                        {
                            Console.WriteLine(e.Message.ToString());
                            packetResult = new PacketResult(PacketResultType.Block);
                        }
                    }
                    else
                    {
                        packetResult = await AsyncServer.PacketHandler.HandleServer(packet, this);
                    }

                    switch (packetResult.PacketResultType)
                    {
                        case PacketResultType.Override:
                            _clientSecurity.Send(packetResult.OverridePacket);
                            break;
                        case PacketResultType.Block:
                            break;
                        case PacketResultType.Disconnect:
                            Stop("receive from server disconnect");
                            break;
                        case PacketResultType.Nothing:
                            _clientSecurity.Send(packet);
                            break;
                        default:
                            throw new ArgumentOutOfRangeException();
                    }
                }

                // transfers packets to the client and starts receiving from server again
                await TransferToClient();
            }
            catch (Exception e)
            {
                Stop("receive from server catch " + e.Message + "\n" + e.InnerException?.Message);
            }
        }

        private async Task TransferToClient()
        {
            if (_exit) return;
            try
            {
                var kvp = _clientSecurity.TransferOutgoing();

                if (kvp == null) return;

                foreach (var t in kvp)
                    //transfers the client packets to the client
                    await _clientTcpClient.GetStream().WriteAsync(t.Key.Buffer, 0, t.Key.Buffer.Length);
            }
            catch (Exception)
            {
                Stop("transfer to client catch");
            }
        }

        private async Task TransferToServer()
        {
            //if (_exit) return;

            try
            {
                var kvp = _serverSecurity.TransferOutgoing();
                if (kvp == null) return;
                foreach (var t in kvp)
                {
                    if (t.Key == null || t.Key.Buffer == null)
                    {
                        Console.WriteLine("Key or Buffer is null");
                        continue;
                    }
                    _packetSize += t.Key.Buffer.Length;
                    //transfers the server packets to the server
                    if (_serverTcpClient?.GetStream() != null)
                    {
                        await _serverTcpClient.GetStream().WriteAsync(t.Key.Buffer, 0, t.Key.Buffer.Length);
                    }
                    else
                    {
                        Console.WriteLine("_serverTcpClient or its stream is null");
                    }
                }
            }
            catch (Exception ex)
            {
                Stop("transfer to server catch");
                Console.WriteLine(ex.Message.ToString());
            }
        }



        #region Features

        //public ITimerManager TimerManager { get; set; }
        //public ICountdownManager CountdownManager { get; set; }
        //public ICharInfo CharInfo { get; init; }
        public string PlayerUserID { get; set; }
        public bool CharacterGameReady { get; set; } = false;
        //public ISessionData SessionData { get; init; }

        #endregion

        #region Protection

        // Packet Modification
        public int PacketLength { get; set; }

        // Packet Flooding
        private int _packetSize;
        private DateTime _lastPacketReset;

        // timer
        public DateTime LastPing { get; set; } = DateTime.Now;

        // False Packets
        public bool CharnameSent { get; set; } = false;
        public bool CharScreen { get; set; } = false;
        public bool UserLoggedIn { get; set; } = false;

        #endregion

    }
}
