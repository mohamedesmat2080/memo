using API.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace API.Model
{
    public class StallNetwork : IDisposable
    {
        public void Dispose()
        {
            sellerName = null;
            continentName = null;
            stallSlot = null;
            invSlot = null;
            tid = null;
            price = null;
            quantity = null;
            uniqueID = null;
            itemInfo = null;
            isActive = null;
        }
        public string? sellerName { get; set; }
        public string? continentName { get; set; }
        public byte? stallSlot { get; set; }
        public byte? invSlot { get; set; }
        public uint? tid { get; set; }
        public ulong? price { get; set; }
        public ushort? quantity { get; set; }
        public uint? uniqueID { get; set; }
        public ItemInfo? itemInfo { get; set; }
        public bool? isActive { get; set; }
        public StallNetwork(string sellerName,uint tid, byte stallSlot, byte invSlot, ulong price,ItemInfo itemInfo,ushort quantity,uint uniqueID)
        {
            this.sellerName = sellerName;
            this.tid = tid;
            this.stallSlot = stallSlot;
            this.invSlot = invSlot;
            this.price = price;
            this.itemInfo = itemInfo;
            this.quantity = quantity;
            this.uniqueID = uniqueID;
            this.continentName = "";
        }
    }
}
