#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <config.h>

HTTPClient http;

void setup() {
  Serial.begin(115200);

  // initialize digital pin as an output.
  pinMode(RELAY_PIN, OUTPUT);

  // Connect to Wi-Fi network
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize the HTTP client
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");
}

void loop() {
  // Read the moisture level from the sensor
  int sensorValue = analogRead(SENSOR_PIN);

  // Convert the sensor value to a moisture level
  float moisture = map(sensorValue, wet, dry, 100, 0);

  if (moisture < 40) {
    digitalWrite(RELAY_PIN, LOW);
  } else {
    digitalWrite(RELAY_PIN, HIGH);
  }

  StaticJsonDocument<200> doc;
  doc["sensor"] = "sensor1";
  JsonObject data = doc.createNestedObject("data");
  data["moisture"] = moisture;

  String payload;
  serializeJson(doc, payload);

  // Send the POST request
  int httpCode = http.POST(payload);
  if (httpCode == HTTP_CODE_OK) {
    Serial.println("Sensor data sent");
  } else {
    Serial.print("Error sending sensor data: ");
    Serial.println(httpCode);
  }

  // Disconnect from the server
  http.end();

  // Wait for some time before sending the next data
  delay(5000);
}
