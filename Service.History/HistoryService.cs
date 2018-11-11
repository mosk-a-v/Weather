using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;

namespace Service.History {
    public class HistoryService {
        readonly Uri serviceUri;
        readonly Uri host;
        public HistoryService(string serviceUrl) {
            this.serviceUri = new Uri(serviceUrl);
            this.host = new Uri(serviceUri.GetLeftPart(UriPartial.Authority));
        }
        public List<HistoryData> GetHistoryFrom(int year, int month, int day) {
            var response = GetResponse(year, month, day);
            var data = ExtractData(response);
            return ParseData(data);
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
                var result = client.PostAsync(serviceUri, formContent).Result;
                result.EnsureSuccessStatusCode();
                return result;
            }
        }
        List<string> ExtractData(HttpResponseMessage response) {
            var content = response.Content.ReadAsStringAsync().Result;
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
                return val == "?" ? 10 : int.Parse(val);
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
                    .Select(item => int.Parse(item))
                    .Average()));
            } else {
                int additionalSymbol = value.IndexOf("{");
                return additionalSymbol > 0 ? int.Parse(value.Substring(0, additionalSymbol)) : int.Parse(value);
            }
        }
    }
}
