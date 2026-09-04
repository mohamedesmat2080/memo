<<<<<<< ours
﻿using System.ComponentModel.DataAnnotations;
=======
using System.ComponentModel.DataAnnotations;
>>>>>>> theirs
using System.ComponentModel.DataAnnotations.Schema;

namespace API.Database.DuckSoup;

[Table("DailyReward")]
public class DailyReward
{
    [Key]
    [Column(Order = 0)]
    [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
    public int ID { get; set; }

    [Required] public int ItemID { get; set; }

    [Required] public int Count { get; set; }

    [ForeignKey("Day")]public virtual ICollection<DailyRewardRecords> DailyRewardRecords { get; set; }

}