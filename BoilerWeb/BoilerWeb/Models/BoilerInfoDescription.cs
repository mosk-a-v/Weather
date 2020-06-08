using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace BoilerWeb {
    public static class BoilerInfoDescription {
        public static Dictionary<int, string> IndoorSensors = new Dictionary<int, string> {
            {8, "Провод" }, { 6,"Кабинет"}, { 2, "Спальня"}, {4, "Зал" }, { 5,"Мансарда"}, { 10, "Кухня"}
        };

        public static Dictionary<int, string> OutdoorSensors = new Dictionary<int, string> {
            { 1, "Радио "}, {9, "Провод" }, {102, "Инет" }
        };

        public static Dictionary<int, string> BoilerSensors = new Dictionary<int, string> {
            {7, "Провод" }, { 3, "Радио"}, { 1000, "Требуется"}
        };

        public static Dictionary<int, string> HumiditySensors = new Dictionary<int, string> {
            { 26, "Кабинет"}, { 24, "Зал"}, {25, "Мансарда" }, { 30, "Кухня"}
        };

        public static Dictionary<int, string> InfoSensors = new Dictionary<int, string> {
            { 100, "Солнце"}, {101, "Ветер" }
        };
    }
}
