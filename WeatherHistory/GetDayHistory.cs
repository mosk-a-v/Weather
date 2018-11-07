using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;

namespace WeatherHistory {
    public class WeatherHistoryService {
        readonly Uri serviceUri;
        readonly Uri host;
        public WeatherHistoryService(string serviceUrl) {
            this.serviceUri = new Uri(serviceUrl);
            this.host = new Uri(serviceUri.GetLeftPart(UriPartial.Authority));
        }
        HttpResponseMessage Get(int year, int month, int day) {
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
        public List<string> GetHistoryFrom(int year, int month, int day) {
            var response = Get(year, month, day);
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
    }
}
