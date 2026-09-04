using API.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace API.Model
{
    public class ItemInfo : IDisposable
    {
        public void Dispose()
        {
            Plus = null;
            Variance = null;
            Durability = null;
            MagParamNum = null;
            MagParamNum = null;
            MagicOptions = null;
            RefItemID = null;
            TypeID2 = null;
            TypeID3 = null;
            TypeID4 = null;
            itemDegree = null;
            Sockets = null;
            Advances = null;
        }
        public byte? Plus { get; set; } 
        public long? Variance { get; set; } 
        public int? Durability { get; set; }
        public byte? MagParamNum { get; set; }
        public List<long?>? MagicOptions { get; set; }
        public int? RefItemID { get; set; }
        public int? TypeID2 { get; set; }
        public int? TypeID3 { get; set; }
        public int? TypeID4 { get; set; }
        public int? itemDegree { get; set; }
        public List<BindingItemInfo>? Sockets { get; set; }
        public List<BindingItemInfo>? Advances { get; set; }
        

        public ItemInfo(byte? Plus, long? Variance, int Durability, byte MagParamNum, long? MagParam1, long? MagParam2, long? MagParam3, long? MagParam4, long? MagParam5, long? MagParam6, long? MagParam7, long? MagParam8, long? MagParam9, long? MagParam10, long? MagParam11, long? MagParam12, int RefItemID, byte TypeID2, byte TypeID3, byte TypeID4,List<BindingItemInfo> bInfo,int ItemDegree = 0)
        {
            MagicOptions = new List<long?>();
            Sockets = new List<BindingItemInfo>();
            Advances = new List<BindingItemInfo>();
            this.Plus = Plus;
            this.Variance = Variance;
            this.Durability = Durability;
            this.MagParamNum = MagParamNum;
            if(MagParam1 > 0 && MagParam1 != null)
                this.MagicOptions.Add(MagParam1);
            if (MagParam2 > 0 && MagParam2 != null)
                this.MagicOptions.Add(MagParam2);
            if (MagParam3 > 0 && MagParam3 != null)
                this.MagicOptions.Add(MagParam3);
            if (MagParam4 > 0 && MagParam4 != null)
                this.MagicOptions.Add(MagParam4);
            if (MagParam5 > 0 && MagParam5 != null)
                this.MagicOptions.Add(MagParam5);
            if (MagParam6 > 0 && MagParam6 != null)
                this.MagicOptions.Add(MagParam6);
            if (MagParam7 > 0 && MagParam7 != null)
                this.MagicOptions.Add(MagParam7);
            if (MagParam8 > 0 && MagParam8 != null)
                this.MagicOptions.Add(MagParam8);
            if (MagParam9 > 0 && MagParam9 != null)
                this.MagicOptions.Add(MagParam9);
            if (MagParam10 > 0 && MagParam10 != null)
                this.MagicOptions.Add(MagParam10);
             if (MagParam11 > 0 && MagParam11 != null)
                this.MagicOptions.Add(MagParam11);
             if (MagParam12 > 0 && MagParam12 != null)
                this.MagicOptions.Add(MagParam12);
            this.RefItemID = RefItemID;
            this.TypeID2 = TypeID2;
            this.TypeID3 = TypeID3;
            this.TypeID4 = TypeID4;
            this.itemDegree = ItemDegree;
            foreach (var bItem in bInfo)
            {
                if (bItem.Type == 1)
                    Sockets.Add(bItem);
                else if (bItem.Type == 2)
                    Advances.Add(bItem);
            }
        }

    }
}
