#include "config.h"           // Konfigurasi pin & ambang batas
#include "sensors.h"          // Pembacaan sensor
#include "automation.h"       // Kontrol otomatis pompa
#include "connectWiFi.h"       // WiFi
// #include "display.h"          // Tampilan LED P5
#include "telegram_bot.h"     // Telegram bot

unsigned long lastSensorRead = 0;
unsigned long lastTelegramCheck = 0;
unsigned long lastWiFiCheck = 0;
const unsigned long sensorInterval = 5000;    // 5 detik
const unsigned long telegramInterval = 1000;  // 1 detik
const unsigned long wifiCheckInterval = 30000; // 30 detik

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  if (DEBUG_SERIAL) Serial.println("[INIT] Memulai sistem hidroponik...");
  
  // Setup sensor, tampilan, Telegram
  setupPins();             // Inisialisasi pin (pompa, sensor, dsb)
  setupSensors();          // Inisialisasi sensor (OneWire, dsb)
  // setupDisplay();          // Inisialisasi LED P5
  
  // Koneksi WiFi dengan timeout
  connectWiFiBlocking();   // Koneksi WiFi blocking di setup
  
  if (WiFi.status() == WL_CONNECTED) {
    setupTelegram();         // Telegram bot init hanya jika WiFi terhubung
  }
  // showStartupScreen();     // LED tampilkan status awal
  
  if (DEBUG_SERIAL) Serial.println("[INIT] Setup selesai!");
}

void loop() {
  // Cek koneksi WiFi secara berkala
  if (millis() - lastWiFiCheck >= wifiCheckInterval) {
    checkAndReconnectWiFi();
    lastWiFiCheck = millis();
  }

  // Hanya jalankan fungsi Telegram jika WiFi terhubung
  if (WiFi.status() == WL_CONNECTED) {
    checkTelegramMessages();
  }
  
  readAllSensors();
  // updateDisplay();
  processAutomation();
}
