using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using JTGuard.Database.Model;
using JTGuard.Database;
using JTGuard.Helpers;
using JTGuard.Server;
using Microsoft.Data.SqlClient;
using SilkroadSecurityAPI;
using JTGuard.Database.ModelsEvents;
using Dapper;

namespace JTGuard.ServerManagers.Events
{
    public partial class LuckyPartyNumber
    {
        private AgentServer AgentServer { get; set; }
        public _ServerSettings Service { get; set; }
        public EventManager eventManager { get; set; }

        public List<____EventLuckyPTNumber> m_LuckyPtNumberSettings { get; set; } = new();
        public int LuckyPartyNumberTargetNumber { get; set; } = 0;
        public bool LuckyPartyNumberEventStatus = false;
        public LuckyPartyNumber(_ServerSettings service, AgentServer agentServer, EventManager eventManagers)
        {
            AgentServer = agentServer;
            Service = service;
            eventManager = eventManagers;
            LuckyPtNumber();
        }
        public async Task LuckyPtNumber()
        {
            try
            {
                m_LuckyPtNumberSettings.Clear();
                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();


                    var search = await connection.QueryAsync<____EventLuckyPTNumber>($"Select * from ____EventLuckyPTNumber");
                    foreach(var item in search)
                    {
                        if(!m_LuckyPtNumberSettings.Contains(item))
                        {
                            m_LuckyPtNumberSettings.Add(item);
                        }
                    }

                    //Console.WriteLine("LuckyPtNumber Loaded : " + m_LuckyPtNumberSettings.Count());
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LuckyPtNumber tables: {ex.Message}");
            }
        }


        public bool isThreadRunning = false;

        public void StartLuckyPartyNumber(int durationMin)
        {

            if (isThreadRunning)
            {
                return;
            }
            isThreadRunning = true;
            Task.Run(() => RunLuckyPartyNumber(durationMin));
        }
        public async Task SendEventNotice(string Text)
        {
            Packet stMsg = new Packet(0x168A);
            stMsg.WriteUInt8(NoticeType.QUEST);
            stMsg.WriteUnicode(Text);
            await AgentServer.BroadcastPacket(stMsg);
        }
        private async Task RunLuckyPartyNumber(int eventDuration)
        {
            try
            {
                int Rounds = m_LuckyPtNumberSettings[0].Round;
                string template = Management.GetNoticeMessage("MSG_LUCKY_PT_NOTICE");
                string startmsg = string.Format(template, Rounds);
                await SendEventNotice(startmsg); /// KAYIT SURESINI GONDER



                await Task.Delay(TimeSpan.FromMinutes(1));

                for (int round = 0; round < Rounds; ++round)
                {
                    LuckyPartyNumberEventStatus = true;

                    int minvalue = m_LuckyPtNumberSettings[0].TargetNumberMin;
                    int maxvalue = m_LuckyPtNumberSettings[0].TargetNumberMax;

                    Random rnd = new Random();
                    int randomValue = rnd.Next(minvalue, maxvalue + 1);

                    // Rastgele sayı üretin ve LuckyPartyNumberTargetNumber'a atayın
                    LuckyPartyNumberTargetNumber = AgentServer.LastPartyMatchingID + randomValue;

                    string targetNumberTemplate = Management.GetNoticeMessage("MSG_LUCKY_PT_TARGET_NUMBER");
                    string targetnumbr = string.Format(targetNumberTemplate, LuckyPartyNumberTargetNumber, eventDuration);

                    await SendEventNotice(targetnumbr);

                    await Task.Delay(TimeSpan.FromMinutes(eventDuration));
                }

                string targetnumbrr = Management.GetNoticeMessage("MSG_LUCKY_PT_TARGET_END");
                await SendEventNotice(targetnumbrr); // Target number'ı gönder
                LuckyPartyNumberEventStatus = false;
                isThreadRunning = false;
            }
            catch (Exception ex)
            {
                isThreadRunning = false;
                Console.WriteLine(ex.Message + " LuckyPtNumber ERROR");
            }
        }
    }
}
