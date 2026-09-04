#region

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text.Json.Serialization;
using JTGuard.Database.Models;
using JTGuard.Helpers;
using SilkroadSecurityAPI;

#endregion

namespace JTGuard.SessionManager;

public class SessionData : ISessionData
{
    public SessionData()
    {
        State = new State();
        CharTitles = new();
        CharacterTitleColors = new();
        CharacterIcons = new();
        CharacterChest = new();
        CharacterAchievement = new();
        CharacterAchievementCondition = new();
        CharacterNewReverseSavedLocations = new();

    }
    public IState State { get; }
    public int JID { get; set; }
    public int Charid { get; set; }
    public string Charname { get; set; }
    public string MailAddress { get; set; } = string.Empty;
    public uint UniqueCharId { get; set; }
    public string Hwid { get; set; }
    public bool FirstSpawn { get; set; } = true;
    public List<int> CharTitles { get; set; }
    public Dictionary<int, _CharacterTitleManagerColor> CharacterTitleColors { get; set; }
    public Dictionary<int, _CharacterIconManager> CharacterIcons { get; set; }
    public Dictionary<int, _ItemChest> CharacterChest { get; set; }
    public Dictionary<int, _Achievement> CharacterAchievement { get; set; }
    public Dictionary<int, _AchievementCondition> CharacterAchievementCondition { get; set; }
    public Dictionary<int, _NewReverseSavedLocations> CharacterNewReverseSavedLocations { get; set; }

    public string LastAttendedDate { get; set; }
    public int AttendanceDayCount { get; set; }

    public bool TitleManagerPacket { get; set; } = false;
    public bool IconMgrPacket { get; set; } = false;
    public bool UniqueHistoryPacket { get; set; } = false;
    public bool EventRegisterPacket { get; set; } = false;
    public bool EventSchedulePacket { get; set; } = false;
    public bool ChestPacket { get; set; } = false;

    public bool RankCategoryPacket { get; set; } = false;
    public uint CharObjID { get; set; }
    public byte CurLevel { get; set; }
    public byte HwanLevel { get; set; }
    public bool EUChar { get; set; }
    public bool CHChar { get; set; }
    public bool MaleChar { get; set; }
    public bool FemaleChar { get; set; }
    public byte JobType { get; set; }
    public int LatestRegion {  get; set; }
    public int WorldID { get; set; }
    public int WorldLayerID { get; set; }
    public string JobName { get; set; }

    public bool AttendancePacket { get; set; }
    public bool HideCharInformation { get; set; } = false;
    public bool SecondPwRememberPC { get; set; }
    public uint FellowPetUniqueID { get; set; }
    public Int64 FellowPetID64 { get; set; }
    public int FellowItemID { get; set; }
    public DateTime LAST_REVERSE_TIME { get; set; } = new DateTime(2020, 12, 31);
    public DateTime LAST_LOCK_MAIL_TIME { get; set; } = new DateTime(2020, 12, 31);
    public DateTime LAST_UNLOCK_MAIL_TIME { get; set; } = new DateTime(2020, 12, 31);
    public DateTime LAST_STALL_TIME { get; set; } = new DateTime(2020, 12, 31);
    public DateTime LAST_EXCHANGE_TIME { get; set; } = new DateTime(2020, 12, 31);

    public DateTime LAST_GUILD_INVITE_TIME { get; set; } = new DateTime(2020, 12, 31);

    public DateTime LAST_UNION_INVITE_TIME { get; set; } = new DateTime(2020, 12, 31);
    public DateTime LAST_LIVE_ITEM_DELAY { get; set; } = new DateTime(2020, 12, 31);
    public DateTime LAST_GLOBAL_TIME { get; set; } = new DateTime(2020, 12, 31);
    public DateTime LAST_SPAWN_TIME { get; set; } = new DateTime(2020, 12, 31);
    public DateTime LAST_RESTART_TIME { get; set; } = new DateTime(2020, 12, 31);
    public DateTime LAST_EXIT_TIME { get; set; } = new DateTime(2020, 12, 31);

   public DateTime LAST_CHAR_INFO_DELAY { get; set; } = new DateTime(2020, 12, 31);


    public int LockCode { get; set; }
    public int UnLockCode { get; set; }
    // GATEWAY

    public long? LastSnowshieldUsage { get; set; }
    public long? LastSkillUsage { get; set; }
    public long? LastBuffUsage { get; set; }

    public bool OnTransport { get; set; }
    public uint TransportUniqueId { get; set; }
    public int SecondaryPassword { get; set; }
    public byte locale { get; set; }
    public string user_id { get; set; }
    public string user_pw { get; set; }
    public ushort ServerID { get; set; }
}