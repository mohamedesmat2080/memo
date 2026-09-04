using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JTGuard.Database.Models
{
    public class SecondaryPasswordData
    {
        public int ID { get; set; }
        public string StrUserID { get; set; }
        public int Password { get; set; }
        public string Hwid { get; set; }
        public bool RememberPC { get; set; }
    }
}
