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
using JTGuard.PacketHandlerManager;
using JTGuard.Server;
using Microsoft.Data.SqlClient;
using SilkroadSecurityAPI;
using static System.Runtime.InteropServices.JavaScript.JSType;
using static JTGuard.ServerManagers.Events.StyriaClash;

namespace JTGuard.ServerManagers.Events
{
    public partial class EventManager
    {
        private AgentServer AgentServer { get; set; }
        public _ServerSettings Service { get; set; }

        public Dictionary<int, bool> LiveTeleportManagement = new Dictionary<int, bool>();
        public Dictionary<int, bool> LiveSkillManagementbyRegionID = new Dictionary<int, bool>();
        public Dictionary<int, bool> LiveSkillManagementbyWorldID = new Dictionary<int, bool>();

        public ConcurrentDictionary<int, int> CreatedTimerListWorldID = new ConcurrentDictionary<int, int>();
        public ConcurrentDictionary<int, int> CreatedTimerListRegionID = new ConcurrentDictionary<int, int>();


        public ConcurrentDictionary<int, string> CreatedKillCounterWorldID = new ConcurrentDictionary<int, string>();
        public ConcurrentDictionary<string, SCreatedKillCounterKillList> KillCounterKillList = new ConcurrentDictionary<string, SCreatedKillCounterKillList>();

        public ConcurrentDictionary<int, string> CreatedTeamKillCounterWorldID = new ConcurrentDictionary<int, string>();
        public ConcurrentDictionary<string, SCreatedTeamKillCounterKillList> TeamKillCounterKillList = new ConcurrentDictionary<string, SCreatedTeamKillCounterKillList>();

        public ConcurrentDictionary<int, string> CreatedJobKillCounterWorldID = new ConcurrentDictionary<int, string>();
        public ConcurrentDictionary<string, SCreatedJobKillCounterKillList> JobKillCounterKillList = new ConcurrentDictionary<string, SCreatedJobKillCounterKillList>();

        public ConcurrentDictionary<string, SFortressWarCounter> FtwKillCounterList = new ConcurrentDictionary<string, SFortressWarCounter>();


        public ConcurrentDictionary<TimerWorldandPlayer, int> CreatedTimerListWorldIDAndLayerID = new ConcurrentDictionary<TimerWorldandPlayer, int>();


        public List<____EventSchedule> EventSchedules = new();
        public StyriaClash styriaClash { get; set; }
        public SurvivalArena survivalArena { get; set; }
        public LastManStanding lastManStanding { get; set; }
        
        public DefendTower defendTower { get; set; }
        public LotterySilk lotterySilk { get; set; }
        public LotteryGold lotteryGold { get; set; }
        public LuckyPartyNumber luckyptnumber { get; set; }
        public EventManager(_ServerSettings service, AgentServer agentServer)
        {
            AgentServer = agentServer;
            Service = service;
            styriaClash = new StyriaClash(service, AgentServer, this);
            survivalArena = new SurvivalArena(service, AgentServer, this);
            lastManStanding = new LastManStanding(service, AgentServer, this);
            defendTower = new DefendTower(service, AgentServer, this);
            lotterySilk = new LotterySilk(service, AgentServer, this);
            lotteryGold = new LotteryGold(service, AgentServer, this);
            luckyptnumber = new LuckyPartyNumber(service, AgentServer, this);
            LoadEventSchedules();

            StartEventManager();
        }
        public async Task LoadEventSchedules()
        {
            try
            {
                EventSchedules.Clear();



                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();


                    string query1 = "SELECT * FROM ____EventSchedule with (nolock)";
                    var result1 = await connection.QueryAsync<____EventSchedule>(query1);
                    foreach (var item in result1)
                    {
                        if (!EventSchedules.Contains(item))
                            EventSchedules.Add(item);
                    }
                    //Console.WriteLine("LoadEventSchedules loaded into cache. Total count: " + EventSchedules.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadEventSchedules tables: {ex.Message}");
            }
        }

        public void StartEventManager()
        {
            Task.Run(async () =>
            {
                while (true)
                {
                    try
                    {
                        await CheckEvents();
                        await Task.Delay(TimeSpan.FromSeconds(30)); // 30 saniyede bir kontrol
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine($"Exception: {e.Message}");
                    }
                }
            });
        }
        private async Task CheckEvents()
        {
            var now = DateTime.Now;
            var daysOfWeek = (int)now.DayOfWeek == 0 ? 7 : (int)now.DayOfWeek; // Pazarı 7 olarak alır, 1 pazartesi olarak devam eder
            var currentHour = now.ToString("HH:mm");

            if (EventSchedules.Count > 0)
            {
                foreach (var data in EventSchedules)
                {
                    if (data.Day == daysOfWeek || data.Day == 8)
                    {
                        if (data.StartTime == currentHour)
                        {
                            switch (data.RefEventID)
                            {
                                case 1:
                                    survivalArena.StartSurvivalArena(data.DurationMin, data.RegisterMin);
                                    break;
                                case 2:
                                    lastManStanding.StartLMS(data.DurationMin, data.RegisterMin);
                                    break;
                                case 3:
                                    defendTower.StartDefendTower(data.DurationMin, data.RegisterMin);
                                    break;
                                case 4:
                                    styriaClash.StartStyriaClash(data.DurationMin, data.RegisterMin);
                                    break;
                                case 5:
                                    lotterySilk.StartLotterySilk(data.DurationMin, data.RegisterMin);
                                    break;
                                case 6:
                                    lotteryGold.StartLotteryGold(data.DurationMin, data.RegisterMin);
                                    break;
                                case 10:
                                    luckyptnumber.StartLuckyPartyNumber(data.DurationMin);
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                }
            }
        }
    }
}

