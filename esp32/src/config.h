// Capacitive Soil Moisture Sensor v1.2

// Sensor values -------------------
// 920 sensor seco
// 824-837 tierra recien regada
// 820 sensor en agua
// ----------------------------------

// Mapping to % of humidity
// const int DRY = 920; // This value will be 0% humidity
// const int WET = 820; // This value will be 100% humidity

// Used to calculate average of some measures
const int NUM_MEASURES = 10;

// const int RELAY_PIN = 3;

// Wait 10 minutes between measures and send data
const int DELAY_MEASURE_TIME = 600000;

// Time to deepSleep (max 71 minutes, 4260e6)
// 3600e6 = 1h | 1800e6 = 30min | 600e6 = 10min | 300e6 = 5min | 60e6 = 1min
const int DELAY_DEEPSLEEP_TIME = 900e6;

// Sensor pins
const int SENSOR_1_PIN = 36;
const int SENSOR_2_PIN = 39;
const int SENSOR_3_PIN = 34;
const int SENSOR_4_PIN = 35;
const int SENSOR_5_PIN = 32;
