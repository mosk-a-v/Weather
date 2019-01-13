using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;

namespace Temperature {
    /* 
             Сервис:
                http://meteocenter.net/27719_fact.htm
             формат ответа:
                Время;Дата;Напр.ветра;Ск.ветра;Видим.;Явл.;Обл.;Форма обл.;Т;Тd;f;Тe;Pmin;dP;Po;Тmin;Tmax;R;s;Сост.почвы;
    
         */
    public class HistoryService {
        readonly Uri serviceUri;
        readonly Uri host;
        readonly string fileName;
        public HistoryService(Uri serviceUri) {
            this.serviceUri = serviceUri;
            this.host = new Uri(serviceUri.GetLeftPart(UriPartial.Authority));
        }
        public HistoryService(string fileName) {
            this.fileName = fileName;
        }
        public GlobalHistoryDTO GetHistoryFrom(int year, int month, int day) {
            List<string> data;
            if(serviceUri != null) {
                var response = GetResponse(year, month, day);
                data = ExtractData(response.Content.ReadAsStringAsync().Result);
            } else {
                data = ExtractData(File.ReadAllText(fileName));
            }
            var parsedData = ParseData(data);
            var result = new GlobalHistoryDTO();
            foreach(var row in parsedData.OrderBy(item => item.Date)) {
                result.Station.Add(new AverageValue { Hour = row.Date.ToLocalTime(), Value = row.EffectiveTemperature });
                result.Overcast.Add(new AverageValue { Hour = row.Date.ToLocalTime(), Value = row.Overcast });
                result.Wind.Add(new AverageValue { Hour = row.Date.ToLocalTime(), Value = row.WindSpeed });
            }
            return result;
        }
        HttpResponseMessage GetResponse(int year, int month, int day) {
            var cookieContainer = new CookieContainer();
            var handler = new HttpClientHandler() { CookieContainer = cookieContainer };
            using(var client = new HttpClient(handler)) {
                var formContent = new FormUrlEncodedContent(new[] {
                    new KeyValuePair<string, string>("Date[0]", day.ToString("00")),
                    new KeyValuePair<string, string>("Date[1]", month.ToString("00")),
                    new KeyValuePair<string, string>("Date[2]", year.ToString("0000")),
                    new KeyValuePair<string, string>("CSV", "on"),
                    new KeyValuePair<string, string>("Station", "27719")
                });
                cookieContainer.Add(host, new Cookie("point[0]", "27719_fact"));
                cookieContainer.Add(host, new Cookie("point[1]", "forecastall.php"));
                cookieContainer.Add(host, new Cookie("csuid", "udP0UVwUFXEesAhmAy7bAg=="));
                cookieContainer.Add(host, new Cookie("CSv", "0tTW1oLUh92FgYCBhtDQ3NHR1YLT0tTW0NfR0dLRgIc="));
                var result = client.PostAsync(serviceUri, formContent).Result;
                result.EnsureSuccessStatusCode();
                return result;
            }
        }
        List<string> ExtractData(string content) {
            int startPos = content.IndexOf("<pre>");
            int endPoss = content.IndexOf("</pre>");
            return startPos == 0 || endPoss == 0 ?
                    new List<string>() :
                    content.Substring(startPos + 6, endPoss - startPos - 6)
                    .Replace("\r", string.Empty)
                    .Split(new string[] { "\n" }, StringSplitOptions.RemoveEmptyEntries)
                    .Skip(1)
                    .ToList();
        }
        List<HistoryData> ParseData(List<string> data) {
            var result = new List<HistoryData>();
            foreach(var line in data) {
                var columns = line.Split(';');
                if(columns.Length < 15 || columns[0].Length < 2) {
                    continue;
                }
                var datePart = columns[1].Split('.').Select(item => int.Parse(item)).ToList();
                if(datePart.Any(item => item == 0)) {
                    continue;
                }
                datePart.Add(int.Parse(columns[0]));
                result.Add(new HistoryData {
                    Date = new DateTime(datePart[2] + 2000, datePart[1], datePart[0], datePart[3], 0, 0, DateTimeKind.Utc),
                    Temperature = double.Parse(columns[8].Replace(".", ",")),
                    EffectiveTemperature = double.Parse(columns[11].Replace(".", ",")),
                    AtmospherePressure = ParsePressure(columns[14]),
                    WindSpeed = ParseWind(columns[3]),
                    Overcast = ParseOvercast(columns[6])
                });
            }
            return result;
        }
        int ParseOvercast(string value) {
            int fractionSymbol = value.IndexOf("/");
            if(fractionSymbol > 0) {
                string val = value.Substring(0, fractionSymbol);
                return val == "?" || val == "�" ? 10 : int.Parse(val);
            }
            return 0;
        }
        int ParsePressure(string value) {
            return Convert.ToInt32(Math.Round(double.Parse(value.Replace(".", ",")) * 0.750063755419211, 0));
        }
        int ParseWind(string value) {
            if(string.IsNullOrEmpty(value)) {
                return 0;
            } else if(value.Contains("-")) {
                return Convert.ToInt32(Math.Round(value.Split('-')
                    .Select(item => ParseWindGust(item))
                    .Average()));
            } else {
                return ParseWindGust(value);
            }
        }
        int ParseWindGust(string value) {
            int additionalSymbol = value.IndexOf("{");
            return additionalSymbol > 0 ? int.Parse(value.Substring(0, additionalSymbol)) : int.Parse(value);
        }
    }

    public class GlobalHistoryDTO {
        public List<AverageValue> Station { get; set; }
        public List<AverageValue> Overcast { get; set; }
        public List<AverageValue> Wind { get; set; }

        public GlobalHistoryDTO() {
            this.Station = new List<AverageValue>();
            this.Overcast = new List<AverageValue>();
            this.Wind = new List<AverageValue>();
        }
    }

    public class HistoryData {
        public DateTime Date { get; set; }
        public double Temperature { get; set; }
        public double EffectiveTemperature { get; set; }
        public int WindSpeed { get; set; }
        public int Overcast { get; set; }
        public int AtmospherePressure { get; set; }
    }
}
