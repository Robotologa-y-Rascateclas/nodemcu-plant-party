#include <Arduino.h>

// Capacitive Soil Moisture Sensor v1.2

// Sensor values -------------------
// 920 sensor seco
// 824-837 tierra recien regada
// 820 sensor en agua
// ----------------------------------

// Mapeo a % humedad
const int dry = 920; // Este valor se considerará 0% humedad
const int wet = 820; // Este valor se considerará 100% humedad

const int RELAY_PIN = 3;  

void setup() {
    pinMode(RELAY_PIN, OUTPUT); // initialize digital pin as an output.
    Serial.begin(9600);
}

void loop() {

    // Read sensor val
    int sensorVal_A0 = analogRead(A0);
    int sensorVal_A1 = analogRead(A1);

    // Calculate % humidity
    int percentageHumididy_A0 = map(sensorVal_A0, wet, dry, 100, 0); 
    int percentageHumididy_A1 = map(sensorVal_A1, wet, dry, 100, 0);

    // Activate or not the rele
    if ((percentageHumididy_A0 < 40) and (percentageHumididy_A0 < 40)) {
        digitalWrite(RELAY_PIN, LOW);
        // Serial.println("ON");
    }
    else{
        digitalWrite(RELAY_PIN, HIGH);
        //Serial.println("OFF");
    }
    
    // Print values
    String output_A0 = String(sensorVal_A0) + " (" + String(percentageHumididy_A0) + "%)";
    String output_A1 = String(sensorVal_A1) + " (" + String(percentageHumididy_A1) + "%)";
    String output = output_A0 + " - " + output_A1;
    Serial.println(output);
    delay(500);

}