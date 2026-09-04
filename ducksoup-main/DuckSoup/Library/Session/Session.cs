<<<<<<< ours
﻿#region
=======
#region
>>>>>>> theirs

using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;
using API;
using API.Database.SRO_VT_SHARD;
using API.Model;
using API.Server;
using API.Session;
using log4net;
using SilkroadSecurityAPI;

#endregion

namespace DuckSoup.Library.Session;

public sealed class Session : ISession
{

    private readonly byte[] _clientBuffer = new byte[4096];
    private readonly Security _clientSecurity = new();

    private readonly ILog _logger = Global.Logger;
    private readonly byte[] _serverBuffer = new byte[4096];
    private readonly Security _serverSecurity = new();
    private TcpClient _clientTcpClient;
    private bool _exit;
    private TcpClient _serverTcpClient;

    public Session(TcpClient clientTcpClient, IAsyncServer asyncServer)
    {
        SessionData = new SessionData();
        AsyncServer = asyncServer;
        _clientTcpClient = clientTcpClient;

        if (SharedObjects.settings["DebugLevel"] >= DebugLevel.Connections)
            _logger.InfoFormat("{0} - Preparing Session..", asyncServer.Service.Name);

        // generates a "unique" id from the address and port hashcode and safes ip
        if (!(_clientTcpClient.Client.RemoteEndPoint is IPEndPoint ep)) return;
        ClientId = ep.Address.GetHashCode() + ep.Port.GetHashCode();
        ClientIp = ep.Address.ToString();
    }

    public IAsyncServer AsyncServer { get; init; }
    public int ClientId { get; set; }
    public string ClientIp { get; set; }

    public void Dispose()
    {
        Dispose("Unknown reason");
    }

    public void Dispose(string reason)
    {
        if (SharedObjects.settings["DebugLevel"] >= DebugLevel.Connections)
            _logger.InfoFormat("{0} - Stop Session - {1} ({2}) - {3}",
                AsyncServer.Service.Name, ClientId,
                ClientIp,
                reason);

        // double socket close prevention
        if (_exit) return;
        _exit = true;
        AsyncServer.RemoveSession(this);

        _clientTcpClient?.Close();
        _clientTcpClient = null;
        _serverTcpClient?.Close();
        _serverTcpClient = null;
        SessionData?.Dispose();
        // removes the session from the session list - the function has a contains check
    }

    public async Task SendToClient(Packet packet)
    {
        if (_clientTcpClient == null || _serverTcpClient == null || _exit)
            return;

        try
        {
            _clientSecurity.Send(packet);
            // probably not needed tho since we're in a permanent circle anyways
            // might be the reason login and so on is FUCKING slow
            await TransferToClient();
        }
        catch (Exception)
        {
            Dispose("send to client");
        }
    }
    public async Task SendToClient(List<Packet> packets)
    {
        if (_clientTcpClient == null || _serverTcpClient == null || _exit)
            return;

        try
        {
            foreach (Packet packet in packets)
                _clientSecurity.Send(packet);
            // probably not needed tho since we're in a permanent circle anyways
            // might be the reason login and so on is FUCKING slow
            await TransferToClient();
        }
        catch (Exception)
        {
            Dispose("send to client");
        }
    }

    public async Task SendToServer(Packet packet)
    {
        if (_clientTcpClient == null || _serverTcpClient == null || _exit)
            return;

        try
        {
            _serverSecurity.Send(packet);
            // probably not needed tho since we're in a permanent circle anyways
            // might be the reason login and so on is FUCKING slow
            await TransferToServer();
        }
        catch (Exception)
        {
            Dispose("send to server");
        }
    }

    public async Task SendNotice(string message)
    {
        var notice = new Packet(0x183B);
        notice.WriteByte(2);
        notice.WriteAscii(message);
        await SendToClient(notice);
    }
    public async Task SendItemLink(string itemNameKey, ItemInfo Item)
    {
        var packet = new Packet(0x180B, false, false);
        packet.WriteAscii(itemNameKey);
        packet.WriteUInt32(0); //Unknwon (could be unique id)
        packet.WriteUInt32(Item.RefItemID); //itemid
        switch (Item.TypeID2)
        {
            case 1:
                packet.WriteUInt8(Item.Plus);
                packet.WriteUInt64(Item.Variance);
                packet.WriteUInt32(Item.Durability);
                packet.WriteUInt8(Item.MagParamNum);
                if (Item.MagParamNum > 0)
                {
                    foreach (var mag in Item.MagicOptions)
                    {
                        packet.WriteUInt32(mag % 4294967296);
                        packet.WriteUInt32(mag / 4294967296);
                    }
                }
                //1 = Socket
                packet.WriteUInt8(1);
                packet.WriteUInt8(Item.Sockets.Count);
                foreach (var socket in Item.Sockets)
                {
                    packet.WriteUInt8(socket.Slot);
                    packet.WriteUInt16(socket.ID);
                    packet.WriteUInt16(socket.Value);
                    packet.WriteUInt32(socket.nParam);
                }
                // 2 = Advanced elixir
                packet.WriteUInt8(2);
                packet.WriteUInt8(Item.Advances.Count);
                foreach (var adv in Item.Advances)
                {
                    packet.WriteUInt8(adv.Slot);
                    packet.WriteUInt32(adv.ID);
                    packet.WriteUInt32(adv.Value);
                }

                break;
            case 2:
                switch (Item.TypeID3)
                {
                    case 1:
                        packet.WriteUInt8(1); //State
                        break;

                    case 2:
                        packet.WriteUInt32(0); //Monster mask or so
                        break;

                    default:
                        if (Item.TypeID4 == 3) //Magic cube
                            packet.WriteInt32(Item.Durability);
                        break;
                }
                break;
            case 3:
                packet.WriteUInt16(Item.Durability);

                if (Item.TypeID3 == 11) //Magic stones
                    if (Item.TypeID4 == 1 || Item.TypeID4 == 2)
                        packet.WriteUInt8(0); //AttributeAssimilationProbability
                    else if (Item.TypeID3 == 14 && Item.TypeID4 == 2) //ITEM_MALL_GACHA_CARD_WIN & LOSE
                        packet.WriteUInt8(0);
                break;
        }
        await SendToClient(packet);
    }
    public async Task SendStallNetwork(uint tid, byte pageID, int ItemDG, Dictionary<int, StallNetwork> StallNetwork)
    {
        SessionData.lastfleaTID = tid;
        SessionData.lastfleaPageID = pageID;
        SessionData.lastfleaitemDG = ItemDG;

        using var db = new SRO_VT_SHARD();
        var continentName =
               (await db.C_RefRegion.Where(x => x.wRegionID == SessionData.LatestRegionId).FirstOrDefaultAsync()).ContinentName;


        List<KeyValuePair<int, StallNetwork>> getallItems;
        if (ItemDG == 0)
            getallItems = StallNetwork.Where(x => x.Value.tid == tid && (bool)x.Value.isActive && x.Value.continentName == continentName).ToList();
        else
            getallItems = StallNetwork.Where(x => x.Value.tid == tid && (bool)x.Value.isActive && x.Value.continentName == continentName && x.Value.itemInfo.itemDegree == ItemDG).ToList();

        var getItems = getallItems.Skip((pageID - 1) * 15).Take(15).ToList();

        Packet b461 = new Packet(0xB461);
        b461.WriteUInt8(01);
        b461.WriteUInt8(getItems.Count);
        b461.WriteUInt8(((getallItems.Count - 1) / 15) + 1);
        foreach (var x in getItems)
        {
            b461.WriteUInt32(0);
            b461.WriteUInt32(x.Value.itemInfo.RefItemID);
            switch (x.Value.itemInfo.TypeID2)
            {
                case 1:
                    b461.WriteUInt8(x.Value.itemInfo.Plus);
                    b461.WriteUInt64(x.Value.itemInfo.Variance);
                    b461.WriteUInt32(x.Value.itemInfo.Durability);
                    b461.WriteUInt8(x.Value.itemInfo.MagParamNum);
                    if (x.Value.itemInfo.MagParamNum > 0)
                    {
                        foreach (var mag in x.Value.itemInfo.MagicOptions)
                        {
                            b461.WriteUInt32(mag % 4294967296);
                            b461.WriteUInt32(mag / 4294967296);
                        }
                    }
                    //1 = Socket
                    b461.WriteUInt8(1);
                    b461.WriteUInt8(x.Value.itemInfo.Sockets.Count);
                    foreach (var socket in x.Value.itemInfo.Sockets)
                    {
                        b461.WriteUInt8(socket.Slot);
                        b461.WriteUInt16(socket.ID);
                        b461.WriteUInt16(socket.Value);
                        b461.WriteUInt32(socket.nParam);
                    }
                    // 2 = Advanced elixir
                    b461.WriteUInt8(2);
                    b461.WriteUInt8(x.Value.itemInfo.Advances.Count);
                    foreach (var adv in x.Value.itemInfo.Advances)
                    {
                        b461.WriteUInt8(adv.Slot);
                        b461.WriteUInt32(adv.ID);
                        b461.WriteUInt32(adv.Value);
                    }

                    break;
                case 2:
                    switch (x.Value.itemInfo.TypeID3)
                    {
                        case 1:
                            b461.WriteUInt8(1); //State
                            break;

                        case 2:
                            b461.WriteUInt32(0); //Monster mask or so
                            break;

                        default:
                            if (x.Value.itemInfo.TypeID4 == 3) //Magic cube
                                b461.WriteInt32(x.Value.itemInfo.Durability);
                            break;
                    }
                    break;
                case 3:
                    b461.WriteUInt16(x.Value.itemInfo.Durability);

                    if (x.Value.itemInfo.TypeID3 == 11) //Magic stones
                        if (x.Value.itemInfo.TypeID4 == 1 || x.Value.itemInfo.TypeID4 == 2)
                            b461.WriteUInt8(0); //AttributeAssimilationProbability
                        else if (x.Value.itemInfo.TypeID3 == 14 && x.Value.itemInfo.TypeID4 == 2) //ITEM_MALL_GACHA_CARD_WIN & LOSE
                            b461.WriteUInt8(0);
                    break;
            }

            b461.WriteUInt32(x.Value.itemInfo.Durability);
            b461.WriteAscii(x.Value.sellerName);
            b461.WriteUInt8(x.Value.invSlot);
            b461.WriteUInt16(x.Value.quantity);
            b461.WriteUInt8(01);
            b461.WriteUInt64(x.Value.price);
            b461.WriteUInt64(x.Key);
        }
        await SendToClient(b461);
    }
    public async Task PurchaseStallNetwork(StallNetwork fleaItem, byte lastFreeSlot, bool closeStall)
    {
        var packet = new Packet(0xB034);
        packet.WriteUInt8(0x01); //Success
        packet.WriteUInt8(0x06); //pickitem
        packet.WriteUInt8(lastFreeSlot); //slot
        packet.WriteUInt32(0); //Unknwon (could be unique id)
        packet.WriteUInt32(fleaItem.itemInfo.RefItemID); //itemid
        switch (fleaItem.itemInfo.TypeID2)
        {
            case 1:
                packet.WriteUInt8(fleaItem.itemInfo.Plus);
                packet.WriteUInt64(fleaItem.itemInfo.Variance);
                packet.WriteUInt32(fleaItem.itemInfo.Durability);
                packet.WriteUInt8(fleaItem.itemInfo.MagParamNum);
                if (fleaItem.itemInfo.MagParamNum > 0)
                {
                    foreach (var mag in fleaItem.itemInfo.MagicOptions)
                    {
                        packet.WriteUInt32(mag % 4294967296);
                        packet.WriteUInt32(mag / 4294967296);
                    }
                }
                //1 = Socket
                packet.WriteUInt8(1);
                packet.WriteUInt8(fleaItem.itemInfo.Sockets.Count);
                foreach (var socket in fleaItem.itemInfo.Sockets)
                {
                    packet.WriteUInt8(socket.Slot);
                    packet.WriteUInt16(socket.ID);
                    packet.WriteUInt16(socket.Value);
                    packet.WriteUInt32(socket.nParam);
                }
                // 2 = Advanced elixir
                packet.WriteUInt8(2);
                packet.WriteUInt8(fleaItem.itemInfo.Advances.Count);
                foreach (var adv in fleaItem.itemInfo.Advances)
                {
                    packet.WriteUInt8(adv.Slot);
                    packet.WriteUInt32(adv.ID);
                    packet.WriteUInt32(adv.Value);
                }
                break;
            case 2:
                switch (fleaItem.itemInfo.TypeID3)
                {
                    case 1:
                        packet.WriteUInt8(1); //State
                        break;

                    case 2:
                        packet.WriteUInt32(0); //Monster mask or so
                        break;

                    default:
                        if (fleaItem.itemInfo.TypeID4 == 3) //Magic cube
                            packet.WriteInt32(fleaItem.itemInfo.Durability);
                        break;
                }
                break;
            case 3:
                packet.WriteUInt16(fleaItem.itemInfo.Durability);

                if (fleaItem.itemInfo.TypeID3 == 11) //Magic stones
                    if (fleaItem.itemInfo.TypeID4 == 1 || fleaItem.itemInfo.TypeID4 == 2)
                        packet.WriteUInt8(0); //AttributeAssimilationProbability
                    else if (fleaItem.itemInfo.TypeID3 == 14 && fleaItem.itemInfo.TypeID4 == 2) //ITEM_MALL_GACHA_CARD_WIN & LOSE
                        packet.WriteUInt8(0);
                break;
        }

        Packet enterStall = new Packet(0x70B3);
        Packet buyItem = new Packet(0x70B4);
        enterStall.WriteUInt32(fleaItem.uniqueID);
        buyItem.WriteUInt8(fleaItem.stallSlot);

        await SendToServer(enterStall);
        await SendToServer(buyItem);
        await SendToClient(packet);
        if (closeStall)
        {
            Packet stallClose = new Packet(0x70B5);
            await SendToServer(stallClose);
        }
    }

    public async Task MoveTo(uint UniqueID)
    {
        Packet packet = new Packet(0x7010);
        packet.WriteUInt16(25);
        packet.WriteUInt8(0x0);
        packet.WriteUInt32(SessionData.UniqueCharId);
        packet.WriteUInt32(UniqueID);
        await SendToServer(packet);
    }
    public async Task Sub_Item_Count(byte slot , uint numtodel)
    {
        Packet packet = new Packet(0x7010);
        packet.WriteUInt16(25);
        packet.WriteUInt8(0x1);
        packet.WriteUInt32(SessionData.UniqueCharId);
        packet.WriteInt32(slot);
        packet.WriteUInt32(numtodel);
        await SendToServer(packet);
    }
    public async Task Start()
    {
        if (SharedObjects.settings["DebugLevel"] >= DebugLevel.Connections)
            _logger.InfoFormat("{0} - Starting Session..", AsyncServer.Service.Name);

        _clientSecurity.GenerateSecurity(true, true, true);
        // creates a new server socket and connects it according to the remote addr. and port 
        _serverTcpClient = new TcpClient();
        await _serverTcpClient.ConnectAsync(AsyncServer.RemoteEndPoint.Address, AsyncServer.RemoteEndPoint.Port);

        // Just making sure to disconnect clients that have fucked up
        _ = Task.Factory.StartNew(() =>
        {
            Thread.Sleep(500);
            if (_serverTcpClient is not {Connected: true} || _clientTcpClient is not {Connected: true})
                Dispose("500 ms");

            return Task.CompletedTask;
        });

        // starts receiving loop from server + client - if a destroy was called the loop brakes
        _ = Task.Factory.StartNew(async () =>
        {
            while (!_exit)
                await DoReceiveFromServer();

            _serverTcpClient?.Close();
            _serverTcpClient = null;
        }, CancellationToken.None, TaskCreationOptions.LongRunning, TaskScheduler.Default);

        _ = Task.Factory.StartNew(async () =>
        {
            while (!_exit)
                await DoReceiveFromClient();

            _clientTcpClient?.Close();
            _clientTcpClient = null;
        }, CancellationToken.None, TaskCreationOptions.LongRunning, TaskScheduler.Default);
    }

    private async Task DoReceiveFromClient()
    {
        try
        {
            // receive stuff
            var clientBufferMemory = new Memory<byte>(_clientBuffer);
            var recvCount = await _clientTcpClient.GetStream().ReadAsync(clientBufferMemory);

            if (recvCount == 0)
            {
                Dispose("receive count 0");
                return;
            }

            // starts receiving again
            _clientSecurity.Recv(_clientBuffer, 0, recvCount);

            // transfers the incoming packets to a list
            var receivedPackets = _clientSecurity.TransferIncoming();

            // if there are no packets start receiving again
            if (receivedPackets == null)
            {
                await TransferToServer();
                return;
            }


            // loop through all received packets
            foreach (var packet in receivedPackets)
            {
                // ignore handshake
                if (packet.Opcode == 0x9000 || packet.Opcode == 0x5000 || packet.Opcode == 0x2001)
                    continue;

                #region Protection

                // Packet Modifying
                PacketLength = packet.GetBytes().Length;

                // Packet Flooding
                // calc the last checktime
                var lastCheckDiff = (DateTime.Now - _lastPacketReset).TotalSeconds;
                // lastcheckdiff * bytelimitation for exact bytes per time - don't need to round it to one second
                var maxBytesPerTime = lastCheckDiff * AsyncServer.Service.ByteLimitation;
                // if the packetsize exceeded the calculated value and it was measured over 2 second (to prevent super random lag dcs)
                if (_packetSize > maxBytesPerTime && lastCheckDiff > 2.0)
                {
                    if (SharedObjects.settings["DebugLevel"] >= DebugLevel.Warning)
                        _logger.WarnFormat(
                            "{0} - Client {1}({2}) exceedet the byte limit: {3} (maximum: {4} - Last check {5} seconds ago)",
                            AsyncServer.Service.Name, ClientId, ClientIp,
                            _packetSize, maxBytesPerTime, lastCheckDiff);
                    Dispose("byte limit");
                }
                else if (lastCheckDiff > 1)
                {
                    // else reset
                    _lastPacketReset = DateTime.Now;
                    _packetSize = 0;
                }

                #endregion

                var packetResult = await AsyncServer.PacketHandler.HandleClient(packet, this);

                // debug
                if (SharedObjects.settings["DebugLevel"] >= DebugLevel.Debug)
                    _logger.DebugFormat("{0} - DoRecvFromClient Packet: 0x{1:X} - {2} ({3}) - Status: {4} ",
                        AsyncServer.Service.Name, packet.Opcode, ClientId, ClientIp,
                        packetResult.PacketResultType);

                switch (packetResult.PacketResultType)
                {
                    case PacketResultType.Override:
                        _serverSecurity.Send(packetResult.OverridePacket);
                        break;
                    case PacketResultType.Block:
                        break;
                    case PacketResultType.Disconnect:
                        Dispose("receive from client disconnect");
                        break;
                    case PacketResultType.Nothing:
                        _serverSecurity.Send(packet);
                        break;
                    default:
                        throw new ArgumentOutOfRangeException();
                }
            }

            // transfers packets to the server and starts receiving from client again
            await TransferToServer();
        }
        catch (Exception e)
        {
            Dispose("receive from client catch " + e?.Message +  "\n"+ e?.InnerException?.Message);
        }
    }

    private async Task DoReceiveFromServer()
    {
        try
        {
            // receive stuff
            var serverBufferMemory = new Memory<byte>(_serverBuffer);
            var recvCount = await _serverTcpClient.GetStream().ReadAsync(serverBufferMemory);
            if (recvCount == 0)
            {
                Dispose("receive from server disconnect");
                return;
            }

            // starts receiving again
            _serverSecurity.Recv(_serverBuffer, 0, recvCount);

            // transfers the incoming packets to a list
            var receivedPackets = _serverSecurity.TransferIncoming();

            // if there are no packets start receiving again
            if (receivedPackets == null)
            {
                await TransferToClient();
                return;
            }

            // loop through all received packets
            foreach (var packet in receivedPackets)
            {
                // ignore handshake
                if (packet.Opcode == 0x5000 || packet.Opcode == 0x9000)
                    continue;

                // debug
                if (SharedObjects.settings["DebugLevel"] >= DebugLevel.Debug)
                    Global.Logger.DebugFormat("{0} - DoRecvFromServer Packet: 0x{1:X} - {2} ({3})",
                        AsyncServer.Service.Name, packet.Opcode, ClientId, ClientIp);

                var packetResult = await AsyncServer.PacketHandler.HandleServer(packet, this);

                switch (packetResult.PacketResultType)
                {
                    case PacketResultType.Override:
                        _clientSecurity.Send(packetResult.OverridePacket);
                        break;
                    case PacketResultType.Block:
                        break;
                    case PacketResultType.Disconnect:
                        Dispose("receive from server disconnect");
                        break;
                    case PacketResultType.Nothing:
                        _clientSecurity.Send(packet);
                        break;
                    default:
                        throw new ArgumentOutOfRangeException();
                }
            }

            // transfers packets to the client and starts receiving from server again
            await TransferToClient();
        }
        catch (Exception e)
        {
            Dispose("receive from server catch " + e?.Message +  "\n"+ e?.InnerException?.Message);
        }
    }

    private async Task TransferToClient()
    {
        if (_exit) return;
        try
        {
            var kvp = _clientSecurity.TransferOutgoing();

            if (kvp == null) return;

            foreach (var t in kvp)
                //transfers the client packets to the client
                await _clientTcpClient.GetStream().WriteAsync(t.Key.Buffer, 0, t.Key.Buffer.Length);
        }
        catch (Exception)
        {
            Dispose("transfer to client catch");
        }
    }

    private async Task TransferToServer()
    {
        if (_exit) return;

        try
        {
            var kvp = _serverSecurity.TransferOutgoing();
            if (kvp == null) return;
            foreach (var t in kvp)
            {
                _packetSize += t.Key.Buffer.Length;
                //transfers the server packets to the server
                await _serverTcpClient.GetStream().WriteAsync(t.Key.Buffer, 0, t.Key.Buffer.Length);
            }
        }
        catch (Exception)
        {
            Dispose("transfer to server catch");
        }
    }


    #region Features

    public string Hwid { get; set; }
    public byte[] hwidHelper { get; set; }
    public bool CharacterGameReady { get; set; } = false;
    public bool FirstSpawn { get; set; } = false;
    public bool Verified { get; set; } = false;
    public ISessionData SessionData { get; init; }
    public Packet TempPacket { get; set; }
    public string ShardName { get; set; }
    #endregion

    #region Protection

    // Packet Modification
    public int PacketLength { get; set; }

    // Packet Flooding
    private int _packetSize;
    private DateTime _lastPacketReset;

    // False Packets
    public bool CharnameSent { get; set; } = false;
    public bool CharScreen { get; set; } = false;
    public bool UserLoggedIn { get; set; } = false;

    #endregion
}