#include <WiFi.h>
#include "config.h"

unsigned long wifiConnectStartTime = 0;   // Waktu mulai koneksi WiFi
unsigned long lastWiFiConnectAttempt = 0;  // Interval antar percobaan koneksi
const unsigned long wifiReconnectInterval = 10000; // 10 detik
const unsigned long wifiTimeoutDuration = 20000;   // 20 detik timeout

// Fungsi koneksi WiFi Blocking untuk setup
void connectWiFiBlocking() {
  if (DEBUG_SERIAL) {
    Serial.println("[WiFi] Mencoba koneksi ke: " + String(WIFI_SSID));
  }
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < wifiTimeoutDuration) {
    delay(500);
    if (DEBUG_SERIAL) Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    if (DEBUG_SERIAL) {
      Serial.println();
      Serial.println("[WiFi] Terhubung ke WiFi!");
      Serial.println("[WiFi] SSID: " + String(WIFI_SSID));
      Serial.println("[WiFi] IP: " + WiFi.localIP().toString());
      Serial.println("[WiFi] Signal: " + String(WiFi.RSSI()) + " dBm");
    }
  } else {
    if (DEBUG_SERIAL) {
      Serial.println();
      Serial.println("[WiFi] GAGAL terhubung ke WiFi!");
      Serial.println("[WiFi] Status: " + String(WiFi.status()));
    }
  }
}

// Fungsi koneksi WiFi Non-blocking untuk loop
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return; // Sudah terhubung
  }

  // Cek apakah sudah mencoba sebelumnya atau perlu koneksi ulang
  if (millis() - lastWiFiConnectAttempt > wifiReconnectInterval) {
    if (wifiConnectStartTime == 0) {
      wifiConnectStartTime = millis();  // Waktu mulai koneksi
      WiFi.mode(WIFI_STA);
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      if (DEBUG_SERIAL) Serial.println("[WiFi] Mencoba reconnect...");
    }

    // Cek apakah koneksi berhasil
    if (WiFi.status() == WL_CONNECTED) {
      if (DEBUG_SERIAL) {
        Serial.println("[WiFi] Reconnect berhasil!");
        Serial.println("[WiFi] IP: " + WiFi.localIP().toString());
      }
      wifiConnectStartTime = 0; // Reset timer
      lastWiFiConnectAttempt = millis();
    } else if (millis() - wifiConnectStartTime > wifiTimeoutDuration) {
      // Timeout, coba lagi
      if (DEBUG_SERIAL) {
        Serial.println("[WiFi] Timeout reconnect, akan coba lagi...");
        Serial.println("[WiFi] Status: " + String(WiFi.status()));
      }
      wifiConnectStartTime = 0; // Reset timer untuk percobaan berikutnya
      lastWiFiConnectAttempt = millis();
    }
  }
}

// Fungsi untuk cek dan reconnect WiFi
void checkAndReconnectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    if (DEBUG_SERIAL) Serial.println("[WiFi] Koneksi terputus, mencoba reconnect...");
    connectWiFi();
  } else {
    if (DEBUG_SERIAL) {
      Serial.println("[WiFi] Status: Terhubung");
      Serial.println("[WiFi] Signal: " + String(WiFi.RSSI()) + " dBm");
    }
  }
}

// Fungsi untuk mendapatkan status WiFi dalam bentuk string
String getWiFiStatusString() {
  switch (WiFi.status()) {
    case WL_CONNECTED: return "Terhubung";
    case WL_NO_SSID_AVAIL: return "SSID tidak ditemukan";
    case WL_CONNECT_FAILED: return "Koneksi gagal";
    case WL_CONNECTION_LOST: return "Koneksi terputus";
    case WL_DISCONNECTED: return "Terputus";
    case WL_IDLE_STATUS: return "Idle";
    default: return "Status tidak dikenal";
  }
}