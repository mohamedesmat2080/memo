<<<<<<< ours
﻿using System.Timers;
=======
using System.Timers;
>>>>>>> theirs
using API;
using API.Command;
using API.Database.DuckSoup;
using API.Model;
using API.Plugin;
using API.Server;
using API.ServiceFactory;
using API.ServiceFactory.Exceptions;
using Plugin.DatabaseCommands.Database;
using SilkroadSecurityAPI;
using Timer = System.Timers.Timer;
using DuckSoup;
namespace Plugin.DatabaseCommands;

public class DatabaseCommands : IPlugin
{
    private Timer? _commandTimer;
    private ICommandManager? _commandManager;
    private ISharedObjects? SharedObjects;

    public void Dispose()
    {
        _commandTimer?.Stop();
        _commandTimer?.Dispose();
        _commandManager = null;
    }

    public string Name => "DatabaseCommands";
    public string Version => "1.0.0";
    public string Author => "b0ykoe";
    public ServerType ServerType => ServerType.None;

    public void OnEnable()
    {
        SharedObjects = ServiceFactory.Load<ISharedObjects>(typeof(ISharedObjects));
        _commandManager = ServiceFactory.Load<ICommandManager>(typeof(ICommandManager));
        using (var context = new API.Database.DuckSoup.DuckSoup())
        {
            context.Database.ExecuteSqlCommand(@"
                if not exists (select * from sysobjects where name='DatabaseCommand' and xtype='U')
                BEGIN
                    create table DatabaseCommand
                    (
                        ID int identity
                        constraint DatabaseCommand_pk
                        primary key,
                            Command   nvarchar(max) not null,
                            Data1 nvarchar(max),
                            Data2 nvarchar(max),
                            Data3 nvarchar(max),
                            Data4 nvarchar(max),
                            Data5 nvarchar(max),
                        SendAfter DATETIME
                    )
                    create unique index DatabaseCommand_ID_uindex
                    on DatabaseCommand (ID)
                END");
        }
        InitializeTimer();
    }

    public void OnServerStart(IAsyncServer server)
    {
        // wont get fired. Duo to ServerType.None
    }

    public List<Command> RegisterCommands()
    {
        return new List<Command>();
    }

    private void InitializeTimer()
    {
        _commandTimer = new Timer(1000);
        _commandTimer.Elapsed += OnTimerTick;
        _commandTimer.AutoReset = true;
        _commandTimer.Enabled = true;
    }
    private async Task SendToAllClients(Packet packet)
    {
        if (SharedObjects?.AgentSessions == null)
        {
            return;
        }
        foreach (var sharedObjectsAgentSession in SharedObjects.AgentSessions.Where(sharedObjectsAgentSession =>
             sharedObjectsAgentSession.CharacterGameReady))
        {
            await sharedObjectsAgentSession.SendToClient(packet);
        }
    }
    private async void OnTimerTick(object? sender, ElapsedEventArgs e)
    {
        using var context = new DuckSoupExt();
        DatabaseCommandTable tempEntry = null;

        try
        {
            var databaseCommandTables = context.DatabaseCommand.ToList();
            foreach (var commandEntry in databaseCommandTables)
            {
                if (commandEntry.SendAfter == null || DateTime.Compare((DateTime)commandEntry.SendAfter, DateTime.Now) < 0)
                {
                    tempEntry = commandEntry;
                    switch (commandEntry.Command)
                    {
                        case 1:
                        case 2:
                            {
                                //1 unique spawned  //2 unique killed
                                if (SharedObjects.UniqueLog.ContainsKey(Convert.ToInt32(commandEntry.Data1)))
                                {
                                    var uLog = SharedObjects.UniqueLog[Convert.ToInt32(commandEntry.Data1)];
                                    uLog.State = commandEntry.Command == 1 ? true : false;
                                    uLog.Killer = commandEntry.Command == 1 ? "<No killer>" : commandEntry.Data2;
                                    uLog.Date = DateTime.Now;
                                }
                                else
                                {
                                    SharedObjects.UniqueLog.Add(Convert.ToInt32(commandEntry.Data1), new UniqueLog()
                                    {
                                        refObjID = Convert.ToInt32(commandEntry.Data1),
                                        State = commandEntry.Command == 1 ? true : false,
                                        Killer = commandEntry.Command == 1 ? "<No killer>" : commandEntry.Data2,
                                        Date = DateTime.Now
                                    });
                                }
                            }
                            break;
                        case 3:
                            {
                                if(commandEntry.Data3 == "0")
                                {
                                    //3 add or update custom icon
                                    if (SharedObjects.CustomIcon.ContainsKey(commandEntry.Data1))
                                        SharedObjects.CustomIcon[commandEntry.Data1].IconID = Convert.ToInt32(commandEntry.Data2);
                                    else
                                    {
                                        var newCustom = new CustomIcon()
                                        {
                                            Charname = commandEntry.Data1,
                                            IconID = Convert.ToInt32(commandEntry.Data2),
                                        };
                                        SharedObjects.CustomIcon.Add(commandEntry.Data1, newCustom);
                                    }
                                    Packet CustomIcon = new Packet(0x181A);
                                    CustomIcon.WriteInt32(1);
                                    CustomIcon.WriteInt32(Convert.ToInt32(commandEntry.Data2));
                                    CustomIcon.WriteAscii(commandEntry.Data1);
                                    await SendToAllClients(CustomIcon);
                                }
                                else
                                {
                                    //3 remove custom icon
                                    if (SharedObjects.CustomIcon.ContainsKey(commandEntry.Data1))
                                        SharedObjects.CustomIcon.Remove(commandEntry.Data1);

                                    Packet CustomIcon = new Packet(0x182D);
                                    CustomIcon.WriteUInt8(2);
                                    CustomIcon.WriteAscii(commandEntry.Data1);
                                    await SendToAllClients(CustomIcon);
                                }
                            }
                            break;
                        case 4:
                            {
                                if (commandEntry.Data3 == "0")
                                {
                                    //4 add or update vip icon
                                    if (SharedObjects.CustomIconVip.ContainsKey(commandEntry.Data1))
                                        SharedObjects.CustomIconVip[commandEntry.Data1].IconID = Convert.ToInt32(commandEntry.Data2);
                                    else
                                    {
                                        var newCustom = new CustomIconVip()
                                        {
                                            Charname = commandEntry.Data1,
                                            IconID = Convert.ToInt32(commandEntry.Data2),
                                        };
                                        SharedObjects.CustomIconVip.Add(commandEntry.Data1, newCustom);
                                    }
                                    Packet CustomIcon = new Packet(0x181C);
                                    CustomIcon.WriteInt32(1);
                                    CustomIcon.WriteInt32(Convert.ToInt32(commandEntry.Data2));
                                    CustomIcon.WriteAscii(commandEntry.Data1);
                                    await SendToAllClients(CustomIcon);
                                }
                                else
                                {
                                    //4 remove vip icon
                                    if (SharedObjects.CustomIconVip.ContainsKey(commandEntry.Data1))
                                        SharedObjects.CustomIconVip.Remove(commandEntry.Data1);

                                    Packet CustomIcon = new Packet(0x182D);
                                    CustomIcon.WriteUInt8(3);
                                    CustomIcon.WriteAscii(commandEntry.Data1);
                                    await SendToAllClients(CustomIcon);
                                }
                            }
                            break;
                        case 5:
                            {
                                if (commandEntry.Data3 == "0")
                                {
                                    //5 add or update custom name
                                    if (SharedObjects.CustomName.ContainsKey(commandEntry.Data1))
                                        SharedObjects.CustomName[commandEntry.Data1].Rank = commandEntry.Data2;
                                    else
                                    {
                                        var newCustom = new CustomName()
                                        {
                                            Charname = commandEntry.Data1,
                                            Rank = commandEntry.Data2,
                                        };
                                        SharedObjects.CustomName.Add(commandEntry.Data1, newCustom);
                                    }
                                    Packet customPck = new Packet(0x181D);
                                    customPck.WriteInt32(1);
                                    customPck.WriteAscii(commandEntry.Data1);
                                    customPck.WriteAscii(commandEntry.Data2);
                                    await SendToAllClients(customPck);
                                }
                                else
                                {
                                    //5 remove custom name
                                    if (SharedObjects.CustomName.ContainsKey(commandEntry.Data1))
                                        SharedObjects.CustomName.Remove(commandEntry.Data1);

                                    Packet CustomIcon = new Packet(0x182D);
                                    CustomIcon.WriteUInt8(1);
                                    CustomIcon.WriteAscii(commandEntry.Data1);
                                    await SendToAllClients(CustomIcon);
                                }
                            }
                            break;
                        case 6:
                            {
                                if (commandEntry.Data3 == "0")
                                {
                                    //6 add or update custom name color
                                    if (SharedObjects.CustomNameColor.ContainsKey(commandEntry.Data1))
                                        SharedObjects.CustomNameColor[commandEntry.Data1].Color = commandEntry.Data2;
                                    else
                                    {
                                        var newCustom = new CustomNameColor()
                                        {
                                            Charname = commandEntry.Data1,
                                            Color = commandEntry.Data2,
                                        };
                                        SharedObjects.CustomNameColor.Add(commandEntry.Data1, newCustom);
                                    }
                                    Packet customPck = new Packet(0x181E);
                                    customPck.WriteInt32(1);
                                    customPck.WriteInt32((int)new System.ComponentModel.Int32Converter().ConvertFromString(commandEntry.Data2));
                                    customPck.WriteAscii(commandEntry.Data1);
                                    await SendToAllClients(customPck);
                                }
                                else
                                {
                                    //6 remove custom name color
                                    if (SharedObjects.CustomNameColor.ContainsKey(commandEntry.Data1))
                                        SharedObjects.CustomNameColor.Remove(commandEntry.Data1);

                                    Packet CustomIcon = new Packet(0x182D);
                                    CustomIcon.WriteUInt8(5);
                                    CustomIcon.WriteAscii(commandEntry.Data1);
                                    await SendToAllClients(CustomIcon);
                                }
                            }
                            break;
                        case 7:
                            {
                                if (commandEntry.Data3 == "0")
                                {
                                    //7 add or update custom title
                                    if (SharedObjects.CustomTitle.ContainsKey(commandEntry.Data1))
                                        SharedObjects.CustomTitle[commandEntry.Data1].Title = commandEntry.Data2;
                                    else
                                    {
                                        var newCustom = new CustomTitle()
                                        {
                                            CharName = commandEntry.Data1,
                                            Title = commandEntry.Data2,
                                        };
                                        SharedObjects.CustomTitle.Add(commandEntry.Data1, newCustom);
                                    }
                                    Packet customPck = new Packet(0x181F);
                                    customPck.WriteInt32(1);
                                    customPck.WriteAscii(commandEntry.Data1);
                                    customPck.WriteAscii(commandEntry.Data2);
                                    await SendToAllClients(customPck);

                                }
                                else
                                {
                                    //7 remove custom name color
                                    if (SharedObjects.CustomTitle.ContainsKey(commandEntry.Data1))
                                        SharedObjects.CustomTitle.Remove(commandEntry.Data1);

                                    Packet CustomIcon = new Packet(0x182D);
                                    CustomIcon.WriteUInt8(0);
                                    CustomIcon.WriteAscii(commandEntry.Data1);
                                    await SendToAllClients(CustomIcon);
                                }
                            }
                                break;
                        case 8:
                            {
                                if (commandEntry.Data3 == "0")
                                {
                                    //8 add or update custom tittle color
                                    if (SharedObjects.CustomTitleColor.ContainsKey(commandEntry.Data1))
                                        SharedObjects.CustomTitleColor[commandEntry.Data1].Color = commandEntry.Data2;
                                    else
                                    {
                                        var newCustom = new CustomTitleColor()
                                        {
                                            Charname = commandEntry.Data1,
                                            Color = commandEntry.Data2,
                                        };
                                        SharedObjects.CustomTitleColor.Add(commandEntry.Data1, newCustom);
                                    }
                                    Packet customPck = new Packet(0x182A);
                                    customPck.WriteInt32(1);
                                    customPck.WriteInt32((int)new System.ComponentModel.Int32Converter().ConvertFromString(commandEntry.Data2));
                                    customPck.WriteAscii(commandEntry.Data1);
                                    await SendToAllClients(customPck);
                                }
                                else
                                {
                                    //8 remove custom tittle color
                                    if (SharedObjects.CustomTitleColor.ContainsKey(commandEntry.Data1))
                                        SharedObjects.CustomTitleColor.Remove(commandEntry.Data1);

                                    Packet CustomIcon = new Packet(0x182D);
                                    CustomIcon.WriteUInt8(6);
                                    CustomIcon.WriteAscii(commandEntry.Data1);
                                    await SendToAllClients(CustomIcon);
                                }
                            }
                            break;
                        case 9:
                            {
                                //9 add an item to itemchest through the trigger
                                string charname = commandEntry.Data1;

                                var session = SharedObjects.AgentSessions.FirstOrDefault(session => session.SessionData.Charname == charname &&
                                    session.CharacterGameReady);

                                if(session != null)
                                {
                                    session.SessionData.chestItems.Add(new ItemChest
                                    {
                                        Charname = charname,
                                        ID = Convert.ToInt32(commandEntry.Data5),
                                        ItemID = Convert.ToInt32(commandEntry.Data2),
                                        From = commandEntry.Data3,
                                        Count = Convert.ToInt32(commandEntry.Data4),
                                        Date = DateTime.Now
                                    });

                                    Packet notice = new Packet(0x183B);
                                    notice.WriteByte(2);
                                    notice.WriteAscii("New item has been added to your chest, check it out.");
                                    await session.SendToClient(notice);
                                }
                               
                            }
                            break;
                        case 10:
                            {
                                //10 add and update title storage of a session
                                string charname = commandEntry.Data1;

                                var session = SharedObjects.AgentSessions.FirstOrDefault(session => session.SessionData.Charname == charname &&
                                        session.CharacterGameReady);
                                if (session != null)
                                {
                                    if (commandEntry.Data3 == "0")
                                    {
                                        if (!session.SessionData.ownTitles.Contains(commandEntry.Data2))
                                            session.SessionData.ownTitles.Add(commandEntry.Data2);
                                    }
                                    else
                                    {
                                        if (session.SessionData.ownTitles.Contains(commandEntry.Data2))
                                            session.SessionData.ownTitles.Remove(commandEntry.Data2);
                                    }
                                }

                            }
                            break;
                        case 11:
                            {
                                if(DuckSoup.Library.SharedObjects.settings.ContainsKey(commandEntry.Data1))
                                    DuckSoup.Library.SharedObjects.settings[commandEntry.Data1] = Convert.ChangeType(commandEntry.Data2, DuckSoup.Library.SharedObjects.settings[commandEntry.Data1]?.GetType());
                            }
                            break;
                    }
                    context.DatabaseCommand.Remove(commandEntry);
                }
            }

            context.SaveChanges();
        }
        catch (Exception ex)
        {
            if(tempEntry != null)
            {
                context.DatabaseCommand.Remove(tempEntry);
                context.SaveChanges();
            }

            Global.Logger.Error(ex.ToString());
        }

    }
}