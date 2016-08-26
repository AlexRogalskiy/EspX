//
// Created by Andri Yadi on 7/30/16.
//

#ifndef XBOARD_IOTHUB_H
#define XBOARD_IOTHUB_H

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include <ArduinoJson.h>
#include <functional>
#include <vector>
#include <map>

#define IOTHUB_HOST "iothub.id"
#define IOTHUB_PORT 1883
#define IOTHUB_DEFAULT_SUBSCRIBE_QOS 2
#define IOTHUB_DEFAULT_PUBLISH_QOS   2

typedef std::function<void(String, String)> IoTHubSubscribedTopicMessageCallback;
typedef std::function<void(String, String)> IoTHubSubscribedPropertyCallback;
typedef std::map<String, IoTHubSubscribedTopicMessageCallback> TopicsHandlerMap_t;
typedef std::map<String, IoTHubSubscribedPropertyCallback> PropsHandlerMap_t;
typedef std::map<const char*, JsonVariant> JsonKeyValueMap;

class IoTHubClient : public AsyncMqttClient {
public:
    IoTHubClient(const char* username, const char* userkey, const char* projectName);
    IoTHubClient();
    ~IoTHubClient();

    AsyncMqttClient& onMessage(AsyncMqttClientInternals::OnMessageUserCallback callback);

    void subscribeWithCallback(String topic, IoTHubSubscribedTopicMessageCallback callback);
    void subscribeProperty(String property, IoTHubSubscribedPropertyCallback callback);

    void publish(String topic, String payload);
    void publishData(String payload);

//    template<typename Value>
//    void publishKeyValue(String key, Value val);

    void triggerIFTTTEvent(const char *iftttEvent, const char *iftttKey = NULL);
    void publishMap(JsonKeyValueMap keyValMap, const char *iftttEvent = NULL, const char *iftttKey = NULL);

    template<typename Value>
    void publishKeyValue(const char *key, Value val) {
        const int BUFFER_SIZE = JSON_OBJECT_SIZE(2);
        StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
        JsonObject &root = jsonBuffer.createObject();
        root[key] = val;

        String jsonStr;
        root.printTo(jsonStr);
        Serial.println(jsonStr);

        publishData(jsonStr);
    }

private:

    const char* username_ = NULL;
    const char* userkey_ = NULL;
    const char* projectName_ = NULL;

    boolean parseMessageAsJson_ = false;

//    std::vector<String> subscribedTopics_;
//    std::vector<IoTHubSubscribedTopicMessageCallback> subscribedCallbacks_;

    TopicsHandlerMap_t subscribedTopics_;
    PropsHandlerMap_t subscribedProperties_;

    AsyncMqttClientInternals::OnMessageUserCallback _onIotHubMessageUserCallback;

    void _onIoTHubMqttMessage(char* topic, char* payload, uint8_t qos, size_t len, size_t index, size_t total);
};


#endif //XBOARD_IOTHUB_H
