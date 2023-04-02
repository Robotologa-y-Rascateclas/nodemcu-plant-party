#include <Arduino.h>

// Capacitive Soil Moisture Sensor v1.2

// Sensor values -------------------
// 920 sensor seco
// 824-837 tierra recien regada
// 820 sensor en agua
// ----------------------------------

// Mapeo a % humedad
const int dry = 940; // Este valor se considerará 0% humedad
const int wet = 800; // Este valor se considerará 100% humedad

void setup() {
  Serial.begin(9600);
}

void loop() {

  // Read sensor val
  int sensorVal_A0 = analogRead(A0);
  int sensorVal_A1 = analogRead(A1);

  // Calculate % humidity
  int percentageHumididy_A0 = map(sensorVal_A0, wet, dry, 100, 0); 
  int percentageHumididy_A1 = map(sensorVal_A1, wet, dry, 100, 0);
 
  // Print values
  String output_A0 = String(sensorVal_A0) + " (" + String(percentageHumididy_A0) + "%)";
  String output_A1 = String(sensorVal_A1) + " (" + String(percentageHumididy_A1) + "%)";
  String output = output_A0 + " - " + output_A1;
  Serial.println(output);
  delay(500);

}