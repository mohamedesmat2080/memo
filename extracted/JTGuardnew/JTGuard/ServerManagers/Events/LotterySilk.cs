using System;
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
using static JTGuard.ServerManagers.Events.DefendTower;

namespace JTGuard.ServerManagers.Events
{
    public partial class LotterySilk
    {
        private AgentServer AgentServer { get; set; }
        public _ServerSettings Service { get; set; }
        public EventManager eventManager { get; set; }
        public int RegisterSilk { get; set; } = 1;
        public Dictionary<string, int> LotterySilkLogger = new();

        public bool RegisterStatus = false;
        public LotterySilk(_ServerSettings service, AgentServer agentServer, EventManager eventManagers)
        {
            AgentServer = agentServer;
            Service = service;
            eventManager = eventManagers;
            GetSilkRegisterPrice();
        }
        public async Task GetSilkRegisterPrice()
        {
            try
            {
                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();


                    var search = await connection.QueryFirstOrDefaultAsync<int>($"Select RegisterSilk from ____EventLotterySilk");
                    RegisterSilk = search;

                    //Console.WriteLine("Lottery Silk Price: " + RegisterSilk);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error GetSilkRegisterPrice tables: {ex.Message}");
            }
        }


        public bool isThreadRunning = false;

        public void StartLotterySilk(int durationMin, int registerMin)
        {

            if (isThreadRunning)
            {
                return;
            }
            isThreadRunning = true;
            Task.Run(() => RunLotterySilk(durationMin, registerMin));
        }
        public async Task SendEventNotice(string Text)
        {
            Packet stMsg = new Packet(0x168A);
            stMsg.WriteUInt8(NoticeType.QUEST);
            stMsg.WriteUnicode(Text);
            await AgentServer.BroadcastPacket(stMsg);
        }
        private async Task RunLotterySilk(int eventDuration, int registerTime)
        {
            try
            {
                LotterySilkLogger.Clear();

                string template = Management.GetNoticeMessage("MSG_LS_REGISTER_OPEN");
                string startmsg = string.Format(template, registerTime, RegisterSilk);

                await SendEventNotice(startmsg); /// KAYIT SURESINI GONDER
                RegisterStatus = true;

                int NewRegisterTime = registerTime / 2;  /// KAYIT SURESININ YARISI KADAR BEKLE

                await Task.Delay(TimeSpan.FromMinutes(NewRegisterTime));
                // 2. register info gönder
                string infomsg = string.Format(Management.GetNoticeMessage("MSG_LS_REGISTER_INFO"), NewRegisterTime);
                await SendEventNotice(infomsg);


                await Task.Delay(TimeSpan.FromMinutes(NewRegisterTime)); /// KALAN KAYIT SURESI KADAR BEKLE


                //// 3... survival kayıt bitti.

                RegisterStatus = false;
                string closedmsg = Management.GetNoticeMessage("MSG_LS_REGISTER_CLOSED");
                await SendEventNotice(closedmsg);
                /// KAYITLARI KAPAT

                await Task.Delay(TimeSpan.FromSeconds(15)); /// KALAN KAYIT SURESI KADAR BEKLE

                if (LotterySilkLogger.Count() > 0)
                {
                    List<string> keys = new List<string>(LotterySilkLogger.Keys);
                    Random rnd = new Random();
                    int randomIndex = rnd.Next(keys.Count);

                    string winnerCharName = keys[randomIndex];

                    string message = $"[Lottery Silk] {winnerCharName} is the lucky person! Earned {LotterySilkLogger.Count * RegisterSilk} silk(s)!";
                    await SendEventNotice(message);
                    await sqlQueryHelper.EXEC_QUERY($"EXEC __LiveSilk {LotterySilkLogger[winnerCharName]}, {LotterySilkLogger.Count * RegisterSilk}, 0, 0");
                }
                else
                {
                    string x = Management.GetNoticeMessage("MSG_EVENT_CANCELLED");
                    await SendEventNotice(x);
                }
                isThreadRunning = false;
                LotterySilkLogger.Clear();

            }
            catch (Exception ex)
            {
                LotterySilkLogger.Clear();
                isThreadRunning = false;
                Console.WriteLine(ex.Message.ToString() + "LOTTER SILK ERROR");
            }
        }
    }
}
