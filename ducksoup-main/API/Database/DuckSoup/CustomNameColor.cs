<<<<<<< ours
﻿using System.ComponentModel.DataAnnotations;
=======
using System.ComponentModel.DataAnnotations;
>>>>>>> theirs
using System.ComponentModel.DataAnnotations.Schema;

namespace API.Database.DuckSoup;

[Table("CustomNameColor")]
public class CustomNameColor
{
    [Key]
    [Column(Order = 0)]
    [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
    public int ID { get; set; }
    [Required] public string Charname { get; set; }
    [Required] public string Color { get; set; }
}