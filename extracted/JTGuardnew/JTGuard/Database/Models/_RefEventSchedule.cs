using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JTGuard.Database.Models
{
    public class _RefEventSchedule
    {
        public int ID { get; set; }
        public string EventName { get; set; }
        public byte Day { get; set; }
        public string Time { get; set; }
    }
}
