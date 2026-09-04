using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace API.Database.DuckSoup;

[Table("DailyRewardRecords")]
public class DailyRewardRecords
{
    [Key]
    [Column(Order = 0)]
    [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
    public int ID { get; set; }
    [Required] public string Charname { get; set; }
    [Required] public bool isTaken { get; set; }
    [Required] public DateTime TakenDate { get; set; }
    [Required] public int Day { get; set; } 
}