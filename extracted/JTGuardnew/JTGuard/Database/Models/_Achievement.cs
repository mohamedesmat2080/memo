using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JTGuard.Database.Models
{
    public class _Achievement
    {
        public int ID { get; set; }
        public int CharID {get; set; }
        public int RefAchievementID {get; set; }
        public byte State {get; set; }
    }
}
