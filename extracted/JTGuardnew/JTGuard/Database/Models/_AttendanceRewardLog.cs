using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JTGuard.Database.Models
{
    public class _AttendanceRewardLog
    {
        public int ID { get; set; }
        public int RefRewardID {get; set; }
        public int CharID {get; set; }
        public int CanTake {get; set; }
        public int AlreadyTaken {get; set; }
    }
}
