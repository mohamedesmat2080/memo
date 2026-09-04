using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JTGuard.Database.ModelsEvents
{
    public class ____EventSurvivalArena
    {
       public int ID { get; set; }
       public int RegionID { get; set; }
       public int PCLimit { get; set; }
       public int MinPlayer { get; set; }
       public int WorldID { get; set; }
       public int StartPosX { get; set; }
       public int StartPosY { get; set; }
       public int StartPosZ { get; set; }
       public int WinnerRewardID { get; set; }
       public string WinnerRewardName { get; set; }
       public int WinnerRewardQuantity { get; set; }
       public int SecondWinnerRewardID { get; set; }
       public string SecondWinnerRewardName { get; set; }
       public int SecondWinnerRewardQuantity { get; set; }
       public int KillRewardID { get; set; }
       public string KillRewardName { get; set; }
       public int KillRewardQuantity { get; set; }
    }
}
