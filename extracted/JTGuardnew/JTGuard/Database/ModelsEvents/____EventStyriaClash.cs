using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JTGuard.Database.ModelsEvents
{
    public class ____EventStyriaClash
    {
        public int ID { get; set; }
        public int PCLimit { get; set; }
        public int MinPlayer { get; set; }
        public int WorldID { get; set; }
        public int RegionIDThief { get; set; }
        public int StartPosXThief { get; set; }
        public int StartPosYThief { get; set; }
        public int StartPosZThief { get; set; }
        public int RegionIDHunter { get; set; }
        public int StartPosXHunter { get; set; }
        public int StartPosYHunter { get; set; }
        public int StartPosZHunter { get; set; }
        public int WinnerRewardID { get; set; }
        public string WinnerRewardName { get; set; }
        public int WinnerRewardQuantity { get; set; }
        public int LoserRewardID { get; set; }
        public string LoserRewardName { get; set; }
        public int LoserRewardQuantity { get; set; }
        public int CharKillRewardID { get; set; }
        public string CharKillRewardName { get; set; }
        public int CharKillRewardQuantity { get; set; }
        public int CharKillPoint { get; set; }
        public int IntervalBoxSpawnMin { get; set; }
    }
}
