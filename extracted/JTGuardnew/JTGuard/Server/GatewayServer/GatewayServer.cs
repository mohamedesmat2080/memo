using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using JTGuard.Helpers;
using JTGuard.SessionManager;
using Microsoft.Data.SqlClient;
using SilkroadSecurityAPI;
using JTGuard.AsyncServerManager;
using JTGuard.SettingManager;
using JTGuard.PacketHandlerManager;
using JTGuard.Database;
using JTGuard.Database.Model;
using JTGuard.Server.GatewayPacketHandler;
namespace JTGuard.Server
{
    public class GatewayServer : AsyncServer
    {
        public GatewayServer(_ServerSettings service) : base(service)
        {
            FakePort = service.GatewayFakePort;
            RealPort = service.GatewayRealPort;
            SessionServerType = ServerType.GatewayServer;

            var whitelist = sqlQueryHelper.GetWhitelistAsync(Program.Connectionstring, Convert.ToInt32(ServerType.GatewayServer)).Result;
            var blacklist = sqlQueryHelper.GetBlacklistAsync(Program.Connectionstring, Convert.ToInt32(ServerType.GatewayServer)).Result;

            var temp3 = new HashSet<ushort>(whitelist.Select(i => ushort.Parse(i.ToString())));
            var temp4 = new HashSet<ushort>(blacklist.Select(i => ushort.Parse(i.ToString())));

            PacketHandler = new PacketHandler(temp3, temp4);

            // ping
            PacketHandler.RegisterClientHandler(0x2002, async (packet, session, _) =>
            {
                session.LastPing = DateTime.Now;
                return new PacketResult();
            });

            PacketHandler.RegisterModuleHandler(0xA102,
             SERVER_GATEWAY_LOGIN_RESPONSE); // Automatically redirect to the AgentServer
            PacketHandler.RegisterModuleHandler(0xA100,
             SERVER_GATEWAY_PATCH_RESPONSE); // Automatically redirect to the DownloadServer
            PacketHandler.RegisterModuleHandler(0x2322,
            SERVER_GATEWAY_LOGIN_IBUV_CHALLENGE); //replacement captcha
       

            var Client = new SERVER_DLL_SETTINGS_RESPONSE(service, this, PacketHandler);
        }
        public string getMD5(string str)
        {
            MD5 mD = MD5.Create();
            byte[] array = mD.ComputeHash(Encoding.Default.GetBytes(str));
            StringBuilder stringBuilder = new StringBuilder();
            for (int i = 0; i < array.Length; i++)
            {
                stringBuilder.Append(array[i].ToString("x2"));
            }
            return stringBuilder.ToString();
        }



        public override async void AddSession(ISession session)
        {
            try
            {
                GatewaySessions.Add(session);
            }
            //Console.WriteLine(2, "session added");
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString());
            }
        }

        public override void RemoveSession(ISession session)
        {
            //Console.WriteLine("session removed");
            GatewaySessions.RemoveWhere(sess => sess.ClientGuid == session.ClientGuid);

        }
        public override void Dispose()
        {
            foreach (var gatewaySession in GatewaySessions)
            {
                gatewaySession.Stop();
            }
            base.Dispose();
        }
        private async Task<PacketResult> SERVER_GATEWAY_PATCH_RESPONSE(Packet packet, ISession session, object obj)
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
                    var bindPort = Service.DownloadFakePort;
                    var bindAddress = Program.MainMachineIP;

                    var downloadServerIp = packet.ReadAscii(); // *	string	DownloadServer.IP
                    var downloadServerPort = packet.ReadUInt16(); //2	ushort	DownloadServer.Port
                    var downloadServerCurVersion = packet.ReadUInt32(); //4	uint	DownloadServer.CurVersion


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

        private async Task<PacketResult> SERVER_GATEWAY_LOGIN_RESPONSE(Packet packet, ISession session, object obj)
        {
            var flag = packet.ReadUInt8();
            // Block everything else
            if (flag != 0x01)
                return new PacketResult();

            var token = packet.ReadUInt32();
            var remoteAddress = packet.ReadAscii(); // host
            var remotePort = packet.ReadUInt16(); // host

            new TokenProvider(session.ClientIp, remoteAddress, remotePort);
            var bindPort = 0;
            var bindAddress = string.Empty;

            bindPort = Service.AgentFakePort;
            bindAddress = Program.MainMachineIP;

            // create a new Packet and override the old client before sending to client
            var redirectPacket = new Packet(0xA102, true);
            redirectPacket.WriteUInt8(0x01);
            redirectPacket.WriteUInt32(token);
            redirectPacket.WriteAscii(bindAddress);
            redirectPacket.WriteUInt16(bindPort);


            Task.Run(async () =>
            {
                await Task.Delay(2000);
                session.Stop("blablabla");
            });
            return new PacketResult(redirectPacket, PacketResultType.Override);
        }
        private async Task<PacketResult> SERVER_GATEWAY_LOGIN_IBUV_CHALLENGE(Packet packet, ISession session, object obj)
        {

            //if remove captcha is not enabled return nothing
            if (!Service.RemoveCaptcha)
            {
                return new PacketResult();
            }

            var removePacket = new Packet(0x6323, false);
            removePacket.WriteAscii(Service.CaptchaValue);
            await session.SendToServer(removePacket);
            return new PacketResult(PacketResultType.Block);
        }


    }
}
