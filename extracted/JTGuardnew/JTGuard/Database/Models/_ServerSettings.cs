using System;
using System.Collections.Generic;
using System.Data.SqlClient;
using System.Threading.Tasks;
using JTGuard.AsyncServerManager;
using JTGuard.Server;
using JTGuard.SettingManager;

namespace JTGuard.Database.Model
{
    public class _ServerSettings
    {
        public string ServerIP { get; set; }
        public int DownloadRealPort { get; set; }
        public int DownloadFakePort { get; set; }
        public int GatewayRealPort { get; set; }
        public int GatewayFakePort { get; set; }
        public int AgentRealPort { get; set; }
        public int AgentFakePort { get; set; }
        public string AccountDB { get; set; }
        public string LogDB { get; set; }
        public string ShardDB { get; set; }
        public bool AutoStart { get; set; }
        public bool RemoveCaptcha { get; set; }
        public string CaptchaValue { get; set; }

        public bool OldLogin { get; set; }
        public bool OldExpBar { get; set; }
        public bool OldAlchemy { get; set; }
        public bool GrantNameButton { get; set; }
        public bool IconManagerButton { get; set; }
        public bool IconManagerRight { get; set; }
        public bool TitleManager { get; set; }
        public bool TitleManagerColor { get; set; }
        public bool DynamicRanking { get; set; }
        public bool UniqueHistory { get; set; }
        public bool EventRegister { get; set; }
        public bool EventSchedule { get; set; }
        public bool Achievements { get; set; }
        public bool SecondarySlot { get; set; }
        public bool MoveSkillBoard { get; set; }
        public bool ServerInfoSkill { get; set; }
        public bool OldMainPopup { get; set; }
        public bool HideOldTitleWhileNewTitle { get; set; }
        public bool ItemComparison { get; set; }
        public bool AutoSort { get; set; }
        public bool PartyMemberViewer { get; set; }
        public bool AutoSkillUpdate { get; set; }
        public int MasteryLimit { get; set; }
        public int ServerMaxLevel { get; set; }
        public bool FixDamageText { get; set; }
        public bool AutoStrInt { get; set; }
        public bool PickupEffect { get; set; }
        public bool PermanentAlchemy { get; set; }
        public bool ShowGuildInJobMode { get; set; }
        public bool UniqueTarget { get; set; }
        public bool Macro { get; set; }
        public bool SecondaryPassword { get; set; }
        public bool NewCharInfo { get; set; }
        public bool NewIdPw { get; set; }
        public string FacebookURL { get; set; }
        public string DiscordURL { get; set; }
        public string WebsiteURL { get; set; }
        public bool Changelog { get; set; }
        public bool ShowChangelogFirstSpawn { get; set; }
        public bool FixNewJobSuit { get; set; }
        public bool OldItemMall { get; set; }
        public bool InsertCommaPrices { get; set; }
        public bool WriteCharacterBound { get; set; }
        public bool NewItemMall { get; set; }
        public bool EmojiSystem { get; set; }
        public bool NewPartyMatch { get; set; }
        public bool NewJobUI { get; set; }
        public bool NewAlchemy { get; set; }
        public bool ShowOnlinePlayers { get; set; }
        public string ServerName { get; set; }
        public int FakePlayerCount { get; set; }
        public bool CheckStatus { get; set; }
        public int HWID_LIMIT { get; set; }
        public int HWID_JOB_LIMIT { get; set; }

        public int AlchemyItemLinkMinLevel { get; set; }

        public bool DisableReverseInJob { get; set; }
        public int ReverseDelay { get; set; }
        public int MaxPlus { get; set; }
        public bool DisableAcademy { get; set; }
        public bool DisableAutoAttack { get; set; }

        public int AutoAttackMaxLevel { get; set; }
        public bool DisableTraceWhileJob { get; set; }

        public int StallDelay { get; set; }
        public byte StallLevel { get; set; }
        public int ExchangeDelay { get; set; }
        public byte ExchangeLevel { get; set; }
        public int GuildInviteDelay { get; set; }
        public int UnionInviteDelay { get; set; }

        public int GlobalDelay { get; set; }
        public byte GlobalLevel { get; set; }
        public int LiveItemDelay { get; set; }
        public int TradePetSpawnDelay { get; set; }

        public int RestartDelay { get; set; }
        public int ExitDelay { get; set; }
        public bool EnableItemTranslation { get; set; }
        public byte ItemTranslationPayment { get; set; }

        public int ItemTranslationPrice { get; set; }
        public int SHOW_CHAR_INFO_DELAY { get; set; }
    }
}
