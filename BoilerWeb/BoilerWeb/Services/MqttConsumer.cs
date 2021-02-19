using Microsoft.Extensions.Hosting;
using MQTTnet;
using MQTTnet.Client.Receiving;
using System.Threading;
using System.Threading.Tasks;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Extensions.Logging;

namespace BoilerWeb {
    public class MqttConsumer : IHostedService {
        const int QueueCapacity = 400;
        readonly IMqttClientService mqttClientService;
        private readonly ILogger logger;
        BoilerInfoModel boilerInfoModel = new BoilerInfoModel { Sensors = new List<Sensor>() };
        DateTime lastFullModelRecieved = DateTime.MinValue;
        Queue<BoilerInfoModel> infoModelsHistory = new Queue<BoilerInfoModel>(QueueCapacity);
        Queue<BoilerInfoModel> voltageModelsHistory = new Queue<BoilerInfoModel>(QueueCapacity * 2);
        Dictionary<int, double> lastValues = new Dictionary<int, double>();

        readonly ReaderWriterLockSlim readerWriterLockSlim;

        public BoilerInfoModel BoilerInfoModel {
            get {
                readerWriterLockSlim.EnterReadLock();
                try {
                    return boilerInfoModel;
                } finally {
                    readerWriterLockSlim.ExitReadLock();
                }
            }
        }
        public List<BoilerInfoModel> History {
            get {
                readerWriterLockSlim.EnterReadLock();
                try {
                    return infoModelsHistory.ToList();
                } finally {
                    readerWriterLockSlim.ExitReadLock();
                }
            }
        }
        public List<BoilerInfoModel> VoltageHistory {
            get {
                readerWriterLockSlim.EnterReadLock();
                try {
                    return voltageModelsHistory.ToList();
                } finally {
                    readerWriterLockSlim.ExitReadLock();
                }
            }
        }

        public MqttConsumer(MqttClientServiceProvider provider, ILogger<MqttConsumer> logger) {
            this.readerWriterLockSlim = new ReaderWriterLockSlim();
            this.mqttClientService = provider.MqttClientService;
            this.logger = logger;
        }

        public Task StartAsync(CancellationToken cancellationToken) {
            mqttClientService.OnMessageReceived += MqttClientService_OnMessageReceived;
            return Task.CompletedTask;
        }

        public Task StopAsync(CancellationToken cancellationToken) {
            mqttClientService.OnMessageReceived -= MqttClientService_OnMessageReceived;
            return Task.CompletedTask;
        }

        void MqttClientService_OnMessageReceived(object sender, MqttApplicationMessageReceivedEventArgs e) {
            string json = e.ApplicationMessage.ConvertPayloadToString();
            try {
                var model = JsonConvert.DeserializeObject<BoilerInfoModel>(json);
                model.Sensors.Add(new Sensor() { 
                    SensorId = Sensor.SensorIdForBoilerRequired,
                    Last = model.BoilerRequired,
                    Time = model.CycleStart,
                    Average = model.BoilerRequired
                });
                model.Sensors.Add(new Sensor() {
                    SensorId = Sensor.SensorIdForBoilerState,
                    Last = model.IsBoilerOn ? 0.4 : 0,
                    Time = model.CycleStart,
                    Average = model.IsBoilerOn ? 0.4 : 0
                });
                model.ReciveTime = DateTime.UtcNow;
                readerWriterLockSlim.EnterWriteLock();
                try {
                    boilerInfoModel = model;
                    SaveValidValues(model);
                    if(boilerInfoModel.ReciveTime.Subtract(lastFullModelRecieved).TotalMinutes >= 10) {
                        if(infoModelsHistory.Count >= QueueCapacity) {
                            infoModelsHistory.Dequeue();
                        }
                        SubstituteEmptyValues(model);
                        infoModelsHistory.Enqueue(model);
                        lastFullModelRecieved = boilerInfoModel.ReciveTime;
                    }
                    if(voltageModelsHistory.Count >= QueueCapacity * 2) {
                        voltageModelsHistory.Dequeue();
                    }
                    model = new BoilerInfoModel() { ReciveTime = DateTime.UtcNow };
                    var sensor = boilerInfoModel.Sensors.First(s => s.SensorId == 103);
                    model.Sensors = new List<Sensor>() { (Sensor)sensor.Clone() };
                    if(voltageModelsHistory.Count == 0 || Math.Abs(voltageModelsHistory.Peek().Sensors[0].Last - sensor.Last) > 0.005 || sensor.Time.Subtract(voltageModelsHistory.Peek().Sensors[0].Time).TotalSeconds > 5) {
                        voltageModelsHistory.Enqueue(model);
                    }
                } finally {
                    readerWriterLockSlim.ExitWriteLock();
                }
            } catch { }
        }

        void SaveValidValues(BoilerInfoModel model) {
            foreach(var sensor in model.Sensors) {
                if(!sensor.IsInvalid || sensor.Last != BoilerInfoModel.DefaultValue) {
                    if(!lastValues.TryAdd(sensor.SensorId, sensor.Last)) {
                        lastValues[sensor.SensorId] = sensor.Last;
                    }
                }
            }
        }

        void SubstituteEmptyValues(BoilerInfoModel model) {
            for(int i = 0; i < model.Sensors.Count; i++) {
                if(model.Sensors[i].IsInvalid || model.Sensors[i].Last == BoilerInfoModel.DefaultValue) {
                    if(lastValues.TryGetValue(model.Sensors[i].SensorId, out double value)) {
                        var sensor = (Sensor)model.Sensors[i].Clone();
                        sensor.Last = value;
                        model.Sensors[i] = sensor;
                    }
                }
            }
        }
    }
}
