using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace API.Database.DuckSoup;

[Table("Achievements")]
public class Achievements
{
    [Key]
    [Column(Order = 0)]
    [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
    public int ID { get; set; }
    [Required] public string Rank { get; set; }
    [Required] public int MaxPoints { get; set; }
    [Required] public string Descr { get; set; }
    public virtual ICollection<AchievementsRecords> AchievementsRecords { get; set; }

}