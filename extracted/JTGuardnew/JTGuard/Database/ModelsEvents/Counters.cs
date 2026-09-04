using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JTGuard.Database.ModelsEvents
{
    public class TimerWorldandPlayer
    {
        public int WorldID { get; set; }
        public int LayerID { get; set; }
    }
    public class SCreatedKillCounterKillList
    {
        public int WorldID { get; set; }
        public string CharName16 { get; set; }
        public int Kill { get; set; }
    };
    public class SCreatedTeamKillCounterKillList
    {
        public int WorldID { get; set; }
        public string CharName16 { get; set; }
        public int Kill { get; set; }
        public int Team { get; set; }
    };
    public class SCreatedJobKillCounterKillList
    {
        public int WorldID { get; set; }
        public string CharName16 { get; set; }
        public int Kill { get; set; }
        public int Team { get; set; }
    };
    public class SFortressWarCounter
    {
        public string CharName { get; set; }
        public string GuildName { get; set; }
        public string UnionName { get; set; }
        public int WorldID { get; set; }
        public int Kill { get; set; }
    };
}
