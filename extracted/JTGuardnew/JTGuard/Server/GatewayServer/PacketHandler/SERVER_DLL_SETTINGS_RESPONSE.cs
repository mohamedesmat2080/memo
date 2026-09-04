using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.NetworkInformation;
using System.Reflection.PortableExecutable;
using System.Text;
using System.Threading.Tasks;
using JTGuard.Database;
using JTGuard.Database.Model;
using JTGuard.Helpers;
using JTGuard.PacketHandlerManager;
using JTGuard.ServerManagers;
using JTGuard.SessionManager;
using Microsoft.IdentityModel.Tokens;
using SilkroadSecurityAPI;

namespace JTGuard.Server.GatewayPacketHandler
{
    public partial class SERVER_DLL_SETTINGS_RESPONSE
    {

        public _ServerSettings Service { get; set; }
        public SERVER_DLL_SETTINGS_RESPONSE(_ServerSettings service, GatewayServer gatewayServer, IPacketHandler packetHandler)
        {
            Service = service;
            packetHandler.RegisterClientHandler(0x165B, CLIENT_HWID_REQUEST);
            packetHandler.RegisterClientHandler(0x1211, CLIENT_SECONDARY_PASSWORD_REQUEST);
            packetHandler.RegisterClientHandler(0x6102, CLIENT_LOGIN_REQUEST);
            packetHandler.RegisterClientHandler(0xA150, SERVER_SENDDLLSETTINGS);
            packetHandler.RegisterModuleHandler(0xA101, SERVER_SHARD_INFO); // Replaces the shard info
            

        }
        private async Task<PacketResult> CLIENT_HWID_REQUEST(Packet packet, ISession session, object obj)
        {
            try
            {
                string Hwid = packet.ReadAscii();
                if(!Hwid.EndsWith("1999"))
                {
                    string noticeMessage = Management.GetNoticeMessage("HWID_ERROR");
                    Packet pck = new Packet(0xA340);
                    pck.WriteUnicode(noticeMessage);
                    await session.SendToClient(pck);
                    return new PacketResult(PacketResultType.Block);
                }
                else
                {
                    string noticeMessage = Management.GetNoticeMessage("HWID_SUCCES");
                    session.SessionData.Hwid = Hwid;
                    Packet pck = new Packet(0xA340);
                    pck.WriteUnicode(noticeMessage);
                    await session.SendToClient(pck);
                    return new PacketResult(PacketResultType.Block);
                }
                return new PacketResult(PacketResultType.Block);
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "CLIENT_HWID_REQUEST");
                return new PacketResult(PacketResultType.Block);
            }
        }

        private async Task<PacketResult> CLIENT_SECONDARY_PASSWORD_REQUEST(Packet packet, ISession session, object obj)
        {
            try
            {
                byte RequestType = packet.ReadUInt8();
                if (session.SessionData.Hwid.IsNullOrEmpty())
                {
                    string noticeMessage = Management.GetNoticeMessage("HWID_ERROR");
                    Packet pck = new Packet(0xA340);
                    pck.WriteUnicode(noticeMessage); // CREATE THE PASSWORD
                    await session.SendToClient(pck);
                    return new PacketResult(PacketResultType.Block);
                }
                if (RequestType == (byte)SecondaryPasswordRequest.CREATE_PASSWORD)
                {
                    int SelectedPassword = packet.ReadInt32();
                    var secondaryPasswordData = await sqlQueryHelper.GetSecondaryPasswordData(session.SessionData.user_id);

                    if (secondaryPasswordData == null)
                    {

                        await sqlQueryHelper.InsertSecondaryPasswordData(session.SessionData.user_id, SelectedPassword, session.SessionData.Hwid, false);
                        session.SessionData.SecondaryPassword = SelectedPassword;
                        Packet pck = new Packet(0x1212);
                        pck.WriteUInt8(SecondaryPasswordResponse.CREATED_SUCCESS_ENTER_THE_PASSWORD); // CREATE THE PASSWORD
                        await session.SendToClient(pck);
                        return new PacketResult(PacketResultType.Block);
                    }
                }
                else if (RequestType == (byte)SecondaryPasswordRequest.CHANGE_PASSWORD)
                {
                    int SelectedPassword = packet.ReadInt32();
                    int TargetPassword = packet.ReadInt32();

                    if (session.SessionData.SecondaryPassword == SelectedPassword)
                    {
                        session.SessionData.SecondaryPassword = TargetPassword;
                        await sqlQueryHelper.UpdateSecondaryPasswordData(session.SessionData.user_id, TargetPassword, false);
                        Packet pck = new Packet(0x1212);
                        pck.WriteUInt8(SecondaryPasswordResponse.PASSWORD_CHANGED_ENTER_THE_PASSWORD); // CREATE THE PASSWORD
                        await session.SendToClient(pck);
                        return new PacketResult(PacketResultType.Block);
                    }
                    else
                    {
                        Packet pck = new Packet(0x1212);
                        pck.WriteUInt8(SecondaryPasswordResponse.WRONG_PASSWORD);
                        await session.SendToClient(pck);
                        return new PacketResult(PacketResultType.Block);
                    }
                }
                else if (RequestType == (byte)SecondaryPasswordRequest.ENTER_THE_PASSWORD)
                {
                    int SelectedPassword = packet.ReadInt32();
                    bool RememberPC = packet.ReadBool();

                    if (SelectedPassword == session.SessionData.SecondaryPassword)
                    {
                        await sqlQueryHelper.UpdateSecondaryPasswordData(session.SessionData.user_id, SelectedPassword, RememberPC);
                        Packet pck = new Packet(0x1212);
                        pck.WriteUInt8(SecondaryPasswordResponse.PW_TRUE);
                        await session.SendToClient(pck);


                        Packet pcka = new Packet(0x6102, true, false);
                        pcka.WriteUInt8(session.SessionData.locale);
                        pcka.WriteAscii(session.SessionData.user_id);
                        pcka.WriteAscii(session.SessionData.user_pw); 
                        pcka.WriteUInt16(session.SessionData.ServerID);
                        await session.SendToServer(pcka);


                        return new PacketResult(PacketResultType.Block);
                    }
                    else
                    {
                        Packet pck = new Packet(0x1212);
                        pck.WriteUInt8(SecondaryPasswordResponse.WRONG_PASSWORD); // CREATE THE PASSWORD
                        await session.SendToClient(pck);
                        return new PacketResult(PacketResultType.Block);
                    }
                }
                return new PacketResult(PacketResultType.Block);
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "CLIENT_SECONDARY_PASSWORD_REQUEST");
                return new PacketResult(PacketResultType.Block);
            }

        }
        private async Task<PacketResult> CLIENT_LOGIN_REQUEST(Packet packet, ISession session, object obj)
        {
            try
            {
                byte locale = packet.ReadUInt8();
                string user_id = packet.ReadAscii().ToLower();
                string user_pw = packet.ReadAscii();
                ushort ServerID = packet.ReadUInt16();

                if(session.SessionData.Hwid.IsNullOrEmpty())
                {
                    string noticeMessage = Management.GetNoticeMessage("HWID_ERROR");
                    Packet pck = new Packet(0xA340);
                    pck.WriteUnicode(noticeMessage);
                    await session.SendToClient(pck);
                    return new PacketResult(PacketResultType.Block);
                }

                bool loginSuccess = await sqlQueryHelper.ValidateUserCredentials(user_id, user_pw);

                if (!loginSuccess)
                {
                    return new PacketResult(PacketResultType.Nothing);
                }
                else
                {
                    session.SessionData.locale = locale;
                    session.SessionData.user_id = user_id;
                    session.SessionData.user_pw = user_pw;
                    session.SessionData.ServerID = ServerID;

                    var secondaryPasswordData = await sqlQueryHelper.GetSecondaryPasswordData(user_id);

                    if (secondaryPasswordData != null)
                    {
                        int ID = secondaryPasswordData.ID;
                        string StrUserID = secondaryPasswordData.StrUserID;
                        int Password = secondaryPasswordData.Password;
                        string Hwid = secondaryPasswordData.Hwid;
                        bool RememberPC = secondaryPasswordData.RememberPC;

                        if(RememberPC)
                        {
                            if(Hwid == session.SessionData.Hwid)
                            {
                                return new PacketResult(PacketResultType.Nothing);
                            }
                        }

                        session.SessionData.SecondaryPassword = Password;
                        
                        Packet pck = new Packet(0x1212);
                        pck.WriteUInt8(SecondaryPasswordResponse.ENTER_THE_PASSWORD); // ENTER THE PASSWORD
                        await session.SendToClient(pck);
                        return new PacketResult(PacketResultType.Block);
                    }
                    else
                    {
                        Packet pck = new Packet(0x1212);
                        pck.WriteUInt8(SecondaryPasswordResponse.CREATE_THE_PASSWORD); // CREATE THE PASSWORD
                        await session.SendToClient(pck);
                        return new PacketResult(PacketResultType.Block);
                    }
                }

                return new PacketResult(PacketResultType.Nothing);
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "CLIENT_LOGIN_REQUEST");
                return new PacketResult(PacketResultType.Nothing);
            }
        }
        private async Task<PacketResult> SERVER_SENDDLLSETTINGS(Packet packet, ISession session, object obj)
        {
            try
            {
                Packet dc = new Packet(0x1210, true);
                dc.WriteBool(Service.OldLogin);
                dc.WriteBool(Service.OldExpBar);
                dc.WriteBool(Service.OldAlchemy);
                dc.WriteBool(Service.GrantNameButton);
                dc.WriteBool(Service.IconManagerButton);
                dc.WriteBool(Service.IconManagerRight);
                dc.WriteBool(Service.TitleManager);
                dc.WriteBool(Service.TitleManagerColor);
                dc.WriteBool(Service.DynamicRanking);
                dc.WriteBool(Service.UniqueHistory);
                dc.WriteBool(Service.EventRegister);
                dc.WriteBool(Service.EventSchedule);
                dc.WriteBool(Service.Achievements);
                dc.WriteBool(Service.SecondarySlot);
                dc.WriteBool(Service.MoveSkillBoard);
                dc.WriteBool(Service.ServerInfoSkill);
                dc.WriteBool(Service.OldMainPopup);
                dc.WriteBool(Service.HideOldTitleWhileNewTitle);
                dc.WriteBool(Service.ItemComparison);
                dc.WriteBool(Service.AutoSort);
                dc.WriteBool(Service.PartyMemberViewer);
                dc.WriteBool(Service.AutoSkillUpdate);
                dc.WriteInt32(Service.MasteryLimit);
                dc.WriteUInt8(Service.ServerMaxLevel);
                dc.WriteBool(Service.FixDamageText);
                dc.WriteBool(Service.AutoStrInt);
                dc.WriteBool(Service.PickupEffect);
                dc.WriteBool(Service.PermanentAlchemy);
                dc.WriteBool(Service.ShowGuildInJobMode);
                dc.WriteBool(Service.UniqueTarget);
                dc.WriteBool(Service.Macro);
                dc.WriteBool(Service.SecondaryPassword);
                dc.WriteBool(Service.NewCharInfo);
                dc.WriteBool(Service.NewIdPw);
                dc.WriteAscii(Service.FacebookURL);
                dc.WriteAscii(Service.DiscordURL);
                dc.WriteAscii(Service.WebsiteURL);
                dc.WriteBool(Service.Changelog);
                dc.WriteBool(Service.ShowChangelogFirstSpawn);
                dc.WriteBool(Service.FixNewJobSuit);
                dc.WriteBool(Service.OldItemMall);
                dc.WriteBool(Service.InsertCommaPrices);
                dc.WriteBool(Service.WriteCharacterBound);
                dc.WriteBool(Service.NewItemMall);
                dc.WriteBool(Service.EmojiSystem);
                dc.WriteBool(Service.NewPartyMatch);
                dc.WriteBool(Service.NewJobUI);
                dc.WriteBool(Service.NewAlchemy);
                dc.WriteBool(Service.EnableItemTranslation);
                dc.WriteUInt8(Service.ItemTranslationPayment);
                dc.WriteInt32(Service.ItemTranslationPrice);

                await session.SendToClient(dc);


                if (RefManager.m_RefNewAvatarMall.Count() > 0)
                {
                    Packet stAckMsg = new Packet(0x209B);
                    stAckMsg.WriteInt32(RefManager.m_RefNewAvatarMall.Count());
                    foreach (var data in RefManager.m_RefNewAvatarMall)
                    {
                        stAckMsg.WriteInt32(data.Value.ID);
                        stAckMsg.WriteUInt8(data.Value.CategoryID);
                        stAckMsg.WriteInt32(data.Value.ItemID);
                        stAckMsg.WriteInt32(data.Value.Silk);
                        stAckMsg.WriteInt32(data.Value.ItemIndex);
                        stAckMsg.WriteInt32(data.Value.PetObjID);
                    }
                    await session.SendToClient(stAckMsg);
                }


                if (RefManager.m_RefNewItemMall.Count() > 0)
                {
                    Packet stAckMsg = new Packet(0x208F);
                    stAckMsg.WriteInt32(RefManager.m_RefNewItemMall.Count());
                    foreach (var data in RefManager.m_RefNewItemMall)
                    {
                        stAckMsg.WriteInt32(data.Value.ID);
                        stAckMsg.WriteAscii(data.Value.CategoryName);
                        stAckMsg.WriteUInt8(data.Value.Type);
                        stAckMsg.WriteInt32(data.Value.ItemID);
                        stAckMsg.WriteInt32(data.Value.ItemCount);
                        stAckMsg.WriteInt32(data.Value.Silk);
                        stAckMsg.WriteUInt8(data.Value.ShowInNewBest);
                        stAckMsg.WriteInt32(data.Value.ItemIndex);
                        stAckMsg.WriteInt32(data.Value.ShowInNewBestIndex);

                    }
                    await session.SendToClient(stAckMsg);
                }



                return new PacketResult(packet, PacketResultType.Block);
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message.ToString() + "SERVER_SENDDLLSETTINGS");
                return new PacketResult(packet, PacketResultType.Block);
            }
           
    
        }
        private async Task<PacketResult> SERVER_SHARD_INFO(Packet packet, ISession session, object obj)
        {

            Packet SERVER_GATEWAY_SHARD_LIST_RESPONSE = new Packet(packet.Opcode, packet.Encrypted, packet.Massive);
            byte GlobalOperationFlag = packet.ReadUInt8();
            SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt8(GlobalOperationFlag);
            Packet pck = new Packet(0x1215);
            pck.WriteUInt8(GlobalOperationFlag);

            if (GlobalOperationFlag == 1)
            {
                byte GlobalOperationType = packet.ReadUInt8();
                string GlobalOperationName = packet.ReadAscii();
                GlobalOperationFlag = packet.ReadUInt8();

                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt8(GlobalOperationType);
                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteAscii(GlobalOperationName);
                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt8(GlobalOperationFlag);
                pck.WriteUInt8(GlobalOperationType);
                pck.WriteAscii(GlobalOperationName);
                pck.WriteUInt8(GlobalOperationFlag);

            }
            byte ShardFlag = packet.ReadUInt8();
            pck.WriteUInt8(ShardFlag);
            SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt8(ShardFlag);
            while (ShardFlag == 1)
            {
                uint ShardID = packet.ReadUInt16();
                string ShardName = packet.ReadAscii();
                uint ShardCurrent = packet.ReadUInt16();

                uint ShardCapacity = packet.ReadUInt16();
                byte ShardStatus = packet.ReadUInt8();
                byte GlobalOperationID = packet.ReadUInt8();

                ShardFlag = packet.ReadUInt8();



                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt16(ShardID);
                pck.WriteUInt16(ShardID);
                if (!Service.CheckStatus)
                {
                    if (Service.ShowOnlinePlayers)
                    {

                        SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteAscii(Service.ServerName + "   " + (Service.FakePlayerCount + ShardCurrent) + "/" + ShardCapacity);

                    }
                    else
                    {
                        SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteAscii(Service.ServerName);
                    }
                    pck.WriteAscii(Service.ServerName);
                    SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt16(ShardCurrent + Service.FakePlayerCount);
                    pck.WriteUInt16(ShardCurrent + Service.FakePlayerCount);


                    SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt16(ShardCapacity);
                    pck.WriteUInt16(ShardCapacity);


                    SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt8(ShardStatus);
                    pck.WriteUInt8(ShardStatus);
                }
                else
                {
                    if (Service.ShowOnlinePlayers)
                    {

                        SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteAscii(Service.ServerName + "   " + (0) + "/" + ShardCapacity);

                    }
                    else
                    {
                        SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteAscii(Service.ServerName);
                    }
                    pck.WriteAscii(Service.ServerName);
                    SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt16(0);
                    pck.WriteUInt16(0);

                    SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt16(ShardCapacity);
                    pck.WriteUInt16(ShardCapacity);

                    SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt8(0);
                    pck.WriteUInt8(0);
                }

                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt8(GlobalOperationID);
                SERVER_GATEWAY_SHARD_LIST_RESPONSE.WriteUInt8(ShardFlag);
                pck.WriteUInt8(GlobalOperationID);
                pck.WriteUInt8(ShardFlag);
                await session.SendToClient(pck);
            }



            if(session.SessionData.Hwid.IsNullOrEmpty())
            {
                Packet hwid = new Packet(0x165A);
                await session.SendToClient(hwid);
            }
            return new PacketResult(SERVER_GATEWAY_SHARD_LIST_RESPONSE, PacketResultType.Override);
        }
    }
}
