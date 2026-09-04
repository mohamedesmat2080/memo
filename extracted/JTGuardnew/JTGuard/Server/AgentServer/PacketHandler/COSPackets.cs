using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Dapper;
using JTGuard.Database.Model;
using JTGuard.Database.Models;
using JTGuard.PacketHandlerManager;
using JTGuard.ServerManagers;
using JTGuard.SessionManager;
using Microsoft.Data.SqlClient;
using SilkroadSecurityAPI;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace JTGuard.Server.AgentPacketHandler
{
    public partial class COSPackets
    {
        private AgentServer AgentServer { get; set; }
        public _ServerSettings Service { get; set; }
        public COSPackets(_ServerSettings service, AgentServer agentServer, IPacketHandler packetHandler)
        {
            Service = service;
            AgentServer = agentServer;
            packetHandler.RegisterModuleHandler(0x30C8, HandleCosDataAck);
            packetHandler.RegisterModuleHandler(0x30C9, SERVER_AGENT_COS_UPDATE);
            packetHandler.RegisterModuleHandler(0xB0CB, SERVER_AGENT_COS_UPDATE_RIDESTATE);
        }
        private async Task<PacketResult> SERVER_AGENT_COS_UPDATE_RIDESTATE(Packet packet, ISession session, object obj)
        {
            if (packet.ReadUInt8() != 0x01)
                return new PacketResult();

            var ownerUniqueId = packet.ReadUInt32();
            var isMounted = packet.ReadBool();
            var cosUniqueId = packet.ReadUInt32();
            session.SessionData.OnTransport = isMounted;
            session.SessionData.TransportUniqueId = cosUniqueId;


            return new PacketResult();
        }
        private async Task<PacketResult> SERVER_AGENT_COS_UPDATE(Packet packet, ISession session, object obj) // UNK
        {
            try
            {
                uint nGameID = packet.ReadUInt32();

                byte type = packet.ReadUInt8();

                if (session.SessionData.FellowPetUniqueID == nGameID)
                {
                    if (type == 1)
                    {
                        session.SessionData.FellowPetUniqueID = 0;
                        session.SessionData.FellowPetID64 = 0;
                        session.SessionData.FellowItemID = 0;

                    }

                }
            }
            catch (Exception EX)
            {
                Program.PrintInColor($"{EX.Message.ToString()}, SERVER_AGENT_COS_UPDATE", ConsoleColor.Red);
            }
            return new PacketResult();
        }
        private async Task<PacketResult> HandleCosDataAck(Packet packet, ISession session, object obj) // UNK
        {
            try
            {
                uint nGameID = packet.ReadUInt32();
                uint nRefObjID = packet.ReadUInt32();

                if (RefManager.m_RefFellowPetRefObjID.Contains(Convert.ToInt32(nRefObjID)))
                {

                    //In vsro 188, fellows are growth pets, so parsing that.
                    uint nCurHealth = packet.ReadUInt32();
                    uint nMaxHealth = packet.ReadUInt32();
                    UInt64 ulExpOffset = packet.ReadUInt64();
                    byte btUnk1 = packet.ReadUInt8();
                    ushort shHungerPoints = packet.ReadUInt16();
                    uint bIsOffensive = packet.ReadUInt32();
                    string strName = packet.ReadAscii();
                    byte btUnk2 = packet.ReadUInt8();
                    uint nOwnerGID = packet.ReadUInt32();
                    byte ItemSlot = packet.ReadUInt8();



                    if (nOwnerGID != session.SessionData.UniqueCharId)
                        return new PacketResult();


                    session.SessionData.FellowPetUniqueID = nGameID;
                    long itemID64 = 0;
                    int refItemID = 0;
                    using (var connection = new SqlConnection(Program.Connectionstring))
                    {
                        using (var command = new SqlCommand("_GetFellowPetID64", connection) { CommandType = CommandType.StoredProcedure })
                        {
                            command.Parameters.AddWithValue("@CharID", session.SessionData.Charid);
                            command.Parameters.AddWithValue("@Slot", ItemSlot);

                            var itemID64Param = new SqlParameter("@ItemID64", SqlDbType.BigInt) { Direction = ParameterDirection.Output };
                            var refItemIDParam = new SqlParameter("@RefItemID", SqlDbType.Int) { Direction = ParameterDirection.Output };
                            command.Parameters.Add(itemID64Param);
                            command.Parameters.Add(refItemIDParam);

                            await connection.OpenAsync();
                            await command.ExecuteNonQueryAsync();

                            itemID64 = (long)itemID64Param.Value;
                            refItemID = (int)refItemIDParam.Value;
                        }

                    }

                    if (itemID64 != 0 && refItemID != 0)
                    {
                        session.SessionData.FellowPetID64 = itemID64;
                        session.SessionData.FellowItemID = refItemID;


                        foreach (var data in RefManager.m_RefFellowData)
                        {
                            if (data.Value.ItemID == refItemID)
                            {
                                /// FELLOW DATA MEVCUT EVET ŞİMDİ FELLOW'UN BILGILERINI GÖNDERELİM.
                                /// int64 ile fellowun bilgilerini bağdaştıralım.. Fellowu karakterden bağımsız kendine özgü yapacağız. önce tablodan kontrol et verileri
                                /// veriler yoksa default ekle ve gönder var ise mevcut verileri cliente gönderelim. hatırlamak için NOT !!!! tabi bunu INT64 ile sorgulayacağız.
                                using (var connection = new SqlConnection(Program.Connectionstring))
                                {
                                    await connection.OpenAsync();

                                    var Fellowdata = await connection.QueryAsync<_FellowSkillData>($"SELECT * FROM _FellowSkillData with (nolock) WHERE ID64 = {itemID64}");

                                    if (Fellowdata.Count() > 0)
                                    {
                                        Packet stAckMsg = new Packet(0x206B);

                                        stAckMsg.WriteUInt8(Fellowdata.Count());

                                        foreach (var Data in Fellowdata)
                                        {
                                            stAckMsg.WriteInt64(Data.ID64);

                                            if (data.Value.SkillType_1 == 0) /// pet skill
                                            {
                                                stAckMsg.WriteUInt8(1);
                                            }
                                            else
                                            {
                                                stAckMsg.WriteUInt8(Data.Enable_Skill_1);
                                            }

                                            if (data.Value.SkillType_2 == 0) /// pet skill
                                            {
                                                stAckMsg.WriteUInt8(1);
                                            }
                                            else
                                            {
                                                stAckMsg.WriteUInt8(Data.Enable_Skill_2);
                                            }

                                            if (data.Value.SkillType_3 == 0) /// pet skill
                                            {
                                                stAckMsg.WriteUInt8(1);
                                            }
                                            else
                                            {
                                                stAckMsg.WriteUInt8(Data.Enable_Skill_3);
                                            }

                                            if (data.Value.SkillType_4 == 0) /// pet skill
                                            {
                                                stAckMsg.WriteUInt8(1);
                                            }
                                            else
                                            {
                                                stAckMsg.WriteUInt8(Data.Enable_Skill_4);
                                            }

                                            if (data.Value.SkillType_5 == 0) /// pet skill
                                            {
                                                stAckMsg.WriteUInt8(1);
                                            }
                                            else
                                            {
                                                stAckMsg.WriteUInt8(Data.Enable_Skill_5);
                                            }

                                        }
                                        await session.SendToClient(stAckMsg);
                                    }
                                    else
                                    {
                                        var str = new _FellowSkillData();
                                        str.ID64 = itemID64;
                                        if (data.Value.SkillType_1 == 0 && data.Value.SkillID_1 != 0)
                                        {
                                            str.Enable_Skill_1 = 0;
                                        }
                                        if (data.Value.SkillType_2 == 0 && data.Value.SkillID_2 != 0)
                                        {
                                            str.Enable_Skill_2 = 0;
                                        }
                                        if (data.Value.SkillType_3 == 0 && data.Value.SkillID_3 != 0)
                                        {
                                            str.Enable_Skill_3 = 0;
                                        }
                                        if (data.Value.SkillType_4 == 0 && data.Value.SkillID_4 != 0)
                                        {
                                            str.Enable_Skill_4 = 0;
                                        }
                                        if (data.Value.SkillType_5 == 0 && data.Value.SkillID_5 != 0)
                                        {
                                            str.Enable_Skill_5 = 0;
                                        }

                                        Packet stAckMsg = new Packet(0x206B);

                                        stAckMsg.WriteUInt8(1);

                                        stAckMsg.WriteInt64(str.ID64);
                                        stAckMsg.WriteUInt8(str.Enable_Skill_1);
                                        stAckMsg.WriteUInt8(str.Enable_Skill_2);
                                        stAckMsg.WriteUInt8(str.Enable_Skill_3);
                                        stAckMsg.WriteUInt8(str.Enable_Skill_4);
                                        stAckMsg.WriteUInt8(str.Enable_Skill_5);
                                        await session.SendToClient(stAckMsg);

                                        string query = $"EXEC _HandleFellowData {str.ID64}, {str.Enable_Skill_1}, {str.Enable_Skill_2}, {str.Enable_Skill_3}, {str.Enable_Skill_4}, {str.Enable_Skill_5}";
                                        using (var command = new SqlCommand(query, connection))
                                        {
                                            await command.ExecuteNonQueryAsync();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception EX)
            {
                Program.PrintInColor($"{EX.Message.ToString()}, HandleCosDataAck", ConsoleColor.Red);
            }
            return new PacketResult();
        }
    }
}
