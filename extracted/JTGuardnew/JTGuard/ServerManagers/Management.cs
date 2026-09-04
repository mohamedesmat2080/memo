using System;
using System.Collections.Generic;
using System.Data;
using System.Data.SqlClient;
using System.Linq;
using System.Threading.Tasks;
using Dapper;
using Microsoft.Data.SqlClient;
using JTGuard.AsyncServerManager;
using JTGuard.Database;
using JTGuard.Database.Model;
using JTGuard.Server;
using System.Net.Mime;
using JTGuard.Helpers;
using JTGuard.Server.DownloadServer;

namespace JTGuard.ServerManagers
{
    public class Management
    {
        public static List<IAsyncServer> Servers { get; set; } = new List<IAsyncServer>();
        public static Dictionary<string, _ServerSettings> UserServers { get; set; } = new Dictionary<string, _ServerSettings>();
        private static List<Notice> NoticesCache { get; set; } = new List<Notice>();

        public static DelayedJobManager g_DelayedJobMgr = new DelayedJobManager();
        public static void AddServer(_ServerSettings service)
        {
            var gatewayServer = new GatewayServer(service);
            Servers.Add(gatewayServer);

            var downloadServer = new DownloadServer(service);
            Servers.Add(downloadServer);

            var agentServer = new AgentServer(service);
            Servers.Add(agentServer);
        }

        private static void TrackServerConnections()
        {
            while (true)
            {
                var gatewayCount = Servers.Where(s => s.SessionServerType == ServerType.GatewayServer).Sum(s => s.GatewaySessions.Count);
                var agentCount = Servers.Where(s => s.SessionServerType == ServerType.AgentServer).Sum(s => s.AgentSessions.Count);
                var downloadCount = Servers.Where(s => s.SessionServerType == ServerType.DownloadServer).Sum(s => s.DownloadSessions.Count);

                Console.Title = $"Gateway Count: {gatewayCount} | Agent Count: {agentCount} | Download Count: {downloadCount}";
                Thread.Sleep(1000); // 1 saniye bekle
            }
        }

        public static void StartServers()
        {
            try
            {
                foreach (var contextService in Servers)
                {
                    if (!contextService.Started)
                    {
                        contextService.Started = true;
                        contextService.Start();
                    }
                }
                var thread = new Thread(TrackServerConnections) { IsBackground = true };
                thread.Start();
            }
            catch (Exception ex)
            {
                Program.PrintInColor($"Error: An unexpected error occurred while starting servers. {ex.Message}", ConsoleColor.Red);
            }
        }
        public static async Task ReloadEventSchedule()
        {
            try
            {
                var agentserver = Servers.Where(s => s.SessionServerType == ServerType.AgentServer).FirstOrDefault();
                if (agentserver != null)
                {
                    await agentserver.eventManager.LoadEventSchedules();
                }
            }
            catch(Exception ex)
            {
                Program.PrintInColor(ex.Message.ToString() + "ReloadEventSchedule ERROR", ConsoleColor.Red); 
            }
        }
        public static async Task ReloadServerSettings()
        {
            try
            {
                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    var query = "SELECT SettingName, Value FROM __ServerSettings WITH (NOLOCK)";
                    var settings = await connection.QueryAsync<(string SettingName, string Value)>(query);

                    if (settings != null)
                    {
                        var properties = typeof(_ServerSettings).GetProperties();

                        foreach (var setting in settings)
                        {
                            var property = properties.FirstOrDefault(p => p.Name.Equals(setting.SettingName, StringComparison.OrdinalIgnoreCase));

                            if (property != null)
                            {
                                bool successfulUpdate = false;
                                foreach (var server in Servers)
                                {
                                    var serverSettings = server.Service;
                                    try
                                    {
                                        if (property.PropertyType == typeof(int))
                                        {
                                            property.SetValue(serverSettings, int.Parse(setting.Value));
                                        }
                                        else if (property.PropertyType == typeof(bool))
                                        {
                                            if (bool.TryParse(setting.Value, out bool boolValue))
                                            {
                                                property.SetValue(serverSettings, boolValue);
                                            }
                                            else
                                            {
                                                Program.PrintInColor($"Invalid value for {setting.SettingName}: {setting.Value}", ConsoleColor.Red);
                                            }
                                        }
                                        else if (property.PropertyType == typeof(string))
                                        {
                                            property.SetValue(serverSettings, setting.Value);
                                        }
                                        else if (property.PropertyType == typeof(byte))
                                        {
                                            property.SetValue(serverSettings, byte.Parse(setting.Value));
                                        }
                                        else if (property.PropertyType == typeof(long))
                                        {
                                            property.SetValue(serverSettings, long.Parse(setting.Value));
                                        }
                                        else if (property.PropertyType == typeof(decimal))
                                        {
                                            property.SetValue(serverSettings, decimal.Parse(setting.Value));
                                        }
                                        else
                                        {
                                            Program.PrintInColor($"Unsupported property type: {property.PropertyType}", ConsoleColor.Red);
                                        }

                                        successfulUpdate = true;
                                    }
                                    catch (Exception ex)
                                    {
                                        Program.PrintInColor($"Error setting value for {setting.SettingName}: {ex.Message}", ConsoleColor.Red);
                                    }
                                }

                                if (successfulUpdate)
                                {
                                    Program.PrintInColor($"{setting.SettingName} => {setting.Value}", ConsoleColor.Yellow);
                                }
                            }
                            else
                            {
                                Program.PrintInColor($"Unknown Setting: {setting.SettingName} => {setting.Value}", ConsoleColor.Yellow);
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Program.PrintInColor($"Error: An unexpected error occurred while reloading server settings. {ex.Message}", ConsoleColor.Red);
            }
        }



        public static async Task CreateServerAsync()
        {
            try
            {
                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    var query = "SELECT SettingName, Value FROM __ServerSettings WITH (NOLOCK)";
                    var settings = await connection.QueryAsync<(string SettingName, string Value)>(query);

                    if (settings != null)
                    {
                        var userServerSetting = new _ServerSettings();
                        var properties = typeof(_ServerSettings).GetProperties();

                        foreach (var setting in settings)
                        {
                            var property = properties.FirstOrDefault(p => p.Name.Equals(setting.SettingName, StringComparison.OrdinalIgnoreCase));
                            if (property != null)
                            {
                                try
                                {
                                    if (property.PropertyType == typeof(int))
                                    {
                                        property.SetValue(userServerSetting, int.Parse(setting.Value));
                                    }
                                    else if (property.PropertyType == typeof(bool))
                                    {
                                        if (bool.TryParse(setting.Value, out bool boolValue))
                                        {
                                            property.SetValue(userServerSetting, boolValue);
                                        }
                                        else
                                        {
                                            Program.PrintInColor($"Invalid value for {setting.SettingName}: {setting.Value}", ConsoleColor.Red);
                                        }
                                    }
                                    else if (property.PropertyType == typeof(string))
                                    {
                                        property.SetValue(userServerSetting, setting.Value);
                                    }
                                    else if (property.PropertyType == typeof(byte))
                                    {
                                        property.SetValue(userServerSetting, byte.Parse(setting.Value));
                                    }
                                    else if (property.PropertyType == typeof(long))
                                    {
                                        property.SetValue(userServerSetting, long.Parse(setting.Value));
                                    }
                                    else if (property.PropertyType == typeof(decimal))
                                    {
                                        property.SetValue(userServerSetting, decimal.Parse(setting.Value));
                                    }
                                    else
                                    {
                                        Program.PrintInColor($"Unsupported property type: {property.PropertyType}", ConsoleColor.Red);
                                    }
                                }
                                catch (Exception ex)
                                {
                                    Program.PrintInColor($"Error setting value for {setting.SettingName}: {ex.Message}", ConsoleColor.Red);
                                }

                                Program.PrintInColor($"{setting.SettingName} => {setting.Value}", ConsoleColor.Yellow);
                            }
                            else
                            {
                                Program.PrintInColor($"Unknown Setting: {setting.SettingName} => {setting.Value}", ConsoleColor.Yellow);
                            }
                        }
                        await LoadNoticesIntoCache();
                        await RankManager.Initialize();
                        await RefManager.Initialize();
                        await RefManager.LoadSomeTables();
                        RefManager.StartLoadRanksTimer();
                        //await EventManager.Initialize();
                        AddServer(userServerSetting);
                        StartServers();
                        g_DelayedJobMgr.Run();
                    }
                }
            }
            catch (Exception ex)
            {
                Program.PrintInColor($"Error: An unexpected error occurred while creating server. {ex.Message}", ConsoleColor.Red);
            }
        }



        public static async Task LoadNoticesIntoCache()
        {
            try
            {
                NoticesCache.Clear();
                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();
                    string query = "SELECT Name, String FROM __Notices";
                    var result = await connection.QueryAsync<Notice>(query);
                    NoticesCache = result.ToList();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error loading notices into cache: {ex.Message}");
            }
        }

        public static string GetNoticeMessage(string name)
        {
            try
            {
                if (NoticesCache == null || !NoticesCache.Any())
                {
                    Console.WriteLine("Notices cache is empty. Make sure to load notices first.");
                    return string.Empty; // Cache boşsa boş string döndür
                }

                var notice = NoticesCache.FirstOrDefault(n => n.Name == name);
                return notice?.String ?? string.Empty; // Notice bulunmazsa boş string döndür
            }
            catch (Exception ex)
            {
                Console.WriteLine($"An error occurred in GetNoticeMessage: {ex.Message}");
                return string.Empty; // Hata durumunda boş string döndür
            }
        }

    }

    public class Notice
    {
        public string Name { get; set; }
        public string String { get; set; }
    }
}

