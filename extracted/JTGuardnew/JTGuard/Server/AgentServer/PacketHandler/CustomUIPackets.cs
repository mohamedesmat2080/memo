using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using System.Data.Common;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Numerics;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using Dapper;
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
using static System.Collections.Specialized.BitVector32;
using static System.Net.Mime.MediaTypeNames;
using static System.Runtime.InteropServices.JavaScript.JSType;
using static JTGuard.ServerManagers.Events.DefendTower;
using static JTGuard.ServerManagers.Events.LastManStanding;
using static JTGuard.ServerManagers.Events.StyriaClash;
using static JTGuard.ServerManagers.Events.SurvivalArena;

namespace JTGuard.Server.AgentPacketHandler
{
    public partial class CustomUIPackets
    {
        private AgentServer AgentServer { get; set; }
        public _ServerSettings Service { get; set; }
        public CustomUIPackets(_ServerSettings service, AgentServer agentServer, IPacketHandler packetHandler)
        {
            AgentServer = agentServer;
            Service = service;
            packetHandler.RegisterClientHandler(0x3560, CLIENT_INFO_REQUEST);
            packetHandler.RegisterClientHandler(0x210A, CUSTOM_ITEM_USAGE);
            packetHandler.RegisterClientHandler(0x189B, USE_FELLOW_SKILL);
            packetHandler.RegisterClientHandler(0x189A, SAVE_FELLOW_SKILL);


            packetHandler.RegisterClientHandler(0x705C, GLOBAL_ITEM_LINK_CLIENT);
            packetHandler.RegisterClientHandler(0x187E, UPDATE_MACRO_SETTING);
            packetHandler.RegisterClientHandler(0x201F, NEW_REVERSE_TELEPORT_PT_MEMBER);

            packetHandler.RegisterClientHandler(0x181C, NEW_REVERSE_TELEPORT_SAVE_LOCATION);
            packetHandler.RegisterClientHandler(0x200C, NEW_REVERSE_SAVE_LOCATION);
            packetHandler.RegisterClientHandler(0x200D, NEW_REVERSE_REMOVE_LOCATION);
            packetHandler.RegisterClientHandler(0x185A, PARTY_MEMBER_VIEWER);
            packetHandler.RegisterClientHandler(0x185C, PARTY_MEMBER_VIEWER2);

            packetHandler.RegisterClientHandler(0x169C, HandleIconManager);
            packetHandler.RegisterClientHandler(0x169B, HandleTitleManager);

            packetHandler.RegisterClientHandler(0x169A, HandleGuiPackets);
            packetHandler.RegisterClientHandler(0xA400, GInterfaceIsReady);
            packetHandler.RegisterClientHandler(0x207B, CLIENT_GRANT_NAME_REQUEST);
            packetHandler.RegisterClientHandler(0x165B, CLIENT_HWID_REQUEST);
            packetHandler.RegisterModuleHandler(0xB007, SERVER_CHARACTER_SELECTION_ACTION);
            packetHandler.RegisterModuleHandler(0xB001, SERVER_ENTER_GAME_ACTION);
            packetHandler.RegisterClientHandler(0x180A, SERVER_RANKS);

            packetHandler.RegisterClientHandler(0x7061, PARTY_LEAVE_REQUEST);
            packetHandler.RegisterClientHandler(0x7063, PARTY_BAN_REQUEST);
        }
        private async Task<PacketResult> PARTY_LEAVE_REQUEST(Packet packet, ISession session, object obj)
        {
            try
            {
                
                if(session.SessionData.WorldID == 99)
                {
                    await sendNotice(session, NoticeType.WARNING, "You cannot withdraw from the party when inside the dungeon.");
                    return new PacketResult(PacketResultType.Block);
                }

               
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "PARTY_LEAVE_REQUEST");
            }
            return new PacketResult(PacketResultType.Nothing);
        }
        private async Task<PacketResult> PARTY_BAN_REQUEST(Packet packet, ISession session, object obj)
        {
            try
            {
                uint memberjid = packet.ReadUInt32();
                if (session.SessionData.WorldID == 99)
                {
                    await sendNotice(session, NoticeType.WARNING, "You cannot disband the party not kick out party members when inside the dungeon.");
                    return new PacketResult(PacketResultType.Block);
                }


            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "PARTY_BAN_REQUEST");
            }
            return new PacketResult(PacketResultType.Nothing);
        }
        private async Task<PacketResult> SERVER_RANKS(Packet packet, ISession session, object obj)
        {
            try
            {
                byte type = packet.ReadUInt8();
                if (type == 0)
                {
                    if (!session.SessionData.RankCategoryPacket)
                    {
                        session.SessionData.RankCategoryPacket = true;
                        if (RefManager.RankCategories.Count() > 0)
                        {
                            Packet stAckMsssg = new Packet(0x170F);
                            stAckMsssg.WriteUInt8(RefManager.RankCategories.Count());

                            foreach (var line in RefManager.RankCategories)
                            {
                                stAckMsssg.WriteInt32(line.Key);
                                stAckMsssg.WriteAscii(line.Value.Category);

                            }
                            await session.SendToClient(stAckMsssg);
                        }
                    }
                }
                else if (type == 1)
                {
                    if (RefManager.Rank_Custom1.Count > 0 && RefManager.Rank_Custom1 != null)
                    {
                        Packet packetr = new Packet(0x170E);
                        if (RefManager.Rank_Custom1.Count < 50)
                        {
                            packetr.WriteUInt8(RefManager.Rank_Custom1.Count);
                        }
                        else
                        {
                            packetr.WriteUInt8(50);
                        }
                        foreach (var line in RefManager.Rank_Custom1.OrderByDescending(x => x.Value.Point).Take(50).ToList())
                        {
                            packetr.WriteAscii(line.Value.CharName16);
                            packetr.WriteAscii(line.Value.GuildName);
                            packetr.WriteInt32(line.Value.Point);
                        }
                        await session.SendToClient(packetr);
                        if (RefManager.Rank_Custom1.ContainsKey(session.SessionData.Charid))
                        {
                            Packet packets = new Packet(0x171A);
                            // var line = RankUnique.Where(x => x.Value.CharName16 == session.SessionData.Charname).FirstOrDefault();
                            int i = 0;
                            foreach (var line in RefManager.Rank_Custom1.OrderByDescending(x => x.Value.Point))
                            {
                                i++;
                                if (line.Value.CharName16 == session.SessionData.Charname)
                                {
                                    break;

                                }
                            }
                            packets.WriteAscii(session.SessionData.Charname);
                            packets.WriteInt32(i);
                            packets.WriteInt32(RefManager.Rank_Custom1[session.SessionData.Charid].Point);
                            await session.SendToClient(packets);
                        }
                    }
                }
                else if (type == 2)
                {
                    if (RefManager.Rank_Custom2.Count > 0 && RefManager.Rank_Custom1 != null)
                    {
                        Packet packetr = new Packet(0x170E);
                        if (RefManager.Rank_Custom2.Count < 50)
                        {
                            packetr.WriteUInt8(RefManager.Rank_Custom2.Count);
                        }
                        else
                        {
                            packetr.WriteUInt8(50);
                        }
                        foreach (var line in RefManager.Rank_Custom2.OrderByDescending(x => x.Value.Point).Take(50).ToList())
                        {
                            packetr.WriteAscii(line.Value.CharName16);
                            packetr.WriteAscii(line.Value.GuildName);
                            packetr.WriteInt32(line.Value.Point);
                        }
                        await session.SendToClient(packetr);
                        if (RefManager.Rank_Custom2.ContainsKey(session.SessionData.Charid))
                        {
                            Packet packets = new Packet(0x171A);
                            // var line = RankUnique.Where(x => x.Value.CharName16 == session.SessionData.Charname).FirstOrDefault();
                            int i = 0;
                            foreach (var line in RefManager.Rank_Custom2.OrderByDescending(x => x.Value.Point))
                            {
                                i++;
                                if (line.Value.CharName16 == session.SessionData.Charname)
                                {
                                    break;

                                }
                            }
                            packets.WriteAscii(session.SessionData.Charname);
                            packets.WriteInt32(i);
                            packets.WriteInt32(RefManager.Rank_Custom1[session.SessionData.Charid].Point);
                            await session.SendToClient(packets);
                        }
                    }
                }
                else if (type == 3)
                {
                    if (RefManager.Rank_Custom3.Count > 0 && RefManager.Rank_Custom1 != null)
                    {
                        Packet packetr = new Packet(0x170E);
                        if (RefManager.Rank_Custom3.Count < 50)
                        {
                            packetr.WriteUInt8(RefManager.Rank_Custom3.Count);
                        }
                        else
                        {
                            packetr.WriteUInt8(50);
                        }
                        foreach (var line in RefManager.Rank_Custom3.OrderByDescending(x => x.Value.Point).Take(50).ToList())
                        {
                            packetr.WriteAscii(line.Value.CharName16);
                            packetr.WriteAscii(line.Value.GuildName);
                            packetr.WriteInt32(line.Value.Point);
                        }
                        await session.SendToClient(packetr);
                        if (RefManager.Rank_Custom3.ContainsKey(session.SessionData.Charid))
                        {
                            Packet packets = new Packet(0x171A);
                            // var line = RankUnique.Where(x => x.Value.CharName16 == session.SessionData.Charname).FirstOrDefault();
                            int i = 0;
                            foreach (var line in RefManager.Rank_Custom3.OrderByDescending(x => x.Value.Point))
                            {
                                i++;
                                if (line.Value.CharName16 == session.SessionData.Charname)
                                {
                                    break;

                                }
                            }
                            packets.WriteAscii(session.SessionData.Charname);
                            packets.WriteInt32(i);
                            packets.WriteInt32(RefManager.Rank_Custom1[session.SessionData.Charid].Point);
                            await session.SendToClient(packets);
                        }
                    }
                }
                else if (type == 4)
                {
                    if (RefManager.Rank_Custom4.Count > 0 && RefManager.Rank_Custom1 != null)
                    {
                        Packet packetr = new Packet(0x170E);
                        if (RefManager.Rank_Custom4.Count < 50)
                        {
                            packetr.WriteUInt8(RefManager.Rank_Custom4.Count);
                        }
                        else
                        {
                            packetr.WriteUInt8(50);
                        }
                        foreach (var line in RefManager.Rank_Custom4.OrderByDescending(x => x.Value.Point).Take(50).ToList())
                        {
                            packetr.WriteAscii(line.Value.CharName16);
                            packetr.WriteAscii(line.Value.GuildName);
                            packetr.WriteInt32(line.Value.Point);
                        }
                        await session.SendToClient(packetr);
                        if (RefManager.Rank_Custom4.ContainsKey(session.SessionData.Charid))
                        {
                            Packet packets = new Packet(0x171A);
                            // var line = RankUnique.Where(x => x.Value.CharName16 == session.SessionData.Charname).FirstOrDefault();
                            int i = 0;
                            foreach (var line in RefManager.Rank_Custom4.OrderByDescending(x => x.Value.Point))
                            {
                                i++;
                                if (line.Value.CharName16 == session.SessionData.Charname)
                                {
                                    break;

                                }
                            }
                            packets.WriteAscii(session.SessionData.Charname);
                            packets.WriteInt32(i);
                            packets.WriteInt32(RefManager.Rank_Custom1[session.SessionData.Charid].Point);
                            await session.SendToClient(packets);
                        }
                    }
                }
                else if (type == 5)
                {
                    if (RefManager.Rank_Custom5.Count > 0 && RefManager.Rank_Custom1 != null)
                    {
                        Packet packetr = new Packet(0x170E);
                        if (RefManager.Rank_Custom5.Count < 50)
                        {
                            packetr.WriteUInt8(RefManager.Rank_Custom5.Count);
                        }
                        else
                        {
                            packetr.WriteUInt8(50);
                        }
                        foreach (var line in RefManager.Rank_Custom5.OrderByDescending(x => x.Value.Point).Take(50).ToList())
                        {
                            packetr.WriteAscii(line.Value.CharName16);
                            packetr.WriteAscii(line.Value.GuildName);
                            packetr.WriteInt32(line.Value.Point);
                        }
                        await session.SendToClient(packetr);
                        if (RefManager.Rank_Custom5.ContainsKey(session.SessionData.Charid))
                        {
                            Packet packets = new Packet(0x171A);
                            // var line = RankUnique.Where(x => x.Value.CharName16 == session.SessionData.Charname).FirstOrDefault();
                            int i = 0;
                            foreach (var line in RefManager.Rank_Custom5.OrderByDescending(x => x.Value.Point))
                            {
                                i++;
                                if (line.Value.CharName16 == session.SessionData.Charname)
                                {
                                    break;

                                }
                            }
                            packets.WriteAscii(session.SessionData.Charname);
                            packets.WriteInt32(i);
                            packets.WriteInt32(RefManager.Rank_Custom1[session.SessionData.Charid].Point);
                            await session.SendToClient(packets);
                        }
                    }
                }
                else if (type == 6)
                {
                    if (RefManager.Rank_Custom6.Count > 0 && RefManager.Rank_Custom1 != null)
                    {
                        Packet packetr = new Packet(0x170E);
                        if (RefManager.Rank_Custom6.Count < 50)
                        {
                            packetr.WriteUInt8(RefManager.Rank_Custom6.Count);
                        }
                        else
                        {
                            packetr.WriteUInt8(50);
                        }
                        foreach (var line in RefManager.Rank_Custom6.OrderByDescending(x => x.Value.Point).Take(50).ToList())
                        {
                            packetr.WriteAscii(line.Value.CharName16);
                            packetr.WriteAscii(line.Value.GuildName);
                            packetr.WriteInt32(line.Value.Point);
                        }
                        await session.SendToClient(packetr);
                        if (RefManager.Rank_Custom6.ContainsKey(session.SessionData.Charid))
                        {
                            Packet packets = new Packet(0x171A);
                            // var line = RankUnique.Where(x => x.Value.CharName16 == session.SessionData.Charname).FirstOrDefault();
                            int i = 0;
                            foreach (var line in RefManager.Rank_Custom6.OrderByDescending(x => x.Value.Point))
                            {
                                i++;
                                if (line.Value.CharName16 == session.SessionData.Charname)
                                {
                                    break;

                                }
                            }
                            packets.WriteAscii(session.SessionData.Charname);
                            packets.WriteInt32(i);
                            packets.WriteInt32(RefManager.Rank_Custom1[session.SessionData.Charid].Point);
                            await session.SendToClient(packets);
                        }
                    }
                }
                else if (type == 7)
                {
                    if (RefManager.Rank_Custom7.Count > 0 && RefManager.Rank_Custom1 != null)
                    {
                        Packet packetr = new Packet(0x170E);
                        if (RefManager.Rank_Custom7.Count < 50)
                        {
                            packetr.WriteUInt8(RefManager.Rank_Custom7.Count);
                        }
                        else
                        {
                            packetr.WriteUInt8(50);
                        }
                        foreach (var line in RefManager.Rank_Custom7.OrderByDescending(x => x.Value.Point).Take(50).ToList())
                        {
                            packetr.WriteAscii(line.Value.CharName16);
                            packetr.WriteAscii(line.Value.GuildName);
                            packetr.WriteInt32(line.Value.Point);
                        }
                        await session.SendToClient(packetr);
                        if (RefManager.Rank_Custom7.ContainsKey(session.SessionData.Charid))
                        {
                            Packet packets = new Packet(0x171A);
                            // var line = RankUnique.Where(x => x.Value.CharName16 == session.SessionData.Charname).FirstOrDefault();
                            int i = 0;
                            foreach (var line in RefManager.Rank_Custom7.OrderByDescending(x => x.Value.Point))
                            {
                                i++;
                                if (line.Value.CharName16 == session.SessionData.Charname)
                                {
                                    break;

                                }
                            }
                            packets.WriteAscii(session.SessionData.Charname);
                            packets.WriteInt32(i);
                            packets.WriteInt32(RefManager.Rank_Custom1[session.SessionData.Charid].Point);
                            await session.SendToClient(packets);
                        }
                    }
                }
                else if (type == 8)
                {
                    if (RefManager.Rank_Custom8.Count > 0 && RefManager.Rank_Custom1 != null)
                    {
                        Packet packetr = new Packet(0x170E);
                        if (RefManager.Rank_Custom8.Count < 50)
                        {
                            packetr.WriteUInt8(RefManager.Rank_Custom8.Count);
                        }
                        else
                        {
                            packetr.WriteUInt8(50);
                        }
                        foreach (var line in RefManager.Rank_Custom8.OrderByDescending(x => x.Value.Point).Take(50).ToList())
                        {
                            packetr.WriteAscii(line.Value.CharName16);
                            packetr.WriteAscii(line.Value.GuildName);
                            packetr.WriteInt32(line.Value.Point);
                        }
                        await session.SendToClient(packetr);
                        if (RefManager.Rank_Custom8.ContainsKey(session.SessionData.Charid))
                        {
                            Packet packets = new Packet(0x171A);
                            // var line = RankUnique.Where(x => x.Value.CharName16 == session.SessionData.Charname).FirstOrDefault();
                            int i = 0;
                            foreach (var line in RefManager.Rank_Custom8.OrderByDescending(x => x.Value.Point))
                            {
                                i++;
                                if (line.Value.CharName16 == session.SessionData.Charname)
                                {
                                    break;

                                }
                            }
                            packets.WriteAscii(session.SessionData.Charname);
                            packets.WriteInt32(i);
                            packets.WriteInt32(RefManager.Rank_Custom1[session.SessionData.Charid].Point);
                            await session.SendToClient(packets);
                        }
                    }
                }
                else if (type == 9)
                {
                    if (RefManager.Rank_Custom9.Count > 0 && RefManager.Rank_Custom1 != null)
                    {
                        Packet packetr = new Packet(0x170E);
                        if (RefManager.Rank_Custom9.Count < 50)
                        {
                            packetr.WriteUInt8(RefManager.Rank_Custom9.Count);
                        }
                        else
                        {
                            packetr.WriteUInt8(50);
                        }
                        foreach (var line in RefManager.Rank_Custom9.OrderByDescending(x => x.Value.Point).Take(50).ToList())
                        {
                            packetr.WriteAscii(line.Value.CharName16);
                            packetr.WriteAscii(line.Value.GuildName);
                            packetr.WriteInt32(line.Value.Point);
                        }
                        await session.SendToClient(packetr);
                        if (RefManager.Rank_Custom9.ContainsKey(session.SessionData.Charid))
                        {
                            Packet packets = new Packet(0x171A);
                            // var line = RankUnique.Where(x => x.Value.CharName16 == session.SessionData.Charname).FirstOrDefault();
                            int i = 0;
                            foreach (var line in RefManager.Rank_Custom9.OrderByDescending(x => x.Value.Point))
                            {
                                i++;
                                if (line.Value.CharName16 == session.SessionData.Charname)
                                {
                                    break;

                                }
                            }
                            packets.WriteAscii(session.SessionData.Charname);
                            packets.WriteInt32(i);
                            packets.WriteInt32(RefManager.Rank_Custom1[session.SessionData.Charid].Point);
                            await session.SendToClient(packets);
                        }
                    }
                }
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString());
            }

            return new PacketResult(packet, PacketResultType.Block);
        }
        private async Task<PacketResult> CLIENT_INFO_REQUEST(Packet packet, ISession session, object obj)
        {
            try
            {
                session.SessionData.JobType = packet.ReadUInt8();
                session.SessionData.LatestRegion = packet.ReadInt32();
                session.SessionData.WorldID = packet.ReadInt32();

                if (session.SessionData.JobType != 4)
                {
                    session.SessionData.JobName = packet.ReadAscii();
                }
                /// TODO EVENTLERI EKLE
                if (AgentServer.eventManager.survivalArena.m_SurvivalArenaSetting.Count() > 0)
                {
                    if (session.SessionData.WorldID == AgentServer.eventManager.survivalArena.m_SurvivalArenaSetting[0].WorldID && !AgentServer.eventManager.survivalArena.SurvivalArenaEventStatus)
                    {
                        var job = new DelayedJobItem(
                             500, session, null,
                             (s, p) =>
                             {
                                 Packet pck = new Packet(0x3535);
                                 session.SendToServer(pck);
                             });

                        Management.g_DelayedJobMgr.CreateJob(job); 

                        return new PacketResult(PacketResultType.Block);
                    }
                }
                if (AgentServer.eventManager.lastManStanding.m_LMSSetting.Count() > 0)
                {
                    if (session.SessionData.WorldID == AgentServer.eventManager.lastManStanding.m_LMSSetting[0].WorldID && !AgentServer.eventManager.lastManStanding.LMSEventStatus)
                    {
                        var job = new DelayedJobItem(
                             500, session, null,
                             (s, p) =>
                             {
                                 Packet pck = new Packet(0x3535);
                                 session.SendToServer(pck);
                             });

                        Management.g_DelayedJobMgr.CreateJob(job); 

                        return new PacketResult(PacketResultType.Block);
                    }
                }

                if (AgentServer.eventManager.styriaClash.m_StyriaClashSetting.Count() > 0)
                {
                    if (session.SessionData.WorldID == AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].WorldID && !AgentServer.eventManager.styriaClash.StyriaClashEventStatus)
                    {
                        var job = new DelayedJobItem(
                             500, session, null,
                             (s, p) =>
                             {
                                 Packet pck = new Packet(0x3535);
                                 session.SendToServer(pck);
                             });

                        Management.g_DelayedJobMgr.CreateJob(job); 

                        return new PacketResult(PacketResultType.Block);
                    }
                }
                if (AgentServer.eventManager.defendTower.m_DefendTowerSetting.Count() > 0)
                {
                    if (session.SessionData.WorldID == AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].WorldID && !AgentServer.eventManager.defendTower.DefendTowerEventStatus)
                    {
                        var job = new DelayedJobItem(
                             500, session, null,
                             (s, p) =>
                             {
                                 Packet pck = new Packet(0x3535);
                                 session.SendToServer(pck);
                             });

                        Management.g_DelayedJobMgr.CreateJob(job); 

                        return new PacketResult(PacketResultType.Block);
                    }
                }

                if (RefManager.m_RefEventMapSettings.ContainsKey(session.SessionData.LatestRegion))
                {
                    if (RefManager.m_RefEventMapSettings[session.SessionData.LatestRegion].AutoCape)
                    {
                        var job = new DelayedJobItem(
                             500, session, null,
                             (s, p) =>
                             {
                                 Packet pck = new Packet(0x3502);
                                 pck.WriteUInt8(5);
                                 session.SendToServer(pck);
                             });

                        Management.g_DelayedJobMgr.CreateJob(job); 
                    }
                }
                if (AgentServer.eventManager.defendTower.m_DefendTowerSetting.Count() > 0)
                {
                    if (session.SessionData.WorldID == AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].WorldID && AgentServer.eventManager.defendTower.DefendTowerEventStatus
                        && AgentServer.eventManager.defendTower.DefendTowerLogger.ContainsKey(session.SessionData.Charname))
                    {
                        if (AgentServer.eventManager.defendTower.DefendTowerLogger[session.SessionData.Charname].Team == 1)
                        {
                            var job = new DelayedJobItem(
                             500, session, null,
                             (s, p) =>
                             {
                                 Packet pck = new Packet(0x3502);
                                 pck.WriteUInt8(1);
                                 session.SendToServer(pck);
                             });

                            Management.g_DelayedJobMgr.CreateJob(job);
                        }
                        else if (AgentServer.eventManager.defendTower.DefendTowerLogger[session.SessionData.Charname].Team == 3)
                        {
                            var job = new DelayedJobItem(
                             500, session, null,
                             (s, p) =>
                             {
                                 Packet pck = new Packet(0x3502);
                                 pck.WriteUInt8(3);
                                 session.SendToServer(pck);
                             });

                            Management.g_DelayedJobMgr.CreateJob(job);
                        }
                        return new PacketResult(PacketResultType.Block);
                    }
                }

                if (AgentServer.eventManager.CreatedTimerListWorldID.ContainsKey(session.SessionData.WorldID))
                {

                    Packet timer = new Packet(0x220A);
                    timer.WriteUInt8(0);
                    timer.WriteInt32(AgentServer.eventManager.CreatedTimerListWorldID[session.SessionData.WorldID]);
                    await session.SendToClient(timer);
                }
                if (AgentServer.eventManager.CreatedTimerListRegionID.ContainsKey(session.SessionData.LatestRegion))
                {
                    Packet timer = new Packet(0x220A);
                    timer.WriteUInt8(0);
                    timer.WriteInt32(AgentServer.eventManager.CreatedTimerListRegionID[session.SessionData.LatestRegion]);
                    await session.SendToClient(timer);
                }
                if (AgentServer.eventManager.CreatedKillCounterWorldID.ContainsKey(session.SessionData.WorldID))
                {
                    Packet timer = new Packet(0x207A);
                    timer.WriteUInt8(1);
                    timer.WriteAscii(AgentServer.eventManager.CreatedKillCounterWorldID[session.SessionData.WorldID]);
                    await session.SendToClient(timer);

                    int WorldID = session.SessionData.WorldID;

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
                        await session.SendToClient(counter);
                    }
                }
                if (AgentServer.eventManager.CreatedTeamKillCounterWorldID.ContainsKey(session.SessionData.WorldID))
                {
                    Packet timer = new Packet(0x189A, false, false);
                    timer.WriteUInt8(1);
                    timer.WriteAscii(AgentServer.eventManager.CreatedTeamKillCounterWorldID[session.SessionData.WorldID]);
                    await session.SendToClient(timer);

                    int WorldID = session.SessionData.WorldID;

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
                        await session.SendToClient(counter);
                    }
                }
                if (AgentServer.eventManager.CreatedJobKillCounterWorldID.ContainsKey(session.SessionData.WorldID))
                {
                    Packet pck = new Packet(0x189C);
                    pck.WriteUInt8(1);
                    await session.SendToClient(pck);

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
                        await session.SendToClient(counter);
                    }
                }

                if (session.SessionData.WorldID >= 2 && session.SessionData.WorldID <= 9)
                {
                    var topPlayerKillers = AgentServer.eventManager.FtwKillCounterList.Where(x => x.Value.WorldID == session.SessionData.WorldID).OrderByDescending(x => x.Value.Kill).Take(5).ToList();

                    if (topPlayerKillers.Count() > 0)
                    {
                        Packet counter = new Packet(0x193F);
                        counter.WriteUInt8(0);
                        counter.WriteUInt8(topPlayerKillers.Count());
                        foreach (var line in topPlayerKillers)
                        {
                            counter.WriteAscii(line.Value.CharName);
                            counter.WriteInt32(line.Value.Kill);
                        }
                        await session.SendToClient(counter);
                    }

                    var topGuildKillers = AgentServer.eventManager.FtwKillCounterList.Where(x => x.Value.WorldID == session.SessionData.WorldID).GroupBy(x => x.Value.GuildName).Select(g => new { GuildName = g.Key, TotalKills = g.Sum(x => x.Value.Kill) }).OrderByDescending(x => x.TotalKills).Take(5).ToList();

                    if (topGuildKillers.Count() > 0)
                    {
                        Packet counter = new Packet(0x193F);
                        counter.WriteUInt8(1);
                        counter.WriteUInt8(topGuildKillers.Count());
                        foreach (var line in topGuildKillers)
                        {
                            counter.WriteAscii(line.GuildName); // GuildName
                            counter.WriteInt32(line.TotalKills); // Total Kills
                        }
                        await session.SendToClient(counter);
                    }

                    // Union Kill Counter (top 5)
                    var topUnionKillers = AgentServer.eventManager.FtwKillCounterList.Where(x => x.Value.WorldID == session.SessionData.WorldID).GroupBy(x => x.Value.UnionName).Select(g => new { UnionName = g.Key, TotalKills = g.Sum(x => x.Value.Kill) }).OrderByDescending(x => x.TotalKills).Take(5).ToList();

                    if (topUnionKillers.Count() > 0)
                    {
                        Packet counter = new Packet(0x193F);
                        counter.WriteUInt8(2);
                        counter.WriteUInt8(topUnionKillers.Count());
                        foreach (var line in topUnionKillers)
                        {
                            counter.WriteAscii(line.UnionName); // GuildName
                            counter.WriteInt32(line.TotalKills); // Total Kills
                        }
                        await session.SendToClient(counter);
                    }

                }
            }
            catch (Exception ex)
            {
                Program.PrintInColor(ex.Message.ToString() + "CLIENT_INF_REQ", ConsoleColor.Red);
            }
            return new PacketResult(PacketResultType.Block);

        }
        private async Task<PacketResult> CUSTOM_ITEM_USAGE(Packet packet, ISession session, object obj)
        {
            try
            {
                int itemId = packet.ReadInt32();
                int itemSlotId = packet.ReadInt32();
                ushort itemTypeId = packet.ReadUInt16();

                if (itemTypeId == 0xC6ED)
                {
                    if (itemSlotId >= 13 && itemSlotId <= 109)
                    {
                        Packet pck = new Packet(0x3530, false, false);
                        pck.WriteInt32(itemId);
                        pck.WriteInt32(itemSlotId);
                        pck.WriteUInt16(itemTypeId);
                        await session.SendToServer(pck);
                    }
                }
                else if (itemTypeId == 0xCEED)
                { // is item locker
                    int targetitemslot = packet.ReadInt32();
                    string Code = packet.ReadUnicode();
                    if (itemSlotId >= 13 && itemSlotId <= 109)
                    {
                        int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_LOCK_MAIL_TIME).TotalSeconds);
                        if (gecensaniye < 180)
                        {
                            if (int.TryParse(Code, out int Codeint))
                            {
                                if (Codeint == session.SessionData.LockCode)
                                {
                                    Packet pck = new Packet(0x3531, false, false);
                                    pck.WriteInt32(itemId);
                                    pck.WriteInt32(itemSlotId);
                                    pck.WriteUInt16(itemTypeId);
                                    pck.WriteInt32(targetitemslot);
                                    await session.SendToServer(pck);
                                    session.SessionData.LAST_LOCK_MAIL_TIME = new DateTime(2020, 12, 31);
                                    session.SessionData.LockCode = 0;
                                }
                                else
                                {
                                    string noticeMessage = Management.GetNoticeMessage("MSG_LOCK_CODE_WRONG");
                                    Packet stMsg = new Packet(0x168A);
                                    stMsg.WriteUInt8(NoticeType.WARNING);
                                    stMsg.WriteUnicode(noticeMessage);
                                    await session.SendToClient(stMsg);


                                    Packet stMsgx = new Packet(0x5015);
                                    stMsgx.WriteUInt8(4);
                                    await session.SendToClient(stMsgx);
                                    return new PacketResult(PacketResultType.Block);
                                }
                            }
                        }
                        else
                        {
                            string noticeMessage = Management.GetNoticeMessage("MSG_LOCK_CODE_EXPIRED");
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.WARNING);
                            stMsg.WriteUnicode(noticeMessage);
                            await session.SendToClient(stMsg);
                            return new PacketResult(PacketResultType.Block);
                        }
                    }
                }
                else if (itemTypeId == 0xD6ED)
                { // is item unlocker
                    int targetitemslot = packet.ReadInt32();

                    string Code = packet.ReadUnicode();
                    if (itemSlotId >= 13 && itemSlotId <= 109)
                    {
                        int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_UNLOCK_MAIL_TIME).TotalSeconds);
                        if (gecensaniye < 180)
                        {
                            if (int.TryParse(Code, out int Codeint))
                            {
                                if (Codeint == session.SessionData.UnLockCode)
                                {
                                    Packet pck = new Packet(0x3532, false, false);
                                    pck.WriteInt32(itemId);
                                    pck.WriteInt32(itemSlotId);
                                    pck.WriteUInt16(itemTypeId);
                                    pck.WriteInt32(targetitemslot);
                                    await session.SendToServer(pck);
                                    session.SessionData.LAST_UNLOCK_MAIL_TIME = new DateTime(2020, 12, 31);
                                    session.SessionData.UnLockCode = 0;
                                }
                                else
                                {
                                    string noticeMessage = Management.GetNoticeMessage("MSG_UNLOCK_CODE_WRONG");
                                    Packet stMsg = new Packet(0x168A);
                                    stMsg.WriteUInt8(NoticeType.WARNING);
                                    stMsg.WriteUnicode(noticeMessage);
                                    await session.SendToClient(stMsg);


                                    Packet stMsgx = new Packet(0x5015);
                                    stMsgx.WriteUInt8(4);
                                    await session.SendToClient(stMsgx);
                                    return new PacketResult(PacketResultType.Block);
                                }
                            }
                        }
                        else
                        {
                            string noticeMessage = Management.GetNoticeMessage("MSG_UNLOCK_CODE_EXPIRED");
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.WARNING);
                            stMsg.WriteUnicode(noticeMessage);
                            await session.SendToClient(stMsg);
                            return new PacketResult(PacketResultType.Block);
                        }
                    }
                }
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "UPDATE_MACRO_SETTING");
            }
            return new PacketResult(PacketResultType.Block);
        }

        private async Task<PacketResult> USE_FELLOW_SKILL(Packet packet, ISession session, object obj)
        {
            try
            {
                if (!session.CharScreen)
                {
                    Int64 ID64 = packet.ReadInt64();
                    string NameStrID = packet.ReadAscii();
                    byte SkillSlot = packet.ReadUInt8();
                    uint UniqueID = packet.ReadUInt32();

                    if (RefManager.m_RefFellowData.ContainsKey(NameStrID))
                    {
                        if (session.SessionData.FellowPetID64 == ID64)
                        {
                            if (SkillSlot == 1 && RefManager.m_RefFellowData[NameStrID].SkillType_1 == 1)
                            {
                                Packet LiveTitle = new Packet(0x3511, true, false);
                                LiveTitle.WriteInt32(RefManager.m_RefFellowData[NameStrID].SkillID_1);
                                LiveTitle.WriteUInt32(UniqueID);
                                //LiveTitle.WriteAscii(NameStrID);
                                LiveTitle.WriteUInt8(RefManager.m_RefFellowData[NameStrID].SkillAnimationID);
                                await session.SendToServer(LiveTitle);
                            }
                            else if (SkillSlot == 1 && RefManager.m_RefFellowData[NameStrID].SkillType_1 == 0)
                            {
                                 Packet LiveTitle = new Packet(0x3511, true, false);
                                LiveTitle.WriteInt32(RefManager.m_RefFellowData[NameStrID].SkillID_1);
                                LiveTitle.WriteUInt32(UniqueID);
                                //LiveTitle.WriteAscii(NameStrID);
                                LiveTitle.WriteUInt8(RefManager.m_RefFellowData[NameStrID].SkillAnimationID);
                                await session.SendToServer(LiveTitle);
                            }
                            else if (SkillSlot == 2)
                            {
                                Packet LiveTitle = new Packet(0x3511, true, false);
                                LiveTitle.WriteInt32(RefManager.m_RefFellowData[NameStrID].SkillID_2);
                                LiveTitle.WriteUInt32(UniqueID);
                                //LiveTitle.WriteAscii(NameStrID);
                                LiveTitle.WriteUInt8(RefManager.m_RefFellowData[NameStrID].SkillAnimationID);
                                await session.SendToServer(LiveTitle);
                            }
                            else if (SkillSlot == 3)
                            {
                                Packet LiveTitle = new Packet(0x3511, true, false);
                                LiveTitle.WriteInt32(RefManager.m_RefFellowData[NameStrID].SkillID_3);
                                LiveTitle.WriteUInt32(UniqueID);
                                //LiveTitle.WriteAscii(NameStrID);
                                LiveTitle.WriteUInt8(RefManager.m_RefFellowData[NameStrID].SkillAnimationID);
                                await session.SendToServer(LiveTitle);
                            }
                            else if (SkillSlot == 4)
                            {
                                Packet LiveTitle = new Packet(0x3511, true, false);
                                LiveTitle.WriteInt32(RefManager.m_RefFellowData[NameStrID].SkillID_4);
                                LiveTitle.WriteUInt32(UniqueID);
                                //LiveTitle.WriteAscii(NameStrID);
                                LiveTitle.WriteUInt8(RefManager.m_RefFellowData[NameStrID].SkillAnimationID);
                                await session.SendToServer(LiveTitle);
                            }
                            else if (SkillSlot == 5)
                            {
                                Packet LiveTitle = new Packet(0x3511, true, false);
                                LiveTitle.WriteInt32(RefManager.m_RefFellowData[NameStrID].SkillID_5);
                                LiveTitle.WriteUInt32(UniqueID);
                                //LiveTitle.WriteAscii(NameStrID);
                                LiveTitle.WriteUInt8(RefManager.m_RefFellowData[NameStrID].SkillAnimationID);
                                await session.SendToServer(LiveTitle);
                            }
                        }
                    }
                }
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "USE_FELLOW_SKILL");
            }
            return new PacketResult(PacketResultType.Block);
        }
        private async Task<PacketResult> SAVE_FELLOW_SKILL(Packet packet, ISession session, object obj)
        {
            try
            {
                if (!session.CharScreen)
                {
                    Int64 ID64 = packet.ReadInt64();
                    string NameStrID = packet.ReadUnicode();
                    byte LastLevel = packet.ReadUInt8();
                    byte SkillSlot = packet.ReadUInt8();
                    byte Active = packet.ReadUInt8();


                    if (RefManager.m_RefFellowData.ContainsKey(NameStrID))
                    {
                        if (session.SessionData.FellowPetID64 == ID64)
                        {
                            using (var connection = new SqlConnection(Program.Connectionstring))
                            {
                                await connection.OpenAsync();
                                string columnName = $"Enable_Skill_{SkillSlot}";

                                string query = $"UPDATE _FellowSkillData SET {columnName} = {Active} WHERE ID64 = {ID64}";
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
                Console.WriteLine(EX.Message.ToString() + "SAVE_FELLOW_SKILL");
            }
            return new PacketResult(PacketResultType.Block);
        }
        private async Task<PacketResult> GLOBAL_ITEM_LINK_CLIENT(Packet packet, ISession session, object obj)
        {
            try
            {
                if (RefManager.m_RefEventMapSettings.ContainsKey(session.SessionData.LatestRegion))
                {
                    if (RefManager.m_RefEventMapSettings[session.SessionData.LatestRegion].DisableChat)
                    {
                        string noticeMessage = Management.GetNoticeMessage("MSG_EVENT_ARENA_CHAT_DISABLED");
                        Packet stMsg = new Packet(0x168A);
                        stMsg.WriteUInt8(NoticeType.WARNING);
                        stMsg.WriteUnicode(noticeMessage);
                        await session.SendToClient(stMsg);
                        return new PacketResult(PacketResultType.Block);
                    }
                }
                int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_GLOBAL_TIME).TotalSeconds);
                if (gecensaniye < Service.GlobalDelay)
                {
                    int kalanSaniye = Service.GlobalDelay - gecensaniye;
                    string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_GLOBAL_CHAT_DELAY"), kalanSaniye);
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }

                if (session.SessionData.CurLevel < Service.GlobalLevel)
                {
                    string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_GLOBAL_CHAT_LEVEL"), Service.GlobalLevel);
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }
                session.SessionData.LAST_GLOBAL_TIME = DateTime.Now;
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "GLOBAL_ITEM_LINK_CLIENT");
            }
            return new PacketResult(PacketResultType.Nothing);
        }
        //private async Task<PacketResult> HandleIconManager(Packet packet, ISession session, object obj)
        //{
        //    try
        //    {
        //    }
        //    catch (Exception EX)
        //    {
        //        Console.WriteLine(EX.Message.ToString() + "HandleTitleManager");
        //    }
        //    return new PacketResult(PacketResultType.Block);
        //}
        private async Task<PacketResult> UPDATE_MACRO_SETTING(Packet packet, ISession session, object obj)
        {
            try
            {
                byte AutoPotion = packet.ReadUInt8();
                byte AutoSkill = packet.ReadUInt8();
                byte AutoHunt = packet.ReadUInt8();
                byte AutoPickup = packet.ReadUInt8();
                byte AutoScroll = packet.ReadUInt8();


                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();
                    string query = $"EXEC _UpdateMacroSetting {session.SessionData.Charid}, {AutoPotion}, {AutoSkill},{AutoHunt}, {AutoPickup}, {AutoScroll}";
                    using (var command = new SqlCommand(query, connection))
                    {
                        await command.ExecuteNonQueryAsync();
                    }
                }
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "UPDATE_MACRO_SETTING");
            }
            return new PacketResult(PacketResultType.Block);
        }
        private async Task<PacketResult> NEW_REVERSE_TELEPORT_PT_MEMBER(Packet packet, ISession session, object obj)
        {
            try
            {
                if (Service.DisableReverseInJob)
                {
                    if (session.SessionData.JobType != 4)
                    {
                        string noticeMessage = Management.GetNoticeMessage("MSG_REVERSE_DISABLED_JOB_MODE");
                        Packet stMsg = new Packet(0x168A);
                        stMsg.WriteUInt8(NoticeType.WARNING);
                        stMsg.WriteUnicode(noticeMessage);
                        await session.SendToClient(stMsg);
                        return new PacketResult(PacketResultType.Block);
                    }
                }
                if (session.SessionData.WorldID == 99)
                {
                    await sendNotice(session, NoticeType.WARNING, "Unable to use that item at this area.");
                    return new PacketResult(PacketResultType.Block);
                }

                // RESTART_DELAY süresini kontrol et

                int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_REVERSE_TIME).TotalSeconds);
                if (gecensaniye < Service.ReverseDelay)
                {
                    int kalanSaniye = Service.ReverseDelay - gecensaniye;
                    string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_REVERSE_DELAY"), kalanSaniye);
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }

                int reverse_Slot = packet.ReadInt32();
                short Region = packet.ReadInt16();
                int Current_x = packet.ReadInt32();
                int Current_y = packet.ReadInt32();
                int Current_z = packet.ReadInt32();
                string Charname = packet.ReadAscii();

                var pSession = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charname == Charname);
                if (pSession == null)
                    return new PacketResult(PacketResultType.Block);


                if (pSession.SessionData.WorldID != 1)
                    return new PacketResult(PacketResultType.Block);

                Packet packetx = new Packet(0x3504);
                packetx.WriteInt32(reverse_Slot);
                packetx.WriteUInt8(1);
                packetx.WriteUInt16(Region);
                packetx.WriteFloat(Current_x);
                packetx.WriteFloat(Current_y);
                packetx.WriteFloat(Current_z);
                await session.SendToServer(packetx);
                session.SessionData.LAST_REVERSE_TIME = DateTime.Now;
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "NEW_REVERSE_TELEPORT_PT_MEMBER");
            }
            return new PacketResult(PacketResultType.Block);
        }
        private async Task<PacketResult> NEW_REVERSE_TELEPORT_SAVE_LOCATION(Packet packet, ISession session, object obj)
        {
            try
            {
                if (Service.DisableReverseInJob)
                {
                    if (session.SessionData.JobType != 4)
                    {
                        string noticeMessage = Management.GetNoticeMessage("MSG_REVERSE_DISABLED_JOB_MODE");
                        Packet stMsg = new Packet(0x168A);
                        stMsg.WriteUInt8(NoticeType.WARNING);
                        stMsg.WriteUnicode(noticeMessage);
                        await session.SendToClient(stMsg);
                        return new PacketResult(PacketResultType.Block);
                    }
                }
                // RESTART_DELAY süresini kontrol et
         
                int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_REVERSE_TIME).TotalSeconds);
                if (gecensaniye < Service.ReverseDelay)
                {
                    int kalanSaniye = Service.ReverseDelay - gecensaniye; 
                    string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_REVERSE_DELAY"), kalanSaniye);
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }

                byte telslot = packet.ReadUInt8();
                byte reverse_Slot = packet.ReadUInt8();

                // Eğer gecikme dolmuşsa, LAST_RESTART_TIME güncellenir
                if (session.SessionData.CharacterNewReverseSavedLocations.ContainsKey(telslot))
                {
                    Packet packetx = new Packet(0x3504);
                    packetx.WriteInt32(reverse_Slot);
                    packetx.WriteUInt8(session.SessionData.CharacterNewReverseSavedLocations[telslot].WorldID);
                    packetx.WriteUInt16(session.SessionData.CharacterNewReverseSavedLocations[telslot].RegionID);
                    packetx.WriteFloat(session.SessionData.CharacterNewReverseSavedLocations[telslot].PosX);
                    packetx.WriteFloat(session.SessionData.CharacterNewReverseSavedLocations[telslot].PosY);
                    packetx.WriteFloat(session.SessionData.CharacterNewReverseSavedLocations[telslot].PosZ);
                    await session.SendToServer(packetx);

                    session.SessionData.LAST_REVERSE_TIME = DateTime.Now;
                    return new PacketResult(PacketResultType.Block);
                }

            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "NEW_REVERSE_TELEPORT_SAVE_LOCATION");
            }
            return new PacketResult(PacketResultType.Block);
        }
        private async Task<PacketResult> NEW_REVERSE_REMOVE_LOCATION(Packet packet, ISession session, PacketData data)
        {
            try
            {
                byte locationID = packet.ReadUInt8();

                if (session.SessionData.CharacterNewReverseSavedLocations.ContainsKey(locationID))
                {
                    session.SessionData.CharacterNewReverseSavedLocations.Remove(locationID);

                    using (var connection = new SqlConnection(Program.Connectionstring))
                    {
                        await connection.OpenAsync();
                        string query = $"DELETE FROM _NewReverseSavedLocations WHERE CharID = {session.SessionData.Charid} AND LocationID = {locationID}";
                        using (var command = new SqlCommand(query, connection))
                        {
                            await command.ExecuteNonQueryAsync();
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message.ToString() + "NEW_REVERSE_REMOVE_LOCATION");
            }

            return new PacketResult(packet, PacketResultType.Block);
        }
        private async Task<PacketResult> NEW_REVERSE_SAVE_LOCATION(Packet packet, ISession session, object obj)
        {
            try
            {
                byte locationID = packet.ReadUInt8();
                ushort Region = packet.ReadUInt16();
                int Current_x = packet.ReadInt32();
                int Current_y = packet.ReadInt32();
                int Current_z = packet.ReadInt32();
                int WorldID = packet.ReadInt32();
                if (WorldID == 1)
                {
                    if (session.SessionData.CharacterNewReverseSavedLocations.ContainsKey(locationID))
                    {
                        return new PacketResult(PacketResultType.Block);
                    }
                    else
                    {
                        var str = new _NewReverseSavedLocations();

                        str.CharID = session.SessionData.Charid;
                        str.LocationID = locationID;
                        str.RegionID = Region;
                        str.PosX = Current_x;
                        str.PosY = Current_y;
                        str.PosZ = Current_z;
                        str.WorldID = WorldID;

                        using (var connection = new SqlConnection(Program.Connectionstring))
                        {
                            await connection.OpenAsync();
                            string query = $"INSERT INTO _NewReverseSavedLocations (CharID, LocationID, RegionID, PosX, PosY, PosZ, WorldID) VALUES " +
                                $"({session.SessionData.Charid}, {locationID},{Region},{Current_x},{Current_y},{Current_z}, {WorldID})";
                            using (var command = new SqlCommand(query, connection))
                            {
                                await command.ExecuteNonQueryAsync();
                            }
                        }

                        session.SessionData.CharacterNewReverseSavedLocations.TryAdd(locationID, str);
                        Packet Info = new Packet(0x180C);
                        Info.WriteUInt8(locationID);
                        Info.WriteInt32(Region);
                        await session.SendToClient(Info);
                        // İşlem başarılı, geri dön
                        return new PacketResult(PacketResultType.Block);
                    }
                }
                else
                {
                    string noticeMessage = Management.GetNoticeMessage("NEW_REVERSE_SAVE_ERROR");
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                }
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "NEW_REVERSE_SAVE_LOCATION");
            }
            return new PacketResult(PacketResultType.Block);
        }
        public class PartyMemberData
        {
            public byte CharLevel { get; set; }
            public int RefObjID { get; set; }
            public int Mastery1 { get; set; }
            public int Mastery2 { get; set; }
        };
        private async Task<PacketResult> PARTY_MEMBER_VIEWER2(Packet packet, ISession session, object obj)
        {
            try
            {
                byte Type = packet.ReadUInt8(); // FromSlot
                string CharNameSendCommand = packet.ReadAscii(); // party Master
                if (Type == 0)
                {
                    byte partymembercount = packet.ReadUInt8();
                    if (partymembercount > 0)
                    {
                        Dictionary<int, PartyMemberData>? PartyMember = new();
                        //PartyMember.Clear();
                        for (byte iss = 0; iss < partymembercount; iss++)
                        {
                            //string PartyMemberName = _pck.ReadAscii();
                            byte CurLevel = packet.ReadUInt8();
                            int RefObjID = packet.ReadInt32(); 
                            int Mastery1 = packet.ReadInt32(); 
                            int Mastery2 = packet.ReadInt32();
                            var Memember = new PartyMemberData();

                            Memember.RefObjID = RefObjID;
                            //  CharName = PartyMemberName,
                            Memember.CharLevel = CurLevel;
                            Memember.Mastery1 = Mastery1;
                            Memember.Mastery2 = Mastery2;

                            PartyMember.TryAdd(iss, Memember);

                        }
                        Packet test = new Packet(0x178E);
                        test.WriteUInt8(1);
                        test.WriteUInt8(PartyMember.Count());

                        foreach (var line in PartyMember)
                        {
                            //test.WriteUnicode(line.Value.CharName);
                            test.WriteInt32(line.Value.RefObjID);
                            test.WriteUInt8(line.Value.CharLevel);
                            test.WriteInt32(line.Value.Mastery1);
                            test.WriteInt32(line.Value.Mastery2);
                        }
                        var Agents = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charname == CharNameSendCommand);
                        if(Agents != null)
                        {
                            await Agents.SendToClient(test);
                        }
                    }
                }
                else if (Type == 1)
                {
                    int RefObjID = packet.ReadInt32();
                    byte CurLevel = packet.ReadUInt8();
                    int mastery1 = packet.ReadInt32();
                    int mastery2 = packet.ReadInt32();

                    Packet GetPartyInfo = new Packet(0x178E);
                    GetPartyInfo.WriteUInt8(3);
                    GetPartyInfo.WriteInt32(RefObjID);
                    GetPartyInfo.WriteUInt8(CurLevel);
                    GetPartyInfo.WriteInt32(mastery1);
                    GetPartyInfo.WriteInt32(mastery2);
                    var Agents = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charname == CharNameSendCommand);
                    if (Agents != null)
                    {
                        await Agents.SendToClient(GetPartyInfo);
                    }
                }
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "HandleTitleManager");
            }
            return new PacketResult(PacketResultType.Block);
        }
        private async Task<PacketResult> PARTY_MEMBER_VIEWER(Packet packet, ISession session, object obj)
        {
            try
            {
                byte Type = packet.ReadUInt8(); // FromSlot
                string CharName = packet.ReadAscii(); // party Master

                var Agents = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charname == CharName);
                var Agentsx = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.JobName == CharName);

                if (Agents != null)
                {
                    if (Type == 0) // Call Party Info From Pt Master
                    {
                        Packet GetPartyInfo = new Packet(0x178E);
                        GetPartyInfo.WriteUInt8(0); // type
                        GetPartyInfo.WriteAscii(session.SessionData.Charname);// Message
                        await Agents.SendToClient(GetPartyInfo);
                    }
                    else if (Type == 1)
                    {
                        Packet GetPartyInfo = new Packet(0x178E);
                        GetPartyInfo.WriteUInt8(2); // typee
                        GetPartyInfo.WriteAscii(session.SessionData.Charname);// Message
                        await Agents.SendToClient(GetPartyInfo);
                    }
                }
                else if (Agentsx != null)
                {
                    if (Type == 0) // Call Party Info From Pt Master
                    {
                        Packet GetPartyInfo = new Packet(0x178E);
                        GetPartyInfo.WriteUInt8(0); // type
                        GetPartyInfo.WriteAscii(session.SessionData.Charname);// Message
                        await Agentsx.SendToClient(GetPartyInfo);
                    }
                    else if (Type == 1)
                    {
                        Packet GetPartyInfo = new Packet(0x178E);
                        GetPartyInfo.WriteUInt8(2); // type
                        GetPartyInfo.WriteAscii(session.SessionData.Charname);// Message
                        await Agentsx.SendToClient(GetPartyInfo);
                    }
                }

            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "PARTY_MEMBER_VIEWER");
            }
            return new PacketResult(PacketResultType.Block);
        }
        private async Task<PacketResult> HandleIconManager(Packet packet, ISession session, object obj)
        {
            try
            {
                byte Side = packet.ReadUInt8();
                int IconDBID = packet.ReadInt32();
                if (Side == 0)
                {
                    if (IconDBID == 0)
                    {
                        string CharName16 = session.SessionData.Charname;
                        if (RefManager.m_LeftCharIcons.ContainsKey(CharName16))
                        {
                            RefManager.m_LeftCharIcons.Remove(CharName16);
                            Packet stAckMsg = new Packet(0x174A);
                            stAckMsg.WriteAscii(CharName16);
                            await AgentServer.BroadcastPacket(stAckMsg);

                            using (var connection = new SqlConnection(Program.Connectionstring))
                            {
                                await connection.OpenAsync();
                                string query = $"DELETE FROM _ActiveIconsLeftSide WHERE CharName16 = '{session.SessionData.Charname}'";

                                using (var command = new SqlCommand(query, connection))
                                {
                                    await command.ExecuteNonQueryAsync();
                                }
                            }
                        }
                    }
                    else
                    {
                        string CharName16 = session.SessionData.Charname;
                    
                        if (session.SessionData.CharacterIcons.ContainsKey(IconDBID))
                        {
                            if (RefManager.m_LeftCharIcons.ContainsKey(CharName16) && session.SessionData.CharacterIcons[IconDBID].Side == 0)
                            {
                                if (RefManager.m_LeftCharIcons[CharName16] != IconDBID)
                                {
                                    RefManager.m_LeftCharIcons[CharName16] = IconDBID;

                                    Packet stAckMsg = new Packet(0x173F);
                                    stAckMsg.WriteAscii(CharName16);
                                    stAckMsg.WriteInt32(session.SessionData.CharacterIcons[IconDBID].IconID);
                                    await AgentServer.BroadcastPacket(stAckMsg);

                                    using (var connection = new SqlConnection(Program.Connectionstring))
                                    {
                                        await connection.OpenAsync();
                                        string query = $"EXEC _HandleIcons '{session.SessionData.Charname}', {session.SessionData.CharacterIcons[IconDBID].IconID}, 0";

                                        using (var command = new SqlCommand(query, connection))
                                        {
                                            await command.ExecuteNonQueryAsync();
                                        }
                                    }
                                }
                            }
                            else if (session.SessionData.CharacterIcons[IconDBID].Side == 0)
                            {
                                RefManager.m_LeftCharIcons.TryAdd(CharName16, IconDBID);
                                Packet stAckMsg = new Packet(0x173F, false, false);
                                stAckMsg.WriteAscii(CharName16);
                                stAckMsg.WriteInt32(IconDBID);
                                await AgentServer.BroadcastPacket(stAckMsg);

                                using (var connection = new SqlConnection(Program.Connectionstring))
                                {
                                    await connection.OpenAsync();
                                    string query = $"EXEC _HandleIcons '{session.SessionData.Charname}', {session.SessionData.CharacterIcons[IconDBID].IconID}, 0";

                                    using (var command = new SqlCommand(query, connection))
                                    {
                                        await command.ExecuteNonQueryAsync();
                                    }
                                }
                            }
                        }

                    }
                }
                else if (Side == 1)
                {
                    if (IconDBID == 0)
                    {
                        string CharName16 = session.SessionData.Charname;
                        if (RefManager.m_RightCharIcons.ContainsKey(CharName16))
                        {
                            RefManager.m_RightCharIcons.Remove(CharName16);
                            Packet stAckMsg = new Packet(0x174E);
                            stAckMsg.WriteAscii(CharName16);
                            await AgentServer.BroadcastPacket(stAckMsg);

                            using (var connection = new SqlConnection(Program.Connectionstring))
                            {
                                await connection.OpenAsync();
                                string query = $"DELETE FROM _ActiveIconsRightSide WHERE CharName16 = '{session.SessionData.Charname}'";

                                using (var command = new SqlCommand(query, connection))
                                {
                                    await command.ExecuteNonQueryAsync();
                                }
                            }
                        }
                    }
                    else
                    {
                        string CharName16 = session.SessionData.Charname;

                        if (session.SessionData.CharacterIcons.ContainsKey(IconDBID))
                        {
                            if (RefManager.m_RightCharIcons.ContainsKey(CharName16) && session.SessionData.CharacterIcons[IconDBID].Side == 1)
                            {
                                if (RefManager.m_RightCharIcons[CharName16] != IconDBID)
                                {
                                    RefManager.m_RightCharIcons[CharName16] = IconDBID;

                                    Packet stAckMsg = new Packet(0x174B);
                                    stAckMsg.WriteAscii(CharName16);
                                    stAckMsg.WriteInt32(session.SessionData.CharacterIcons[IconDBID].IconID);
                                    await AgentServer.BroadcastPacket(stAckMsg);

                                    using (var connection = new SqlConnection(Program.Connectionstring))
                                    {
                                        await connection.OpenAsync();
                                        string query = $"EXEC _HandleIcons '{session.SessionData.Charname}', {session.SessionData.CharacterIcons[IconDBID].IconID}, 1";

                                        using (var command = new SqlCommand(query, connection))
                                        {
                                            await command.ExecuteNonQueryAsync();
                                        }
                                    }
                                }
                            }
                            else if (session.SessionData.CharacterIcons[IconDBID].Side == 1)
                            {
                                RefManager.m_RightCharIcons.TryAdd(CharName16, IconDBID);
                                Packet stAckMsg = new Packet(0x174B, false, false);
                                stAckMsg.WriteAscii(CharName16);
                                stAckMsg.WriteInt32(IconDBID);
                                await AgentServer.BroadcastPacket(stAckMsg);

                                using (var connection = new SqlConnection(Program.Connectionstring))
                                {
                                    await connection.OpenAsync();
                                    string query = $"EXEC _HandleIcons '{session.SessionData.Charname}', {session.SessionData.CharacterIcons[IconDBID].IconID}, 1";

                                    using (var command = new SqlCommand(query, connection))
                                    {
                                        await command.ExecuteNonQueryAsync();
                                    }
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "HandleIconManager");
            }
            return new PacketResult(PacketResultType.Block);
        }
        private async Task<PacketResult> HandleTitleManager(Packet packet, ISession session, object obj)
        {
            try
            {
                byte type = packet.ReadUInt8();
                if (type == 0)
                {
                    int TitleID = packet.ReadInt32();
                    if (session.SessionData.State.BodyState == BodyState.Berserk)
                    {
                        return new PacketResult(PacketResultType.Block);
                    }
                    if (TitleID == 0)
                    {
                        Packet stAckMsg = new Packet(0x3501);
                        //stAckMsg.data.Write_Ascii(KEY_OF_SECRET);
                        stAckMsg.WriteUInt8(TitleID);
                        await session.SendToServer(stAckMsg);
                        return new PacketResult(PacketResultType.Block);
                    }
                    if (session.SessionData.CharTitles.Count() > 0)
                    {
                        if (session.SessionData.CharTitles.Contains(TitleID))
                        {
                            Packet stAckMsg = new Packet(0x3501);
                            stAckMsg.WriteUInt8(TitleID);
                            await session.SendToServer(stAckMsg);
                            return new PacketResult(PacketResultType.Block);
                        }
                    }
                }
                else if (type == 1)
                {
                    int DBID = packet.ReadInt32();
                    if (DBID == 0)
                    {
                        if (RefManager.m_ActiveTitleColors.ContainsKey(session.SessionData.Charname))
                        {
                            RefManager.m_ActiveTitleColors.Remove(session.SessionData.Charname);


                            Packet stAckMsg = new Packet(0x170B);
                            stAckMsg.WriteAscii(session.SessionData.Charname);
                            await AgentServer.BroadcastPacket(stAckMsg);
                            using (var connection = new SqlConnection(Program.Connectionstring))
                            {
                                await connection.OpenAsync();
                                string query = $"DELETE FROM _ActiveTitleColors WHERE CharName16='{session.SessionData.Charname}'";

                                using (var command = new SqlCommand(query, connection))
                                {
                                    await command.ExecuteNonQueryAsync();
                                }
                            }


                            return new PacketResult(PacketResultType.Block);
                        }
                    }


                    if (session.SessionData.CharacterTitleColors.ContainsKey(DBID))
                    {
                        int argbInputColor = Int32.Parse(session.SessionData.CharacterTitleColors[DBID].ColorCode.Replace("#", ""), NumberStyles.HexNumber);

                        Packet stAckMsg = new Packet(0x170A);
                        stAckMsg.WriteAscii(session.SessionData.Charname);
                        stAckMsg.WriteUInt32(argbInputColor);
                        await AgentServer.BroadcastPacket(stAckMsg);

                        if (RefManager.m_ActiveTitleColors.ContainsKey(session.SessionData.Charname))
                        {
                            RefManager.m_ActiveTitleColors.TryAdd(session.SessionData.Charname, session.SessionData.CharacterTitleColors[DBID].ColorCode);
                        }
                        else
                        {
                            RefManager.m_ActiveTitleColors[session.SessionData.Charname] = session.SessionData.CharacterTitleColors[DBID].ColorCode;
                        }

                        using (var connection = new SqlConnection(Program.Connectionstring))
                        {
                            await connection.OpenAsync();
                            string query = $"EXEC _HandleTitleColors '{session.SessionData.Charname}', {session.SessionData.CharacterTitleColors[DBID].ColorCode}";

                            using (var command = new SqlCommand(query, connection))
                            {
                                await command.ExecuteNonQueryAsync();
                            }
                        }

                        return new PacketResult(PacketResultType.Block);
                    }
                }
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "HandleTitleManager");
            }
            return new PacketResult(PacketResultType.Block);
        }

        private async Task sendNotice(ISession session, NoticeType type, string Text)
        {
            Packet stMsg = new Packet(0x168A);
            stMsg.WriteUInt8(type);
            stMsg.WriteUnicode(Text);
            await session.SendToClient(stMsg);
        }
        private async Task<PacketResult> HandleGuiPackets(Packet packet, ISession session, object obj)
        {
            try
            {
                byte type = packet.ReadUInt8();
                if (type == 0)
                {
                    if (!session.SessionData.TitleManagerPacket)
                    {
                        session.SessionData.TitleManagerPacket = true;

                        if (session.SessionData.CharTitles.Count() > 0)
                        {
                            Packet stAckMsg = new Packet(0x200A);
                            stAckMsg.WriteUInt8(session.SessionData.CharTitles.Count());

                            foreach (var stRecord in session.SessionData.CharTitles.OrderBy(x => x))
                            {
                                stAckMsg.WriteUInt8(stRecord);
                                if (session.SessionData.EUChar)
                                {

                                    if (AgentServer.RefHwan.ContainsKey(Convert.ToByte(stRecord)))
                                    {
                                        stAckMsg.WriteAscii(AgentServer.RefHwan[(Convert.ToByte(stRecord))].Title_EU70);
                                    }
                                    else
                                    {
                                        stAckMsg.WriteAscii("dummy");
                                    }
                                }
                                else if (session.SessionData.CHChar)
                                {
                                    if (AgentServer.RefHwan.ContainsKey(Convert.ToByte(stRecord)))
                                    {
                                        stAckMsg.WriteAscii(AgentServer.RefHwan[(Convert.ToByte(stRecord))].Title_CH70);
                                    }
                                    else
                                    {
                                        stAckMsg.WriteAscii("dummy");
                                    }
                                }
                            }
                            await session.SendToClient(stAckMsg);
                        }

                        if (session.SessionData.CharacterTitleColors.Count() > 0)
                        {
                            Packet stAckMsgColor = new Packet(0x200B);
                            stAckMsgColor.WriteUInt8(session.SessionData.CharacterTitleColors.Count());

                            foreach (var stRecord in session.SessionData.CharacterTitleColors.OrderBy(x => x.Value.ID))
                            {
                                stAckMsgColor.WriteInt32(stRecord.Value.ID);
                                stAckMsgColor.WriteAscii(stRecord.Value.ColorName);

                                int argbInputColor = Int32.Parse(stRecord.Value.ColorCode.Replace("#", ""), NumberStyles.HexNumber);
                                stAckMsgColor.WriteUInt32(argbInputColor);
                            }

                            await session.SendToClient(stAckMsgColor);
                        }

                    }

                }
                else if (type == 1)
                {
                    if (!session.SessionData.IconMgrPacket)
                    {
                        session.SessionData.IconMgrPacket = true;

                        int nCharDBID = session.SessionData.Charid;

                        if (session.SessionData.CharacterIcons.Count() > 0 && RefManager.m_RefIconsMediaPath.Count() > 0)
                        {
                            Packet stAckMsg = new Packet(0x201B);
                            stAckMsg.WriteUInt8(session.SessionData.CharacterIcons.Count());
                            foreach (var stRecord in session.SessionData.CharacterIcons)
                            {
                                stAckMsg.WriteInt32(stRecord.Value.IconID);
                                if (RefManager.m_RefIconsMediaPath.ContainsKey(stRecord.Key))
                                {
                                    stAckMsg.WriteAscii(RefManager.m_RefIconsMediaPath[stRecord.Key]);
                                }
                                else
                                {
                                    stAckMsg.WriteAscii("dummy");

                                }
                                stAckMsg.WriteUInt8(stRecord.Value.Side);
                            }
                            await session.SendToClient(stAckMsg);
                        }


                    }

                }
                else if (type == 2)
                {
                    if (!session.SessionData.UniqueHistoryPacket)
                    {
                        session.SessionData.UniqueHistoryPacket = true;

                        if (RefManager.UniqueLog.Count() > 0)
                        {
                            Packet stAckMsg = new Packet(0x171B);
                            stAckMsg.WriteUInt8(RefManager.UniqueLog.Count());

                            foreach (var it in RefManager.UniqueLog.OrderBy(x => x.Value.State))
                            {
                                stAckMsg.WriteInt32(it.Key);
                                stAckMsg.WriteUnicode(it.Value.KillerName);
                                stAckMsg.WriteUInt8(it.Value.State);
                                stAckMsg.WriteInt64(it.Value.Time);
                                stAckMsg.WriteInt32(it.Value.KilledRegionID);
                                stAckMsg.WriteInt32(it.Value.KilledX);
                                stAckMsg.WriteInt32(it.Value.KilledY);
                                stAckMsg.WriteInt32(it.Value.KilledZ);
                            }

                            await session.SendToClient(stAckMsg);
                        }
                    }
                }
                else if (type == 3)
                {
                    if (!session.SessionData.EventRegisterPacket)
                    {
                        session.SessionData.EventRegisterPacket = true;
                        if (RefManager.m_RefEventRegister.Count() > 0)
                        {
                            Packet stAckMsg = new Packet(0x171C);
                            stAckMsg.WriteUInt8(RefManager.m_RefEventRegister.Count());

                            foreach (var it in RefManager.m_RefEventRegister)
                            {
                                stAckMsg.WriteUInt8(it.Key);
                                stAckMsg.WriteUnicode(it.Value.Name);
                                stAckMsg.WriteUnicode(it.Value.Description);
                            }

                            await session.SendToClient(stAckMsg);
                        }
                    }
                }
                else if (type == 4)
                {
                    if (!session.SessionData.EventSchedulePacket)
                    {
                        session.SessionData.EventSchedulePacket = true;
                        if (RefManager.m_RefEventSchedule.Count() > 0)
                        {
                            Packet stAckMsg = new Packet(0x171E);
                            stAckMsg.WriteUInt8(RefManager.m_RefEventSchedule.Count());

                            foreach (var it in RefManager.m_RefEventSchedule)
                            {
                                stAckMsg.WriteInt32(it.Value.ID);
                                stAckMsg.WriteUnicode(it.Value.EventName);
                                stAckMsg.WriteUInt8(it.Value.Day);
                                stAckMsg.WriteUnicode(it.Value.Time);
                            }

                            await session.SendToClient(stAckMsg);
                        }
                    }
                }
                else if (type == 5)
                {
                    if (!session.SessionData.ChestPacket)
                    {
                        session.SessionData.ChestPacket = true;

                        if (session.SessionData.CharacterChest.Count() != 0)
                        {
                            Packet stAckMsg = new Packet(0x205E);
                            stAckMsg.WriteInt32(session.SessionData.CharacterChest.Count());
                            foreach (var stRecord in session.SessionData.CharacterChest)
                            {
                                stAckMsg.WriteInt32(stRecord.Value.ID);
                                stAckMsg.WriteInt32(stRecord.Value.ItemID);
                                stAckMsg.WriteInt32(stRecord.Value.Quantity);
                                stAckMsg.WriteAscii(stRecord.Value.Date ?? string.Empty);
                                stAckMsg.WriteAscii(stRecord.Value.Type ?? string.Empty);
                                stAckMsg.WriteUInt8(stRecord.Value.Plus);
                            }
                            await session.SendToClient(stAckMsg);
                        }
                    }
                }
                else if (type == 6)
                {
                    if (RefManager.m_ActiveTitleNameNew.ContainsKey(session.SessionData.Charname))
                    {
                        RefManager.m_ActiveTitleNameNew.Remove(session.SessionData.Charname);

                        Packet stAckMsg = new Packet(0x202C);
                        stAckMsg.WriteAscii(session.SessionData.Charname);
                        await AgentServer.BroadcastPacket(stAckMsg);

                        using (var connection = new SqlConnection(Program.Connectionstring))
                        {
                            await connection.OpenAsync();
                            string query = $"DELETE FROM _ActiveTitleNameNew WHERE CharName16 = '{session.SessionData.Charname}'";

                            using (var command = new SqlCommand(query, connection))
                            {
                                await command.ExecuteNonQueryAsync();
                            }
                        }

                    }
                }
                else if (type == 7)
                {
                    int DBID = packet.ReadInt32();

                    if (session.SessionData.CharacterChest.ContainsKey(DBID))
                    {
                        using (var connection = new SqlConnection(Program.Connectionstring))
                        {
                            await connection.OpenAsync();
                            string query = $"DELETE FROM _ItemChest WHERE CharID = {session.SessionData.Charid} and ID = {DBID}";

                            using (var command = new SqlCommand(query, connection))
                            {
                                await command.ExecuteNonQueryAsync();
                            }
                        }

                        Packet packetx = new Packet(0x3506);
                        packetx.WriteAscii(session.SessionData.CharacterChest[DBID].ItemCodeName);
                        packetx.WriteInt32(session.SessionData.CharacterChest[DBID].Quantity);
                        packetx.WriteUInt8(0);
                        packetx.WriteUInt8(session.SessionData.CharacterChest[DBID].Plus);
                        await session.SendToServer(packetx);
                        session.SessionData.CharacterChest.Remove(DBID);

                    }
                }
                else if (type == 8)
                {
                    int AchievementID = packet.ReadInt32();
                    if (session.SessionData.CharacterAchievement.ContainsKey(AchievementID))
                    {
                        if (session.SessionData.CharacterAchievement[AchievementID].State == 1)
                        {
                            if (RefManager.m_RefAchievements[AchievementID].RewardType == 0)
                            {
                                byte TitleID = RefManager.m_RefAchievements[AchievementID].RewardTitleID;
                                if (RefManager.m_RefTitleNameNew.ContainsKey(TitleID))
                                {
                                    if (RefManager.m_ActiveTitleNameNew.ContainsKey(session.SessionData.Charname))
                                    {
                                        RefManager.m_ActiveTitleNameNew[session.SessionData.Charname] = TitleID;
                                    }
                                    else
                                    {
                                        RefManager.m_ActiveTitleNameNew.TryAdd(session.SessionData.Charname, TitleID);
                                    }
                                    Packet stAckMsg = new Packet(0x202B);
                                    stAckMsg.WriteAscii(session.SessionData.Charname);
                                    stAckMsg.WriteAscii(RefManager.m_RefTitleNameNew[TitleID]);
                                    await AgentServer.BroadcastPacket(stAckMsg);
                                    using (var connection = new SqlConnection(Program.Connectionstring))
                                    {
                                        await connection.OpenAsync();
                                        string query = $"EXEC _HandleNewTitles '{session.SessionData.Charname}', {TitleID}";

                                        using (var command = new SqlCommand(query, connection))
                                        {
                                            await command.ExecuteNonQueryAsync();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (type == 10)
                {
                    int RefRewardDbID = packet.ReadInt32();

                    if (RefManager.m_AttendanceReward.ContainsKey(RefRewardDbID))
                    {
                        IEnumerable<_AttendanceRewardLog> rewardlog;
                        using (var connection = new SqlConnection(Program.Connectionstring))
                        {
                            await connection.OpenAsync();

                            rewardlog = await connection.QueryAsync<_AttendanceRewardLog>($"SELECT * FROM _AttendanceRewardLog with (nolock) WHERE CharID = {session.SessionData.Charid} AND CanTake = 1 AND AlreadyTaken = 0");


                            if (rewardlog.Count() > 0)
                            {
                                foreach (var line in rewardlog)
                                {
                                    if (line.RefRewardID == RefRewardDbID)
                                    {
                                        await sqlQueryHelper.AddItemToChest(session.SessionData.Charid, RefManager.m_AttendanceReward[RefRewardDbID].ItemCodeName128,
                                            RefManager.m_AttendanceReward[RefRewardDbID].ItemCount, "Daily Login Event", 0);

                                        string query = $"UPDATE _AttendanceRewardLog SET CanTake=0, AlreadyTaken =1 WHERE  CharID = {session.SessionData.Charid} and RefRewardID = {RefRewardDbID}";

                                        using (var command = new SqlCommand(query, connection))
                                        {
                                            await command.ExecuteNonQueryAsync();

                                        }
                                        Packet stMsg = new Packet(0x169C);
                                        stMsg.WriteUInt8(12);
                                        stMsg.WriteUnicode("item [%ls] added to chest.");
                                        stMsg.WriteInt32(RefManager.m_AttendanceReward[RefRewardDbID].ItemID);
                                        await session.SendToClient(stMsg);
                                    }
                                }


                                var rewardlogsecond = await connection.QueryAsync<_AttendanceRewardLog>($"SELECT * FROM _AttendanceRewardLog with (nolock) WHERE CharID = {session.SessionData.Charid} AND CanTake = 1 AND AlreadyTaken = 0");
                                if (rewardlogsecond.Count() > 0)
                                {
                                    Packet logpacket = new Packet(0x209A);
                                    logpacket.WriteUInt8(rewardlogsecond.Count());
                                    foreach (var line in rewardlogsecond)
                                    {
                                        logpacket.WriteInt32(line.RefRewardID);
                                    }
                                    await session.SendToClient(logpacket);
                                }
                            }
                        }
                    }
                }
                else if (type == 11)
                {
                    int ItemMallDbID = packet.ReadInt32();
                    int Quantity = packet.ReadInt32();

                    if (RefManager.m_RefNewItemMall.ContainsKey(ItemMallDbID))
                    {
                        int Price = RefManager.m_RefNewItemMall[ItemMallDbID].Silk;

                        // Silk miktarını sorgula ve güncelle

                        int CurrentSilk = await sqlQueryHelper.prod_int($"SELECT silk_own FROM {Service.AccountDB}..SK_Silk WITH (NOLOCK) WHERE JID = {session.SessionData.JID}", Program.Connectionstring);

                        if (CurrentSilk != -1 && CurrentSilk >= Price * Quantity)
                        {
                            // Yeterli silk varsa güncelle
                            int newSilkAmount = CurrentSilk - Price * Quantity;
                            using (var connection = new SqlConnection(Program.Connectionstring))
                            {
                                await connection.OpenAsync();
                                string query = $"EXEC _OnNewItemMallPurchased_EDIT {session.SessionData.JID}, {session.SessionData.Charid}, '{session.SessionData.Charname}', {RefManager.m_RefNewItemMall[ItemMallDbID].ItemID}, {Price * Quantity}";
                                using (var command = new SqlCommand(query, connection))
                                {
                                    await command.ExecuteNonQueryAsync();
                                }
                                string query2 = $"UPDATE SRO_VT_ACCOUNT..SK_Silk SET silk_own = {newSilkAmount} WHERE JID = {session.SessionData.JID}";
                                using (var command = new SqlCommand(query2, connection))
                                {
                                    await command.ExecuteNonQueryAsync();
                                }
                            }
                            Packet packeta = new Packet(0x3527);
                            packeta.WriteInt32(newSilkAmount);
                            await session.SendToServer(packeta);

                            int i = 0;
                            while (i < Quantity && Quantity > 0)
                            {
                                i++;

                                await sqlQueryHelper.AddItemToChest(session.SessionData.Charid, RefManager.m_RefNewItemMall[ItemMallDbID].CodeName128,
                                RefManager.m_RefNewItemMall[ItemMallDbID].ItemCount, "New Item Mall", 0);

                                Packet stMsg = new Packet(0x169C);
                                stMsg.WriteUInt8(12);
                                stMsg.WriteUnicode("item [%ls] added to chest.");
                                stMsg.WriteInt32(RefManager.m_RefNewItemMall[ItemMallDbID].ItemID);
                                await session.SendToClient(stMsg);
                            }
                        }
                        else
                        {
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.WARNING);
                            stMsg.WriteUnicode("This purchase cannot complete due to insufficient silks.");
                            await session.SendToClient(stMsg);
                        }

                    }
                }
                else if (type == 12)
                {
                    int ItemMallDbID = packet.ReadInt32();

                    if (RefManager.m_RefNewAvatarMall.ContainsKey(ItemMallDbID))
                    {
                        int Price = RefManager.m_RefNewAvatarMall[ItemMallDbID].Silk;

                        int CurrentSilk = await sqlQueryHelper.prod_int($"SELECT silk_own FROM {Service.AccountDB}..SK_Silk WITH (NOLOCK) WHERE JID = {session.SessionData.JID}", Program.Connectionstring);

                        if (CurrentSilk != -1 && CurrentSilk >= Price)
                        {
                            // Yeterli silk varsa güncelle
                            int newSilkAmount = CurrentSilk - Price;
                            using (var connection = new SqlConnection(Program.Connectionstring))
                            {
                                await connection.OpenAsync();
                                string query = $"EXEC _OnNewItemMallPurchased_EDIT {session.SessionData.JID}, {session.SessionData.Charid}, '{session.SessionData.Charname}',{RefManager.m_RefNewAvatarMall[ItemMallDbID].ItemID}, {Price}";
                                using (var command = new SqlCommand(query, connection))
                                {
                                    await command.ExecuteNonQueryAsync();
                                }

                                string query2 = $"UPDATE SRO_VT_ACCOUNT..SK_Silk SET silk_own = {newSilkAmount} WHERE JID = {session.SessionData.JID}";
                                using (var command = new SqlCommand(query2, connection))
                                {
                                    await command.ExecuteNonQueryAsync();
                                }
                            }

                            // Paket gönderme işlemi
                            Packet packeta = new Packet(0x3527);
                            packeta.WriteInt32(newSilkAmount);
                            await session.SendToServer(packeta);


                            await sqlQueryHelper.AddItemToChest(session.SessionData.Charid, RefManager.m_RefNewAvatarMall[ItemMallDbID].CodeName128,
                                1, "New Avatar Mall", 0);



                            Packet stMsg = new Packet(0x169C);
                            stMsg.WriteUInt8(12);
                            stMsg.WriteUnicode("item [%ls] added to chest.");
                            stMsg.WriteInt32(RefManager.m_RefNewAvatarMall[ItemMallDbID].ItemID);
                            await session.SendToClient(stMsg);
                        }
                        else
                        {
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.WARNING);
                            stMsg.WriteUnicode("This purchase cannot complete due to insufficient silks.");
                            await session.SendToClient(stMsg);
                        }

                    }
                }
                else if (type == 13)
                {
                    if (!session.SessionData.AttendancePacket)
                    {
                        session.SessionData.AttendancePacket = true;

                        Packet logpacket = new Packet(0x208C);
                        logpacket.WriteUInt8(session.SessionData.AttendanceDayCount);
                        await session.SendToClient(logpacket);


                        IEnumerable<_AttendanceRewardLog> rewardlog;
                        using (var connection = new SqlConnection(Program.Connectionstring))
                        {
                            await connection.OpenAsync();
                            rewardlog = await connection.QueryAsync<_AttendanceRewardLog>($"SELECT * FROM _AttendanceRewardLog with (nolock) WHERE CharID = {session.SessionData.Charid} AND CanTake = 1 AND AlreadyTaken = 0");
                        }
                        if (rewardlog.Count() > 0)
                        {
                            Packet logpacketx = new Packet(0x209A);
                            logpacketx.WriteUInt8(rewardlog.Count());
                            foreach (var line in rewardlog)
                            {
                                logpacketx.WriteInt32(line.RefRewardID);
                            }
                            await session.SendToClient(logpacketx);

                        }
                    }
                }
                else if (type == 14)
                {
                    DateTime currentTime = DateTime.Now;
                    string currentDate = currentTime.ToString("yyyy-MM-dd");

                    if (session.SessionData.LastAttendedDate != currentDate)
                    {
                        session.SessionData.LastAttendedDate = currentDate;

                        int Result = -1;
                        using (var connection = new SqlConnection(Program.Connectionstring))
                        {
                            await connection.OpenAsync();

                            using (var command = new SqlCommand("[dbo].[_AttendanceEvent]", connection))
                            {
                                command.CommandType = CommandType.StoredProcedure;

                                command.Parameters.AddWithValue("@CharID", session.SessionData.Charid);
                                command.Parameters.AddWithValue("@Date", currentDate);

                                var returnValueParam = new SqlParameter("@ReturnValue", SqlDbType.Int)
                                {
                                    Direction = ParameterDirection.Output
                                };
                                command.Parameters.Add(returnValueParam);

                                await command.ExecuteNonQueryAsync();

                                Result = (int)returnValueParam.Value;
                            }
                        }
                        if (Result == 1)
                        {
                            session.SessionData.AttendanceDayCount++;

                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.YELLOW_RIGHT);
                            stMsg.WriteUnicode("Attended.Cheer up your day!!");
                            await session.SendToClient(stMsg);



                            Packet logpacket = new Packet(0x208C);
                            logpacket.WriteUInt8(session.SessionData.AttendanceDayCount);
                            await session.SendToClient(logpacket);


                            IEnumerable<_AttendanceRewardLog> rewardlog;
                            using (var connection = new SqlConnection(Program.Connectionstring))
                            {
                                await connection.OpenAsync();
                                rewardlog = await connection.QueryAsync<_AttendanceRewardLog>($"SELECT * FROM _AttendanceRewardLog with (nolock) WHERE CharID = {session.SessionData.Charid} AND CanTake = 1 AND AlreadyTaken = 0");
                            }

                            if (rewardlog.Count() > 0)
                            {
                                Packet logpacket2 = new Packet(0x209A);
                                logpacket2.WriteUInt8(rewardlog.Count());
                                foreach (var line in rewardlog)
                                {
                                    logpacket2.WriteInt32(line.RefRewardID);
                                }
                                await session.SendToClient(logpacket2);

                            }
                        }
                    }
                }
                else if (type == 15)
                {
                    byte datasize = packet.ReadUInt8();
                    for (int i = 0; i < datasize; i++)
                    {
                        byte Slot = packet.ReadUInt8();
                        byte Activate = packet.ReadUInt8();
                        byte Value = packet.ReadUInt8();
                        using (var connection = new SqlConnection(Program.Connectionstring))
                        {
                            await connection.OpenAsync();
                            string query = $"EXEC _SaveMacroAutoPotionSetting {session.SessionData.Charid}, {Slot}, {Activate}, {Value}";
                            using (var command = new SqlCommand(query, connection))
                            {
                                await command.ExecuteNonQueryAsync();
                            }
                        }
                    }
                }
                else if (type == 16)
                {
                    byte RefEventID = packet.ReadUInt8();
                    if (RefEventID == 1)
                    {
                        if (AgentServer.eventManager.survivalArena.SurvivalArenaEventRegisters)
                        {
                            if (AgentServer.eventManager.survivalArena.SurvivalArenaLogger.ContainsKey(session.SessionData.Charname))
                            {
                                string closedmsg = Management.GetNoticeMessage("MSG_ALREADY_REGISTER");
                                await sendNotice(session, NoticeType.QUEST, closedmsg);
                                return new PacketResult(packet, PacketResultType.Block);
                            }
                            else
                            {
                                if (AgentServer.eventManager.survivalArena.SurvivalArenaLogger.Count() > 0)
                                {
                                    int SizeOfSameHwid = 0;
                                    foreach (var Users in AgentServer.eventManager.survivalArena.SurvivalArenaLogger)
                                    {
                                        if (Users.Value.Hwid == session.SessionData.Hwid)
                                        {
                                            SizeOfSameHwid++;
                                        }
                                    }
                                    if (SizeOfSameHwid >= AgentServer.eventManager.survivalArena.m_SurvivalArenaSetting[0].PCLimit)
                                    {
                                        string closedmsg = Management.GetNoticeMessage("MSG_EVENT_HWID_REACHED");
                                        await sendNotice(session, NoticeType.QUEST, closedmsg);
                                        return new PacketResult(packet, PacketResultType.Block);
                                    }
                                }
                                if (session.SessionData.JobType == 4)
                                {
                                    var str = new SSurvivalArenaLogger();
                                    str.Kill = 0;
                                    str.CharID = session.SessionData.Charid;
                                    str.Hwid = session.SessionData.Hwid;
                                    AgentServer.eventManager.survivalArena.SurvivalArenaLogger.TryAdd(session.SessionData.Charname, str);

                                    string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_SUCCESSFULL");
                                    await sendNotice(session, NoticeType.QUEST, closedmsg);
                                    return new PacketResult(packet, PacketResultType.Block);
                                }
                                else
                                {
                                    string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_NON_JOB");
                                    await sendNotice(session, NoticeType.QUEST, closedmsg);
                                    return new PacketResult(packet, PacketResultType.Block);
                                }
                            }
                        }
                        else
                        {
                            string closedmsg = Management.GetNoticeMessage("MSG_EVENT_REGISTER_CURRENTLY_CLOSED");
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.QUEST);
                            stMsg.WriteUnicode(closedmsg);
                            await session.SendToClient(stMsg);
                            return new PacketResult(packet, PacketResultType.Block);
                        }
                    }
                    else if (RefEventID == 2)
                    {
                        if (AgentServer.eventManager.lastManStanding.LMSEventRegisters)
                        {
                            if (AgentServer.eventManager.lastManStanding.LMSLogger.ContainsKey(session.SessionData.Charname))
                            {
                                string closedmsg = Management.GetNoticeMessage("MSG_ALREADY_REGISTER");
                                await sendNotice(session, NoticeType.QUEST, closedmsg);
                                return new PacketResult(packet, PacketResultType.Block);
                            }
                            else
                            {
                                if (AgentServer.eventManager.lastManStanding.LMSLogger.Count() > 0)
                                {
                                    int SizeOfSameHwid = 0;
                                    foreach (var Users in AgentServer.eventManager.lastManStanding.LMSLogger)
                                    {
                                        if (Users.Value.Hwid == session.SessionData.Hwid)
                                        {
                                            SizeOfSameHwid++;
                                        }
                                    }
                                    if (SizeOfSameHwid >= AgentServer.eventManager.lastManStanding.m_LMSSetting[0].PCLimit)
                                    {
                                        string closedmsg = Management.GetNoticeMessage("MSG_EVENT_HWID_REACHED");
                                        await sendNotice(session, NoticeType.QUEST, closedmsg);
                                        return new PacketResult(packet, PacketResultType.Block);
                                    }
                                }
                                if (session.SessionData.JobType == 4)
                                {
                                    var str = new SLMSLogger();
                                    str.Kill = 0;
                                    str.CharID = session.SessionData.Charid;
                                    str.Hwid = session.SessionData.Hwid;
                                    AgentServer.eventManager.lastManStanding.LMSLogger.TryAdd(session.SessionData.Charname, str);

                                    string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_SUCCESSFULL");
                                    await sendNotice(session, NoticeType.QUEST, closedmsg);
                                    return new PacketResult(packet, PacketResultType.Block);
                                }
                                else
                                {
                                    string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_NON_JOB");
                                    await sendNotice(session, NoticeType.QUEST, closedmsg);
                                    return new PacketResult(packet, PacketResultType.Block);
                                }
                            }
                        }
                        else
                        {
                            string closedmsg = Management.GetNoticeMessage("MSG_EVENT_REGISTER_CURRENTLY_CLOSED");
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.QUEST);
                            stMsg.WriteUnicode(closedmsg);
                            await session.SendToClient(stMsg);
                            return new PacketResult(packet, PacketResultType.Block);
                        }
                    }
                    else if (RefEventID == 3)
                    {
                        if (AgentServer.eventManager.defendTower.DefendTowerRegisterStatus)
                        {
                            if (AgentServer.eventManager.defendTower.DefendTowerLogger.ContainsKey(session.SessionData.Charname))
                            {
                                string closedmsg = Management.GetNoticeMessage("MSG_ALREADY_REGISTER");
                                await sendNotice(session, NoticeType.QUEST, closedmsg);
                                return new PacketResult(packet, PacketResultType.Block);
                            }
                            else
                            {
                                if (AgentServer.eventManager.defendTower.DefendTowerLogger.Count() > 0)
                                {
                                    int SizeOfSameHwid = 0;
                                    foreach (var Users in AgentServer.eventManager.defendTower.DefendTowerLogger)
                                    {
                                        if (Users.Value.Hwid == session.SessionData.Hwid)
                                        {
                                            SizeOfSameHwid++;
                                        }
                                    }
                                    if (SizeOfSameHwid >= AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].PCLimit)
                                    {
                                        string closedmsg = Management.GetNoticeMessage("MSG_EVENT_HWID_REACHED");
                                        await sendNotice(session, NoticeType.QUEST, closedmsg);
                                        return new PacketResult(packet, PacketResultType.Block);
                                    }
                                }
                                if (session.SessionData.JobType == 4)
                                {
                                    if (AgentServer.eventManager.defendTower.DefendTowerLogger.Count() % 2 == 0)
                                    {
                                        var str = new SDefendTowerLogger();
                                        str.Kill = 0;
                                        str.CharID = session.SessionData.Charid;
                                        str.Hwid = session.SessionData.Hwid;
                                        str.Team = 3;
                                        AgentServer.eventManager.defendTower.DefendTowerLogger.TryAdd(session.SessionData.Charname, str);
                                        string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_SUCCESSFULL");
                                        await sendNotice(session, NoticeType.QUEST, closedmsg);
                                        return new PacketResult(packet, PacketResultType.Block);
                                    }
                                    else
                                    {
                                        var str = new SDefendTowerLogger();
                                        str.Kill = 0;
                                        str.CharID = session.SessionData.Charid;
                                        str.Hwid = session.SessionData.Hwid;
                                        str.Team = 1;
                                        AgentServer.eventManager.defendTower.DefendTowerLogger.TryAdd(session.SessionData.Charname, str);
                                        string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_SUCCESSFULL");
                                        await sendNotice(session, NoticeType.QUEST, closedmsg);
                                        return new PacketResult(packet, PacketResultType.Block);
                                    }
                                }
                                else
                                {
                                    string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_NON_JOB");
                                    await sendNotice(session, NoticeType.QUEST, closedmsg);
                                    return new PacketResult(packet, PacketResultType.Block);
                                }
                            }
                        }
                        else
                        {
                            string closedmsg = Management.GetNoticeMessage("MSG_EVENT_REGISTER_CURRENTLY_CLOSED");
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.QUEST);
                            stMsg.WriteUnicode(closedmsg);
                            await session.SendToClient(stMsg);
                            return new PacketResult(packet, PacketResultType.Block);

                        }
                    }
                    else if (RefEventID == 4)
                    {
                        if (AgentServer.eventManager.styriaClash.StyriaClashRegisterStatus)
                        {
                            if (AgentServer.eventManager.styriaClash.StyriaClashLogger.ContainsKey(session.SessionData.Charname))
                            {
                                string closedmsg = Management.GetNoticeMessage("MSG_ALREADY_REGISTER");
                                await sendNotice(session, NoticeType.QUEST, closedmsg);
                                return new PacketResult(packet, PacketResultType.Block);
                            }
                            else
                            {
                                if (AgentServer.eventManager.styriaClash.StyriaClashLogger.Count() > 0)
                                {
                                    int SizeOfSameHwid = 0;
                                    foreach (var Users in AgentServer.eventManager.styriaClash.StyriaClashLogger)
                                    {
                                        if (Users.Value.Hwid == session.SessionData.Hwid)
                                        {
                                            SizeOfSameHwid++;
                                        }
                                    }
                                    if (SizeOfSameHwid >= AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].PCLimit)
                                    {
                                        string closedmsg = Management.GetNoticeMessage("MSG_EVENT_HWID_REACHED");
                                        await sendNotice(session, NoticeType.QUEST, closedmsg);
                                        return new PacketResult(packet, PacketResultType.Block);
                                    }
                                }
                                if (session.SessionData.JobType != 4)
                                {
                                    if (session.SessionData.JobType == 2) // its teaf
                                    {
                                        var str = new SStyriaClashLogger();
                                        str.Kill = 0;
                                        str.CharID = session.SessionData.Charid;
                                        str.Hwid = session.SessionData.Hwid;
                                        str.Team = 1;
                                        str.JobName = session.SessionData.JobName;
                                        AgentServer.eventManager.styriaClash.StyriaClashLogger.TryAdd(session.SessionData.Charname, str);

                                        string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_SUCCESSFULL");
                                        await sendNotice(session, NoticeType.QUEST, closedmsg);
                                        return new PacketResult(packet, PacketResultType.Block);
                                    }
                                    else if (session.SessionData.JobType == 1 || session.SessionData.JobType == 3) /// hunter trader
                                    {
                                        var str = new SStyriaClashLogger();
                                        str.Kill = 0;
                                        str.CharID = session.SessionData.Charid;
                                        str.Hwid = session.SessionData.Hwid;
                                        str.Team = 3;
                                        str.JobName = session.SessionData.JobName;
                                        AgentServer.eventManager.styriaClash.StyriaClashLogger.TryAdd(session.SessionData.Charname, str);

                                        string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_SUCCESSFULL");
                                        await sendNotice(session, NoticeType.QUEST, closedmsg);
                                        return new PacketResult(packet, PacketResultType.Block);
                                    }
                                }
                                else
                                {
                                    string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_CON_JOB");
                                    await sendNotice(session, NoticeType.QUEST, closedmsg);
                                    return new PacketResult(packet, PacketResultType.Block);
                                }
                            }
                        }
                        else
                        {
                            string closedmsg = Management.GetNoticeMessage("MSG_EVENT_REGISTER_CURRENTLY_CLOSED");
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.QUEST);
                            stMsg.WriteUnicode(closedmsg);
                            await session.SendToClient(stMsg);
                            return new PacketResult(packet, PacketResultType.Block);

                        }
                    }

                    else if (RefEventID == 5)
                    {
                        if (AgentServer.eventManager.lotterySilk.RegisterStatus)
                        {
                            if (AgentServer.eventManager.lotterySilk.LotterySilkLogger.ContainsKey(session.SessionData.Charname))
                            {
                                string closedmsg = Management.GetNoticeMessage("MSG_ALREADY_REGISTER");
                                await sendNotice(session, NoticeType.QUEST, closedmsg);
                                return new PacketResult(packet, PacketResultType.Block);
                            }
                            else
                            {
                                int CurrentSilk = await sqlQueryHelper.prod_int($"SELECT silk_own FROM {Service.AccountDB}..SK_Silk WITH (NOLOCK) WHERE JID = {session.SessionData.JID}", Program.Connectionstring);


                                if (CurrentSilk != -1 && CurrentSilk >= AgentServer.eventManager.lotterySilk.RegisterSilk)
                                {
                                    // Yeterli silk varsa güncelle
                                    int newSilkAmount = CurrentSilk - AgentServer.eventManager.lotterySilk.RegisterSilk;
                                    using (var connection = new SqlConnection(Program.Connectionstring))
                                    {
                                        await connection.OpenAsync();
                                        string query2 = $"UPDATE SRO_VT_ACCOUNT..SK_Silk SET silk_own = {newSilkAmount} WHERE JID = {session.SessionData.JID}";
                                        using (var command = new SqlCommand(query2, connection))
                                        {
                                            await command.ExecuteNonQueryAsync();
                                        }
                                    }

                                    Packet packeta = new Packet(0x3527);
                                    packeta.WriteInt32(newSilkAmount);
                                    await session.SendToServer(packeta);

                                    AgentServer.eventManager.lotterySilk.LotterySilkLogger.TryAdd(session.SessionData.Charname, session.SessionData.Charid);
                                    string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_SUCCESSFULL");
                                    await sendNotice(session, NoticeType.QUEST, closedmsg);
                                    return new PacketResult(packet, PacketResultType.Block);
                                }
                                else
                                {
                                    await sendNotice(session, NoticeType.QUEST, "This registration cannot complete due to insufficient silks.");
                                    return new PacketResult(packet, PacketResultType.Block);
                                }
                            }
                        }
                        else
                        {
                            string closedmsg = Management.GetNoticeMessage("MSG_EVENT_REGISTER_CURRENTLY_CLOSED");
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.QUEST);
                            stMsg.WriteUnicode(closedmsg);
                            await session.SendToClient(stMsg);
                            return new PacketResult(packet, PacketResultType.Block);
                        }
                    }
                    else if (RefEventID == 6)
                    {
                        if (AgentServer.eventManager.lotteryGold.RegisterStatus)
                        {
                            if (AgentServer.eventManager.lotteryGold.LotteryGoldLogger.ContainsKey(session.SessionData.Charname))
                            {
                                string closedmsg = Management.GetNoticeMessage("MSG_ALREADY_REGISTER");
                                await sendNotice(session, NoticeType.QUEST, closedmsg);
                                return new PacketResult(packet, PacketResultType.Block);
                            }
                            else
                            {
                                Int64 Currentgold = -1;

                                using (var connection = new SqlConnection(Program.Connectionstring))
                                {
                                    Currentgold = await connection.QueryFirstOrDefaultAsync<Int64>($"SELECT RemainGold FROM {Service.ShardDB}.._Char WHERE CharID = {session.SessionData.Charid}", Program.Connectionstring);
                                }
                                if (Currentgold != -1 && Currentgold >= AgentServer.eventManager.lotteryGold.RegisterGold)
                                {
                                    // Yeterli silk varsa güncelle
                                    Packet packets = new Packet(0x3507);
                                    packets.WriteInt64(AgentServer.eventManager.lotteryGold.RegisterGold);
                                    await session.SendToServer(packets);

                                    AgentServer.eventManager.lotteryGold.LotteryGoldLogger.TryAdd(session.SessionData.Charname, session.SessionData.Charid);
                                    string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_SUCCESSFULL");
                                    await sendNotice(session, NoticeType.QUEST, closedmsg);
                                    return new PacketResult(packet, PacketResultType.Block);
                                }
                                else
                                {
                                    await sendNotice(session, NoticeType.QUEST, "This registration cannot complete due to insufficient golds.");
                                    return new PacketResult(packet, PacketResultType.Block);
                                }
                            }
                        }
                        else
                        {
                            string closedmsg = Management.GetNoticeMessage("MSG_EVENT_REGISTER_CURRENTLY_CLOSED");
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.QUEST);
                            stMsg.WriteUnicode(closedmsg);
                            await session.SendToClient(stMsg);
                            return new PacketResult(packet, PacketResultType.Block);
                        }
                    }
                }
                else if (type == 17)
                {
                    byte RefEventID = packet.ReadUInt8();
                    if (RefEventID == 1)
                    {
                        if (AgentServer.eventManager.survivalArena.SurvivalArenaEventRegisters)
                        {
                            if (AgentServer.eventManager.survivalArena.SurvivalArenaLogger.ContainsKey(session.SessionData.Charname))
                            {
                                AgentServer.eventManager.survivalArena.SurvivalArenaLogger.TryRemove(session.SessionData.Charname, out SSurvivalArenaLogger value);
                                string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_REMOVED");
                                await sendNotice(session, NoticeType.QUEST, closedmsg);
                                return new PacketResult(PacketResultType.Block);
                            }
                        }
                    }
                    else if (RefEventID == 2)
                    {
                        if (AgentServer.eventManager.lastManStanding.LMSEventRegisters)
                        {
                            if (AgentServer.eventManager.lastManStanding.LMSLogger.ContainsKey(session.SessionData.Charname))
                            {
                                AgentServer.eventManager.lastManStanding.LMSLogger.TryRemove(session.SessionData.Charname, out SLMSLogger value);
                                string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_REMOVED");
                                await sendNotice(session, NoticeType.QUEST, closedmsg);
                                return new PacketResult(PacketResultType.Block);
                            }
                        }
                    }
                    else if (RefEventID == 3)
                    {
                        if (AgentServer.eventManager.defendTower.DefendTowerRegisterStatus)
                        {
                            if (AgentServer.eventManager.defendTower.DefendTowerLogger.ContainsKey(session.SessionData.Charname))
                            {
                                AgentServer.eventManager.defendTower.DefendTowerLogger.TryRemove(session.SessionData.Charname, out SDefendTowerLogger value);
                                string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_REMOVED");
                                await sendNotice(session, NoticeType.QUEST, closedmsg);
                                return new PacketResult(PacketResultType.Block);
                            }
                        }
                    }
                    else if (RefEventID == 4)
                    {
                        if (AgentServer.eventManager.styriaClash.StyriaClashRegisterStatus)
                        {
                            if (AgentServer.eventManager.styriaClash.StyriaClashLogger.ContainsKey(session.SessionData.Charname))
                            {
                                AgentServer.eventManager.styriaClash.StyriaClashLogger.TryRemove(session.SessionData.Charname, out SStyriaClashLogger value);
                                string closedmsg = Management.GetNoticeMessage("MSG_REGISTER_REMOVED");
                                await sendNotice(session, NoticeType.QUEST, closedmsg);
                                return new PacketResult(PacketResultType.Block);
                            }
                        }
                    }
                }
                else if (type == 18)
                {
                    uint FellowUniqueID = packet.ReadUInt32();
                    byte FellowLevel = packet.ReadUInt8();
                    if (session.SessionData.FellowPetUniqueID == FellowUniqueID && session.SessionData.FellowItemID != 0)
                    {
                        foreach (var data in RefManager.m_RefFellowData)
                        {
                            if (data.Value.ItemID == session.SessionData.FellowItemID && data.Value.SelfSkill_1 > 0 && FellowLevel >= data.Value.SelfSkill_Active_Level_1)
                            {
                                //CFilterDbSet::LivePetSkill(pState->GetFellowUniqueID(), data.second.SelfSkill_1);
                                Packet stMsg = new Packet(0x3528, false, false);
                                stMsg.WriteUInt32(session.SessionData.FellowPetUniqueID);
                                stMsg.WriteInt32(data.Value.SelfSkill_1);
                                await session.SendToServer(stMsg);
                                break;

                            }
                        }
                    }
                }
                else if (type == 19)
                {

                    int pingedregionID = packet.ReadInt32();
                    int pingedPosX = packet.ReadInt32();
                    int pingedPosY = packet.ReadInt32();
                    int pingedPosZ = packet.ReadInt32();
                    int Is0OutSide2IsInside = packet.ReadInt32();
                    int Normal0Town1Dungeon2 = packet.ReadInt32();
                    byte PtMemberscount = packet.ReadUInt8();
                    for (byte iss = 0; iss < PtMemberscount; iss++)
                    {
                        //string PartyMemberName = _pck.ReadAscii();
                        string PtMemberName = packet.ReadAscii();


                        var Agents = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charname == PtMemberName);
                        if (Agents != null)
                        {
                            Packet packeta = new Packet(0x193E);
                            packeta.WriteUInt8(1);
                            packeta.WriteInt32(pingedregionID);
                            packeta.WriteInt32(pingedPosX);
                            packeta.WriteInt32(pingedPosY);
                            packeta.WriteInt32(pingedPosZ);
                            packeta.WriteInt32(Is0OutSide2IsInside);
                            packeta.WriteInt32(Normal0Town1Dungeon2);
                            await Agents.SendToClient(packeta);
                        }
                    }
                }
                else if (type == 20)
                {
                    byte plustype = packet.ReadUInt8();
                    if (plustype == 0)
                    {
                        byte TargetItemSlot = packet.ReadUInt8();
                        byte EncSlot = packet.ReadUInt8();
                        byte ProofSlot = packet.ReadUInt8();

                        SItemInfoDbRecord stItemInfoRecord = new SItemInfoDbRecord();
                        bool bHasItemInfo = await AgentServer.TryGetItemInfoAsync(stItemInfoRecord, session.SessionData.Charid, TargetItemSlot);

                        if (bHasItemInfo)
                        {
                            if (stItemInfoRecord.btOptLevel >= Service.MaxPlus)
                            {
                                string noticeMessage = Management.GetNoticeMessage("ALCHEMY_MAX_ITEM_PLUS_NOADV");
                                Packet stMsg = new Packet(0x168A);
                                stMsg.WriteUInt8(NoticeType.WARNING);
                                stMsg.WriteUnicode(noticeMessage);
                                await session.SendToClient(stMsg);
                                return new PacketResult(PacketResultType.Block);
                            }
                        }
                        Packet xxx = new Packet(0x3534, false, false);
                        xxx.WriteUInt8(plustype);
                        xxx.WriteUInt8(TargetItemSlot);
                        xxx.WriteUInt8(EncSlot);
                        xxx.WriteUInt8(ProofSlot);
                        await session.SendToServer(xxx);

                    }
                    else if (plustype == 1)
                    {
                        byte TargetItemSlot = packet.ReadUInt8();
                        byte EncSlot = packet.ReadUInt8();

                        SItemInfoDbRecord stItemInfoRecord = new SItemInfoDbRecord();
                        bool bHasItemInfo = await AgentServer.TryGetItemInfoAsync(stItemInfoRecord, session.SessionData.Charid, TargetItemSlot);

                        if (bHasItemInfo)
                        {
                            if (stItemInfoRecord.btOptLevel >= Service.MaxPlus)
                            {
                                string noticeMessage = Management.GetNoticeMessage("ALCHEMY_MAX_ITEM_PLUS_NOADV");
                                Packet stMsg = new Packet(0x168A);
                                stMsg.WriteUInt8(NoticeType.WARNING);
                                stMsg.WriteUnicode(noticeMessage);
                                await session.SendToClient(stMsg);
                                return new PacketResult(PacketResultType.Block);
                            }
                        }

                        Packet xxx = new Packet(0x3534, false, false);
                        xxx.WriteUInt8(plustype);
                        xxx.WriteUInt8(TargetItemSlot);
                        xxx.WriteUInt8(EncSlot);
                        await session.SendToServer(xxx);
                    }
                }
                else if (type == 21)
                {
                    byte SlotIndex = packet.ReadUInt8();
                    string ItemCodeName = packet.ReadAscii();
                    string TargetItemCodeName = packet.ReadAscii();

                    bool isvalid = false;
                    string query = $"SELECT 1 FROM {Service.ShardDB}.._RefObjCommon with (nolock) WHERE CodeName128 = '{TargetItemCodeName}' and Service = 1";
                    using (var connection = new SqlConnection(Program.Connectionstring))
                    {
                        using (var command = new SqlCommand(query, connection))
                        {
                            await connection.OpenAsync();
                            var result = await command.ExecuteScalarAsync();
                            if (result != null)
                            {
                                isvalid = true;
                            }
                        }
                    }
                    if (Service.ItemTranslationPayment == 0) /// silk
                    {
                        int CurrentSilk = await sqlQueryHelper.prod_int($"SELECT silk_own FROM {Service.AccountDB}..SK_Silk WITH (NOLOCK) WHERE JID = {session.SessionData.JID}", Program.Connectionstring);


                        if (CurrentSilk != -1 && CurrentSilk >= Service.ItemTranslationPrice)
                        {
                            // Yeterli silk varsa güncelle
                            int newSilkAmount = CurrentSilk - Service.ItemTranslationPrice;
                            using (var connection = new SqlConnection(Program.Connectionstring))
                            {
                                await connection.OpenAsync();
                                string query2 = $"UPDATE SRO_VT_ACCOUNT..SK_Silk SET silk_own = {newSilkAmount} WHERE JID = {session.SessionData.JID}";
                                using (var command = new SqlCommand(query2, connection))
                                {
                                    await command.ExecuteNonQueryAsync();
                                }
                            }

                            Packet packeta = new Packet(0x3527);
                            packeta.WriteInt32(newSilkAmount);
                            await session.SendToServer(packeta);

                            if (isvalid)
                            {
                                Packet pck = new Packet(0x3505, false, false);
                                pck.WriteUInt8(Service.ItemTranslationPayment);
                                pck.WriteUInt8(SlotIndex);
                                pck.WriteAscii(ItemCodeName);
                                pck.WriteAscii(TargetItemCodeName);
                                await session.SendToServer(pck);
                            }
                            else
                            {
                                Packet stMsg = new Packet(0x168A);
                                stMsg.WriteUInt8(NoticeType.WARNING);
                                stMsg.WriteUnicode("Target item is not valid. Please contact GM.");
                                await session.SendToClient(stMsg);
                            }
                        }
                        else
                        {
                            Packet stMsgx = new Packet(0x5015);
                            stMsgx.WriteUInt8(6);
                            await session.SendToClient(stMsgx);
                            return new PacketResult(PacketResultType.Block);
                        }
                    }
                    else
                    {
                        if (isvalid)
                        {
                            Packet pck = new Packet(0x3505, false, false);
                            pck.WriteUInt8(Service.ItemTranslationPayment);
                            pck.WriteUInt8(SlotIndex);
                            pck.WriteAscii(ItemCodeName);
                            pck.WriteAscii(TargetItemCodeName);
                            pck.WriteInt32(Service.ItemTranslationPrice);
                            await session.SendToServer(pck);
                        }
                        else
                        {
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.WARNING);
                            stMsg.WriteUnicode("Target item is not valid. Please contact GM.");
                            await session.SendToClient(stMsg);
                        }
                    }
                    /// TARGETITEM VALIDATION !!!!

                }
                else if (type == 22)
                {
                    byte safetytype = packet.ReadUInt8();
                    if (safetytype == 0)
                    {
                        int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_LOCK_MAIL_TIME).TotalSeconds);
                        if (gecensaniye < 180)
                        {
                            int kalanSaniye = 180 - gecensaniye;
                            string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_LOCK_NOT_COMPLETED"), kalanSaniye);
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.WARNING);
                            stMsg.WriteUnicode(noticeMessage);
                            await session.SendToClient(stMsg);
                            return new PacketResult(PacketResultType.Block);
                        }

                        session.SessionData.LAST_LOCK_MAIL_TIME = DateTime.Now;
                        Random random = new Random();
                        int randomNumber = random.Next(10000000, 100000000);

                        string recipientEmail = session.SessionData.MailAddress;
                        string subject = "JTGuard - Item Lock Code";
                        string body = $"<p>Merhaba {session.SessionData.Charname},</p><p>Kilitlemek istediğin itemin doğrulama kodu : {randomNumber}</p><p>Kodu girmek için 3 dakikanız var.</p>";

                        // E-posta gönderimi arka planda gerçekleştirme
                        SendEmailInBackground(recipientEmail, subject, body);

                        session.SessionData.LockCode = randomNumber;

                        Packet Mailpck = new Packet(0x1209, false, false);
                        Mailpck.WriteUInt8(1); /// START TIMER
                        await session.SendToClient(Mailpck);
                    }
                    else if (safetytype == 1)
                    {
                        int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_UNLOCK_MAIL_TIME).TotalSeconds);
                        if (gecensaniye < 180)
                        {
                            int kalanSaniye = 180 - gecensaniye;
                            string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_UNLOCK_NOT_COMPLETED"), kalanSaniye);
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.WARNING);
                            stMsg.WriteUnicode(noticeMessage);
                            await session.SendToClient(stMsg);
                            return new PacketResult(PacketResultType.Block);
                        }

                        session.SessionData.LAST_UNLOCK_MAIL_TIME = DateTime.Now;
                        Random random = new Random();
                        int randomNumber = random.Next(10000000, 100000000);

                        string recipientEmail = session.SessionData.MailAddress;
                        string subject = "JTGuard - Item Unlock Code";
                        string body = $"<p>Merhaba {session.SessionData.Charname},</p><p>Kilidini açmak istediğin itemin doğrulama kodu : {randomNumber}</p><p>Kodu girmek için 3 dakikanız var.</p>";

                        // E-posta gönderimi arka planda gerçekleştirme
                        SendEmailInBackground(recipientEmail, subject, body);

                        session.SessionData.UnLockCode = randomNumber;

                        Packet Mailpck = new Packet(0x1209, false, false);
                        Mailpck.WriteUInt8(2); /// START TIMER // UUNLOCKER
                        await session.SendToClient(Mailpck);
                    }
                }
                else if (type == 23)
                {
                    string TargetName = packet.ReadUnicode();
                    int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_CHAR_INFO_DELAY).TotalSeconds);
                    if (gecensaniye < Service.SHOW_CHAR_INFO_DELAY)
                    {
                        int kalanSaniye = Service.GlobalDelay - gecensaniye;
                        string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_CHAR_INFO_DELAY"), kalanSaniye);
                        Packet stMsg = new Packet(0x168A);
                        stMsg.WriteUInt8(NoticeType.WARNING);
                        stMsg.WriteUnicode(noticeMessage);
                        await session.SendToClient(stMsg);
                        return new PacketResult(PacketResultType.Block);
                    }
                    var pSession = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charname == TargetName);
                    if (pSession != null)
                    {
                        if (pSession.SessionData.HideCharInformation)
                        {
                            Packet stMsgx = new Packet(0x5015);
                            stMsgx.WriteUInt8(7);
                            await session.SendToClient(stMsgx);
                            return new PacketResult(PacketResultType.Block);
                        }
                        else
                        {
                            Packet stMsgx = new Packet(0x5015);
                            stMsgx.WriteUInt8(8);
                            await session.SendToClient(stMsgx);

                            session.SessionData.LAST_CHAR_INFO_DELAY = DateTime.Now;
                            Packet xpacket = new Packet(0x3537);
                            xpacket.WriteAscii(session.SessionData.Charname); // its sender
                            await pSession.SendToServer(xpacket);

                        }
                    }
                }
                else if (type == 24)
                {
                    bool Value = packet.ReadBool();
                    bool SecondaryPWRememberPC = packet.ReadBool();
                    if (session.SessionData.HideCharInformation != Value)
                    {
                        session.SessionData.HideCharInformation = Value;
                        using (var connection = new SqlConnection(Program.Connectionstring))
                        {
                            await connection.OpenAsync();
                            string query = $"EXEC _HandleCharSettings '{session.SessionData.Charname}', {Value}";
                            using (var command = new SqlCommand(query, connection))
                            {
                                await command.ExecuteNonQueryAsync();

                            }
                        }
                    }


                    if (session.SessionData.SecondPwRememberPC != SecondaryPWRememberPC)
                    {
                        var secondaryPasswordData = await sqlQueryHelper.GetSecondaryPasswordData(session.PlayerUserID);
                        if (secondaryPasswordData != null)
                        {
                            await sqlQueryHelper.UpdateSecondaryPasswordData(session.PlayerUserID, secondaryPasswordData.Password, SecondaryPWRememberPC);
                            session.SessionData.SecondPwRememberPC = SecondaryPWRememberPC;
                        }
                    }


                }
                else if (type == 25)
                {
                    byte UniqueType = packet.ReadUInt8();
                    string column = "";

                    if (UniqueType == 0) column = "TGCalled";
                    else if (UniqueType == 1) column = "CerberusCalled";
                    else if (UniqueType == 2) column = "IvyCalled";
                    else if (UniqueType == 3) column = "UruchiCalled";
                    else if (UniqueType == 4) column = "IsyCalled";

                    if (!string.IsNullOrEmpty(column))
                    {
                        using (var connection = new SqlConnection(Program.Connectionstring))
                        {
                            await connection.OpenAsync();

                            var line = await connection.QueryFirstOrDefaultAsync<_CharInstanceWorldData>($"SELECT * FROM {Service.ShardDB}.._CharInstanceWorldData with(nolock) WHERE CharID = {session.SessionData.Charid} and WorldID = {99}");
                            if (line != null)
                            {
                                var getshadowmobstate = await connection.QueryAsync<_____ShadowDungeon>($"SELECT * FROM _____ShadowDungeon with (nolock) WHERE WorldID = {99} and LayerID = {line.LayerID}");
                                if (getshadowmobstate != null && getshadowmobstate.Count() > 0)
                                {
                                    foreach (var linex in getshadowmobstate)
                                    {
                                        var columnValue = linex.GetType().GetProperty(column).GetValue(linex, null);
                                        if (columnValue != null && (bool)columnValue)
                                        {
                                            return new PacketResult(PacketResultType.Block);
                                        }
                                        else
                                        {
                                            await sqlQueryHelper.EXEC_QUERY($"UPDATE _____ShadowDungeon SET {column} = 1 WHERE WorldID = {line.WorldID} and LayerID = {line.LayerID}");
                                            Packet oopck = new Packet(0x3538);
                                            oopck.WriteUInt8(UniqueType);
                                            await session.SendToServer(oopck);
                                        }
                                    }
                                }
                                else
                                {
                                    Packet aapck = new Packet(0x3538);
                                    aapck.WriteUInt8(UniqueType);
                                    await session.SendToServer(aapck);
                                    await sqlQueryHelper.EXEC_QUERY($"INSERT INTO _____ShadowDungeon (CharID, WorldID, LayerID, TGCalled, CerberusCalled, IvyCalled, UruchiCalled, IsyCalled) VALUES ({session.SessionData.Charid}, {line.WorldID}, {line.LayerID}, {(UniqueType == 0 ? 1 : 0)}, {(UniqueType == 1 ? 1 : 0)}, {(UniqueType == 2 ? 1 : 0)}, {(UniqueType == 3 ? 1 : 0)}, {(UniqueType == 4 ? 1 : 0)})");
                                }

                                // Sorgulanan uniquelerin sayısını almak için
                                int uniqueSpawnCount = await connection.QueryFirstOrDefaultAsync<int>($"SELECT SUM((CASE WHEN TGCalled = 1 THEN 1 ELSE 0 END) + (CASE WHEN CerberusCalled = 1 THEN 1 ELSE 0 END) + (CASE WHEN IvyCalled = 1 THEN 1 ELSE 0 END) + (CASE WHEN UruchiCalled = 1 THEN 1 ELSE 0 END) + (CASE WHEN IsyCalled = 1 THEN 1 ELSE 0 END)) AS TrueCount FROM _____ShadowDungeon with (nolock) WHERE WorldID = 99 AND LayerID = {line.LayerID}");

                                Packet uniquecountpck = new Packet(0x5040);
                                uniquecountpck.WriteInt32(uniqueSpawnCount);
                                uniquecountpck.WriteInt32(5);
                                await AgentServer.BroadcastPacketbyWorldIDAndLayerID(line.WorldID, line.LayerID, uniquecountpck);

                                Console.WriteLine($"{uniqueSpawnCount}");
                                var TimerWorldandPlayer = new TimerWorldandPlayer();
                                TimerWorldandPlayer.WorldID = line.WorldID;
                                TimerWorldandPlayer.LayerID = line.LayerID;


                                if (!AgentServer.eventManager.CreatedTimerListWorldIDAndLayerID.ContainsKey(TimerWorldandPlayer))
                                {
                                    AgentServer.eventManager.CreatedTimerListWorldIDAndLayerID.TryAdd(TimerWorldandPlayer, 180000);
                                    Packet pckx = new Packet(0x220A);
                                    pckx.WriteUInt8(0);
                                    pckx.WriteInt32(180000);

                                    await AgentServer.BroadcastPacketbyWorldIDAndLayerID(line.WorldID, line.LayerID, pckx);



                                    if (AgentServer.dbCommands.timers == null)
                                    {
                                        AgentServer.dbCommands.timers = new System.Timers.Timer();
                                        AgentServer.dbCommands.timers.Elapsed += (sender, e) => AgentServer.UpdateTimers(null); // Timer tetiklendiğinde UpdateTimers fonksiyonunu çağır
                                        AgentServer.dbCommands.timers.Interval = 1000; // Timer'ı her 1 saniyede bir tetiklemek için interval değerini ayarla
                                        AgentServer.dbCommands.timers.AutoReset = true; // Timer'ın otomatik olarak yeniden başlatılmasını etkinleştir
                                        AgentServer.dbCommands.timers.Start(); // Timer'ı başlat
                                    }
                                }
                            }
                        }
                    }
                }

            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "HandleGuiPackets");
            }
            return new PacketResult(PacketResultType.Block);
        }
        private void SendEmailInBackground(string recipient, string subject, string body)
        {
            EmailService emailService = new EmailService();
            Task.Run(() => emailService.SendEmailAsync(recipient, subject, body));
        }

        private async Task<PacketResult> GInterfaceIsReady(Packet packet, ISession session, object obj)
        {
            try
            {
                if (session.SessionData.FirstSpawn)
                {
                    Packet stMsgx = new Packet(0x5015);
                    stMsgx.WriteUInt8(9);
                    stMsgx.WriteBool(session.SessionData.HideCharInformation);
                    stMsgx.WriteBool(session.SessionData.SecondPwRememberPC);
                    await session.SendToClient(stMsgx);

                    Packet Mailpck = new Packet(0x1209);
                    Mailpck.WriteUInt8(0);
                    if(session.SessionData.MailAddress != null)
                    Mailpck.WriteAscii(session.SessionData.MailAddress);
                    else
                    Mailpck.WriteAscii("");
                    await session.SendToClient(Mailpck);

                    int nCharDBID = session.SessionData.Charid;
                    IEnumerable<_MacroAutoPotion> MacroAutoPotion;
                    IEnumerable<_MacroSetting> MacroSettings;
                    using (var connection = new SqlConnection(Program.Connectionstring))
                    {
                        await connection.OpenAsync();

                        // İlk sorgu: _ActiveTitleColors tablosu
                        var titles = await connection.QueryAsync<int>($"SELECT TitleID FROM _CharacterTitleManager WHERE CharID = {nCharDBID}");
                        foreach (var item in titles)
                        {
                            if (!session.SessionData.CharTitles.Contains(item))
                            {
                                session.SessionData.CharTitles.Add(item);
                            }
                        }
                      
                        var colors = await connection.QueryAsync<_CharacterTitleManagerColor>($"SELECT * FROM _CharacterTitleManagerColor with (nolock) WHERE CharID = {nCharDBID}");
                        foreach (var item in colors)
                        {
                            if(!session.SessionData.CharacterTitleColors.ContainsKey(item.ID))
                            {
                                session.SessionData.CharacterTitleColors.TryAdd(item.ID, item);
                            }
                        }

                        var icons = await connection.QueryAsync<_CharacterIconManager>($"SELECT * FROM _CharacterIconManager with (nolock) WHERE CharID = {nCharDBID}");
                        foreach (var item in icons)
                        {
                            if (!session.SessionData.CharacterIcons.ContainsKey(item.ID))
                            {
                                session.SessionData.CharacterIcons.TryAdd(item.IconID, item);
                            }
                        }

                        var chest = await connection.QueryAsync<_ItemChest>($"SELECT * FROM _ItemChest with (nolock) WHERE CharID = {nCharDBID}");
                        foreach (var item in chest)
                        {
                            if (!session.SessionData.CharacterChest.ContainsKey(item.ID))
                            {
                                session.SessionData.CharacterChest.TryAdd(item.ID, item);
                            }
                        }

                        var achievement = await connection.QueryAsync<_Achievement>($"SELECT * FROM _Achievement with (nolock) WHERE CharID = {nCharDBID}");
                        foreach (var item in achievement)
                        {
                            if (!session.SessionData.CharacterAchievement.ContainsKey(item.RefAchievementID))
                            {
                                session.SessionData.CharacterAchievement.TryAdd(item.RefAchievementID, item);
                            }
                        }

                        var achievementcon = await connection.QueryAsync<_AchievementCondition>($"SELECT * FROM _AchievementCondition with (nolock) WHERE CharID = {nCharDBID}");
                        foreach (var item in achievementcon)
                        {
                            if (!session.SessionData.CharacterAchievementCondition.ContainsKey(item.RefAchievementConditionID))
                            {
                                session.SessionData.CharacterAchievementCondition.TryAdd(item.RefAchievementConditionID, item);
                            }
                        }

                        var newrev = await connection.QueryAsync<_NewReverseSavedLocations>($"SELECT * FROM _NewReverseSavedLocations with (nolock) WHERE CharID = {nCharDBID}");
                        foreach (var item in newrev)
                        {
                            if (!session.SessionData.CharacterNewReverseSavedLocations.ContainsKey(item.LocationID))
                            {
                                session.SessionData.CharacterNewReverseSavedLocations.TryAdd(item.LocationID, item);
                            }
                        }

                        MacroAutoPotion = await connection.QueryAsync<_MacroAutoPotion>($"SELECT * FROM _MacroAutoPotion with (nolock) WHERE CharID = {nCharDBID}");
                        MacroSettings = await connection.QueryAsync<_MacroSetting>($"SELECT * FROM _MacroSetting with (nolock) WHERE CharID = {nCharDBID}");

                    }


                    if (session.SessionData.CharacterAchievement.Count() > 0)
                    {
                        Packet stAckMsg = new Packet(0x177A);
                        stAckMsg.WriteInt32(session.SessionData.CharacterAchievement.Count());
                        foreach (var stRecord in session.SessionData.CharacterAchievement)
                        {
                            stAckMsg.WriteInt32(stRecord.Value.RefAchievementID);
                            stAckMsg.WriteUInt8(stRecord.Value.State);

                        }
                        await session.SendToClient(stAckMsg);

                        if (session.SessionData.CharacterAchievementCondition.Count() > 0)
                        {
                            Packet stAchConPck = new Packet(0x177B);
                            stAchConPck.WriteInt32(session.SessionData.CharacterAchievementCondition.Count());
                            foreach (var stRecord in session.SessionData.CharacterAchievementCondition)
                            {
                                stAchConPck.WriteInt32(stRecord.Value.RefAchievementConditionID);
                                stAchConPck.WriteInt32(stRecord.Value.AchievementID);
                                stAchConPck.WriteInt64(stRecord.Value.ProgressCount);
                            }
                            await session.SendToClient(stAchConPck);
                        }
                    }

                    if (session.SessionData.CharacterNewReverseSavedLocations.Count() > 0)
                    {
                        Packet stAckMsg = new Packet(0x206C);
                        stAckMsg.WriteUInt8(session.SessionData.CharacterNewReverseSavedLocations.Count());
                        foreach (var stRecord in session.SessionData.CharacterNewReverseSavedLocations)
                        {
                            stAckMsg.WriteUInt8(stRecord.Value.LocationID);
                            stAckMsg.WriteInt32(stRecord.Value.RegionID);
                            stAckMsg.WriteInt32(stRecord.Value.WorldID);
                        }
                        await session.SendToClient(stAckMsg);
                    }

                    if (RankManager.m_SilkRank.ContainsKey(nCharDBID))
                    {
                        Packet packetx = new Packet(0x209C);
                        packetx.WriteInt32(RankManager.m_SilkRank[nCharDBID].SilkHistory);
                        packetx.WriteInt32(RankManager.m_SilkRank[nCharDBID].SilkRank);
                        await session.SendToClient(packetx);
                    }


                    if (RefManager.m_AttendanceReward.Count() > 0)
                    {
                        Packet packetx = new Packet(0x208B);
                        packetx.WriteUInt8(RefManager.m_AttendanceReward.Count());
                        foreach (var line in RefManager.m_AttendanceReward)
                        {
                            packetx.WriteInt32(line.Value.ID);
                            packetx.WriteInt32(line.Value.ItemID);
                            packetx.WriteInt32(line.Value.ItemCount);
                            packetx.WriteInt32(line.Value.DayCount);
                        }
                        await session.SendToClient(packetx);
                    }

                    if (MacroAutoPotion.Count() > 0)
                    {
                        Packet stAckMsg = new Packet(0x204A);
                        stAckMsg.WriteUInt8(MacroAutoPotion.Count());
                        foreach (var data in MacroAutoPotion)
                        {
                            stAckMsg.WriteUInt8(data.Slot);
                            stAckMsg.WriteUInt8(data.Active);
                            stAckMsg.WriteUInt8(data.Value);
                        }
                        await session.SendToClient(stAckMsg);
                    }

                    if (MacroSettings.Count() > 0)
                    {
                        Packet stAckMsg = new Packet(0x204B);
                        foreach (var data in MacroSettings)
                        {
                            stAckMsg.WriteUInt8(data.AutoPotion);
                            stAckMsg.WriteUInt8(data.AutoSkill);
                            stAckMsg.WriteUInt8(data.AutoHunt);
                            stAckMsg.WriteUInt8(data.AutoPickup);
                            stAckMsg.WriteUInt8(data.AutoScroll);
                        }
                        await session.SendToClient(stAckMsg);
                    }
                }
                return new PacketResult(PacketResultType.Block);
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "GINTERFACE READY");
                return new PacketResult(PacketResultType.Block);
            }
        }
        private async Task<PacketResult> SERVER_ENTER_GAME_ACTION(Packet packet, ISession session, object obj)
        {
            try
            {
                byte btType = packet.ReadUInt8();

                if (btType != 1)
                    return new PacketResult(PacketResultType.Nothing);


                string strCharName = session.SessionData.Charname;
                int charId = await sqlQueryHelper.prod_int($"SELECT CharID FROM {Service.ShardDB}.._Char WITH (NOLOCK) WHERE CharName16 = '{strCharName}'", Program.Connectionstring);
                session.SessionData.Charid = charId;

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    // İlk sorgu: _ActiveTitleColors tablosu
                    string query1 = $"SELECT Email, JID FROM {Service.AccountDB}..TB_User WITH (NOLOCK) WHERE StrUserID = '{session.PlayerUserID}'";
                    var result1 = await connection.QueryAsync<(string Mail, int JID)>(query1);
                    foreach (var item in result1)
                    {
                        session.SessionData.MailAddress = item.Mail;
                        session.SessionData.JID = item.JID;
                    }
                }

                //pState->SetDBID(CFilterDbSet::GetCharDBID(strCharName));

                ////Update player online state
                //CFilterDbSet::UpdatePlayerOnlineState(
                //    pState->GetDBID(),
                //    pState->GetCharName(),
                //    true
                //);

                if (RefManager.m_ActiveTitleColors.Count() > 0)
                {
                    Packet stAckMsg = new Packet(0x204D);
                    stAckMsg.WriteInt32(RefManager.m_ActiveTitleColors.Count());
                    foreach (var line in RefManager.m_ActiveTitleColors)
                    {
                        stAckMsg.WriteAscii(line.Key); /// charname maybe
                        int argbInputColor = Int32.Parse(line.Value.Replace("#", ""), NumberStyles.HexNumber);
                        stAckMsg.WriteUInt32(argbInputColor);
                    }
                    await session.SendToClient(stAckMsg);
                }

                if (RefManager.m_RefIconsMediaPath.Count() > 0)
                {
                    Packet stAckMsg = new Packet(0x204F);
                    stAckMsg.WriteInt32(RefManager.m_RefIconsMediaPath.Count());
                    foreach (var line in RefManager.m_RefIconsMediaPath)
                    {
                        stAckMsg.WriteInt32(line.Key);
                        stAckMsg.WriteAscii(line.Value);
                    }
                    await session.SendToClient(stAckMsg);
                }

                if (RefManager.m_LeftCharIcons.Count() > 0)
                {
                    Packet stAckMsg = new Packet(0x205A);
                    stAckMsg.WriteInt32(RefManager.m_LeftCharIcons.Count());

                    foreach (var data in RefManager.m_LeftCharIcons)
                    {
                        stAckMsg.WriteAscii(data.Key);
                        stAckMsg.WriteInt32(data.Value);

                    }
                    await session.SendToClient(stAckMsg);
                }
                if (RefManager.m_RightCharIcons.Count() > 0)
                {
                    Packet stAckMsg = new Packet(0x205B);
                    stAckMsg.WriteInt32(RefManager.m_RightCharIcons.Count());

                    foreach (var data in RefManager.m_RightCharIcons)
                    {
                        stAckMsg.WriteAscii(data.Key);
                        stAckMsg.WriteInt32(data.Value);

                    }
                    await session.SendToClient(stAckMsg);
                }


                if (RefManager.m_ActiveTitleNameNew.Count() > 0)
                {
                    Packet stAckMsg = new Packet(0x204C);
                    stAckMsg.WriteInt32(RefManager.m_ActiveTitleNameNew.Count());
                    foreach (var data in RefManager.m_ActiveTitleNameNew)
                    {
                        stAckMsg.WriteAscii(data.Key);

                        if (RefManager.m_RefTitleNameNew.ContainsKey(data.Value))
                        {
                            stAckMsg.WriteAscii(RefManager.m_RefTitleNameNew[data.Value]);
                        }
                        else
                        {
                            stAckMsg.WriteAscii("");
                        }
                    }
                    await session.SendToClient(stAckMsg);
                }

                if (RefManager.m_RefAchievements.Count() > 0)
                {
                    Packet stAckMsg = new Packet(0x205C);
                    stAckMsg.WriteInt32(RefManager.m_RefAchievements.Count());
                    foreach (var data in RefManager.m_RefAchievements)
                    {
                        stAckMsg.WriteInt32(data.Value.ID);
                        stAckMsg.WriteUInt8(data.Value.Category);
                        stAckMsg.WriteAscii(data.Value.Name);
                        stAckMsg.WriteUInt8(data.Value.RewardType);
                        if (data.Value.RewardType == 0)
                        {
                            if (RefManager.m_RefTitleNameNew.ContainsKey(data.Value.RewardTitleID))
                            {
                                stAckMsg.WriteAscii(RefManager.m_RefTitleNameNew[data.Value.RewardTitleID]);
                            }
                            else
                            {
                                stAckMsg.WriteAscii("");
                            }

                        }

                        stAckMsg.WriteInt32(data.Value.RewardSkillPoint);
                        stAckMsg.WriteInt64(data.Value.RewardGold);

                    }
                    await session.SendToClient(stAckMsg);
                }
                if (RefManager.m_RefAchievementsCondition.Count() > 0)
                {
                    Packet stAckMsg = new Packet(0x175A);
                    stAckMsg.WriteInt32(RefManager.m_RefAchievementsCondition.Count());
                    foreach (var data in RefManager.m_RefAchievementsCondition)
                    {
                        stAckMsg.WriteInt32(data.Value.ID);
                        stAckMsg.WriteAscii(data.Value.Name);
                        stAckMsg.WriteInt32(data.Value.RefAchievementID);
                        stAckMsg.WriteInt64(data.Value.CompleteCount);
                        stAckMsg.WriteUInt8(data.Value.Type);
                    }
                    await session.SendToClient(stAckMsg);
                }


                if (RefManager.m_HideSkillEffects.Count() > 0)
                {
                    Packet stAckMsg = new Packet(0x209E);
                    stAckMsg.WriteInt32(RefManager.m_HideSkillEffects.Count());
                    foreach (var data in RefManager.m_HideSkillEffects)
                    {
                        stAckMsg.WriteInt32(data.Key);
                        stAckMsg.WriteBool(data.Value.JobMode);
                        stAckMsg.WriteBool(data.Value.MapSettings);

                    }
                    await session.SendToClient(stAckMsg);
                }

                if (RefManager.m_RefEventMapSettings.Count() > 0)
                {
                    Packet stAckMsg = new Packet(0x207E);
                    stAckMsg.WriteInt32(RefManager.m_RefEventMapSettings.Count());
                    foreach (var data in RefManager.m_RefEventMapSettings)
                    {
                        stAckMsg.WriteInt32(data.Key);
                        stAckMsg.WriteBool(data.Value.EventSuit);
                        stAckMsg.WriteBool(data.Value.HideBuffViewer);
                        stAckMsg.WriteBool(data.Value.DisablePetSpawn);
                        stAckMsg.WriteBool(data.Value.DisableParty);
                        stAckMsg.WriteBool(data.Value.AutoCape);
                        stAckMsg.WriteBool(data.Value.HideMiniMap);
                    }
                    await session.SendToClient(stAckMsg);
                }

                if (RefManager.m_RefFellowData.Count() > 0)
                {
                    Packet stAckMsg = new Packet(0x206A);
                    stAckMsg.WriteUInt8(RefManager.m_RefFellowData.Count());
                    foreach (var data in RefManager.m_RefFellowData)
                    {
                        stAckMsg.WriteAscii(data.Key);
                        stAckMsg.WriteInt32(data.Value.SkillID_1);
                        stAckMsg.WriteUInt8(data.Value.Active_Level_1);
                        stAckMsg.WriteUInt8(data.Value.SkillType_1);
                        stAckMsg.WriteInt32(data.Value.SkillID_2);
                        stAckMsg.WriteUInt8(data.Value.Active_Level_2);
                        stAckMsg.WriteUInt8(data.Value.SkillType_2);
                        stAckMsg.WriteInt32(data.Value.SkillID_3);
                        stAckMsg.WriteUInt8(data.Value.Active_Level_3);
                        stAckMsg.WriteUInt8(data.Value.SkillType_3);
                        stAckMsg.WriteInt32(data.Value.SkillID_4);
                        stAckMsg.WriteUInt8(data.Value.Active_Level_4);
                        stAckMsg.WriteUInt8(data.Value.SkillType_4);
                        stAckMsg.WriteInt32(data.Value.SkillID_5);
                        stAckMsg.WriteUInt8(data.Value.Active_Level_5);
                        stAckMsg.WriteUInt8(data.Value.SkillType_5);

                        stAckMsg.WriteInt32(data.Value.SelfSkill_1);
                        stAckMsg.WriteUInt8(data.Value.SelfSkill_Active_Level_1);

                        stAckMsg.WriteInt32(data.Value.SelfSkill_2);
                        stAckMsg.WriteUInt8(data.Value.SelfSkill_Active_Level_2);

                    }
                    await session.SendToClient(stAckMsg);
                }

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    // İlk sorgu: _ActiveTitleColors tablosu
                    string query1 = $"SELECT ID,CharID,DayCount,LastAttendedDate FROM _Attendance with (nolock) WHERE CharID = {session.SessionData.Charid}";
                    var result1 = await connection.QueryAsync<_Attendance>(query1);
                    foreach (var item in result1)
                    {
                        session.SessionData.AttendanceDayCount = item.DayCount;
                        session.SessionData.LastAttendedDate = item.LastAttendedDate;
                    }

                    var result2 = await connection.QueryAsync<_CharacterSettings>("SELECT * FROM _CharacterSettings with (nolock)");
                    foreach (var item in result2)
                    {
                        session.SessionData.HideCharInformation = item.HideItemInfo;
                    }
                   

                }
                
                return new PacketResult(PacketResultType.Nothing);
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "SERVER_ENTER_GAME_ACTION");
                return new PacketResult(PacketResultType.Block);
            }
        }
        private async Task<PacketResult> CLIENT_GRANT_NAME_REQUEST(Packet packet, ISession session, object obj)
        {
            try
            {
                string NewGrantName = packet.ReadAscii();
                // Belirli özel karakterleri tanımla
                char[] specialChars = new[] { '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '-', '=', '+', '[', ']', '{', '}', '\\', '|', ';', ':', '\'', '"', ',', '<', '>', '/', '?', 'ğ', 'ç', 'ş', 'ü', 'ö', 'ı', 'Ğ', 'Ç', 'Ş', 'Ü', 'Ö', 'İ', ' ' };

                // '\0' karakterini temizle
                NewGrantName = NewGrantName.Replace("\0", "");

                // Özel karakter kontrolü
                bool containsSpecialChar = NewGrantName.Any(ch => specialChars.Contains(ch) && ch != '_');

                // '_' hariç diğer özel karakterler, boşluk veya Türkçe harfler varsa
                if (containsSpecialChar)
                {
                    return new PacketResult(PacketResultType.Block);
                }
                Packet LiveTitle = new Packet(0x3500);
                LiveTitle.WriteAscii(NewGrantName);
                await session.SendToServer(LiveTitle);
                return new PacketResult(PacketResultType.Block);
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "CLIENT_GRANT_NAME_REQUEST");
                return new PacketResult(PacketResultType.Block);
            }
        }
        private async Task<PacketResult> CLIENT_HWID_REQUEST(Packet packet, ISession session, object obj)
        {
            try
            {
                string Hwid = packet.ReadAscii();
                if (!Hwid.EndsWith("1999"))
                {
                    string noticeMessage = Management.GetNoticeMessage("HWID_ERROR");
                    Packet pck = new Packet(0xA340);
                    pck.WriteUnicode(noticeMessage);
                    await session.SendToClient(pck);
                    return new PacketResult(PacketResultType.Block);
                }
                else
                {
                    string noticeMessage = Management.GetNoticeMessage("HWID_SUCCES");
                    session.SessionData.Hwid = Hwid;
                    Packet pck = new Packet(0xA340);
                    pck.WriteUnicode(noticeMessage);
                    await session.SendToClient(pck);
                    return new PacketResult(PacketResultType.Block);
                }
                return new PacketResult(PacketResultType.Block);
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "CLIENT_HWID_REQUEST");
                return new PacketResult(PacketResultType.Block);
            }
        }
        private async Task<PacketResult> SERVER_CHARACTER_SELECTION_ACTION(Packet packet, ISession session, object obj)
        {
            try
            {
                if (string.IsNullOrEmpty(session.SessionData.Hwid))
                {
                    Packet hwid = new Packet(0x165A);
                    await session.SendToClient(hwid);
                }
                if (packet.ReadUInt8() == 0x02)
                {
                    using (var connection = new SqlConnection(Program.Connectionstring))
                    {
                        await connection.OpenAsync();
                        if (packet.ReadUInt8() == 0x01)
                        {
                            byte char_count = packet.ReadUInt8();
                            //MenuSettings();
                            Packet Info = new Packet(0x1199);
                            Info.WriteUInt8(char_count);
                            for (int cc = 0; cc < char_count; cc++)
                            {
                                #region MainEntry

                                packet.ReadUInt32(); //Model
                                string cn16 = packet.ReadAscii(); // Name
                                packet.ReadUInt8(); //Volume/Height
                                packet.ReadUInt8(); //Level
                                packet.ReadUInt64(); //Exp
                                packet.ReadUInt16(); //STR
                                packet.ReadUInt16(); //INT
                                packet.ReadUInt16(); //Stats points
                                packet.ReadUInt32(); //Hp
                                packet.ReadUInt32(); //Mp
                                                     //int Last = await sqlQueryHelper.prod_int($"SELECT LatestRegion FROM {Service.ShardDB}.._Char WITH (NOLOCK) WHERE CharName16 = '{cn16}'", Program.Connectionstring);
                                int Last = 0;

                                var search = await connection.QueryFirstOrDefaultAsync<int>($"Select LatestRegion from {Service.ShardDB}.._Char with (nolock) Where CharName16 = '{cn16}'");
                                if (search != 0)
                                {
                                    Last = search;
                                }


                                Info.WriteAscii(cn16);
                                Info.WriteInt32(Last);
                                #endregion MainEntry

                                #region Deletion

                                byte char_delete = packet.ReadUInt8();
                                if (char_delete == 1)
                                {
                                    packet.ReadUInt32();
                                }
                                packet.ReadUInt8(); //Unknown
                                packet.ReadUInt8(); //Unknown
                                packet.ReadUInt8(); //Unknown
                                Info.WriteUInt8(char_delete);
                                #endregion Deletion

                                #region Items

                                int ls_itemscount = packet.ReadUInt8();
                                //Console.WriteLine($"Total Items Count: {itemscount} for {cn16}");
                                for (int ic = 0; ic < ls_itemscount; ic++)
                                {
                                    uint Item_id = packet.ReadUInt32(); //Item ID
                                    packet.ReadUInt8(); //Plus Value

                                }

                                #endregion Items

                                #region Avatars

                                int avatarcount = packet.ReadUInt8(); //Avatar count
                                for (int ac = 0; ac < avatarcount; ac++)
                                {
                                    packet.ReadUInt32(); // Avatar ID
                                    packet.ReadUInt8(); // Plus
                                }

                                #endregion Avatars
                            }
                            await session.SendToClient(Info);
                        }
                    }
                }
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "-SERVER_CHARACTER_SELECTION_ACTION ");
            }
            return new PacketResult(packet, PacketResultType.Nothing);
        }
    }
}

