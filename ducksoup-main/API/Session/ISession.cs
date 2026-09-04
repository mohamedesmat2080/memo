using API.Model;
using API.Server;
using SilkroadSecurityAPI;

namespace API.Session;

public interface ISession : IDisposable
{
    IAsyncServer AsyncServer { get; init; }
    int ClientId { get; set; }
    string ClientIp { get; set; }

    Task SendToClient(Packet packet);
    Task SendToClient(List<Packet> packets);
    Task SendToServer(Packet packet);
    Task SendNotice(string message);
    Task Start();
    Task SendItemLink(string itemNameKey, ItemInfo Item);
    Task SendStallNetwork(uint tid, byte pageID, int ItemDG, Dictionary<int, StallNetwork> StallNetwork);
    Task PurchaseStallNetwork(StallNetwork fleaItem, byte lastFreeSlot, bool closeStall);
    Task MoveTo(uint UniqueID);
    Task Sub_Item_Count(byte slot, uint numtodel);

    #region Features

    string Hwid { get; set; }
    byte[] hwidHelper { get; set; }
    bool CharacterGameReady { get; set; }
    bool FirstSpawn { get; set; }
    bool Verified { get; set; }
    Packet TempPacket { get; set; }
    ISessionData SessionData { get; init; }
    string ShardName { get; set; }
    #endregion

    #region Protection

    // Packet Modification
    int PacketLength { get; set; }
    // False Packets
    bool CharnameSent { get; set; }
    bool CharScreen { get; set; }
    bool UserLoggedIn { get; set; }

    #endregion
}