using System.Data.Entity;
using System.Data.Entity.ModelConfiguration.Conventions;

namespace API.Database.DuckSoup
{
    public partial class DuckSoup : DbContext
    {
        public DuckSoup()
            : base(DatabaseManager.DuckSoupConnectionString)
        {
            
        }
        public virtual DbSet<Setting> Settings { get; set; }
        public virtual DbSet<Machine> Machines { get; set; }
        public virtual DbSet<Service> Services { get; set; }
        public virtual DbSet<Blacklist> Blacklist { get; set; }
        public virtual DbSet<Whitelist> Whitelist { get; set; }
        public virtual DbSet<Event> Event { get; set; }
        public virtual DbSet<RegionInfo> RegionInfo { get; set; }
        public virtual DbSet<DefaultUniques> DefaultUniques { get; set; }
        public virtual DbSet<CustomIcon> CustomIcon { get; set; }
        public virtual DbSet<CustomIconData> CustomIconData { get; set; }
        public virtual DbSet<CustomIconVip> CustomIconVip { get; set; }
        public virtual DbSet<CustomIconVipData> CustomIconVipData { get; set; }
        public virtual DbSet<CustomName> CustomName { get; set; }
        public virtual DbSet<CustomNameColor> CustomNameColor { get; set; }
        public virtual DbSet<CustomTitle> CustomTitle { get; set; }
        public virtual DbSet<CustomTitleColor> CustomTitleColor { get; set; }
        public virtual DbSet<ItemChest> ItemChest { get; set; }
        public virtual DbSet<TitleStorage> TitleStorage { get; set; }
        public virtual DbSet<Achievements> Achievements { get; set; }
        public virtual DbSet<AchievementsRecords> AchievementsRecords { get; set; }
        public virtual DbSet<DailyReward> DaiyReward { get; set; }
        public virtual DbSet<DailyRewardRecords> DailyRewardRecords { get; set; }
        public virtual DbSet <AccountLock> AccountLock { get; set; }
        public virtual DbSet <AccountLockRecords> AccountLockRecords { get; set; }
        public virtual DbSet<CharRanking> CharRanking { get; set; }
        public virtual DbSet<CharRankingRecords> CharRankingRecords { get; set; }

        protected override void OnModelCreating(DbModelBuilder modelBuilder)
        {
            modelBuilder.Conventions.Remove<ManyToManyCascadeDeleteConvention>();
            modelBuilder.Conventions.Remove<OneToManyCascadeDeleteConvention>();

            // modelBuilder.Entity<GlobalSetting>()
            //     .Property(e => e.Name)
            //     .IsUnicode(false);
        }
    }
}
