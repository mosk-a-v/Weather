using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Temperature {
    public static class MatrixGenerator {
        public static void OutToCSVFile(LocalHistoryDTO localHistoryData, GlobalHistoryDTO globalHistoryData) {
            var indoor = new List<AverageValue>();
            var outdoor = new List<AverageValue>();
            var boiler = new List<AverageValue>();
            var overcast = new List<AverageValue>();
            var wind = new List<AverageValue>();

            DateTime[] ignoredDates = new DateTime[] { new DateTime(2018, 11, 03), new DateTime(2018, 11, 11), new DateTime(2018, 11, 14) };
            foreach(var item in globalHistoryData.Wind) {
                if(ignoredDates.Contains(item.Hour.Date)) {
                    continue;
                }
                wind.Add(new AverageValue {
                    Hour = item.Hour,
                    Value = globalHistoryData.Wind.First(x => x.Hour == item.Hour).Value
                });
                overcast.Add(new AverageValue {
                    Hour = item.Hour,
                    Value = globalHistoryData.Overcast.First(x => x.Hour == item.Hour).Value
                });
                boiler.Add(new AverageValue {
                    Hour = item.Hour,
                    Value = localHistoryData.Boiler
                        .Where(x => x.Hour <= item.Hour.AddHours(2) && x.Hour >= item.Hour.AddHours(-2))
                        .Average(x => x.Value)
                });
                outdoor.Add(new AverageValue {
                    Hour = item.Hour,
                    Value = localHistoryData.Outdoor
                        .Where(x => x.Hour <= item.Hour.AddHours(2) && x.Hour >= item.Hour.AddHours(-2))
                        .Average(x => x.Value)
                });
                indoor.Add(new AverageValue {
                    Hour = item.Hour,
                    Value = localHistoryData.Indoor
                        .Where(x => x.Hour <= item.Hour.AddHours(2) && x.Hour >= item.Hour.AddHours(-2))
                        .Average(x => x.Value)
                });
            }
            var result = new double[wind.Count, 5];
            for(int i = 0; i < wind.Count; i++) {
                result[i, 0] = (outdoor[i].Value + 25) / (15 + 25); /*-25...15*/
                result[i, 1] = (indoor[i].Value - 18) / (22 - 18); /*18...22*/
                result[i, 2] = (overcast[i].Hour.TimeOfDay.Hours > 8 && overcast[i].Hour.TimeOfDay.Hours < 16 ? 10 - overcast[i].Value : 0) / 10; /*10 - no sun*/
                result[i, 3] = wind[i].Value / 20;
                result[i, 4] = (boiler[i].Value - 25) / (60 - 25); /*25...60*/
            }
            StringBuilder sb = new StringBuilder();
            for(int i = 0; i < wind.Count; i++) {
                sb.AppendLine($"{result[i, 0]};{result[i, 1]};{result[i, 2]};{result[i, 3]};{result[i, 4]}");
            }
            File.WriteAllText(@"d:\out.csv", sb.ToString());
        }
    }
}
