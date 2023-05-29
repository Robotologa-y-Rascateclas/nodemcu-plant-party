#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <UniversalTelegramBot.h>
#include <time.h>

// Load config variables
#include <configNet.h>

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

const unsigned long BOT_MTBS = 1000; // mean time between scan messages
unsigned long bot_lasttime; // last time messages' scan has been done

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void initWifi() {
  // attempt to connect to Wifi network:
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
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

void bot_setup() {
  const String commands = F("["
                          "{\"command\":\"help\", \"description\":\"Bot help\"},"
                          "{\"command\":\"start\", \"description\":\"Start bot\"},"
                          "]");
  bot.setMyCommands(commands);
}

void sendSensorData(
  int sensor1value,
  int sensor2value,
  int sensor3value,
  int sensor4value,
  int sensor5value
) {
  // Send sensor data to server by WiFi
  if (WiFi.status() != WL_CONNECTED) {
      reconnectWifi();
  }

  // Create a JSON array to hold the sensors data
  StaticJsonDocument<500> doc;
  JsonArray sensors = doc.to<JsonArray>();

  // Create the first sensor object
  JsonObject sensor1 = sensors.createNestedObject();
  sensor1["sensor"] = "sensor_1";
  JsonObject data1 = sensor1.createNestedObject("data");
  data1["moisture"] = sensor1value;

  // Create the second sensor object
  JsonObject sensor2 = sensors.createNestedObject();
  sensor2["sensor"] = "sensor_2";
  JsonObject data2 = sensor2.createNestedObject("data");
  data2["moisture"] = sensor2value;

  // Create the third sensor object
  JsonObject sensor3 = sensors.createNestedObject();
  sensor3["sensor"] = "sensor_3";
  JsonObject data3 = sensor3.createNestedObject("data");
  data3["moisture"] = sensor3value;

  // Create the fourth sensor object
  JsonObject sensor4 = sensors.createNestedObject();
  sensor4["sensor"] = "sensor_4";
  JsonObject data4 = sensor4.createNestedObject("data");
  data4["moisture"] = sensor4value;

  // Create the fifth sensor object
  JsonObject sensor5 = sensors.createNestedObject();
  sensor5["sensor"] = "sensor_5";
  JsonObject data5 = sensor5.createNestedObject("data");
  data5["moisture"] = sensor5value;

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

void sendBotMessage(
  int sensorValue1,
  int sensorValue2,
  int sensorValue3,
  int sensorValue4,
  int sensorValue5
) {
  // Send sensors data to Telegram bot
  bot.sendMessage(CHAT_ID, "Sensors data measured: " +
    String(sensorValue1) + ", " +
    String(sensorValue2) + ", " +
    String(sensorValue3) + ", " +
    String(sensorValue4) + ", " +
    String(sensorValue5), "Markdown");
}

void sendSensorDataToInfluxDB(const std::vector<int>& sensorValues) {
  if (WiFi.status() != WL_CONNECTED) {
    reconnectWifi();
  }

  for (int i = 0; i < sensorValues.size(); ++i) {
    InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
    Point sensor("sensors_data");
    sensor.addTag("device", "ESP32");
    sensor.addTag("sensor", "sensor_" + String(i + 1));
    sensor.addField("moisture", sensorValues[i]);
    // sensor.setTime(influxdb::InfluxDBUtils::getEpochTime());
    if (!client.writePoint(sensor)) {
      Serial.print("InfluxDB sensor_");
      Serial.print(i + 1);
      Serial.print(" write failed: ");
      Serial.println(client.getLastErrorMessage());
    }
  }
}
