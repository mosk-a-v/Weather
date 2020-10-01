using Microsoft.Extensions.Hosting;
using MQTTnet;
using MQTTnet.Client.Receiving;
using System.Threading;
using System.Threading.Tasks;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;

namespace BoilerWeb {
    public class MqttConsumer : IHostedService {
        const int QueueCapacity = 400;
        readonly IMqttClientService mqttClientService;
        BoilerInfoModel boilerInfoModel = new BoilerInfoModel { Sensors = new List<Sensor>() };
        DateTime lastFullModelRecieved = DateTime.MinValue;
        Queue<BoilerInfoModel> infoModelsHistory = new Queue<BoilerInfoModel>(QueueCapacity);
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

        public MqttConsumer(MqttClientServiceProvider provider) {
            readerWriterLockSlim = new ReaderWriterLockSlim();
            mqttClientService = provider.MqttClientService;
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
