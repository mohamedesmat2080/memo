using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Common;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Dapper;
using JTGuard.AsyncServerManager;
using JTGuard.Database;
using JTGuard.Database.Model;
using JTGuard.Database.Models;
using JTGuard.Helpers;
using JTGuard.PacketHandlerManager;
using JTGuard.Server.AgentPacketHandler;
using JTGuard.ServerManagers;
using JTGuard.ServerManagers.Events;
using JTGuard.SessionManager;
using Microsoft.Data.SqlClient;
using SilkroadSecurityAPI;


namespace JTGuard.Server
{
    public class AgentServer : AsyncServer
    {
        public _ServerSettings Service { get; set; }
        public DatabaseCommands dbCommands { get; set; }
        public AgentServer(_ServerSettings service) : base(service)
        {
            Service = service;
            FakePort = service.AgentFakePort;
            RealPort = service.AgentRealPort;
            SessionServerType = ServerType.AgentServer;

            RefHwan = new();

            LoadRefHwan();
            var whitelist = sqlQueryHelper.GetWhitelistAsync(Program.Connectionstring, Convert.ToInt32(ServerType.AgentServer)).Result;
            var blacklist = sqlQueryHelper.GetBlacklistAsync(Program.Connectionstring, Convert.ToInt32(ServerType.AgentServer)).Result;

            var temp3 = new HashSet<ushort>(whitelist.Select(i => ushort.Parse(i.ToString())));
            var temp4 = new HashSet<ushort>(blacklist.Select(i => ushort.Parse(i.ToString())));

            PacketHandler = new PacketHandler(temp3, temp4);

            // ping
            PacketHandler.RegisterClientHandler(0x2002, async (packet, session, _) =>
            {
                session.LastPing = DateTime.Now;
                return new PacketResult();
            });

            PacketHandler.RegisterModuleHandler(0x3013, SERVER_AGENT_CHARACTER_DATA); // Character Spawn Packet
            var Client = new ExploitFixPackets(service, this, PacketHandler);
            var ClientPacket = new CustomUIPackets(service, this, PacketHandler);
            var GS = new CustomGameServerPacketHandler(service, this, PacketHandler);
            var Alchemy = new AlchemyPackets(service, this, PacketHandler);
            dbCommands = new DatabaseCommands(service, this);
            var COS = new COSPackets (service, this, PacketHandler);
            var Def = new DefaultPackets(service, this, PacketHandler);

            eventManager = new EventManager(service, this);

        }

        public async Task BroadcastPacketbyWorldIDAndLayerID(int WorldID, int LayerID, Packet packet, bool clientIsReady = true)
        {
            await Task.Run(() =>
            {
                if (AgentSessions != null)
                {
                    foreach (var targetSession in AgentSessions)
                    {
                        if (clientIsReady && !targetSession.CharacterGameReady)
                        {
                            continue;
                        }

                        if (targetSession.SessionData.WorldID == WorldID && targetSession.SessionData.WorldLayerID == LayerID)
                        {
                            targetSession.SendToClient(packet);
                        }
                    }
                }

            });
        }

        public async Task BroadcastPacket(Packet packet, ServerType serverType = ServerType.AgentServer, bool clientIsReady = true)
        {
            await Task.Run(() =>
            {
                if (AgentSessions != null)
                {
                    foreach (var session in AgentSessions)
                    {
                        session.SendToClient(packet);
                    }
                }

            });
        }
        public async Task BroadcastPacketToCharName(string CharName, Packet packet, bool clientIsReady = true)
        {
            await Task.Run(() =>
            {
                if (AgentSessions != null)
                {
                    foreach (var targetSession in AgentSessions)
                    {
                        if (clientIsReady && !targetSession.CharacterGameReady)
                        {
                            continue;
                        }

                        if (targetSession.SessionData.Charname == CharName)
                        {
                            targetSession.SendToClient(packet);
                        }
                    }
                }

            });
        }
        public async Task BroadcastPacketbyWorldID(int WorldID, Packet packet, bool clientIsReady = true)
        {
            await Task.Run(() =>
            {
                if (AgentSessions != null)
                {
                    foreach (var targetSession in AgentSessions)
                    {
                        if (clientIsReady && !targetSession.CharacterGameReady)
                        {
                            continue;
                        }

                        if (targetSession.SessionData.WorldID == WorldID)
                        {
                            targetSession.SendToClient(packet);
                        }
                    }
                }

            });
        }
        public async Task BroadcastPacketbyRegionID(int Region, Packet packet, bool clientIsReady = true)
        {
            await Task.Run(() =>
            {
                if (AgentSessions != null)
                {
                    foreach (var targetSession in AgentSessions)
                    {
                        if (clientIsReady && !targetSession.CharacterGameReady)
                        {
                            continue;
                        }

                        if (targetSession.SessionData.LatestRegion == Region)
                        {
                            targetSession.SendToClient(packet);
                        }
                    }
                }

            });
        }
        public async Task<bool> TryGetItemInfoAsync(SItemInfoDbRecord stResult, int nCharID, byte btSlotIndex)
        {
            string query = "EXEC _GetItemInfo @CharID, @SlotIndex, @RefItemID OUTPUT, @OptLevel OUTPUT, @CodeName OUTPUT, @ItemDBID OUTPUT, @AdvOptLevel OUTPUT";

            using (var connection = new SqlConnection(Program.Connectionstring))
            {
                using (var command = new SqlCommand(query, connection))
                {
                    command.Parameters.AddWithValue("@CharID", nCharID);
                    command.Parameters.AddWithValue("@SlotIndex", btSlotIndex);

                    var refItemIDParam = new SqlParameter("@RefItemID", SqlDbType.Int) { Direction = ParameterDirection.Output };
                    var optLevelParam = new SqlParameter("@OptLevel", SqlDbType.TinyInt) { Direction = ParameterDirection.Output };
                    var codeNameParam = new SqlParameter("@CodeName", SqlDbType.VarChar, 128) { Direction = ParameterDirection.Output };
                    var itemDBIDParam = new SqlParameter("@ItemDBID", SqlDbType.Int) { Direction = ParameterDirection.Output };
                    var advOptLevelParam = new SqlParameter("@AdvOptLevel", SqlDbType.TinyInt) { Direction = ParameterDirection.Output };

                    command.Parameters.Add(refItemIDParam);
                    command.Parameters.Add(optLevelParam);
                    command.Parameters.Add(codeNameParam);
                    command.Parameters.Add(itemDBIDParam);
                    command.Parameters.Add(advOptLevelParam);

                    //Console.WriteLine("Query: " + query);
                    //Console.WriteLine("Parameters: CharID={0}, SlotIndex={1}", nCharID, btSlotIndex);

                    await connection.OpenAsync();
                    //Console.WriteLine("Bağlantı başarıyla açıldı.");

                    await command.ExecuteNonQueryAsync();

                    stResult.nRefItemID = (int)refItemIDParam.Value;
                    stResult.btOptLevel = (byte)optLevelParam.Value;
                    stResult.szCodeName = (string)codeNameParam.Value;
                    stResult.nItemDBID = (int)itemDBIDParam.Value;
                    stResult.btAdvOptLevel = (byte)advOptLevelParam.Value;

                    //Console.WriteLine("Results: nRefItemID={0}, btOptLevel={1}, szCodeName={2}, nItemDBID={3}, btAdvOptLevel={4}",
                    //    stResult.nRefItemID, stResult.btOptLevel, stResult.szCodeName, stResult.nItemDBID, stResult.btAdvOptLevel);

                    // Değerlerin null olup olmadığını kontrol etme
                    if (refItemIDParam.Value != DBNull.Value && optLevelParam.Value != DBNull.Value &&
                        codeNameParam.Value != DBNull.Value && itemDBIDParam.Value != DBNull.Value &&
                        advOptLevelParam.Value != DBNull.Value)
                    {
                        //Console.WriteLine("Tüm parametreler başarılı şekilde dolduruldu.");
                        return true;
                    }
                    else
                    {
                        //Console.WriteLine("Bir veya daha fazla parametre null değeri döndü.");
                        return false;
                    }
                }
            }
        }


        public async void UpdateTimers(object state)
        {
            // CreatedTimerListWorldID koleksiyonunu kontrol et
            foreach (var key in eventManager.CreatedTimerListWorldID.Keys)
            {
                if (eventManager.CreatedTimerListWorldID.TryGetValue(key, out int remainingTime))
                {
                    remainingTime -= 1000; // 1 saniye eksilt
                    if (remainingTime <= 0)
                    {
                        eventManager.CreatedTimerListWorldID.TryRemove(key, out _); // Süresi dolanları kaldır
                        Packet timer = new Packet(0x220A);
                        timer.WriteUInt8(1);
                        await BroadcastPacketbyWorldID(key, timer);
                    }
                    else
                    {
                        eventManager.CreatedTimerListWorldID[key] = remainingTime;
                    }
                }
            }

            // CreatedTimerListRegionID koleksiyonunu kontrol et
            foreach (var key in eventManager.CreatedTimerListRegionID.Keys)
            {
                if (eventManager.CreatedTimerListRegionID.TryGetValue(key, out int remainingTime))
                {
                    remainingTime -= 1000; // 1 saniye eksilt
                    if (remainingTime <= 0)
                    {
                        eventManager.CreatedTimerListRegionID.TryRemove(key, out _); // Süresi dolanları kaldır
                        Packet timer = new Packet(0x220A);
                        timer.WriteUInt8(1);
                        await BroadcastPacketbyRegionID(key, timer);
                    }
                    else
                    {
                        eventManager.CreatedTimerListRegionID[key] = remainingTime;
                    }
                }
            }



            foreach (var key in eventManager.CreatedTimerListWorldIDAndLayerID.Keys)
            {
                if (eventManager.CreatedTimerListWorldIDAndLayerID.TryGetValue(key, out int remainingTime))
                {
                    remainingTime -= 1000; // 1 saniye eksilt
                    if (remainingTime <= 0)
                    {
                        await sqlQueryHelper.EXEC_QUERY("INSERT INTO _AsyncGameServerCommands(Action_ID, Data1, Data2) VALUES(22, @worldId, @layerId)", new { worldId = key.WorldID, layerId = key.LayerID });
                        await sqlQueryHelper.EXEC_QUERY($"UPDATE {Service.ShardDB}.._CharInstanceWorldData set DungeonKeyID = 0 where WorldID = @worldId and LayerID = @layerId", new { worldId = key.WorldID, layerId = key.LayerID });
                        eventManager.CreatedTimerListWorldIDAndLayerID.TryRemove(key, out _); // Süresi dolanları kaldır
                        //Packet timer = new Packet(0x220A);
                        //timer.WriteUInt8(1);
                        //await BroadcastPacketbyWorldIDAndLayerID(key.WorldID, key.LayerID, timer);
                    }
                    else
                    {
                        eventManager.CreatedTimerListWorldIDAndLayerID[key] = remainingTime;

                    }
                }
            }
        }
        private async Task<PacketResult> SERVER_AGENT_CHARACTER_DATA(Packet packet, ISession session, object obj)
        {
            if (packet.GetBytes().Length > 59)
            {
                var serverTime = packet.ReadUInt32(); // * 4   uint    ServerTime      //SROTimeStamp
                var refObjId = packet.ReadUInt32(); // 4   uint    RefObjID
                var scale = packet.ReadUInt8(); // 1   byte    Scale
                var curLevel = packet.ReadUInt8(); // 1   byte    CurLevel
                var maxLevel = packet.ReadUInt8(); // 1   byte    MaxLevel
                var expOffset = packet.ReadUInt64(); // 8   ulong   ExpOffset
                var sExpOffset = packet.ReadUInt32(); // 4   uint    SExpOffset
                var remainGold = packet.ReadUInt64(); // 8   ulong   RemainGold
                var remainSkillPoint = packet.ReadUInt32(); // 4   uint    RemainSkillPoint
                var remainStatPoint = packet.ReadUInt16(); // 2   ushort  RemainStatPoint
                var remainHwanCount = packet.ReadUInt8(); // 1   byte    RemainHwanCount
                var gatheredExpPoint = packet.ReadUInt32(); // 4   uint    GatheredExpPoint
                var hp = packet.ReadUInt32(); // 4   uint    HP
                var mp = packet.ReadUInt32(); // 4   uint    MP
                var autoInverstExp = packet.ReadUInt8(); // 1   byte    AutoInverstExp
                var dailyPk = packet.ReadUInt8(); // 1   byte    DailyPK
                var totalPk = packet.ReadUInt16(); // 2   ushort  TotalPK
                var pkPenaltyPoint = packet.ReadUInt32(); // 4   uint    PKPenaltyPoint
                var hwanLevel = packet.ReadUInt8(); // 1   byte    HwanLevel
                session.SessionData.State.PvpCape = (PVPCape)packet.ReadUInt8(); // 1   byte    FreePVP     //0 = None, 1 = Red, 2 = Gray, 3 = Blue, 4 = White, 5 = Gold
                session.SessionData.CharObjID = refObjId;
                session.SessionData.CurLevel = curLevel;
                session.SessionData.HwanLevel = hwanLevel;
                if (refObjId < 14875)
                {
                    // Char is chinese
                    session.SessionData.EUChar = false;
                    session.SessionData.CHChar = true;
                }
                else
                {
                    // Char is European
                    session.SessionData.CHChar = false;
                    session.SessionData.EUChar = true;
                }
                if (refObjId >= 1907 && refObjId <= 1919)
                {
                    session.SessionData.MaleChar = true;
                }
                else if (refObjId >= 14873 && refObjId <= 14887)
                {
                    session.SessionData.MaleChar = true;
                }
                else if (refObjId >= 1920 && refObjId <= 1932)
                {
                    session.SessionData.FemaleChar = true;
                }
                else if (refObjId >= 14888 && refObjId <= 14900)
                {
                    session.SessionData.FemaleChar = true;
                }
            }

            return new PacketResult();
        }
        public override void AddSession(ISession session)
        {
            AgentSessions.Add(session);
        }

        public override void RemoveSession(ISession session)
        {
            AgentSessions.RemoveWhere(sess => sess.ClientGuid == session.ClientGuid);
        }

        public override void Dispose()
        {
            //if (UserDatabase != null)
            //{
            //    foreach (var agentSession in AgentSessions)
            //    {
            //     agentSession.Stop();
            //    }
            //}

            //UserDatabase = null;

            base.Dispose();
        }
       public string FormatNumber(long num)
        {
            if (num >= 100000000)
            {
                return (num / 1000000D).ToString("0.#M");
            }
            if (num >= 1000000)
            {
                return (num / 1000000D).ToString("0.##M");
            }
            if (num >= 100000)
            {
                return (num / 1000D).ToString("0.#k");
            }
            if (num >= 10000)
            {
                return (num / 1000D).ToString("0.##k");
            }

            return num.ToString("#,0");
        }
        public async Task LoadRefHwan()
        {
            try
            {
                RefHwan.Clear();
                // Diğer önbellekleri de temizleyin
                // Örnek: m_AnotherReferenceTable.Clear();

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    // İlk sorgu: _ActiveTitleColors tablosu
                    string query1 = $"SELECT HwanLevel, Title_CH70, Title_EU70 FROM {Service.ShardDB}.._RefHWANLevel with (nolock)";
                    var result1 = await connection.QueryAsync<_RefHWANLevel>(query1);
                    foreach (var item in result1)
                    {
                        if (!RefHwan.ContainsKey(item.HwanLevel))
                            RefHwan.TryAdd(item.HwanLevel, item);
                    }
                    //Console.WriteLine("LoadRefHwan loaded into cache. Total count: " + RefHwan.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadRefHwan tables: {ex.Message}");
            }
        }

    }
}
