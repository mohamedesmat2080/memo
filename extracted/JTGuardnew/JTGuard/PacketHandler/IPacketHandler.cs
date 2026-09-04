using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using JTGuard.SessionManager;
using SilkroadSecurityAPI;

namespace JTGuard.PacketHandlerManager;

// ReSharper disable once InconsistentNaming
public delegate Task<PacketResult> _PacketHandler(Packet packet, ISession session, PacketData data);
public enum PacketResultType
{
    Disconnect, // if used in a multi handler setup it will disconnect the player immediately 
    Override, // if used in a multi handler setup the next handler will use the overridden packet! 
    Block, // if used in a multi handler setup it will block ALL FURTHER PACKETHANDLERS
    Nothing // if used in a multi handler setup it will do nothing and the following handler will use just use the packet again
}
public class PacketData
{
    public object? Data { get; }
    public int PreviousPriority { get; }
    public PacketResultType PreviousResult { get; }

    public bool HasData()
    {
        return Data != null;
    }

    public PacketData(object data, int priority, PacketResultType previousResult)
    {
        Data = data;
        PreviousResult = previousResult;
    }

    public PacketData(object data, PacketResultType previousResult)
    {
        Data = data;
        PreviousResult = previousResult;
    }

    public PacketData(int previousPriority, PacketData data)
    {
        Data = data.Data;
        PreviousPriority = previousPriority;
        PreviousResult = data.PreviousResult;
    }

    public override string ToString()
    {
        return "{Data: " + Data + ",\nPriority: " + PreviousPriority + ",\nResult: " + PreviousResult + "\n}";
    }
}

public class PacketResult : IDisposable
{
    public PacketResult(PacketData data, Packet? packet, PacketResultType packetResultType = PacketResultType.Nothing)
    {
        Data = new PacketData(data, packetResultType);
        OverridePacket = null;
        if (packetResultType == PacketResultType.Override)
            OverridePacket = packet;

        PacketResultType = packetResultType;
    }

    public PacketResult(Packet? packet, PacketResultType packetResultType = PacketResultType.Nothing)
    {
        Data = new PacketData(null, packetResultType);
        OverridePacket = null;
        if (packetResultType == PacketResultType.Override)
            OverridePacket = packet;

        PacketResultType = packetResultType;
    }

    public PacketResult(object data, PacketResultType packetResultType = PacketResultType.Nothing)
    {
        Data = new PacketData(data, packetResultType);
        OverridePacket = null;
        PacketResultType = packetResultType;
    }

    public PacketResult(PacketResultType packetResultType = PacketResultType.Nothing)
    {
        Data = new PacketData(null, packetResultType);
        OverridePacket = null;
        PacketResultType = packetResultType;
    }

    public PacketData Data { get; init; }
    public Packet? OverridePacket { get; init; }
    public PacketResultType PacketResultType { get; init; }

    public void Dispose()
    {
        throw new NotImplementedException();
    }
}
public interface IPacketHandler : IDisposable
{
    HashSet<ushort> _clientBlacklist { get; init; }
    SortedDictionary<ushort, SortedDictionary<int, _PacketHandler>> _clientHandlers { get; init; }
    HashSet<ushort> _clientWhitelist { get; init; }
    SortedDictionary<ushort, SortedDictionary<int, _PacketHandler>> _serverHandlers { get; init; }

    _PacketHandler _blockHandler { get; set; }
    _PacketHandler _defaultHandler { get; set; }
    _PacketHandler _disconnectHandler { get; set; }

    void SetDefaultHandler(_PacketHandler handler);
    Task<PacketResult> HandleDefault(Packet packet, ISession session, PacketData data);
    void SetBlockHandler(_PacketHandler handler);
    Task<PacketResult> HandleDisconnect(Packet packet, ISession session, PacketData data);
    void SetDisconnectHandler(_PacketHandler handler);
    Task<PacketResult> HandleBlock(Packet packet, ISession session, PacketData data);

    void RegisterModuleHandler(ushort msgId, _PacketHandler handler);
    void RegisterModuleHandler(ushort msgId, int priority, _PacketHandler handler);
    void UnregisterModuleHandler(ushort msgId, _PacketHandler handler);
    void UnregisterAllModuleHandler(ushort msgId);
    void RegisterClientHandler(ushort msgId, _PacketHandler handler);
    void RegisterClientHandler(ushort msgId, int priority, _PacketHandler handler);
    void UnregisterClientHandler(ushort msgId, _PacketHandler handler);
    void UnregisterAllClientHandler(ushort msgId);

    Task<PacketResult> HandleClient(Packet packet, ISession session);
    Task<PacketResult> HandleServer(Packet packet, ISession session);
}