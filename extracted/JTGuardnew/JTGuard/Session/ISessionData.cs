using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using JTGuard.Database.Models;
using JTGuard.Helpers;

namespace JTGuard.SessionManager;
public interface ISessionData
{
    IState State { get; }
    int JID { get; set; }
    int Charid { get; set; }
    string Charname { get; set; }
    string MailAddress { get; set; }
    uint UniqueCharId { get; set; }
    string Hwid { get; set; }
    bool FirstSpawn { get; set; }
    List<int> CharTitles { get; set; }
    Dictionary<int, _CharacterTitleManagerColor> CharacterTitleColors { get; set; }
    Dictionary<int, _CharacterIconManager> CharacterIcons { get; set; }
    Dictionary<int, _ItemChest> CharacterChest { get; set; }
    Dictionary<int, _Achievement> CharacterAchievement { get; set; }
    Dictionary<int, _AchievementCondition> CharacterAchievementCondition { get; set; }
    Dictionary<int, _NewReverseSavedLocations> CharacterNewReverseSavedLocations { get; set; }
    string LastAttendedDate { get; set; }
    int AttendanceDayCount { get; set; }

    bool TitleManagerPacket { get; set; }
    bool IconMgrPacket { get; set; }
    bool UniqueHistoryPacket { get; set; }
    bool EventRegisterPacket { get; set; }

    bool EventSchedulePacket { get; set; }
    bool ChestPacket { get; set; }
    bool RankCategoryPacket { get; set; }
    uint CharObjID { get; set; }
    byte CurLevel { get; set; }

    byte HwanLevel { get; set; }
    bool EUChar { get; set; }
    bool CHChar { get; set; }
    bool MaleChar { get; set; }
    bool FemaleChar { get; set; }

    byte JobType { get; set; }
    int LatestRegion { get; set; }
    int WorldID { get; set; }
    int WorldLayerID { get; set; }
    string JobName { get; set; }

    bool AttendancePacket { get; set; }
    bool HideCharInformation { get; set; }
    bool SecondPwRememberPC { get; set; }
    uint FellowPetUniqueID { get; set; }
    Int64 FellowPetID64 { get; set; }
    int FellowItemID { get; set; }
    DateTime LAST_REVERSE_TIME { get; set; }
    DateTime LAST_LOCK_MAIL_TIME { get; set; }

    DateTime LAST_UNLOCK_MAIL_TIME { get; set; }
    DateTime LAST_STALL_TIME { get; set; }
    DateTime LAST_EXCHANGE_TIME { get; set; }
    DateTime LAST_GUILD_INVITE_TIME { get; set; }
    DateTime LAST_UNION_INVITE_TIME { get; set; }
    DateTime LAST_LIVE_ITEM_DELAY { get; set; }
    DateTime LAST_GLOBAL_TIME { get; set; }
    DateTime LAST_SPAWN_TIME { get; set; }
    DateTime LAST_RESTART_TIME { get; set; }
    DateTime LAST_EXIT_TIME { get; set; }
    DateTime LAST_CHAR_INFO_DELAY { get; set; }

    long? LastBuffUsage { get; set; }
    long? LastSnowshieldUsage { get; set; }
    long? LastSkillUsage { get; set; }

    bool OnTransport { get; set; }
    uint TransportUniqueId { get; set; }


    int LockCode { get; set; }
    int UnLockCode { get; set; }
    int SecondaryPassword { get; set; }
    //// NEWS
    byte locale { get; set; }
    string user_id { get; set; }
    string user_pw { get; set; }
    ushort ServerID { get; set; }
}