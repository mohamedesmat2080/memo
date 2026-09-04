using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

#pragma warning disable CS8618

namespace Plugin.DatabaseCommands.Database;

[Table("DatabaseCommand")]
public class DatabaseCommandTable
{
    [Key]
    [Column(Order = 0)]
    [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
    public int ID { get; set; }
    [Required] public int Command { get; set; }
    public string? Data1 { get; set; }
    public string? Data2{ get; set; }
    public string? Data3 { get; set; }
    public string? Data4 { get; set; }
    public string? Data5 { get; set; }
    public DateTime? SendAfter { get; set; }
}