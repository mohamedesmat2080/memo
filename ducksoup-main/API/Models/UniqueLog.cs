using API.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace API.Model
{
	public class UniqueLog : IDisposable
	{
		public void Dispose()
		{
			refObjID = null;
			Killer = null;
			State = null;
			Date = null;
		}
		public int? refObjID { get; set; }
		public string? Killer { get; set; }
		public bool? State { get; set; }
		public DateTime? Date { get; set; }

	}
}
