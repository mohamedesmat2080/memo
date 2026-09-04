using System.Collections.Generic;
using System.Data.Entity.Migrations;
using System.Linq;
using System.Data.Entity;
using API;
using API.Database;
using API.Database.DuckSoup;
using API.Database.SRO_VT_ACCOUNT;
using API.Database.SRO_VT_SHARD;
using API.Model;
using API.ServiceFactory;
using API.Session;
using PacketLibrary;
using System;

namespace DuckSoup.Library;

public class SharedObjects : ISharedObjects
{
    
    public static Dictionary<string, dynamic> settings;
    public SharedObjects()
    {
        ServiceFactory.Register<ISharedObjects>(typeof(ISharedObjects), this);
        //todo register settings professionally
        settings = new Dictionary<string, dynamic>();

        settings.Add("Name", DatabaseHelper.GetSettingOrDefault("Name", "Filter"));
        settings.Add("DebugLevel", (DebugLevel)int.Parse(DatabaseHelper.GetSettingOrDefault("DebugLevel", ((byte)DebugLevel.Info).ToString())));

        settings.Add("IPLimit", Convert.ToInt32(DatabaseHelper.GetSettingOrDefault("IPLimit", "0")));
        settings.Add("IPLimit_Notice", DatabaseHelper.GetSettingOrDefault("IPLimit_Notice", "You have reached maximum allowed accounts to be opened per one ip."));
        settings.Add("PCLimit", Convert.ToInt32(DatabaseHelper.GetSettingOrDefault("PCLimit", "0")));
        settings.Add("PCLimit_Notice", DatabaseHelper.GetSettingOrDefault("PCLimit_Notice", "You have reached maximum allowed accounts to be opened per one pc."));

        settings.Add("MaxPlus", Convert.ToInt32(DatabaseHelper.GetSettingOrDefault("MaxPlus", "0")));
        settings.Add("MaxPlus_Notice", DatabaseHelper.GetSettingOrDefault("MaxPlus_Notice", "You cannot fuse this item since it has reached the plus limit."));
        settings.Add("MaxPlusDevil", Convert.ToInt32(DatabaseHelper.GetSettingOrDefault("MaxPlusDevil", "0")));
        settings.Add("MaxPlusDevil_Notice", DatabaseHelper.GetSettingOrDefault("MaxPlusDevil_Notice", "You cannot fuse this item since it has reached the devil plus limit."));

        settings.Add("ShardName", DatabaseHelper.GetSettingOrDefault("ShardName", "DuckSoup"));
        settings.Add("ShowOnlinePlayers", Convert.ToBoolean(DatabaseHelper.GetSettingOrDefault("ShowOnlinePlayers", "true")));
        settings.Add("FakeCount", Convert.ToInt32(DatabaseHelper.GetSettingOrDefault("FakeCount", "0")));
        settings.Add("RemoveCaptcha", Convert.ToBoolean(DatabaseHelper.GetSettingOrDefault("RemoveCaptcha", "false")));
        settings.Add("ReplacementCaptcha", DatabaseHelper.GetSettingOrDefault("ReplacementCaptcha", "0"));

        AgentSessions = new HashSet<ISession>();
        DownloadSessions = new HashSet<ISession>();
        GatewaySessions = new HashSet<ISession>();

        Notice = new Dictionary<int, C_Notice>();
        RefObjCommon = new Dictionary<int, C_RefObjCommon>();
        RefSkill = new Dictionary<int, C_RefSkill>();
        ItemLinkInfo = new Dictionary<string, ItemInfo>();
        StallNetwork = new Dictionary<int, StallNetwork>();
        RegionInfo = new Dictionary<int, RegionInfo>();
        CharRanking = new Dictionary<int, CharRanking>();
        DefaultUniques = new List<DefaultUniques>();
        UniqueLog = new Dictionary<int, UniqueLog>();
        Events = new List<API.Database.DuckSoup.Event>();
        CustomIcon = new Dictionary<string, CustomIcon>();
        CustomIconData = new Dictionary<string, CustomIconData>();
        CustomIconVip = new Dictionary<string, CustomIconVip>();
        CustomIconVipData = new Dictionary<string, CustomIconVipData>();
        CustomName = new Dictionary<string, CustomName>();
        CustomNameColor = new Dictionary<string, CustomNameColor>();
        CustomTitle = new Dictionary<string, CustomTitle>();
        CustomTitleColor = new Dictionary<string, CustomTitleColor>();
        Achievements = new List<Achievements>();
        AccountLock = new Dictionary<string, AccountLock>();

        using (var context = new API.Database.DuckSoup.DuckSoup())
        {
            if (context.Whitelist.ToList().Count == 0)
            {
                foreach (var msgId in DefaultPacketlist.DownloadClientWhitelist)
                {
                    if(DefaultPacketlist.DownloadClientWhitelistFull.ContainsKey(msgId))
                        context.Whitelist.AddOrUpdate(new Whitelist()
                        {
                            MsgId = msgId, Comment = DefaultPacketlist.DownloadClientWhitelistFull[msgId],
                            ServerType = ServerType.DownloadServer
                        });
                }
                foreach (var msgId in DefaultPacketlist.GatewayClientWhitelist)
                {
                if (DefaultPacketlist.GatewayClientWhitelistFull.ContainsKey(msgId))
                    context.Whitelist.AddOrUpdate(new Whitelist()
                    {
                        MsgId = msgId, Comment = DefaultPacketlist.GatewayClientWhitelistFull[msgId],
                        ServerType = ServerType.GatewayServer
                    });
                }
                foreach (var msgId in DefaultPacketlist.AgentClientWhitelist)
                {
                if (DefaultPacketlist.AgentClientWhitelistFull.ContainsKey(msgId))
                    context.Whitelist.AddOrUpdate(new Whitelist()
                    {
                        MsgId = msgId, Comment = DefaultPacketlist.AgentClientWhitelistFull[msgId],
                        ServerType = ServerType.AgentServer
                    });
                }

                context.SaveChanges();
            }
        }

        LoadItems();
        LoadSkills();
        LoadNotices();
        LoadRegions();
        LoadCharRank();
        LoadDefaultUniques();
        LoadEvents();
        LoadCustomIcon();
        LoadCustomIconData();
        LoadCustomIconVip();
        LoadCustomIconVipData();
        LoadCustomName();
        LoadCustomNameColor();
        LoadCustomTitle();
        LoadCustomTitleColor();
        LoadAcheivements();
        LoadDailyReward();
        LoadAccountLock();
    }

    public HashSet<ISession> AgentSessions { get; private set; }
    public HashSet<ISession> DownloadSessions { get; private set; }
    public HashSet<ISession> GatewaySessions { get; private set; }
    public Dictionary<int, C_Notice> Notice { get; private set; }
    public List<API.Database.DuckSoup.Event> Events { get; private set; }
    public Dictionary<int, C_RefObjCommon> RefObjCommon { get; private set; }
    public Dictionary<int, C_RefSkill> RefSkill { get; private set; }
    public Dictionary<string, ItemInfo> ItemLinkInfo { get; set; }
    public Dictionary<int, StallNetwork> StallNetwork { get; set; }
    public Dictionary<int, RegionInfo> RegionInfo { get; private set; }
    public Dictionary<int, CharRanking> CharRanking { get; set; }
    public List<DefaultUniques> DefaultUniques { get; private set; }
    public Dictionary<int, UniqueLog> UniqueLog { get; set; }
    public Dictionary<string, CustomIcon> CustomIcon { get; set; }
    public Dictionary<string, CustomIconData> CustomIconData { get; set; }
    public Dictionary<string, CustomIconVip> CustomIconVip { get; set; }
    public Dictionary<string, CustomIconVipData> CustomIconVipData { get; set; }
    public Dictionary<string, CustomName> CustomName { get; set; }
    public Dictionary<string, CustomNameColor> CustomNameColor { get; set; }
    public Dictionary<string, CustomTitle> CustomTitle { get; set; }
    public Dictionary<string, CustomTitleColor> CustomTitleColor { get; set; }
    public List<Achievements> Achievements { get; set; }
    public List<DailyReward> DailyReward { get; set; }
    public Dictionary<string, AccountLock> AccountLock { get; set; }

    public void Dispose()
    {
        AgentSessions = null;
        DownloadSessions = null;
        GatewaySessions = null;

        Notice = null;
        RefObjCommon = null;
        RefSkill = null;
    }

    private void LoadItems()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read RefObjCommon. This might take a while!", settings["Name"]);
        }

        using (var db = new SRO_VT_SHARD())
        {
            RefObjCommon = db.C_RefObjCommon.ToDictionary(x => x.ID);
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;

        var count = RefObjCommon.Count;
        Global.Logger.InfoFormat("{0} - {1} Items loaded.", settings["Name"], count);
    }

    private void LoadSkills()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read RefSkill. This might take a while!", settings["Name"]);
        }

        using (var db = new SRO_VT_SHARD())
        {
            RefSkill = db.C_RefSkill.ToDictionary(x => x.ID);
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;

        var count = RefSkill.Count;
        Global.Logger.InfoFormat("{0} - {1} Skills loaded.", settings["Name"], count);
    }

    private void LoadNotices()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read Notice. This might take a while!", settings["Name"]);
        }

        using (var db = new SRO_VT_ACCOUNT())
        {
            Notice = db.C_Notice.ToDictionary(x => x.ID);
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
        Global.Logger.InfoFormat("{0} - {1} Notices loaded.", settings["Name"], Notice.Count);
    }

    private void LoadRegions()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read Regions. This might take a while!", settings["Name"]);
        }
        using (var db = new API.Database.DuckSoup.DuckSoup())
        {
            RegionInfo = db.RegionInfo.ToDictionary(x => x.RegionID);
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
            Global.Logger.InfoFormat("{0} - {1} Regions loaded.", settings["Name"], RegionInfo.Count);
    }
    private void LoadCharRank()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read Char Rank. This might take a while!", settings["Name"]);
        }
        using (var db = new API.Database.DuckSoup.DuckSoup())
        {
            CharRanking = db.CharRanking.Include(x=>x.CharRankingRecords).ToDictionary(t => t.ID);
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
            Global.Logger.InfoFormat("{0} - {1} Char Rank loaded.", settings["Name"], CharRanking.Count);
    }
    private void LoadDefaultUniques()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read Default Uniques. This might take a while!", settings["Name"]);
        }
        using (var db = new API.Database.DuckSoup.DuckSoup())
        {
            DefaultUniques = db.DefaultUniques.ToList();
        }
        foreach(var item in DefaultUniques)
        {
            UniqueLog uLog = new UniqueLog
            {
                refObjID = item.refObjID,
                Killer = "<No Killer>",
                State = true,
                Date = System.DateTime.Now
            };
            if(!UniqueLog.ContainsKey(item.refObjID))
                UniqueLog.Add(item.refObjID, uLog);
        }
        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
            Global.Logger.InfoFormat("{0} - {1} Uniques loaded.", settings["Name"], CharRanking.Count);
    }
    private void LoadEvents()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read Events. This might take a while!", settings["Name"]);
        }
        using (var db = new API.Database.DuckSoup.DuckSoup())
        {
            Events = db.Event.ToList();
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
            Global.Logger.InfoFormat("{0} - {1} Events loaded.", settings["Name"], CharRanking.Count);
    }
    private void LoadCustomIcon()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read Custom Icon. This might take a while!", settings["Name"]);
        }
        using (var db = new API.Database.DuckSoup.DuckSoup())
        {
            CustomIcon = db.CustomIcon.ToDictionary(x => x.Charname);
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
            Global.Logger.InfoFormat("{0} - {1} Custom Icon loaded.", settings["Name"], CustomIcon.Count);
    }
    private void LoadCustomIconData()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read Custom Icon Data. This might take a while!", settings["Name"]);
        }
        using (var db = new API.Database.DuckSoup.DuckSoup())
        {
            CustomIconData = db.CustomIconData.ToDictionary(x => x.path);
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
            Global.Logger.InfoFormat("{0} - {1} Custom Icon Data loaded.", settings["Name"], CustomIconData.Count);
    }
    private void LoadCustomIconVip()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read Custom Icon Vip. This might take a while!", settings["Name"]);
        }
        using (var db = new API.Database.DuckSoup.DuckSoup())
        {
            CustomIconVip = db.CustomIconVip.ToDictionary(x => x.Charname);
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
        Global.Logger.InfoFormat("{0} - {1} Custom Icon Vip loaded.", settings["Name"], CustomIconVip.Count);
    }
    private void LoadCustomIconVipData()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read Custom Icon Data Vip. This might take a while!", settings["Name"]);
        }
        using (var db = new API.Database.DuckSoup.DuckSoup())
        {
            CustomIconVipData = db.CustomIconVipData.ToDictionary(x => x.path);
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
            Global.Logger.InfoFormat("{0} - {1} Custom Icon Data Vip loaded.", settings["Name"], CustomIconVipData.Count);
    }
    private void LoadCustomName()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read CustomName. This might take a while!", settings["Name"]);
        }
        using (var db = new API.Database.DuckSoup.DuckSoup())
        {
            CustomName = db.CustomName.ToDictionary(x => x.Charname);
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
        Global.Logger.InfoFormat("{0} - {1} CustomName loaded.", settings["Name"], CustomName.Count);
    }

    private void LoadCustomNameColor()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read CustomNameColor. This might take a while!", settings["Name"]);
        }
        using (var db = new API.Database.DuckSoup.DuckSoup())
        {
            CustomNameColor = db.CustomNameColor.ToDictionary(x => x.Charname);
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
        Global.Logger.InfoFormat("{0} - {1} CustomNameColor loaded.", settings["Name"], CustomNameColor.Count);
    }
    private void LoadCustomTitle()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read CustomTitle. This might take a while!", settings["Name"]);
        }
        using (var db = new API.Database.DuckSoup.DuckSoup())
        {
            CustomTitle = db.CustomTitle.ToDictionary(x => x.CharName);
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
        Global.Logger.InfoFormat("{0} - {1} CustomTitle loaded.", settings["Name"], CustomTitle.Count);
    }
    private void LoadCustomTitleColor()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read CustomTitleColor. This might take a while!", settings["Name"]);
        }
        using (var db = new API.Database.DuckSoup.DuckSoup())
        {
            CustomTitleColor = db.CustomTitleColor.ToDictionary(x => x.Charname);
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
        Global.Logger.InfoFormat("{0} - {1} CustomTitleColor loaded.", settings["Name"], CustomTitleColor.Count);
    }
    private void LoadAcheivements()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read LoadAcheivements. This might take a while!", settings["Name"]);
        }
        using (var db = new API.Database.DuckSoup.DuckSoup())
        {
            Achievements = db.Achievements.Include(x => x.AchievementsRecords).ToList();
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
        Global.Logger.InfoFormat("{0} - {1} LoadAcheivements loaded.", settings["Name"], Achievements.Count);
    }
    private void LoadDailyReward()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read LoadDailyReward. This might take a while!", settings["Name"]);
        }
        using (var db = new API.Database.DuckSoup.DuckSoup())
        {
            DailyReward = db.DaiyReward.Include(x => x.DailyRewardRecords).ToList();
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
        Global.Logger.InfoFormat("{0} - {1} LoadDailyReward loaded.", settings["Name"], DailyReward.Count);
    }
    private void LoadAccountLock()
    {
        if ( settings["DebugLevel"] >=  DebugLevel.Info)
        {
            Global.Logger.InfoFormat("{0} - Starting to read LoadAccountLock. This might take a while!", settings["Name"]);
        }
        using (var db = new API.Database.DuckSoup.DuckSoup())
        {
            AccountLock = db.AccountLock.Include(x => x.AccountLockRecords).ToDictionary(x=>x.Username);
        }

        if ( settings["DebugLevel"] <  DebugLevel.Info) return;
        Global.Logger.InfoFormat("{0} - {1} LoadAccountLock loaded.", settings["Name"], AccountLock.Count);
    }
}