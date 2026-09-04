using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading.Tasks;
using JTGuard.Database;
using JTGuard.Database.Model;
using JTGuard.Database.Models;
using JTGuard.Database.ModelsEvents;
using JTGuard.Helpers;
using JTGuard.PacketHandlerManager;
using JTGuard.ServerManagers;
using JTGuard.SessionManager;
using Microsoft.Data.SqlClient;
using SilkroadSecurityAPI;
using static System.Net.Mime.MediaTypeNames;
using static JTGuard.ServerManagers.Events.LastManStanding;

namespace JTGuard.Server.AgentPacketHandler
{
    public partial class CustomGameServerPacketHandler
    {
        private AgentServer AgentServer { get; set; }
        public _ServerSettings Service { get; set; }
        public CustomGameServerPacketHandler(_ServerSettings service, AgentServer agentServer, IPacketHandler packetHandler)
        {
            AgentServer = agentServer;
            Service = service;
            packetHandler.RegisterModuleHandler(0x5038, TARGET_PLAYER_ITEM_INFO);
            packetHandler.RegisterModuleHandler(0x5033, GLOBAL_ITEM_LINK);
            packetHandler.RegisterModuleHandler(0xB034, SERVER_ITEM_MOVE);
            packetHandler.RegisterModuleHandler(0x30BF, SERVER_ENTITY_STATE_UPDATE);
            packetHandler.RegisterModuleHandler(0x5013, SERVER_KILL_LOGGER);
            packetHandler.RegisterModuleHandler(0x5010, UNIQUE_DPS); 
            packetHandler.RegisterModuleHandler(0x5014, SERVER_MOB_KILL_LOGGER);
            packetHandler.RegisterModuleHandler(0x5030, SERVER_ITEM_LOCK_INFO_LOCKED);
            packetHandler.RegisterModuleHandler(0x5031, SERVER_ITEM_LOCK_INFO_UNLOCKED);
            packetHandler.RegisterModuleHandler(0x5017, SERVER_NEW_ALCHEMY_RESULT);
            packetHandler.RegisterModuleHandler(0x5034, SERVER_ALCHEMY_LINK);
            packetHandler.RegisterModuleHandler(0x5035, GET_POS_INFO_FROM_GS);

        }
        private async Task<PacketResult> GET_POS_INFO_FROM_GS(Packet packet, ISession session, object obj) // UNK
        {
            try
            {

                var worldid = packet.ReadUInt16();
                session.SessionData.WorldLayerID = packet.ReadUInt16();
            }
            catch (Exception EX)
            {
                Program.PrintInColor($"{EX.Message.ToString()}, GET_POS_INFO_FROM_GS", ConsoleColor.Red);
                return new PacketResult(packet, PacketResultType.Block);
            }
            return new PacketResult(packet, PacketResultType.Block);
        }
        private async Task<PacketResult> TARGET_PLAYER_ITEM_INFO(Packet packet, ISession session, object obj) // UNK
        {
            try
            {
                byte ItemSlot = packet.ReadUInt8();

                int Len = packet.ReadInt32();
                List<byte> Bytes = new();
                for (int i = 0; i < Len; i++)
                {
                    byte p = packet.ReadUInt8();
                    Bytes.Add(p);
                }

                int ItemID = packet.ReadInt32();
                string SenderName = packet.ReadAscii();
                var pSession = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charname == SenderName);
                if (pSession != null)
                {
                    Packet itemlink = new Packet(0x5039);
                    itemlink.WriteUInt8(ItemSlot);
                    itemlink.WriteInt32(Len);
                    foreach (var data in Bytes)
                    {
                        itemlink.WriteUInt8(data);
                    }
                    itemlink.WriteInt32(ItemID);
                    itemlink.WriteAscii(session.SessionData.Charname); /// its my charname to sender
                    await pSession.SendToClient(itemlink);
                }
            }
            catch (Exception EX)
            {
                Program.PrintInColor($"{EX.Message.ToString()}, TARGET_PLAYER_ITEM_INFO", ConsoleColor.Red);
                return new PacketResult(packet, PacketResultType.Block);
            }
            return new PacketResult(packet, PacketResultType.Block);
        }
        private async Task<PacketResult> GLOBAL_ITEM_LINK(Packet packet, ISession session, object obj) // UNK
        {
            try
            {
                byte GlobalType = packet.ReadUInt8();
                byte GlobalSlot = packet.ReadUInt8();
                ushort GlobalItemType = packet.ReadUInt16();
                int GlobalItemID = packet.ReadInt32();
                string Message = packet.ReadUnicode();
                string RGBColor = "";
                if (RefManager.RefGlobalColor.ContainsKey(GlobalItemID))
                {
                    RGBColor = RefManager.RefGlobalColor[GlobalItemID];
                }
                else
                {
                    RGBColor = "#FFFF00";
                }
                int argbInputColor = Int32.Parse(RGBColor.Replace("#", ""), NumberStyles.HexNumber);

                Packet reverse = new Packet(0x704C, true, false);
                reverse.WriteUInt8(GlobalSlot);
                reverse.WriteUInt16(GlobalItemType);
                reverse.WriteAscii(Message);
                await session.SendToServer(reverse);

                if (GlobalType == 0)
                {
                    Packet itemlink = new Packet(0x179B);
                    itemlink.WriteUInt8(GlobalType);
                    itemlink.WriteUnicode(session.SessionData.Charname + ":" + Message);// Message
                    itemlink.WriteInt32(argbInputColor);
                    await AgentServer.BroadcastPacket(itemlink);
                }
                else if (GlobalType == 1)
                {
                    int Len = packet.ReadInt32();
                    List<byte> Bytes = new();
                    for (int i = 0; i < Len; i++)
                    {
                        byte p = packet.ReadUInt8();
                        Bytes.Add(p);
                    }

                    int ItemID = packet.ReadInt32();

                    Packet itemlink = new Packet(0x179B);
                    itemlink.WriteUInt8(GlobalType);
                    itemlink.WriteUnicode(session.SessionData.Charname + ":" + Message);// Message
                    itemlink.WriteInt32(argbInputColor);

                    itemlink.WriteInt32(Len);
                    foreach (var data in Bytes)
                    {
                        itemlink.WriteUInt8(data);
                    }



                    itemlink.WriteInt32(ItemID);
                    await AgentServer.BroadcastPacket(itemlink);
                }
          
            }
            catch (Exception EX)
            {
                Program.PrintInColor($"{EX.Message.ToString()}, SERVER_ALCHEMY_LINK", ConsoleColor.Red);
                return new PacketResult(packet, PacketResultType.Block);
            }
            return new PacketResult(packet, PacketResultType.Block);
        }
        private async Task<PacketResult> SERVER_ALCHEMY_LINK(Packet packet, ISession session, object obj) // UNK
        {
            try
            {
                int Len = packet.ReadInt32();
                List<byte> Bytes = new();
                for (int i = 0; i < Len; i++)
                {
                    byte p = packet.ReadUInt8();
                    Bytes.Add(p);
                }
               
                int ItemID = packet.ReadInt32();
                byte ItemPlus = packet.ReadUInt8();
                byte AdvLevel = packet.ReadUInt8();


                Packet itemlink = new Packet(0x209F);
                itemlink.WriteUnicode(session.SessionData.Charname);
                itemlink.WriteInt32(ItemID);
                itemlink.WriteUInt8(ItemPlus);
                itemlink.WriteUInt8(AdvLevel);
                itemlink.WriteInt32(Len);
                foreach (var data in Bytes)
                {
                    itemlink.WriteUInt8(data);
                }



                itemlink.WriteInt32(ItemID);
                await AgentServer.BroadcastPacket(itemlink);
            }
            catch (Exception EX)
            {
                Program.PrintInColor($"{EX.Message.ToString()}, SERVER_ALCHEMY_LINK", ConsoleColor.Red);
                return new PacketResult(packet, PacketResultType.Block);
            }
            return new PacketResult(packet, PacketResultType.Block);
        }
        private async Task<PacketResult> SERVER_ITEM_LOCK_INFO_LOCKED(Packet packet, ISession session, object obj) // UNK
        {
            try
            {

                int LockedItemSlot = packet.ReadInt32();
                Int64 id64 = packet.ReadInt64();

                Packet p = new Packet(0xF200);
                p.WriteUInt8(0);
                p.WriteUInt8(LockedItemSlot);
                await session.SendToClient(p);




                string noticeMessage = Management.GetNoticeMessage("MSG_LOCK_SUCCESS");
                Packet stMsg = new Packet(0xF201);
                stMsg.WriteUnicode(noticeMessage);
                stMsg.WriteUInt8(LockedItemSlot);
                await session.SendToClient(stMsg);


                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();
                    string query = "INSERT INTO _LockedItemList (ItemID64) VALUES(@id64)";
                    using (var command = new SqlCommand(query, connection))
                    {
                        command.Parameters.AddWithValue("@id64", id64);
                        await command.ExecuteNonQueryAsync();

                    }
                }
            }
            catch (Exception EX)
            {
                Program.PrintInColor($"{EX.Message.ToString()}, SERVER_LOCK_ITEM", ConsoleColor.Red);
                return new PacketResult(packet, PacketResultType.Block);
            }
            return new PacketResult(packet, PacketResultType.Block);
        }
        private async Task<PacketResult> SERVER_ITEM_LOCK_INFO_UNLOCKED(Packet packet, ISession session, object obj) // UNK
        {
            try
            {
                int UnlockedItemSlot = packet.ReadInt32();
                Int64 id64 = packet.ReadInt64();


                Packet p = new Packet(0x5028);
                p.WriteUInt8(UnlockedItemSlot);
                await session.SendToClient(p);

                string noticeMessage = Management.GetNoticeMessage("MSG_ITEM_UNLOCKED");
                Packet stMsg = new Packet(0xF201);
                stMsg.WriteUnicode(noticeMessage);
                stMsg.WriteUInt8(UnlockedItemSlot);
                await session.SendToClient(stMsg);


                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();
                    string query = "DELETE FROM _LockedItemList where ItemID64 = @id64";
                    using (var command = new SqlCommand(query, connection))
                    {
                        command.Parameters.AddWithValue("@id64", id64);
                        await command.ExecuteNonQueryAsync();

                    }
                }

            }
            catch (Exception EX)
            {
                Program.PrintInColor($"{EX.Message.ToString()}, SERVER_ITEM_LOCK_INFO_UNLOCKED", ConsoleColor.Red);
                return new PacketResult(packet, PacketResultType.Block);
            }
            return new PacketResult(packet, PacketResultType.Block);
        }
        private async Task<PacketResult> SERVER_NEW_ALCHEMY_RESULT(Packet packet, ISession session, object obj)
        {
            try
            {
                byte FuseType = packet.ReadUInt8();
                if (FuseType == 0)
                {
                    byte result = packet.ReadUInt8();
                    if (result != 2)
                    {
                        byte ItemSlot = packet.ReadUInt8();
                        byte EnhancerSlot = packet.ReadUInt8();
                        byte ProofSlot = packet.ReadUInt8();
                        byte btNewOptLevel = packet.ReadUInt8();
                        int nRefItemID = packet.ReadInt32();
                        byte AdvPlus = packet.ReadUInt8();

                        if (result == 0)
                        {
                            if (ItemSlot != 0)
                            {
                                if (Service.AlchemyItemLinkMinLevel != 0 && btNewOptLevel + AdvPlus >= Service.AlchemyItemLinkMinLevel)
                                {
                                    var job = new DelayedJobItem(
                                        500, session, null,
                                        (s, p) =>
                                        {
                                            Packet pck = new Packet(0x3533);
                                            pck.WriteUInt8(ItemSlot);
                                            pck.WriteUInt8(AdvPlus);
                                            session.SendToServer(pck);
                                        });

                                    Management.g_DelayedJobMgr.CreateJob(job); 
                                }
                                using (var connection = new SqlConnection(Program.Connectionstring))
                                {
                                    await connection.OpenAsync();
                                    string query = $"EXEC _OnAlchemySuccess_EDIT {session.SessionData.Charid}, '{session.SessionData.Charname}', {nRefItemID}, {btNewOptLevel}, {AdvPlus}, {ItemSlot}";
                                    using (var command = new SqlCommand(query, connection))
                                    {
                                        await command.ExecuteNonQueryAsync();

                                    }
                                }
                            }
                        }
                    }
                }
                else if (FuseType == 1)
                {
                    byte result = packet.ReadUInt8();
                    if (result == 0)
                    {
                        byte ItemSlot = packet.ReadUInt8();
                        byte EnhancerSlot = packet.ReadUInt8();
                        byte btNewOptLevel = packet.ReadUInt8();
                        int nRefItemID = packet.ReadInt32();
                        byte AdvPlus = packet.ReadUInt8();

                        if (ItemSlot != 0)
                        {
                            if (Service.AlchemyItemLinkMinLevel != 0 && btNewOptLevel + AdvPlus >= Service.AlchemyItemLinkMinLevel)
                            {
                                var job = new DelayedJobItem(
                                    500, session, null,
                                    (s, p) =>
                                    {
                                        Packet pck = new Packet(0x3533);
                                        pck.WriteUInt8(ItemSlot);
                                        pck.WriteUInt8(AdvPlus);
                                        session.SendToServer(pck);
                                    });

                                Management.g_DelayedJobMgr.CreateJob(job); 
                            }
                            using (var connection = new SqlConnection(Program.Connectionstring))
                            {
                                await connection.OpenAsync();
                                string query = $"EXEC _OnAlchemySuccess_EDIT {session.SessionData.Charid}, '{session.SessionData.Charname}', {nRefItemID}, {btNewOptLevel}, {AdvPlus}, {ItemSlot}";
                                using (var command = new SqlCommand(query, connection))
                                {
                                    await command.ExecuteNonQueryAsync();

                                }
                            }
                        }
                    }
                }
            }
            catch (Exception EX)
            {
                Program.PrintInColor($"{EX.Message}, SERVER_NEW_ALCHEMY_RESULT", ConsoleColor.Red);
                return new PacketResult(packet, PacketResultType.Nothing);
            }
            return new PacketResult(packet, PacketResultType.Nothing);
        }


        private async Task<PacketResult> SERVER_ITEM_MOVE(Packet packet, ISession session, object obj) // UNK
        {
            try
            {
                //byte successflag = packet.ReadUInt8();
                //if (successflag == 1)
                //{
                //    byte movementtype = packet.ReadUInt8();
                //    if (movementtype == 8)
                //    {
                //        byte itemTab = packet.ReadUInt8();
                //        byte itemSlot = packet.ReadUInt8();
                //        byte  numberOfslotsUsed = packet.ReadUInt8();
                //        byte toSlot = packet.ReadUInt8();
                //        byte toSlot2 = packet.ReadUInt8();
                //        ushort stackAmount = packet.ReadUInt16();
                //        using (var connection = new SqlConnection(Program.Connectionstring))
                //        {
                //            await connection.OpenAsync();
                //            string query = $"EXEC _OnItemPurchasedFromNPC_EDIT {session.SessionData.Charid}, {itemTab}, {itemSlot}, {numberOfslotsUsed}, {toSlot}";
                //            using (var command = new SqlCommand(query, connection))
                //            {
                //                await command.ExecuteNonQueryAsync();

                //            }
                //        }
                //        /// item tab 2
                //        /// item slot 0 
                //        /// ?? 1 
                //        /// ToSlot = 53  ?
                //        /// ToSlot2 = 196 ? 
                //        return new PacketResult();


                //    }
                //}
            }
            catch (Exception EX)
            {
                Program.PrintInColor($"{EX.Message.ToString()}, SERVER_ITEM_MOVE", ConsoleColor.Red);
            }
            return new PacketResult(packet, PacketResultType.Nothing);
        }
        private async Task<PacketResult> SERVER_ENTITY_STATE_UPDATE(Packet packet, ISession session, object obj) // UNK
        {
            try
            {
                uint uniqueId = packet.ReadUInt32();
                if (session.SessionData.UniqueCharId != uniqueId)
                    return new PacketResult(packet, PacketResultType.Nothing);

                byte updateType = packet.ReadUInt8();
                byte updateState = packet.ReadUInt8();

                if (updateType == 4)
                {
                    session.SessionData.State.BodyState = (BodyState)updateState;
                }
            }
            catch (Exception EX)
            {
                Program.PrintInColor($"{EX.Message.ToString()}, SERVER_ENTITY_STATE_UPDATE", ConsoleColor.Red);
                return new PacketResult(packet, PacketResultType.Nothing);
            }
            return new PacketResult(packet, PacketResultType.Nothing);
        }
        private async Task<PacketResult> SERVER_KILL_LOGGER(Packet packet, ISession session, object obj) // UNK
        {
            try
            {
                string KillerCharName = packet.ReadAscii();
                int KillerCharID = packet.ReadInt32();


                int RegionID = packet.ReadInt32();
                int WorldID = packet.ReadInt32();
                byte JobStatus = packet.ReadUInt8();
                string KillerGuildName = packet.ReadAscii();


                if (AgentServer.eventManager.survivalArena.m_SurvivalArenaSetting.Count() > 0)
                {
                    if (AgentServer.eventManager.survivalArena.SurvivalArenaEventStatus)
                    {
                        if (AgentServer.eventManager.survivalArena.SurvivalArenaLogger.ContainsKey(KillerCharName))
                        {
                            AgentServer.eventManager.survivalArena.SurvivalArenaLogger[KillerCharName].Kill += 1;

                            if (AgentServer.eventManager.survivalArena.m_SurvivalArenaSetting[0].KillRewardQuantity != 0)
                            {
                                await sqlQueryHelper.AddItemToChest(KillerCharID, AgentServer.eventManager.survivalArena.m_SurvivalArenaSetting[0].KillRewardName, AgentServer.eventManager.survivalArena.m_SurvivalArenaSetting[0].KillRewardQuantity,
                                    "Survival Arena", 0);
                                var state = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == KillerCharID);
                                if (state != null)
                                {
                                    Packet stMsg = new Packet(0x169C);
                                    stMsg.WriteUInt8(12);
                                    stMsg.WriteUnicode("item [%ls] added to chest.");
                                    stMsg.WriteInt32(AgentServer.eventManager.survivalArena.m_SurvivalArenaSetting[0].KillRewardID);
                                    await state.SendToClient(stMsg);
                                }

                            }

                            if (AgentServer.eventManager.CreatedKillCounterWorldID.ContainsKey(WorldID))
                            {
                                string Charname = KillerCharName;
                                if (AgentServer.eventManager.KillCounterKillList.ContainsKey(Charname))
                                {
                                    if (AgentServer.eventManager.KillCounterKillList[Charname].WorldID == WorldID)
                                    {
                                        AgentServer.eventManager.KillCounterKillList[Charname].Kill += 1;
                                        var topKillers = AgentServer.eventManager.KillCounterKillList.Where(x => x.Value.WorldID == WorldID).OrderByDescending(x => x.Value.Kill).Take(5).ToList();


                                        if (topKillers.Count() > 0)
                                        {
                                            Packet counter = new Packet(0x207C);
                                            counter.WriteUInt8(topKillers.Count());
                                            foreach (var line in topKillers)
                                            {
                                                counter.WriteAscii(line.Value.CharName16);
                                                counter.WriteInt32(line.Value.Kill);
                                            }
                                            await AgentServer.BroadcastPacketbyWorldID(WorldID, counter);
                                        }

                                    }
                                }
                                else
                                {
                                    var Create = new SCreatedKillCounterKillList();
                                    Create.WorldID = WorldID;
                                    Create.CharName16 = Charname;
                                    Create.Kill = 1;
                                    AgentServer.eventManager.KillCounterKillList.TryAdd(Charname, Create);
                                    var topKillers = AgentServer.eventManager.KillCounterKillList.Where(x => x.Value.WorldID == WorldID).OrderByDescending(x => x.Value.Kill).Take(5).ToList();

                                    if (topKillers.Count() > 0)
                                    {
                                        Packet counter = new Packet(0x207C);
                                        counter.WriteUInt8(topKillers.Count());
                                        foreach (var line in topKillers)
                                        {
                                            counter.WriteAscii(line.Value.CharName16);
                                            counter.WriteInt32(line.Value.Kill);
                                        }
                                        await AgentServer.BroadcastPacketbyWorldID(WorldID, counter);
                                    }
                                }
                            }
                        }

                    }
                }
                if (AgentServer.eventManager.lastManStanding.m_LMSSetting.Count() > 0)
                {
                    if (AgentServer.eventManager.lastManStanding.LMSEventStatus)
                    {
                        if (AgentServer.eventManager.lastManStanding.LMSLogger.ContainsKey(KillerCharName))
                        {
                            AgentServer.eventManager.lastManStanding.LMSLogger[KillerCharName].Kill += 1;

                            if (AgentServer.eventManager.CreatedKillCounterWorldID.ContainsKey(WorldID))
                            {
                                if (AgentServer.eventManager.KillCounterKillList.ContainsKey(KillerCharName))
                                {
                                    if (AgentServer.eventManager.KillCounterKillList[KillerCharName].WorldID == WorldID)
                                    {
                                        AgentServer.eventManager.KillCounterKillList[KillerCharName].Kill += 1;
                                        var topKillers = AgentServer.eventManager.KillCounterKillList.Where(x => x.Value.WorldID == WorldID).OrderByDescending(x => x.Value.Kill).Take(5).ToList();


                                        if (topKillers.Count() > 0)
                                        {
                                            Packet counter = new Packet(0x207C);
                                            counter.WriteUInt8(topKillers.Count());
                                            foreach (var line in topKillers)
                                            {
                                                counter.WriteAscii(line.Value.CharName16);
                                                counter.WriteInt32(line.Value.Kill);
                                            }
                                            await AgentServer.BroadcastPacketbyWorldID(WorldID, counter);
                                        }
                                    }
                                }
                                else
                                {
                                    var Create = new SCreatedKillCounterKillList();
                                    Create.WorldID = WorldID;
                                    Create.CharName16 = KillerCharName;
                                    Create.Kill = 1;
                                    AgentServer.eventManager.KillCounterKillList.TryAdd(KillerCharName, Create);
                                    var topKillers = AgentServer.eventManager.KillCounterKillList.Where(x => x.Value.WorldID == WorldID).OrderByDescending(x => x.Value.Kill).Take(5).ToList();


                                    if (topKillers.Count() > 0)
                                    {
                                        Packet counter = new Packet(0x207C);
                                        counter.WriteUInt8(topKillers.Count());
                                        foreach (var line in topKillers)
                                        {
                                            counter.WriteAscii(line.Value.CharName16);
                                            counter.WriteInt32(line.Value.Kill);
                                        }
                                        await AgentServer.BroadcastPacketbyWorldID(WorldID, counter);
                                    }
                                }
                            }

                            if (AgentServer.eventManager.lastManStanding.LMSLogger.ContainsKey(session.SessionData.Charname))
                            {
                                if (AgentServer.eventManager.lastManStanding.m_LMSSetting[0].EliminateRewardQuantity != 0)
                                {
                                    await sqlQueryHelper.AddItemToChest(session.SessionData.Charid, AgentServer.eventManager.lastManStanding.m_LMSSetting[0].EliminateRewardName,
                                         AgentServer.eventManager.lastManStanding.m_LMSSetting[0].EliminateRewardQuantity,
                                        "Last Man Standing", 0);
                                    Packet stMsg = new Packet(0x169C);
                                    stMsg.WriteUInt8(12);
                                    stMsg.WriteUnicode("item [%ls] added to chest.");
                                    stMsg.WriteInt32(AgentServer.eventManager.lastManStanding.m_LMSSetting[0].EliminateRewardID);
                                    await session.SendToClient(stMsg);

                                    string closedmsg = Management.GetNoticeMessage("MSG_LMS_EVENT_ELIMINATED");
                                    Packet notice = new Packet(0x168A);
                                    notice.WriteUInt8(NoticeType.QUEST);
                                    notice.WriteUnicode(closedmsg);
                                    await session.SendToClient(notice);

                                    AgentServer.eventManager.lastManStanding.LMSLogger.TryRemove(session.SessionData.Charname, out SLMSLogger valu);

                                }
                            }

                        }
                    }
                }
                if (AgentServer.eventManager.defendTower.m_DefendTowerSetting.Count() > 0)
                {
                    if (AgentServer.eventManager.defendTower.DefendTowerEventStatus)
                    {
                        if (AgentServer.eventManager.defendTower.DefendTowerLogger.ContainsKey(KillerCharName))
                        {
                            AgentServer.eventManager.defendTower.DefendTowerLogger[KillerCharName].Kill += 1;

                            if (AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].KillRewardQuantity != 0)
                            {
                                await sqlQueryHelper.AddItemToChest(KillerCharID, AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].KillRewardName, AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].KillRewardQuantity,
                                    "Defend Tower", 0);
                                var state = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == KillerCharID);
                                if (state != null)
                                {
                                    Packet stMsg = new Packet(0x169C);
                                    stMsg.WriteUInt8(12);
                                    stMsg.WriteUnicode("item [%ls] added to chest.");
                                    stMsg.WriteInt32(AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].KillRewardID);
                                    await state.SendToClient(stMsg);
                                }

                            }

                            if (AgentServer.eventManager.CreatedTeamKillCounterWorldID.ContainsKey(WorldID))
                            {
                                if (AgentServer.eventManager.TeamKillCounterKillList.ContainsKey(KillerCharName))
                                {
                                    if (AgentServer.eventManager.TeamKillCounterKillList[KillerCharName].WorldID == WorldID)
                                    {
                                        AgentServer.eventManager.TeamKillCounterKillList[KillerCharName].Kill += 1;
                                        var topKillers = AgentServer.eventManager.TeamKillCounterKillList.Where(x => x.Value.WorldID == WorldID).OrderByDescending(x => x.Value.Kill).Take(5).ToList();


                                        int redteamkillss = 0;
                                        int blueteamkillss = 0;
                                        foreach (var x in topKillers)
                                        {

                                            if (x.Value.Team == 1)
                                            {
                                                redteamkillss += x.Value.Kill;
                                            }
                                            else if (x.Value.Team == 3)
                                            {
                                                blueteamkillss += x.Value.Kill;
                                            }
                                        }

                                        if (topKillers.Count() > 0)
                                        {
                                            Packet counter = new Packet(0x189B);
                                            counter.WriteUInt8(topKillers.Count());
                                            foreach (var line in topKillers)
                                            {
                                                counter.WriteAscii(line.Value.CharName16);
                                                counter.WriteUInt8(line.Value.Team);
                                                counter.WriteInt32(line.Value.Kill);
                                                counter.WriteInt32(redteamkillss);
                                                counter.WriteInt32(blueteamkillss);
                                            }
                                            await AgentServer.BroadcastPacketbyWorldID(WorldID, counter);
                                        }
                                    }
                                }
                                else
                                {
                                    var Create = new SCreatedTeamKillCounterKillList();
                                    Create.WorldID = WorldID;
                                    Create.CharName16 = KillerCharName;
                                    Create.Kill = 1;
                                    Create.Team = AgentServer.eventManager.defendTower.DefendTowerLogger[KillerCharName].Team;
                                    var topKillers = AgentServer.eventManager.TeamKillCounterKillList.Where(x => x.Value.WorldID == WorldID).OrderByDescending(x => x.Value.Kill).Take(5).ToList();


                                    int redteamkillss = 0;
                                    int blueteamkillss = 0;
                                    foreach (var x in topKillers)
                                    {

                                        if (x.Value.Team == 1)
                                        {
                                            redteamkillss += x.Value.Kill;
                                        }
                                        else if (x.Value.Team == 3)
                                        {
                                            blueteamkillss += x.Value.Kill;
                                        }
                                    }

                                    if (topKillers.Count() > 0)
                                    {
                                        Packet counter = new Packet(0x189B);
                                        counter.WriteUInt8(topKillers.Count());
                                        foreach (var line in topKillers)
                                        {
                                            counter.WriteAscii(line.Value.CharName16);
                                            counter.WriteUInt8(line.Value.Team);
                                            counter.WriteInt32(line.Value.Kill);
                                            counter.WriteInt32(redteamkillss);
                                            counter.WriteInt32(blueteamkillss);
                                        }
                                        await AgentServer.BroadcastPacketbyWorldID(WorldID, counter);
                                    }
                                }
                            }
                        }

                    }
                }
                if (AgentServer.eventManager.styriaClash.m_StyriaClashSetting.Count() > 0)
                {
                    if (AgentServer.eventManager.styriaClash.StyriaClashEventStatus)
                    {
                        if (AgentServer.eventManager.styriaClash.StyriaClashLogger.ContainsKey(KillerCharName))
                        {
                            AgentServer.eventManager.styriaClash.StyriaClashLogger[KillerCharName].Kill += AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].CharKillPoint;

                            if (AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].CharKillRewardQuantity != 0)
                            {
                                await sqlQueryHelper.AddItemToChest(KillerCharID, AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].CharKillRewardName, AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].CharKillRewardQuantity,
                                    "Styria Clash", 0);

                                var state = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == KillerCharID);
                                if (state != null)
                                {
                                    Packet stMsg = new Packet(0x169C);
                                    stMsg.WriteUInt8(12);
                                    stMsg.WriteUnicode("item [%ls] added to chest.");
                                    stMsg.WriteInt32(AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].CharKillRewardID);
                                    await state.SendToClient(stMsg);
                                }
                            }
                            var KillerCharState = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charname == KillerCharName);
                            if (KillerCharState != null)
                            {

                                if (AgentServer.eventManager.CreatedJobKillCounterWorldID.ContainsKey(WorldID))
                                {
                                    string JobName = KillerCharState.SessionData.JobName;
                                    if (AgentServer.eventManager.JobKillCounterKillList.ContainsKey(JobName))
                                    {
                                        if (AgentServer.eventManager.JobKillCounterKillList[JobName].WorldID == WorldID)
                                        {
                                            AgentServer.eventManager.JobKillCounterKillList[JobName].Kill += AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].CharKillPoint;
                                            // update counter

                                            var topKillers = AgentServer.eventManager.JobKillCounterKillList.Where(x => x.Value.WorldID == session.SessionData.WorldID).OrderByDescending(x => x.Value.Kill).Take(5).ToList();


                                            int redteamkillss = 0;
                                            int blueteamkillss = 0;
                                            foreach (var x in topKillers)
                                            {

                                                if (x.Value.Team == 1)
                                                {
                                                    redteamkillss += x.Value.Kill;
                                                }
                                                else if (x.Value.Team == 3)
                                                {
                                                    blueteamkillss += x.Value.Kill;
                                                }
                                            }

                                            if (topKillers.Count() > 0)
                                            {
                                                Packet counter = new Packet(0x189D);
                                                counter.WriteUInt8(topKillers.Count());
                                                foreach (var line in topKillers)
                                                {
                                                    counter.WriteAscii(line.Value.CharName16);
                                                    counter.WriteUInt8(line.Value.Team);
                                                    counter.WriteInt32(line.Value.Kill);
                                                    counter.WriteInt32(redteamkillss);
                                                    counter.WriteInt32(blueteamkillss);
                                                }
                                                await AgentServer.BroadcastPacket(counter);
                                            }

                                        }
                                    }
                                    else
                                    {
                                        var Create = new SCreatedJobKillCounterKillList();
                                        Create.WorldID = WorldID;
                                        Create.CharName16 = JobName;
                                        Create.Kill = AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].CharKillPoint;
                                        Create.Team = AgentServer.eventManager.styriaClash.StyriaClashLogger[KillerCharName].Team;
                                        AgentServer.eventManager.JobKillCounterKillList.TryAdd(JobName, Create);

                                        var topKillers = AgentServer.eventManager.JobKillCounterKillList.Where(x => x.Value.WorldID == session.SessionData.WorldID).OrderByDescending(x => x.Value.Kill).Take(5).ToList();


                                        int redteamkillss = 0;
                                        int blueteamkillss = 0;
                                        foreach (var x in topKillers)
                                        {

                                            if (x.Value.Team == 1)
                                            {
                                                redteamkillss += x.Value.Kill;
                                            }
                                            else if (x.Value.Team == 3)
                                            {
                                                blueteamkillss += x.Value.Kill;
                                            }
                                        }

                                        if (topKillers.Count() > 0)
                                        {
                                            Packet counter = new Packet(0x189D);
                                            counter.WriteUInt8(topKillers.Count());
                                            foreach (var line in topKillers)
                                            {
                                                counter.WriteAscii(line.Value.CharName16);
                                                counter.WriteUInt8(line.Value.Team);
                                                counter.WriteInt32(line.Value.Kill);
                                                counter.WriteInt32(redteamkillss);
                                                counter.WriteInt32(blueteamkillss);
                                            }
                                            await AgentServer.BroadcastPacket(counter);
                                        }
                                    }
                                }

                            }

                        }
                    }
                }
            }
            catch (Exception EX)
            {
                Program.PrintInColor($"{EX.Message.ToString()}, SERVER_KILL_LOGGER", ConsoleColor.Red);
                return new PacketResult(packet, PacketResultType.Block);
            }
            return new PacketResult(packet, PacketResultType.Block);
        }
        private async Task<PacketResult> UNIQUE_DPS(Packet packet, ISession session, object obj)
        {
            try
            {
                ConcurrentDictionary<string, int> LiveDps = new ConcurrentDictionary<string, int>();
                int MobID = packet.ReadInt32();//mobID
                int cont = packet.ReadInt32();//atack yapan kişi sayısı
                for (int i = 1; i <= cont; i++)
                {
                    string GuildNames = packet.ReadAscii();
                    int Atackvalue = packet.ReadInt32();//atack value

                    if (GuildNames != "")
                        LiveDps.TryAdd(GuildNames, Atackvalue);
                }
                if (LiveDps.Count != 0)
                {
                    Packet packets = new Packet(0x177F);

                    packets.WriteInt32(MobID);
                    packets.WriteUInt8(LiveDps.Count);
                    int i = 0;
                    var top5 = LiveDps.OrderByDescending(o => o.Value).ToList().Take(8);
                    foreach (var line in top5)
                    {
                        packets.WriteAscii(line.Key);
                        packets.WriteAscii(AgentServer.FormatNumber(line.Value));
                        i++;
                    }
                    await AgentServer.BroadcastPacket(packets);
                    
                }
                return new PacketResult(packet, PacketResultType.Block);
            }
            catch(Exception EX)
            {
                Program.PrintInColor($"{EX.Message.ToString()}, DPSMETER", ConsoleColor.Red);
                return new PacketResult(packet, PacketResultType.Block);
            }

         
        }
        private async Task<PacketResult> SERVER_MOB_KILL_LOGGER(Packet packet, ISession session, object obj)
        {
            try
            {

                int MonsterClass = packet.ReadInt32();
                int BRegionID = packet.ReadInt32();
                int MobID = packet.ReadInt32();

                if (MonsterClass == 3)
                {
                    // İlk başta orijinal tipleri koruyoruz
                    ushort RegionID = packet.ReadUInt16();
                    float X = packet.ReadFloat();
                    float Y = packet.ReadFloat();
                    float Z = packet.ReadFloat();

                    DateTime now = DateTime.UtcNow;
                    long unixTimestamp = (long)(now - new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)).TotalSeconds;

                    if (RefManager.UniqueLog.ContainsKey(MobID))
                    {
                        // Orijinal veri tipleriyle işlem yapıyoruz, sonradan dönüşüm olacak
                        RefManager.UniqueLog[MobID].State = 0;
                        RefManager.UniqueLog[MobID].KillerName = session.SessionData.Charname;
                        RefManager.UniqueLog[MobID].Time = unixTimestamp;

                        // Dönüşümler sadece burada, kayıt yaparken yapılıyor
                        RefManager.UniqueLog[MobID].KilledRegionID = RegionID;
                        RefManager.UniqueLog[MobID].KilledX = X;
                        RefManager.UniqueLog[MobID].KilledY = Y;
                        RefManager.UniqueLog[MobID].KilledZ = Z;
                    }
                    else
                    {
                        // Yeni log oluştururken de yine aynı dönüşümleri sonradan yapıyoruz
                        var newlog = new _UniqueHistory();
                        newlog.UniqueID = MobID;
                        newlog.State = 0;
                        newlog.KillerName = session.SessionData.Charname;
                        newlog.Time = unixTimestamp;
                        newlog.KilledRegionID = RegionID;
                        newlog.KilledX = X;
                        newlog.KilledY = Y;
                        newlog.KilledZ = Z;
                        RefManager.UniqueLog.TryAdd(MobID, newlog);
                    }

                    Packet pck = new Packet(0x208A);

                    // Paket yazımı sırasında da değerleri int tipine çevirerek yazıyoruz
                    pck.WriteUInt32(RefManager.UniqueLog[MobID].UniqueID);
                    pck.WriteUnicode(RefManager.UniqueLog[MobID].KillerName);
                    pck.WriteUInt8(RefManager.UniqueLog[MobID].State);
                    pck.WriteInt64(RefManager.UniqueLog[MobID].Time);
                    pck.WriteInt32(RefManager.UniqueLog[MobID].KilledRegionID);
                    pck.WriteInt32(RefManager.UniqueLog[MobID].KilledX);
                    pck.WriteInt32(RefManager.UniqueLog[MobID].KilledY);
                    pck.WriteInt32(RefManager.UniqueLog[MobID].KilledZ);

                    await AgentServer.BroadcastPacket(pck);
                }


                if (RefManager.m_RefLoggerMobKill.Contains(MobID))
                {
                    await sqlQueryHelper.CallLoggerMobKill(session.SessionData.Charname, session.SessionData.Charid, MobID, BRegionID, session.SessionData.WorldID, MonsterClass);
                }

                /// TODO ADD EVENTS
                if (AgentServer.eventManager.styriaClash.StyriaClashEventStatus)
                {
                    if (AgentServer.eventManager.styriaClash.m_StyriaClashSetting.Count() > 0 && AgentServer.eventManager.styriaClash.m_StyriaClashMonsters.Count() > 0)
                    {
                        string KillerCharName = session.SessionData.Charname;
                        string KillerJobName = session.SessionData.JobName;
                        int WorldID = session.SessionData.WorldID;
                        foreach (var mobdata in AgentServer.eventManager.styriaClash.m_StyriaClashMonsters)
                        {
                            if (mobdata.MobID == MobID)
                            {
                                if (AgentServer.eventManager.styriaClash.StyriaClashLogger.ContainsKey(KillerCharName))
                                {
                                    AgentServer.eventManager.styriaClash.StyriaClashLogger[KillerCharName].Kill += mobdata.Point;
                                    if(AgentServer.eventManager.CreatedJobKillCounterWorldID.ContainsKey(WorldID))
                                {
                                        string JobName = KillerJobName;
                                        if (AgentServer.eventManager.JobKillCounterKillList.ContainsKey(JobName))
                                        {
                                            if (AgentServer.eventManager.JobKillCounterKillList[JobName].WorldID == WorldID)
                                            {
                                                AgentServer.eventManager.JobKillCounterKillList[JobName].Kill += mobdata.Point;
                                                // update counter

                                                var topKillers = AgentServer.eventManager.JobKillCounterKillList.Where(x => x.Value.WorldID == session.SessionData.WorldID).OrderByDescending(x => x.Value.Kill).Take(5).ToList();


                                                int redteamkillss = 0;
                                                int blueteamkillss = 0;
                                                foreach (var x in topKillers)
                                                {

                                                    if (x.Value.Team == 1)
                                                    {
                                                        redteamkillss += x.Value.Kill;
                                                    }
                                                    else if (x.Value.Team == 3)
                                                    {
                                                        blueteamkillss += x.Value.Kill;
                                                    }
                                                }

                                                if (topKillers.Count() > 0)
                                                {
                                                    Packet counter = new Packet(0x189D);
                                                    counter.WriteUInt8(topKillers.Count());
                                                    foreach (var line in topKillers)
                                                    {
                                                        counter.WriteAscii(line.Value.CharName16);
                                                        counter.WriteUInt8(line.Value.Team);
                                                        counter.WriteInt32(line.Value.Kill);
                                                        counter.WriteInt32(redteamkillss);
                                                        counter.WriteInt32(blueteamkillss);
                                                    }
                                                    await AgentServer.BroadcastPacket(counter);
                                                }

                                            }
                                        }
                                        else
                                        {
                                            var Create = new SCreatedJobKillCounterKillList();
                                            Create.WorldID = WorldID;
                                            Create.CharName16 = JobName;
                                            Create.Kill = AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].CharKillPoint;
                                            Create.Team = AgentServer.eventManager.styriaClash.StyriaClashLogger[KillerCharName].Team;
                                            AgentServer.eventManager.JobKillCounterKillList.TryAdd(JobName, Create);

                                            var topKillers = AgentServer.eventManager.JobKillCounterKillList.Where(x => x.Value.WorldID == session.SessionData.WorldID).OrderByDescending(x => x.Value.Kill).Take(5).ToList();


                                            int redteamkillss = 0;
                                            int blueteamkillss = 0;
                                            foreach (var x in topKillers)
                                            {

                                                if (x.Value.Team == 1)
                                                {
                                                    redteamkillss += x.Value.Kill;
                                                }
                                                else if (x.Value.Team == 3)
                                                {
                                                    blueteamkillss += x.Value.Kill;
                                                }
                                            }

                                            if (topKillers.Count() > 0)
                                            {
                                                Packet counter = new Packet(0x189D);
                                                counter.WriteUInt8(topKillers.Count());
                                                foreach (var line in topKillers)
                                                {
                                                    counter.WriteAscii(line.Value.CharName16);
                                                    counter.WriteUInt8(line.Value.Team);
                                                    counter.WriteInt32(line.Value.Kill);
                                                    counter.WriteInt32(redteamkillss);
                                                    counter.WriteInt32(blueteamkillss);
                                                }
                                                await AgentServer.BroadcastPacket(counter);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if (AgentServer.eventManager.defendTower.DefendTowerEventStatus)
                {
                    if (AgentServer.eventManager.defendTower.m_DefendTowerSetting.Count() > 0)
                    {
                        if (AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].BlueTowerObjID == MobID)
                        {
                            AgentServer.eventManager.defendTower.DefendTowerEventStatus = false;


                            string winmst = Management.GetNoticeMessage("MSG_DEFEND_TOWER_WIN_RED");
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.QUEST);
                            stMsg.WriteUnicode(winmst);
                            await AgentServer.BroadcastPacket(stMsg);

                            await sqlQueryHelper.EXEC_QUERY($"EXEC __DeSpawnMonsterByWorldID {AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].WorldID}");

                            ///  red team winn
                            foreach (var data in AgentServer.eventManager.defendTower.DefendTowerLogger)
                            {
                                if (data.Value.Team == 1)
                                {
                                    await sqlQueryHelper.AddItemToChest(data.Value.CharID, AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].WinnerRewardName, AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].WinnerRewardQuantity,
                                        "Defend Tower", 0);


                                    var state = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == data.Value.CharID);
                                    if (state != null)
                                    {
                                        Packet x = new Packet(0x169C);
                                        x.WriteUInt8(12);
                                        x.WriteUnicode("item [%ls] added to chest.");
                                        x.WriteInt32(AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].WinnerRewardID);
                                        await state.SendToClient(x);
                                    }
                                }
                                else if (data.Value.Team == 3)
                                {
                                    await sqlQueryHelper.AddItemToChest(data.Value.CharID, AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].LoserRewardName, AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].LoserRewardQuantity,
                                        "Defend Tower", 0);

                                    var state = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == data.Value.CharID);
                                    if (state != null)
                                    {
                                        Packet x = new Packet(0x169C);
                                        x.WriteUInt8(12);
                                        x.WriteUnicode("item [%ls] added to chest.");
                                        x.WriteInt32(AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].LoserRewardID);
                                        await state.SendToClient(x);
                                    }

                                }

                            }

                        }
                        else if (AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].RedTowerObjID == MobID)
                        {
                            string winmst = Management.GetNoticeMessage("MSG_DEFEND_TOWER_WIN_BLUE");
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.QUEST);
                            stMsg.WriteUnicode(winmst);
                            await AgentServer.BroadcastPacket(stMsg);

                            await sqlQueryHelper.EXEC_QUERY($"EXEC __DeSpawnMonsterByWorldID {AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].WorldID}");

                            ///  red team winn
                            foreach (var data in AgentServer.eventManager.defendTower.DefendTowerLogger)
                            {
                                if (data.Value.Team == 3)
                                {
                                    await sqlQueryHelper.AddItemToChest(data.Value.CharID, AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].WinnerRewardName, AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].WinnerRewardQuantity,
                                        "Defend Tower", 0);


                                    var state = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == data.Value.CharID);
                                    if (state != null)
                                    {
                                        Packet x = new Packet(0x169C);
                                        x.WriteUInt8(12);
                                        x.WriteUnicode("item [%ls] added to chest.");
                                        x.WriteInt32(AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].WinnerRewardID);
                                        await state.SendToClient(x);
                                    }
                                }
                                else if (data.Value.Team == 1)
                                {
                                    await sqlQueryHelper.AddItemToChest(data.Value.CharID, AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].LoserRewardName, AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].LoserRewardQuantity,
                                        "Defend Tower", 0);

                                    var state = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == data.Value.CharID);
                                    if (state != null)
                                    {
                                        Packet x = new Packet(0x169C);
                                        x.WriteUInt8(12);
                                        x.WriteUnicode("item [%ls] added to chest.");
                                        x.WriteInt32(AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].LoserRewardID);
                                        await state.SendToClient(x);
                                    }

                                }

                            }
                        }
                    }
                }
                return new PacketResult(PacketResultType.Block);
            }
            catch (Exception ex)
            {

                Console.WriteLine(ex.Message.ToString());
                return new PacketResult(PacketResultType.Block);
            }
        }
    }
}
