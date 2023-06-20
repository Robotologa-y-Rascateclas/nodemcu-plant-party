#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

// Load config variables
#include <configNet.h>

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

void initWifi() {
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
  // configTime(1 * 3600, 0, "pool.ntp.org");      // get UTC time via NTP
  timeSync("UTC1", "pool.ntp.org", "time.nis.gov");
  while (!time(nullptr)) {
    delay(1000);
    Serial.println("Waiting for time synchronization...");
  }
}

void reconnectWifi() {
  Serial.print("Reconnecting...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected");
  while (!time(nullptr)) {
    delay(1000);
    Serial.println("Waiting for time synchronization...");
  }
}

void sendSensorData(const std::vector<int>& sensorValues) {
  // Send sensor data to server by WiFi
  if (WiFi.status() != WL_CONNECTED) {
      reconnectWifi();
  }

  // Create a JSON array to hold the sensors data
  StaticJsonDocument<500> doc;
  JsonArray sensors = doc.to<JsonArray>();

  // Iterate over the sensor values and create sensor objects
  for (int i = 0; i < sensorValues.size(); ++i) {
    JsonObject sensor = sensors.createNestedObject();
    sensor["sensor"] = "sensor_" + String(i + 1);
    JsonObject data = sensor.createNestedObject("data");
    data["moisture"] = sensorValues[i];
  }

  String payload;
  serializeJson(doc, payload);

  // Init HTTP client
  WiFiClient wifiClient;
  HTTPClient http;
  http.begin(wifiClient, SERVER_URL);
  http.addHeader("Content-Type", "application/json");

  // Send HTTP POST request
  int httpResponseCode = http.POST(payload);
  if (httpResponseCode == 200) {
    Serial.println("Sensor data sent");
  } else {
    Serial.print("Error sending sensor data: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
