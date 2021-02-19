using Microsoft.Extensions.Hosting;
using MQTTnet;
using MQTTnet.Client.Connecting;
using MQTTnet.Client.Disconnecting;
using MQTTnet.Client.Receiving;
using System;

namespace BoilerWeb {
    public interface IMqttClientService : IHostedService,
                                          IMqttClientConnectedHandler,
                                          IMqttClientDisconnectedHandler,
                                          IMqttApplicationMessageReceivedHandler,
                                          IApplicationMessagePublisher {
        event EventHandler<MQTTnet.MqttApplicationMessageReceivedEventArgs> OnMessageReceived;

    }
}
