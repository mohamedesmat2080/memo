#region

using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Linq;
using System.Threading.Tasks;
using API;
using API.Model;
using API.Database.DuckSoup;
using API.Database.SRO_VT_ACCOUNT;
using API.Database.SRO_VT_SHARD;
using API.Server;
using API.ServiceFactory;
using API.Session;
using DuckSoup.Library.Server;
using PacketLibrary;
using PacketLibrary.Agent.Client;
using SilkroadSecurityAPI;
using NCrontab;
using System.Text;
using System.Data.Entity.Validation;
using API.Models;

#endregion

// ReSharper disable UnusedVariable

#pragma warning disable 1998

namespace DuckSoup.Agent;

public class AgentServer : AsyncServer
{
    public AgentServer(Service service) : base(service)
    {
        SharedObjects = ServiceFactory.Load<ISharedObjects>(typeof(ISharedObjects));
        using (var context = new API.Database.DuckSoup.DuckSoup())
        {
            if (!context.Whitelist.Any(s => s.ServerType == ServerType.AgentServer))
            {
                foreach (var (key, value) in DefaultPacketlist.AgentClientWhitelistFull)
                {
                    context.Whitelist.Add(new Whitelist
                    { MsgId = key, Comment = value, ServerType = ServerType.AgentServer });
                }

                context.SaveChanges();
            }

            // Conversion shit because Database actually only supports int not ushort sadge
            var temp1 = context.Whitelist.Where(s => s.ServerType == ServerType.AgentServer).Select(s => s.MsgId)
                .ToList();
            var temp2 = context.Blacklist.Where(s => s.ServerType == ServerType.AgentServer).Select(s => s.MsgId)
                .ToList();
            var temp3 = new HashSet<ushort>();
            var temp4 = new HashSet<ushort>();
            foreach (var i in temp1)
            {
                temp3.Add(ushort.Parse(i.ToString()));
            }

            foreach (var i in temp2)
            {
                temp4.Add(ushort.Parse(i.ToString()));
            }

            PacketHandler = new PacketHandler(temp3, temp4);
        }


        // Register all handlers here
        // Mainly Exploits
        PacketHandler.RegisterClientHandler(0x7001,
            AGENT_CHARACTER_SELECTION_JOIN); // charname modify and not really logged in Exploit - 0x7001 - https://www.elitepvpers.com/forum/sro-pserver-guides-releases/4232366-release-disconnect-players-exploit-found-iwa-4.html 
        PacketHandler.RegisterClientHandler(0x705E,
            AGENT_SIEGE_ACTION); // SQL Injection - 0x705E - Also contains Tax / checkout checks - https://www.elitepvpers.com/forum/sro-private-server/4141360-information-sql-injection-ingame.html
        PacketHandler.RegisterClientHandler(0x70F9,
            AGENT_GUILD_UPDATE_NOTICE); // Guild Notice - 0x70F9 - Better safe than sorry.
        PacketHandler.RegisterClientHandler(0x34A9,
            AGENT_MAGICOPTION_GRANT); // Avatar Exploit - 0x34A9 - https://www.elitepvpers.com/forum/sro-pserver-guides-releases/3991992-release-invincible-avatar-magopt-exploit-3.html
        PacketHandler.RegisterClientHandler(0x7005,
            AGENT_LOGOUT); // [x] Crash Exploit - 0x7005 - https://www.elitepvpers.com/forum/sro-pserver-guides-releases/4232366-release-disconnect-players-exploit-found-iwa.html
        PacketHandler.RegisterClientHandler(0x7007, CLIENT_AGENT_CHARACTER_SELECTION_ACTION_REQUEST); // same as above
        PacketHandler.RegisterClientHandler(0x70A7,
            CLIENT_PLAYER_BERSERK); // Zerk Exploit - 0x70A7 - https://www.elitepvpers.com/forum/sro-pserver-guides-releases/3991992-release-invincible-avatar-magopt-exploit-3.html
        PacketHandler.RegisterClientHandler(0x70A2,
            AGENT_SKILL_MASTERY_LEARN); // Skill Exploit - 0x70A2 - https://www.maxigame.com/forum/t/251583-meshur-vsro-mastery-exploit-ini-delirius-engelleme
        PacketHandler.RegisterClientHandler(0x3510,
            CLIENT_EXPLOIT_GSCRASH); // GS Crash Exploit - 0x3510 - https://www.elitepvpers.com/forum/sro-pserver-guides-releases/4383384-release-fix-gameserver-crash-runtime-error-exploit.html
        PacketHandler.RegisterModuleHandler(0xA103, AGENT_AUTH); // Exploit Prevention

        PacketHandler.RegisterClientHandler(0x7074, CLIENT_CHARACTER_ACTION_REQUEST); // Snow Shield fix

        // Mainly Data / Information
        PacketHandler.RegisterModuleHandler(0x3013, SERVER_AGENT_CHARACTER_DATA); // Character Spawn Packet
        PacketHandler.RegisterClientHandler(0x3012, AGENT_GAME_READY); // GameReady true
        PacketHandler.RegisterClientHandler(0x705A, AGENT_TELEPORT_USE); // GameReady false
        PacketHandler.RegisterModuleHandler(0x3020, AGENT_ENVIRONMENT_CELESTIAL_POSITION); // CharacterUniqueId
        PacketHandler.RegisterModuleHandler(0x30BF, SERVER_ENTITY_STATE_UPDATE);
        PacketHandler.RegisterModuleHandler(0xB021, AGENT_MOVEMENT_SERVER);

        //Features

        //Item Link
        PacketHandler.RegisterClientHandler(0x180A, CLIENT_ITEM_CHAT_LINKING);
        PacketHandler.RegisterClientHandler(0x180B, CLIENT_ITEM_CHAT_LINKING_REQUEST);

        //Stall Network
        PacketHandler.RegisterClientHandler(0x70BA, CLIENT_STALL_ACTION);
        PacketHandler.RegisterClientHandler(0x70b5, CLIENT_STALL_LEAVE);
        PacketHandler.RegisterClientHandler(0x7461, CLIENT_STALL_NETWORK_SEARCH);
        PacketHandler.RegisterClientHandler(0x7462, CLIENT_STALL_NETWORK_CLOSE);
        PacketHandler.RegisterClientHandler(0x7463, CLIENT_STALL_NETWORK_PURCHASE);
        PacketHandler.RegisterModuleHandler(0x30b7, SERVER_STALL_ACTION);

        //char ranking
        PacketHandler.RegisterClientHandler(0x180C, CLIENT_CHAR_RANKING);

        //Unique Log
        PacketHandler.RegisterClientHandler(0x180D, CLIENT_UNIQUE_LOG);
        PacketHandler.RegisterModuleHandler(0x300C, SERVER_GAME_NOTIFY);

        //Event Schedule
        PacketHandler.RegisterClientHandler(0x180E, CLIENT_EVENT_SCHEDULE);

        //Detecting first teleport
        PacketHandler.RegisterModuleHandler(0x3305, SERVER_COMMUNITY_FRIEND_INFO);

        //Item Chest
        PacketHandler.RegisterClientHandler(0x180F, CLIENT_ITEM_CHEST);

        //title & acheivments
        PacketHandler.RegisterClientHandler(0x181A, CLIENT_TITLE);
        PacketHandler.RegisterClientHandler(0x181B, CLIENT_ACHEIEVMENT);
        PacketHandler.RegisterClientHandler(0x181C, CLIENT_PURCHASE_CUSTOMS);

        //daily reward
        PacketHandler.RegisterClientHandler(0x181D, CLIENT_DAILY_REWARD);

        //hwid && acc lock
        PacketHandler.RegisterClientHandler(0x181E, CLIENT_HWID);
        PacketHandler.RegisterClientHandler(0x181F, CLIENT_ACC_LOCK);
        PacketHandler.RegisterModuleHandler(0xB007, SERVER_AGENT_CHARACTER_SELECTION_RESPONSE);


        //login request
        PacketHandler.RegisterClientHandler(0x6103, CLIENT_LOGIN_REQUEST);

        //DPS meter by chernobyl
        PacketHandler.RegisterModuleHandler(0x5010, SERVER_GAMESERVER_HANDLE_DPS_INFO);

        //move to party member
        PacketHandler.RegisterClientHandler(0x182A, CLIENT_MOVE_TO_PARTY_MEMBER);

        //plus limit
        PacketHandler.RegisterClientHandler(0x7150, CLIENT_ALCHEMY_PACKET);


    }

    private ISharedObjects SharedObjects { get; set; }

    public override void AddSession(ISession session)
    {
        base.AddSession(session);
        SharedObjects.AgentSessions.Add(session);
    }

    public override void RemoveSession(ISession session)
    {
        base.RemoveSession(session);
        if (SharedObjects.AgentSessions.Contains(session)) SharedObjects.AgentSessions.Remove(session);

        //CustomTitles?.RemoveSession(session);
    }

    public override void Dispose()
    {
        if (SharedObjects != null)
        {
            foreach (var agentSession in SharedObjects.AgentSessions)
            {
                agentSession.Dispose();
            }
        }

        SharedObjects = null;

        base.Dispose();
    }

    private async Task<PacketResult> CLIENT_CHARACTER_ACTION_REQUEST(Packet packet, ISession session)
    {
        var unk1 = packet.ReadUInt8();
        if (unk1 != 0x01) return new PacketResult();

        var action = (CharacterAction)packet.ReadUInt8();
        if (action != CharacterAction.SkillCast) return new PacketResult();

        var skillId = (int)packet.ReadUInt32();
        var skill = SharedObjects.RefSkill.GetValueOrDefault(skillId, null);
        if (skill == null) return new PacketResult();

        if (!skill.Basic_Code.Contains("COLD_SHIELD")) return new PacketResult();

        if (session.SessionData.LastSnowshieldUsage + skill.Action_ReuseDelay >
            DateTimeOffset.UtcNow.ToUnixTimeMilliseconds())
        {
            session.SendNotice("You cannot use Snow Shield again. Please wait another " +
                               (int)((DateTimeOffset.UtcNow.ToUnixTimeMilliseconds() -
                                       session.SessionData.LastSnowshieldUsage - skill.Action_ReuseDelay) / 1000 *
                                      -1) + " seconds!");
            return new PacketResult(PacketResultType.Block);
        }

        session.SessionData.LastSnowshieldUsage = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();
        return new PacketResult();
    }

    private async Task<PacketResult> SERVER_ENTITY_STATE_UPDATE(Packet packet, ISession session)
    {
        var uniqueId = packet.ReadUInt32();
        if (session.SessionData.UniqueCharId != uniqueId) return new PacketResult();

        var updateType = packet.ReadUInt8();
        var updateState = packet.ReadUInt8();
        switch (updateType)
        {
            case 0: // LifeState
                break;
            case 1: // MotionState
                break;
            case 4:
                session.SessionData.GameStateType = (GameState)updateState;
                break;
            case 7:
                session.SessionData.PvpState = (PvpState)updateState;
                break;
            case 8:
                session.SessionData.InCombat = updateState == 1;
                break;
            case 11:
                session.SessionData.ScrollingType = (Scrolling)updateState;
                break;
        }

        return new PacketResult();
    }

    private async Task<PacketResult> SERVER_AGENT_CHARACTER_DATA(Packet packet, ISession session)
    {
        try
        {
            var serverTime = packet.ReadUInt32(); // * 4   uint    ServerTime               //SROTimeStamp
            var refObjId = packet.ReadUInt32(); // 4   uint    RefObjID
            var scale = packet.ReadUInt8(); // 1   byte    Scale
            var curLevel = packet.ReadUInt8(); // 1   byte    CurLevel
            var maxLevel = packet.ReadUInt8(); // 1   byte    MaxLevel
            var expOffset = packet.ReadUInt64(); // 8   ulong   ExpOffset
            var sExpOffset = packet.ReadUInt32(); // 4   uint    SExpOffset
            var remainGold = packet.ReadUInt64(); // 8   ulong   RemainGold
            var remainSkillPoint = packet.ReadUInt32(); // 4   uint    RemainSkillPoint
            var remainStatPoint = packet.ReadUInt16(); // 2   ushort  RemainStatPoint
            var remainHwanCount = packet.ReadUInt8(); // 1   byte    RemainHwanCount
            var gatheredExpPoint = packet.ReadUInt32(); // 4   uint    GatheredExpPoint
            var hp = packet.ReadUInt32(); // 4   uint    HP
            var mp = packet.ReadUInt32(); // 4   uint    MP
            var autoInverstExp = packet.ReadUInt8(); // 1   byte    AutoInverstExp
            var dailyPk = packet.ReadUInt8(); // 1   byte    DailyPK
            var totalPk = packet.ReadUInt16(); // 2   ushort  TotalPK
            var pkPenaltyPoint = packet.ReadUInt32(); // 4   uint    PKPenaltyPoint
            var hwanLevel = packet.ReadUInt8(); // 1   byte    HwanLevel
            var freePvp =
                packet.ReadUInt8(); // 1   byte    FreePVP           //0 = None, 1 = Red, 2 = Gray, 3 = Blue, 4 = White, 5 = Gold

            // //Inventory
            var inventorySize = packet.ReadUInt8(); // 1   byte    Inventory.Size
            var inventoryItemCount = packet.ReadUInt8(); // 1   byte    Inventory.ItemCount
            for (var i = 0; i < inventoryItemCount; i++) // for (int i = 0; i < Inventory.ItemCount; i++)
            {
                var itemSlot = packet.ReadUInt8(); //     1   byte    item.Slot
                var itemRentType = packet.ReadUInt32(); //     4   uint    item.RentType
                if (itemRentType == 1)
                {
                    var itemRentInfoCanDelete = packet.ReadUInt16(); //         2   ushort  item.RentInfo.CanDelete
                    var itemRentInfoPeriodBeginTime =
                        packet.ReadUInt32(); //         4   uint    item.RentInfo.PeriodBeginTime
                    var itemRentInfoPeriodEndTime =
                        packet.ReadUInt32(); //         4   uint    item.RentInfo.PeriodEndTime        
                }
                else if (itemRentType == 2)
                {
                    var itemRentInfoCanDelete = packet.ReadUInt16(); //         2   ushort  item.RentInfo.CanDelete
                    var itemRentInfoCanRecharge = packet.ReadUInt16(); //         2   ushort  item.RentInfo.CanRecharge
                    var itemRentInfoMeterRateTime =
                        packet.ReadUInt32(); //         4   uint    item.RentInfo.MeterRateTime        
                }
                else if (itemRentType == 3)
                {
                    var itemRentInfoCanDelete = packet.ReadUInt16(); //         2   ushort  item.RentInfo.CanDelete
                    var itemRentInfoCanRecharge = packet.ReadUInt16(); //         2   ushort  item.RentInfo.CanRecharge
                    var itemRentInfoPeriodBeginTime =
                        packet.ReadUInt32(); //         4   uint    item.RentInfo.PeriodBeginTime
                    var itemRentInfoPeriodEndTime =
                        packet.ReadUInt32(); //         4   uint    item.RentInfo.PeriodEndTime   
                    var itemRentInfoPackingTime =
                        packet.ReadUInt32(); //         4   uint    item.RentInfo.PackingTime        
                }

                var itemRefItemId = packet.ReadUInt32(); //     4   uint    item.RefItemID
                var item = SharedObjects.RefObjCommon[(int)itemRefItemId];

                if (item == null) continue;
                if (item.TypeID1 == 3)
                {
                    //ITEM_        
                    if (item.TypeID2 == 1)
                    {
                        //ITEM_CH
                        //ITEM_EU
                        //AVATAR_
                        var itemOptLevel = packet.ReadUInt8(); // 1   byte    item.OptLevel
                        var itemVariance = packet.ReadUInt64(); // 8   ulong   item.Variance
                        var itemData = packet.ReadUInt32(); // 4   uint    item.Data       //Durability
                        var itemMagParamNum = packet.ReadUInt8(); // 1   byte    item.MagParamNum
                        for (var paramIndex = 0; paramIndex < itemMagParamNum; paramIndex++)
                        {
                            var magParamType = packet.ReadUInt32(); // 4   uint    magParam.Type
                            var magParamValue = packet.ReadUInt32(); // 4   uint    magParam.Value                
                        }

                        var bindingOptionType = packet.ReadUInt8(); // 1   byte    bindingOptionType   //1 = Socket
                        var bindingOptionCount = packet.ReadUInt8(); // 1   byte    bindingOptionCount
                        for (var bindingOptionIndex = 0; bindingOptionIndex < bindingOptionCount; bindingOptionIndex++)
                        {
                            var bindingOptionSlot = packet.ReadUInt8(); // 1   byte bindingOption.Slot
                            var bindingOptionId = packet.ReadUInt32(); // 4   uint bindingOption.ID
                            var bindingOptionParam1 = packet.ReadUInt32(); // 4   uint bindingOption.nParam1
                        }

                        var bindingOptionType2 =
                            packet.ReadUInt8(); // 1   byte    bindingOptionType   //2 = Advanced elixir
                        var bindingOptionCount2 = packet.ReadUInt8(); // 1   byte    bindingOptionCount2
                        for (var bindingOptionIndex = 0; bindingOptionIndex < bindingOptionCount2; bindingOptionIndex++)
                        {
                            var bindingOptionSlot = packet.ReadUInt8(); // 1   byte bindingOption.Slot
                            var bindingOptionId = packet.ReadUInt32(); // 4   uint bindingOption.ID
                            var bindingOptionOptValue = packet.ReadUInt32(); // 4   uint bindingOption.OptValue
                        }
                    }
                    else if (item.TypeID2 == 2)
                    {
                        if (item.TypeID3 == 1)
                        {
                            //ITEM_COS_P
                            var cosState = packet.ReadUInt8(); //1   byte    State
                            if (cosState == 2 || cosState == 3 || cosState == 4)
                            {
                                var cosRefObjId = packet.ReadUInt32(); // 4 uint RefObjID
                                var cosName = packet.ReadAscii(); // 2 ushort Name.Length //     * string Name
                                if (item.TypeID4 == 2)
                                {
                                    //ITEM_COS_P (Ability)
                                    var cosSecondsToRentEndTime = packet.ReadUInt32(); // 4 uint SecondsToRentEndTime
                                }

                                // Maybe?!
                                // might be service thing
                                var hasInventoryTime = packet.ReadUInt8(); // 1 byte unkByte0

                                if (hasInventoryTime == 0x1)
                                {
                                    // Perhaps inventory span
                                    var unk1222 = packet.ReadUInt8(); // NANI
                                    var unk1223 = packet.ReadUInt32(); // THE
                                    var unk1224 = packet.ReadUInt32(); // FUCK
                                    var unk1225 = packet.ReadUInt32(); // ?!
                                    var unk1226 = packet.ReadUInt8();
                                    //Global.Logger.InfoFormat("{0}, {1}, {2}, {3}, {4}", unk1222, unk1223, unk1224,
                                    //    unk1225, unk1226);
                                }
                            }
                        }
                        else if (item.TypeID3 == 2)
                        {
                            //ITEM_ETC_TRANS_MONSTER
                            var etcRefObjId = packet.ReadUInt32(); // 4   uint    RefObjID
                        }
                        else if (item.TypeID3 == 3)
                        {
                            //MAGIC_CUBE
                            var quantity =
                                packet
                                    .ReadUInt32(); // 4   uint    Quantity        //Do not confuse with StackCount, this indicates the amount of elixirs in the cube
                        }
                    }
                    else if (item.TypeID2 == 3)
                    {
                        //ITEM_ETC
                        var itemStackCount = packet.ReadUInt16(); // 2   ushort  item.StackCount

                        if (item.TypeID3 == 11)
                        {
                            if (item.TypeID4 == 1 || item.TypeID4 == 2)
                            {
                                //MAGICSTONE, ATTRSTONE
                                var attributeAssimilationProbability =
                                    packet.ReadUInt8(); // 1   byte    AttributeAssimilationProbability
                            }
                        }
                        else if (item.TypeID3 == 14 && item.TypeID4 == 2)
                        {
                            //ITEM_MALL_GACHA_CARD_WIN
                            //ITEM_MALL_GACHA_CARD_LOSE
                            var magParamNum = packet.ReadUInt8(); // 1   byte    item.MagParamCount
                            for (var paramIndex = 0; paramIndex < magParamNum; paramIndex++)
                            {
                                var magParamType = packet.ReadUInt32(); //4   uint magParam.Type
                                var magParamValue = packet.ReadUInt32(); //4   uint magParam.Value
                            }
                        }
                    }
                }
            }

            //AvatarInventory
            var avatarInventorySize = packet.ReadUInt8(); // 1 byte AvatarInventory.Size
            var avatarInventoryItemCount = packet.ReadUInt8(); // 1 byte AvatarInventory.ItemCount
            for (var i = 0; i < avatarInventoryItemCount; i++)
            {
                packet.ReadUInt8(); // 1 byte item.Slot
                var itemRentType = packet.ReadUInt32(); // 4 uint item.RentType
                if (itemRentType == 1)
                {
                    packet.ReadUInt16(); // 2 ushort item.RentInfo.CanDelete
                    packet.ReadUInt16(); // 4 uint item.RentInfo.PeriodBeginTime
                    packet.ReadUInt32(); // 4 uint item.RentInfo.PeriodEndTime
                }
                else if (itemRentType == 2)
                {
                    packet.ReadUInt16(); // 2 ushort item.RentInfo.CanDelete
                    packet.ReadUInt16(); // 2 ushort item.RentInfo.CanRecharge
                    packet.ReadUInt32(); // 4 uint item.RentInfo.MeterRateTime
                }
                else if (itemRentType == 3)
                {
                    packet.ReadUInt16(); // 2 ushort item.RentInfo.CanDelete
                    packet.ReadUInt16(); // 2 ushort item.RentInfo.CanRecharge
                    packet.ReadUInt32(); // 4 uint item.RentInfo.PeriodBeginTime
                    packet.ReadUInt32(); // 4 uint item.RentInfo.PeriodEndTime
                    packet.ReadUInt32(); // 4 uint item.RentInfo.PackingTime
                }

                var itemRefItemId = packet.ReadUInt32(); // 4 uint item.RefItemID
                var item = SharedObjects.RefObjCommon[(int)itemRefItemId];

                if (item.TypeID1 == 3)
                    //ITEM_        
                    if (item.TypeID2 == 1)
                    {
                        //ITEM_CH
                        //ITEM_EU
                        //AVATAR_
                        packet.ReadUInt8(); // 1 byte item.OptLevel
                        packet.ReadUInt64(); // 8 ulong item.Variance
                        packet.ReadUInt32(); // 4 uint item.Data //Durability
                        var itemMagParamNum = packet.ReadUInt8(); // 1 byte item.MagParamNum
                        for (var paramIndex = 0; paramIndex < itemMagParamNum; paramIndex++)
                        {
                            packet.ReadUInt32(); // 4 uint magParam.Type
                            packet.ReadUInt32(); // 4 uint magParam.Value
                        }

                        packet.ReadUInt8(); // 1 byte bindingOptionType //1 = Socket
                        var bindingOptionCount = packet.ReadUInt8(); // 1 byte bindingOptionCount
                        for (var bindingOptionIndex = 0;
                             bindingOptionIndex < bindingOptionCount;
                             bindingOptionIndex++)
                        {
                            packet.ReadUInt8(); // 1 byte bindingOption.Slot
                            packet.ReadUInt32(); // 4 uint bindingOption.ID
                            packet.ReadUInt32(); // 4 uint bindingOption.nParam1
                        }

                        packet.ReadUInt8(); // 1 byte bindingOptionType //2 = Advanced elixir
                        var bindingOptionCount2 = packet.ReadUInt8(); // 1 byte bindingOptionCount
                        for (var bindingOptionIndex = 0;
                             bindingOptionIndex < bindingOptionCount2;
                             bindingOptionIndex++)
                        {
                            packet.ReadUInt8(); // 1 byte bindingOption.Slot
                            packet.ReadUInt32(); // 4 uint bindingOption.ID
                            packet.ReadUInt32(); // 4 uint bindingOption.OptValue
                        }
                    }
            }

            packet.ReadUInt8(); //1 byte unkByte1 //not a counter

            //Masteries
            var nextMastery = packet.ReadUInt8(); // 1   byte    nextMastery
            while (nextMastery == 1)
            {
                var masteryId = packet.ReadUInt32(); // 4   uint    mastery.ID
                var masteryLevel = packet.ReadUInt8(); // 1   byte    mastery.Level   
                nextMastery = packet.ReadUInt8(); // 1   byte    nextMastery
            }

            packet.ReadUInt8(); // 1   byte    unkByte2    //not a counter

            //Skills
            var nextSkill = packet.ReadUInt8(); // 1   byte    nextSkill
            while (nextSkill == 1)
            {
                var skillId = packet.ReadUInt32(); // 4   uint    skill.ID
                var skillEnabled = packet.ReadUInt8(); // 1   byte    skill.Enabled   

                nextSkill = packet.ReadUInt8(); // 1   byte    nextSkill
            }

            //Quests
            var completedQuestCount = packet.ReadUInt16(); // 2   ushort  CompletedQuestCount
            var completedQuests = packet.ReadUInt32Array(completedQuestCount); // *   uint[]  CompletedQuests

            var activeQuestCount = packet.ReadUInt8(); // 1   byte    ActiveQuestCount
            for (var activeQuestIndex = 0; activeQuestIndex < activeQuestCount; activeQuestIndex++)
            {
                var questRefQuestId = packet.ReadUInt32(); // 4   uint    quest.RefQuestID
                var questAchievementCount = packet.ReadUInt8(); // 1   byte    quest.AchievementCount
                var questRequiresAutoShareParty = packet.ReadUInt8(); // 1   byte    quest.RequiresAutoShareParty
                var questType = packet.ReadUInt8(); // 1   byte    quest.Type
                if (questType == 28)
                {
                    var questRemainingTime = packet.ReadUInt32(); // 4   uint    remainingTime
                }

                var questStatus = packet.ReadUInt8(); // 1   byte    quest.Status

                if (questType != 8)
                {
                    var questObjectiveCount = packet.ReadUInt8(); // 1   byte    quest.ObjectiveCount
                    for (var objectiveIndex = 0; objectiveIndex < questObjectiveCount; objectiveIndex++)
                    {
                        var questObjectiveId = packet.ReadUInt8(); // 1   byte    objective.ID
                        var questObjectiveStatus =
                            packet.ReadUInt8(); // 1   byte    objective.Status        //0 = Done, 1  = On
                        var questObjectiveName =
                            packet.ReadAscii(); // 2   ushort  objective.Name.Length // *   string  objective.Name
                        var objectiveTaskCount = packet.ReadUInt8(); // 1   byte    objective.TaskCount
                        for (var taskIndex = 0; taskIndex < objectiveTaskCount; taskIndex++)
                        {
                            var questTaskValue = packet.ReadUInt32(); // 4   uint    task.Value
                        }
                    }
                }

                if (questType == 88)
                {
                    var refObjCount = packet.ReadUInt8(); // 1   byte    RefObjCount
                    for (var refObjIndex = 0; refObjIndex < refObjCount; refObjIndex++)
                        packet.ReadUInt32(); // 4   uint    RefObjID    //NPCs
                }
            }

            packet.ReadUInt8(); // 1   byte    unkByte3        //Structure changes!!!

            //CollectionBook
            var startedCollectionCount = packet.ReadUInt32(); // 4   uint    CollectionBookStartedThemeCount
            for (var i = 0; i < startedCollectionCount; i++)
            {
                var themeIndex = packet.ReadUInt32(); // 4   uint    theme.Index
                var themeStartedDateTime = packet.ReadUInt32(); // 4   uint    theme.StartedDateTime   //SROTimeStamp
                var themePages = packet.ReadUInt32(); // 4   uint    theme.Pages
            }

            session.SessionData.UniqueCharId = packet.ReadUInt32(); // 4   uint    UniqueID

            //Position
            session.SessionData.LatestRegionId = packet.ReadUInt16(); // 2   ushort  Position.RegionID
            session.SessionData.PositionX = packet.ReadFloat(); // 4   float   Position.X
            session.SessionData.PositionY = packet.ReadFloat(); // 4   float   Position.Y
            session.SessionData.PositionZ = packet.ReadFloat(); // 4   float   Position.Z
            var positionAngle = packet.ReadUInt16(); // 2   ushort  Position.Angle

            //Movement
            var movementHasDestination = packet.ReadUInt8(); // 1   byte    Movement.HasDestination
            var movementType = packet.ReadUInt8(); // 1   byte    Movement.Type
            if (movementHasDestination == 1)
            {
                var movementDestionationRegion = packet.ReadUInt16(); // 2   ushort  Movement.DestinationRegion        
                if (session.SessionData.LatestRegionId < short.MaxValue)
                {
                    //World
                    var movementDestinationOffsetX = packet.ReadUInt16(); // 2   ushort  Movement.DestinationOffsetX
                    var movementDestinationOffsetY = packet.ReadUInt16(); // 2   ushort  Movement.DestinationOffsetY
                    var movementDestinationOffsetZ = packet.ReadUInt16(); // 2   ushort  Movement.DestinationOffsetZ
                }
                else
                {
                    //Dungeon
                    var movementDestinationOffsetX = packet.ReadUInt32(); // 4   uint  Movement.DestinationOffsetX
                    var movementDestinationOffsetY = packet.ReadUInt32(); // 4   uint  Movement.DestinationOffsetY
                    var movementDestinationOffsetZ = packet.ReadUInt32(); // 4   uint  Movement.DestinationOffsetZ
                }
            }
            else
            {
                var movementSource =
                    packet.ReadUInt8(); // 1   byte    Movement.Source     //0 = Spinning, 1 = Sky-/Key-walking
                var movementAngle =
                    packet.ReadUInt16(); // 2   ushort  Movement.Angle      //Represents the new angle, character is looking at
            }

            //State
            var lifeState = packet.ReadUInt8(); // 1   byte    State.LifeState         //1 = Alive, 2 = Dead
            packet.ReadUInt8(); // 1   byte    State.unkByte0
            var stateMotionState =
                packet.ReadUInt8(); // 1   byte    State.MotionState       //0 = None, 2 = Walking, 3 = Running, 4 = Sitting
            session.SessionData.GameStateType =
                (GameState)packet
                    .ReadUInt8(); // 1   byte    State.Status            //0 = None, 1 = Hwan, 2 = Untouchable, 3 = GameMasterInvincible, 5 = GameMasterInvisible, 5 = ?, 6 = Stealth, 7 = Invisible
            var stateWalkSpeed = packet.ReadFloat(); // 4   float   State.WalkSpeed
            var stateRunSpeed = packet.ReadFloat(); // 4   float   State.RunSpeed
            var stateHwanSpeed = packet.ReadFloat(); // 4   float   State.HwanSpeed
            var stateBuffCount = packet.ReadUInt8(); // 1   byte    State.BuffCount

            for (var i = 0; i < stateBuffCount; i++)
            {
                var buffRefSkillId = packet.ReadUInt32(); // 4   uint    Buff.RefSkillID
                var buffDuration = packet.ReadUInt32(); // 4   uint    Buff.Duration

                var skill = SharedObjects.RefSkill[(int)buffRefSkillId];

                if (skill == null) continue;
                if (skill.ParamsContains(1701213281))
                {
                    //1701213281 -> atfe -> "auto transfer effect" like Recovery Division
                    var isCreator = packet.ReadUInt8(); // 1   bool    IsCreator
                }
            }

            var name = packet.ReadAscii(); // 2   ushort  Name.Length // *   string  Name
            session.SessionData.Charname = name;
            var jobName = packet.ReadAscii(); // 2   ushort  JobName.Length // *   string  JobName
            session.SessionData.JobType = (Job)packet.ReadUInt8(); // 1   byte    JobType
            var jobLevel = packet.ReadUInt8(); // 1   byte    JobLevel
            var jobExp = packet.ReadUInt32(); // 4   uint    JobExp
            var jobContribution = packet.ReadUInt32(); // 4   uint    JobContribution
            var jobReward = packet.ReadUInt32(); // 4   uint    JobReward
            session.SessionData.PvpState =
                (PvpState)packet.ReadUInt8(); // 1   byte    PVPState                //0 = White, 1 = Purple, 2 = Red
            var transportFlag = packet.ReadUInt8(); // 1   byte    TransportFlag
            session.SessionData.InCombat = packet.ReadBool(); // 1   byte    InCombat
            if (transportFlag == 1)
            {
                var transportUniqueId = packet.ReadUInt32(); // 4   uint    Transport.UniqueID
            }

            var pvpFlag =
                packet.ReadUInt8(); // 1   byte    PVPFlag                 //0 = Red Side, 1 = Blue Side, 0xFF = None
            var guideFlag = packet.ReadUInt64(); // 8   ulong   GuideFlag
            var jid = packet.ReadUInt32(); // 4   uint    JID
            var gmFlag = packet.ReadUInt8(); // 1   byte    GMFlag

            var activationFlag =
                packet.ReadUInt8(); // 1   byte    ActivationFlag          //ConfigType:0 --> (0 = Not activated, 7 = activated)
            var hotkeyCount = packet.ReadUInt8(); // 1   byte    Hotkeys.Count           //ConfigType:1
            for (var i = 0; i < hotkeyCount; i++)
            {
                var hotkeySlotSeq = packet.ReadUInt8(); // 1   byte    hotkey.SlotSeq
                var hotkeySlotContentType = packet.ReadUInt8(); // 1   byte    hotkey.SlotContentType
                var hotkeySlotData = packet.ReadUInt32(); // 4   uint    hotkey.SlotData
            }

            var autoHpConfig = packet.ReadUInt16(); // 2   ushort  AutoHPConfig            //ConfigType:11
            var autoMpConfig = packet.ReadUInt16(); // 2   ushort  AutoMPConfig            //ConfigType:12
            var autoUniversalConfig = packet.ReadUInt16(); // 2   ushort  AutoUniversalConfig     //ConfigType:13
            var autoPotionDelay = packet.ReadUInt8(); // 1   byte    AutoPotionDelay         //ConfigType:14

            var blockedWhisperCount = packet.ReadUInt8(); // 1   byte    blockedWhisperCount
            for (var i = 0; i < blockedWhisperCount; i++)
            {
                var target = packet.ReadAscii(); // 2   ushort  Target.Length // *   string  Target
            }

            packet.ReadUInt32(); // 4   uint    unkUShort0      //Structure changes!!!
            packet.ReadUInt8(); // 1   byte    unkByte4        //Structure changes!!!

        }
        catch (Exception ex)
        {
            Global.Logger.Info(ex.ToString());
        }
        return new PacketResult();
    }

    private async Task<PacketResult> AGENT_MOVEMENT_SERVER(Packet packet, ISession session)
    {
        var target = packet.ReadUInt32(); // Unique ID from player

        if (target != session.SessionData.UniqueCharId) return new PacketResult();

        // sky = 0, ground = 1
        var groundClick = packet.ReadUInt8(); //sky or ground click
        if (groundClick == 0x00) return new PacketResult();

        session.SessionData.LatestRegionId = packet.ReadUInt16(); // Region ID
        if (session.SessionData.LatestRegionId >= short.MaxValue)
        {
            var x = packet.ReadUInt32();
            var y = packet.ReadUInt32();
            var z = packet.ReadUInt32();
        }
        else
        {
            session.SessionData.PositionX = packet.ReadUInt16();
            session.SessionData.PositionY = packet.ReadUInt16();
            session.SessionData.PositionZ = packet.ReadUInt16();
        }

        return new PacketResult();
    }

    private async Task<PacketResult> AGENT_TELEPORT_USE(Packet packet, ISession session)
    {
        session.CharacterGameReady = false;
        return new PacketResult();
    }

    private async Task<PacketResult> AGENT_GAME_READY(Packet packet, ISession session)
    {
        // fix to not crash on autonotice
        session.CharacterGameReady = true;
        return new PacketResult();
    }

    private async Task<PacketResult> AGENT_ENVIRONMENT_CELESTIAL_POSITION(Packet packet, ISession session)
    {
        session.CharScreen = false;
        //.CharacterData.UniqueCharId = packet.ReadUInt32();
        return new PacketResult();
    }

    private async Task<PacketResult> CLIENT_AGENT_CHARACTER_SELECTION_ACTION_REQUEST(Packet packet, ISession session)
    {
        if (!session.CharScreen)
        {
            Global.Logger.WarnFormat("Client {0}({1}) attempted to send 0x7007 outside char screen!", session.SessionData.Charname,
                packet.Opcode);
            return new PacketResult(PacketResultType.Disconnect);
        }

        await new CLIENT_AGENT_CHARACTER_SELECTION_ACTION_REQUEST().Read(packet);
        if (packet.RemainingRead() != 0)
        {
            Global.Logger.WarnFormat("Client {0}({1}) attempted to crash SHARD_MANAGER!", session.SessionData.Charname,
                packet.Opcode);
            return new PacketResult(PacketResultType.Disconnect);
        }


        return new PacketResult();
    }

    private async Task<PacketResult> CLIENT_EXPLOIT_GSCRASH(Packet packet, ISession session)
    {
        Global.Logger.WarnFormat("EXPLOIT - {0} tried to use GS_CRASH_EXPLOIT - {1:X}", session.SessionData.Charname,
            packet.Opcode);
        return new PacketResult(PacketResultType.Disconnect);
    }

    private async Task<PacketResult> AGENT_SKILL_MASTERY_LEARN(Packet packet, ISession session)
    {
        packet.ReadUInt32(); // masteryid
        var level = packet.ReadUInt8();

        if (level == 1) return new PacketResult();

        Global.Logger.WarnFormat("EXPLOIT - {0} tried to use SKILL_EXPLOIT - {1:X}", session.SessionData.Charname,
            packet.Opcode);
        return new PacketResult(PacketResultType.Disconnect);
    }

    private async Task<PacketResult> CLIENT_PLAYER_BERSERK(Packet packet, ISession session)
    {
        var flag = packet.ReadUInt8();
        if (flag == 1) return new PacketResult();

        Global.Logger.WarnFormat("EXPLOIT - {0} tried to use INVIS EXPLOIT - {1:X}", session.SessionData.Charname,
            packet.Opcode);
        return new PacketResult(PacketResultType.Disconnect);
    }

    private async Task<PacketResult> AGENT_LOGOUT(Packet packet, ISession session)
    {
        if (session.CharScreen)
        {
            Global.Logger.WarnFormat("EXPLOIT - {0} tried to use AS_CRASH_EXPLOIT - {1:X} at 1",
                session.SessionData.Charname,
                packet.Opcode);
            return new PacketResult(PacketResultType.Disconnect);
        }

        if (session.SessionData.Charid <= 0)
        {
            Global.Logger.WarnFormat("EXPLOIT - {0} tried to use AS_CRASH_EXPLOIT - {1:X} at 2",
                session.SessionData.Charname,
                packet.Opcode);
            return new PacketResult(PacketResultType.Disconnect);
        }

        var logoutMode = packet.ReadUInt8();
        if (logoutMode > 2)
        {
            Global.Logger.WarnFormat("EXPLOIT - {0} tried to use AS_CRASH_EXPLOIT - {1:X} at 3",
                session.SessionData.Charname,
                packet.Opcode);
            return new PacketResult(PacketResultType.Disconnect);
        }

        return new PacketResult();
    }

    private async Task<PacketResult> AGENT_MAGICOPTION_GRANT(Packet packet, ISession session)
    {
        var avatarBlue = packet.ReadAscii().ToLower();
        if (avatarBlue.Contains("avatar")) return new PacketResult();

        Global.Logger.WarnFormat("EXPLOIT - {0} tried to use AVATAR_EXPLOIT - {1:X}", session.SessionData.Charname,
            packet.Opcode);
        return new PacketResult(PacketResultType.Disconnect);
    }

    private async Task<PacketResult> AGENT_GUILD_UPDATE_NOTICE(Packet packet, ISession session)
    {
        var guildNoticeTitle = packet.ReadAscii();
        var guildNoticeMessage = packet.ReadAscii();

        if (!guildNoticeMessage.Contains('\'') &&
            !guildNoticeMessage.Contains('\"') &&
            !guildNoticeMessage.Contains('-') &&
            !guildNoticeTitle.Contains('\'') &&
            !guildNoticeTitle.Contains('\"') &&
            !guildNoticeTitle.Contains('-'))
            return new PacketResult();

        Global.Logger.WarnFormat("EXPLOIT - {0} tried to use GUILD_SQL_INJECTION - {1:X}",
            session.SessionData.Charname,
            packet.Opcode);
        await session.SendNotice(
            "You're not allowed to use special characters in this textfield. We've replaced them for you.");

        guildNoticeTitle = guildNoticeTitle
            .Replace('\'', ' ').Replace('-', ' ').Replace('\"', ' ').Replace(';', ' ');
        guildNoticeMessage = guildNoticeMessage
            .Replace('\'', ' ').Replace('-', ' ').Replace('\"', ' ')
            .Replace(';', ' ');

        var newPacket = new Packet(packet.Opcode, packet.Encrypted, packet.Massive);
        newPacket.WriteAscii(guildNoticeTitle);
        newPacket.WriteAscii(guildNoticeMessage);

        return new PacketResult(newPacket, PacketResultType.Override);
    }

    private async Task<PacketResult> AGENT_SIEGE_ACTION(Packet packet, ISession session)
    {
        packet.ReadUInt32();
        var unk2 = packet.ReadUInt8();
        uint unk3 = 0;
        if (unk2 == 1 || unk2 == 2 || unk2 == 26) unk3 = packet.ReadUInt32();

        // About guild
        if (unk2 != 26 || unk3 != 1) return new PacketResult();
        var message = packet.ReadAscii();

        if (!message.Contains("\'") && !message.Contains("\"") && !message.Contains("-"))
            return new PacketResult();

        Global.Logger.WarnFormat("EXPLOIT - {0} tried to use FW_SQL_INJECTION - {1:X}",
            session.SessionData.Charname,
            packet.Opcode);
        await session.SendNotice("You're not allowed to use special characters in this textfield.");
        return new PacketResult(PacketResultType.Block);
    }

    private async Task<PacketResult> AGENT_AUTH(Packet packet, ISession session)
    {
        if (packet.ReadUInt8() == 1)
            session.UserLoggedIn = true;

        if (session.UserLoggedIn)
            session.CharScreen = true;

        return new PacketResult();
    }

    private async Task<PacketResult> AGENT_CHARACTER_SELECTION_JOIN(Packet packet, ISession session)
    {
        if(!session.Verified)
            return new PacketResult(PacketResultType.Block);

        if (session.CharnameSent)
            return new PacketResult(PacketResultType.Block);

        if (!session.CharScreen)
        {
            Global.Logger.WarnFormat("Client {0}({1}) attempted to send 0x7001 outside char screen!",
                session.ClientId, session.ClientIp);
            return new PacketResult(PacketResultType.Disconnect);
        }

        session.SessionData.Charname = packet.ReadAscii();

        if (session.PacketLength - session.SessionData.Charname.Length != 2)
        {
            Global.Logger.WarnFormat("Client {0}({1})attempted to modify 0x7001!", session.ClientId,
                session.ClientIp);
            return new PacketResult(PacketResultType.Disconnect);
        }

        session.CharnameSent = true;

        using var db = new SRO_VT_SHARD();
        session.SessionData.Charid =
            (await db.C_Char.Where(x => x.CharName16 == session.SessionData.Charname).FirstAsync()).CharID;
        session.SessionData.JID =
            (await db.C_User.Where(x => x.CharID == session.SessionData.Charid).FirstAsync()).UserJID;

        return new PacketResult();
    }
    #region ITEM_CHAT_LINKING
    private async Task<PacketResult> CLIENT_ITEM_CHAT_LINKING(Packet packet, ISession session)
    {
        
        var ItemName = packet.ReadAscii();
        var ItemSlot = packet.ReadUInt8();
        var ItemID = packet.ReadInt32();

        using var db = new SRO_VT_SHARD();
        var ItemInfo = await (from I in db.C_Items
                                join C in db.C_Inventory on I.ID64 equals C.ItemID
                                join R in db.C_RefObjCommon on ItemID equals R.ID
                                where C.Slot == ItemSlot && C.CharID == session.SessionData.Charid
                                select new
                                {
                                    Plus = I.OptLevel,
                                    Variance = I.Variance,
                                    Durability = I.Data,
                                    MagParamNum = I.MagParamNum,
                                    MagParam1 = I.MagParam1,
                                    MagParam2 = I.MagParam2,
                                    MagParam3 = I.MagParam3,
                                    MagParam4 = I.MagParam4,
                                    MagParam5 = I.MagParam5,
                                    MagParam6 = I.MagParam6,
                                    MagParam7 = I.MagParam7,
                                    MagParam8 = I.MagParam8,
                                    MagParam9 = I.MagParam9,
                                    MagParam10 = I.MagParam10,
                                    MagParam11 = I.MagParam11,
                                    MagParam12 = I.MagParam12,
                                    RefItemID = I.RefItemID,
                                    TypeID2 = R.TypeID2,
                                    TypeID3 = R.TypeID3,
                                    TypeID4 = R.TypeID4,
                                    ID64 = I.ID64
                                }).FirstOrDefaultAsync();

        var bItemInfo = await db.C_BindingOptionWithItem.Where(b => b.nItemDBID == ItemInfo.ID64)
                                                    .Select(b => new BindingItemInfo
                                                    {
                                                        Type = b.bOptType,
                                                        ID = b.nOptID,
                                                        Slot = b.nSlot,
                                                        nParam = b.nParam1,
                                                        Value = b.nOptValue,
                                                        nOptLvl = b.nOptLvl
                                                    }).ToListAsync();

        var _ItemInfo = new ItemInfo(ItemInfo.Plus, ItemInfo.Variance, ItemInfo.Durability, ItemInfo.MagParamNum, ItemInfo.MagParam1, ItemInfo.MagParam2, ItemInfo.MagParam3, ItemInfo.MagParam4, ItemInfo.MagParam5, ItemInfo.MagParam6, ItemInfo.MagParam7, ItemInfo.MagParam8, ItemInfo.MagParam9, ItemInfo.MagParam10, ItemInfo.MagParam11, ItemInfo.MagParam12, ItemInfo.RefItemID, ItemInfo.TypeID2, ItemInfo.TypeID3, ItemInfo.TypeID4, bItemInfo);

        if (!SharedObjects.ItemLinkInfo.ContainsKey($"{session.SessionData.Charname}<{ItemName}>"))
            SharedObjects.ItemLinkInfo.TryAdd($"{session.SessionData.Charname}<{ItemName}>", _ItemInfo);
        else
            SharedObjects.ItemLinkInfo[$"{session.SessionData.Charname}<{ItemName}>"] = _ItemInfo;

        return new PacketResult(PacketResultType.Block);
    }
    private async Task<PacketResult> CLIENT_ITEM_CHAT_LINKING_REQUEST(Packet packet, ISession session)
    {
        var itemNameKey = packet.ReadAscii();

        if (SharedObjects.ItemLinkInfo.ContainsKey(itemNameKey))
            await session.SendItemLink(itemNameKey, SharedObjects.ItemLinkInfo[itemNameKey]);

        return new PacketResult(PacketResultType.Block);
    }
    #endregion

    #region STALL NETWORK
    private async Task<PacketResult> CLIENT_STALL_ACTION(Packet packet, ISession session)
    {
        byte stallType = packet.ReadUInt8();

        switch (stallType)
        {
            case 0x01:
                {
                    //update an item
                    byte stallSlot = packet.ReadUInt8();
                    ushort quantity = packet.ReadUInt16();
                    ulong price = packet.ReadUInt64();
                    var getItem = SharedObjects.StallNetwork.FirstOrDefault(x => x.Value.sellerName == session.SessionData.Charname && x.Value.stallSlot == stallSlot);
                    if (getItem.Key > 0)
                    {
                        getItem.Value.quantity = quantity;
                        getItem.Value.price = price;
                        getItem.Value.uniqueID = session.SessionData.UniqueCharId;
                    }
                }
                break;
            case 0x02:
                {
                    //add an item
                    byte stallSlot = packet.ReadUInt8();
                    byte invSlot = packet.ReadUInt8();
                    ushort quantity = packet.ReadUInt16();
                    ulong price = packet.ReadUInt64();
                    uint tid = packet.ReadUInt32();
                    packet.ReadUInt16();

                    using var db = new SRO_VT_SHARD();
                    var ItemInfo = await (from I in db.C_Items
                                          join C in db.C_Inventory on I.ID64 equals C.ItemID
                                          join R in db.C_RefObjCommon on I.RefItemID equals R.ID
                                          where C.Slot == invSlot && C.CharID == session.SessionData.Charid
                                          select new
                                          {
                                              Plus = I.OptLevel,
                                              Variance = I.Variance,
                                              Durability = I.Data,
                                              MagParamNum = I.MagParamNum,
                                              MagParam1 = I.MagParam1,
                                              MagParam2 = I.MagParam2,
                                              MagParam3 = I.MagParam3,
                                              MagParam4 = I.MagParam4,
                                              MagParam5 = I.MagParam5,
                                              MagParam6 = I.MagParam6,
                                              MagParam7 = I.MagParam7,
                                              MagParam8 = I.MagParam8,
                                              MagParam9 = I.MagParam9,
                                              MagParam10 = I.MagParam10,
                                              MagParam11 = I.MagParam11,
                                              MagParam12 = I.MagParam12,
                                              RefItemID = I.RefItemID,
                                              TypeID2 = R.TypeID2,
                                              TypeID3 = R.TypeID3,
                                              TypeID4 = R.TypeID4,
                                              ID64 = I.ID64
                                          }).FirstOrDefaultAsync();

                    var bItemInfo = await db.C_BindingOptionWithItem.Where(b => b.nItemDBID == ItemInfo.ID64)
                                                                .Select(b => new BindingItemInfo
                                                                {
                                                                    Type = b.bOptType,
                                                                    ID = b.nOptID,
                                                                    Slot = b.nSlot,
                                                                    nParam = b.nParam1,
                                                                    Value = b.nOptValue,
                                                                    nOptLvl = b.nOptLvl
                                                                }).ToListAsync();

                    int Link = SharedObjects.RefObjCommon[ItemInfo.RefItemID].Link;
                    var ItemClass =
                            (await db.C_RefObjItem.Where(x => x.ID == Link).FirstOrDefaultAsync()).ItemClass;

                    var itemDG = 0;
                    if (ItemInfo.TypeID2 == 3 && ItemInfo.TypeID3 == 11 && (ItemInfo.TypeID4 == 1 || ItemInfo.TypeID4 == 2 || ItemInfo.TypeID4 == 3))
                    {
                        itemDG = ItemClass;
                    }
                    else
                    {
                        if ((ItemClass % 3) == 0)
                            itemDG = ItemClass / 3;
                        else
                            itemDG = ItemClass / 3 + 1;
                    }

                    var _ItemInfo = new ItemInfo(ItemInfo.Plus, ItemInfo.Variance, ItemInfo.Durability, ItemInfo.MagParamNum, ItemInfo.MagParam1, ItemInfo.MagParam2, ItemInfo.MagParam3, ItemInfo.MagParam4, ItemInfo.MagParam5, ItemInfo.MagParam6, ItemInfo.MagParam7, ItemInfo.MagParam8, ItemInfo.MagParam9, ItemInfo.MagParam10, ItemInfo.MagParam11, ItemInfo.MagParam12, ItemInfo.RefItemID, ItemInfo.TypeID2, ItemInfo.TypeID3, ItemInfo.TypeID4, bItemInfo, itemDG);
                    var _StallNetwork = new StallNetwork(session.SessionData.Charname, tid, stallSlot, invSlot, price, _ItemInfo, quantity, session.SessionData.UniqueCharId);
                    SharedObjects.StallNetwork.TryAdd(SharedObjects.StallNetwork.Count + 1, _StallNetwork);
                }
                break;
            case 0x03:
                {
                    //remove an item
                    byte stallSlot = packet.ReadUInt8();
                    var getItem = SharedObjects.StallNetwork.FirstOrDefault(x => x.Value.sellerName == session.SessionData.Charname && x.Value.stallSlot == stallSlot);
                    if (getItem.Key > 0)
                        SharedObjects.StallNetwork.Remove(getItem.Key);
                }
                break;
            case 0x05:
                {
                    //stall opened or closed
                    bool isActive = Convert.ToBoolean(packet.ReadUInt8());

                    using var db = new SRO_VT_SHARD();
                    var continentName =
                          (await db.C_RefRegion.Where(x => x.wRegionID == session.SessionData.LatestRegionId).FirstOrDefaultAsync()).ContinentName;

                    var getallItems = SharedObjects.StallNetwork.Where(x => x.Value.sellerName == session.SessionData.Charname).Select(x => x.Key).ToList();
                    foreach (var item in getallItems)
                    {
                        SharedObjects.StallNetwork[item].isActive = isActive;
                        SharedObjects.StallNetwork[item].continentName = continentName;
                    }
                }
                break;
        }

        return new PacketResult(PacketResultType.Nothing);
    }
    private async Task<PacketResult> CLIENT_STALL_NETWORK_SEARCH(Packet packet, ISession session)
    {
        try
        {
            packet.ReadUInt8();
            byte pageID = (byte)(packet.ReadUInt8() + 0x01);
            uint tid = packet.ReadUInt32();
            int ItemDG = packet.ReadUInt8();

            if (SharedObjects.RegionInfo.ContainsKey((int)session.SessionData.LatestRegionId) && SharedObjects.RegionInfo[(int)session.SessionData.LatestRegionId].Type != "Town")
                await session.SendNotice("Move to town to be allowed to use the stall network");
            else
                await session.SendStallNetwork(tid, pageID, ItemDG, SharedObjects.StallNetwork);

            session.SessionData.stallNetwork = true;
        }
        catch (Exception ex)
        {
            Global.Logger.WarnFormat("Client {0}", ex.ToString());
        }

        return new PacketResult(PacketResultType.Block);
    }
    private async Task<PacketResult> CLIENT_STALL_LEAVE(Packet packet, ISession session)
    {
        if (session.SessionData.fleablockStall)
        {
            session.SessionData.fleablockStall = false;
            return new PacketResult(PacketResultType.Block);
        }
        return new PacketResult(PacketResultType.Nothing);
    }
    private async Task<PacketResult> CLIENT_STALL_NETWORK_PURCHASE(Packet packet, ISession session)
    {
        packet.ReadUInt32();
        packet.ReadUInt8();
        packet.ReadInt64();
        packet.ReadUInt16();
        packet.ReadUInt8();
        int fleaitemID = (int)packet.ReadInt64();

        if (!SharedObjects.StallNetwork.ContainsKey(fleaitemID))
        {
            await session.SendNotice("Item not found please refresh the search!");
            return new PacketResult(PacketResultType.Block);
        }
        var getfleaItem = SharedObjects.StallNetwork[fleaitemID];
        if (getfleaItem != null)
        {
            using var db = new SRO_VT_SHARD();
            long curGold =
                (await db.C_Char.Where(x => x.CharName16 == session.SessionData.Charname).FirstAsync()).RemainGold;
            if ((ulong)curGold < getfleaItem.price)
            {
                await session.SendNotice("You dont have enough gold to purchase this item!");
                return new PacketResult(PacketResultType.Block);
            }
            int remainSlots =
               await (from I in db.C_Inventory
                      join C in db.C_Char on session.SessionData.Charid equals C.CharID
                      where I.CharID == session.SessionData.Charid && I.ItemID == 0 && I.Slot >= 13 && I.Slot < C.InventorySize
                      select I.Slot).CountAsync();

            if (remainSlots < 1)
            {
                await session.SendNotice("Your inventory is full!");
                return new PacketResult(PacketResultType.Block);
            }

            if ((bool)!getfleaItem.isActive)
            {
                await session.SendNotice("Item not found please refresh the search!");
                return new PacketResult(PacketResultType.Block);
            }

            int lastFreeSlot =
                await (from I in db.C_Inventory
                       join C in db.C_Char on session.SessionData.Charid equals C.CharID
                       where I.CharID == session.SessionData.Charid && I.ItemID == 0 && I.Slot >= 13 && I.Slot < C.InventorySize
                       orderby I.Slot
                       select I.Slot).FirstOrDefaultAsync();


            int itemCount = SharedObjects.StallNetwork.Where(x => x.Value.sellerName == getfleaItem.sellerName).Count();
            if (itemCount > 1)
            {
                session.SessionData.fleablockStall = false;
                await session.PurchaseStallNetwork(getfleaItem, (byte)lastFreeSlot, true);
            }
            else
            {
                session.SessionData.fleablockStall = true;
                await session.PurchaseStallNetwork(getfleaItem, (byte)lastFreeSlot, false);
            }

            //making sure that the item has been purchased successfully
            //await Task.Delay(500);
            //int lastFreeSlotAlt =
            //   await (from I in db.C_Inventory
            //          join C in db.C_Char on session.SessionData.Charid equals C.CharID
            //          where I.CharID == session.SessionData.Charid && I.ItemID == 0 && I.Slot >= 13 && I.Slot < C.InventorySize
            //          orderby I.Slot
            //          select I.Slot).FirstOrDefaultAsync();
            //if (lastFreeSlot != lastFreeSlotAlt)
            //{
            
            //}
            SharedObjects.StallNetwork.Remove(fleaitemID);
            await session.SendStallNetwork(session.SessionData.lastfleaTID, session.SessionData.lastfleaPageID, session.SessionData.lastfleaitemDG, SharedObjects.StallNetwork);

        }
        else
            await session.SendNotice("Item has been removed please refresh the search!");

        return new PacketResult(PacketResultType.Block);
    }
    private async Task<PacketResult> CLIENT_STALL_NETWORK_CLOSE(Packet packet, ISession session)
    {
        session.SessionData.stallNetwork = false;

        return new PacketResult(PacketResultType.Block);
    }
    private async Task<PacketResult> SERVER_STALL_ACTION(Packet packet, ISession session)
    {
        byte type = packet.ReadUInt8();

        switch (type)
        {
            case 0x01:
            case 0x02:
                //since we bypassing the distance protection of entering stall, its important not to let client enter his stall
                uint uniqueviewerID = packet.ReadUInt32();
                var getViewer = SharedObjects.AgentSessions.Where(x => x.SessionData.UniqueCharId == uniqueviewerID).FirstOrDefault();
                if (getViewer != null && getViewer.SessionData.stallNetwork)
                    return new PacketResult(PacketResultType.Block);
                break;

        }

        return new PacketResult(PacketResultType.Nothing);
    }
    #endregion

    #region CHARACTER RANKING
    private async Task<PacketResult> CLIENT_CHAR_RANKING(Packet packet, ISession session)
    {
        try
        {
            var rankType = packet.ReadInt32();
            var pageIndex = packet.ReadInt32();
            var RankingRecords = SharedObjects.CharRanking[rankType].CharRankingRecords.Take(100).ToList();
            var sendList = RankingRecords.OrderByDescending(x => x.Points).Skip((pageIndex - 1) * 11).Take(11);

            Packet rank = new Packet(0x180C);
            rank.WriteInt8(RankingRecords.Count());
            rank.WriteInt8(sendList.Count());

            foreach (var item in sendList)
            {
                rank.WriteUInt8(RankingRecords.IndexOf(item));
                rank.WriteAscii(item.Charname);
                rank.WriteUInt8(item.Race);
                rank.WriteUInt8(item.Level);
                rank.WriteUInt32(item.Points);
                rank.WriteAscii(item.Guild);

            }
            await session.SendToClient(rank);
        }
        catch (Exception ex)
        {
            Global.Logger.Info(ex.ToString());
        }
        return new PacketResult(PacketResultType.Block);
    }
    #endregion

    #region UNIQUE LOG
    private async Task<PacketResult> CLIENT_UNIQUE_LOG(Packet packet, ISession session)
    {
        byte pageIndex = packet.ReadUInt8();

        List<KeyValuePair<int, UniqueLog>> sendList = new List<KeyValuePair<int, UniqueLog>>();
        sendList = SharedObjects.UniqueLog.OrderByDescending(x => x.Value.Date).ToList();

        Packet uLog = new Packet(0x180D);
        uLog.WriteInt16(sendList.Count);

        sendList = sendList.Skip((pageIndex - 1) * 10).Take(10).ToList();
        uLog.WriteUInt8(sendList.Count);

        foreach (var item in sendList)
        {
            uLog.WriteInt32(item.Value.refObjID);
            uLog.WriteAscii(item.Value.Killer);
            uLog.WriteUInt8(item.Value.State);

            string spawnTime = $"{(int)DateTime.Now.Subtract((DateTime)item.Value.Date).Days}d{(int)DateTime.Now.Subtract((DateTime)item.Value.Date).Hours}h{(int)DateTime.Now.Subtract((DateTime)item.Value.Date).Minutes}m ago";
            uLog.WriteAscii(spawnTime);
        }

        await session.SendToClient(uLog);
        return new PacketResult(PacketResultType.Block);
    }

    private async Task<PacketResult> SERVER_GAME_NOTIFY(Packet packet, ISession session)
    {
        //TODO: replace it with database commands since this packet is executed for all players
        int type = packet.ReadInt8();
        packet.ReadInt8();

        int refObjID = 0;
        string killer = "<No Killer>";
        bool state = true;

        switch (type)
        {
            case 0x05:
                refObjID = packet.ReadInt32();
                break;
            case 0x06:
                int k_MOBID = packet.ReadInt32();
                string killerName = packet.ReadAscii();
                if (killerName == session.SessionData.Charname)
                {
                    refObjID = k_MOBID;
                    killer = killerName;
                    state = false;
                }
                break;
        }

        if (refObjID > 0)
        {
            if (SharedObjects.UniqueLog.ContainsKey(refObjID))
            {
                var uLog = SharedObjects.UniqueLog[refObjID];
                uLog.State = state;
                uLog.Killer = killer;
                uLog.Date = DateTime.Now;
            }
            else
            {
                SharedObjects.UniqueLog.Add(refObjID, new UniqueLog()
                {
                    refObjID = refObjID,
                    State = state,
                    Killer = killer,
                    Date = DateTime.Now
                });
            }
        }

        return new PacketResult(PacketResultType.Nothing);
    }
    #endregion

    #region EVENT SCHEDULE
    private async Task<PacketResult> CLIENT_EVENT_SCHEDULE(Packet packet, ISession session)
    {
        byte pageIndex = packet.ReadUInt8();

        Packet eventPck = new Packet(0x180E);
        eventPck.WriteUInt8(SharedObjects.Events.Count);

        var sendList = SharedObjects.Events.OrderBy(x => CrontabSchedule.Parse(x.Crontime).GetNextOccurrence(DateTime.Now)).Skip((pageIndex - 1) * 10).Take(10).ToList();
        eventPck.WriteUInt8(sendList.Count);

        foreach (var item in sendList)
        {
            eventPck.WriteAscii(item.Eventname);
            var schedule = CrontabSchedule.Parse(item.Crontime);
            var nextOccur = schedule.GetNextOccurrence(DateTime.Now);
            string nextOccurence = $"{Math.Abs(DateTime.Now.Subtract(nextOccur).Days)}d{Math.Abs(DateTime.Now.Subtract(nextOccur).Hours)}h{Math.Abs(DateTime.Now.Subtract(nextOccur).Minutes)}m Remaining";
            eventPck.WriteAscii(nextOccurence);
        }

        await session.SendToClient(eventPck);
        return new PacketResult(PacketResultType.Block);
    }
    #endregion

    #region FIRST SPAWN 
    private async Task<PacketResult> SERVER_COMMUNITY_FRIEND_INFO(Packet packet, ISession session)
    {
        try
        {
            if (!session.FirstSpawn)
            {
                session.FirstSpawn = true;

                //todo make the packets static and retreive it one time
                List<Packet> Packets = new List<Packet>();

                Packet CustomIconData = new Packet(0x180F);
                CustomIconData.WriteInt32(SharedObjects.CustomIconData.Count);
                foreach (var icon in SharedObjects.CustomIconData)
                {
                    CustomIconData.WriteInt32(icon.Value.IconID);
                    CustomIconData.WriteAscii(icon.Key);
                }
                Packets.Add(CustomIconData);

                Packet CustomIcon = new Packet(0x181A);
                CustomIcon.WriteInt32(SharedObjects.CustomIcon.Count);
                foreach (var icon in SharedObjects.CustomIcon)
                {
                    CustomIcon.WriteInt32(icon.Value.IconID);
                    CustomIcon.WriteAscii(icon.Key);
                }
                Packets.Add(CustomIcon);

                Packet CustomIconVipData = new Packet(0x181B);
                CustomIconVipData.WriteInt32(SharedObjects.CustomIconVipData.Count);
                foreach (var icon in SharedObjects.CustomIconVipData)
                {
                    CustomIconVipData.WriteInt32(icon.Value.IconID);
                    CustomIconVipData.WriteAscii(icon.Key);
                }
                Packets.Add(CustomIconVipData);

                Packet CustomIconVip = new Packet(0x181C);
                CustomIconVip.WriteInt32(SharedObjects.CustomIconVip.Count);
                foreach (var icon in SharedObjects.CustomIconVip)
                {
                    CustomIconVip.WriteInt32(icon.Value.IconID);
                    CustomIconVip.WriteAscii(icon.Key);
                }
                Packets.Add(CustomIconVip);

                Packet CustomName = new Packet(0x181D);
                CustomName.WriteInt32(SharedObjects.CustomName.Count);
                foreach (var icon in SharedObjects.CustomName)
                {
                    CustomName.WriteAscii(icon.Key);
                    CustomName.WriteAscii(icon.Value.Rank);
                }
                Packets.Add(CustomName);

                Packet CustomNameColor = new Packet(0x181E);
                CustomNameColor.WriteInt32(SharedObjects.CustomNameColor.Count);
                foreach (var icon in SharedObjects.CustomNameColor)
                {
                    CustomNameColor.WriteInt32((int)new System.ComponentModel.Int32Converter().ConvertFromString(icon.Value.Color));
                    CustomNameColor.WriteAscii(icon.Key);
                }
                Packets.Add(CustomNameColor);

                Packet CustomTitle = new Packet(0x181F);
                CustomTitle.WriteInt32(SharedObjects.CustomTitle.Count);
                foreach (var item in SharedObjects.CustomTitle)
                {
                    CustomTitle.WriteAscii(item.Key);
                    CustomTitle.WriteAscii(item.Value.Title);
                }
                Packets.Add(CustomTitle);

                Packet CustomTitleColor = new Packet(0x182A);
                CustomTitleColor.WriteInt32(SharedObjects.CustomTitleColor.Count);
                foreach (var icon in SharedObjects.CustomTitleColor)
                {
                    CustomTitleColor.WriteInt32((int)new System.ComponentModel.Int32Converter().ConvertFromString(icon.Value.Color));
                    CustomTitleColor.WriteAscii(icon.Key);
                }
                Packets.Add(CustomTitleColor);

                Packet RankInfo = new Packet(0x183C);
                RankInfo.WriteInt32(SharedObjects.CharRanking.Count);
                foreach (var item in SharedObjects.CharRanking)
                {
                    RankInfo.WriteInt32(item.Key);
                    RankInfo.WriteAscii(item.Value.RankName);
                }
                Packets.Add(RankInfo);

                await session.SendToClient(Packets);

                using var db = new API.Database.DuckSoup.DuckSoup();
                session.SessionData.chestItems = await db.ItemChest.Where(x => x.Charname == session.SessionData.Charname).OrderByDescending(x => x.Date).ToListAsync();
                session.SessionData.ownTitles = await db.TitleStorage.Where(x => x.CharName == session.SessionData.Charname).Select(t => t.Title).ToListAsync();

                if (SharedObjects.DailyReward.Count > 0)
                {
                    var entity = SharedObjects.DailyReward.Where(x => x.DailyRewardRecords.Any(x => x.Charname == session.SessionData.Charname)).OrderByDescending(x => x.ID).FirstOrDefault();
                    if (entity != null)
                    {
                        var lastRecord = entity.DailyRewardRecords.Where(x => x.Charname == session.SessionData.Charname).OrderByDescending(x => x.TakenDate).FirstOrDefault();
                        if (lastRecord != null)
                        {
                            var date = lastRecord.TakenDate;
                            if (DateTime.Now.Subtract(date).TotalDays >= 1)
                            {
                                int Index = SharedObjects.DailyReward.IndexOf(entity);
                                if (Index < SharedObjects.DailyReward.Count - 1)
                                {
                                    var Day = SharedObjects.DailyReward[Index + 1].ID;
                                    var newRecord = new DailyRewardRecords
                                    {
                                        Charname = session.SessionData.Charname,
                                        Day = Day,
                                        isTaken = false,
                                        TakenDate = DateTime.Now
                                    };
                                    db.DailyRewardRecords.Add(newRecord);
                                    entity.DailyRewardRecords.Add(newRecord);
                                    var enttiy2 = SharedObjects.DailyReward[SharedObjects.DailyReward.IndexOf(entity) + 1];
                                    enttiy2.DailyRewardRecords.Add(newRecord);
                                }

                            }
                        }
                    }
                    else
                    {
                        var newRecord = new DailyRewardRecords
                        {
                            Charname = session.SessionData.Charname,
                            Day = SharedObjects.DailyReward[0].ID,
                            isTaken = false,
                            TakenDate = DateTime.Now
                        };
                        db.DailyRewardRecords.Add(newRecord);
                        SharedObjects.DailyReward[0].DailyRewardRecords.Add(newRecord);
                    }
                }

                await db.SaveChangesAsync();
            }

        }
 
        catch(Exception ex)
        {
            Global.Logger.Info(ex.ToString());
        }
        return new PacketResult();
    }
    #endregion

    #region ITEM CHEST
    private async Task<PacketResult> CLIENT_ITEM_CHEST(Packet packet, ISession session)
    {
        try
        {
            byte Type = packet.ReadUInt8();
            byte pageIndex = packet.ReadUInt8();

            switch (Type)
            {
                case 1:
                    {
                        //take
                        using var shardDB = new API.Database.SRO_VT_SHARD.SRO_VT_SHARD();
                        int remainSlots =
                               await (from I in shardDB.C_Inventory
                                      join C in shardDB.C_Char on session.SessionData.Charid equals C.CharID
                                      where I.CharID == session.SessionData.Charid && I.ItemID == 0 && I.Slot >= 13 && I.Slot < C.InventorySize
                                      select I.Slot).CountAsync();

                        if (remainSlots > 0)
                        {
                            int index = packet.ReadInt32();

                            var item = session.SessionData.chestItems[index];

                            if (item != null)
                            {
                                using var db = new API.Database.DuckSoup.DuckSoup();
                                var itemtoDel = await db.ItemChest.SingleOrDefaultAsync(x => x.ID == item.ID);

                                session.SessionData.chestItems.RemoveAt(index);
                                db.ItemChest.Remove(itemtoDel);

                                await db.SaveChangesAsync();
                            }
                        }

                    }
                    break;
                case 2:
                    {
                        //take all
                        if (session.SessionData.chestItems.Count > 0)
                        {
                            using var shardDB = new API.Database.SRO_VT_SHARD.SRO_VT_SHARD();
                            int remainSlots =
                                   await (from I in shardDB.C_Inventory
                                          join C in shardDB.C_Char on session.SessionData.Charid equals C.CharID
                                          where I.CharID == session.SessionData.Charid && I.ItemID == 0 && I.Slot >= 13 && I.Slot < C.InventorySize
                                          select I.Slot).CountAsync();

                            var itemstoDel = session.SessionData.chestItems.Take(remainSlots).ToList();

                            using var db = new API.Database.DuckSoup.DuckSoup();
                            foreach (var item in itemstoDel)
                            {
                                var itemtoDel = await db.ItemChest.SingleOrDefaultAsync(x => x.ID == item.ID);

                                session.SessionData.chestItems.Remove(item);
                                db.ItemChest.Remove(itemtoDel);
                            }
                            await db.SaveChangesAsync();
                        }
                    }
                    break;
            }
            var chestItemsExt = session.SessionData.chestItems.Skip((pageIndex - 1) * 10).Take(10).ToList();
            Packet chest = new Packet(0x182B);

            chest.WriteInt32(session.SessionData.chestItems.Count);
            chest.WriteUInt8(chestItemsExt.Count);

            foreach (var item in chestItemsExt)
            {
                chest.WriteInt32(session.SessionData.chestItems.IndexOf(item));
                chest.WriteInt32(item.ItemID);
                chest.WriteAscii(item.From);
                chest.WriteInt32(item.Count);

                string dateAdded = $"{(int)DateTime.Now.Subtract((DateTime)item.Date).Days}d{(int)DateTime.Now.Subtract((DateTime)item.Date).Hours}h{(int)DateTime.Now.Subtract((DateTime)item.Date).Minutes}m ago";
                chest.WriteAscii(dateAdded);
            }

            await session.SendToClient(chest);
        }
        catch (Exception ex)
        {
            Global.Logger.Error($"{ex.ToString()}");
        }
        return new PacketResult(PacketResultType.Block);
    }

    #endregion

    #region TITLES & ACHIEVEMENTS
    private async Task<PacketResult> CLIENT_TITLE(Packet packet, ISession session)
    {
        byte Type = packet.ReadUInt8();
        switch (Type)
        {
            case 0:
                {
                    Packet titleList = new Packet(0x182C);
                    titleList.WriteUInt32(session.SessionData.ownTitles.Count);
                    foreach (var item in session.SessionData.ownTitles)
                        titleList.WriteAscii(item);
                    await session.SendToClient(titleList);
                }
                break;
            case 1:
                {
                    string title = packet.ReadAscii();
                    using var db = new API.Database.DuckSoup.DuckSoup();
                    if (await db.TitleStorage.AnyAsync(x => x.Title == title && x.CharName == session.SessionData.Charname))
                    {
                        var entity = await db.CustomTitle.Where(x => x.CharName == session.SessionData.Charname).FirstOrDefaultAsync();
                        if (entity != null)
                        {
                            entity.Title = title;
                        }
                        else
                        {
                            var newCustom = new CustomTitle()
                            {
                                CharName = session.SessionData.Charname,
                                Title = title,
                            };
                            db.CustomTitle.Add(newCustom);
                        }
                    }
                    await db.SaveChangesAsync();
                }
                break;
            case 2:
                {
                    using var db = new API.Database.DuckSoup.DuckSoup();
                    var entity = await db.CustomTitle.Where(x => x.CharName == session.SessionData.Charname).FirstOrDefaultAsync();
                    if (entity != null)
                        db.CustomTitle.Remove(entity);
                    await db.SaveChangesAsync();
                }
                break;
        }

        return new PacketResult(PacketResultType.Block);
    }

    private async Task<PacketResult> CLIENT_ACHEIEVMENT(Packet packet, ISession session)
    {
        try
        {
            byte Type = packet.ReadUInt8();
            switch (Type)
            {
                case 0:
                    {
                        Packet achievList = new Packet(0x182E);
                        achievList.WriteUInt32(SharedObjects.Achievements.Count);
                        foreach (var item in SharedObjects.Achievements)
                        {
                            achievList.WriteAscii(item.Rank);
                            string Descr = string.Empty;
                            var record = item.AchievementsRecords.Where(x => x.Charname == session.SessionData.Charname).FirstOrDefault();
                            if (record != null)
                                Descr = item.Descr.Replace("%count", record.Points.ToString()).Replace("%max", item.MaxPoints.ToString());
                            else
                                Descr = item.Descr.Replace("%count", "0").Replace("%max", item.MaxPoints.ToString());

                            achievList.WriteAscii(Descr);
                            if (record != null && record.Points >= item.MaxPoints)
                                achievList.WriteUInt8(1);
                            else
                                achievList.WriteUInt8(0);
                        }
                        await session.SendToClient(achievList);
                    }
                    break;
                case 1:
                    {
                        string rank = packet.ReadAscii();

                        using var db = new API.Database.DuckSoup.DuckSoup();

                        var item = SharedObjects.Achievements.Where(x => x.Rank == rank && x.AchievementsRecords.Any(x => x.Charname == session.SessionData.Charname)).FirstOrDefault();
                        if (item != null)
                        {
                            var entity = await db.CustomName.Where(x => x.Charname == session.SessionData.Charname).FirstOrDefaultAsync();
                            if (entity != null)
                            {
                                entity.Rank = rank;
                            }
                            else
                            {
                                var newCustom = new CustomName()
                                {
                                    Charname = session.SessionData.Charname,
                                    Rank = rank,
                                };
                                db.CustomName.Add(newCustom);
                            }
                            await db.SaveChangesAsync();
                        }
                    }
                    break;
                case 2:
                    {
                        using var db = new API.Database.DuckSoup.DuckSoup();
                        var entity = await db.CustomName.Where(x => x.Charname == session.SessionData.Charname).FirstOrDefaultAsync();
                        if (entity != null)
                        {
                            db.CustomName.Remove(entity);
                            await db.SaveChangesAsync();
                        }
                    }
                    break;
            }

        }

        catch (Exception ex)
        {
            Global.Logger.Fatal($"{ex.ToString()}");
        }
        return new PacketResult(PacketResultType.Block);
    }

    private async Task<PacketResult> CLIENT_PURCHASE_CUSTOMS(Packet packet, ISession session)
    {
        using var db_acc = new SRO_VT_ACCOUNT();
        var ownSilk =
            (await db_acc.SK_Silk.Where(x => x.JID == session.SessionData.JID).FirstAsync()).silk_own;

        if (ownSilk < 500)
        {
            await session.SendNotice("Insufficient silk.");
            return new PacketResult(PacketResultType.Block);
        }

        var type = packet.ReadUInt8();
        switch (type)
        {
            case 0:
                {
                    var titleName = packet.ReadAscii();

                    //todo check for special characters
                    if (titleName.Length == 0 || titleName.Length > 20)
                    {
                        await session.SendNotice("Check your title length.");
                        return new PacketResult(PacketResultType.Block);
                    }

                    using var db_duck = new API.Database.DuckSoup.DuckSoup();
                    if (await db_duck.TitleStorage.AnyAsync(x => x.CharName == session.SessionData.Charname && x.Title == titleName))
                    {
                        await session.SendNotice("You already have this title.");
                        return new PacketResult(PacketResultType.Block);
                    }

                    var entityTitle = await db_duck.CustomTitle.Where(x => x.CharName == session.SessionData.Charname).FirstOrDefaultAsync();
                    if (entityTitle != null)
                    {
                        entityTitle.Title = titleName;
                    }
                    else
                    {
                        var newCustom = new CustomTitle()
                        {
                            CharName = session.SessionData.Charname,
                            Title = titleName,
                        };
                        db_duck.CustomTitle.Add(newCustom);
                    }

                    var Custom = new TitleStorage()
                    {
                        CharName = session.SessionData.Charname,
                        Title = titleName,
                    };
                    db_duck.TitleStorage.Add(Custom);

                    await db_duck.SaveChangesAsync();
                }
                break;
            case 1:
                {
                    int titleColor = packet.ReadInt32();
                    if (titleColor != 0)
                    {
                        using var db_duck = new API.Database.DuckSoup.DuckSoup();

                        var entityColor = await db_duck.CustomTitleColor.Where(x => x.Charname == session.SessionData.Charname).FirstOrDefaultAsync();
                        if (entityColor != null)
                        {
                            entityColor.Color = titleColor.ToString();
                        }
                        else
                        {
                            var newCustom = new CustomTitleColor()
                            {
                                Charname = session.SessionData.Charname,
                                Color = titleColor.ToString()
                            };
                            db_duck.CustomTitleColor.Add(newCustom);
                        }

                        await db_duck.SaveChangesAsync();
                    }
                }
                break;
            case 2:
                {
                    int nameColor = packet.ReadInt32();
                    if (nameColor != 0)
                    {
                        using var db_duck = new API.Database.DuckSoup.DuckSoup();

                        var entityColor = await db_duck.CustomNameColor.Where(x => x.Charname == session.SessionData.Charname).FirstOrDefaultAsync();
                        if (entityColor != null)
                        {
                            entityColor.Color = nameColor.ToString();
                        }
                        else
                        {
                            var newCustom = new CustomNameColor()
                            {
                                Charname = session.SessionData.Charname,
                                Color = nameColor.ToString()
                            };
                            db_duck.CustomNameColor.Add(newCustom);
                        }
                        await db_duck.SaveChangesAsync();
                    }
                }
                break;
        }

        int lastOrder = 0;
        var entity = (await db_acc.SK_SilkBuyList.OrderByDescending(x => x.OrderNumber).FirstOrDefaultAsync());

        if (entity != null)
            lastOrder = Convert.ToInt32(entity.OrderNumber);

        await db_acc.Database.ExecuteSqlCommandAsync($"EXEC CGI.CGI_WebPurchaseSilk '{lastOrder + 1}', '{session.SessionData.Username}','1','-500',1");

        return new PacketResult(PacketResultType.Block);
    }

    #endregion

    #region DAILY REWARD
    private async Task<PacketResult> CLIENT_DAILY_REWARD(Packet packet, ISession session)
    {
        byte type = packet.ReadUInt8();

        if(type == 0x00)
        {
            int pageIndex = packet.ReadInt32();
            var customList = SharedObjects.DailyReward.Skip((pageIndex - 1) * 6).Take(6).OrderBy(x => x.ID).ToList();

            Packet pck = new Packet(0x182F);

            pck.WriteInt32(customList.Count);
            pck.WriteInt32(SharedObjects.DailyReward.Count);
            foreach(var item in customList)
            {
                var itemRecord = item.DailyRewardRecords.Where(x => x.Charname == session.SessionData.Charname).FirstOrDefault();
                pck.WriteInt32(SharedObjects.DailyReward.IndexOf(item));
                pck.WriteInt32(item.ItemID);
                pck.WriteInt32(item.Count);
                if (itemRecord != null)
                    pck.WriteUInt8(itemRecord.isTaken);
                else
                    pck.WriteUInt8(0x03);
            }

            await session.SendToClient(pck);
        }
        else
        {
            int index = packet.ReadInt32();
            using var db = new API.Database.DuckSoup.DuckSoup();
            index = SharedObjects.DailyReward[index].ID;
            var entity = await db.DaiyReward.Where(x => x.ID == index).FirstOrDefaultAsync();
            var record = entity.DailyRewardRecords.Where(x => x.isTaken == false && x.Charname == session.SessionData.Charname).FirstOrDefault();
            if (record != null)
            {
                record.isTaken = true;
                var newRecord = new ItemChest
                {
                    Charname = session.SessionData.Charname,
                    Count = entity.Count,
                    From = "Daily Reward",
                    Date = DateTime.Now,
                    ItemID = entity.ItemID
                };
                db.ItemChest.Add(newRecord);
                SharedObjects.DailyReward.Where(x => x.ID == index).FirstOrDefault().DailyRewardRecords.Where(x => x.isTaken == false && x.Charname == session.SessionData.Charname).FirstOrDefault().isTaken = true;
            }
            await db.SaveChangesAsync();
        }
        return new PacketResult(PacketResultType.Block);
    }
    #endregion

    #region LOGIN REQUEST
    private async Task<PacketResult> CLIENT_LOGIN_REQUEST(Packet packet, ISession session)
    {
        packet.ReadUInt32();
        string username = packet.ReadAscii().ToLower(); 
        if (username.Contains("\'") || username.Contains("\"") || username.Contains("-"))
        {
            return new PacketResult(PacketResultType.Disconnect);
        }
        session.SessionData.Username = username;
        return new PacketResult(PacketResultType.Nothing);
    }
    #endregion

    private async Task<PacketResult> CLIENT_HWID(Packet packet, ISession session)
    {
        string hwid = packet.ReadAscii();

        StringBuilder hwid2 = new StringBuilder(hwid);
        for (int i = 0; i < hwid.Length; i++)
        {
            hwid2[i] = (char)(hwid[i] ^ session.hwidHelper[i]);
        }
        session.Hwid = hwid2.ToString();

        if (!session.Hwid.EndsWith("1999"))
        {
            Packet acclock = new Packet(0x183B);
            acclock.WriteUInt8(0x05);
            await session.SendToClient(acclock);
            return new PacketResult(PacketResultType.Disconnect);
        }
        if(Library.SharedObjects.settings["PCLimit"] > 0)
        {
            var hwidCount = SharedObjects.AgentSessions.Count(Session => Session.Hwid == session.Hwid);
            if (hwidCount > Library.SharedObjects.settings["PCLimit"])
            {
                Packet acclock = new Packet(0x183B);
                acclock.WriteUInt8(0x06);
                await session.SendToClient(acclock);
                return new PacketResult(PacketResultType.Disconnect);
            }
        }

        if (SharedObjects.AccountLock.ContainsKey(session.SessionData.Username))
        {
            if (SharedObjects.AccountLock[session.SessionData.Username].AccountLockRecords.Any(x => x.hwid == session.Hwid))
            {
                session.Verified = true;
                await session.SendToClient(session.TempPacket);
            }
            else
            {
                Packet acclock = new Packet(0x183B);
                acclock.WriteUInt8(0x03);
                await session.SendToClient(acclock);
            }
        }
        else
        {
            Packet acclock = new Packet(0x183B);
            acclock.WriteUInt8(0x02);
            await session.SendToClient(acclock);
        }
        return new PacketResult(PacketResultType.Block);
    }
    private async Task<PacketResult> SERVER_AGENT_CHARACTER_SELECTION_RESPONSE(Packet packet, ISession session)
    {
        if(string.IsNullOrEmpty(session.Hwid))
        {
            session.Verified = false;
            Random rand = new Random();
            session.hwidHelper = new byte[72];

            Packet hwid = new Packet(0x183A);
            for (int i = 0; i < session.hwidHelper.Length; i++)
            {
                session.hwidHelper[i] = (byte)rand.Next(1, 9);
                hwid.WriteUInt8(session.hwidHelper[i]);
            }
            await session.SendToClient(hwid);

            session.TempPacket = packet;
            return new PacketResult(PacketResultType.Block);
        }
        else
            return new PacketResult(PacketResultType.Nothing);

    }
    private async Task<PacketResult> CLIENT_ACC_LOCK(Packet packet, ISession session)
    {
        if (string.IsNullOrEmpty(session.Hwid))
        {
            return new PacketResult(PacketResultType.Disconnect);
        }

        byte type = packet.ReadUInt8();

        switch (type)
        {
            case 0:
                {
                    string pw = packet.ReadAscii();
                    string pw_confirm = packet.ReadAscii();

                    if (pw.Length < 6 || pw.Length > 8)
                    {
                        Packet errorcode0 = new Packet(0x183B);
                        errorcode0.WriteUInt8(0x00);
                        errorcode0.WriteUInt8(0x00);
                        await session.SendToClient(errorcode0);

                        return new PacketResult(PacketResultType.Block);
                    }

                    if (pw != pw_confirm)
                    {
                        Packet errorcode1 = new Packet(0x183B);
                        errorcode1.WriteUInt8(0x00);
                        errorcode1.WriteUInt8(0x01);
                        await session.SendToClient(errorcode1);

                        return new PacketResult(PacketResultType.Block);
                    }

                    var record = new AccountLockRecords
                    {
                        hwid = session.Hwid
                    };

                    var list = new List<AccountLockRecords>();
                    list.Add(record);

                    SharedObjects.AccountLock.Add(session.SessionData.Username,
                        new AccountLock
                        {
                            Username = session.SessionData.Username,
                            Passcode = pw,
                            AccountLockRecords = list
                        });

                    using var db = new API.Database.DuckSoup.DuckSoup();
                    db.AccountLock.Add(new AccountLock
                    {
                        Username = session.SessionData.Username,
                        Passcode = pw,
                        AccountLockRecords = list
                    });

                    await db.SaveChangesAsync();

                    Packet success = new Packet(0x183B);
                    success.WriteUInt8(0x01);
                    success.WriteUInt8(0x00);
                    await session.SendToClient(success);
                }
                break;
            case 1:
                {
                    string oldPw = packet.ReadAscii();
                    string newPw = packet.ReadAscii();
                    string newPwConfirm = packet.ReadAscii();
                    bool deleteAll = packet.ReadBool();

                    if (newPw.Length < 6 || newPw.Length > 8)
                    {
                        Packet errorcode0 = new Packet(0x183B);
                        errorcode0.WriteUInt8(0x00);
                        errorcode0.WriteUInt8(0x00);
                        await session.SendToClient(errorcode0);

                        return new PacketResult(PacketResultType.Block);
                    }
                    if(newPw != newPwConfirm)
                    {
                        Packet errorcode1 = new Packet(0x183B);
                        errorcode1.WriteUInt8(0x00);
                        errorcode1.WriteUInt8(0x01);
                        await session.SendToClient(errorcode1);

                        return new PacketResult(PacketResultType.Block);
                    }
                    if(SharedObjects.AccountLock.ContainsKey(session.SessionData.Username))
                    {
                        if(SharedObjects.AccountLock[session.SessionData.Username].Passcode != oldPw)
                        {
                            Packet errorcode2 = new Packet(0x183B);
                            errorcode2.WriteUInt8(0x00);
                            errorcode2.WriteUInt8(0x02);
                            await session.SendToClient(errorcode2);

                            return new PacketResult(PacketResultType.Block);
                        }

                        using var db = new API.Database.DuckSoup.DuckSoup();
                        var entity = await db.AccountLock.Where(x => x.Username == session.SessionData.Username).FirstOrDefaultAsync();
                        entity.Passcode = newPw;
                        SharedObjects.AccountLock[session.SessionData.Username].Passcode = newPw;

                        if(deleteAll)
                        {
                            int ID = SharedObjects.AccountLock[session.SessionData.Username].ID;
                            SharedObjects.AccountLock[session.SessionData.Username].AccountLockRecords.Clear();
                            db.AccountLockRecords.RemoveRange(db.AccountLockRecords.Where(x => x.AccountLock_ID == ID));
                        }

                        await db.SaveChangesAsync();
                    }

                    Packet success = new Packet(0x183B);
                    success.WriteUInt8(0x01);
                    success.WriteUInt8(0x01);
                    await session.SendToClient(success);
                }
                break;
            case 2:
                {
                    string pw = packet.ReadAscii();
                    bool savePC = packet.ReadBool();

                    if (SharedObjects.AccountLock[session.SessionData.Username].Passcode != pw)
                    {
                        Packet errorcode2 = new Packet(0x183B);
                        errorcode2.WriteUInt8(0x00);
                        errorcode2.WriteUInt8(0x02);
                        await session.SendToClient(errorcode2);

                        return new PacketResult(PacketResultType.Block);
                    }

                    session.Verified = true;

                    if (savePC)
                        if (!SharedObjects.AccountLock[session.SessionData.Username].AccountLockRecords.Any(x=>x.hwid == session.Hwid))
                        {
                            using var db = new API.Database.DuckSoup.DuckSoup();

                            var record = new AccountLockRecords
                            {
                                hwid = session.Hwid
                            };

                            SharedObjects.AccountLock[session.SessionData.Username].AccountLockRecords.Add(record);

                            var entity = await db.AccountLock.Where(x => x.Username == session.SessionData.Username).FirstOrDefaultAsync();
                            entity.AccountLockRecords.Add(record);

                            await db.SaveChangesAsync();
                        }

                    await session.SendToClient(session.TempPacket);

                    Packet success = new Packet(0x183B);
                    success.WriteUInt8(0x04);
                    await session.SendToClient(success);
                }
                break;
        }

        return new PacketResult(PacketResultType.Block);
    }
    private async Task<PacketResult> SERVER_GAMESERVER_HANDLE_DPS_INFO(Packet packet, ISession session)
    {
        GameServerMsgHandler GSAddon = new GameServerMsgHandler();
        GSAddon.HandleUniqueMonsterDamageInfo(packet);

        return new PacketResult(PacketResultType.Block);
    }
    private async Task<PacketResult> CLIENT_MOVE_TO_PARTY_MEMBER(Packet packet , ISession session)
    {
        //todo check if the player in the party
        //todo handle it from gameserver
        byte invSlot = (byte)(packet.ReadUInt8() + 13);
        var charname = packet.ReadAscii();

        using var db = new SRO_VT_SHARD();

        int RefItemID =
            await (from I in db.C_Items
                   join V in db.C_Inventory on I.ID64 equals V.ItemID
               where V.Slot == invSlot && V.CharID == session.SessionData.Charid
               select I.RefItemID).FirstOrDefaultAsync();

        var Reverse = SharedObjects.RefObjCommon[RefItemID];

        var ReverseTID = Reverse.CashItem 
            + (Reverse.TypeID1 * 4) + (Reverse.TypeID2 * 32) + (Reverse.TypeID3 * 128) + (Reverse.TypeID4 * 2048);

        if(ReverseTID == 0x19ED || ReverseTID == 0x19EC)
        {
            var charSession = SharedObjects.AgentSessions.FirstOrDefault(session => session.SessionData.Charname == charname &&
                                   session.CharacterGameReady);

            if(charSession != null)
            {
                await session.Sub_Item_Count(invSlot, 1);
                await Task.Delay(500);
                await session.MoveTo(charSession.SessionData.UniqueCharId);
            }
        }
        return new PacketResult(PacketResultType.Block);
    }
    private async Task<PacketResult> CLIENT_ALCHEMY_PACKET(Packet packet, ISession session)
    {
        if (packet.ReadUInt8() == 2 && packet.ReadUInt8() == 3)
        {
            packet.ReadUInt8(); // Unknown
            var itemSlot = packet.ReadUInt8();
            var elixirSlot = packet.ReadUInt8();

            using var shardDB = new API.Database.SRO_VT_SHARD.SRO_VT_SHARD();
            var itemType =
                   await (from R in shardDB.C_RefObjCommon
                          join I in shardDB.C_Items on R.ID equals I.RefItemID
                          join V in shardDB.C_Inventory on I.ID64 equals V.ItemID
                          where V.CharID == session.SessionData.Charid && V.Slot == itemSlot
                          select new
                          {
                              R.TypeID3,
                              I.OptLevel
                          }).FirstOrDefaultAsync();
           
            if(itemType != null)
            { 
                if(Library.SharedObjects.settings["MaxPlusDevil"] > 0 && itemType.TypeID3 ==14 && itemType.OptLevel >= Library.SharedObjects.settings["MaxPlusDevil"])
                {
                    await session.SendNotice(Library.SharedObjects.settings["MaxPlusDevil_Notice"]);
                    return new PacketResult(PacketResultType.Block);
                }
                else if(Library.SharedObjects.settings["MaxPlus"] > 0 && itemType.OptLevel >= Library.SharedObjects.settings["MaxPlus"])
                {
                    await session.SendNotice(Library.SharedObjects.settings["MaxPlus_Notice"]);
                    return new PacketResult(PacketResultType.Block);
                }
            }

        }
        return new PacketResult(PacketResultType.Nothing);

    }
}
