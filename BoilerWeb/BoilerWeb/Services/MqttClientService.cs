using MQTTnet;
using MQTTnet.Client;
using MQTTnet.Client.Connecting;
using MQTTnet.Client.Disconnecting;
using MQTTnet.Client.Options;
using MQTTnet.Client.Publishing;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace BoilerWeb {
    public class MqttClientService : IMqttClientService {
        private IMqttClient mqttClient;
        private IMqttClientOptions options;
        public event EventHandler<MqttApplicationMessageReceivedEventArgs> OnMessageReceived;

        public MqttClientService(IMqttClientOptions options) {
            this.options = options;
            mqttClient = new MqttFactory().CreateMqttClient();
            ConfigureMqttClient();
        }

        private void ConfigureMqttClient() {
            mqttClient.ConnectedHandler = this;
            mqttClient.DisconnectedHandler = this;
            mqttClient.ApplicationMessageReceivedHandler = this;
        }

        public Task HandleApplicationMessageReceivedAsync(MqttApplicationMessageReceivedEventArgs eventArgs) {
            return Task.Run(() => OnMessageReceived?.Invoke(this, eventArgs));
        }

        public async Task HandleConnectedAsync(MqttClientConnectedEventArgs eventArgs) {
            await mqttClient.SubscribeAsync("#");
        }

        public Task HandleDisconnectedAsync(MqttClientDisconnectedEventArgs eventArgs) {
            Task.Delay(TimeSpan.FromSeconds(5));
            return mqttClient.ConnectAsync(options, CancellationToken.None);
        }

        public async Task StartAsync(CancellationToken cancellationToken) {
            await mqttClient.ConnectAsync(options);
            if (!mqttClient.IsConnected) {
                await mqttClient.ReconnectAsync();
            }
        }

        public async Task StopAsync(CancellationToken cancellationToken) {
            if (cancellationToken.IsCancellationRequested) {
                var disconnectOption = new MqttClientDisconnectOptions {
                    ReasonCode = MqttClientDisconnectReason.NormalDisconnection,
                    ReasonString = "NormalDiconnection"
                };
                await mqttClient.DisconnectAsync(disconnectOption, cancellationToken);
            }
            await mqttClient.DisconnectAsync();
        }

        public Task<MqttClientPublishResult> PublishAsync(MqttApplicationMessage applicationMessage, CancellationToken cancellationToken) {
            return mqttClient.PublishAsync(applicationMessage, cancellationToken);
        }
    }
}
