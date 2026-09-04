<<<<<<< ours
﻿using System.ComponentModel.DataAnnotations;
=======
using System.ComponentModel.DataAnnotations;
>>>>>>> theirs
using System.ComponentModel.DataAnnotations.Schema;

namespace API.Database.DuckSoup;

[Table("CharRanking")]
public class CharRanking
{
    [Key]
    [Column(Order = 0)]
    [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
    public int ID { get; set; }
    [Required] public string RankName { get; set; }
    [ForeignKey("CharRanking_ID")] public virtual ICollection<CharRankingRecords> CharRankingRecords { get; set; }

}