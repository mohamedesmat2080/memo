using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JTGuard.Database.ModelsEvents
{
    public class ____EventSchedule
    {
        public int ID { get; set; }
        public string EventName { get; set; }
        public int RefEventID { get; set; }
        public byte Day { get; set; }
        public string StartTime { get; set; }
        public int DurationMin { get; set; }
        public int RegisterMin { get; set; }
    }
}
