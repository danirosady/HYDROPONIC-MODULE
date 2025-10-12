#ifndef TELEGRAM_BOT_H
#define TELEGRAM_BOT_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "config.h"

// Inisialisasi objek WiFiClient dan bot
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

unsigned long lastMessageTime = 0;
#define BOT_MTBS 1000  // Millisecond per check Telegram (1 detik)

unsigned long lastTelegramCheckTime = 0;
const unsigned long telegramPollingInterval = 1000; 

// ===== Variabel Auto-Notification =====
bool autoNotificationEnabled = false;  // Default OFF
unsigned long lastAutoNotification = 0;
const unsigned long autoNotificationInterval = 300000; // 5 menit (300000 ms)

// Variabel untuk tracking perubahan data sensor
float lastNotifiedPPM = -1;
float lastNotifiedPH = -1;
float lastNotifiedTemp = -1;
bool lastNotifiedFlow = true;
float lastNotifiedWaterLevel = -1;

// ===== Setup Telegram =====
void setupTelegram() {
  // PENTING: Set insecure untuk bypass SSL certificate
  client.setInsecure();
  
  // Set timeout untuk koneksi
  client.setTimeout(10000); // 10 detik timeout
  
  if (DEBUG_SERIAL) Serial.println("[Telegram] Menginisialisasi bot...");
  
  // Test koneksi dengan mengirim pesan startup
  delay(1000); // Beri waktu untuk stabilisasi
  
  String startupMsg = "🟢 Sistem Hidroponik Online!\n";
  startupMsg += "Bot siap menerima perintah.\n";
  startupMsg += "📢 Auto-notification: " + String(autoNotificationEnabled ? "ON" : "OFF");
  
  bool startupSent = bot.sendMessage(String(CHAT_ID), startupMsg, "");
  
  if (startupSent) {
    if (DEBUG_SERIAL) Serial.println("[Telegram] ✅ Bot berhasil diinisialisasi dan pesan startup terkirim!");
  } else {
    if (DEBUG_SERIAL) {
      Serial.println("[Telegram] ❌ GAGAL mengirim pesan startup!");
      Serial.println("[Telegram] Periksa BOT_TOKEN dan CHAT_ID!");
    }
  }
  
  if (DEBUG_SERIAL) {
    Serial.println("[Telegram] BOT_TOKEN: " + String(BOT_TOKEN));
    Serial.println("[Telegram] CHAT_ID: " + String(CHAT_ID));
  }
}

// ===== Kirim Pesan ke Telegram dengan Retry =====
void sendTelegramMessage(String message) {
  if (WiFi.status() != WL_CONNECTED) {
    if (DEBUG_SERIAL) Serial.println("[Telegram] WiFi tidak terhubung, tidak bisa kirim pesan");
    return;
  }
  
  if (DEBUG_SERIAL) Serial.println("[Telegram] Mencoba kirim: " + message);
  
  // Coba kirim pesan dengan retry
  int maxRetries = 3;
  bool sent = false;
  
  for (int i = 0; i < maxRetries && !sent; i++) {
    if (i > 0) {
      if (DEBUG_SERIAL) Serial.println("[Telegram] Retry ke-" + String(i));
      delay(1000); // Delay sebelum retry
    }
    
    sent = bot.sendMessage(String(CHAT_ID), message, "");
    
    if (sent) {
      if (DEBUG_SERIAL) Serial.println("[Telegram] ✅ Pesan berhasil dikirim: " + message);
    } else {
      if (DEBUG_SERIAL) Serial.println("[Telegram] ❌ Gagal kirim (percobaan " + String(i+1) + ")");
    }
  }
  
  if (!sent) {
    if (DEBUG_SERIAL) {
      Serial.println("[Telegram] ❌ GAGAL mengirim setelah " + String(maxRetries) + " percobaan!");
      Serial.println("[Telegram] Periksa koneksi internet dan konfigurasi bot!");
    }
  }
}

// ===== Fungsi Auto-Notification =====
void checkAutoNotification() {
  if (!autoNotificationEnabled || WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  // Cek apakah sudah waktunya untuk notifikasi berkala
  bool timeForPeriodicUpdate = (millis() - lastAutoNotification >= autoNotificationInterval);
  
  // Cek apakah ada perubahan signifikan pada sensor
  bool significantChange = false;
  
  // Cek perubahan PPM (toleransi ±10)
  if (abs(ppmValue - lastNotifiedPPM) >= 10) {
    significantChange = true;
  }
  
  // Cek perubahan pH (toleransi ±0.2)
  if (abs(pHValue - lastNotifiedPH) >= 0.2) {
    significantChange = true;
  }
  
  // Cek perubahan suhu (toleransi ±2°C)
  if (abs(temperatureC - lastNotifiedTemp) >= 2.0) {
    significantChange = true;
  }
  
  // Cek perubahan flow sensor
  if (flowDetected != lastNotifiedFlow) {
    significantChange = true;
  }
  
  // Cek perubahan level air (toleransi ±5cm)
  if (abs(waterLevelCM - lastNotifiedWaterLevel) >= 5.0) {
    significantChange = true;
  }
  
  // Kirim notifikasi jika ada perubahan signifikan atau sudah waktunya update berkala
  if (significantChange || timeForPeriodicUpdate) {
    String autoMessage = "📊 Update Otomatis Sensor:\n\n";
    autoMessage += "💧 PPM: " + String(ppmValue);
    if (ppmValue < PPM_TARGET_LOW) autoMessage += " ⬇️ RENDAH";
    else if (ppmValue > PPM_TARGET_HIGH) autoMessage += " ⬆️ TINGGI";
    else autoMessage += " ✅ NORMAL";
    autoMessage += "\n";
    
    autoMessage += "⚗️ pH: " + String(pHValue);
    if (pHValue < PH_LOW) autoMessage += " ⬇️ RENDAH";
    else if (pHValue > PH_HIGH) autoMessage += " ⬆️ TINGGI";
    else autoMessage += " ✅ NORMAL";
    autoMessage += "\n";
    
    autoMessage += "🌡️ Suhu: " + String(temperatureC) + "°C";
    if (temperatureC < MIN_TEMP_C) autoMessage += " ❄️ DINGIN";
    else autoMessage += " ✅ NORMAL";
    autoMessage += "\n";
    
    autoMessage += "💨 Flow: " + String(flowDetected ? "✅ OK" : "❌ ERROR") + "\n";
    
    autoMessage += "📏 Level: " + String(waterLevelCM) + " cm";
    if (waterLevelCM < MIN_WATER_LEVEL_CM) autoMessage += " ⚠️ RENDAH";
    else autoMessage += " ✅ NORMAL";
    
    if (significantChange) {
      autoMessage += "\n\n🔔 Perubahan signifikan terdeteksi!";
    }
    
    sendTelegramMessage(autoMessage);
    
    // Update nilai terakhir yang dikirim
    lastNotifiedPPM = ppmValue;
    lastNotifiedPH = pHValue;
    lastNotifiedTemp = temperatureC;
    lastNotifiedFlow = flowDetected;
    lastNotifiedWaterLevel = waterLevelCM;
    lastAutoNotification = millis();
    
    if (DEBUG_SERIAL) Serial.println("[Telegram] Auto-notification terkirim");
  }
}

// ===== Periksa dan Tangani Pesan dari Telegram =====
void handleTelegramMessages() {
  if (millis() - lastMessageTime > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);  // Mulai dari ID pesan baru
    while (numNewMessages) {
      String message = bot.messages[numNewMessages - 1].text;
      String fromName = bot.messages[numNewMessages - 1].from_name;

      if (DEBUG_SERIAL) {
        Serial.println("[Telegram] Pesan baru dari " + fromName + ": " + message);
      }

      // Perintah /add_ab
      if (message == "/add_ab") {
        sendTelegramMessage("🔵 Menambahkan pupuk A & B...");
        activatePump(POMPA_A_PIN, DOSING_DURATION_MS);
        delay(1000);
        activatePump(POMPA_B_PIN, DOSING_DURATION_MS);
        sendTelegramMessage("✅ Pupuk A & B berhasil ditambahkan!");
      }
      // Perintah /ph_up
      else if (message == "/ph_up") {
        sendTelegramMessage("🔵 Menambahkan pH UP...");
        activatePump(PH_UP_PIN, DOSING_DURATION_MS);
        sendTelegramMessage("✅ pH UP berhasil ditambahkan!");
      }
      // Perintah /ph_down
      else if (message == "/ph_down") {
        sendTelegramMessage("🔵 Menambahkan pH DOWN...");
        activatePump(PH_DOWN_PIN, DOSING_DURATION_MS);
        sendTelegramMessage("✅ pH DOWN berhasil ditambahkan!");
      }
      // Perintah /status
      else if (message == "/status") {
        String statusMessage = "📊 Status Sistem Hidroponik:\n\n";
        statusMessage += "🌐 WiFi: " + getWiFiStatusString() + "\n";
        statusMessage += "📶 Signal: " + String(WiFi.RSSI()) + " dBm\n";
        statusMessage += "🌐 IP: " + WiFi.localIP().toString() + "\n\n";
        statusMessage += "💧 PPM: " + String(ppmValue) + "\n";
        statusMessage += "⚗️ pH: " + String(pHValue) + "\n";
        statusMessage += "🌡️ Suhu: " + String(temperatureC) + "°C\n";
        statusMessage += "💨 Flow: " + String(flowDetected ? "✅ OK" : "❌ Error") + "\n";
        statusMessage += "📏 Level Air: " + String(waterLevelCM) + " cm\n\n";
        statusMessage += "📢 Auto-notification: " + String(autoNotificationEnabled ? "ON" : "OFF");

        sendTelegramMessage(statusMessage);
      }
      // Perintah /wifi untuk cek WiFi
      else if (message == "/wifi") {
        String wifiMessage = "🌐 Status WiFi:\n\n";
        wifiMessage += "📡 Status: " + getWiFiStatusString() + "\n";
        wifiMessage += "🏠 SSID: " + String(WIFI_SSID) + "\n";
        wifiMessage += "🌐 IP: " + WiFi.localIP().toString() + "\n";
        wifiMessage += "📶 Signal: " + String(WiFi.RSSI()) + " dBm\n";
        wifiMessage += "🔧 MAC: " + WiFi.macAddress();
        
        sendTelegramMessage(wifiMessage);
      }
      // Perintah /notify_on - Aktifkan auto-notification
      else if (message == "/notify_on") {
        autoNotificationEnabled = true;
        lastAutoNotification = millis(); // Reset timer
        sendTelegramMessage("🔔 Auto-notification DIAKTIFKAN!\n📅 Update setiap 5 menit atau saat ada perubahan signifikan");
      }
      // Perintah /notify_off - Nonaktifkan auto-notification
      else if (message == "/notify_off") {
        autoNotificationEnabled = false;
        sendTelegramMessage("🔕 Auto-notification DINONAKTIFKAN!\n📱 Gunakan /status untuk cek manual");
      }
      // Perintah /notify_status - Status auto-notification
      else if (message == "/notify_status") {
        String notifyMsg = "📢 Status Auto-Notification:\n\n";
        notifyMsg += "Status: " + String(autoNotificationEnabled ? "🔔 AKTIF" : "🔕 NONAKTIF") + "\n";
        if (autoNotificationEnabled) {
          unsigned long nextUpdate = (autoNotificationInterval - (millis() - lastAutoNotification)) / 1000;
          notifyMsg += "Update berikutnya: " + String(nextUpdate) + " detik\n";
          notifyMsg += "Interval: 5 menit\n";
          notifyMsg += "Trigger: Perubahan signifikan sensor";
        }
        sendTelegramMessage(notifyMsg);
      }
      // Perintah /help
      else if (message == "/help" || message == "/start") {
        String helpMessage = "🤖 Bot Hidroponik - Perintah:\n\n\n";
        helpMessage += "/status - Status sistem\n\n";
        helpMessage += "/wifi - Status WiFi\n\n";
        helpMessage += "/add_ab - Tambah pupuk A&B\n\n";
        helpMessage += "/ph_up - Tambah pH UP\n\n";
        helpMessage += "/ph_down - Tambah pH DOWN\n\n";
        helpMessage += "/test - Test koneksi bot\n\n";
        helpMessage += "/help - Tampilkan bantuan\n\n";
        helpMessage += "/notify_on - Aktifkan auto-notification\n\n";
        helpMessage += "/notify_off - Nonaktifkan auto-notification\n\n";
        
        sendTelegramMessage(helpMessage);
      }
      // Perintah /test
      else if (message == "/test") {
        sendTelegramMessage("✅ Bot aktif dan berfungsi normal!\n🕐 " + String(millis()/1000) + " detik uptime");
      }
      
      // Cek pesan baru
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastMessageTime = millis();
  }
}

void checkTelegramMessages() {
  if (millis() - lastTelegramCheckTime >= telegramPollingInterval) {
    handleTelegramMessages();  // Periksa pesan baru
    lastTelegramCheckTime = millis();  // Update waktu pengecekan pesan
  }
  
  // Cek auto-notification
  checkAutoNotification();
}

// ===== Notifikasi Telegram =====
void sendAlertNotification(String alertMessage) {
  sendTelegramMessage("⚠️ ALERT: " + alertMessage);
}

// ===== Test Koneksi Telegram =====
void testTelegramConnection() {
  if (WiFi.status() == WL_CONNECTED) {
    sendTelegramMessage("🔧 Test koneksi - Bot aktif dan siap!");
  } else {
    if (DEBUG_SERIAL) Serial.println("[Telegram] WiFi tidak terhubung untuk test");
  }
}

#endif
