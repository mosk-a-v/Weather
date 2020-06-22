using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using BoilerWeb.Models;

namespace BoilerWeb.Controllers {
    public enum BoilerMode {
        Auto, Direct
    }

    public class HomeController : Controller {

        readonly ILogger<HomeController> _logger;
        readonly MqttConsumer mqttConsumer;
        readonly MqttPublisher mqttPublisher;

        public HomeController(ILogger<HomeController> logger, MqttConsumer mqttConsumer, MqttPublisher mqttPublisher) {
            this._logger = logger;
            this.mqttConsumer = mqttConsumer;
            this.mqttPublisher = mqttPublisher;
        }

        [HttpGet]
        public IActionResult Index() {
            return View(mqttConsumer.BoilerInfoModel);
        }

        [HttpGet]
        public IActionResult Simple() {
            return View(mqttConsumer.BoilerInfoModel);
        }

        [HttpGet]
        public IActionResult OutdoorHistory() {
            var model = GetHistory(BoilerInfoDescription.OutdoorSensors);
            return View("History", model);
        }

        [HttpGet]
        public IActionResult IndoorHistory() {
            var model = GetHistory(BoilerInfoDescription.IndoorSensors);
            return View("History", model);
        }

        [HttpGet]
        public IActionResult BoilerHistory() {
            var model = GetHistory(BoilerInfoDescription.BoilerSensors);
            return View("History", model);
        }

        [HttpGet]
        public IActionResult HumidityHistory() {
            var model = GetHistory(BoilerInfoDescription.HumiditySensors);
            return View("History", model);
        }

        [HttpGet]
        public IActionResult InfoHistory() {
            var model = GetHistory(BoilerInfoDescription.InfoSensors);
            return View("History", model);
        }

        [HttpPost]
        public IActionResult SetMode(BoilerMode mode, float value) {
            switch(mode) {
                case BoilerMode.Auto:
                    mqttPublisher.Publish(new CommandModel { Command = "Normal", Value = 0 });
                    break;
                case BoilerMode.Direct:
                    mqttPublisher.Publish(new CommandModel { Command = "Direct", Value = value });
                    break;
            }
            return RedirectToAction("Index");
        }

        [HttpGet]
        public IActionResult Settings() {
            return View();
        }

        public BoilerHistoryModel GetHistory(Dictionary<int, string> description) {
            var indexies = description.Select(x => x.Key).Where(x => x < 999).ToArray();
            var sensors = mqttConsumer.History
                .Select(x => new {
                    Sensors = x.Sensors.Where(s => indexies.Contains(s.SensorId)).ToList(),
                    Time = x.ReciveTime
                })
                .Select(x => new {
                    Sensors = x.Sensors.Select(s => new Sensor { Last = s.Last, Time = x.Time }).OrderBy(s => s.SensorId).ToList(),
                    Time = x.Time
                })
                .OrderBy(x => x.Time)
                .Select(x => x.Sensors)
                .ToList();
            var model = new BoilerHistoryModel {
                Sensors = sensors,
                Description = description.Where(d => indexies.Contains(d.Key)).OrderBy(x => x.Key).Select(x => x.Value).ToList()
            };
            return model;
        }
    }
}
