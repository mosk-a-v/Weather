using Microsoft.AspNetCore.Html;
using Microsoft.AspNetCore.Mvc.Rendering;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web.Mvc;

namespace BoilerWeb.HtmlHelpers {
    public static class BoilerInfoHelper {
        static TimeZoneInfo local = TimeZoneInfo.FindSystemTimeZoneById("Europe/Moscow");

        public static IHtmlContent GetSensorInfo(this IHtmlHelper html, BoilerInfoModel boilerInfoModel, int sensorIndex) {
            return new HtmlString(boilerInfoModel.GetSensorInfo(sensorIndex));
        }
        public static string GetSensorInfo(this BoilerInfoModel boilerInfoModel, int sensorIndex) {
            if(boilerInfoModel.Sensors == null) {
                return string.Empty;
            }
            var sensorInfo = boilerInfoModel.Sensors.FirstOrDefault(s => s.SensorId == sensorIndex);
            if(sensorInfo.SensorId == default(int)) {
                return string.Empty;
            }
            if(sensorInfo.IsInvalid) {
                return "---";
            }
            return $"{(sensorInfo.Warning ? "!": string.Empty)} {sensorInfo.Last:N1} ({ConvertToLocal(sensorInfo.Time).ToString("HH: mm: ss")}) [{sensorInfo.Average:N1}]";
        }
        public static DateTime ConvertToLocal(DateTime dateTime) {
            var dt = DateTime.SpecifyKind(dateTime, DateTimeKind.Utc);
            return TimeZoneInfo.ConvertTimeFromUtc(dt, local);
        }
    }
}
