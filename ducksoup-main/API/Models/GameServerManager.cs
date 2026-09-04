using SilkroadSecurityAPI;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace API.Models
{
    public class GameServerManager
    {
        private const int UniqueDmgDisplayQueuePollIntervalMs = 1000;
        private const int UniqueDmgDisplayTimeMs = 10000;
        private static ISharedObjects? SharedObjects;
        public static int DMGMeterForm
        { get; set; } = 11446;

        private static readonly ConcurrentQueue<UniqueDamageInfo> UniqueDmgDisplayQueue = new ConcurrentQueue<UniqueDamageInfo>();
        private static Task? UniqueDmgDisplayTask = null;
        private static CancellationTokenSource CancellationTokenSource = new CancellationTokenSource();

        static GameServerManager()
        {
            UniqueDmgDisplayQueue = new ConcurrentQueue<UniqueDamageInfo>();
            UniqueDmgDisplayTask = null;
            CancellationTokenSource = new CancellationTokenSource();
            SharedObjects = ServiceFactory.ServiceFactory.Load<ISharedObjects>(typeof(ISharedObjects));
        }

        public static ManualResetEventSlim Mre = new ManualResetEventSlim(initialState: false);
        private static void UniqueDmgDisplayTaskWorker()
        {
            Mre.Wait(); //block the current thread till receiving data

            while (!CancellationTokenSource.IsCancellationRequested)
            {
                while (UniqueDmgDisplayQueue.TryDequeue(out UniqueDamageInfo? record))
                    BroadcastUniqueDmgInfo(record);

                Mre.Reset(); //should switch to unsignaled
            }
        }
        public static void Start() => UniqueDmgDisplayTask = Task.Factory.StartNew(() => UniqueDmgDisplayTaskWorker(),
                CancellationTokenSource.Token,
                TaskCreationOptions.LongRunning, TaskScheduler.Default);

        public static void Shutdown()
        {
            CancellationTokenSource.Cancel();
        }
        public static void HandleUniqueDamageInfo(UniqueDamageInfo record)
        {
            UniqueDmgDisplayQueue.Enqueue(record);
            Mre.Set();
        }

        private static async void BroadcastUniqueDmgInfo(UniqueDamageInfo record)
        {
            if (SharedObjects != null)
            {
                var uniqueInfoMsg = new Packet(0x183A);

                uniqueInfoMsg.WriteInt32(record.RefObjID);

                //TODO: Sort, remove items if over max.
                Dictionary<uint, int> records = record.GetSorted(6);

                uniqueInfoMsg.WriteInt32(records.Count);

                foreach (var kvp in records)
                {
                    var session = SharedObjects.AgentSessions.Where(sharedObjectsAgentSession =>
                    sharedObjectsAgentSession.CharacterGameReady && sharedObjectsAgentSession.SessionData.UniqueCharId == kvp.Key).FirstOrDefault();

                    uniqueInfoMsg.WriteAscii(session == null ? "<No Name>" : session.SessionData.Charname);
                    uniqueInfoMsg.WriteAscii(FormatNumber(kvp.Value));
                }
            

                foreach (var sharedObjectsAgentSession in SharedObjects.AgentSessions.Where(sharedObjectsAgentSession =>
                        sharedObjectsAgentSession.CharacterGameReady))
                {
                    await sharedObjectsAgentSession.SendToClient(uniqueInfoMsg);
                }
            }
        }

        public static string FormatNumber(long num)
        {
            try
            {
                if (num >= 100000000)
                {
                    return (num / 1000000D).ToString("0.#M");
                }
                if (num >= 1000000)
                {
                    return (num / 1000000D).ToString("0.##M");
                }
                if (num >= 100000)
                {
                    return (num / 1000D).ToString("0.#k");
                }
                if (num >= 10000)
                {
                    return (num / 1000D).ToString("0.##k");
                }

                return num.ToString("#,0");

            }
            catch
            {
                return "0";
            }

        }

    }

}
