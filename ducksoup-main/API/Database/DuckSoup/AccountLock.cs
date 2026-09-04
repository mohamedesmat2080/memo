using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace API.Database.DuckSoup;

[Table("AccountLock")]
public class AccountLock
{
    [Key]
    [Column(Order = 0)]
    [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
    public int ID { get; set; }
    [Required] public string Username { get; set; }
    [Required] public string Passcode { get; set; }
    [ForeignKey("AccountLock_ID")]public virtual ICollection<AccountLockRecords> AccountLockRecords { get; set; }

}