using System.Data.Entity;
using API.Database.DuckSoup;

namespace Plugin.DatabaseCommands.Database;

public class DuckSoupExt : API.Database.DuckSoup.DuckSoup
{
    public virtual DbSet<DatabaseCommandTable> DatabaseCommand { get; set; }
}