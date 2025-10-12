#ifndef CONFIG_H
#define CONFIG_H

/*************** WiFi & Telegram ***************/
const char* WIFI_SSID     = "Sekawan 4G_EXT";
const char* WIFI_PASSWORD = "sekawan04";

const char* BOT_TOKEN = "6020037136:AAGGK5fgJ38zUhmBiP0-_Ltq8wtVzIqkp-Y";  // Ganti dengan token bot Anda
const int64_t CHAT_ID  = 127491210;  // Ganti dengan chat ID Anda (bisa didapat dari Telegram)

/*************** Sensor Pinout ***************/
// pH sensor (analog)
#define PH_PIN             34  // ADC1 (gunakan pin ADC, bukan ADC2)
// TDS sensor (analog)
#define TDS_PIN            35  // ADC1

// DS18B20 temperature sensor
#define ONE_WIRE_BUS       4   // Digital pin untuk OneWire (suhu air)

// Waterflow sensor
#define FLOW_SENSOR_PIN    5   // Gunakan pin interrupt (misalnya D5)

// Ultrasonic JSN-SR04
#define TRIG_PIN           19
#define ECHO_PIN           18

/*************** Pompa Peristaltik ***************/
#define POMPA_A_PIN        13
#define POMPA_B_PIN        14
#define PH_UP_PIN          27
#define PH_DOWN_PIN        26

// Pompa sirkulasi (bekerja nonstop, tapi tetap didefinisikan)
#define CIRCULATION_PUMP_PIN  26

/*************** Parameter Sistem ***************/
// Ambang batas TDS (PPM)
#define PPM_TARGET_LOW     250
#define PPM_TARGET_HIGH    500

// Ambang batas pH
#define PH_LOW             5.5
#define PH_HIGH            6.5

// Suhu minimum (°C)
#define MIN_TEMP_C         20.0

// Level air minimum (dalam cm)
#define MIN_WATER_LEVEL_CM  20.0

// Durasi pompa peristaltik ON (dalam ms)
#define DOSING_DURATION_MS  3000  // 3 detik per koreksi

/*************** LED Matrix P5 ***************/
// Ukuran panel P5 (misal 64x32)
#define PANEL_WIDTH        64
#define PANEL_HEIGHT       32
#define PANEL_CHAIN        1

/*************** Flag Debugging ***************/
#define DEBUG_SERIAL       true  // Tampilkan log ke Serial Monitor

#endif
