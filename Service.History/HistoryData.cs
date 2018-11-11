using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Service.History {
    public class HistoryData {
        public DateTime Date { get; set; }
        public double Temperature { get; set; }
        public double EffectiveTemperature { get; set; }
        public int WindSpeed { get; set; }
        public int Overcast { get; set; }
        public int AtmospherePressure { get; set; }
    }
}
