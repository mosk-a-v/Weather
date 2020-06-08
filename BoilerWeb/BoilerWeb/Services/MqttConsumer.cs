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
        const int QueueCapacity = 200;
        readonly IMqttClientService mqttClientService;
        BoilerInfoModel boilerInfoModel = new BoilerInfoModel { Sensors = new List<Sensor>() };
        DateTime lastFullModelRecieved = DateTime.MinValue;
        Queue<BoilerInfoModel> infoModelsHistory = new Queue<BoilerInfoModel>(QueueCapacity);
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
                    if(boilerInfoModel.ReciveTime.Subtract(lastFullModelRecieved).TotalMinutes >= 10
                        /*boilerInfoModel.Sensors.All(s => s.Last != BoilerInfoModel.DefaultValue)*/) {
                        if(infoModelsHistory.Count >= QueueCapacity) {
                            infoModelsHistory.Dequeue();
                        }
                        infoModelsHistory.Enqueue(model);
                        lastFullModelRecieved = boilerInfoModel.ReciveTime;
                    }
                } finally {
                    readerWriterLockSlim.ExitWriteLock();
                }
            } catch { }
        }
    }
}
