using MQTTnet;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace BoilerWeb {
    public class MqttPublisher {
        readonly IMqttClientService mqttClientService;

        public MqttPublisher(MqttClientServiceProvider provider) {
            mqttClientService = provider.MqttClientService;
        }

        public Task Publish(CommandModel command) {
            var message = new MqttApplicationMessageBuilder()
                .WithTopic("BOILER_COMMAND/JSON")
                .WithPayload(JsonConvert.SerializeObject(command))
                .Build();
            return mqttClientService.PublishAsync(message, CancellationToken.None);
        }
    }
}
