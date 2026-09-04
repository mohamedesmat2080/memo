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
using static JTGuard.ServerManagers.Events.SurvivalArena;

namespace JTGuard.ServerManagers.Events
{
    public partial class LastManStanding
    {
        private AgentServer AgentServer { get; set; }
        public _ServerSettings Service { get; set; }
        public EventManager eventManager { get; set; }
        public List<____EventLastManStanding> m_LMSSetting { get; set; } = new();
        public ConcurrentDictionary<string, SLMSLogger> LMSLogger = new();
        public bool LMSEventRegisters = false;
        public bool LMSEventStatus = false;
        public LastManStanding(_ServerSettings service, AgentServer agentServer, EventManager eventManagers)
        {
            AgentServer = agentServer;
            Service = service;
            eventManager = eventManagers;
            //StartEventManager();
            LoadSettings();
        }
        public async Task LoadSettings()
        {
            try
            {
                m_LMSSetting.Clear();

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();


                    string query1 = "SELECT * FROM ____EventLastManStanding with (nolock)";
                    var result1 = await connection.QueryAsync<____EventLastManStanding>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_LMSSetting.Contains(item))
                            m_LMSSetting.Add(item);
                    }
                    //Console.WriteLine("LastManStanding loaded into cache. Total count: " + m_LMSSetting.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LastManStanding tables: {ex.Message}");
            }
        }

        public bool isLmsThreadRunning = false;

        public void StartLMS(int durationMin, int registerMin)
        {

            if (isLmsThreadRunning)
            {
                return;
            }
            isLmsThreadRunning = true;
            Task.Run(() => RunLMS(durationMin, registerMin));
        }
        private async Task RunLMS(int eventDuration, int registerTime)
        {
            try
            {
                LMSLogger.Clear();

                /// SKILL ATAKLARINI KAPAT !!!!
                if (eventManager.LiveSkillManagementbyWorldID.ContainsKey(m_LMSSetting[0].WorldID))
                {
                    eventManager.LiveSkillManagementbyWorldID[m_LMSSetting[0].WorldID] = false;
                }
                else
                {
                    eventManager.LiveSkillManagementbyWorldID.TryAdd(m_LMSSetting[0].WorldID, false);
                }

                // 1. Kayıt İçin Mesaj Gönder ve RegisterTime Dakika Bekle


                string startmsg = string.Format(Management.GetNoticeMessage("MSG_LMS_REGISTER_OPEN"), registerTime);
                await SendEventNotice(startmsg); /// KAYIT SURESINI GONDER
                LMSEventRegisters = true;

                int NewRegisterTime = registerTime / 2;  /// KAYIT SURESININ YARISI KADAR BEKLE

                await Task.Delay(TimeSpan.FromMinutes(NewRegisterTime));
                // 2. register info gönder
                string infomsg = string.Format(Management.GetNoticeMessage("MSG_LMS_REGISTER_INFO"), NewRegisterTime);
                await SendEventNotice(infomsg);


                await Task.Delay(TimeSpan.FromMinutes(NewRegisterTime)); /// KALAN KAYIT SURESI KADAR BEKLE


                //// 3... survival kayıt bitti.

                LMSEventRegisters = false;
                string closedmsg = Management.GetNoticeMessage("MSG_LMS_REGISTER_CLOSED");
                await SendEventNotice(closedmsg);
                /// KAYITLARI KAPAT

                await Task.Delay(TimeSpan.FromSeconds(10)); /// KALAN KAYIT SURESI KADAR BEKLE


                /// Find list:
                if (LMSLogger.Count() >= m_LMSSetting[0].MinPlayer)
                {
                    LMSEventStatus = true;
                    foreach (var data in LMSLogger)
                    {
                        var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charname == data.Key);
                        if (session == null)
                        {
                            LMSLogger.TryRemove(data.Key, out SLMSLogger value);
                        }
                        else
                        {
                            var packet = new Packet(0x3503, false, false);
                            packet.WriteUInt8(m_LMSSetting[0].WorldID);
                            packet.WriteUInt16(m_LMSSetting[0].RegionID);
                            packet.WriteFloat(m_LMSSetting[0].StartPosX);
                            packet.WriteFloat(m_LMSSetting[0].StartPosY);
                            packet.WriteFloat(m_LMSSetting[0].StartPosZ);
                            await session.SendToServer(packet);
                        }
                    }
                    await Task.Delay(TimeSpan.FromMinutes(1)); /// KALAN KAYIT SURESI KADAR BEKLE


                    string readymsg = Management.GetNoticeMessage("MSG_LMS_EVENT_WILL_START");
                    await SendNoticetoWorldID(readymsg, m_LMSSetting[0].WorldID);

                    await CreateKillCounter(m_LMSSetting[0].WorldID, "Last Man Standing"); /// kill counter oluştur



                    await Task.Delay(TimeSpan.FromSeconds(30));  /// 30 saniye hazırlanma süresi ekstra



                    /// SKILL ATAKLARINI AÇ !!!!
                    if (eventManager.LiveSkillManagementbyWorldID.ContainsKey(m_LMSSetting[0].WorldID))
                    {
                        eventManager.LiveSkillManagementbyWorldID[m_LMSSetting[0].WorldID] = true;
                    }

                    string startnewmsg = Management.GetNoticeMessage("MSG_LMS_EVENT_START");
                    await SendNoticetoWorldID(startnewmsg, m_LMSSetting[0].WorldID);



                    Packet pck = new Packet(0x220A);
                    pck.WriteUInt8(0);
                    pck.WriteInt32((eventDuration * 60) * 1000);
                    await AgentServer.BroadcastPacketbyWorldID(m_LMSSetting[0].WorldID, pck);

                    if (eventManager.CreatedTimerListWorldID.ContainsKey(m_LMSSetting[0].WorldID))
                    {
                        eventManager.CreatedTimerListWorldID[m_LMSSetting[0].WorldID] = (eventDuration * 60) * 1000;
                    }
                    else
                    {
                        eventManager.CreatedTimerListWorldID.TryAdd(m_LMSSetting[0].WorldID, (eventDuration * 60) * 1000);
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
                    bool eventIsFinished = false;

                    while (DateTime.Now < endTime)
                    {
                        await Task.Delay(TimeSpan.FromSeconds(3));

                        if(LMSLogger.Count() > 1)
                        {
                            foreach (var data in LMSLogger)
                            {
                                var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charname == data.Key);
                                if (session != null)
                                {
                                    if (session.SessionData.WorldID != m_LMSSetting[0].WorldID)
                                    {
                                        LMSLogger.TryRemove(data.Key, out SLMSLogger value);
                                    }
                                }
                                else
                                {
                                    LMSLogger.TryRemove(data.Key, out SLMSLogger value);
                                }                               
                            }
                        }
                        else if (LMSLogger.Count() == 1)
                        {
                            eventIsFinished = true;
                            /// select winner and break 
                            var winners = LMSLogger.OrderByDescending(x => x.Value.Kill).Take(1).ToList();
                            foreach(var line in winners)
                            {
                                string winmsg = $"[Last Man Standing] {line.Key} is winner with #{line.Value.Kill} kills.";
                                await SendEventNotice(winmsg);
                                await sqlQueryHelper.AddItemToChest(line.Value.CharID, m_LMSSetting[0].WinnerRewardName,
                                       m_LMSSetting[0].WinnerRewardQuantity, "Last Man Standing", 0);

                                var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charid == line.Value.CharID);
                                if (session != null)
                                {
                                    Packet st = new Packet(0x169C);
                                    st.WriteUInt8(12);
                                    st.WriteUnicode("item [%ls] added to chest.");
                                    st.WriteInt32(m_LMSSetting[0].WinnerRewardID);
                                    await session.SendToClient(st);

                                }
                            }
                            break;
                        }
                    }
                    await Task.Delay(TimeSpan.FromSeconds(30));
                    if (eventIsFinished)
                    {
                        LMSEventStatus = false;
                        string finishmsg = Management.GetNoticeMessage("MSG_LMS_FINISHED");
                        await SendEventNotice(finishmsg);

                        await Task.Delay(TimeSpan.FromSeconds(10));


                        await sqlQueryHelper.EXEC_QUERY($"EXEC __TeleportToTownAllbyWorldID {m_LMSSetting[0].WorldID}");
                    }
                    else
                    {
                        LMSEventStatus = false;
                        string finishmsg = Management.GetNoticeMessage("MSG_LMS_DRAW");
                        await SendEventNotice(finishmsg);

                        await Task.Delay(TimeSpan.FromSeconds(10));


                        await sqlQueryHelper.EXEC_QUERY($"EXEC __TeleportToTownAllbyWorldID {m_LMSSetting[0].WorldID}");

                    }

                }
                else
                {
                    LMSEventStatus = false;
                    string closedmsgx = Management.GetNoticeMessage("MSG_EVENT_CANCELLED");
                    await SendEventNotice(closedmsgx);

                }
                LMSEventStatus = false;
                /// KİLL KONTROL
                LMSLogger.Clear();
                isLmsThreadRunning = false;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Exception in RunSurvivalArena: {ex.Message}");
                isLmsThreadRunning = false;
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
        public async Task CreateKillCounter(int WorldID, string Text)
        {
            if (eventManager.CreatedKillCounterWorldID.ContainsKey(WorldID))
            {
                Packet pck = new Packet(0x207A);
                pck.WriteUInt8(0);
                pck.WriteAscii(Text);
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
                eventManager.CreatedKillCounterWorldID.TryAdd(WorldID, Text);
                Packet pck = new Packet(0x207A);
                pck.WriteUInt8(1);
                pck.WriteAscii(Text);
                await AgentServer.BroadcastPacketbyWorldID(WorldID, pck);

            }
        }
        public class SLMSLogger
        {
            public int CharID { get; set; }
            public int Kill { get; set; }
            public string Hwid { get; set; }
        }
    }
}
