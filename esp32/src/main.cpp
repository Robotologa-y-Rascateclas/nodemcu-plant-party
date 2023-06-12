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
  int sensor1value = getSensorValue(SENSOR_1_PIN);
  int sensor2value = getSensorValue(SENSOR_2_PIN);
  int sensor3value = getSensorValue(SENSOR_3_PIN);
  int sensor4value = getSensorValue(SENSOR_4_PIN);
  int sensor5value = getSensorValue(SENSOR_5_PIN);

  // float moisture = map(sensorValue, 1023, 0, 0, 100) / 100.0;

  // Send sensors data to nodejs server by WiFi
  sendSensorData(
    sensor1value,
    sensor2value,
    sensor3value,
    sensor4value,
    sensor5value
  );

  sendBotMessage(
    sensor1value,
    sensor2value,
    sensor3value,
    sensor4value,
    sensor5value
  );

  // Send sensors data to InfluxDB
  std::vector<int> sensorValues = {
    sensor1value,
    sensor2value,
    sensor3value,
    sensor4value,
    sensor5value
  };
  sendSensorDataToInfluxDB(sensorValues);

  // Print payload in serial
  // Serial.println(payload);

  esp_sleep_enable_timer_wakeup(DELAY_DEEPSLEEP_TIME);

  esp_sleep_pd_config(ESP_PD_DOMAIN_MAX, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  esp_deep_sleep_start();

  // Wait for some time before sending the next data
  // delay(500);
}
