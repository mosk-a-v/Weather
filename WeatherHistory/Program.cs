using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WeatherHistory {
    static class Program {
        static void Main() {
            var service = new WeatherHistoryService("http://meteocenter.net/forecast/all.php");
            var result = service.GetHistoryFrom(2018, 10, 01);
        }
    }
}
