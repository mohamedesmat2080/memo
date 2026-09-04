using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using JTGuard.Database;
using JTGuard.Database.Model;
using JTGuard.Helpers;
using JTGuard.PacketHandlerManager;
using JTGuard.ServerManagers;
using JTGuard.SessionManager;
using Microsoft.Data.SqlClient;
using SilkroadSecurityAPI;

namespace JTGuard.Server.AgentPacketHandler
{
    public partial class AlchemyPackets
    {
        private AgentServer AgentServer { get; set; }
        public _ServerSettings Service { get; set; }
        public AlchemyPackets(_ServerSettings service, AgentServer agentServer, IPacketHandler packetHandler)
        {
            AgentServer = agentServer;
            Service = service;
            packetHandler.RegisterClientHandler(0x7150, CLIENT_REINFORCE_REQUEST);
            //packetHandler.RegisterClientHandler(0x7157, CLIENT_DISMANTLE_REQUEST);
            //packetHandler.RegisterClientHandler(0x7155, CLIENT_DISJOIN_REQUEST);
            packetHandler.RegisterModuleHandler(0xB150, SERVER_REINFORCE_RESPONSE);
        }
        private async Task<PacketResult> CLIENT_REINFORCE_REQUEST(Packet packet, ISession session, object obj) // UNK
        {
            try
            {
                byte eBaseAction = packet.ReadUInt8();

                if (eBaseAction == 2)
                {
                    byte eSubAction = packet.ReadUInt8();
                    //Particularly for 'normal' elixir, but *could* also be for adv elixir.
                    if (eSubAction == 3)
                    {
                        //adv has 1 more byte here (0x02)
                        byte btElixirSlot = packet.ReadUInt8();
                        byte btItemSlot = packet.ReadUInt8();

                        SItemInfoDbRecord stItemInfoRecord = new SItemInfoDbRecord(); 
                        bool bHasItemInfo = await AgentServer.TryGetItemInfoAsync(stItemInfoRecord, session.SessionData.Charid, btItemSlot);

                        if (bHasItemInfo)
                        {
                            if (stItemInfoRecord.btOptLevel >= Service.MaxPlus)
                            {
                                string noticeMessage = Management.GetNoticeMessage("ALCHEMY_MAX_ITEM_PLUS_NOADV");
                                Packet stMsg = new Packet(0x168A);
                                stMsg.WriteUInt8(NoticeType.WARNING);
                                stMsg.WriteUnicode(noticeMessage);
                                await session.SendToClient(stMsg);
                                return new PacketResult(PacketResultType.Block);
                            }
                        }
                    }
                }
            }
            catch (Exception EX)
            {
                Program.PrintInColor($"{EX.Message.ToString()}, CLIENT_REINFORCE_REQUEST", ConsoleColor.Red);
            }
            return new PacketResult();
        }
        private async Task<PacketResult> SERVER_REINFORCE_RESPONSE(Packet packet, ISession session, object obj) // UNK
        {
            try
            {
                //1
                byte btResult = packet.ReadUInt8();
                if (btResult != 1)
                    return new PacketResult();

                //2
                byte btResult2 = packet.ReadUInt8();
                if (btResult2 != 2)
                    return new PacketResult();

                //3
                byte btResult3 = packet.ReadUInt8();
                if (btResult3 != 1)
                    return new PacketResult();

                //4
                byte btSlotIndex = packet.ReadUInt8();
                uint nItemGameID = packet.ReadUInt32();
                uint nRefItemID = packet.ReadUInt32();
                byte btNewOptLevel = packet.ReadUInt8();
                UInt64 qwVariance = packet.ReadUInt64();
                uint nDurability = packet.ReadUInt32();
                byte unk3 = packet.ReadUInt8();
                byte unk4 = packet.ReadUInt8();
                byte unk5 = packet.ReadUInt8();
                byte unk6 = packet.ReadUInt8();
                byte AdvStatus = packet.ReadUInt8();



                if (AdvStatus == 1)
                {
                    byte unk7 = packet.ReadUInt8();
                    uint AdvItemID = packet.ReadUInt32();
                    byte AdvPlus = packet.ReadUInt8();

                    using (var connection = new SqlConnection(Program.Connectionstring))
                    {
                        await connection.OpenAsync();
                        string query = $"EXEC _OnAlchemySuccess_EDIT {session.SessionData.Charid}, '{session.SessionData.Charname}', {nRefItemID}, {btNewOptLevel}, {AdvPlus}, {btSlotIndex}";
                        using (var command = new SqlCommand(query, connection))
                        {
                            await command.ExecuteNonQueryAsync();

                        }
                    }

                    if (Service.AlchemyItemLinkMinLevel != 0 && btNewOptLevel + AdvPlus >= Service.AlchemyItemLinkMinLevel)
                    {
                        var job = new DelayedJobItem(
                             500, session, null,
                             (s, p) =>
                             {
                                 Packet pck = new Packet(0x3533);
                                 pck.WriteUInt8(btSlotIndex);
                                 pck.WriteUInt8(AdvPlus);
                                 session.SendToServer(pck);
                             });

                        Management.g_DelayedJobMgr.CreateJob(job); 
                    }
                }
                else
                {
                    using (var connection = new SqlConnection(Program.Connectionstring))
                    {
                        await connection.OpenAsync();
                        string query = $"EXEC _OnAlchemySuccess_EDIT {session.SessionData.Charid}, '{session.SessionData.Charname}', {nRefItemID}, {btNewOptLevel}, {0}, {btSlotIndex}";
                        using (var command = new SqlCommand(query, connection))
                        {
                            await command.ExecuteNonQueryAsync();

                        }
                    }


                    if (Service.AlchemyItemLinkMinLevel != 0 && btNewOptLevel >= Service.AlchemyItemLinkMinLevel)
                    {
                        var job = new DelayedJobItem(
                             500, session, null,
                             (s, p) =>
                             {
                                 Packet pck = new Packet(0x3533);
                                 pck.WriteUInt8(btSlotIndex);
                                 pck.WriteUInt8(0);
                                 session.SendToServer(pck);
                             });

                        Management.g_DelayedJobMgr.CreateJob(job); 
                    }



                }
            }
            catch (Exception EX)
            {
                Program.PrintInColor($"{EX.Message.ToString()}, CLIENT_REINFORCE_REQUEST", ConsoleColor.Red);
            }
            return new PacketResult();
        }
    }
}
