using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;


namespace JTGuard.Helpers
{
    public class DelayedJobManager
    {
        private readonly List<DelayedJobItem> _jobs = new List<DelayedJobItem>();
        private readonly object _lock = new object();
        private bool _isRunning = false;

        public void Run()
        {
            _isRunning = true;
            Task.Run(() => ThreadWorker());
        }

        public void Stop()
        {
            _isRunning = false;
        }

        public void CreateJob(DelayedJobItem jobItem)
        {
            lock (_lock)
            {
                jobItem.RegisterTime = DateTime.UtcNow;
                _jobs.Add(jobItem);
            }
        }

        private void ThreadWorker()
        {
            while (_isRunning)
            {
                lock (_lock)
                {
                    var currentTime = DateTime.UtcNow;
                    var executedJobs = _jobs.Where(job => (currentTime - job.RegisterTime).TotalMilliseconds > job.ExecAfterMs).ToList();

                    foreach (var job in executedJobs)
                    {
                        job.Handler?.Invoke(job.Session, job.Param);
                        _jobs.Remove(job);
                    }
                }
                Thread.Sleep(1);
            }
        }
    }


}
