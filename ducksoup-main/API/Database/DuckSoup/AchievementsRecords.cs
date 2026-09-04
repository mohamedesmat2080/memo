using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace API.Database.DuckSoup;

[Table("AchievementsRecords")]
public class AchievementsRecords
{
    [Key]
    [Column(Order = 0)]
    [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
    public int Achievement_ID { get; set; }
    [Required] public string Charname { get; set; }
    [Required] public int Points { get; set; }
    public Achievements Achievements { get; set; }
}