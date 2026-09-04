using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JTGuard.Database.Models
{
    public class _AchievementCondition
    {
        public int ID { get; set; }
        public int CharID { get; set;}
        public int AchievementID { get; set;}
        public int RefAchievementConditionID { get; set;}
        public Int64 ProgressCount { get; set;}
    }
}
