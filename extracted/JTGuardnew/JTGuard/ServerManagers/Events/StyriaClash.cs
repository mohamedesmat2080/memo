using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Dapper;
using JTGuard.Database;
using JTGuard.Database.Model;
using JTGuard.Database.ModelsEvents;
using JTGuard.Helpers;
using JTGuard.Server;
using Microsoft.Data.SqlClient;
using SilkroadSecurityAPI;
using static System.Net.Mime.MediaTypeNames;

namespace JTGuard.ServerManagers.Events
{
    public partial class StyriaClash
    {
        private AgentServer AgentServer { get; set; }
        public _ServerSettings Service { get; set; }
        public EventManager eventManager {  get; set; }

        public List<____EventStyriaClash> m_StyriaClashSetting = new();
        public ConcurrentDictionary<string, SStyriaClashLogger> StyriaClashLogger = new();
        public bool StyriaClashEventStatus = false;
        public bool StyriaClashRegisterStatus = false;
        public List<____EventStyriaClashMob> m_StyriaClashMonsters = new();
        public StyriaClash(_ServerSettings service, AgentServer agentServer, EventManager eventManagers)
        {
            AgentServer = agentServer;
            Service = service;
            eventManager = eventManagers;
            //StartEventManager();
            LoadStyriaClash();
            LoadStyriaClashMob();
        }
        public async Task LoadStyriaClash()
        {
            try
            {
                m_StyriaClashSetting.Clear();

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();


                    string query1 = "SELECT * FROM ____EventStyriaClash with (nolock)";
                    var result1 = await connection.QueryAsync<____EventStyriaClash>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_StyriaClashSetting.Contains(item))
                            m_StyriaClashSetting.Add(item);
                    }
                    //Console.WriteLine("LoadStyriaClash loaded into cache. Total count: " + m_StyriaClashSetting.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadStyriaClash tables: {ex.Message}");
            }
        }
        public async Task LoadStyriaClashMob()
        {
            try
            {
                m_StyriaClashMonsters.Clear();

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();


                    string query1 = "SELECT * FROM ____EventStyriaClashMob with (nolock)";
                    var result1 = await connection.QueryAsync<____EventStyriaClashMob>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_StyriaClashMonsters.Contains(item))
                            m_StyriaClashMonsters.Add(item);
                    }
                    //Console.WriteLine("LoadStyriaClashMob loaded into cache. Total count: " + m_StyriaClashMonsters.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadStyriaClash tables: {ex.Message}");
            }
        }




        public bool isStyriaThreadRunning = false;
        public void StartStyriaClash(int durationMin, int registerMin)
        {

            if (isStyriaThreadRunning)
            {
                return;
            }
            isStyriaThreadRunning = true;
            Task.Run(() => RunStyriaClash(durationMin, registerMin));
        }
        private async Task RunStyriaClash(int eventDuration, int registerTime)
        {
            try
            {
                StyriaClashLogger.Clear();
                await sqlQueryHelper.EXEC_QUERY($"EXEC __DeSpawnMonsterByWorldID {m_StyriaClashSetting[0].WorldID}");

                if (eventManager.LiveSkillManagementbyWorldID.ContainsKey(m_StyriaClashSetting[0].WorldID))
                {
                    eventManager.LiveSkillManagementbyWorldID[m_StyriaClashSetting[0].WorldID] = false;
                }
                else
                {
                    eventManager.LiveSkillManagementbyWorldID.Add(m_StyriaClashSetting[0].WorldID, false);
                }

                // 1. Kayıt İçin Mesaj Gönder ve RegisterTime Dakika Bekle


              
                string startMsg = string.Format(Management.GetNoticeMessage("MSG_STYRIA_REGISTER_OPEN"), registerTime);
                await SendEventNotice(startMsg);
                

                StyriaClashRegisterStatus = true;
                
                int newRegisterTime = registerTime / 2;

                await Task.Delay(TimeSpan.FromMinutes(newRegisterTime));

                // 2. register info gönder
                string infoMsg = string.Format(Management.GetNoticeMessage("MSG_STYRIA_REGISTER_INFO"), newRegisterTime);
                await SendEventNotice(infoMsg);

                await Task.Delay(TimeSpan.FromMinutes(newRegisterTime));

                //// 3... survival kayıt bitti.
                ///
                StyriaClashRegisterStatus = false;
                string closedMsg = Management.GetNoticeMessage("MSG_STYRIA_REGISTER_CLOSED");
                await SendEventNotice(closedMsg);

                await Task.Delay(TimeSpan.FromSeconds(10));

                int totalhunter = 0;
                int totalthief = 0;
                foreach (var data in StyriaClashLogger)
                {
                    if (data.Value.Team == 3)
                    {
                        totalhunter++;
                    }
                    if (data.Value.Team == 1)
                    {
                        totalthief++;
                    }
                }
                /// Find list:
                if (StyriaClashLogger.Count >= m_StyriaClashSetting[0].MinPlayer && totalhunter > 0 && totalthief > 0)
                {
                    StyriaClashEventStatus = true;
                    foreach (var data in StyriaClashLogger)
                    {
                        var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charname == data.Key);
                        if (session == null)
                        {
                            StyriaClashLogger.TryRemove(data.Key, out SStyriaClashLogger value);
                        }
                        else
                        {
                            if (session != null && session.SessionData.JobType != 4)
                            {
                                if (data.Value.Team == 1) // thief
                                {
                                    var packet = new Packet(0x3503, false, false);
                                    packet.WriteUInt8(m_StyriaClashSetting[0].WorldID);
                                    packet.WriteUInt16(m_StyriaClashSetting[0].RegionIDThief);
                                    packet.WriteFloat(m_StyriaClashSetting[0].StartPosXThief);
                                    packet.WriteFloat(m_StyriaClashSetting[0].StartPosYThief);
                                    packet.WriteFloat(m_StyriaClashSetting[0].StartPosZThief);
                                    await session.SendToServer(packet);
                                }
                                else if (data.Value.Team == 3) // trader
                                {
                                    var packet = new Packet(0x3503, false, false);
                                    packet.WriteUInt8(m_StyriaClashSetting[0].WorldID);
                                    packet.WriteUInt16(m_StyriaClashSetting[0].RegionIDHunter);
                                    packet.WriteFloat(m_StyriaClashSetting[0].StartPosXHunter);
                                    packet.WriteFloat(m_StyriaClashSetting[0].StartPosYHunter);
                                    packet.WriteFloat(m_StyriaClashSetting[0].StartPosZHunter);
                                    await session.SendToServer(packet);
                                }
                            }
                        }
                    }

                    await Task.Delay(TimeSpan.FromSeconds(45));

                    foreach (var data in StyriaClashLogger)
                    {
                        var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charname == data.Key);
                        if (session != null)
                        {
                            if (session.SessionData.WorldID != m_StyriaClashSetting[0].WorldID)
                            {
                                StyriaClashLogger.TryRemove(data.Key, out SStyriaClashLogger value);
                            }
                        }
                    }

                    string readyMsg = Management.GetNoticeMessage("MSG_STYRIA_EVENT_WILL_START");
                    await SendNoticetoWorldID(readyMsg, m_StyriaClashSetting[0].WorldID);

                    await CreateJobKillCounter(m_StyriaClashSetting[0].WorldID);

                    await Task.Delay(TimeSpan.FromSeconds(30));

                    if (eventManager.LiveSkillManagementbyWorldID.ContainsKey(m_StyriaClashSetting[0].WorldID))
                    {
                        eventManager.LiveSkillManagementbyWorldID[m_StyriaClashSetting[0].WorldID] = true;
                    }

                    string readyMsg2 = Management.GetNoticeMessage("MSG_STYRIA_EVENT_START");
                    await SendNoticetoWorldID(readyMsg2, m_StyriaClashSetting[0].WorldID);
                 

                    Packet pck = new Packet(0x220A);
                    pck.WriteUInt8(0);
                    pck.WriteInt32((eventDuration * 60) * 1000);
                    await AgentServer.BroadcastPacketbyWorldID(m_StyriaClashSetting[0].WorldID, pck);

                    if (eventManager.CreatedTimerListWorldID.ContainsKey(m_StyriaClashSetting[0].WorldID))
                    {
                        eventManager.CreatedTimerListWorldID[m_StyriaClashSetting[0].WorldID] = (eventDuration * 60) * 1000;
                    }
                    else
                    {
                        eventManager.CreatedTimerListWorldID.TryAdd(m_StyriaClashSetting[0].WorldID, (eventDuration * 60) * 1000);
                    }
                    if (AgentServer.dbCommands.timers == null)
                    {
                        AgentServer.dbCommands.timers = new System.Timers.Timer();
                        AgentServer.dbCommands.timers.Elapsed += (sender, e) => AgentServer.UpdateTimers(null); // Timer tetiklendiğinde UpdateTimers fonksiyonunu çağır
                        AgentServer.dbCommands.timers.Interval = 1000; // Timer'ı her 1 saniyede bir tetiklemek için interval değerini ayarla
                        AgentServer.dbCommands.timers.AutoReset = true; // Timer'ın otomatik olarak yeniden başlatılmasını etkinleştir
                        AgentServer.dbCommands.timers.Start(); // Timer'ı başlat
                    }
                    StartStyriaMobSpawn();

                    var startTime = DateTime.Now;
                    var endTime = startTime.AddMinutes(eventDuration);
                    bool eventIsFinished = false;

                    while (DateTime.Now < endTime)
                    {
                        await Task.Delay(TimeSpan.FromSeconds(3));

                        foreach (var data in StyriaClashLogger)
                        {
                            var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charname == data.Key);
                            if (session != null)
                            {
                                if (session.SessionData.WorldID != m_StyriaClashSetting[0].WorldID)
                                {
                                    StyriaClashLogger.TryRemove(data.Key, out SStyriaClashLogger value);
                                }
                            }
                            else
                            {
                                StyriaClashLogger.TryRemove(data.Key, out SStyriaClashLogger value);
                            }
                            if (StyriaClashLogger.Count < 2)
                            {
                                eventIsFinished = true;
                                break;
                            }
                        }

                        if (eventIsFinished)
                        {
                            break;
                        }
                    }

                    StyriaClashEventStatus = false;

                    string readyMsg3 = Management.GetNoticeMessage("MSG_STYRIA_FINISHED");
                    Packet stMsg3 = new Packet(0x168A);
                    stMsg3.WriteUInt8(NoticeType.QUEST);
                    stMsg3.WriteUnicode(readyMsg3);
                    await AgentServer.BroadcastPacketbyWorldID(m_StyriaClashSetting[0].WorldID, stMsg3);


                    await sqlQueryHelper.EXEC_QUERY($"EXEC __DeSpawnMonsterByWorldID {m_StyriaClashSetting[0].WorldID}");
                    await Task.Delay(TimeSpan.FromSeconds(10));

                    await sqlQueryHelper.EXEC_QUERY($"EXEC __TeleportToTownAllbyWorldID {m_StyriaClashSetting[0].WorldID}");

                    await Task.Delay(TimeSpan.FromSeconds(10));

                    int totalthiefpoint = 0;
                    int totalhunterpoint = 0;

                    foreach (var data in StyriaClashLogger)
                    {
                        if (data.Value.Team == 1)
                        {
                            totalthiefpoint += data.Value.Kill;
                        }
                        else if (data.Value.Team == 3)
                        {
                            totalhunterpoint += data.Value.Kill;
                        }
                    }

                    if (totalhunterpoint > totalthiefpoint)
                    {
                        string strWelcomeMsg = Management.GetNoticeMessage("MSG_STYRIA_HUNTER_WIN");
                        await SendEventNotice(strWelcomeMsg);


                        foreach (var data in StyriaClashLogger)
                        {
                            if (data.Value.Team == 3)
                            {
                                await sqlQueryHelper.AddItemToChest(data.Value.CharID, m_StyriaClashSetting[0].WinnerRewardName,
                                    m_StyriaClashSetting[0].WinnerRewardQuantity, "Styria Clash", 0);

                                var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charid == data.Value.CharID);
                                if (session != null)
                                {
                                    Packet st = new Packet(0x169C);
                                    st.WriteUInt8(12);
                                    st.WriteUnicode("item [%ls] added to chest.");
                                    st.WriteInt32(m_StyriaClashSetting[0].WinnerRewardID);
                                    await session.SendToClient(st);
                                }
                            }
                            else if (data.Value.Team == 1)
                            {
                                await sqlQueryHelper.AddItemToChest(data.Value.CharID, m_StyriaClashSetting[0].LoserRewardName,
                                    m_StyriaClashSetting[0].LoserRewardQuantity, "Styria Clash", 0);

                                var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charid == data.Value.CharID);
                                if (session != null)
                                {
                                    Packet st = new Packet(0x169C);
                                    st.WriteUInt8(12);
                                    st.WriteUnicode("item [%ls] added to chest.");
                                    st.WriteInt32(m_StyriaClashSetting[0].LoserRewardID);
                                    await session.SendToClient(st);
                                }
                            }
                        }
                    }
                    else if (totalthiefpoint > totalhunterpoint)
                    {
                        string strWelcomeMsg = Management.GetNoticeMessage("MSG_STYRIA_THIEF_WIN");
                        await SendEventNotice(strWelcomeMsg);


                        foreach (var data in StyriaClashLogger)
                        {
                            if (data.Value.Team == 1)
                            {
                                await sqlQueryHelper.AddItemToChest(data.Value.CharID, m_StyriaClashSetting[0].WinnerRewardName,
                                    m_StyriaClashSetting[0].WinnerRewardQuantity, "Styria Clash", 0);

                                var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charid == data.Value.CharID);
                                if (session != null)
                                {
                                    Packet st = new Packet(0x169C);
                                    st.WriteUInt8(12);
                                    st.WriteUnicode("item [%ls] added to chest.");
                                    st.WriteInt32(m_StyriaClashSetting[0].WinnerRewardID);
                                    await session.SendToClient(st);
                                }
                            }
                            else if (data.Value.Team == 3)
                            {
                                await sqlQueryHelper.AddItemToChest(data.Value.CharID, m_StyriaClashSetting[0].LoserRewardName,
                                    m_StyriaClashSetting[0].LoserRewardQuantity, "Styria Clash", 0);

                                var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charid == data.Value.CharID);
                                if (session != null)
                                {
                                    Packet st = new Packet(0x169C);
                                    st.WriteUInt8(12);
                                    st.WriteUnicode("item [%ls] added to chest.");
                                    st.WriteInt32(m_StyriaClashSetting[0].LoserRewardID);
                                    await session.SendToClient(st);
                                }
                            }
                        }
                    }
                    else if (totalhunterpoint == totalthiefpoint)
                    {
                        string strWelcomeMsg = Management.GetNoticeMessage("MSG_STYRIA_DRAW");
                        await SendEventNotice(strWelcomeMsg);

                        foreach (var data in StyriaClashLogger)
                        {
                            await sqlQueryHelper.AddItemToChest(data.Value.CharID, m_StyriaClashSetting[0].LoserRewardName,
                                                       m_StyriaClashSetting[0].LoserRewardQuantity, "Styria Clash", 0);


                            var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charid == data.Value.CharID);
                            if (session != null)
                            {
                                Packet st = new Packet(0x169C);
                                st.WriteUInt8(12);
                                st.WriteUnicode("item [%ls] added to chest.");
                                st.WriteInt32(m_StyriaClashSetting[0].LoserRewardID);
                                await session.SendToClient(st);
                            }
                        }
                    }
                    StyriaClashLogger.Clear();
                }
                else
                {
                    FinishLabel:
                    StyriaClashEventStatus = false;
                    string closedmsg = Management.GetNoticeMessage("MSG_EVENT_CANCELLED");
                    await SendEventNotice(closedmsg);
                    StyriaClashLogger.Clear();
                }

                isStyriaThreadRunning = false;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Exception in Styria Clash Event: {ex.Message}");
                isStyriaThreadRunning = false;
            }
        }
        public async Task SendNoticetoWorldID(string Text, int WorldID)
        {
            Packet stMsg = new Packet(0x168A);
            stMsg.WriteUInt8(NoticeType.QUEST);
            stMsg.WriteUnicode(Text);
            await AgentServer.BroadcastPacketbyWorldID(WorldID, stMsg);
        }

        public async Task SendEventNotice(string Text)
        {
            Packet stMsg = new Packet(0x168A);
            stMsg.WriteUInt8(NoticeType.QUEST);
            stMsg.WriteUnicode(Text);
            await AgentServer.BroadcastPacket(stMsg);
        }
        public async Task CreateJobKillCounter(int WorldID)
        {
            if (eventManager.CreatedJobKillCounterWorldID.ContainsKey(WorldID))
            {
                Packet pck = new Packet(0x189C);
                pck.WriteUInt8(0);
                await AgentServer.BroadcastPacketbyWorldID(WorldID, pck);
                if (eventManager.JobKillCounterKillList.Count() > 0)
                {
                    foreach (var line in eventManager.JobKillCounterKillList)
                    {
                        if (line.Value.WorldID == WorldID)
                        {
                            eventManager.JobKillCounterKillList.TryRemove(line.Value.CharName16, out SCreatedJobKillCounterKillList a);
                        }
                    }
                }
            }
            else
            {
                eventManager.CreatedJobKillCounterWorldID.TryAdd(WorldID, "0");
                Packet pck = new Packet(0x189C);
                pck.WriteUInt8(1);
                await AgentServer.BroadcastPacketbyWorldID(WorldID, pck);
            }
        }

        private static bool isStyriaMobThreadRunning = false;

        private void StartStyriaMobSpawn()
        {
            if (isStyriaMobThreadRunning) return;

            isStyriaMobThreadRunning = true;
            Task.Run(async () =>
            {
                try
                {
                    int mobSpawnCount = 5;
                    int currentCount = 0;

                    while (currentCount <= mobSpawnCount)
                    {
                        if (StyriaClashEventStatus)
                        {
                            foreach (var data in m_StyriaClashMonsters)
                            {
                                await sqlQueryHelper.EXEC_QUERY($"EXEC __SpawnMonsterAtPosition {data.MobID}, {data.WorldID}, {data.RegionID}, {data.PosX}, {data.PosY}, {data.PosZ}, {data.Radius}");
                            }
                        }
                        // Monstrları spawn etme işlemleri
                        await Task.Delay(TimeSpan.FromMinutes(m_StyriaClashSetting[0].IntervalBoxSpawnMin)); // Aralıklı spawn işlemi
                        currentCount++;
                    }

                    isStyriaMobThreadRunning = false;
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Exception in Styria Mob Spawn: {ex.Message}");
                    isStyriaMobThreadRunning = false;
                }
            });
        }
        public class SStyriaClashLogger
        {

           public string CharName16 { get; set; }
           public int CharID { get; set; }
           public int Kill { get; set; }
           public byte Team { get; set; }
           public string Hwid { get; set; }
           public string JobName { get; set; }
        };
        public class ____EventStyriaClashMob
        {
            public int ID { get; set; }
            public int MobID { get; set; }
            public int WorldID { get; set; }
            public int RegionID { get; set; }
            public int PosX { get; set; }
            public int PosY { get; set; }
            public int PosZ { get; set; }
            public int Radius { get; set; }
            public int Point { get; set; }
        };
    }
}
