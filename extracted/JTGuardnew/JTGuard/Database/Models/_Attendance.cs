using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JTGuard.Database.Models
{
    public class _Attendance
    {
        public int ID { get; set; }
        public int CharID { get; set; }
        public int DayCount { get; set; }
        public string LastAttendedDate { get; set; }
    }
}
