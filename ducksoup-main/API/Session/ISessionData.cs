#region

#endregion

using API.Database.DuckSoup;

namespace API.Session
{
    public interface ISessionData : IDisposable
    {
        long? LastSnowshieldUsage { get; set; }
        int? JID { get; set; }
        int? Charid { get; set; }
        string Charname { get; set; }
        uint UniqueCharId { get; set; }
        int? LatestRegionId { get; set; }
        int? SectorX { get; set; }
        int? SectorY { get; set; }
        float? PositionX { get; set; }
        float? PositionY { get; set; }
        float? PositionZ { get; set; }
        PvpState? PvpState { get; set; }
        bool? InCombat { get; set; }
        Scrolling? ScrollingType { get; set; }
        GameState? GameStateType { get; set; }
        Job? JobType { get; set; }
        uint? PartyMatchingId { get; set; }
        public object this[string field] { get; set; }
        uint lastfleaTID { get; set; }
        byte lastfleaPageID { get; set; }
        int lastfleaitemDG { get; set; }
        bool fleablockStall { get; set; }
        bool stallNetwork { get; set; }
        List<ItemChest> chestItems { get; set; }
        List<String> ownTitles { get; set; }
        string Username { get; set; }
    }
}