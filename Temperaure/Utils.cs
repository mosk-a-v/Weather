using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Temperature {
    public static class Utils {
        public static DateTime GetDate(object date) {
            return DateTime.SpecifyKind(((DateTime)date), DateTimeKind.Utc).ToLocalTime();
        }
    }
}
