#include <Arduino.h>


const int RELAY_PIN = 3;  // the Arduino pin, which connects to the IN pin of relay

void setup() {
    
    // initialize digital pin as an output.
    pinMode(RELAY_PIN, OUTPUT);

    Serial.begin(9600);
}

void loop() {

    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("OFF");
    delay(1000);

    digitalWrite(RELAY_PIN, LOW);
    Serial.println("ON");
    delay(1000);

}