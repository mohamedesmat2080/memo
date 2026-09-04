using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Dapper;
using JTGuard.Database.Model;
using JTGuard.Database.Models;
using Microsoft.Data.SqlClient;

namespace JTGuard.ServerManagers
{
    public class RefManager
    {
        public static Dictionary<string, string> m_ActiveTitleColors { get; set; } = new Dictionary<string, string>();
        public static Dictionary<string, int> m_LeftCharIcons { get; set; } = new Dictionary<string, int>();
        public static Dictionary<string, int> m_RightCharIcons { get; set; } = new Dictionary<string, int>();
        public static Dictionary<string, byte> m_ActiveTitleNameNew { get; set; } = new Dictionary<string, byte>();


        public static Dictionary<int, string> m_RefIconsMediaPath { get; set; } = new Dictionary<int, string>();

        public static Dictionary<byte, string> m_RefTitleNameNew { get; set; } = new Dictionary<byte, string>();

        public static Dictionary<int, _RefAchievement> m_RefAchievements { get; set; } = new Dictionary<int, _RefAchievement>();

        public static Dictionary<int, _RefAchievementCondition> m_RefAchievementsCondition { get; set; } = new Dictionary<int, _RefAchievementCondition>();
        public static Dictionary<int, _RefHideSkillEffect> m_HideSkillEffects { get; set; } = new Dictionary<int, _RefHideSkillEffect>();
        public static Dictionary<int, _RefMapSettings> m_RefEventMapSettings { get; set; } = new Dictionary<int, _RefMapSettings>();
        public static Dictionary<string, _RefFellowData> m_RefFellowData { get; set; } = new Dictionary<string, _RefFellowData>();

        public static Dictionary<int, _UniqueHistory> UniqueLog { get; set; } = new Dictionary<int, _UniqueHistory>();
        public static List<int> m_RefLoggerMobKill { get; set; } = new List<int>();


        public static Dictionary<int, _RefAttendanceReward> m_AttendanceReward { get; set; } = new Dictionary<int, _RefAttendanceReward>();
        public static Dictionary<int, _RefEventRegister> m_RefEventRegister { get; set; } = new Dictionary<int, _RefEventRegister>();
        public static Dictionary<int, _RefEventSchedule> m_RefEventSchedule = new Dictionary<int, _RefEventSchedule>();
        public static Dictionary<int, string> RefGlobalColor = new Dictionary<int, string>();


        public static Dictionary<int, _RefNewAvatarMall> m_RefNewAvatarMall = new Dictionary<int, _RefNewAvatarMall>();
        public static Dictionary<int, _RefNewItemMall> m_RefNewItemMall = new Dictionary<int, _RefNewItemMall>();

        public static List<int> m_RefFellowPetRefObjID { get; set; } = new List<int>();

        public static Dictionary<int, _RefRankCategories> RankCategories = new();
        public static Dictionary<int, _Rank_Custom1> Rank_Custom1 { get; set; } = new();
        public static Dictionary<int, _Rank_Custom1> Rank_Custom2 { get; set; } = new();
        public static Dictionary<int, _Rank_Custom1> Rank_Custom3 { get; set; } = new();
        public static Dictionary<int, _Rank_Custom1> Rank_Custom4 { get; set; } = new();
        public static Dictionary<int, _Rank_Custom1> Rank_Custom5 { get; set; } = new();
        public static Dictionary<int, _Rank_Custom1> Rank_Custom6 { get; set; } = new();
        public static Dictionary<int, _Rank_Custom1> Rank_Custom7 { get; set; } = new();
        public static Dictionary<int, _Rank_Custom1> Rank_Custom8 { get; set; } = new();
        public static Dictionary<int, _Rank_Custom1> Rank_Custom9 { get; set; } = new();

        private static Timer _timer;

        public static void StartLoadRanksTimer()
        {
            // Her 30 dakikada bir (1800000 milisaniye) LoadRanks metodunu çağır
            _timer = new Timer(async _ => await LoadRanks(), null, 0, 1800000);
        }

        public static async Task Initialize()
        {
            await LoadRefTitleNameNew();
            await LoadRefIconMediaPath();
            await LoadRefAchievements();
            await LoadRefAchievementsCondition();
            await LoadRefHideSkillEffect();
            await LoadRefMapSettings();
            await LoadRefFellowData();
            await LoadRefLoggerMobKill();
            await LoadRefAttendanceReward();
            await LoadRefEventRegister();
            await LoadRefEventSchedule();
            await LoadRefGlobalColor();
            await LoadRefNewAvatarMall();
            await LoadRefNewItemMall();
            await LoadRefFellowPetRefObjID();
            await LoadRefRankCagetorys();
            await LoadRanks();
        }
        public static async Task LoadRanks()
        {
            try
            {
                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();
                    var rank1 = await connection.QueryAsync<_Rank_Custom1>("SELECT *  FROM _Rank_Custom1 with (nolock)");
                    Rank_Custom1.Clear();
                    foreach (var data in rank1)
                    {
                        if (!Rank_Custom1.ContainsKey(data.CharID))
                        {
                            Rank_Custom1.TryAdd(data.CharID, data);
                        }
                    }

                    var rank2 = await connection.QueryAsync<_Rank_Custom1>("SELECT *  FROM _Rank_Custom2 with (nolock)");
                    Rank_Custom2.Clear();
                    foreach (var data in rank1)
                    {
                        if (!Rank_Custom2.ContainsKey(data.CharID))
                        {
                            Rank_Custom2.TryAdd(data.CharID, data);
                        }
                    }
                    var rank3 = await connection.QueryAsync<_Rank_Custom1>("SELECT *  FROM _Rank_Custom3 with (nolock)");
                    Rank_Custom3.Clear();
                    foreach (var data in rank1)
                    {
                        if (!Rank_Custom3.ContainsKey(data.CharID))
                        {
                            Rank_Custom3.TryAdd(data.CharID, data);
                        }
                    }

                    var rank4 = await connection.QueryAsync<_Rank_Custom1>("SELECT *  FROM _Rank_Custom4 with (nolock)");
                    Rank_Custom4.Clear();
                    foreach (var data in rank1)
                    {
                        if (!Rank_Custom4.ContainsKey(data.CharID))
                        {
                            Rank_Custom4.TryAdd(data.CharID, data);
                        }
                    }
                    var rank5 = await connection.QueryAsync<_Rank_Custom1>("SELECT *  FROM _Rank_Custom5 with (nolock)");
                    Rank_Custom5.Clear();
                    foreach (var data in rank1)
                    {
                        if (!Rank_Custom5.ContainsKey(data.CharID))
                        {
                            Rank_Custom5.TryAdd(data.CharID, data);
                        }
                    }
                    var rank6 = await connection.QueryAsync<_Rank_Custom1>("SELECT *  FROM _Rank_Custom6 with (nolock)");
                    Rank_Custom6.Clear();
                    foreach (var data in rank1)
                    {
                        if (!Rank_Custom6.ContainsKey(data.CharID))
                        {
                            Rank_Custom6.TryAdd(data.CharID, data);
                        }
                    }
                    var rank7 = await connection.QueryAsync<_Rank_Custom1>("SELECT *  FROM _Rank_Custom7 with (nolock)");
                    Rank_Custom7.Clear();
                    foreach (var data in rank1)
                    {
                        if (!Rank_Custom7.ContainsKey(data.CharID))
                        {
                            Rank_Custom7.TryAdd(data.CharID, data);
                        }
                    }
                    var rank8 = await connection.QueryAsync<_Rank_Custom1>("SELECT *  FROM _Rank_Custom8 with (nolock)");
                    Rank_Custom8.Clear();
                    foreach (var data in rank1)
                    {
                        if (!Rank_Custom8.ContainsKey(data.CharID))
                        {
                            Rank_Custom8.TryAdd(data.CharID, data);
                        }
                    }
                    var rank9 = await connection.QueryAsync<_Rank_Custom1>("SELECT *  FROM _Rank_Custom9 with (nolock)");
                    Rank_Custom9.Clear();
                    foreach (var data in rank1)
                    {
                        if (!Rank_Custom9.ContainsKey(data.CharID))
                        {
                            Rank_Custom9.TryAdd(data.CharID, data);
                        }
                    }
                }
                //Console.WriteLine($"Ranks Loaded");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"{ex.Message} - LoadCustomRanksFromSQL");
            }
        }
        public static async Task LoadRefRankCagetorys()
        {
            try
            {
                RankCategories.Clear();



                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();


                    string query1 = "SELECT * FROM _RefRankCategories with (nolock) where Active = 1";
                    var result1 = await connection.QueryAsync<_RefRankCategories>(query1);
                    foreach (var item in result1)
                    {
                        if (!RankCategories.ContainsKey(item.ID))
                            RankCategories.TryAdd(item.ID, item);
                    }
                    //Console.WriteLine("_RefRankCategories loaded into cache. Total count: " + RankCategories.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error _RefRankCategories tables: {ex.Message}");
            }
        }
        public static async Task LoadRefFellowPetRefObjID()
        {
            try
            {
                m_RefFellowPetRefObjID.Clear();



                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();


                    string query1 = "SELECT * FROM _RefFellowPetRefObjID with (nolock)";
                    var result1 = await connection.QueryAsync<int>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_RefFellowPetRefObjID.Contains(item))
                            m_RefFellowPetRefObjID.Add(item);
                    }
                    //Console.WriteLine("LoadRefFellowPetRefObjID loaded into cache. Total count: " + m_RefNewItemMall.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadRefFellowPetRefObjID tables: {ex.Message}");
            }
        }

        public static async Task LoadRefNewItemMall()
        {
            try
            {
                m_RefNewItemMall.Clear();



                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();


                    string query1 = "SELECT * FROM _RefNewItemMall with (nolock) where Service = 1";
                    var result1 = await connection.QueryAsync<_RefNewItemMall>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_RefNewItemMall.ContainsKey(item.ID))
                            m_RefNewItemMall.TryAdd(item.ID, item);
                    }
                    //Console.WriteLine("LoadRefNewItemMall loaded into cache. Total count: " + m_RefNewItemMall.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadRefNewItemMall tables: {ex.Message}");
            }
        }
        public static async Task LoadRefNewAvatarMall()
        {
            try
            {
                m_RefNewAvatarMall.Clear();



                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();


                    string query1 = "SELECT * FROM _RefNewAvatarMall with (nolock) where Service = 1";
                    var result1 = await connection.QueryAsync<_RefNewAvatarMall>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_RefNewAvatarMall.ContainsKey(item.ID))
                            m_RefNewAvatarMall.TryAdd(item.ID, item);
                    }
                    //Console.WriteLine("LoadRefNewAvatarMall loaded into cache. Total count: " + m_RefNewAvatarMall.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadRefNewAvatarMall tables: {ex.Message}");
            }
        }

        public static async Task LoadRefGlobalColor()
        {
            try
            {
                RefGlobalColor.Clear();



                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();


                    string query1 = "SELECT GlobalItemID, Color FROM _RefGlobalColor with (nolock)";
                    var result1 = await connection.QueryAsync<(int GlobalItemID, string Color)>(query1);
                    foreach (var item in result1)
                    {
                        if (!RefGlobalColor.ContainsKey(item.GlobalItemID))
                            RefGlobalColor.TryAdd(item.GlobalItemID, item.Color);
                    }
                    //Console.WriteLine("LoadRefTitleNameNew loaded into cache. Total count: " + RefGlobalColor.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadRefTitleNameNew tables: {ex.Message}");
            }
        }
        public static async Task<bool> LoadRefEventSchedule()
        {
            const string querySelectAllRef = "SELECT ID, EventName, Day, Time FROM _RefEventSchedule with (nolock)";
            const string querySelectAllNew = "SELECT EventName, Day, StartTime FROM ____EventSchedule";


            m_RefEventSchedule.Clear();

            int uniqueID = 1;

            
            using (var connection = new SqlConnection(Program.Connectionstring))
            {
                await connection.OpenAsync();

                
                using (var command = new SqlCommand(querySelectAllRef, connection))
                using (var reader = await command.ExecuteReaderAsync())
                {
                    while (await reader.ReadAsync())
                    {
                        var record = new _RefEventSchedule
                        {
                            ID = reader.GetInt32(0),
                            EventName = reader.GetString(1),
                            Day = reader.GetByte(2),
                            Time = reader.GetString(3)
                        };
                        m_RefEventSchedule[record.ID] = record;
                        uniqueID = record.ID + 1;
                    }
                }

                
                using (var command = new SqlCommand(querySelectAllNew, connection))
                using (var reader = await command.ExecuteReaderAsync())
                {
                    while (await reader.ReadAsync())
                    {
                        var record = new _RefEventSchedule
                        {
                            ID = uniqueID++,
                            EventName = reader.GetString(0),
                            Day = reader.GetByte(1),
                            Time = reader.GetString(2)
                        };
                        m_RefEventSchedule[record.ID] = record;
                    }
                }
            }

            //Console.WriteLine($"Loaded {m_RefEventSchedule.Count} RefEventSchedule");
            return true;
        }
        public static async Task LoadRefEventRegister()
        {
            try
            {
                m_RefEventRegister.Clear();
                
                

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    
                    string query1 = "SELECT * FROM _RefEventRegister with (nolock)";
                    var result1 = await connection.QueryAsync<_RefEventRegister>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_RefEventRegister.ContainsKey(item.ID))
                            m_RefEventRegister.TryAdd(item.ID, item);
                    }
                    //Console.WriteLine("LoadRefEventRegister loaded into cache. Total count: " + m_RefEventRegister.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadRefEventRegister tables: {ex.Message}");
            }
        }
        public static async Task LoadRefAttendanceReward()
        {
            try
            {
                m_AttendanceReward.Clear();
                
                

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    
                    string query1 = "SELECT * FROM _RefAttendanceReward with (nolock)";
                    var result1 = await connection.QueryAsync<_RefAttendanceReward>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_AttendanceReward.ContainsKey(item.ID))
                            m_AttendanceReward.TryAdd(item.ID, item);
                    }
                    //Console.WriteLine("LoadRefAttendanceReward loaded into cache. Total count: " + m_AttendanceReward.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadRefAttendanceReward tables: {ex.Message}");
            }
        }
        public static async Task LoadRefLoggerMobKill()
        {
            try
            {
                m_RefLoggerMobKill.Clear();
                
                

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    
                    string query1 = "SELECT RefMobID FROM _RefLoggerMobKill with (nolock)";
                    var result1 = await connection.QueryAsync<int>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_RefLoggerMobKill.Contains(item))
                            m_RefLoggerMobKill.Add(item);
                    }
                    //Console.WriteLine("LoadRefLoggerMobKill loaded into cache. Total count: " + m_RefLoggerMobKill.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadRefLoggerMobKill tables: {ex.Message}");
            }
        }
        public static async Task LoadRefFellowData()
        {
            try
            {
                m_RefFellowData.Clear();
                
                

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    
                    string query1 = "SELECT * FROM _RefFellowPetSystem with (nolock)";
                    var result1 = await connection.QueryAsync<_RefFellowData>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_RefFellowData.ContainsKey(item.PetNameStrID))
                            m_RefFellowData.TryAdd(item.PetNameStrID, item);
                    }
                    //Console.WriteLine("LoadRefFellowData loaded into cache. Total count: " + m_RefFellowData.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadRefFellowData tables: {ex.Message}");
            }
        }
        public static async Task LoadRefMapSettings()
        {
            try
            {
                m_RefEventMapSettings.Clear();
                
                

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    
                    string query1 = "SELECT * FROM _RefMapSettings with (nolock)";
                    var result1 = await connection.QueryAsync<_RefMapSettings>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_RefEventMapSettings.ContainsKey(item.RegionID))
                            m_RefEventMapSettings.TryAdd(item.RegionID, item);
                    }
                    //Console.WriteLine("LoadRefMapSettings loaded into cache. Total count: " + m_RefEventMapSettings.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadRefMapSettings tables: {ex.Message}");
            }
        }
        public static async Task LoadRefHideSkillEffect()
        {
            try
            {
                m_HideSkillEffects.Clear();
                
                

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    
                    string query1 = "SELECT * FROM _RefHideSkillEffect with (nolock)";
                    var result1 = await connection.QueryAsync<_RefHideSkillEffect>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_HideSkillEffects.ContainsKey(item.SkillID))
                            m_HideSkillEffects.TryAdd(item.SkillID, item);
                    }
                    //Console.WriteLine("LoadRefHideSkillEffect loaded into cache. Total count: " + m_HideSkillEffects.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadRefHideSkillEffect tables: {ex.Message}");
            }
        }
        public static async Task LoadRefAchievementsCondition()
        {
            try
            {
                m_RefAchievementsCondition.Clear();
                
                

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    
                    string query1 = "SELECT * FROM _RefAchievementCondition with (nolock)";
                    var result1 = await connection.QueryAsync<_RefAchievementCondition>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_RefAchievementsCondition.ContainsKey(item.ID))
                            m_RefAchievementsCondition.TryAdd(item.ID, item);
                    }
                    //Console.WriteLine("LoadRefAchievementsCondition loaded into cache. Total count: " + m_RefAchievementsCondition.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadRefAchievementsCondition tables: {ex.Message}");
            }
        }
        public static async Task LoadRefAchievements()
        {
            try
            {
                m_RefAchievements.Clear();
                
                

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    
                    string query1 = "SELECT * FROM _RefAchievement with (nolock)";
                    var result1 = await connection.QueryAsync<_RefAchievement>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_RefAchievements.ContainsKey(item.ID))
                            m_RefAchievements.TryAdd(item.ID, item);
                    }
                    //Console.WriteLine("LoadRefAchievements loaded into cache. Total count: " + m_RefAchievements.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadRefAchievements tables: {ex.Message}");
            }
        }
        public static async Task LoadRefTitleNameNew()
        {
            try
            {
                m_RefTitleNameNew.Clear();
                
                

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    
                    string query1 = "SELECT TitleID, TitleName FROM _RefTitleNameNew with (nolock)";
                    var result1 = await connection.QueryAsync<(byte TitleID, string TitleName)>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_RefTitleNameNew.ContainsKey(item.TitleID))
                            m_RefTitleNameNew.TryAdd(item.TitleID, item.TitleName);
                    }
                    //Console.WriteLine("LoadRefTitleNameNew loaded into cache. Total count: " + m_RefTitleNameNew.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadRefTitleNameNew tables: {ex.Message}");
            }
        }
        public static async Task LoadRefIconMediaPath()
        {
            try
            {
                m_RefIconsMediaPath.Clear();
                
                

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    
                    string query1 = "SELECT IconID, MediaPath FROM _RefIconsMediaPath with (nolock)";
                    var result1 = await connection.QueryAsync<(int IconID, string MediaPath)>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_RefIconsMediaPath.ContainsKey(item.IconID))
                            m_RefIconsMediaPath.TryAdd(item.IconID, item.MediaPath);
                    }
                    //Console.WriteLine("Ref Icon Media Patch loaded into cache. Total count: " + m_RefIconsMediaPath.Count);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error LoadRefIconMediaPath tables: {ex.Message}");
            }
        }
        public static async Task LoadSomeTables()
        {
            try
            {
                m_ActiveTitleColors.Clear();
                m_LeftCharIcons.Clear();
                m_ActiveTitleNameNew.Clear();

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    
                    string query1 = "SELECT CharName16, ColorCode FROM _ActiveTitleColors with (nolock)";
                    var result1 = await connection.QueryAsync<(string CharName16, string ColorCode)>(query1);
                    foreach (var item in result1)
                    {
                        if (!m_ActiveTitleColors.ContainsKey(item.CharName16))
                            m_ActiveTitleColors.TryAdd(item.CharName16, item.ColorCode);
                    }
                    //Console.WriteLine("Active Title Colors loaded into cache. Total count: " + m_ActiveTitleColors.Count);


                    string query2 = "SELECT CharName16, IconID FROM _ActiveIconsLeftSide";
                    var result2 = await connection.QueryAsync<(string CharName16, int IconID)>(query2);
                    foreach (var item in result2)
                    {
                        if (!m_LeftCharIcons.ContainsKey(item.CharName16))
                            m_LeftCharIcons.TryAdd(item.CharName16, item.IconID);
                    }
                    //Console.WriteLine("Left Icons loaded into cache. Total count: " + m_LeftCharIcons.Count);


                    string query3 = "SELECT CharName16, IconID FROM _ActiveIconsRightSide";
                    var result3 = await connection.QueryAsync<(string CharName16, int IconID)>(query3);
                    foreach (var item in result3)
                    {
                        if (!m_RightCharIcons.ContainsKey(item.CharName16))
                            m_RightCharIcons.TryAdd(item.CharName16, item.IconID);
                    }
                    //Console.WriteLine("Right Icons loaded into cache. Total count: " + m_RightCharIcons.Count);


                    string query4 = "SELECT CharName16, RefTitleNameNewID FROM _ActiveTitleNameNew";
                    var result4 = await connection.QueryAsync<(string CharName16, byte RefTitleNameNewID)>(query4);
                    foreach (var item in result4)
                    {
                        if (!m_ActiveTitleNameNew.ContainsKey(item.CharName16))
                            m_ActiveTitleNameNew.TryAdd(item.CharName16, item.RefTitleNameNewID);
                    }
                    //Console.WriteLine("New Titles loaded into cache. Total count: " + m_ActiveTitleNameNew.Count);

                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error loading some tables: {ex.Message}");
            }
        }

    }
}
