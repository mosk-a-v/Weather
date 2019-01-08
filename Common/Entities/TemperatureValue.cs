using DevExpress.Xpo;
using System;

namespace Common {
    [Persistent("Temperature")]
    public class TemperatureValue : XPLiteObject {
        [Persistent("Id"), Key(true)]
        int id;
        DateTime time;
        float _value;
        int device;

        [PersistentAlias("id")]
        public int Id {
            get { return id; }
        }

        [Indexed]
        public DateTime Time {
            get { return time; }
            set { SetPropertyValue(nameof(Time), ref time, value); }
        }
        public float Value {
            get { return _value; }
            set { SetPropertyValue(nameof(Value), ref _value, value); }
        }
        public int Device {
            get { return device; }
            set { SetPropertyValue(nameof(Device), ref device, value); }
        }

        public TemperatureValue(Session session) : base(session) { }
    }
}
