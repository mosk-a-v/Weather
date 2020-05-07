#include "MqttConsumer.h"

MqttConsumer::MqttConsumer(std::string topic, std::string clientId) {
    this->topic = topic;
    this->clientId = clientId;
}
MqttConsumer::~MqttConsumer() {
    Disconnect();
}

void MqttConsumer::Connect() {
    if(client == nullptr || !client->is_connected()) {
        connOpts.set_keep_alive_interval(20);
        connOpts.set_clean_session(true);
        this->client = new mqtt::async_client(MOSQUITTO_BROKER_ADDRESS, clientId);
        client->connect(connOpts)->wait();
        client->start_consuming();
        client->subscribe(topic, MOSQUITTO_BROKER_QOS)->wait();
    }
}
void MqttConsumer::Disconnect() {
    if(client != nullptr && client->is_connected()) {
        try {
            client->unsubscribe(topic)->wait();
            client->stop_consuming();
            client->disconnect()->wait();
        } catch(std::exception &e) {
            std::stringstream ss;
            ss << "Disconnect from " << topic << " error: " << e.what() << std::endl;
            Utils::WriteLogInfo(LOG_ERR, ss.str());
        }
    }
    if(client != nullptr) {
        delete client;
        client = nullptr;
    }
}
std::string MqttConsumer::ReadMessage() {
    try {
        Connect();
        auto msg = client->consume_message();
        if(!msg) {
            Disconnect();
            return std::string();
        }
        return msg->get_payload_str();
    } catch(std::exception &e) {
        std::stringstream ss;
        ss << "Read " << topic << " response error: " << e.what() << std::endl;
        Utils::WriteLogInfo(LOG_ERR, ss.str());
        Disconnect();
    }
    return std::string();
}
