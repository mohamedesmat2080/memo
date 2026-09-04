using API;
using API.ServiceFactory;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
using System.Data.Entity;
namespace DuckSoup.Library.Timers
{
    public class TimerManager : IDisposable
    {
        private Timer? _rankTimer;
        private Timer? _updateTitleTimer;
        private ISharedObjects? SharedObjects;

        public TimerManager()
        {
            SharedObjects = ServiceFactory.Load<ISharedObjects>(typeof(ISharedObjects));
            InitializeTimers();
        }

        private void InitializeTimers()
        {
            _rankTimer = new Timer(5 * 60 * 1000);
            _rankTimer.Elapsed += OnUpdateRank;
            _rankTimer.AutoReset = true;
            _rankTimer.Enabled = true;

            _updateTitleTimer = new Timer(3 * 1000);
            _updateTitleTimer.Elapsed += OnUpdateTitle;
            _updateTitleTimer.AutoReset = true;
            _updateTitleTimer.Enabled = true;
        }
        private void OnUpdateRank(object? sender, ElapsedEventArgs e)
        {
            Task.Run(() =>
            {
                SharedObjects.CharRanking.Clear();
                using var db = new API.Database.DuckSoup.DuckSoup();
                SharedObjects.CharRanking = db.CharRanking.Include(x => x.CharRankingRecords).ToDictionary(t => t.ID);
            });
            
        }
        private void OnUpdateTitle(object? sender, ElapsedEventArgs e)
        {
            _updateTitleTimer.Stop();
            Console.Title = $"[DownloadServer]: {SharedObjects.DownloadSessions.Count}   [GatewayServer]: {SharedObjects.GatewaySessions.Count}   [AgentServer]: {SharedObjects.AgentSessions.Count}";
            _updateTitleTimer.Start();
        }
        public void Dispose()
        {
            _rankTimer?.Stop();
            _rankTimer?.Dispose();
            _rankTimer = null;
        }
    }
}
