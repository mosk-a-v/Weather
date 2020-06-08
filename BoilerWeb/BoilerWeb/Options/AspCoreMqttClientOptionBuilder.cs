using MQTTnet.Client.Options;
using System;

namespace BoilerWeb {
    public class AspCoreMqttClientOptionBuilder : MqttClientOptionsBuilder {
        public IServiceProvider ServiceProvider { get; }

        public AspCoreMqttClientOptionBuilder(IServiceProvider serviceProvider) {
            ServiceProvider = serviceProvider;
        }
    }
}
