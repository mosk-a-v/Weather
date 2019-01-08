using Common;
using DevExpress.Xpo;
using DevExpress.Xpo.DB;
using DevExpress.Xpo.Metadata;
using System;
using System.Collections.Generic;
using System.Configuration;

namespace Temperature {
    public class LocalHistoryService {
        const string linesQuery = @"
select device, date_add(makedate(year(time), DAYOFYEAR(time)), interval hour(time) HOUR) as date_hour, avg(Value) 
from weather.Temperature
where device in(113,150,179) 
group by device, date_add(makedate(year(time), DAYOFYEAR(time)), interval hour(time) HOUR)
order by device, date_add(makedate(year(time), DAYOFYEAR(time)), interval hour(time) HOUR)";
        const string totalQuery = @"select max(time), count(*) from weather.Temperature";

        readonly DBProvider dBProvider;

        public LocalHistoryService(DBProvider dBProvider) {
            this.dBProvider = dBProvider;
        }
        public LocalHistoryDTO GetLocalHistory() {
            var result = new LocalHistoryDTO();
            using(var uow = dBProvider.GetRaspperyUnitOwWork()) {
                var selectResult = uow.ExecuteQuery(totalQuery).ResultSet[0].Rows[0].Values;
                result.Last = Utils.GetDate(selectResult[0]);
                result.Count = Convert.ToInt32((long)selectResult[1]);
                foreach(var row in uow.ExecuteQuery(linesQuery).ResultSet[0].Rows) {
                    if((int)row.Values[0] == 150) {
                        result.Indoor.Add(new AverageValue { Hour = Utils.GetDate(row.Values[1]), Value = (double)row.Values[2] });
                    } else if((int)row.Values[0] == 113) {
                        result.Outdoor.Add(new AverageValue { Hour = Utils.GetDate(row.Values[1]), Value = (double)row.Values[2] });
                    } else if((int)row.Values[0] == 179) {
                        result.Boiler.Add(new AverageValue { Hour = Utils.GetDate(row.Values[1]), Value = (double)row.Values[2] });
                    }
                }
            }
            return result;
        }
    }

    public class LocalHistoryDTO {
        public List<AverageValue> Indoor { get; private set; }
        public List<AverageValue> Outdoor { get; private set; }
        public List<AverageValue> Boiler { get; private set; }
        public DateTime Last { get; set; }
        public int Count { get; set; }

        public LocalHistoryDTO() {
            this.Indoor = new List<AverageValue>();
            this.Outdoor = new List<AverageValue>();
            this.Boiler = new List<AverageValue>();
        }
    }
}
