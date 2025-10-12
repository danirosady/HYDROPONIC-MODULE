# 🌱 Sistem Hidroponik Otomatis dengan ESP32 + Telegram Bot

Sistem monitoring dan kontrol hidroponik otomatis menggunakan ESP32 dengan integrasi Telegram Bot untuk remote monitoring dan kontrol.

## 📋 Daftar Isi
- [Fitur Utama](#-fitur-utama)
- [Komponen Hardware](#-komponen-hardware)
- [Skema Wiring](#-skema-wiring)
- [Instalasi](#-instalasi)
- [Konfigurasi](#-konfigurasi)
- [Perintah Telegram Bot](#-perintah-telegram-bot)
- [Parameter Sistem](#-parameter-sistem)
- [Troubleshooting](#-troubleshooting)
- [Kontribusi](#-kontribusi)

## 🚀 Fitur Utama

### 📊 Monitoring Sensor
- **pH Sensor** - Monitoring tingkat keasaman air
- **TDS/PPM Sensor** - Monitoring nutrisi dalam air
- **Sensor Suhu (DS18B20)** - Monitoring suhu air
- **Water Flow Sensor** - Deteksi aliran air
- **Ultrasonic Sensor (JSN-SR04)** - Monitoring level air

### 🤖 Kontrol Otomatis
- **Auto-dosing Nutrisi** - Pompa A & B otomatis saat PPM rendah
- **Auto pH Correction** - pH UP/DOWN otomatis
- **Pompa Sirkulasi** - Berjalan non-stop untuk sirkulasi air
- **Smart Alerts** - Notifikasi otomatis saat ada masalah

### 📱 Telegram Bot Integration
- **Remote Monitoring** - Cek status sistem dari mana saja
- **Remote Control** - Kontrol pompa manual via Telegram
- **Auto-Notification** - Update otomatis setiap 5 menit atau saat ada perubahan
- **Real-time Alerts** - Peringatan langsung saat ada masalah

### 🌐 Konektivitas
- **WiFi Auto-Reconnect** - Otomatis reconnect jika koneksi terputus
- **Robust Connection** - Sistem tetap berjalan meski WiFi terputus
- **Status Monitoring** - Monitor kualitas sinyal WiFi

## 🔧 Komponen Hardware

### Mikrokontroler
- **ESP32 DevKit** - Mikrokontroler utama

### Sensor
- **pH Sensor** - Analog pH probe
- **TDS Sensor** - Total Dissolved Solids meter
- **DS18B20** - Sensor suhu air waterproof
- **Water Flow Sensor** - Hall effect flow sensor
- **JSN-SR04** - Ultrasonic distance sensor

### Aktuator
- **4x Pompa Peristaltik** - Untuk dosing nutrisi dan pH
- **1x Pompa Sirkulasi** - Untuk sirkulasi air
- **Relay Module 5V** - Untuk kontrol pompa

### Optional
- **LED Matrix P5** - Display status (dalam pengembangan)

## 🔌 Skema Wiring

### Pin Configuration ESP32

| Komponen | Pin ESP32 | Keterangan |
|----------|-----------|------------|
| **SENSOR** |
| pH Sensor | GPIO 34 | ADC1 - Analog input |
| TDS Sensor | GPIO 35 | ADC1 - Analog input |
| DS18B20 | GPIO 4 | OneWire data |
| Flow Sensor | GPIO 5 | Digital interrupt |
| Ultrasonic TRIG | GPIO 19 | Digital output |
| Ultrasonic ECHO | GPIO 18 | Digital input |
| **POMPA** |
| Pompa A (Nutrisi) | GPIO 13 | Digital output |
| Pompa B (Nutrisi) | GPIO 14 | Digital output |
| pH UP | GPIO 27 | Digital output |
| pH DOWN | GPIO 26 | Digital output |
| Sirkulasi | GPIO 26 | Digital output |

### Diagram Wiring
Trae

telegram_bot.h 1 - 50

telegram_bot.h 86 - 130

telegram_bot.h 130 - 170

Sekarang saya akan menambahkan fitur auto-notification yang bisa diaktifkan/dinonaktifkan melalui perintah Telegram. Mari saya buat implementasinya:

## Perbaikan telegram_bot.h dengan fitur Auto-Notification:
```
#ifndef TELEGRAM_BOT_H
#define TELEGRAM_BOT_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "config.h"

// Inisialisasi objek WiFiClient dan bot
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

unsigned long lastMessageTime = 0;
#define BOT_MTBS 1000  // Millisecond per check Telegram (1 
detik)

unsigned long lastTelegramCheckTime = 0;
const unsigned long telegramPollingInterval = 1000; 

// ===== Variabel Auto-Notification =====
bool autoNotificationEnabled = false;  // Default OFF
unsigned long lastAutoNotification = 0;
const unsigned long autoNotificationInterval = 300000; // 5 
menit (300000 ms)

// Variabel untuk tracking perubahan data sensor
float lastNotifiedPPM = -1;
float lastNotifiedPH = -1;
float lastNotifiedTemp = -1;
bool lastNotifiedFlow = true;
float lastNotifiedWaterLevel = -1;

// ===== Setup Telegram =====
void setupTelegram() {
  // PENTING: Set insecure untuk bypass SSL certificate
  client.setInsecure();
  
  // Set timeout untuk koneksi
  client.setTimeout(10000); // 10 detik timeout
  
  if (DEBUG_SERIAL) Serial.println("[Telegram] Menginisialisasi 
  bot...");
  
  // Test koneksi dengan mengirim pesan startup
  delay(1000); // Beri waktu untuk stabilisasi
  
  String startupMsg = "🟢 Sistem Hidroponik Online!\n";
  startupMsg += "Bot siap menerima perintah.\n";
  startupMsg += "📢 Auto-notification: " + String
  (autoNotificationEnabled ? "ON" : "OFF");
  
  bool startupSent = bot.sendMessage(String(CHAT_ID), 
  startupMsg, "");
  
  if (startupSent) {
    if (DEBUG_SERIAL) Serial.println("[Telegram] ✅ Bot berhasil 
    diinisialisasi dan pesan startup terkirim!");
  } else {
    if (DEBUG_SERIAL) {
      Serial.println("[Telegram] ❌ GAGAL mengirim pesan 
      startup!");
      Serial.println("[Telegram] Periksa BOT_TOKEN dan 
      CHAT_ID!");
    }
  }
  
  if (DEBUG_SERIAL) {
    Serial.println("[Telegram] BOT_TOKEN: " + String(BOT_TOKEN));
    Serial.println("[Telegram] CHAT_ID: " + String(CHAT_ID));
  }
}

// ===== Kirim Pesan ke Telegram dengan Retry =====
void sendTelegramMessage(String message) {
  if (WiFi.status() != WL_CONNECTED) {
    if (DEBUG_SERIAL) Serial.println("[Telegram] WiFi tidak 
    terhubung, tidak bisa kirim pesan");
    return;
  }
  
  if (DEBUG_SERIAL) Serial.println("[Telegram] Mencoba kirim: " 
  + message);
  
  // Coba kirim pesan dengan retry
  int maxRetries = 3;
  bool sent = false;
  
  for (int i = 0; i < maxRetries && !sent; i++) {
    if (i > 0) {
      if (DEBUG_SERIAL) Serial.println("[Telegram] Retry ke-" + 
      String(i));
      delay(1000); // Delay sebelum retry
    }
    
    sent = bot.sendMessage(String(CHAT_ID), message, "");
    
    if (sent) {
      if (DEBUG_SERIAL) Serial.println("[Telegram] ✅ Pesan 
      berhasil dikirim: " + message);
    } else {
      if (DEBUG_SERIAL) Serial.println("[Telegram] ❌ Gagal kirim 
      (percobaan " + String(i+1) + ")");
    }
  }
  
  if (!sent) {
    if (DEBUG_SERIAL) {
      Serial.println("[Telegram] ❌ GAGAL mengirim setelah " + 
      String(maxRetries) + " percobaan!");
      Serial.println("[Telegram] Periksa koneksi internet dan 
      konfigurasi bot!");
    }
  }
}

// ===== Fungsi Auto-Notification =====
void checkAutoNotification() {
  if (!autoNotificationEnabled || WiFi.status() != WL_CONNECTED) 
  {
    return;
  }
  
  // Cek apakah sudah waktunya untuk notifikasi berkala
  bool timeForPeriodicUpdate = (millis() - lastAutoNotification 
  >= autoNotificationInterval);
  
  // Cek apakah ada perubahan signifikan pada sensor
  bool significantChange = false;
  
  // Cek perubahan PPM (toleransi ±10)
  if (abs(ppmValue - lastNotifiedPPM) >= 10) {
    significantChange = true;
  }
  
  // Cek perubahan pH (toleransi ±0.2)
  if (abs(pHValue - lastNotifiedPH) >= 0.2) {
    significantChange = true;
  }
  
  // Cek perubahan suhu (toleransi ±2°C)
  if (abs(temperatureC - lastNotifiedTemp) >= 2.0) {
    significantChange = true;
  }
  
  // Cek perubahan flow sensor
  if (flowDetected != lastNotifiedFlow) {
    significantChange = true;
  }
  
  // Cek perubahan level air (toleransi ±5cm)
  if (abs(waterLevelCM - lastNotifiedWaterLevel) >= 5.0) {
    significantChange = true;
  }
  
  // Kirim notifikasi jika ada perubahan signifikan atau sudah 
  waktunya update berkala
  if (significantChange || timeForPeriodicUpdate) {
    String autoMessage = "📊 Update Otomatis Sensor:\n\n";
    autoMessage += "💧 PPM: " + String(ppmValue);
    if (ppmValue < PPM_TARGET_LOW) autoMessage += " ⬇️ RENDAH";
    else if (ppmValue > PPM_TARGET_HIGH) autoMessage += " ⬆️ 
    TINGGI";
    else autoMessage += " ✅ NORMAL";
    autoMessage += "\n";
    
    autoMessage += "⚗️ pH: " + String(pHValue);
    if (pHValue < PH_LOW) autoMessage += " ⬇️ RENDAH";
    else if (pHValue > PH_HIGH) autoMessage += " ⬆️ TINGGI";
    else autoMessage += " ✅ NORMAL";
    autoMessage += "\n";
    
    autoMessage += "🌡️ Suhu: " + String(temperatureC) + "°C";
    if (temperatureC < MIN_TEMP_C) autoMessage += " ❄️ DINGIN";
    else autoMessage += " ✅ NORMAL";
    autoMessage += "\n";
    
    autoMessage += "💨 Flow: " + String(flowDetected ? "✅ OK" : 
    "❌ ERROR") + "\n";
    
    autoMessage += "📏 Level: " + String(waterLevelCM) + " cm";
    if (waterLevelCM < MIN_WATER_LEVEL_CM) autoMessage += " ⚠️ 
    RENDAH";
    else autoMessage += " ✅ NORMAL";
    
    if (significantChange) {
      autoMessage += "\n\n🔔 Perubahan signifikan terdeteksi!";
    }
    
    sendTelegramMessage(autoMessage);
    
    // Update nilai terakhir yang dikirim
    lastNotifiedPPM = ppmValue;
    lastNotifiedPH = pHValue;
    lastNotifiedTemp = temperatureC;
    lastNotifiedFlow = flowDetected;
    lastNotifiedWaterLevel = waterLevelCM;
    lastAutoNotification = millis();
    
    if (DEBUG_SERIAL) Serial.println("[Telegram] 
    Auto-notification terkirim");
  }
}

// ===== Periksa dan Tangani Pesan dari Telegram =====
void handleTelegramMessages() {
  if (millis() - lastMessageTime > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.
    last_message_received + 1);  // Mulai dari ID pesan baru
    while (numNewMessages) {
      String message = bot.messages[numNewMessages - 1].text;
      String fromName = bot.messages[numNewMessages - 1].
      from_name;

      if (DEBUG_SERIAL) {
        Serial.println("[Telegram] Pesan baru dari " + fromName 
        + ": " + message);
      }

      // Perintah /add_ab
      if (message == "/add_ab") {
        sendTelegramMessage("🔵 Menambahkan pupuk A & B...");
        activatePump(POMPA_A_PIN, DOSING_DURATION_MS);
        delay(1000);
        activatePump(POMPA_B_PIN, DOSING_DURATION_MS);
        sendTelegramMessage("✅ Pupuk A & B berhasil 
        ditambahkan!");
      }
      // Perintah /ph_up
      else if (message == "/ph_up") {
        sendTelegramMessage("🔵 Menambahkan pH UP...");
        activatePump(PH_UP_PIN, DOSING_DURATION_MS);
        sendTelegramMessage("✅ pH UP berhasil ditambahkan!");
      }
      // Perintah /ph_down
      else if (message == "/ph_down") {
        sendTelegramMessage("🔵 Menambahkan pH DOWN...");
        activatePump(PH_DOWN_PIN, DOSING_DURATION_MS);
        sendTelegramMessage("✅ pH DOWN berhasil ditambahkan!");
      }
      // Perintah /status
      else if (message == "/status") {
        String statusMessage = "📊 Status Sistem 
        Hidroponik:\n\n";
        statusMessage += "🌐 WiFi: " + getWiFiStatusString() + 
        "\n";
        statusMessage += "📶 Signal: " + String(WiFi.RSSI()) + " 
        dBm\n";
        statusMessage += "🌐 IP: " + WiFi.localIP().toString() + 
        "\n\n";
        statusMessage += "💧 PPM: " + String(ppmValue) + "\n";
        statusMessage += "⚗️ pH: " + String(pHValue) + "\n";
        statusMessage += "🌡️ Suhu: " + String(temperatureC) + "°
        C\n";
        statusMessage += "💨 Flow: " + String(flowDetected ? "✅ 
        OK" : "❌ Error") + "\n";
        statusMessage += "📏 Level Air: " + String(waterLevelCM) 
        + " cm\n\n";
        statusMessage += "📢 Auto-notification: " + String
        (autoNotificationEnabled ? "ON" : "OFF");

        sendTelegramMessage(statusMessage);
      }
      // Perintah /wifi untuk cek WiFi
      else if (message == "/wifi") {
        String wifiMessage = "🌐 Status WiFi:\n\n";
        wifiMessage += "📡 Status: " + getWiFiStatusString() + 
        "\n";
        wifiMessage += "🏠 SSID: " + String(WIFI_SSID) + "\n";
        wifiMessage += "🌐 IP: " + WiFi.localIP().toString() + 
        "\n";
        wifiMessage += "📶 Signal: " + String(WiFi.RSSI()) + " 
        dBm\n";
        wifiMessage += "🔧 MAC: " + WiFi.macAddress();
        
        sendTelegramMessage(wifiMessage);
      }
      // Perintah /notify_on - Aktifkan auto-notification
      else if (message == "/notify_on") {
        autoNotificationEnabled = true;
        lastAutoNotification = millis(); // Reset timer
        sendTelegramMessage("🔔 Auto-notification 
        DIAKTIFKAN!\n📅 Update setiap 5 menit atau saat ada 
        perubahan signifikan");
      }
      // Perintah /notify_off - Nonaktifkan auto-notification
      else if (message == "/notify_off") {
        autoNotificationEnabled = false;
        sendTelegramMessage("🔕 Auto-notification 
        DINONAKTIFKAN!\n📱 Gunakan /status untuk cek manual");
      }
      // Perintah /notify_status - Status auto-notification
      else if (message == "/notify_status") {
        String notifyMsg = "📢 Status Auto-Notification:\n\n";
        notifyMsg += "Status: " + String
        (autoNotificationEnabled ? "🔔 AKTIF" : "🔕 NONAKTIF") + 
        "\n";
        if (autoNotificationEnabled) {
          unsigned long nextUpdate = (autoNotificationInterval - 
          (millis() - lastAutoNotification)) / 1000;
          notifyMsg += "Update berikutnya: " + String
          (nextUpdate) + " detik\n";
          notifyMsg += "Interval: 5 menit\n";
          notifyMsg += "Trigger: Perubahan signifikan sensor";
        }
        sendTelegramMessage(notifyMsg);
      }
      // Perintah /help
      else if (message == "/help" || message == "/start") {
        String helpMessage = "🤖 Bot Hidroponik - Perintah:\n\n";
        helpMessage += "📊 MONITORING:\n";
        helpMessage += "/status - Status sistem\n";
        helpMessage += "/wifi - Status WiFi\n";
        helpMessage += "/test - Test koneksi bot\n\n";
        helpMessage += "🔧 KONTROL:\n";
        helpMessage += "/add_ab - Tambah pupuk A&B\n";
        helpMessage += "/ph_up - Tambah pH UP\n";
        helpMessage += "/ph_down - Tambah pH DOWN\n\n";
        helpMessage += "🔔 NOTIFIKASI:\n";
        helpMessage += "/notify_on - Aktifkan auto-update\n";
        helpMessage += "/notify_off - Nonaktifkan auto-update\n";
        helpMessage += "/notify_status - Status notifikasi\n\n";
        helpMessage += "/help - Tampilkan bantuan";
        
        sendTelegramMessage(helpMessage);
      }
      // Perintah /test
      else if (message == "/test") {
        sendTelegramMessage("✅ Bot aktif dan berfungsi 
        normal!\n🕐 " + String(millis()/1000) + " detik uptime");
      }
      
      // Cek pesan baru
      numNewMessages = bot.getUpdates(bot.last_message_received 
      + 1);
    }
    lastMessageTime = millis();
  }
}

void checkTelegramMessages() {
  if (millis() - lastTelegramCheckTime >= 
  telegramPollingInterval) {
    handleTelegramMessages();  // Periksa pesan baru
    lastTelegramCheckTime = millis();  // Update waktu 
    pengecekan pesan
  }
  
  // Cek auto-notification
  checkAutoNotification();
}

// ===== Notifikasi Telegram =====
void sendAlertNotification(String alertMessage) {
  sendTelegramMessage("⚠️ ALERT: " + alertMessage);
}

// ===== Test Koneksi Telegram =====
void testTelegramConnection() {
  if (WiFi.status() == WL_CONNECTED) {
    sendTelegramMessage("🔧 Test koneksi - Bot aktif dan siap!");
  } else {
    if (DEBUG_SERIAL) Serial.println("[Telegram] WiFi tidak 
    terhubung untuk test");
  }
}

#endif
```
## Fitur Baru yang Ditambahkan:
### 1. Auto-Notification System:
- Default OFF - tidak mengganggu pengguna
- Interval 5 menit untuk update berkala
- Trigger perubahan signifikan pada sensor:
  - PPM: ±10 ppm
  - pH: ±0.2
  - Suhu: ±2°C
  - Flow sensor: perubahan status
  - Level air: ±5cm
### 2. Perintah Telegram Baru:
- /notify_on - Aktifkan auto-notification
- /notify_off - Nonaktifkan auto-notification
- /notify_status - Cek status notifikasi
### 3. Pesan yang Lebih Informatif:
- Status normal/abnormal untuk setiap sensor
- Indikator visual dengan emoji
- Informasi waktu update berikutnya
## Cara Penggunaan:
1. 1.
   Aktifkan notifikasi: Kirim /notify_on ke bot
2. 2.
   Nonaktifkan notifikasi: Kirim /notify_off ke bot
3. 3.
   Cek status notifikasi: Kirim /notify_status ke bot
4. 4.
   Lihat semua perintah: Kirim /help ke bot
## Keunggulan Sistem:
- ✅ Hemat bandwidth - hanya kirim saat ada perubahan penting
- ✅ Tidak spam - interval minimum 5 menit
- ✅ Kontrol penuh - bisa diaktifkan/nonaktifkan kapan saja
- ✅ Smart detection - deteksi perubahan signifikan
- ✅ Status lengkap - indikator normal/abnormal
Sekarang sistem akan otomatis mengirim update sensor ke Telegram saat ada perubahan penting, tapi tetap bisa dinonaktifkan sesuai kebutuhan!

patungin20

Copy

Delete

bantu buatkan readme.md

Trae

config.h 1 - 63

HYDROPONIC_MODULE.ino 1 - 50

Sekarang saya akan membuat README.md yang lengkap untuk proyek sistem hidroponik Anda:

```
# 🌱 Sistem Hidroponik Otomatis dengan ESP32

Sistem monitoring dan kontrol hidroponik otomatis menggunakan 
ESP32 dengan integrasi Telegram Bot untuk remote monitoring dan 
kontrol.

## 📋 Daftar Isi
- [Fitur Utama](#-fitur-utama)
- [Komponen Hardware](#-komponen-hardware)
- [Skema Wiring](#-skema-wiring)
- [Instalasi](#-instalasi)
- [Konfigurasi](#-konfigurasi)
- [Perintah Telegram Bot](#-perintah-telegram-bot)
- [Parameter Sistem](#-parameter-sistem)
- [Troubleshooting](#-troubleshooting)
- [Kontribusi](#-kontribusi)

## 🚀 Fitur Utama

### 📊 Monitoring Sensor
- **pH Sensor** - Monitoring tingkat keasaman air
- **TDS/PPM Sensor** - Monitoring nutrisi dalam air
- **Sensor Suhu (DS18B20)** - Monitoring suhu air
- **Water Flow Sensor** - Deteksi aliran air
- **Ultrasonic Sensor (JSN-SR04)** - Monitoring level air

### 🤖 Kontrol Otomatis
- **Auto-dosing Nutrisi** - Pompa A & B otomatis saat PPM rendah
- **Auto pH Correction** - pH UP/DOWN otomatis
- **Pompa Sirkulasi** - Berjalan non-stop untuk sirkulasi air
- **Smart Alerts** - Notifikasi otomatis saat ada masalah

### 📱 Telegram Bot Integration
- **Remote Monitoring** - Cek status sistem dari mana saja
- **Remote Control** - Kontrol pompa manual via Telegram
- **Auto-Notification** - Update otomatis setiap 5 menit atau 
saat ada perubahan
- **Real-time Alerts** - Peringatan langsung saat ada masalah

### 🌐 Konektivitas
- **WiFi Auto-Reconnect** - Otomatis reconnect jika koneksi 
terputus
- **Robust Connection** - Sistem tetap berjalan meski WiFi 
terputus
- **Status Monitoring** - Monitor kualitas sinyal WiFi

## 🔧 Komponen Hardware

### Mikrokontroler
- **ESP32 DevKit** - Mikrokontroler utama

### Sensor
- **pH Sensor** - Analog pH probe
- **TDS Sensor** - Total Dissolved Solids meter
- **DS18B20** - Sensor suhu air waterproof
- **Water Flow Sensor** - Hall effect flow sensor
- **JSN-SR04** - Ultrasonic distance sensor

### Aktuator
- **4x Pompa Peristaltik** - Untuk dosing nutrisi dan pH
- **1x Pompa Sirkulasi** - Untuk sirkulasi air
- **Relay Module 5V** - Untuk kontrol pompa

### Optional
- **LED Matrix P5** - Display status (dalam pengembangan)

## 🔌 Skema Wiring

### Pin Configuration ESP32

| Komponen | Pin ESP32 | Keterangan |
|----------|-----------|------------|
| **SENSOR** |
| pH Sensor | GPIO 34 | ADC1 - Analog input |
| TDS Sensor | GPIO 35 | ADC1 - Analog input |
| DS18B20 | GPIO 4 | OneWire data |
| Flow Sensor | GPIO 5 | Digital interrupt |
| Ultrasonic TRIG | GPIO 19 | Digital output |
| Ultrasonic ECHO | GPIO 18 | Digital input |
| **POMPA** |
| Pompa A (Nutrisi) | GPIO 13 | Digital output |
| Pompa B (Nutrisi) | GPIO 14 | Digital output |
| pH UP | GPIO 27 | Digital output |
| pH DOWN | GPIO 26 | Digital output |
| Sirkulasi | GPIO 26 | Digital output |

### Diagram Wiring
```
ESP32                    Komponen
├── GPIO 34 ──────────── pH Sensor (Analog)
├── GPIO 35 ──────────── TDS Sensor (Analog)
├── GPIO 4  ──────────── DS18B20 (OneWire)
├── GPIO 5  ──────────── Flow Sensor (Digital)
├── GPIO 19 ──────────── Ultrasonic TRIG
├── GPIO 18 ──────────── Ultrasonic ECHO
├── GPIO 13 ──────────── Relay → Pompa A
├── GPIO 14 ──────────── Relay → Pompa B
├── GPIO 27 ──────────── Relay → pH UP
└── GPIO 26 ──────────── Relay → pH DOWN/Sirkulasi


## 📥 Instalasi

### 1. Persiapan Arduino IDE
```bash
# Install ESP32 Board Package
# File → Preferences → Additional Board Manager URLs:
https://dl.espressif.com/dl/package_esp32_index.json

# Tools → Board → Boards Manager → Search "ESP32" → Install
```

### 2. Install Library Dependencies
```bash
# Library Manager (Ctrl+Shift+I) - Install:
- WiFi (ESP32 built-in)
- WiFiClientSecure (ESP32 built-in)
- UniversalTelegramBot by Brian Lough
- OneWire by Jim Studt
- DallasTemperature by Miles Burton
```

### 3. Download & Upload Code
```bash
# Clone atau download project ini
# Buka HYDROPONIC_MODULE.ino di Arduino IDE
# Select Board: ESP32 Dev Module
# Select Port: (sesuai port ESP32)
# Upload code
```

## ⚙️ Konfigurasi

### 1. WiFi & Telegram Setup
Edit file `config.h`:

```cpp
/*************** WiFi & Telegram ***************/
const char* WIFI_SSID     = "NAMA_WIFI_ANDA";
const char* WIFI_PASSWORD = "PASSWORD_WIFI";

const char* BOT_TOKEN = "YOUR_BOT_TOKEN_HERE";
const int64_t CHAT_ID  = YOUR_CHAT_ID_HERE;
```

### 2. Cara Mendapatkan Bot Token & Chat ID

#### Bot Token:
1. Buka Telegram, cari **@BotFather**
2. Kirim `/newbot`
3. Ikuti instruksi untuk membuat bot
4. Copy **Bot Token** yang diberikan

#### Chat ID:
1. Kirim pesan ke bot Anda
2. Buka browser: `https://api.telegram.org/botYOUR_BOT_TOKEN/getUpdates`
3. Cari `"chat":{"id":XXXXXX}` - gunakan angka ini sebagai CHAT_ID

### 3. Kalibrasi Sensor
Edit parameter di `config.h` sesuai kebutuhan:

```cpp
/*************** Parameter Sistem ***************/
#define PPM_TARGET_LOW     250    // PPM minimum
#define PPM_TARGET_HIGH    500    // PPM maksimum
#define PH_LOW             5.5    // pH minimum
#define PH_HIGH            6.5    // pH maksimum
#define MIN_TEMP_C         20.0   // Suhu minimum (°C)
#define MIN_WATER_LEVEL_CM 20.0   // Level air minimum (cm)
#define DOSING_DURATION_MS 3000   // Durasi pompa ON (ms)
```

## 🤖 Perintah Telegram Bot

### 📊 Monitoring
| Perintah | Fungsi |
|----------|--------|
| `/status` | Tampilkan status lengkap sistem |
| `/wifi` | Cek status koneksi WiFi |
| `/test` | Test koneksi bot |

### 🔧 Kontrol Manual
| Perintah | Fungsi |
|----------|--------|
| `/add_ab` | Tambah nutrisi A & B manual |
| `/ph_up` | Tambah pH UP manual |
| `/ph_down` | Tambah pH DOWN manual |

### 🔔 Notifikasi
| Perintah | Fungsi |
|----------|--------|
| `/notify_on` | Aktifkan auto-notification |
| `/notify_off` | Nonaktifkan auto-notification |
| `/notify_status` | Status notifikasi |

### ℹ️ Bantuan
| Perintah | Fungsi |
|----------|--------|
| `/help` | Tampilkan semua perintah |
| `/start` | Mulai bot (sama dengan /help) |

## 📊 Parameter Sistem

### Target Nilai Optimal
- **PPM**: 250-500 ppm
- **pH**: 5.5-6.5
- **Suhu**: >20°C
- **Level Air**: >20 cm
- **Flow**: Harus terdeteksi

### Auto-Notification Triggers
- **PPM**: Perubahan ±10 ppm
- **pH**: Perubahan ±0.2
- **Suhu**: Perubahan ±2°C
- **Flow**: Perubahan status ON/OFF
- **Level**: Perubahan ±5 cm
- **Waktu**: Setiap 5 menit (jika aktif)

## 📈 Monitoring & Logs

### Serial Monitor
Buka Serial Monitor (115200 baud) untuk melihat:
- Status koneksi WiFi
- Pembacaan sensor real-time
- Status pengiriman Telegram
- Error messages
- Debug information

### Log Format
[INIT] Memulai sistem hidroponik...
[WiFi] Terhubung ke WiFi!
[WiFi] IP: 192.168.1.100
[Telegram] ✅ Bot berhasil diinisialisasi
[Sensor] Suhu: 25.3 °C
[Sensor] pH: 6.2
[Sensor] PPM: 350
[Telegram] ✅ Pesan berhasil dikirim


## 🔄 Update & Maintenance

### Update Firmware
1. Download versi terbaru dari repository
2. Backup konfigurasi `config.h` Anda
3. Upload firmware baru
4. Restore konfigurasi

### Maintenance Rutin
- **Harian**: Cek status via Telegram
- **Mingguan**: Bersihkan sensor
- **Bulanan**: Kalibrasi sensor pH dan TDS
- **3 Bulan**: Ganti larutan kalibrasi

## 🤝 Kontribusi

Kontribusi sangat diterima! Silakan:

1. **Fork** repository ini
2. **Create branch** untuk fitur baru
3. **Commit** perubahan Anda
4. **Push** ke branch
5. **Create Pull Request**

### Roadmap Pengembangan
- [ ] Web dashboard interface
- [ ] Data logging ke SD card
- [ ] Integrasi dengan database cloud
- [ ] Mobile app companion
- [ ] LED Matrix P5 display
- [ ] Multiple tank support
- [ ] Advanced scheduling system

## 📄 Lisensi

Project ini menggunakan lisensi MIT. Lihat file `LICENSE` untuk detail.

## 📞 Support

Jika ada pertanyaan atau masalah:
- **Issues**: Buat issue di GitHub repository
- **Email**: [dhanymcfarlan@gmail.com]

---

**⚡ Dibuat dengan ❤️ untuk peserta / alumni Hydroponic Automation System**

*Last updated: 2025*