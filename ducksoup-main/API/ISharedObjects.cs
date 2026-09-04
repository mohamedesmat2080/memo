using API.Database.DuckSoup;
using API.Database.SRO_VT_ACCOUNT;
using API.Database.SRO_VT_SHARD;
using API.Model;
using API.Session;

namespace API;

public interface ISharedObjects : IDisposable
{
    Dictionary<int, C_RefObjCommon> RefObjCommon { get; }
    Dictionary<int, C_RefSkill> RefSkill { get; }
    Dictionary<int, C_Notice> Notice { get; }
    Dictionary<string, ItemInfo> ItemLinkInfo { get; set; }
    Dictionary<int, StallNetwork> StallNetwork { get; set; }
    Dictionary<int, RegionInfo> RegionInfo { get;}
    Dictionary<int, CharRanking> CharRanking { get; set; }
    List<DefaultUniques> DefaultUniques { get; }
    Dictionary<int, UniqueLog> UniqueLog { get; set; }
    List<API.Database.DuckSoup.Event> Events { get; }
    Dictionary<string, CustomIcon> CustomIcon { get; set; }
    Dictionary<string, CustomIconData> CustomIconData { get; set; }
    Dictionary<string, CustomIconVip> CustomIconVip { get; set; }
    Dictionary<string, CustomIconVipData> CustomIconVipData { get; set; }
    Dictionary<string, CustomName> CustomName { get; set; }
    Dictionary<string, CustomNameColor> CustomNameColor { get; set; }
    Dictionary<string, CustomTitle> CustomTitle { get; set; }
    Dictionary<string, CustomTitleColor> CustomTitleColor { get; set; }
    List<Achievements> Achievements { get; set; }
    List<DailyReward> DailyReward { get; set; }
    Dictionary<string, AccountLock> AccountLock { get; set; }


    HashSet<ISession> DownloadSessions { get; }
    HashSet<ISession> GatewaySessions { get; }
    HashSet<ISession> AgentSessions { get; }
}