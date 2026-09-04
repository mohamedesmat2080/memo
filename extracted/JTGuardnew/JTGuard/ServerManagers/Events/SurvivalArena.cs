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
using static JTGuard.ServerManagers.Events.StyriaClash;

namespace JTGuard.ServerManagers.Events
{
    public partial class SurvivalArena
    {

        private AgentServer AgentServer { get; set; }
        public _ServerSettings Service { get; set; }
        public EventManager eventManager { get; set; }

        public List<____EventSurvivalArena> m_SurvivalArenaSetting = new();
        public ConcurrentDictionary<string, SSurvivalArenaLogger> SurvivalArenaLogger = new();
        public bool SurvivalArenaEventRegisters = false;
        public bool SurvivalArenaEventStatus = false;
        public SurvivalArena(_ServerSettings service, AgentServer agentServer, EventManager eventManagers)
        {
            AgentServer = agentServer;
            Service = service;
            eventManager = eventManagers;
            LoadSurvivalArena();
        }
        public async Task LoadSurvivalArena()
        {
            try
            {
                m_SurvivalArenaSetting.Clear();

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();


                    string query1 = "SELECT * FROM ____EventSurvivalArena with (nolock)";
                    var result1 = await connection.QueryAsync<____EventSurvivalArena>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_SurvivalArenaSetting.Contains(item))
                            m_SurvivalArenaSetting.Add(item);
                    }
                    //Console.WriteLine("LoadSurvivalArena loaded into cache. Total count: " + m_SurvivalArenaSetting.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadSurvivalArena tables: {ex.Message}");
            }
        }
        public bool isSurvivalThreadRunning = false;

        public void StartSurvivalArena(int durationMin, int registerMin)
        {

            if (isSurvivalThreadRunning)
            {
                return;
            }
            isSurvivalThreadRunning = true;
            Task.Run(() => RunSurvivalArena(durationMin, registerMin));
        }
        private async Task RunSurvivalArena(int eventDuration, int registerTime)
        {
            try
            {
                SurvivalArenaLogger.Clear();

                /// SKILL ATAKLARINI KAPAT !!!!
                if (eventManager.LiveSkillManagementbyWorldID.ContainsKey(m_SurvivalArenaSetting[0].WorldID))
                {
                    eventManager.LiveSkillManagementbyWorldID[m_SurvivalArenaSetting[0].WorldID] = false;
                }
                else
                {
                    eventManager.LiveSkillManagementbyWorldID.TryAdd(m_SurvivalArenaSetting[0].WorldID, false);
                }

                // 1. Kayıt İçin Mesaj Gönder ve RegisterTime Dakika Bekle


                string startmsg = string.Format(Management.GetNoticeMessage("MSG_SURVIVAL_REGISTER_OPEN"), registerTime);
                await SendEventNotice(startmsg); /// KAYIT SURESINI GONDER
                SurvivalArenaEventRegisters = true;

                int NewRegisterTime = registerTime / 2;  /// KAYIT SURESININ YARISI KADAR BEKLE

                await Task.Delay(TimeSpan.FromMinutes(NewRegisterTime));
                // 2. register info gönder
                string infomsg = string.Format(Management.GetNoticeMessage("MSG_SURVIVAL_REGISTER_INFO"), NewRegisterTime);
                await SendEventNotice(infomsg);


                await Task.Delay(TimeSpan.FromMinutes(NewRegisterTime)); /// KALAN KAYIT SURESI KADAR BEKLE


                //// 3... survival kayıt bitti.

                SurvivalArenaEventRegisters = false;
                string closedmsg = Management.GetNoticeMessage("MSG_SURVIVAL_REGISTER_CLOSED");
                await SendEventNotice(closedmsg);
                /// KAYITLARI KAPAT

                await Task.Delay(TimeSpan.FromSeconds(10)); /// KALAN KAYIT SURESI KADAR BEKLE


                /// Find list:
                if (SurvivalArenaLogger.Count() >= m_SurvivalArenaSetting[0].MinPlayer)
                {
                    SurvivalArenaEventStatus = true;
                    foreach (var data in SurvivalArenaLogger)
                    {
                        var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charname == data.Key);
                        if (session == null)
                        {
                            SurvivalArenaLogger.TryRemove(data.Key, out SSurvivalArenaLogger value);
                        }
                        else
                        {
                            var packet = new Packet(0x3503, false, false);
                            packet.WriteUInt8(m_SurvivalArenaSetting[0].WorldID);
                            packet.WriteUInt16(m_SurvivalArenaSetting[0].RegionID);
                            packet.WriteFloat(m_SurvivalArenaSetting[0].StartPosX);
                            packet.WriteFloat(m_SurvivalArenaSetting[0].StartPosY);
                            packet.WriteFloat(m_SurvivalArenaSetting[0].StartPosZ);
                            await session.SendToServer(packet);
                        }
                    }
                    await Task.Delay(TimeSpan.FromMinutes(1)); /// KALAN KAYIT SURESI KADAR BEKLE

                                                               /// KillCounter oluştur. !!!!

                    string readymsg = Management.GetNoticeMessage("MSG_SURVIVAL_ARENA_EVENT_WILL_START");
                    await SendNoticetoWorldID(readymsg, m_SurvivalArenaSetting[0].WorldID);

                    await CreateKillCounter(m_SurvivalArenaSetting[0].WorldID, "Survival Arena"); /// kill counter oluştur



                    await Task.Delay(TimeSpan.FromSeconds(30));  /// 30 saniye hazırlanma süresi ekstra



                    /// SKILL ATAKLARINI AÇ !!!!
                    if (eventManager.LiveSkillManagementbyWorldID.ContainsKey(m_SurvivalArenaSetting[0].WorldID))
                    {
                        eventManager.LiveSkillManagementbyWorldID[m_SurvivalArenaSetting[0].WorldID] = true;
                    }

                    string startnewmsg = Management.GetNoticeMessage("MSG_SURVIVAL_ARENA_EVENT_START");
                    await SendNoticetoWorldID(startnewmsg, m_SurvivalArenaSetting[0].WorldID);



                    Packet pck = new Packet(0x220A);
                    pck.WriteUInt8(0);
                    pck.WriteInt32((eventDuration * 60) * 1000);
                    await AgentServer.BroadcastPacketbyWorldID(m_SurvivalArenaSetting[0].WorldID, pck);

                    if (eventManager.CreatedTimerListWorldID.ContainsKey(m_SurvivalArenaSetting[0].WorldID))
                    {
                        eventManager.CreatedTimerListWorldID[m_SurvivalArenaSetting[0].WorldID] = (eventDuration * 60) * 1000;
                    }
                    else
                    {
                        eventManager.CreatedTimerListWorldID.TryAdd(m_SurvivalArenaSetting[0].WorldID, (eventDuration * 60) * 1000);
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

                        foreach (var data in SurvivalArenaLogger)
                        {
                            var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charname == data.Key);
                            if (session != null)
                            {
                                if (session.SessionData.WorldID != m_SurvivalArenaSetting[0].WorldID)
                                {
                                    SurvivalArenaLogger.TryRemove(data.Key, out SSurvivalArenaLogger value);
                                }
                            }
                            else
                            {
                                SurvivalArenaLogger.TryRemove(data.Key, out SSurvivalArenaLogger value);
                            }
                            if (SurvivalArenaLogger.Count < 2)
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


                    SurvivalArenaEventStatus = false;
                    string finishmsg = Management.GetNoticeMessage("MSG_SURVIVAL_FINISHED");
                    await SendEventNotice(finishmsg);

                    await Task.Delay(TimeSpan.FromSeconds(10));


                    await sqlQueryHelper.EXEC_QUERY($"EXEC __TeleportToTownAllbyWorldID {m_SurvivalArenaSetting[0].WorldID}");

                    await Task.Delay(TimeSpan.FromSeconds(30));

                    // Verileri vector içine al

                    var winners = SurvivalArenaLogger.OrderByDescending(x => x.Value.Kill).Take(2).ToList();

                    int i = 0;
                    foreach (var line in winners)
                    {
                        if (i == 0)
                        {
                            string winmsg = $"[Survival Arena] {line.Key} is first winner with #{line.Value.Kill} kills.";
                            await SendEventNotice(winmsg);
                            await sqlQueryHelper.AddItemToChest(line.Value.CharID, m_SurvivalArenaSetting[0].WinnerRewardName,
                                   m_SurvivalArenaSetting[0].WinnerRewardQuantity, "Survival Arena", 0);

                            var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charid == line.Value.CharID);
                            if (session != null)
                            {
                                Packet st = new Packet(0x169C);
                                st.WriteUInt8(12);
                                st.WriteUnicode("item [%ls] added to chest.");
                                st.WriteInt32(m_SurvivalArenaSetting[0].WinnerRewardID);
                                await session.SendToClient(st);
                            }
                        }
                        else if (i == 1)
                        {
                            string winmsg = $"[Survival Arena] {line.Key} is second winner with #{line.Value.Kill} kills.";
                            await SendEventNotice(winmsg);
                            await sqlQueryHelper.AddItemToChest(line.Value.CharID, m_SurvivalArenaSetting[0].SecondWinnerRewardName,
                                   m_SurvivalArenaSetting[0].SecondWinnerRewardQuantity, "Survival Arena", 0);

                            var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charid == line.Value.CharID);
                            if (session != null)
                            {
                                Packet st = new Packet(0x169C);
                                st.WriteUInt8(12);
                                st.WriteUnicode("item [%ls] added to chest.");
                                st.WriteInt32(m_SurvivalArenaSetting[0].SecondWinnerRewardID);
                                await session.SendToClient(st);
                            }
                        }

                        i++;
                    }
                }
                else
                {
                    SurvivalArenaEventStatus = false;
                    string closedmsgx = Management.GetNoticeMessage("MSG_EVENT_CANCELLED");
                    await SendEventNotice(closedmsgx);

                }
                SurvivalArenaEventStatus = false;
                /// KİLL KONTROL
                SurvivalArenaLogger.Clear();
                isSurvivalThreadRunning = false;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Exception in RunSurvivalArena: {ex.Message}");
                isSurvivalThreadRunning = false;
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
        public class SSurvivalArenaLogger
        {
            public int CharID { get; set; }
            public int Kill { get; set; }
            public string Hwid { get; set; }
        }
    }
}
