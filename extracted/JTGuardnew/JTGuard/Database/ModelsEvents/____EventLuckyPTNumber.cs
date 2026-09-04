using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JTGuard.Database.ModelsEvents
{
    public class ____EventLuckyPTNumber
    {
        public int ID { get; set; }
        public int Round { get; set; }
        public int TargetNumberMin { get; set; }
        public int TargetNumberMax { get; set; }
        public int RewardID { get; set; }
        public string RewardName { get; set; }
        public int RewardQuantity { get; set; }
    }
}
