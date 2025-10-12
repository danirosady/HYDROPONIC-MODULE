#ifndef AUTOMATION_H
#define AUTOMATION_H

#include "config.h"

unsigned long lastPumpTime = 0;  // Timestamp untuk kontrol pompa
bool pumpActive = false;

// Fungsi bantuan untuk menyalakan pompa peristaltik selama durasi tertentu
void activatePump(int pin, unsigned long duration) {
  if (!pumpActive) {
    pumpActive = true;
    digitalWrite(pin, HIGH);  // Menyalakan pompa
    lastPumpTime = millis();  // Simpan waktu sekarang
  }

  // Matikan pompa setelah durasi
  if (pumpActive && (millis() - lastPumpTime >= duration)) {
    digitalWrite(pin, LOW);
    pumpActive = false;
  }
}

// Setup pin output pompa dan sirkulasi
void setupPins() {
  pinMode(POMPA_A_PIN, OUTPUT);
  pinMode(POMPA_B_PIN, OUTPUT);
  pinMode(PH_UP_PIN, OUTPUT);
  pinMode(PH_DOWN_PIN, OUTPUT);
  pinMode(CIRCULATION_PUMP_PIN, OUTPUT);

  // Pompa sirkulasi ON non-stop
  digitalWrite(CIRCULATION_PUMP_PIN, HIGH);
  if (DEBUG_SERIAL) Serial.println("[INIT] Pompa sirkulasi ON nonstop");
}

// Fungsi utama otomatisasi
void processAutomation() {
  if (!flowDetected) {
    // if (DEBUG_SERIAL) Serial.println("[AUTO] PERINGATAN: Aliran air TIDAK terdeteksi!");
    // Bisa tambahkan notifikasi Telegram di sini
    return;
  }

  if (waterLevelCM > MIN_WATER_LEVEL_CM) {
    if (DEBUG_SERIAL) Serial.println("[AUTO] PERINGATAN: Air hampir habis!");
    // Bisa tambahkan notifikasi Telegram
    return;
  }

  if (temperatureC < MIN_TEMP_C) {
    if (DEBUG_SERIAL) Serial.println("[AUTO] PERINGATAN: Suhu terlalu rendah!");
    // Bisa aktifkan heater atau kirim notifikasi
  }

  // Koreksi TDS (PPM)
  if (ppmValue < PPM_TARGET_LOW) {
    if (DEBUG_SERIAL) Serial.println("[AUTO] PPM rendah, menambahkan pupuk A & B...");
    activatePump(POMPA_A_PIN, DOSING_DURATION_MS);
    delay(1000);  // jeda antar pompa
    activatePump(POMPA_B_PIN, DOSING_DURATION_MS);
  }

  // Koreksi pH
  if (pHValue < PH_LOW) {
    if (DEBUG_SERIAL) Serial.println("[AUTO] pH terlalu rendah, menambahkan pH UP...");
    activatePump(PH_UP_PIN, DOSING_DURATION_MS);
  } else if (pHValue > PH_HIGH) {
    if (DEBUG_SERIAL) Serial.println("[AUTO] pH terlalu tinggi, menambahkan pH DOWN...");
    activatePump(PH_DOWN_PIN, DOSING_DURATION_MS);
  }
}

#endif
