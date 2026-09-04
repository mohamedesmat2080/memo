#region

using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using API;
using API.Database.DuckSoup;
using API.Server;
using API.ServiceFactory;
using API.Session;
using DuckSoup.Library.Server;
using PacketLibrary;
using SilkroadSecurityAPI;

#endregion

#pragma warning disable 1998

namespace DuckSoup.Gateway
{
    public class GatewayServer : AsyncServer
    {
        public GatewayServer(Service service) : base(service)
        {
            SharedObjects = ServiceFactory.Load<ISharedObjects>(typeof(ISharedObjects));
            ServerManager = ServiceFactory.Load<IServerManager>(typeof(IServerManager));

            using (var context = new API.Database.DuckSoup.DuckSoup())
            {
                if (!context.Whitelist.Any(s => s.ServerType == ServerType.GatewayServer))
                {
                    foreach (var (key, value) in DefaultPacketlist.GatewayClientWhitelistFull)
                    {
                        context.Whitelist.Add(new Whitelist
                            {MsgId = key, Comment = value, ServerType = ServerType.GatewayServer});
                    }

                    context.SaveChanges();
                }

                // Conversion shit because Database actually only supports int not ushort sadge
                var temp1 = context.Whitelist.Where(s => s.ServerType == ServerType.GatewayServer).Select(s => s.MsgId)
                    .ToList();
                var temp2 = context.Blacklist.Where(s => s.ServerType == ServerType.GatewayServer).Select(s => s.MsgId)
                    .ToList();
                var temp3 = new HashSet<ushort>();
                var temp4 = new HashSet<ushort>();
                foreach (var i in temp1)
                {
                    temp3.Add(ushort.Parse(i.ToString()));
                }

                foreach (var i in temp2)
                {
                    temp4.Add(ushort.Parse(i.ToString()));
                }

                PacketHandler = new PacketHandler(temp3, temp4);
            }

            PacketHandler.RegisterModuleHandler(0xA102,
                SERVER_GATEWAY_LOGIN_RESPONSE); // Automatically redirect to the AgentServer
            PacketHandler.RegisterModuleHandler(0xA100,
                SERVER_GATEWAY_PATCH_RESPONSE); // Automatically redirect to the DownloadServer
            PacketHandler.RegisterModuleHandler(0xA104,
                SERVER_GATEWAY_NOTICE_RESPONSE); // Replaces the Notices
            PacketHandler.RegisterModuleHandler(0xA101,
                SERVER_SHARD_INFO); // Replaces the shard info
            PacketHandler.RegisterModuleHandler(0x2322,
                SERVER_GATEWAY_LOGIN_IBUV_CHALLENGE); //replacement captcha
        }

        private ISharedObjects SharedObjects { get; set; }
        private IServerManager ServerManager { get; set; }

        public override void AddSession(ISession session)
        {
            base.AddSession(session);
            SharedObjects.GatewaySessions.Add(session);
        }

        public override void RemoveSession(ISession session)
        {
            base.RemoveSession(session);
            if (SharedObjects.GatewaySessions.Contains(session))
            {
                SharedObjects.GatewaySessions.Remove(session);
            }
        }

        public override void Dispose()
        {
            foreach (var gatewaySession in SharedObjects.GatewaySessions)
            {
                gatewaySession.Dispose();
            }

            SharedObjects = null;
            ServerManager = null;

            base.Dispose();
        }

        private async Task<PacketResult> SERVER_GATEWAY_PATCH_RESPONSE(Packet packet, ISession session)
        {
            var overridePacket = new Packet(0xA100, packet.Encrypted, packet.Massive);
            var result = packet.ReadUInt8(); // 1	byte	result
            overridePacket.WriteUInt8(result);
            if (result == 0x02)
            {
                var errorCode = packet.ReadUInt8(); // 1	byte	errorCode
                overridePacket.WriteUInt8(errorCode);

                if (errorCode == 0x02)
                {
                    var bindPort = 0;
                    var bindAddress = "0.0.0.0";

                    var downloadServerIp = packet.ReadAscii(); // *	string	DownloadServer.IP
                    var downloadServerPort = packet.ReadUInt16(); //2	ushort	DownloadServer.Port
                    var downloadServerCurVersion = packet.ReadUInt32(); //4	uint	DownloadServer.CurVersion

                    // finds the according downloadServer to the given port and address and writes the redirect packet
                    foreach (var downloadServer in ServerManager.Servers.Where(agentServer =>
                                 agentServer.Service.RemotePort == downloadServerPort &&
                                 agentServer.Service.RemoteMachine.Address == downloadServerIp))
                    {
                        bindAddress = downloadServer.Service.LocalMachine.Address;
                        bindPort = downloadServer.Service.BindPort;

                        if (downloadServer.Service.SpoofMachine != null &&
                            downloadServer.Service.SpoofMachine.Address != "")
                        {
                            bindAddress = downloadServer.Service.SpoofMachine.Address;
                        }
                    }

                    overridePacket.WriteAscii(bindAddress);
                    overridePacket.WriteUInt16(bindPort);
                    overridePacket.WriteUInt32(downloadServerCurVersion);

                    while (true)
                    {
                        var hasEntries = packet.ReadBool(); // 1	bool hasEntries
                        overridePacket.WriteBool(hasEntries);
                        if (!hasEntries)
                            break;


                        overridePacket.WriteUInt32(packet.ReadUInt32()); //4	uint	file.ID
                        overridePacket.WriteAscii(packet.ReadAscii()); //    *	string	file.Name
                        overridePacket.WriteAscii(packet.ReadAscii()); //    *	string	file.Path
                        overridePacket.WriteUInt32(packet.ReadUInt32()); //4	uint	file.Length //in bytes
                        overridePacket.WriteBool(packet.ReadBool()); //1	bool	file.ToBePacked //into pk2
                    }
                }
            }

            return new PacketResult(overridePacket, PacketResultType.Override);
        }

        private async Task<PacketResult> SERVER_GATEWAY_LOGIN_RESPONSE(Packet packet, ISession session)
        {
            if (Library.SharedObjects.settings["IPLimit"] >0)
            {
                var ipCount = SharedObjects.AgentSessions.Count(Session => Session.ClientIp == session.ClientIp);
                if (ipCount >= Library.SharedObjects.settings["IPLimit"])
                {
                    Packet ipLimit = new Packet(0x183B);
                    ipLimit.WriteUInt8(0x01);
                    await session.SendToClient(ipLimit);
                    return new PacketResult(PacketResultType.Disconnect);
                }
            }

            var flag = packet.ReadUInt8();

            // Block everything else
            if (flag != 0x01)
                return new PacketResult();

            var token = packet.ReadUInt32();
            var remoteAddress = packet.ReadAscii(); // host
            var remotePort = packet.ReadUInt16(); // host

            var bindPort = 0;
            var bindAddress = "0.0.0.0";

            // finds the according agentServer to the given port and address and writes the redirect packet
            foreach (var agentServer in ServerManager.Servers.Where(agentServer =>
                         agentServer.Service.RemotePort == remotePort &&
                         agentServer.Service.RemoteMachine.Address == remoteAddress))
            {
                bindAddress = agentServer.Service.LocalMachine.Address;
                bindPort = agentServer.Service.BindPort;

                if (agentServer.Service.SpoofMachine != null && agentServer.Service.SpoofMachine.Address != "")
                {
                    bindAddress = agentServer.Service.SpoofMachine.Address;
                }
            }

            if (Library.SharedObjects.settings["DebugLevel"] >= DebugLevel.Debug)
                Global.Logger.DebugFormat("{0} - Redirect - source {1}:{2} - target {3}:{4}",
                    Library.SharedObjects.settings["Name"], remoteAddress, remotePort, bindAddress, bindPort);

            // create a new Packet and override the old client before sending to client
            var redirectPacket = new Packet(0xA102, true);
            redirectPacket.WriteUInt8(0x01);
            redirectPacket.WriteUInt32(token);
            redirectPacket.WriteAscii(bindAddress);
            redirectPacket.WriteUInt16((ushort) bindPort);


            Task.Run(async () =>
            {
                await Task.Delay(2000);
                session.Dispose();
            });
            return new PacketResult(redirectPacket, PacketResultType.Override);
        }

        private async Task<PacketResult> SERVER_GATEWAY_NOTICE_RESPONSE(Packet packet, ISession session)
        {
            var newPacket = new Packet(packet.Opcode, packet.Encrypted, packet.Massive);
            newPacket.WriteByte((byte) SharedObjects.Notice.Count);
            foreach (var (_, value) in SharedObjects.Notice)
            {
                newPacket.WriteAscii(value.Subject);
                newPacket.WriteAscii(value.Article);
                newPacket.WriteUInt16((ushort) value.EditDate.Year);
                newPacket.WriteUInt16((ushort) value.EditDate.Month);
                newPacket.WriteUInt16((ushort) value.EditDate.Day);
                newPacket.WriteUInt16((ushort) value.EditDate.Hour);
                newPacket.WriteUInt16((ushort) value.EditDate.Minute);
                newPacket.WriteUInt16((ushort) value.EditDate.Second);
                newPacket.WriteUInt32((uint) value.EditDate.Millisecond);
            }

            return new PacketResult(newPacket, PacketResultType.Override);
        }

        private async Task<PacketResult> SERVER_SHARD_INFO(Packet packet, ISession session)
        {
            Packet SERVER_GATEWAY_SHARD_LIST_RESPONSE = new Packet(packet.Opcode,packet.Encrypted,packet.Massive);
            byte GlobalOperationFlag = packet.ReadUInt8();
            SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt8(GlobalOperationFlag);

            while (GlobalOperationFlag == 1)
            {
                byte GlobalOperationType = packet.ReadUInt8();
                string GlobalOperationName = packet.ReadAscii();
                GlobalOperationFlag = packet.ReadUInt8();

                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt8(GlobalOperationType);
                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteAscii(GlobalOperationName);
                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt8(GlobalOperationFlag);
            }
            byte ShardFlag = packet.ReadUInt8();
            SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt8(ShardFlag);
            while (ShardFlag == 1)
            {
                uint ShardID = packet.ReadUInt16();
                string ShardName = packet.ReadAscii();
                uint ShardCurrent = packet.ReadUInt16();

                uint ShardCapacity = packet.ReadUInt16();
                byte ShardStatus = packet.ReadUInt8();
                byte GlobalOperationID = packet.ReadUInt8();

                ShardFlag = packet.ReadUInt8();

                int PlayersCount = (int)(Library.SharedObjects.settings["FakeCount"] > 0 ? Library.SharedObjects.settings["FakeCount"] + ShardCurrent : ShardCurrent);
                string OnlinePlayers = Library.SharedObjects.settings["ShowOnlinePlayers"] ? $"({PlayersCount}/{ShardCapacity})" : "";
                if (string.IsNullOrEmpty(session.ShardName))
                    session.ShardName =
                           Library.SharedObjects.settings["ShardName"] + OnlinePlayers;

                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt16(ShardID);
                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteAscii(session.ShardName);
                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt16(Library.SharedObjects.settings["FakeCount"] + ShardCurrent);


                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt16(ShardCapacity);
                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt8(ShardStatus);
                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt8(GlobalOperationID);
                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt8(ShardFlag);
            }
            return new PacketResult(SERVER_GATEWAY_SHARD_LIST_RESPONSE, PacketResultType.Override);
        }

        private async Task<PacketResult> SERVER_GATEWAY_LOGIN_IBUV_CHALLENGE(Packet packet, ISession session)
        {
            // if remove captcha is not enabled return nothing
            if (!Library.SharedObjects.settings["RemoveCaptcha"]) return new PacketResult();

            // create new packet, write it the captcha code, send it to the server and block the packet to the client
            var removePacket = new Packet(0x6323, false);
            removePacket.WriteAscii(Library.SharedObjects.settings["ReplacementCaptcha"]);
            await session.SendToServer(removePacket);

            return new PacketResult(PacketResultType.Block);
        }
    }
}