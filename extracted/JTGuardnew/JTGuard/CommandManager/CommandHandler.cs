using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using JTGuard.ServerManagers;

namespace JTGuard.CommandManager
{
    public class CommandHandler
    {
        // Bu sınıfta komutları işlemek için yöntemler bulunur
        public async void ExecuteCommand(string command)
        {
            try
            {
                if (command.Equals("start", StringComparison.OrdinalIgnoreCase))
                {
                    Console.WriteLine("Lütfen UserID'nizi girin:");
                    string userId = Console.ReadLine();
                    if (userId != null)
                    {
                        Console.WriteLine(userId);
                    }
                    else
                    {
                        Console.WriteLine("Geçersiz UserID!");
                    }
                    // Kullanıcıdan alınan UserID ile yapılacak işlemler buraya eklenebilir
                }
                else if (command.Equals("/reload", StringComparison.OrdinalIgnoreCase))
                {
                    Program.PrintInColor("Please Write Reload Type / = (1) Reload _ServerSettings" +
                        "\n (2) Reload __Notices \n" +
                        "(3) Reload EventSchedule", ConsoleColor.Magenta);
                }
                else if (command.Equals("/reload 1", StringComparison.OrdinalIgnoreCase))
                {
                    Program.PrintInColor("Reloading _ServerSettings...", ConsoleColor.Magenta);
                    await Management.ReloadServerSettings();
                    Program.PrintInColor("Reload _ServerSettings completed.", ConsoleColor.Green);
                }
                else if (command.Equals("/reload 2", StringComparison.OrdinalIgnoreCase))
                {
                    Program.PrintInColor("Reloading __Notices...", ConsoleColor.Magenta);
                    await Management.LoadNoticesIntoCache();
                    Program.PrintInColor("Reload __Notices completed.", ConsoleColor.Green);
                }
                else if (command.Equals("/reload 3", StringComparison.OrdinalIgnoreCase))
                {
                    Program.PrintInColor("Reloading EventSchedule...", ConsoleColor.Magenta);
                    await Management.ReloadEventSchedule();
                    Program.PrintInColor("Reload EventSchedule completed.", ConsoleColor.Green);
                }
                else
                {
                    Console.WriteLine("Bilinmeyen komut: {0}", command);
                }
            }
            catch (Exception EX)
            {
                Console.WriteLine(EX.Message);
            }
        }
    }

}
