#include <Arduino.h>
#include <config.h>

void setup() {
  Serial.begin(9600);
  Serial.println();

  pinMode(SENSOR_PIN, INPUT);

}

void loop() {
  // Read the moisture level from the sensor
  int sensorValue = analogRead(SENSOR_PIN);
  Serial.println(sensorValue);

  // Wait for some time before sending the next data
  delay(5000);
}
