<<<<<<< ours
﻿using System.Data.Entity;
=======
using System.Data.Entity;
>>>>>>> theirs
using API.Database.DuckSoup;

namespace Plugin.AutoNotice.Database;

public class DuckSoupExt : DuckSoup
{
    public virtual DbSet<AutoNoticeTable> AutoNotice { get; set; }
}