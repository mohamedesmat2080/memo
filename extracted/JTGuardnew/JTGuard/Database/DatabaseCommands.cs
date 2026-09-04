using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Dapper;
using System.Timers;
using Timer = System.Timers.Timer;
using JTGuard.Database.Model;
using JTGuard.PacketHandlerManager;
using JTGuard.Server;
using Microsoft.Data.SqlClient;
using SilkroadSecurityAPI;
using JTGuard.Helpers;
using JTGuard.Database.Models;
using System.Globalization;
using System.Reflection.PortableExecutable;
using static System.Runtime.InteropServices.JavaScript.JSType;
using System.Net.NetworkInformation;
using JTGuard.ServerManagers;
using JTGuard.Database.RankModels;
using JTGuard.Database.ModelsEvents;

namespace JTGuard.Database
{
    public partial class DatabaseCommands
    {
        private AgentServer AgentServer { get; set; }
        public _ServerSettings Service { get; set; }
        private static SqlConnection sharedConnection;
        private Timer _commandTimer;

        public System.Timers.Timer timers;
        public DatabaseCommands(_ServerSettings service, AgentServer agentServer)
        {
            Service = service;
            AgentServer = agentServer;

            InitializeSharedConnection().GetAwaiter().GetResult();
            InitializeTimer();
        }
        private void InitializeTimer()
        {
            _commandTimer = new Timer(500);
            _commandTimer.Elapsed += OnTimerTick;
            _commandTimer.AutoReset = true;
            _commandTimer.Enabled = true;
        }
        private async Task InitializeSharedConnection()
        {
            if (sharedConnection == null)
            {
                sharedConnection = new SqlConnection(Program.Connectionstring);
                await sharedConnection.OpenAsync();
            }
        }

        public async void OnTimerTick(object sender, ElapsedEventArgs e)
        {
            try
            {
                DateTime currentTime = DateTime.Now;

                if (sharedConnection == null || sharedConnection.State != System.Data.ConnectionState.Open)
                {
                    await InitializeSharedConnection();
                }

                var pendingCommands = await sharedConnection.QueryAsync<_AsyncFilterCommands>("SELECT * FROM _AsyncFilterCommands WHERE Status = 1");

                foreach (var command in pendingCommands)
                {
                    try
                    {
                        int CommandID = command.CommandID;
                        if (CommandID == 1)
                        {
                            if (command.Data1 != null)
                            {
                                if (AgentServer.AgentSessions != null)
                                {
                                    var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charname == command.Data1);
                                    if (session != null)
                                    {
                                        session.Stop();
                                        AgentServer.AgentSessions.Remove(session);
                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        } // tested
                        else if (CommandID == 2)
                        {
                            if (AgentServer.AgentSessions != null)
                            {
                                foreach (var line in AgentServer.AgentSessions.ToList())
                                {
                                    line.Stop();
                                    AgentServer.AgentSessions.Remove(line);
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        } // tested
                        else if (CommandID == 3)
                        {
                            if (command.Data1 != null && command.Data2 != null && command.Data3 != null)
                            {
                                if (byte.TryParse(command.Data2, out byte type))
                                {
                                    if (command.Data1.ToLower().Contains("sendall"))
                                    {
                                        Packet Notice = new Packet(0x168A);
                                        Notice.WriteUInt8(type);
                                        Notice.WriteUnicode(command.Data3);// Message
                                        await AgentServer.BroadcastPacket(Notice);
                                    }
                                    else if (command.Data1.ToLower().Contains("sendchar") && command.Data4 != null)
                                    {
                                        if (command.Data4.Length > 0)
                                        {
                                            Packet Notice = new Packet(0x168A);
                                            Notice.WriteUInt8(type);
                                            Notice.WriteUnicode(command.Data3);// Message
                                            if (command.Data4 != null)
                                            {
                                                await AgentServer.BroadcastPacketToCharName(command.Data4, Notice);
                                            }
                                        }
                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        } // tested
                        else if (CommandID == 4)
                        {
                            if (command.Data1 != null && command.Data2 != null)
                            {
                                if (int.TryParse(command.Data1, out int charid) && byte.TryParse(command.Data2, out byte titleid))
                                {
                                    var session = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == charid);

                                    if (session != null)
                                    {
                                        if (!session.SessionData.CharTitles.Contains(titleid))
                                        {
                                            session.SessionData.CharTitles.Add(titleid);
                                            Packet Title = new Packet(0x168B);
                                            Title.WriteUInt8(titleid);
                                            if (session.SessionData.CHChar)
                                            {
                                                Title.WriteAscii(AgentServer.RefHwan[titleid].Title_CH70);
                                            }
                                            if (session.SessionData.EUChar)
                                            {
                                                Title.WriteAscii(AgentServer.RefHwan[titleid].Title_EU70);
                                            }
                                            await session.SendToClient(Title);
                                        }
                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }  // tested
                        else if (CommandID == 5)
                        {
                            if (command.Data1 != null && command.Data2 != null && command.Data3 != null && command.Data4 != null)
                            {
                                if (int.TryParse(command.Data1, out int charid) && int.TryParse(command.Data4, out int ColorDBID))
                                {
                                    var session = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == charid);

                                    if (session != null)
                                    {
                                        if (!session.SessionData.CharacterTitleColors.ContainsKey(ColorDBID))
                                        {
                                            var titlec = new _CharacterTitleManagerColor
                                            {
                                                CharID = session.SessionData.Charid,
                                                ColorName = command.Data2,
                                                ColorCode = command.Data3,
                                            };

                                            session.SessionData.CharacterTitleColors.TryAdd(ColorDBID, titlec);

                                            Packet TitleColors = new Packet(0x168C);
                                            TitleColors.WriteInt32(ColorDBID);

                                            TitleColors.WriteAscii(command.Data2);
                                            int argbInputColor = Int32.Parse(command.Data3.Replace("#", ""), NumberStyles.HexNumber);
                                            TitleColors.WriteUInt32(argbInputColor);

                                            await session.SendToClient(TitleColors);
                                        }
                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        } // tested
                        else if (CommandID == 6)
                        {
                            if (command.Data1 != null && command.Data2 != null && command.Data3 != null && command.Data4 != null)
                            {
                                if (int.TryParse(command.Data1, out int CharID) && int.TryParse(command.Data2, out int IconID)
                                    && byte.TryParse(command.Data3, out byte Side) && int.TryParse(command.Data4, out int DBID))
                                {
                                    var session = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == CharID);

                                    if (session != null)
                                    {
                                        if (!session.SessionData.CharacterIcons.ContainsKey(DBID))
                                        {

                                            var icon = new _CharacterIconManager
                                            {
                                                ID = DBID,
                                                CharID = session.SessionData.Charid,
                                                IconID = IconID,
                                                Side = Side,
                                            };

                                            session.SessionData.CharacterIcons.TryAdd(IconID, icon);
                                            Packet IconPacket = new Packet(0x168D);

                                            IconPacket.WriteInt32(DBID);

                                            if (RefManager.m_RefIconsMediaPath.ContainsKey(IconID))
                                            {
                                                IconPacket.WriteAscii(RefManager.m_RefIconsMediaPath[IconID]);
                                            }
                                            else
                                            {
                                                IconPacket.WriteAscii("dummy");
                                            }
                                            IconPacket.WriteUInt8(Side);
                                            await session.SendToClient(IconPacket);
                                        }
                                    }

                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 7)
                        {
                            if (command.Data1 != null && command.Data2 != null)
                            {
                                if (int.TryParse(command.Data1, out int CharID) && int.TryParse(command.Data2, out int TitleID))
                                {
                                    var session = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == CharID);

                                    if (session != null)
                                    {
                                        if (session.SessionData.State.BodyState != BodyState.Berserk)
                                        {
                                            Packet stAckMsg = new Packet(0x3501);
                                            //stAckMsg.data.Write_Ascii(KEY_OF_SECRET);
                                            stAckMsg.WriteUInt8(TitleID);
                                            await session.SendToServer(stAckMsg);
                                        }
                                    }
                                }

                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        } // tested
                        else if (CommandID == 8)
                        {
                            if (command.Data1 != null && command.Data2 != null)
                            {
                                if (RefManager.m_ActiveTitleColors.ContainsKey(command.Data1))
                                {
                                    RefManager.m_ActiveTitleColors[command.Data1] = command.Data2;
                                }
                                else
                                {
                                    RefManager.m_ActiveTitleColors.TryAdd(command.Data1, command.Data2);
                                }

                                int argbInputColor = Int32.Parse(command.Data2.Replace("#", ""), NumberStyles.HexNumber);

                                Packet stAckMsg = new Packet(0x170A);
                                stAckMsg.WriteAscii(command.Data1);
                                stAckMsg.WriteUInt32(argbInputColor);
                                await AgentServer.BroadcastPacket(stAckMsg);
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        } // tested UPDATE TITLE COLOR
                        else if (CommandID == 9) /// REMOVE TITLE COLOR
                        {
                            if (command.Data1 != null)
                            {
                                if (RefManager.m_ActiveTitleColors.ContainsKey(command.Data1))
                                {
                                    RefManager.m_ActiveTitleColors.Remove(command.Data1);
                                }
                                Packet stAckMsg = new Packet(0x170B);
                                stAckMsg.WriteAscii(command.Data1);
                                await AgentServer.BroadcastPacket(stAckMsg);
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 10) /// UPDATE LEFT CHAR ICON
                        {
                            if (command.Data1 != null && command.Data2 != null)
                            {
                                if (int.TryParse(command.Data2, out int IconID))
                                {
                                    if (RefManager.m_LeftCharIcons.ContainsKey(command.Data1))
                                    {
                                        RefManager.m_LeftCharIcons[command.Data1] = IconID;
                                    }
                                    else
                                    {
                                        RefManager.m_LeftCharIcons.TryAdd(command.Data1, IconID);
                                    }
                                    Packet stAckMsg = new Packet(0x173F);
                                    stAckMsg.WriteAscii(command.Data1);
                                    stAckMsg.WriteInt32(IconID);
                                    await AgentServer.BroadcastPacket(stAckMsg);
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 11) /// UPDATE RIGHT CHAR ICON
                        {
                            if (command.Data1 != null && command.Data2 != null)
                            {
                                if (int.TryParse(command.Data2, out int IconID))
                                {
                                    if (RefManager.m_RightCharIcons.ContainsKey(command.Data1))
                                    {
                                        RefManager.m_RightCharIcons[command.Data1] = IconID;
                                    }
                                    else
                                    {
                                        RefManager.m_RightCharIcons.TryAdd(command.Data1, IconID);
                                    }
                                    Packet stAckMsg = new Packet(0x174B);
                                    stAckMsg.WriteAscii(command.Data1);
                                    stAckMsg.WriteInt32(IconID);
                                    await AgentServer.BroadcastPacket(stAckMsg);
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 12) /// REMOVE LEFT CHAR ICON
                        {
                            if (command.Data1 != null)
                            {

                                if (RefManager.m_LeftCharIcons.ContainsKey(command.Data1))
                                {
                                    RefManager.m_LeftCharIcons.Remove(command.Data1);
                                }
                                Packet stAckMsg = new Packet(0x174A);
                                stAckMsg.WriteAscii(command.Data1);
                                await AgentServer.BroadcastPacket(stAckMsg);
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 13) /// REMOVE right CHAR ICON
                        {
                            if (command.Data1 != null)
                            {

                                if (RefManager.m_RightCharIcons.ContainsKey(command.Data1))
                                {
                                    RefManager.m_RightCharIcons.Remove(command.Data1);
                                }
                                Packet stAckMsg = new Packet(0x174E);
                                stAckMsg.WriteAscii(command.Data1);
                                await AgentServer.BroadcastPacket(stAckMsg);
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 15)
                        {
                            if (command.Data1 != null && command.Data2 != null)
                            {
                                if (byte.TryParse(command.Data2, out byte TitleID))
                                {
                                    if (RefManager.m_RefTitleNameNew.ContainsKey(TitleID))
                                    {
                                        if (RefManager.m_ActiveTitleNameNew.ContainsKey(command.Data1))
                                        {
                                            RefManager.m_ActiveTitleNameNew[command.Data1] = TitleID;
                                        }
                                        else
                                        {
                                            RefManager.m_ActiveTitleNameNew.TryAdd(command.Data1, TitleID);
                                        }
                                        Packet stAckMsg = new Packet(0x202B);
                                        stAckMsg.WriteAscii(command.Data1);
                                        stAckMsg.WriteAscii(RefManager.m_RefTitleNameNew[TitleID]);
                                        await AgentServer.BroadcastPacket(stAckMsg);
                                    }
                                }


                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 16) /// REMOVE new title
                        {
                            if (command.Data1 != null)
                            {

                                if (RefManager.m_ActiveTitleNameNew.ContainsKey(command.Data1))
                                {
                                    RefManager.m_ActiveTitleNameNew.Remove(command.Data1);
                                }
                                Packet stAckMsg = new Packet(0x202C);
                                stAckMsg.WriteAscii(command.Data1);
                                await AgentServer.BroadcastPacket(stAckMsg);
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 17)
                        {
                            if (command.Data1 != null && command.Data2 != null && command.Data3 != null && command.Data4 != null && command.Data5 != null &&
                                command.Data6 != null && command.Data7 != null && command.Data8 != null)
                            {

                                if (int.TryParse(command.Data1, out int DbID) && int.TryParse(command.Data2, out int CharID) && int.TryParse(command.Data4, out int ItemIdx)
                                    && int.TryParse(command.Data5, out int Quantity) && byte.TryParse(command.Data8, out byte Plus))
                                {
                                    var session = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == CharID);
                                    if (session != null)
                                    {
                                        if (!session.SessionData.CharacterChest.ContainsKey(DbID))
                                        {
                                            var newrow = new _ItemChest();
                                            newrow.ID = DbID;
                                            newrow.CharID = CharID;
                                            newrow.ItemCodeName = command.Data3;
                                            newrow.ItemID = ItemIdx;
                                            newrow.Quantity = Quantity;
                                            newrow.Date = command.Data6;
                                            newrow.Plus = Plus;

                                            session.SessionData.CharacterChest.TryAdd(DbID, newrow);

                                            Packet ach = new Packet(0x203B);
                                            ach.WriteInt32(DbID);
                                            ach.WriteInt32(ItemIdx);
                                            ach.WriteInt32(Quantity);

                                            ach.WriteAscii(command.Data6 ?? string.Empty);
                                            ach.WriteAscii(command.Data7 ?? string.Empty);
                                            ach.WriteUInt8(Plus);
                                            await session.SendToClient(ach);

                                         

                                        }
                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 18)
                        {
                            if (command.Data1 != null && command.Data2 != null)
                            {
                                if (int.TryParse(command.Data1, out int GateID) && bool.TryParse(command.Data2, out bool State))
                                {
                                    if (AgentServer.eventManager.LiveTeleportManagement.ContainsKey(GateID))
                                    {
                                        AgentServer.eventManager.LiveTeleportManagement[GateID] = State;
                                    }
                                    else
                                    {
                                        AgentServer.eventManager.LiveTeleportManagement.TryAdd(GateID, State);
                                    }
                                }

                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 19)
                        {
                            if (command.Data1 != null && command.Data2 != null)
                            {
                                if (int.TryParse(command.Data1, out int RegionID) && bool.TryParse(command.Data2, out bool State))
                                {
                                    if (AgentServer.eventManager.LiveSkillManagementbyRegionID.ContainsKey(RegionID))
                                    {
                                        AgentServer.eventManager.LiveSkillManagementbyRegionID[RegionID] = State;
                                    }
                                    else
                                    {
                                        AgentServer.eventManager.LiveSkillManagementbyRegionID.TryAdd(RegionID, State);
                                    }
                                }

                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 20)
                        {
                            if (command.Data1 != null && command.Data2 != null)
                            {
                                if (int.TryParse(command.Data1, out int WorldID) && bool.TryParse(command.Data2, out bool State))
                                {
                                    if (AgentServer.eventManager.LiveSkillManagementbyWorldID.ContainsKey(WorldID))
                                    {
                                        AgentServer.eventManager.LiveSkillManagementbyWorldID[WorldID] = State;
                                    }
                                    else
                                    {
                                        AgentServer.eventManager.LiveSkillManagementbyWorldID.TryAdd(WorldID, State);
                                    }
                                }

                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 21)
                        {
                            if (command.Data1 != null && command.Data2 != null)
                            {
                                if (int.TryParse(command.Data1, out int TimeMin) && int.TryParse(command.Data2, out int WorldID))
                                {
                                    Packet pck = new Packet(0x220A);
                                    pck.WriteUInt8(0);
                                    pck.WriteInt32(TimeMin * 1000);

                                    await AgentServer.BroadcastPacketbyWorldID(WorldID, pck);
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 22)
                        {
                            if (command.Data1 != null && command.Data2 != null)
                            {
                                if (int.TryParse(command.Data1, out int TimeMin) && int.TryParse(command.Data2, out int RegionID))
                                {
                                    Packet pck = new Packet(0x220A);
                                    pck.WriteUInt8(0);
                                    pck.WriteInt32(TimeMin * 1000);

                                    await AgentServer.BroadcastPacketbyRegionID(RegionID, pck);
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 23)
                        {
                            if (command.Data1 != null && command.Data2 != null)
                            {
                                if (int.TryParse(command.Data1, out int TimeMin) && int.TryParse(command.Data2, out int WorldID))
                                {
                                    Packet pck = new Packet(0x220A);
                                    pck.WriteUInt8(0);
                                    pck.WriteInt32(TimeMin * 1000);

                                    await AgentServer.BroadcastPacketbyWorldID(WorldID, pck);

                                    if (AgentServer.eventManager.CreatedTimerListWorldID.ContainsKey(WorldID))
                                    {
                                        AgentServer.eventManager.CreatedTimerListWorldID[WorldID] = TimeMin * 1000;
                                    }
                                    else
                                    {
                                        AgentServer.eventManager.CreatedTimerListWorldID.TryAdd(WorldID, TimeMin * 1000);
                                    }
                                    if (timers == null)
                                    {
                                        timers = new System.Timers.Timer();
                                        timers.Elapsed += (sender, e) => AgentServer.UpdateTimers(null); // Timer tetiklendiğinde UpdateTimers fonksiyonunu çağır
                                        timers.Interval = 1000; // Timer'ı her 1 saniyede bir tetiklemek için interval değerini ayarla
                                        timers.AutoReset = true; // Timer'ın otomatik olarak yeniden başlatılmasını etkinleştir
                                        timers.Start(); // Timer'ı başlat
                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 24)
                        {
                            if (command.Data1 != null && command.Data2 != null)
                            {
                                if (int.TryParse(command.Data1, out int TimeMin) && int.TryParse(command.Data2, out int RegionID))
                                {
                                    Packet pck = new Packet(0x220A);
                                    pck.WriteUInt8(0);
                                    pck.WriteInt32(TimeMin * 1000);

                                    await AgentServer.BroadcastPacketbyRegionID(RegionID, pck);

                                    if (AgentServer.eventManager.CreatedTimerListRegionID.ContainsKey(RegionID))
                                    {
                                        AgentServer.eventManager.CreatedTimerListRegionID[RegionID] = TimeMin * 1000;
                                    }
                                    else
                                    {
                                        AgentServer.eventManager.CreatedTimerListRegionID.TryAdd(RegionID, TimeMin * 1000);
                                    }
                                    if (timers == null)
                                    {
                                        timers = new System.Timers.Timer();
                                        timers.Elapsed += (sender, e) => AgentServer.UpdateTimers(null); // Timer tetiklendiğinde UpdateTimers fonksiyonunu çağır
                                        timers.Interval = 1000; // Timer'ı her 1 saniyede bir tetiklemek için interval değerini ayarla
                                        timers.AutoReset = true; // Timer'ın otomatik olarak yeniden başlatılmasını etkinleştir
                                        timers.Start(); // Timer'ı başlat
                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 25)
                        {
                            if (command.Data1 != null && command.Data2 != null)
                            {
                                if (int.TryParse(command.Data2, out int WorldID))
                                {

                                    if (AgentServer.eventManager.CreatedKillCounterWorldID.ContainsKey(WorldID))
                                    {
                                        Packet pck = new Packet(0x207A);
                                        pck.WriteUInt8(0);
                                        pck.WriteAscii(command.Data1);
                                        await AgentServer.BroadcastPacketbyWorldID(WorldID, pck);

                                        if (AgentServer.eventManager.KillCounterKillList.Count() > 0)
                                        {
                                            foreach (var line in AgentServer.eventManager.KillCounterKillList)
                                            {
                                                if (line.Value.WorldID == WorldID)
                                                {
                                                    AgentServer.eventManager.KillCounterKillList.TryRemove(line.Value.CharName16, out SCreatedKillCounterKillList s);
                                                }
                                            }
                                        }

                                    }
                                    else
                                    {
                                        AgentServer.eventManager.CreatedKillCounterWorldID.TryAdd(WorldID, command.Data1);
                                        Packet pck = new Packet(0x207A);
                                        pck.WriteUInt8(1);
                                        pck.WriteAscii(command.Data1);
                                        await AgentServer.BroadcastPacketbyWorldID(WorldID, pck);

                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 26)
                        {

                            if (command.Data1 != null && command.Data2 != null && command.Data3 != null)
                            {
                                if (int.TryParse(command.Data1, out int WorldID) && int.TryParse(command.Data3, out int Kill))
                                {
                                    if (AgentServer.eventManager.CreatedKillCounterWorldID.ContainsKey(WorldID))
                                    {
                                        string Charname = command.Data2;
                                        if (AgentServer.eventManager.KillCounterKillList.ContainsKey(Charname))
                                        {
                                            if (AgentServer.eventManager.KillCounterKillList[Charname].WorldID == WorldID)
                                            {
                                                AgentServer.eventManager.KillCounterKillList[Charname].Kill += Kill;
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
                                            Create.Kill = Kill;
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
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 27)
                        {

                            if (command.Data1 != null && command.Data2 != null && command.Data3 != null && command.Data4 != null)
                            {

                                if (int.TryParse(command.Data1, out int CharID) && int.TryParse(command.Data2, out int JID) && int.TryParse(command.Data3, out int Silk) && int.TryParse(command.Data4, out int SilkRank))
                                {
                                    if (RankManager.m_SilkRank.ContainsKey(CharID))
                                    {
                                        RankManager.m_SilkRank[CharID].SilkHistory += Silk;
                                        RankManager.m_SilkRank[CharID].SilkRank = SilkRank;
                                    }
                                    else
                                    {
                                        var srank = new _SilkRank();
                                        srank.CharID = CharID;
                                        srank.JID = JID;
                                        srank.SilkHistory = Silk;
                                        srank.SilkRank = SilkRank;
                                        RankManager.m_SilkRank.TryAdd(CharID, srank);
                                    }

                                    var session = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == CharID);
                                    if (session != null)
                                    {
                                        Packet packet = new Packet(0x209C);
                                        packet.WriteInt32(RankManager.m_SilkRank[CharID].SilkHistory);
                                        packet.WriteInt32(RankManager.m_SilkRank[CharID].SilkRank);
                                        await session.SendToClient(packet);
                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 28)
                        {
                            if (command.Data1 != null && command.Data2 != null && command.Data3 != null)
                            {
                                if (int.TryParse(command.Data1, out int CharID) && byte.TryParse(command.Data3, out byte Type))
                                {
                                    var session = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == CharID);

                                    if (session != null)
                                    {
                                        Packet Notice = new Packet(0x168A, false, false);
                                        Notice.WriteUInt8(Type);
                                        Notice.WriteUnicode(command.Data2); // Message
                                        await session.SendToClient(Notice);
                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 29)
                        {
                            if (command.Data1 != null && command.Data2 != null && command.Data3 != null && command.Data4 != null && command.Data5 != null)
                            {
                                if (int.TryParse(command.Data1, out int CharID) && int.TryParse(command.Data2, out int RefAchievementID)
                                    && int.TryParse(command.Data3, out int RefAchievementConditionID) && Int64.TryParse(command.Data4, out Int64 Progress)
                                    && byte.TryParse(command.Data5, out byte State))
                                {
                                    var session = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == CharID);

                                    if (session != null)
                                    {
                                        if (session.SessionData.CharacterAchievement.ContainsKey(RefAchievementID))
                                        {
                                            session.SessionData.CharacterAchievement[RefAchievementID].State = State;
                                            if (session.SessionData.CharacterAchievementCondition.ContainsKey(RefAchievementConditionID))
                                            {
                                                int CompleteCount = 0;
                                                /*if (g_RefDataMgr.m_RefAchiemeventsCondition.find(RefAchievementConditionID) != g_RefDataMgr.m_RefAchiemeventsCondition.end())
                                                {
                                                    CompleteCount = g_RefDataMgr.m_RefAchiemeventsCondition[RefAchievementConditionID].CompleteCount;
                                                }
                                                if (Progress >= CompleteCount)
                                                {

                                                }*/
                                                session.SessionData.CharacterAchievementCondition[RefAchievementConditionID].ProgressCount = Progress;
                                                Packet pck = new Packet(0x177E);
                                                pck.WriteInt32(RefAchievementID);
                                                pck.WriteInt32(RefAchievementConditionID);
                                                pck.WriteInt64(Progress);
                                                pck.WriteUInt8(State);
                                                await session.SendToClient(pck);
                                            }
                                        }
                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });

                        }
                        else if (CommandID == 30)
                        {
                            if (command.Data1 != null)
                            {
                                if (int.TryParse(command.Data1, out int WorldID))
                                {
                                    if (AgentServer.eventManager.CreatedTeamKillCounterWorldID.ContainsKey(WorldID))
                                    {
                                        Packet pck = new Packet(0x189A);
                                        pck.WriteUInt8(0);
                                        await AgentServer.BroadcastPacketbyWorldID(WorldID, pck);
                                        if (AgentServer.eventManager.TeamKillCounterKillList.Count() > 0)
                                        {
                                            foreach (var line in AgentServer.eventManager.TeamKillCounterKillList)
                                            {
                                                if (line.Value.WorldID == WorldID)
                                                {
                                                    AgentServer.eventManager.TeamKillCounterKillList.TryRemove(line.Value.CharName16, out SCreatedTeamKillCounterKillList a);
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        AgentServer.eventManager.CreatedTeamKillCounterWorldID.TryAdd(WorldID, command.Data1);
                                        Packet pck = new Packet(0x189A);
                                        pck.WriteUInt8(1);
                                        await AgentServer.BroadcastPacketbyWorldID(WorldID, pck);

                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 31)
                        {
                            if (command.Data1 != null && command.Data2 != null && command.Data3 != null && command.Data4 != null)
                            {
                                if (int.TryParse(command.Data1, out int WorldID) && int.TryParse(command.Data3, out int Kill) && int.TryParse(command.Data4, out int Team))
                                {
                                    if (AgentServer.eventManager.CreatedTeamKillCounterWorldID.ContainsKey(WorldID))
                                    {
                                        string Charname = command.Data2;
                                        if (AgentServer.eventManager.TeamKillCounterKillList.ContainsKey(Charname))
                                        {
                                            if (AgentServer.eventManager.TeamKillCounterKillList[Charname].WorldID == WorldID)
                                            {
                                                AgentServer.eventManager.TeamKillCounterKillList[Charname].Kill += Kill;
                                                // update counter
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
                                            Create.CharName16 = Charname;
                                            Create.Kill = Kill;
                                            Create.Team = Team;
                                            AgentServer.eventManager.TeamKillCounterKillList.TryAdd(Charname, Create);

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
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 32)
                        {
                            if (command.Data1 != null)
                            {
                                if (int.TryParse(command.Data1, out int WorldID))
                                {
                                    if (AgentServer.eventManager.CreatedJobKillCounterWorldID.ContainsKey(WorldID))
                                    {
                                        Packet pck = new Packet(0x189C);
                                        pck.WriteUInt8(0);
                                        await AgentServer.BroadcastPacketbyWorldID(WorldID, pck);
                                        if (AgentServer.eventManager.JobKillCounterKillList.Count() > 0)
                                        {
                                            foreach (var line in AgentServer.eventManager.JobKillCounterKillList)
                                            {
                                                if (line.Value.WorldID == WorldID)
                                                {
                                                    AgentServer.eventManager.JobKillCounterKillList.TryRemove(line.Value.CharName16, out SCreatedJobKillCounterKillList a);
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        AgentServer.eventManager.CreatedJobKillCounterWorldID.TryAdd(WorldID, command.Data1);
                                        Packet pck = new Packet(0x189C);
                                        pck.WriteUInt8(1);
                                        await AgentServer.BroadcastPacketbyWorldID(WorldID, pck);
                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 33)
                        {
                            if (command.Data1 != null && command.Data2 != null && command.Data3 != null && command.Data4 != null)
                            {
                                if (int.TryParse(command.Data1, out int WorldID) && int.TryParse(command.Data3, out int Kill) && int.TryParse(command.Data4, out int Team))
                                {
                                    if (AgentServer.eventManager.CreatedJobKillCounterWorldID.ContainsKey(WorldID))
                                    {
                                        string Charname = command.Data2;
                                        if (AgentServer.eventManager.JobKillCounterKillList.ContainsKey(Charname))
                                        {
                                            if (AgentServer.eventManager.JobKillCounterKillList[Charname].WorldID == WorldID)
                                            {
                                                AgentServer.eventManager.JobKillCounterKillList[Charname].Kill += Kill;
                                                // update counter
                                                var topKillers = AgentServer.eventManager.JobKillCounterKillList.Where(x => x.Value.WorldID == WorldID).OrderByDescending(x => x.Value.Kill).Take(5).ToList();


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
                                                    await AgentServer.BroadcastPacketbyWorldID(WorldID, counter);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            var Create = new SCreatedJobKillCounterKillList();
                                            Create.WorldID = WorldID;
                                            Create.CharName16 = Charname;
                                            Create.Kill = Kill;
                                            Create.Team = Team;
                                            AgentServer.eventManager.JobKillCounterKillList.TryAdd(Charname, Create);

                                            var topKillers = AgentServer.eventManager.JobKillCounterKillList.Where(x => x.Value.WorldID == WorldID).OrderByDescending(x => x.Value.Kill).Take(5).ToList();

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
                                                await AgentServer.BroadcastPacketbyWorldID(WorldID, counter);
                                            }
                                        }
                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 34)
                        {
                            if (command.Data1 != null)
                            {
                                if (int.TryParse(command.Data1, out int CharID))
                                {
                                    var session = AgentServer.AgentSessions?.FirstOrDefault(x => x.SessionData.Charid == CharID);
                                    if (session != null)
                                    {
                                        session.Stop();
                                        AgentServer.AgentSessions?.Remove(session);
                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                        else if (CommandID == 35)
                        {
                            if (command.Data1 != null && command.Data2 != null && command.Data3 != null && command.Data4 != null)
                            {
                                if (int.TryParse(command.Data4, out int WorldID))
                                {
                                    string CharnameKiller = command.Data1;
                                    string GuildName = command.Data2;
                                    string UnionName = command.Data3;
                                    if (AgentServer.eventManager.FtwKillCounterList.ContainsKey(CharnameKiller))
                                    {
                                        if (AgentServer.eventManager.FtwKillCounterList[CharnameKiller].WorldID == WorldID)
                                        {
                                            AgentServer.eventManager.FtwKillCounterList[CharnameKiller].Kill += 1;
                                        }
                                    }
                                    else
                                    {
                                        var SFtwCounter = new SFortressWarCounter();
                                        SFtwCounter.CharName = CharnameKiller;
                                        SFtwCounter.GuildName = GuildName;
                                        SFtwCounter.UnionName = UnionName;
                                        SFtwCounter.WorldID = WorldID;
                                        SFtwCounter.Kill = 1;

                                        AgentServer.eventManager.FtwKillCounterList.TryAdd(CharnameKiller, SFtwCounter);
                                    }

                                    var topPlayerKillers = AgentServer.eventManager.FtwKillCounterList.Where(x => x.Value.WorldID == WorldID).OrderByDescending(x => x.Value.Kill).Take(5).ToList();

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
                                        await AgentServer.BroadcastPacketbyWorldID(WorldID, counter);
                                    }

                                    var topGuildKillers = AgentServer.eventManager.FtwKillCounterList.Where(x => x.Value.WorldID == WorldID).GroupBy(x => x.Value.GuildName).Select(g => new { GuildName = g.Key, TotalKills = g.Sum(x => x.Value.Kill) }).OrderByDescending(x => x.TotalKills).Take(5).ToList();

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
                                        await AgentServer.BroadcastPacketbyWorldID(WorldID, counter);
                                    }

                                    // Union Kill Counter (top 5)
                                    var topUnionKillers = AgentServer.eventManager.FtwKillCounterList.Where(x => x.Value.WorldID == WorldID).GroupBy(x => x.Value.UnionName).Select(g => new { UnionName = g.Key, TotalKills = g.Sum(x => x.Value.Kill) }).OrderByDescending(x => x.TotalKills).Take(5).ToList();

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
                                        await AgentServer.BroadcastPacketbyWorldID(WorldID, counter);
                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });

                        }
                        else if (CommandID == 36)
                        {
                            if (command.Data1 != null && command.Data2 != null && command.Data3 != null && command.Data4 != null && command.Data5 != null && command.Data6 != null)
                            {
                                if(int.TryParse(command.Data1, out int CharID) && int.TryParse(command.Data2, out int RegionID)
                                    && int.TryParse(command.Data3, out int PosX) && int.TryParse(command.Data4, out int PosY) &&
                                    int.TryParse(command.Data5, out int PosZ) && int.TryParse(command.Data6, out int Second))
                                {
                                    var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charid == CharID);
                                    if(session != null)
                                    {
                                        Packet packet = new Packet(0x193E);
                                        packet.WriteUInt8(0);
                                        packet.WriteInt32(RegionID);
                                        packet.WriteInt32(PosX);
                                        packet.WriteInt32(PosY);
                                        packet.WriteInt32(PosZ);
                                        packet.WriteInt32(Second);
                                        await session.SendToClient(packet);
                                    }
                                }
                            }
                            await sharedConnection.ExecuteAsync("DELETE FROM _AsyncFilterCommands WHERE CommandID = @id", new { id = CommandID });
                        }
                    }
                    catch (Exception ex)
                    {
                        // Her bir komut işleme sırasında oluşan hataları yakalayın ve loglayın
                        Console.WriteLine($"Error processing command {command.CommandID}: {ex.Message}");
                    }
                }
            }
            catch (Exception ex)
            {
                // Genel hataları yakalayın ve loglayın, bağlantıyı kapatmadan devam edin
                Console.WriteLine($"Error during OnTimerTick: {ex.Message}");
            }
        }

        public void Dispose()
        {
            // Bağlantıyı kapat
            if (sharedConnection != null && sharedConnection.State == System.Data.ConnectionState.Open)
            {
                sharedConnection.Close();
                sharedConnection.Dispose();
                sharedConnection = null;
            }
        }
    }
}

