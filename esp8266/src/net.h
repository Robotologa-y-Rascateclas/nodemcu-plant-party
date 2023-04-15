#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <UniversalTelegramBot.h>
#include <time.h>

// Load config variables
#include <configNet.h>

const unsigned long BOT_MTBS = 1000; // mean time between scan messages
unsigned long bot_lasttime; // last time messages' scan has been done

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void initWifi() {
  // attempt to connect to Wifi network:
  configTime(1 * 3600, 0, "pool.ntp.org");      // get UTC time via NTP
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
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

void sendSensorData(String payload) {
  // Send sensor data to server by WiFi
  if (WiFi.status() != WL_CONNECTED) {
      reconnectWifi();
  }

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

void sendBotMessage(int sensorValue) {
  // Send sensor data to Telegram bot
  bot.sendMessage(CHAT_ID, "Sensor data measured: " + String(sensorValue), "Markdown");
}
