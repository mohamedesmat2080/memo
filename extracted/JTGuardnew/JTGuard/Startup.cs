using JTGuard.CommandManager;
using JTGuard.Database.Model;
using JTGuard.ServerManagers;
using JTGuard.SettingManager;
public static class Program
{
    public static string Connectionstring { get; set; }
    public static string MainMachineIP { get; set; }
    private static void Main()
    {
        var settings = new SettingsManager();

        Connectionstring = $"data source={settings.Settings.Address};initial catalog={settings.Settings.ProxyDb};persist security info =True; User Id={settings.Settings.Username};Password={settings.Settings.Password};MultipleActiveResultSets=True;App=EntityFramework;Encrypt=False;Pooling=true;Max Pool Size={settings.Settings.MaximumPool};Min Pool Size={settings.Settings.MinimumPool};Connection Lifetime={settings.Settings.ConnectionLifetime};";
        MainMachineIP = settings.Settings.ServerIP;



        PrintInColor("JTGuard Started at :  " + DateTime.Now, ConsoleColor.Yellow);

        PrintInColor("JTGuard IP Address : " + MainMachineIP, ConsoleColor.Yellow);

        Console.Title = "JTGuard...";
        Management.CreateServerAsync();


        CommandHandler commandHandler = new CommandHandler();
        try
        {
            while (true)
            {
                Console.WriteLine("Komut girin: ");
                var command = Console.ReadLine();
                if (string.IsNullOrEmpty(command))
                    continue;
                if (command.Equals("/exit", StringComparison.OrdinalIgnoreCase))
                {
                    Console.WriteLine("Programdan çıkılıyor...");
                    break; 
                   
                }
                commandHandler.ExecuteCommand(command);
            }
        }
        catch (Exception exception)
        {
            Console.WriteLine("Program.cs Main| {0}", exception.Message);
            Console.WriteLine("Program.cs Main| {0}", exception.StackTrace);
        }



    }
    public static void PrintInColor(string message, ConsoleColor color)
    {
        var originalColor = Console.ForegroundColor;
        Console.ForegroundColor = color;
        Console.WriteLine(message);
        Console.ForegroundColor = originalColor;
    }
}