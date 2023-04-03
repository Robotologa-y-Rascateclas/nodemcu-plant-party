const char* WIFI_SSID = "your_SSID";
const char* WIFI_PASSWORD = "your_PASSWORD";
const char* SERVER_URL = "http://your_server_ip:3000/send-data";

// Capacitive Soil Moisture Sensor v1.2

// Sensor values -------------------
// 920 sensor seco
// 824-837 tierra recien regada
// 820 sensor en agua
// ----------------------------------

// Mapeo a % humedad
const int DRY = 920; // Este valor se considerará 0% humedad
const int WET = 820; // Este valor se considerará 100% humedad

const int SENSOR_PIN = A0;

const int RELAY_PIN = 3;
