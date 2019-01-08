using Common;
using DevExpress.Xpo;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Temperature {
    public static class MatrixGenerator {
        const int BoilerLover = 25;
        const int BoilerHight = 60;
        const int IndoorLover = 18;
        const int IndoorHight = 22;
        const int OutdoorLover = -25;
        const int OutdoorHight = 15;
        const int SunLover = 0;
        const int SunHihg = 10;
        const int WindLover = 0;
        const int WindHigh = 20;
        const float K_Free = 0.65F;
        const float K_Indoor = 0.105F;
        const float K_Outdoor = -0.415F;
        const float K_Sun = 0;
        const float K_Wind = 0;

        public static void WriteControlMatrix(DBProvider dBProvider) {
            using(var uow = dBProvider.GetRaspperyUnitOwWork()) {
                var oldValues = new XPQuery<ControlValue>(uow).ToList();
                oldValues.ForEach(v => v.Delete());
                uow.CommitChanges();
                for(int indoor = IndoorLover; indoor <= IndoorHight; indoor++) {
                    for(int outdoor = OutdoorLover; outdoor <= OutdoorHight; outdoor++) {
                        //for(int sun = SunLover; sun <= SunHihg; sun++) {
                        //    for(int wind = WindLover; wind <= WindHigh; wind++) {
                        new ControlValue(uow) {
                            Boiler = CalcBoilerValue(indoor, outdoor, SunLover, WindLover),
                            Indoor = indoor,
                            Outdoor = outdoor,
                            Sun = SunLover,
                            Wind = WindLover
                        };
                        // }
                        //}
                    }
                }
                uow.CommitChanges();
            }
        }
        public static void WriteSettings(DBProvider dBProvider) {
            using(var uow = dBProvider.GetRaspperyUnitOwWork()) {
                var oldValues = new XPQuery<SettingValue>(uow).ToList();
                oldValues.ForEach(v => v.Delete());
                uow.CommitChanges();
                for(int day = 1; day <= 7; day++) {
                    for(int hour = 0; hour <= 23; hour++) {
                        new SettingValue(uow) {
                            WeekDay = day,
                            Hour = hour,
                            Temperature = 20
                        };
                    }
                }
                uow.CommitChanges();
            }
        }
        public static void OutToCSVFile(LocalHistoryDTO localHistoryData, GlobalHistoryDTO globalHistoryData) {
            var indoor = new List<AverageValue>();
            var outdoor = new List<AverageValue>();
            var boiler = new List<AverageValue>();
            var overcast = new List<AverageValue>();
            var wind = new List<AverageValue>();

            DateTime[] ignoredDates = new DateTime[] { new DateTime(2018, 11, 03), new DateTime(2018, 11, 11),
                new DateTime(2018, 11, 14), new DateTime(2019, 01, 02), new DateTime(2019, 01, 03), new DateTime(2019, 01, 04),
                new DateTime(2019, 01, 05), new DateTime(2019, 01, 06), new DateTime(2019, 01, 07)};
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
                result[i, 0] = (outdoor[i].Value - OutdoorLover) / (OutdoorHight - OutdoorLover);
                result[i, 1] = (indoor[i].Value - IndoorLover) / (IndoorHight - IndoorLover);
                result[i, 2] = ((overcast[i].Hour.TimeOfDay.Hours > 8 && overcast[i].Hour.TimeOfDay.Hours < 16 ? SunHihg - overcast[i].Value : 0) - SunLover) / (SunHihg - SunLover);
                result[i, 3] = (wind[i].Value - WindLover) / (WindHigh - WindLover);
                result[i, 4] = (boiler[i].Value - BoilerLover) / (BoilerHight - BoilerLover);
            }
            StringBuilder sb = new StringBuilder();
            sb.AppendLine($"Улица; Дом; Солнце; Ветер; Котел");
            for(int i = 0; i < wind.Count; i++) {
                sb.AppendLine($"{result[i, 0]};{result[i, 1]};{result[i, 2]};{result[i, 3]};{result[i, 4]}");
            }
            File.WriteAllText(@"d:\out.csv", sb.ToString());
        }
        static float CalcBoilerValue(int indoor, int outdoor, int overcast, int wind) {
            return (float)Math.Round((K_Free +
                K_Indoor * (((float)indoor - IndoorLover) / (IndoorHight - IndoorLover)) +
                K_Outdoor * (((float)outdoor - OutdoorLover) / (OutdoorHight - OutdoorLover)) +
                K_Sun * (((SunHihg - (float)overcast) - SunLover) / (SunHihg - SunLover)) +
                K_Wind * (((float)wind - WindLover) / (WindHigh - WindLover)))
                * (BoilerHight - BoilerLover) + BoilerLover, 1);
        }
    }
}
