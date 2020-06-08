using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace BoilerWeb {
    public class BoilerHistoryModel {
        public List<string> Description { get; set; }
        public List<List<Sensor>> Sensors { get; set; }
    }
}
