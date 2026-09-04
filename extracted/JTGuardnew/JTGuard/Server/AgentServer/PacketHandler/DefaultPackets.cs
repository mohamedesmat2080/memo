using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Net.NetworkInformation;
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
using static System.Net.Mime.MediaTypeNames;

namespace JTGuard.Server.AgentPacketHandler
{
    public partial class DefaultPackets
    {
        private AgentServer AgentServer { get; set; }
        public _ServerSettings Service { get; set; }
        public DefaultPackets(_ServerSettings service, AgentServer agentServer, IPacketHandler packetHandler)
        {
            AgentServer = agentServer;
            Service = service;
            packetHandler.RegisterModuleHandler(0xB069, REMOTE_PARTY_MATCHING_FORM);

            packetHandler.RegisterClientHandler(0x3053, AGENT_RESPAWN); // GameReady true
            packetHandler.RegisterClientHandler(0x705A, AGENT_TELEPORT_USE); // DONE

            packetHandler.RegisterClientHandler(0x7470, AGENT_ACADEMY_CREATE);
            packetHandler.RegisterClientHandler(0x7472, AGENT_ACADEMY_CREATE2);
            packetHandler.RegisterClientHandler(0x747E, AGENT_ACADEMY_MATCHING_JOIN);
            packetHandler.RegisterModuleHandler(0x3054, SERVER_LEVELINFO);
            packetHandler.RegisterClientHandler(0x7074, CLIENT_CHARACTER_ACTION_REQUEST); // Snow Shield fix
            packetHandler.RegisterClientHandler(0x70BA, AGENT_STALL_UPDATE);
            packetHandler.RegisterClientHandler(0x70B1, AGENT_STALL_CREATE);
            packetHandler.RegisterClientHandler(0x7081, AGENT_EXCHANGE_START);
            packetHandler.RegisterClientHandler(0x70F3, AGENT_GUILD_INVITE);
            packetHandler.RegisterClientHandler(0x70FB, AGENT_UNION_INVITE);
            packetHandler.RegisterClientHandler(0x7025, HandleChatReq);
            packetHandler.RegisterClientHandler(0x7516, AGENT_FRPVP_UPDATE);
            packetHandler.RegisterClientHandler(0x704C, AGENT_INVENTORY_ITEM_USE);
            packetHandler.RegisterClientHandler(0x7069, CLIENT_AGENT_PARTY_MATCHING_FORM_REQUEST);
            packetHandler.RegisterClientHandler(0x7060, CLIENT_AGENT_PARTY_CREATE_REQUEST);
            packetHandler.RegisterClientHandler(0x706D, CLIENT_AGENT_PARTY_MATCHING_JOIN_REQUEST);
            packetHandler.RegisterClientHandler(0x7062, CLIENT_AGENT_PARTY_INVITE_REQUEST);
            packetHandler.RegisterModuleHandler(0xB516, SERVER_FRPVP_UPDATE);

        }
        private async Task<PacketResult> REMOTE_PARTY_MATCHING_FORM(Packet packet, ISession session, PacketData data)
        {
            try
            {
                var btResult = packet.ReadUInt8(); // 1   byte    result
                if (btResult == 0x01)
                {
                    uint nMatchingID = packet.ReadUInt32();
                    //Console.WriteLine(nMatchingID);
                    if (AgentServer.eventManager.luckyptnumber.LuckyPartyNumberEventStatus)
                    {
                        if (AgentServer.eventManager.luckyptnumber.LuckyPartyNumberTargetNumber == nMatchingID)
                        {
                            /// WINNNNNNNNNN
                            AgentServer.eventManager.luckyptnumber.LuckyPartyNumberEventStatus = false;

                            string WinNotice = $"[Lucky Party Number] {session.SessionData.Charname} is win this round!";
                            Packet Packets = new Packet(0x168A);
                            Packets.WriteUInt8(NoticeType.QUEST);
                            Packets.WriteUnicode(WinNotice);
                            await AgentServer.BroadcastPacket(Packets);

                            await sqlQueryHelper.AddItemToChest(session.SessionData.Charid, AgentServer.eventManager.luckyptnumber.m_LuckyPtNumberSettings[0].RewardName, AgentServer.eventManager.luckyptnumber.m_LuckyPtNumberSettings[0].RewardQuantity,
                                "Lucky Party Number", 0);
                            Packet stMsg = new Packet(0x169C);
                            stMsg.WriteUInt8(12);
                            stMsg.WriteUnicode("item [%ls] added to chest.");
                            stMsg.WriteInt32(AgentServer.eventManager.luckyptnumber.m_LuckyPtNumberSettings[0].RewardID);
                            await session.SendToClient(stMsg);

                        }
                    }
                    AgentServer.LastPartyMatchingID = Convert.ToInt32(nMatchingID);
                }
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "REMOTE_PARTY_MATCHING_FORM");
            }
           
            return new PacketResult();
        }
        private async Task<PacketResult> SERVER_FRPVP_UPDATE(Packet packet, ISession session, PacketData data)
        {
            var result = packet.ReadUInt8(); // 1   byte    result
            if (result == 1)
            {
                var uniqueId = packet.ReadUInt32(); // 4   uint    Player.UniqueID
                var cape = (PVPCape)packet.ReadUInt8(); // 1   byte    Player.FRPVPMode
                if (session.SessionData.UniqueCharId == uniqueId)
                {
                    session.SessionData.State.PvpCape = cape;
                }
            }
            else if (result == 2)
            {
                packet.ReadUInt16(); // 2   ushort  errorCode
            }

            return new PacketResult();
        }
        private async Task<PacketResult> AGENT_RESPAWN(Packet packet, ISession session, object obj)
        {
            try
            {
                byte x = packet.ReadUInt8();

                if (AgentServer.eventManager.lastManStanding.m_LMSSetting.Count() > 0)
                {
                    if (session.SessionData.WorldID == AgentServer.eventManager.lastManStanding.m_LMSSetting[0].WorldID)
                    {
                        await sqlQueryHelper.EXEC_QUERY($"EXEC __TeleportToTownbyCharID {session.SessionData.Charid}");


                        return new PacketResult(packet, PacketResultType.Block);
                    }
                }



                if (AgentServer.eventManager.defendTower.m_DefendTowerSetting.Count() > 0)
                {
                    if (session.SessionData.WorldID == AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].WorldID)
                    {
                        if (AgentServer.eventManager.styriaClash.StyriaClashEventStatus)
                        {
                            if (AgentServer.eventManager.styriaClash.StyriaClashLogger.ContainsKey(session.SessionData.Charname))
                            {
                                if (AgentServer.eventManager.styriaClash.StyriaClashLogger[session.SessionData.Charname].Team == 1)
                                {
                                    //Packet packetx = new Packet(0x3536);
                                    //packetx.WriteUInt8(AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].WorldID);
                                    //packetx.WriteUInt16(AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].RegionIDThief);
                                    //packetx.WriteFloat(AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].StartPosXThief);
                                    //packetx.WriteFloat(AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].StartPosYThief);
                                    //packetx.WriteFloat(AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].StartPosZThief);
                                    //await session.SendToServer(packetx);

                                    await sqlQueryHelper.EXEC_QUERY($"EXEC __RessurrectCharacter {session.SessionData.Charid}");
                                    await sqlQueryHelper.EXEC_QUERY($"EXEC __TeleportToPosition {session.SessionData.Charid}, {AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].WorldID}, {AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].RegionIDThief}, " +
                                        $"{AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].StartPosXThief}, {AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].StartPosYThief}," +
                                        $"{ AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].StartPosZThief}");


                                    return new PacketResult(packet, PacketResultType.Block);
                                }
                                else if (AgentServer.eventManager.styriaClash.StyriaClashLogger[session.SessionData.Charname].Team == 3)
                                {
                                    //Packet packetx = new Packet(0x3536);
                                    //packetx.WriteUInt8(AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].WorldID);
                                    //packetx.WriteUInt16(AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].RegionIDHunter);
                                    //packetx.WriteFloat(AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].StartPosXHunter);
                                    //packetx.WriteFloat(AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].StartPosYHunter);
                                    //packetx.WriteFloat(AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].StartPosZHunter);
                                    //await session.SendToServer(packetx);

                                    await sqlQueryHelper.EXEC_QUERY($"EXEC __RessurrectCharacter {session.SessionData.Charid}");
                                    await sqlQueryHelper.EXEC_QUERY($"EXEC __TeleportToPosition {session.SessionData.Charid}, {AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].WorldID}, {AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].RegionIDHunter}, " +
                                        $"{AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].StartPosXHunter}, {AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].StartPosYHunter}," +
                                        $"{AgentServer.eventManager.styriaClash.m_StyriaClashSetting[0].StartPosZHunter}");

                                    return new PacketResult(packet, PacketResultType.Block);
                                }
                            }
                            else
                            {
                                Packet pck = new Packet(0x3535);
                                await session.SendToServer(pck);
                                return new PacketResult(packet, PacketResultType.Block);
                            }
                        }
                        else
                        {
                            Packet pck = new Packet(0x3535);
                            await session.SendToServer(pck);
                            return new PacketResult(packet, PacketResultType.Block);
                        }
                        return new PacketResult(packet, PacketResultType.Block);
                    }
                }



                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();

                    var parameterReturn = new SqlParameter("@ReturnValue", System.Data.SqlDbType.Int)
                    {
                        Direction = System.Data.ParameterDirection.Output
                    };

                    var command = new SqlCommand($"EXEC @ReturnValue = _OnCharacterGetUp_EDIT @CharID, @CharName, @LatestRegion, @LatestWorld, @PVPState; SELECT @ReturnValue;", connection);
                    command.Parameters.AddWithValue("@CharID", session.SessionData.Charid);
                    command.Parameters.AddWithValue("@CharName", session.SessionData.Charname);
                    command.Parameters.AddWithValue("@LatestRegion", session.SessionData.LatestRegion);
                    command.Parameters.AddWithValue("@LatestWorld", session.SessionData.WorldID);
                    command.Parameters.AddWithValue("@PVPState", Convert.ToByte(session.SessionData.State.PvpCape));


                    command.Parameters.Add(parameterReturn);

                    var result = await command.ExecuteScalarAsync();

                    var returnValue = (int)parameterReturn.Value;

                    if (returnValue == 0)
                    {
                        return new PacketResult(packet, PacketResultType.Block);
                    }
                    else
                    {
                        return new PacketResult(packet, PacketResultType.Nothing);
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message.ToString() + "AGENT_RESPAWN");
            }

            return new PacketResult(packet, PacketResultType.Nothing);

        }
        private async Task<PacketResult> CLIENT_AGENT_PARTY_INVITE_REQUEST(Packet packet, ISession session, object obj)
        {
            try
            {
                if (RefManager.m_RefEventMapSettings.ContainsKey(session.SessionData.LatestRegion))
                {
                    if (RefManager.m_RefEventMapSettings[session.SessionData.LatestRegion].DisableParty)
                    {
                        string noticeMessage = Management.GetNoticeMessage("MSG_PARTY_DISABLED");
                        Packet stMsg = new Packet(0x168A);
                        stMsg.WriteUInt8(NoticeType.WARNING);
                        stMsg.WriteUnicode(noticeMessage);
                        await session.SendToClient(stMsg);
                        return new PacketResult(PacketResultType.Block);
                    }
                }
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "CLIENT_AGENT_PARTY_MATCHING_FORM_REQUEST");
            }

            return new PacketResult();
        }
        private async Task<PacketResult> CLIENT_AGENT_PARTY_MATCHING_FORM_REQUEST(Packet packet, ISession session, object obj)
        {
            try
            {
                if (RefManager.m_RefEventMapSettings.ContainsKey(session.SessionData.LatestRegion))
                {
                    if (RefManager.m_RefEventMapSettings[session.SessionData.LatestRegion].DisableParty)
                    {
                        string noticeMessage = Management.GetNoticeMessage("MSG_PARTY_DISABLED");
                        Packet stMsg = new Packet(0x168A);
                        stMsg.WriteUInt8(NoticeType.WARNING);
                        stMsg.WriteUnicode(noticeMessage);
                        await session.SendToClient(stMsg);
                        return new PacketResult(PacketResultType.Block);
                    }
                }
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "CLIENT_AGENT_PARTY_MATCHING_FORM_REQUEST");
            }

            return new PacketResult();
        }
        private async Task<PacketResult> CLIENT_AGENT_PARTY_CREATE_REQUEST(Packet packet, ISession session, object obj)
        {
            if (RefManager.m_RefEventMapSettings.ContainsKey(session.SessionData.LatestRegion))
            {
                if (RefManager.m_RefEventMapSettings[session.SessionData.LatestRegion].DisableParty)
                {
                    string noticeMessage = Management.GetNoticeMessage("MSG_PARTY_DISABLED");
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }
            }

            return new PacketResult();
        }
        private async Task<PacketResult> CLIENT_AGENT_PARTY_MATCHING_JOIN_REQUEST(Packet packet, ISession session, object obj)
        {

            if (RefManager.m_RefEventMapSettings.ContainsKey(session.SessionData.LatestRegion))
            {
                if (RefManager.m_RefEventMapSettings[session.SessionData.LatestRegion].DisableParty)
                {
                    string noticeMessage = Management.GetNoticeMessage("MSG_PARTY_DISABLED");
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }
            }
            return new PacketResult();
        }
        private async Task<PacketResult> AGENT_INVENTORY_ITEM_USE(Packet packet, ISession session, object obj)
        {
            try
            {
                byte num = packet.ReadUInt8();
                uint value = packet.ReadUInt16();
                STypeID stTypeID = new STypeID((ushort)value);

                if (stTypeID.IsCOS)
                {
                    if (RefManager.m_RefEventMapSettings.ContainsKey(session.SessionData.LatestRegion))
                    {
                        if (RefManager.m_RefEventMapSettings[session.SessionData.LatestRegion].DisablePetSpawn)
                        {
                            string noticeMessage = Management.GetNoticeMessage("MSG_PET_SPAWN_IS_DISABLED");
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.WARNING);
                            stMsg.WriteUnicode(noticeMessage);
                            await session.SendToClient(stMsg);
                            return new PacketResult(PacketResultType.Block);
                        }
                    }
                }

                //Console.WriteLine(value.ToString());
                switch (value)
                {
                    #region itemmm
                    case 0xEED:
                    case 0xEEC:
                        {
                            //if (Dellay(session, session.SessionData.LAST_BOX_TIME, 2, "You're must be wait [{time}] for reuse scroll."))
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //session.SessionData.LAST_BOX_TIME = DateTime.Now;
                        }

                        break;
                    #endregion
                    #region BERSECK_POTION
                    case 0x40EC:
                        {
                            #region Disable berserk pot in fortress
                            //if (MainForm.ANTI_FORTRESS_ZERK_POTION)
                            //{
                            //    if (this.isFortress)
                            //    {
                            //     this.SendNotice(MainForm.FORTRESS_ZERK_POTION_NOTICE);
                            //     continue;
                            //    }
                            //}
                            #endregion

                            #region Disable berserk pot in job
                            //if (MainForm.ANTI_JOB_ZERK_POTION)
                            //{
                            //    if (this.isCharJob)
                            //    {
                            //     this.SendNotice(MainForm.JOB_ZERK_POTION_NOTICE);
                            //     continue;
                            //    }
                            //}
                            #endregion
                        }
                        break;
                    #endregion

                    #region FIX BUG TRADE PET2
                    case 0x08CD:
                    case 4300:
                        {
                            //if (HideNameRegions.ContainsKey(session.SessionData.LatestRegionId))
                            //{
                            //    SendMessageViaText(session, 3, $"LEXA_BLOCK_PET_SPAWN_NOTICE");
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //}
                            //if (session.SessionData.WorldID > 1 && session.SessionData.WorldID < 10)
                            //{
                            //    SendMessageViaText(session, 3, $"LEXA_BLOCK_PET_SPAWN_NOTICE_FORTRESS");
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //}
                            //if (___EventConfig[1].WorldID == session.SessionData.WorldID)
                            //{
                            //    SendMessageViaText(session, 3, $"LEXA_BLOCK_PET_SPAWN_NOTICE");
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //}
                            //if (___EventConfig[2].WorldID == session.SessionData.WorldID)
                            //{
                            //    SendMessageViaText(session, 3, $"LEXA_BLOCK_PET_SPAWN_NOTICE");
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //}
                            //if (___EventConfig[3].WorldID == session.SessionData.WorldID)
                            //{
                            //    SendMessageViaText(session, 3, $"LEXA_BLOCK_PET_SPAWN_NOTICE");
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //}
                            //if (___EventConfig[4].WorldID == session.SessionData.WorldID)
                            //{
                            //    SendMessageViaText(session, 3, $"LEXA_BLOCK_PET_SPAWN_NOTICE");
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //}
                            //foreach(var line in HideNameRegions)
                            //{
                            //    if(line.Value.RegionID == session.SessionData.LatestRegionId)
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //}
                        }
                        break;
                    #endregion

                    #region REVERSE SCROLL
                    case 0x19ED:
                    case 0x19EC:
                        {


                            uint num2 = packet.ReadUInt8();

                            //if (___EventConfig[1].WorldID == session.SessionData.WorldID)
                            //{
                            //    SendMessageViaText(session, 3, $"LEXA_EVENT_REVERSE_BLOCK2_NOTICE");
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //}
                            //if (___EventConfig[2].WorldID == session.SessionData.WorldID)
                            //{
                            //    SendMessageViaText(session, 3, $"LEXA_EVENT_REVERSE_BLOCK2_NOTICE");
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //}
                            //if (___EventConfig[3].WorldID == session.SessionData.WorldID)
                            //{
                            //    SendMessageViaText(session, 3, $"LEXA_EVENT_REVERSE_BLOCK2_NOTICE");
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //}
                            //if (___EventConfig[4].WorldID == session.SessionData.WorldID)
                            //{
                            //    SendMessageViaText(session, 3, $"LEXA_EVENT_REVERSE_BLOCK2_NOTICE");
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //}


                            #region REVERSE DISABLE IN JOBBING
                            if (Service.DisableReverseInJob)
                            {
                                if (session.SessionData.JobType != 4)
                                {
                                    string noticeMessage = Management.GetNoticeMessage("MSG_REVERSE_DISABLED_JOB_MODE");
                                    Packet stMsg = new Packet(0x168A);
                                    stMsg.WriteUInt8(NoticeType.WARNING);
                                    stMsg.WriteUnicode(noticeMessage);
                                    await session.SendToClient(stMsg);
                                    return new PacketResult(PacketResultType.Block);
                                }
                            }
                            #endregion

                            #region REVERSE DELAY


                            int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_REVERSE_TIME).TotalSeconds);
                            if (gecensaniye < Service.ReverseDelay)
                            {
                                int kalanSaniye = Service.ReverseDelay - gecensaniye;
                                string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_REVERSE_DELAY"), kalanSaniye);
                                Packet stMsg = new Packet(0x168A);
                                stMsg.WriteUInt8(NoticeType.WARNING);
                                stMsg.WriteUnicode(noticeMessage);
                                await session.SendToClient(stMsg);
                                return new PacketResult(PacketResultType.Block);
                            }
                            session.SessionData.LAST_REVERSE_TIME = DateTime.Now;
                            #endregion


                            if (num2 == 2)
                            {
                                //int deadworldID = QuerySide.prod_int($"SELECT TelWorldID FROM {Serverinfo.ShardDB}.dbo._Char with (nolock) where CharID = {session.SessionData.Charid}", ConnectionstringShardDatabase).Result;
                                //foreach (var line in ___EventConfig)
                                //{
                                //    if (line.Value.WorldID == deadworldID)
                                //    {
                                //        SendMessageViaText(session, 1, $"LEXA_EVENT_REVERSE_BLOCK_NOTICE");
                                //        return new PacketResult(packet, PacketResultType.Block);
                                //    }
                                //}
                            }
                            else if (num2 == 3)
                            {
                                //// int deaddregion = await sqlCon.Get_DeadRegionID_by_CharIDD(CharID);
                                //int deadworldID = QuerySide.prod_int($"SELECT DiedWorldID FROM {Serverinfo.ShardDB}.dbo._Char with (nolock) where CharID = {session.SessionData.Charid}", ConnectionstringShardDatabase).Result;
                                //foreach (var line in ___EventConfig)
                                //{
                                //    if (line.Value.WorldID == deadworldID)
                                //    {
                                //        SendMessageViaText(session, 1, $"LEXA_EVENT_REVERSE_BLOCK_NOTICE");
                                //        return new PacketResult(packet, PacketResultType.Block);
                                //    }
                                //}
                            }
                        }
                        break;
                    #endregion

                    #region TELEPORT TO TOWN SCROLL
                    case 0x09ED:
                    case 0x09EC:
                        {
                            //session.SessionData.CharacterInReturn = true;
                        }
                        break;
                    #endregion

                    #region GLOBAL SCROLL
                    case 0x29ED:
                    case 0x29EC:
                        {
                            //string message = packet.ReadAscii();

                            #region GLOBAL DELAY
                            try
                            {
                                if (RefManager.m_RefEventMapSettings.ContainsKey(session.SessionData.LatestRegion))
                                {
                                    if (RefManager.m_RefEventMapSettings[session.SessionData.LatestRegion].DisableChat)
                                    {
                                        string noticeMessage = Management.GetNoticeMessage("MSG_EVENT_ARENA_CHAT_DISABLED");
                                        Packet stMsg = new Packet(0x168A);
                                        stMsg.WriteUInt8(NoticeType.WARNING);
                                        stMsg.WriteUnicode(noticeMessage);
                                        await session.SendToClient(stMsg);
                                        return new PacketResult(PacketResultType.Block);
                                    }
                                }
                                int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_GLOBAL_TIME).TotalSeconds);
                                if (gecensaniye < Service.GlobalDelay)
                                {
                                    int kalanSaniye = Service.GlobalDelay - gecensaniye;
                                    string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_GLOBAL_CHAT_DELAY"), kalanSaniye);
                                    Packet stMsg = new Packet(0x168A);
                                    stMsg.WriteUInt8(NoticeType.WARNING);
                                    stMsg.WriteUnicode(noticeMessage);
                                    await session.SendToClient(stMsg);
                                    return new PacketResult(PacketResultType.Block);
                                }

                                if (session.SessionData.CurLevel < Service.GlobalLevel)
                                {
                                    string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_GLOBAL_CHAT_LEVEL"), Service.GlobalLevel);
                                    Packet stMsg = new Packet(0x168A);
                                    stMsg.WriteUInt8(NoticeType.WARNING);
                                    stMsg.WriteUnicode(noticeMessage);
                                    await session.SendToClient(stMsg);
                                    return new PacketResult(PacketResultType.Block);
                                }
                                session.SessionData.LAST_GLOBAL_TIME = DateTime.Now; // aga şu alta  bi cont ekleyip denesem :D içidme kalacak :D
                            }
                            catch { }
                            #endregion
                        }
                        break;
                    #endregion

                    #region RESCURRENT SCROLL
                    case 0x36ED:
                    case 0x36EC:
                        {
                            //if (___EventConfig[1].WorldID == session.SessionData.WorldID)
                            //{
                            //    SendMessageViaText(session, 3, $"UIIT_MSG_FORT_NOT_USE_ITEM");
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //}
                            //if (___EventConfig[2].WorldID == session.SessionData.WorldID)
                            //{
                            //    SendMessageViaText(session, 3, $"UIIT_MSG_FORT_NOT_USE_ITEM");
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //}
                            //if (___EventConfig[3].WorldID == session.SessionData.WorldID)
                            //{
                            //    SendMessageViaText(session, 3, $"UIIT_MSG_FORT_NOT_USE_ITEM");
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //}
                            //if (___EventConfig[4].WorldID == session.SessionData.WorldID)
                            //{
                            //    SendMessageViaText(session, 3, $"UIIT_MSG_FORT_NOT_USE_ITEM");
                            //    return new PacketResult(packet, PacketResultType.Block);
                            //}
                        }
                        break;
                    #endregion

                    #region BUG TT
                    case 0x11ED:
                    case 0x11EC:
                        {
                            try
                            {
                                int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_SPAWN_TIME).TotalSeconds);
                                if (gecensaniye < Service.TradePetSpawnDelay)
                                {
                                    int kalanSaniye = Service.TradePetSpawnDelay - gecensaniye;
                                    string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_TRADE_PET_DELAY"), kalanSaniye);
                                    Packet stMsg = new Packet(0x168A);
                                    stMsg.WriteUInt8(NoticeType.WARNING);
                                    stMsg.WriteUnicode(noticeMessage);
                                    await session.SendToClient(stMsg);
                                    return new PacketResult(PacketResultType.Block);
                                }
                                session.SessionData.LAST_SPAWN_TIME = DateTime.Now;
                            }
                            catch { }
                        }
                        break;
                    #endregion
                    case 0x5EED:
                    case 0x5EEC:
                        {

                            byte slot = packet.ReadUInt8();
                            //if (slot >= 13 && slot <= 109)
                            //{
                            //    Packet pck = new Packet(0x3517);
                            //    pck.WriteUInt8(num);
                            //    pck.WriteUInt8(slot);
                            //    await session.SendToServer(pck);
                            //}
                            int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_LIVE_ITEM_DELAY).TotalSeconds);
                            if (gecensaniye < Service.LiveItemDelay)
                            {
                                int kalanSaniye = Service.LiveItemDelay - gecensaniye;
                                string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_LIVE_ITEM_DELAY"), kalanSaniye);
                                Packet stMsg = new Packet(0x168A);
                                stMsg.WriteUInt8(NoticeType.WARNING);
                                stMsg.WriteUnicode(noticeMessage);
                                await session.SendToClient(stMsg);
                                return new PacketResult(PacketResultType.Block);
                            }
                            session.SessionData.LAST_LIVE_ITEM_DELAY = DateTime.Now;
                            
                            using (var connection = new SqlConnection(Program.Connectionstring))
                            {
                                await connection.OpenAsync();
                                string query = $"EXEC _OnSelectableScroll_Used_EDIT '{session.SessionData.Charname}', {session.SessionData.Charid}, {num}, {slot}";

                                using (var command = new SqlCommand(query, connection))
                                {
                                    await command.ExecuteNonQueryAsync();
                                }
                            }

                            //return new PacketResult(packet, PacketResultType.Block);
                        }
                        break;

                }

            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "AGENT_INVENTORY_ITEM_USE");
            }
            return new PacketResult(packet, PacketResultType.Nothing);
        }
        private async Task<PacketResult> AGENT_FRPVP_UPDATE(Packet packet, ISession session, PacketData data)
        {
            if (RefManager.m_RefEventMapSettings.ContainsKey(session.SessionData.LatestRegion))
            {
                if (RefManager.m_RefEventMapSettings[session.SessionData.LatestRegion].AutoCape)
                {
                    return new PacketResult(PacketResultType.Block);
                }
            }
            if (AgentServer.eventManager.defendTower.m_DefendTowerSetting.Count() > 0)
            {
                if (session.SessionData.WorldID == AgentServer.eventManager.defendTower.m_DefendTowerSetting[0].WorldID && !AgentServer.eventManager.defendTower.DefendTowerEventStatus)
                {
                    return new PacketResult(PacketResultType.Block);
                }
            }
            return new PacketResult();
        }
        private async Task<PacketResult> HandleChatReq(Packet packet, ISession session, object obj)
        {
            try
            {
                if (RefManager.m_RefEventMapSettings.ContainsKey(session.SessionData.LatestRegion))
                {
                    if (RefManager.m_RefEventMapSettings[session.SessionData.LatestRegion].DisableChat)
                    {
                        string noticeMessage = Management.GetNoticeMessage("MSG_EVENT_ARENA_CHAT_DISABLED");
                        Packet stMsg = new Packet(0x168A);
                        stMsg.WriteUInt8(NoticeType.WARNING);
                        stMsg.WriteUnicode(noticeMessage);
                        await session.SendToClient(stMsg);
                        return new PacketResult(PacketResultType.Block);
                    }
                }
            }
            catch { }

            return new PacketResult();
        }
        //private async Task<PacketResult> HandleChatReq(Packet packet, ISession session, object obj)
        //{
        //    try
        //    {

        //    }
        //    catch { }

        //    return new PacketResult();
        //}
        //private async Task<PacketResult> HandleChatReq(Packet packet, ISession session, object obj)
        //{
        //    try
        //    {

        //    }
        //    catch { }

        //    return new PacketResult();
        //}
        //private async Task<PacketResult> HandleChatReq(Packet packet, ISession session, object obj)
        //{
        //    try
        //    {

        //    }
        //    catch { }

        //    return new PacketResult();
        //}
        private async Task<PacketResult> AGENT_UNION_INVITE(Packet packet, ISession session, object obj)
        {
            try
            {

                int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_UNION_INVITE_TIME).TotalSeconds);
                if (gecensaniye < Service.UnionInviteDelay)
                {
                    int kalanSaniye = Service.UnionInviteDelay - gecensaniye;
                    string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_UNION_INVITE_DELAY"), kalanSaniye);
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }
                session.SessionData.LAST_UNION_INVITE_TIME = DateTime.Now;

            }
            catch { }

            return new PacketResult();
        }
        private async Task<PacketResult> AGENT_GUILD_INVITE(Packet packet, ISession session, object obj)
        {
            try
            {

                int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_GUILD_INVITE_TIME).TotalSeconds);
                if (gecensaniye < Service.GuildInviteDelay)
                {
                    int kalanSaniye = Service.GuildInviteDelay - gecensaniye;
                    string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_GUILD_INVITE_DELAY"), kalanSaniye);
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }
                session.SessionData.LAST_GUILD_INVITE_TIME = DateTime.Now;

            }
            catch { }

            return new PacketResult();

        }
        private async Task<PacketResult> AGENT_EXCHANGE_START(Packet packet, ISession session, object obj)
        {
            try
            {

                int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_EXCHANGE_TIME).TotalSeconds);
                if (gecensaniye < Service.ExchangeDelay)
                {
                    int kalanSaniye = Service.ExchangeDelay - gecensaniye;
                    string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_EXCHANGE_DELAY"), kalanSaniye);
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }

                if (session.SessionData.CurLevel < Service.ExchangeLevel)
                {
                    string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_EXCHANGE_LEVEL"), Service.ExchangeLevel);
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }
                session.SessionData.LAST_EXCHANGE_TIME = DateTime.Now;

            }
            catch { }

            return new PacketResult();
        }
        private async Task<PacketResult> AGENT_STALL_CREATE(Packet packet, ISession session, object obj)
        {
            string stallname = packet.ReadAscii();
            try
            {
                int gecensaniye = Convert.ToInt32(DateTime.Now.Subtract(session.SessionData.LAST_STALL_TIME).TotalSeconds);
                if (gecensaniye < Service.StallDelay)
                {
                    int kalanSaniye = Service.StallDelay - gecensaniye;
                    string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_STALL_DELAY"), kalanSaniye);
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }

                if (session.SessionData.CurLevel < Service.StallLevel)
                {
                    string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_STALL_LEVEL"), Service.StallLevel);
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }
                if (session.SessionData.OnTransport)
                {
                    string noticeMessage = Management.GetNoticeMessage("MSG_STALL_DISABLED_ON_TRANSPORT");
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }
                //foreach (var line in ServerEventRules)
                //{
                //    if (line.Value.EventWorldID == session.SessionData.WorldID)
                //    {
                //     SendMessage(session, 1, $"You're cannot create stall in special event arena.");
                //     continue;
                //    }
                //}
                session.SessionData.LAST_STALL_TIME = DateTime.Now;


            }
            catch { }


            return new PacketResult();
        }
        private async Task<PacketResult> AGENT_STALL_UPDATE(Packet packet, ISession session, object obj)
        {
            if (session.SessionData.OnTransport)
            {
                string noticeMessage = Management.GetNoticeMessage("MSG_STALL_DISABLED_ON_TRANSPORT");
                Packet stMsg = new Packet(0x168A);
                stMsg.WriteUInt8(NoticeType.WARNING);
                stMsg.WriteUnicode(noticeMessage);
                await session.SendToClient(stMsg);
                return new PacketResult(PacketResultType.Block);
            }
            return new PacketResult(packet, PacketResultType.Nothing);
        }

        private async Task<PacketResult> AGENT_TELEPORT_USE(Packet packet, ISession session, object obj)
        {
            if (packet.GetBytes().Length >= 9)
            {
                int UniqueID = packet.ReadInt32();
                byte TelType = packet.ReadUInt8();
                int targetTeleport = packet.ReadInt32();

                if (AgentServer.eventManager.LiveTeleportManagement.ContainsKey(targetTeleport))
                {
                    if (!AgentServer.eventManager.LiveTeleportManagement[targetTeleport])
                    {
                        string noticeMessage = Management.GetNoticeMessage("MSG_TELEPORT_CLOSED_AT_THIS_TIME");
                        Packet stMsg = new Packet(0x168A);
                        stMsg.WriteUInt8(NoticeType.WARNING);
                        stMsg.WriteUnicode(noticeMessage);
                        await session.SendToClient(stMsg);
                        return new PacketResult(PacketResultType.Block);
                    }
                }

                using (var connection = new SqlConnection(Program.Connectionstring))
                {
                    await connection.OpenAsync();
                    bool CanTeleport = true;

                    using (var command = new SqlCommand("[dbo].[_OnTeleportControl_EDIT]", connection))
                    {
                        command.CommandType = CommandType.StoredProcedure;

                        command.Parameters.AddWithValue("@CharID", session.SessionData.Charid);
                        command.Parameters.AddWithValue("@RefTeleportID", targetTeleport);

                        var returnValueParam = new SqlParameter("@CanTeleport", SqlDbType.Bit)
                        {
                            Direction = ParameterDirection.Output
                        };
                        command.Parameters.Add(returnValueParam);

                        await command.ExecuteNonQueryAsync();

                        CanTeleport = (bool)returnValueParam.Value;
                    }

                    if (CanTeleport)
                    {
                        return new PacketResult(packet, PacketResultType.Nothing);
                    }
                    else
                    {
                        return new PacketResult(packet, PacketResultType.Block);
                    }
                }

            }
            return new PacketResult();
        }
        private async Task<PacketResult> CLIENT_CHARACTER_ACTION_REQUEST(Packet packet, ISession session, object obj)
        {
            var unk1 = packet.ReadUInt8();
            if (unk1 != 0x01) return new PacketResult();

            var action = (CharacterAction)packet.ReadUInt8();
            if (action == CharacterAction.CommonAttack)
            {
                if (AgentServer.eventManager.LiveSkillManagementbyWorldID.ContainsKey(session.SessionData.WorldID))
                {
                    if (!AgentServer.eventManager.LiveSkillManagementbyWorldID[session.SessionData.WorldID])
                    {
                        string noticeMessage = Management.GetNoticeMessage("MSG_SKILL_USAGE_CLOSED_AT_THIS_TIME");
                        Packet stMsg = new Packet(0x168A);
                        stMsg.WriteUInt8(NoticeType.WARNING);
                        stMsg.WriteUnicode(noticeMessage);
                        await session.SendToClient(stMsg);
                        return new PacketResult(PacketResultType.Block);
                    }
                }
                if (AgentServer.eventManager.LiveSkillManagementbyRegionID.ContainsKey(session.SessionData.LatestRegion))
                {
                    if (!AgentServer.eventManager.LiveSkillManagementbyRegionID[session.SessionData.LatestRegion])
                    {
                        string noticeMessage = Management.GetNoticeMessage("MSG_SKILL_USAGE_CLOSED_AT_THIS_TIME");
                        Packet stMsg = new Packet(0x168A);
                        stMsg.WriteUInt8(NoticeType.WARNING);
                        stMsg.WriteUnicode(noticeMessage);
                        await session.SendToClient(stMsg);
                        return new PacketResult(PacketResultType.Block);
                    }
                }
              
                if (Service.DisableAutoAttack)
                {
                    if (!(session.SessionData.WorldID >= 2 && session.SessionData.WorldID <= 9))
                    {
                        string noticeMessage = Management.GetNoticeMessage("MSG_AUTO_ATTACK_DISABLED");
                        Packet stMsg = new Packet(0x168A);
                        stMsg.WriteUInt8(NoticeType.WARNING);
                        stMsg.WriteUnicode(noticeMessage);
                        await session.SendToClient(stMsg);
                        return new PacketResult(PacketResultType.Block);
                    }
                }
                if (Service.AutoAttackMaxLevel > 0)
                {
                    if (!(session.SessionData.WorldID >= 2 && session.SessionData.WorldID <= 9))
                    {
                        if (Service.AutoAttackMaxLevel < session.SessionData.CurLevel)
                        {
                            string noticeMessage = string.Format(Management.GetNoticeMessage("MSG_AUTO_ATTACK_DISABLED_LEVEL"), Service.AutoAttackMaxLevel);
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.WARNING);
                            stMsg.WriteUnicode(noticeMessage);
                            await session.SendToClient(stMsg);
                            return new PacketResult(PacketResultType.Block);

                        }
                    }
                }

            }
            else if (action == CharacterAction.Trace)
            {
                if (Service.DisableTraceWhileJob)
                {
                    if (session.SessionData.JobType != 4)
                    {
                        string noticeMessage = Management.GetNoticeMessage("MSG_TRACE_DISABLED_WHILE_JOB_MODE");
                        Packet stMsg = new Packet(0x168A);
                        stMsg.WriteUInt8(NoticeType.WARNING);
                        stMsg.WriteUnicode(noticeMessage);
                        await session.SendToClient(stMsg);
                        return new PacketResult(PacketResultType.Block);
                    }
                }
                if (RefManager.m_RefEventMapSettings.ContainsKey(session.SessionData.LatestRegion))
                {
                    if (RefManager.m_RefEventMapSettings[session.SessionData.LatestRegion].DisableTrace)
                    {
                        string noticeMessage = Management.GetNoticeMessage("MSG_TRACE_DISABLED");
                        Packet stMsg = new Packet(0x168A);
                        stMsg.WriteUInt8(NoticeType.WARNING);
                        stMsg.WriteUnicode(noticeMessage);
                        await session.SendToClient(stMsg);
                        return new PacketResult(PacketResultType.Block);
                    }
                }
            }
            else if (action == CharacterAction.SkillCast)
            {
                var skillId = (int)packet.ReadUInt32();
                byte action2 = packet.ReadUInt8(); // el yakmada 0 skill de 1 geliyor
                if (action2 == 0)
                {
                    if (session.SessionData.LastBuffUsage + 150 > DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() || session.SessionData.LastSkillUsage + 150 > DateTimeOffset.UtcNow.ToUnixTimeMilliseconds())
                    {
                        return new PacketResult(PacketResultType.Block);
                    }
                    session.SessionData.LastBuffUsage = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();
                }
                if (action2 == 1)
                {
                    if (session.SessionData.LastSkillUsage + 150 > DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() || session.SessionData.LastBuffUsage + 150 > DateTimeOffset.UtcNow.ToUnixTimeMilliseconds())
                    {
                        return new PacketResult(PacketResultType.Block);
                    }
                    session.SessionData.LastSkillUsage = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();

                    if (AgentServer.eventManager.LiveSkillManagementbyWorldID.ContainsKey(session.SessionData.WorldID))
                    {
                        if (!AgentServer.eventManager.LiveSkillManagementbyWorldID[session.SessionData.WorldID])
                        {
                            string noticeMessage = Management.GetNoticeMessage("MSG_SKILL_USAGE_CLOSED_AT_THIS_TIME");
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.WARNING);
                            stMsg.WriteUnicode(noticeMessage);
                            await session.SendToClient(stMsg);
                            return new PacketResult(PacketResultType.Block);
                        }
                    }
                    if (AgentServer.eventManager.LiveSkillManagementbyRegionID.ContainsKey(session.SessionData.LatestRegion))
                    {
                        if (!AgentServer.eventManager.LiveSkillManagementbyRegionID[session.SessionData.LatestRegion])
                        {
                            string noticeMessage = Management.GetNoticeMessage("MSG_SKILL_USAGE_CLOSED_AT_THIS_TIME");
                            Packet stMsg = new Packet(0x168A);
                            stMsg.WriteUInt8(NoticeType.WARNING);
                            stMsg.WriteUnicode(noticeMessage);
                            await session.SendToClient(stMsg);
                            return new PacketResult(PacketResultType.Block);
                        }
                    }
                }

                //foreach (var line in BlockSkillUsagebyRegionID)
                //{
                //    if (line.Value.SkillID == skillId && line.Value.RegionID == session.SessionData.LatestRegionId)
                //    {
                //        SendMessageViaText(session, 3, "VFILTER_NOTICE_REGION_SKILL_BLOCK");
                //        return new PacketResult(PacketResultType.Block);
                //    }
                //}

                //if (session.SessionData.JobType != 4)
                //{
                //    foreach (var line in BlockSkillUsageWhileJob)
                //    {
                //        if (line.Value.SkillID == skillId)
                //        {
                //            SendMessageViaText(session, 3, "VFILTER_NOTICE_JOB_SKILL_BLOCK");
                //            return new PacketResult(PacketResultType.Block);
                //        }
                //    }
                //}

            }
            else if (action == CharacterAction.SkillRemove)
            {
                var skillId = (int)packet.ReadUInt32();
                //foreach (var line in ServerBlockRemoveBuff)
                //{
                //    if (line.Value.SkillID == skillId)
                //    {
                //        SendMessageViaText(session, 3, "LEXA_BLOCK_CUSTOM_SKILL_CLOSE_NOTICE");
                //        return new PacketResult(PacketResultType.Block);
                //    }
                //}
            }
            return new PacketResult();
        }
        private async Task<PacketResult> SERVER_LEVELINFO(Packet packet, ISession session, object obj)
        {
            uint PlayerUniqueID = packet.ReadUInt32();
            if (PlayerUniqueID == session.SessionData.UniqueCharId)
            {
                session.SessionData.CurLevel += 1;
                //Console.WriteLine($"{CHARNAME16} has leveled up.", Utils.LOG_TYPE.Special);
            }
            return new PacketResult(packet, PacketResultType.Nothing);
        }
        private async Task<PacketResult> AGENT_ACADEMY_MATCHING_JOIN(Packet packet, ISession session, object obj)
        {
            try
            {
                if (Service.DisableAcademy)
                {

                    string noticeMessage = Management.GetNoticeMessage("MSG_ACADEMY_DISABLED");
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }
            }
            catch { }

            return new PacketResult();
        }
        private async Task<PacketResult> AGENT_ACADEMY_CREATE2(Packet packet, ISession session, object obj)
        {
            try
            {
                if (Service.DisableAcademy)
                {

                    string noticeMessage = Management.GetNoticeMessage("MSG_ACADEMY_DISABLED");
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }
            }
            catch { }

            return new PacketResult();
        }
        private async Task<PacketResult> AGENT_ACADEMY_CREATE(Packet packet, ISession session, object obj)
        {
            try
            {
                if (Service.DisableAcademy)
                {
                    string noticeMessage = Management.GetNoticeMessage("MSG_ACADEMY_DISABLED");
                    Packet stMsg = new Packet(0x168A);
                    stMsg.WriteUInt8(NoticeType.WARNING);
                    stMsg.WriteUnicode(noticeMessage);
                    await session.SendToClient(stMsg);
                    return new PacketResult(PacketResultType.Block);
                }
            }
            catch { }

            return new PacketResult();
        }

    }
}
