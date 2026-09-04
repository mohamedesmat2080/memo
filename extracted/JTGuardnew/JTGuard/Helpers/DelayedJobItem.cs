using System;

namespace JTGuard.Helpers
{    public class DelayedJobItem
    {
        public DateTime RegisterTime { get; set; }
        public int ExecAfterMs { get; set; }
        public object Session { get; set; }
        public object Param { get; set; }
        public Action<object, object> Handler { get; set; }

        public DelayedJobItem(int execAfterMs, object session, object param, Action<object, object> handler)
        {
            ExecAfterMs = execAfterMs;
            Session = session;
            Param = param;
            Handler = handler;
            RegisterTime = DateTime.UtcNow;
        }
    }


}
