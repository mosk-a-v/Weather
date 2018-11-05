using DevExpress.Xpo;
using System;

namespace Temperaure {
    class Temperature : XPLiteObject {
        [Persistent("Id"), Key(true)]
        int id;
        DateTime time;
        float _value;

        [PersistentAlias("id")]
        public int Id {
            get { return id; }
        }

        public DateTime Time {
            get { return time; }
            set { SetPropertyValue(nameof(Time), ref time, value); }
        }
        public float Value {
            get { return _value; }
            set { SetPropertyValue(nameof(Value), ref _value, value); }
        }

        public Temperature(Session session) : base(session) { }
    }
}
