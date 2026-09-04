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
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace JTGuard.ServerManagers.Events
{
    public partial class DefendTower
    {
        private AgentServer AgentServer { get; set; }
        public _ServerSettings Service { get; set; }
        public EventManager eventManager { get; set; }

        public List<____EventDefendTower> m_DefendTowerSetting { get; set; } = new();
        public bool DefendTowerRegisterStatus = false;
        public bool DefendTowerEventStatus = false;
        public ConcurrentDictionary<string, SDefendTowerLogger> DefendTowerLogger = new();
        public DefendTower(_ServerSettings service, AgentServer agentServer, EventManager eventManagers)
        {
            AgentServer = agentServer;
            Service = service;
            eventManager = eventManagers;
            LoadDefendTower();
        }
        public async Task LoadDefendTower()
        {
            try
            {
                m_DefendTowerSetting.Clear();

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();


                    string query1 = "SELECT * FROM ____EventDefendTower with (nolock)";
                    var result1 = await connection.QueryAsync<____EventDefendTower>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_DefendTowerSetting.Contains(item))
                            m_DefendTowerSetting.Add(item);
                    }
                    //Console.WriteLine("LoadDefendTower loaded into cache. Total count: " + m_DefendTowerSetting.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadDefendTower tables: {ex.Message}");
            }
        }

        public bool isDefendThreadRunning = false;

        public void StartDefendTower(int durationMin, int registerMin)
        {

            if (isDefendThreadRunning)
            {
                return;
            }
            isDefendThreadRunning = true;
            Task.Run(() => RunDefendTower(durationMin, registerMin));
        }
        private async Task RunDefendTower(int eventDuration, int registerTime)
        {
            try
            {
                DefendTowerLogger.Clear();

                /// SKILL ATAKLARINI KAPAT !!!!
                if (eventManager.LiveSkillManagementbyWorldID.ContainsKey(m_DefendTowerSetting[0].WorldID))
                {
                    eventManager.LiveSkillManagementbyWorldID[m_DefendTowerSetting[0].WorldID] = false;
                }
                else
                {
                    eventManager.LiveSkillManagementbyWorldID.TryAdd(m_DefendTowerSetting[0].WorldID, false);
                }

                // 1. Kayıt İçin Mesaj Gönder ve RegisterTime Dakika Bekle


                string startmsg = string.Format(Management.GetNoticeMessage("MSG_DEFEND_TOWER_REGISTER_OPEN"), registerTime);
                await SendEventNotice(startmsg); /// KAYIT SURESINI GONDER
                DefendTowerRegisterStatus = true;

                int NewRegisterTime = registerTime / 2;  /// KAYIT SURESININ YARISI KADAR BEKLE

                await Task.Delay(TimeSpan.FromMinutes(NewRegisterTime));
                // 2. register info gönder
                string infomsg = string.Format(Management.GetNoticeMessage("MSG_DEFEND_TOWER_REGISTER_INFO"), NewRegisterTime);
                await SendEventNotice(infomsg);


                await Task.Delay(TimeSpan.FromMinutes(NewRegisterTime)); /// KALAN KAYIT SURESI KADAR BEKLE


                //// 3... survival kayıt bitti.

                DefendTowerRegisterStatus = false;
                string closedmsg = Management.GetNoticeMessage("MSG_DEFEND_TOWER_REGISTER_CLOSED");
                await SendEventNotice(closedmsg);
                /// KAYITLARI KAPAT

                await Task.Delay(TimeSpan.FromSeconds(10)); /// KALAN KAYIT SURESI KADAR BEKLE


                /// Find list:
                if (DefendTowerLogger.Count() >= m_DefendTowerSetting[0].MinPlayer)
                {
                    DefendTowerEventStatus = true;
                    foreach (var data in DefendTowerLogger)
                    {
                        var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charname == data.Key);
                        if (session == null)
                        {
                            DefendTowerLogger.TryRemove(data.Key, out SDefendTowerLogger value);
                        }
                        else
                        {
                            if(data.Value.Team == 1)
                            {
                                var packet = new Packet(0x3503, false, false);
                                packet.WriteUInt8(m_DefendTowerSetting[0].WorldID);
                                packet.WriteUInt16(m_DefendTowerSetting[0].RegionIDTeamRed);
                                packet.WriteFloat(m_DefendTowerSetting[0].StartPosXTeamRed);
                                packet.WriteFloat(m_DefendTowerSetting[0].StartPosYTeamRed);
                                packet.WriteFloat(m_DefendTowerSetting[0].StartPosZTeamRed);
                                await session.SendToServer(packet);
                            }
                            else if(data.Value.Team == 3)
                            {
                                var packet = new Packet(0x3503, false, false);
                                packet.WriteUInt8(m_DefendTowerSetting[0].WorldID);
                                packet.WriteUInt16(m_DefendTowerSetting[0].RegionIDTeamBlue);
                                packet.WriteFloat(m_DefendTowerSetting[0].StartPosXTeamBlue);
                                packet.WriteFloat(m_DefendTowerSetting[0].StartPosYTeamBlue);
                                packet.WriteFloat(m_DefendTowerSetting[0].StartPosZTeamBlue);
                                await session.SendToServer(packet);
                            }
                         
                        }
                    }


                    await Task.Delay(TimeSpan.FromSeconds(30)); 

                    await sqlQueryHelper.EXEC_QUERY($"EXEC __SpawnMonsterAtPosition {m_DefendTowerSetting[0].RedTowerObjID}, {m_DefendTowerSetting[0].WorldID}, " +
                        $"{m_DefendTowerSetting[0].RedTowerRegionID}, {m_DefendTowerSetting[0].RedTowerSpawnPosX}, {m_DefendTowerSetting[0].RedTowerSpawnPosY}, {m_DefendTowerSetting[0].RedTowerSpawnPosZ}, 1");

                    await sqlQueryHelper.EXEC_QUERY($"EXEC __SpawnMonsterAtPosition {m_DefendTowerSetting[0].BlueTowerObjID}, {m_DefendTowerSetting[0].WorldID}, " +
                      $"{m_DefendTowerSetting[0].BlueTowerRegionID}, {m_DefendTowerSetting[0].BlueTowerSpawnPosX}, {m_DefendTowerSetting[0].BlueTowerSpawnPosY}, {m_DefendTowerSetting[0].BlueTowerSpawnPosZ}, 1");

                    await Task.Delay(TimeSpan.FromSeconds(30));

                    foreach (var data in DefendTowerLogger)
                    {
                        var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charname == data.Key);
                        if (session != null)
                        {
                            if(session.SessionData.WorldID == m_DefendTowerSetting[0].WorldID)
                            {
                                if (data.Value.Team == 1) // red
                                {
                                    Packet packet = new Packet(0x3502);
                                    packet.WriteUInt8(1);
                                    await session.SendToServer(packet);
                                }
                                else if (data.Value.Team == 3) // blue
                                {
                                    Packet packet = new Packet(0x3502);
                                    packet.WriteUInt8(3);
                                    await session.SendToServer(packet);
                                }
                            }
                            else
                            {
                                DefendTowerLogger.TryRemove(data.Key, out SDefendTowerLogger value);
                            }
                        }
                    }

                    string readymsg = Management.GetNoticeMessage("MSG_DEFEND_TOWER_EVENT_WILL_START");
                    await SendNoticetoWorldID(readymsg, m_DefendTowerSetting[0].WorldID);
                    await CreateKillCounter(m_DefendTowerSetting[0].WorldID); /// kill counter oluştur



                    await Task.Delay(TimeSpan.FromSeconds(30));  /// 30 saniye hazırlanma süresi ekstra



                    /// SKILL ATAKLARINI AÇ !!!!
                    if (eventManager.LiveSkillManagementbyWorldID.ContainsKey(m_DefendTowerSetting[0].WorldID))
                    {
                        eventManager.LiveSkillManagementbyWorldID[m_DefendTowerSetting[0].WorldID] = true;
                    }
                    string startnewmsg = Management.GetNoticeMessage("MSG_DEFEND_TOWER_EVENT_START");
                    await SendNoticetoWorldID(startnewmsg, m_DefendTowerSetting[0].WorldID);



                    Packet pck = new Packet(0x220A);
                    pck.WriteUInt8(0);
                    pck.WriteInt32((eventDuration * 60) * 1000);
                    await AgentServer.BroadcastPacketbyWorldID(m_DefendTowerSetting[0].WorldID, pck);

                    if (eventManager.CreatedTimerListWorldID.ContainsKey(m_DefendTowerSetting[0].WorldID))
                    {
                        eventManager.CreatedTimerListWorldID[m_DefendTowerSetting[0].WorldID] = (eventDuration * 60) * 1000;
                    }
                    else
                    {
                        eventManager.CreatedTimerListWorldID.TryAdd(m_DefendTowerSetting[0].WorldID, (eventDuration * 60) * 1000);
                    }
                    if (AgentServer.dbCommands.timers == null)
                    {
                        AgentServer.dbCommands.timers = new System.Timers.Timer();
                        AgentServer.dbCommands.timers.Elapsed += (sender, e) => AgentServer.UpdateTimers(null); // Timer tetiklendiğinde UpdateTimers fonksiyonunu çağır
                        AgentServer.dbCommands.timers.Interval = 1000; // Timer'ı her 1 saniyede bir tetiklemek için interval değerini ayarla
                        AgentServer.dbCommands.timers.AutoReset = true; // Timer'ın otomatik olarak yeniden başlatılmasını etkinleştir
                        AgentServer.dbCommands.timers.Start(); // Timer'ı başlat
                    }

                    // 4. Etkinliği Başlat ve EventDuration Dakika Boyunca Bekle
                    /*LOG_WRITE(LOG_FATAL, ("Survival Arena: Etkinlik başladı, " + std::to_string(EventDuration) + " dakika sürecek.").c_str());*/

                    var startTime = DateTime.Now;
                    var endTime = startTime.AddMinutes(eventDuration);
                    bool EventisFinished = false;

                    while (DateTime.Now < endTime)
                    {
                        await Task.Delay(TimeSpan.FromSeconds(3));

                        foreach (var data in DefendTowerLogger)
                        {
                            var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charname == data.Key);
                            if (session != null)
                            {
                                if (session.SessionData.WorldID != m_DefendTowerSetting[0].WorldID)
                                {
                                    DefendTowerLogger.TryRemove(data.Key, out SDefendTowerLogger value);
                                }
                            }
                            else
                            {
                                DefendTowerLogger.TryRemove(data.Key, out SDefendTowerLogger value);
                            }
                        }
                        /// check tower status meeen
                        if (!DefendTowerEventStatus)
                        {
                            EventisFinished = true;
                            /// EVENT IS OWER
                            break;
                        }
                    }

                    DefendTowerEventStatus = false;
                    DefendTowerLogger.Clear();

                    if(!EventisFinished)
                    {
                        string finishmsg = Management.GetNoticeMessage("MSG_DEFEND_TOWER_DRAW");
                        await SendEventNotice(finishmsg);
                    }
        

                    await Task.Delay(TimeSpan.FromSeconds(10));


                    await sqlQueryHelper.EXEC_QUERY($"EXEC __TeleportToTownAllbyWorldID {m_DefendTowerSetting[0].WorldID}");
                    await sqlQueryHelper.EXEC_QUERY($"EXEC __DeSpawnMonsterByWorldID {m_DefendTowerSetting[0].WorldID}");
                    await Task.Delay(TimeSpan.FromSeconds(30));

                }
                else
                {
                    DefendTowerEventStatus = false;
                    string closedmsgx = Management.GetNoticeMessage("MSG_EVENT_CANCELLED");
                    await SendEventNotice(closedmsgx);

                }
                DefendTowerEventStatus = false;
                /// KİLL KONTROL
                DefendTowerLogger.Clear();
                isDefendThreadRunning = false;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Exception in RunDefendTower: {ex.Message}");
                isDefendThreadRunning = false;
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
        public async Task CreateKillCounter(int WorldID)
        {
            if (eventManager.CreatedTeamKillCounterWorldID.ContainsKey(WorldID))
            {
                Packet pck = new Packet(0x189A);
                pck.WriteUInt8(0);
                await AgentServer.BroadcastPacketbyWorldID(WorldID, pck);
                if (eventManager.TeamKillCounterKillList.Count() > 0)
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
                eventManager.CreatedTeamKillCounterWorldID.TryAdd(WorldID, "0");
                Packet pck = new Packet(0x189A);
                pck.WriteUInt8(1);
                await AgentServer.BroadcastPacketbyWorldID(WorldID, pck);

            }
        }
        public class SDefendTowerLogger
        {
            public string CharName16 { get; set; }
            public int CharID { get; set; }
            public int Kill { get; set; }
            public byte Team { get; set; }
            public string Hwid { get; set; }
            public string JobName { get; set; }
        };
    }
}
