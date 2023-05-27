#include <Arduino.h>
#include <ArduinoJson.h>
#include <net.h>

// Load config variables
#include <config.h>

void setup() {

  // Init Serial
  Serial.begin(115200);
  Serial.println();

  // Connect to Wi-Fi
  initWifi();

  // Init bot
  bot_setup();
}

void getTime() {
  time_t now = time(nullptr);
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  timeinfo.tm_hour += 2; // adjust for Madrid time (UTC+2)
  mktime(&timeinfo);

  // Format time string
  char timeStr[6];
  strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);

  // Print time string
  Serial.println(timeStr);
}

/**
 * Get the average of 5 measurements from a sensor
 */
int getSensorValue(int sensorPin) {
  int sensorValue = 0;
  for (int i = 0; i < NUM_MEASURES; i++) {
    sensorValue += analogRead(sensorPin);
    delay(500);
  }

  sensorValue = sensorValue / NUM_MEASURES;
  Serial.println(sensorValue);
  return sensorValue;
}

void loop() {
  // Read the moisture level from the sensors
  int sensor1value = getSensorValue(36);
  int sensor2value = getSensorValue(39);
  int sensor3value = getSensorValue(34);
  int sensor4value = getSensorValue(35);
  int sensor5value = getSensorValue(32);

  // float moisture = map(sensorValue, 1023, 0, 0, 100) / 100.0;

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

  // Send sensors data to server by WiFi
  sendSensorData(payload);

  sendBotMessage(sensor1value, sensor2value, sensor3value, sensor4value, sensor5value);

  // Print payload in serial
  // Serial.println(payload);

  esp_sleep_enable_timer_wakeup(DELAY_DEEPSLEEP_TIME);
  esp_deep_sleep_start();

  // Wait for some time before sending the next data
  // delay(500);
}
