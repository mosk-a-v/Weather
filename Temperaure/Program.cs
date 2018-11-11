using DevExpress.Xpo;
using DevExpress.Xpo.DB;
using DevExpress.Xpo.Metadata;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Windows.Forms;
using WeatherHistory;

namespace Temperature {
    static class Program {
        const string linesQuery = @"
select device, date_add(makedate(year(time), DAYOFYEAR(time)), interval hour(time) HOUR) as date_hour, avg(Value) 
from weather.Temperature
where device in(113,150,179) 
group by device, date_add(makedate(year(time), DAYOFYEAR(time)), interval hour(time) HOUR)
order by device, date_add(makedate(year(time), DAYOFYEAR(time)), interval hour(time) HOUR)";
        const string totalQuery = @"select max(time), count(*) from weather.Temperature";
        static IDataLayer dataLayer;
        static IDataStore dataStore;

        [STAThread]
        static void Main() {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            InitDB();
            List<AverageValue> indoor = new List<AverageValue>();
            List<AverageValue> outdoor = new List<AverageValue>();
            List<AverageValue> boiler = new List<AverageValue>();
            DateTime last;
            int count;
            using(var uow = GetSesion()) {
                var result = uow.ExecuteQuery(totalQuery).ResultSet[0].Rows[0].Values;
                last = GetDate(result[0]);
                count = Convert.ToInt32((long)result[1]);
                foreach(var row in uow.ExecuteQuery(linesQuery).ResultSet[0].Rows) {
                    if((int)row.Values[0] == 150) {
                        indoor.Add(new AverageValue { Hour = GetDate(row.Values[1]), Value = (double)row.Values[2] });
                    } else if((int)row.Values[0] == 113) {
                        outdoor.Add(new AverageValue { Hour = GetDate(row.Values[1]), Value = (double)row.Values[2] });
                    } else if((int)row.Values[0] == 179) {
                        boiler.Add(new AverageValue { Hour = GetDate(row.Values[1]), Value = (double)row.Values[2] });
                    }
                }
            }
            var weatherService = new WeatherHistoryService(ConfigurationManager.AppSettings["HistoryService"]);
            var historyData = weatherService.GetHistoryFrom(2018, 11, 03);
            List<AverageValue> station = new List<AverageValue>();
            List<AverageValue> overcast = new List<AverageValue>();
            List<AverageValue> wind = new List<AverageValue>();
            foreach(var row in historyData.OrderBy(item=>item.Date)) {
                station.Add(new AverageValue { Hour = row.Date.ToLocalTime(), Value = row.EffectiveTemperature });
                overcast.Add(new AverageValue { Hour = row.Date.ToLocalTime(), Value = row.Overcast });
                wind.Add(new AverageValue { Hour = row.Date.ToLocalTime(), Value = row.WindSpeed });
            }
            Application.Run(new Form1(indoor, outdoor, boiler, count, last, station, overcast, wind));
        }

        static void InitDB() {
            dataStore = XpoDefault.GetConnectionProvider(ConfigurationManager.ConnectionStrings["Weather"].ConnectionString, AutoCreateOption.SchemaAlreadyExists);
            var dict = new ReflectionDictionary();
            dict.CollectClassInfos(typeof(TemperatureValue).Assembly);
            dataLayer = new ThreadSafeDataLayer(dict, dataStore);
        }

        static UnitOfWork GetSesion() {
            return new UnitOfWork(dataLayer);
        }

        static DateTime GetDate(object date) {
            return DateTime.SpecifyKind(((DateTime)date), DateTimeKind.Utc).ToLocalTime();
        }
    }
}
