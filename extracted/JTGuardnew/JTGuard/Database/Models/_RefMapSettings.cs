using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JTGuard.Database.Models
{
    public class _RefMapSettings
    {
        public int RegionID { get; set ; }
        public bool EventSuit { get; set ; }
        public bool HideBuffViewer { get; set ; }
        public bool DisablePetSpawn { get; set ; }
        public bool DisableParty { get; set ; }
        public bool AutoCape { get; set ; }
        public bool DisableChat { get; set ; }
        public bool DisableTrace { get; set ; }
        public bool DisableZerk { get; set ; }
        public bool DisableTradeGoodsDrop { get; set ; }
        public bool HideMiniMap { get; set; }
    }
}
