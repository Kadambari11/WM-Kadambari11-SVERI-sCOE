#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "WIFI_NAME";
const char* password = "PASSWORD";
const char* mqttServer = "IOT_HUB_NAME.azure-devices.net";
const int mqttPort = 8883;
const char* deviceId = "EVICE_ID";
const char* mqttUser = "IOT_HUB_NAME.azure-devices.net/YOUR_DEVICE_ID/?api-version=2021-04-12";
const char* mqttPassword = "SAS_TOKEN";

String telemetryTopic = "devices/" + String(deviceId) + "/messages/events/";
WiFiClientSecure espClient;
PubSubClient client(espClient);
void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to Azure IoT Hub...");
    if (client.connect(deviceId, mqttUser, mqttPassword)) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.println(client.state());
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  espClient.setInsecure();   // For demo purposes
  client.setServer(mqttServer, mqttPort);
  connectMQTT();
}
void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();
  float fillLevel = 87.5;
  String payload = "{";
  payload += "\"deviceId\":\"" + String(deviceId) + "\",";
  payload += "\"fillLevel\":" + String(fillLevel);
  payload += "}";
  client.publish(telemetryTopic.c_str(), payload.c_str());
  Serial.println("Data sent to Azure IoT Hub:");
  Serial.println(payload);

  delay(10000);  // Send every 10 seconds
}
