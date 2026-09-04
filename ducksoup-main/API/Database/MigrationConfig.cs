<<<<<<< ours
﻿using System.Data.Entity;
=======
using System.Data.Entity;
>>>>>>> theirs

namespace API.Database;

public class MigrationConfig<T> : System.Data.Entity.Migrations.DbMigrationsConfiguration<T> where T : DbContext
{
    public MigrationConfig()
    {
        AutomaticMigrationsEnabled = true;
        AutomaticMigrationDataLossAllowed = true;
    }
    
}