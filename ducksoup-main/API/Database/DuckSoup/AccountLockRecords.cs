<<<<<<< ours
﻿using System.ComponentModel.DataAnnotations;
=======
using System.ComponentModel.DataAnnotations;
>>>>>>> theirs
using System.ComponentModel.DataAnnotations.Schema;

namespace API.Database.DuckSoup;

[Table("AccountLockRecords")]
public class AccountLockRecords
{
    [Key]
    [Column(Order = 0)]
    [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
    public int ID { get; set; }
    [Required] public string hwid { get; set; }
    public int AccountLock_ID { get; set; }
}