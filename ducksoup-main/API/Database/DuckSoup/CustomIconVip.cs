using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace API.Database.DuckSoup;

[Table("CustomIconVip")]
public class CustomIconVip
{
    [Key]
    [Column(Order = 0)]
    [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
    public int ID { get; set; }
    [Required] public string Charname { get; set; }
    [Required] public int IconID { get; set; }
}