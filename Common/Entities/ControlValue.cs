using DevExpress.Xpo;
using System;

namespace Common {
    [Persistent("ControlTable")]
    public class ControlValue : XPLiteObject {
        [Persistent("Id"), Key(true)]
        int id;
        int sun;
        int wind;
        float indoor;
        float outdoor;
        float boiler;

        [PersistentAlias("id")]
        public int Id {
            get { return id; }
        }
        public int Sun {
            get { return sun; }
            set { SetPropertyValue(nameof(Sun), ref sun, value); }
        }
        public int Wind {
            get { return wind; }
            set { SetPropertyValue(nameof(Wind), ref wind, value); }
        }
        public float Indoor {
            get { return indoor; }
            set { SetPropertyValue(nameof(Indoor), ref indoor, value); }
        }
        public float Outdoor {
            get { return outdoor; }
            set { SetPropertyValue(nameof(Outdoor), ref outdoor, value); }
        }
        public float Boiler {
            get { return boiler; }
            set { SetPropertyValue(nameof(Boiler), ref boiler, value); }
        }

        public ControlValue(Session session) : base(session) { }
    }
}
