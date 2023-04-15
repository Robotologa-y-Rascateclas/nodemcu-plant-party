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

  // Init pins
  pinMode(SENSOR_PIN, INPUT);

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

void loop() {
  // Read the moisture level from the sensor
  int sensorValue = analogRead(SENSOR_PIN);
  Serial.println(sensorValue);

  // float moisture = map(sensorValue, 1023, 0, 0, 100) / 100.0;

  StaticJsonDocument<200> doc;
  doc["sensor"] = "sensor_poc";
  JsonObject data = doc.createNestedObject("data");
  data["moisture"] = sensorValue;

  String payload;
  serializeJson(doc, payload);

  // Send sensor data to server by WiFi
  sendSensorData(payload);

  sendBotMessage(sensorValue);

  // We will use getTime() to get the time from the NTP server
  // and only use the reles if the time is during the night

  ESP.deepSleep(DELAY_DEEPSLEEP_TIME);

  // Wait for some time before sending the next data
  // delay(DELAY_MEASURE_TIME);
}
