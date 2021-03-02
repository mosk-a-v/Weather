using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using System;

namespace BoilerWeb {
 
    public struct LogInfoModel {
        public string Data { get; set; }
        public Priorities Priority { get; set; }
        public string Text { get; set; }
        [JsonConverter(typeof(UnixDateTimeConverter))]
        public DateTime Time { get; set; }
    }

    public enum Priorities {
        LOG_EMERG = 0,
        LOG_ALERT = 1,
        LOG_CRIT = 2,
        LOG_ERR = 3,
        LOG_WARNING = 4,
        LOG_NOTICE = 5,
        LOG_INFO = 6,
        LOG_DEBUG = 7
    }
}
