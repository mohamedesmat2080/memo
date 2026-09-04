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
using static System.Runtime.InteropServices.JavaScript.JSType;
using static JTGuard.ServerManagers.Events.DefendTower;

namespace JTGuard.ServerManagers.Events
{
    public partial class LotteryGold
    {
        private AgentServer AgentServer { get; set; }
        public _ServerSettings Service { get; set; }
        public EventManager eventManager { get; set; }
        public Int64 RegisterGold { get; set; } = 1;
        public Dictionary<string, int> LotteryGoldLogger = new();

        public bool RegisterStatus = false;
        public LotteryGold(_ServerSettings service, AgentServer agentServer, EventManager eventManagers)
        {
            AgentServer = agentServer;
            Service = service;
            eventManager = eventManagers;
            GetGoldRegisterPrice();
        }
        public async Task GetGoldRegisterPrice()
        {
            try
            {
                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();


                    var search = await connection.QueryFirstOrDefaultAsync<Int64>($"Select RegisterGold from ____EventLotteryGold");
                    RegisterGold= search;

                    //Console.WriteLine("Lottery Gold Price: " + RegisterGold);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error GetGoldRegisterPrice tables: {ex.Message}");
            }
        }


        public bool isThreadRunning = false;

        public void StartLotteryGold(int durationMin, int registerMin)
        {

            if (isThreadRunning)
            {
                return;
            }
            isThreadRunning = true;
            Task.Run(() => RunLotteryGold(durationMin, registerMin));
        }
        public async Task SendEventNotice(string Text)
        {
            Packet stMsg = new Packet(0x168A);
            stMsg.WriteUInt8(NoticeType.QUEST);
            stMsg.WriteUnicode(Text);
            await AgentServer.BroadcastPacket(stMsg);
        }
        private async Task RunLotteryGold(int eventDuration, int registerTime)
        {
            try
            {
                LotteryGoldLogger.Clear();


                string startmsg = string.Format(Management.GetNoticeMessage("MSG_LG_REGISTER_OPEN"), registerTime, RegisterGold);
                await SendEventNotice(startmsg); /// KAYIT SURESINI GONDER
                RegisterStatus = true;

                int NewRegisterTime = registerTime / 2;  /// KAYIT SURESININ YARISI KADAR BEKLE

                await Task.Delay(TimeSpan.FromMinutes(NewRegisterTime));
                // 2. register info gönder
                string infomsg = string.Format(Management.GetNoticeMessage("MSG_LG_REGISTER_INFO"), NewRegisterTime);
                await SendEventNotice(infomsg);


                await Task.Delay(TimeSpan.FromMinutes(NewRegisterTime)); /// KALAN KAYIT SURESI KADAR BEKLE


                //// 3... survival kayıt bitti.

                RegisterStatus = false;
                string closedmsg = Management.GetNoticeMessage("MSG_LG_REGISTER_CLOSED");
                await SendEventNotice(closedmsg);
                /// KAYITLARI KAPAT

                await Task.Delay(TimeSpan.FromSeconds(15)); /// KALAN KAYIT SURESI KADAR BEKLE

                if (LotteryGoldLogger.Count() > 0)
                {
                    List<string> keys = new List<string>(LotteryGoldLogger.Keys);
                    Random rnd = new Random();
                    int randomIndex = rnd.Next(keys.Count);

                    string winnerCharName = keys[randomIndex];

                    string message = $"[Lottery Silk] {winnerCharName} is the lucky person! Earned {LotteryGoldLogger.Count * RegisterGold} gold(s)!";
                    await SendEventNotice(message);
                    await sqlQueryHelper.EXEC_QUERY($"EXEC __LiveSilk {LotteryGoldLogger[winnerCharName]}, {LotteryGoldLogger.Count * RegisterGold}, 0, 0");

                    var session = AgentServer.AgentSessions.FirstOrDefault(x => x.SessionData.Charname == winnerCharName);
                    if (session != null)
                    {
                        await sqlQueryHelper.EXEC_QUERY($"EXEC __LiveGold {LotteryGoldLogger[winnerCharName]}, {LotteryGoldLogger.Count * RegisterGold}, 1");
                    }
                    else
                    {
                        await sqlQueryHelper.EXEC_QUERY($"UPDATE {Service.ShardDB}.._Char set RemainGold={LotteryGoldLogger.Count * RegisterGold} where CharID = {LotteryGoldLogger[winnerCharName]}");
                    }

                }
                else
                {
                    string x = Management.GetNoticeMessage("MSG_EVENT_CANCELLED");
                    await SendEventNotice(x);
                }
                isThreadRunning = false;
                LotteryGoldLogger.Clear();

            }
            catch (Exception ex)
            {
                LotteryGoldLogger.Clear();
                isThreadRunning = false;
                Console.WriteLine(ex.Message.ToString() + "LOTTER GOLD ERROR");
            }
        }
    }
}
