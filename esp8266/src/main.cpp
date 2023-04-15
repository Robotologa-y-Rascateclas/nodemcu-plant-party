#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <config.h>
#include <net.h>

void setup() {

  // Init Serial
  Serial.begin(115200);
  Serial.println();

  // Connect to Wi-Fi
  initWifi();

  // Init pins
  pinMode(SENSOR_PIN, INPUT);

}

void loop() {
  // Read the moisture level from the sensor
  int sensorValue = analogRead(SENSOR_PIN);
  Serial.println(sensorValue);

  // Send sensor data to server by WiFi
  // .....
  // .....

  // Wait for some time before sending the next data
  delay(5000);
}
