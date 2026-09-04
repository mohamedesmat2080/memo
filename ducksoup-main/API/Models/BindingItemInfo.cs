using API.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace API.Model
{
	public class BindingItemInfo : IDisposable
	{
        public void Dispose()
        {
			Type = 0;
			ID = 0;
			Slot = 0;
			Value = 0;
			nParam = null;
			nOptLvl = 0;
		}
        public byte Type { get; set; }
		public int ID { get; set; }
		public byte Slot { get; set; }
		public int Value { get; set; }
		public int? nParam { get; set; }
		public byte nOptLvl { get; set; }
	}
}
