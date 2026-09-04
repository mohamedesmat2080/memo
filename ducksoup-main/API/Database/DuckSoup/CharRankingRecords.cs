using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace API.Database.DuckSoup;

[Table("CharRankingRecords")]
public class CharRankingRecords
{
    [Key]
    [Column(Order = 0)]
    [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
    [Required] public string Charname { get; set; }
    [Required] public bool Race { get; set; }
    [Required] public int Level { get; set; }
    [Required] public int Points { get; set; }
    [Required] public string Guild { get; set; }
    public int CharRanking_ID { get; set; }

}
