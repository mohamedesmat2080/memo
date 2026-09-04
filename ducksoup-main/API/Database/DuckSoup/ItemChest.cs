using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace API.Database.DuckSoup;

[Table("ItemChest")]
public class ItemChest
{
    [Key]
    [Column(Order = 0)]
    [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
    public int ID { get; set; }
    [Required] public string Charname { get; set; }
    [Required] public int ItemID { get; set; }
    [Required] public string From { get; set; }
    [Required] public int Count { get; set; }
    [Required] public DateTime Date { get; set; }
}