using System.ComponentModel;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace API.Database.DuckSoup;

[Table("TitleStorage")]
public class TitleStorage
{
    [Key]
    [Column(Order = 0)]
    [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
    public int ID { get; set; }
    [Required] public string CharName { get; set; }
    [Required] public string Title { get; set; }
}