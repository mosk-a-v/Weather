#include "MqttPublisher.h"

MqttPublisher::MqttPublisher(std::string topic, std::string clientId) {
    this->topic = topic;
    this->clientId = clientId;
}
MqttPublisher::~MqttPublisher() {
    Disconnect();
}

void MqttPublisher::Connect() {
    if (client == nullptr || !client->is_connected()) {
        connOpts.set_keep_alive_interval(20);
        connOpts.set_clean_session(true);
        this->client = new mqtt::async_client(MOSQUITTO_BROKER_ADDRESS, clientId);
        client->connect(connOpts)->wait();
    }
}
void MqttPublisher::Disconnect() {
    if (client != nullptr && client->is_connected()) {
        try {
            client->disconnect()->wait();
        }
        catch (std::exception& e) {
            std::stringstream ss;
            ss << "Disconnect from " << topic << " error: " << e.what() << std::endl;
            Utils::WriteLogInfo(LOG_ERR, ss.str());
        }
    }
    if (client != nullptr) {
        delete client;
        client = nullptr;
    }
}
void MqttPublisher::Publish(std::string payload) {
    try {
        Connect();
        auto msg = mqtt::make_message(topic, payload);
        msg->set_qos(MOSQUITTO_BROKER_QOS);
        client->publish(msg);
    }
    catch (std::exception& e) {
        std::stringstream ss;
        ss << "Publish to " << topic << " error: " << e.what() << std::endl;
        Utils::WriteLogInfo(LOG_ERR, ss.str());
        Disconnect();
    }
}
