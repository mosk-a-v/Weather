using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace BoilerWeb {
    public struct BoilerInfoModel {
        public const int DefaultValue = -100;

        [JsonIgnore]
        public DateTime ReciveTime { get; set; }
        public double BoilerRequired { get; set; }
        [JsonConverter(typeof(UnixDateTimeConverter))]
        public DateTime CycleStart { get; set; }
        public double Delta { get; set; }
        public bool IsBoilerOn { get; set; }
        public bool IsHeating { get; set; }
        public string AdditionalInfo { get; set; }
        public List<Sensor> Sensors { get; set; }
    }

    public struct Sensor : ICloneable {
        public const int SensorIdForBoilerRequired = 1000;
        public const int SensorIdForBoilerState = 1001;

        public int SensorId { get; set; }
        public bool IsInvalid { get; set; }
        public double Average { get; set; }
        public double Last { get; set; }
        [JsonConverter(typeof(UnixDateTimeConverter))]
        public DateTime Time { get; set; }
        public bool Warning { get; set; }

        public object Clone() {
            return new Sensor {
                SensorId = this.SensorId,
                IsInvalid = this.IsInvalid,
                Average = this.Average,
                Last = this.Last,
                Time = this.Time,
                Warning = this.Warning
            };
        }
    }
}
