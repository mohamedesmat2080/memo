using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using JTGuard.AsyncServerManager;
using JTGuard.Database;
using JTGuard.Database.Model;
using JTGuard.Helpers;
using JTGuard.PacketHandlerManager;
using JTGuard.SessionManager;
using Microsoft.Data.SqlClient;

namespace JTGuard.Server.DownloadServer
{
    public class DownloadServer : AsyncServer
    {
        public DownloadServer(_ServerSettings service) : base(service)
        {
            FakePort = service.DownloadFakePort;
            RealPort = service.DownloadRealPort;
            SessionServerType = ServerType.DownloadServer;

            var whitelist = sqlQueryHelper.GetWhitelistAsync(Program.Connectionstring, Convert.ToInt32(ServerType.DownloadServer)).Result;
            var blacklist = sqlQueryHelper.GetBlacklistAsync(Program.Connectionstring, Convert.ToInt32(ServerType.DownloadServer)).Result;

            var temp3 = new HashSet<ushort>(whitelist.Select(i => ushort.Parse(i.ToString())));
            var temp4 = new HashSet<ushort>(blacklist.Select(i => ushort.Parse(i.ToString())));

            PacketHandler = new PacketHandler(temp3, temp4);

            // ping
            PacketHandler.RegisterClientHandler(0x2002, async (packet, session, _) =>
            {
                session.LastPing = DateTime.Now;
                return new PacketResult();
            });
        }
        public override void AddSession(ISession session)
        {
            DownloadSessions.Add(session);
        }

        public override void RemoveSession(ISession session)
        {
            DownloadSessions.RemoveWhere(sess => sess.ClientGuid == session.ClientGuid);
        }

        public override void Dispose()
        {
            foreach (var downloadSession in DownloadSessions)
            {
                downloadSession.Stop();
            }
            base.Dispose();
        }
    }
}
