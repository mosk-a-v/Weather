using DevExpress.Xpo;
using System;

namespace Common {
    [Persistent("TemperatureSettings")]
    public class SettingValue : XPLiteObject {
        [Persistent("Id"), Key(true)]
        int id;
        int weekDay;
        int hour;
        float temperature;

        [PersistentAlias("id")]
        public int Id {
            get { return id; }
        }
        public int WeekDay {
            get { return weekDay; }
            set { SetPropertyValue(nameof(WeekDay), ref weekDay, value); }
        }
        public int Hour {
            get { return hour; }
            set { SetPropertyValue(nameof(Hour), ref hour, value); }
        }
        public float Temperature {
            get { return temperature; }
            set { SetPropertyValue(nameof(Temperature), ref temperature, value); }
        }

        public SettingValue(Session session) : base(session) { }
    }
}
