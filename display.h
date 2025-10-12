#ifndef DISPLAY_H
#define DISPLAY_H

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Adafruit_GFX.h>
#include "config.h"

unsigned long lastDisplayUpdateTime = 0;
const unsigned long displayUpdateInterval = 2000;  // Interval pembaruan display (2 detik)

// Inisialisasi panel matrix
HUB75_I2S_CFG mxconfig(
  PANEL_WIDTH,   // Width
  PANEL_HEIGHT,  // Height
  PANEL_CHAIN    // Total panels chained
);

MatrixPanel_I2S_DMA display(mxconfig);

// Warna
#define COLOR_NORMAL display.color565(0, 255, 0)
#define COLOR_WARN   display.color565(255, 0, 0)
#define COLOR_INFO   display.color565(0, 200, 255)

// ===== Inisialisasi Display =====
void setupDisplay() {
  display.begin();
  display.setBrightness8(40); // Sesuaikan tingkat kecerahan (0-255)
  display.clear();
  display.setTextWrap(false);
  display.setTextSize(1);
  display.setTextColor(COLOR_INFO);
}

// ===== Tampilkan Pesan Startup =====
void showStartupScreen() {
  display.clear();
  display.setCursor(2, 10);
  display.setTextColor(COLOR_INFO);
  display.print("SMART HYDROPONIC");
  display.setCursor(10, 24);
  display.print("SYSTEM READY");
  delay(2000);
  display.clear();
}

void updateDisplayNonBlocking() {
  if (millis() - lastDisplayUpdateTime >= displayUpdateInterval) {
    updateDisplay();  // Update tampilan
    lastDisplayUpdateTime = millis();  // Update waktu pembaruan display
  }
}

// ===== Update Display Real-time =====
void updateDisplay() {
  display.clear();

  // Baris 1 - PPM dan pH
  display.setCursor(0, 0);
  display.setTextColor(COLOR_NORMAL);
  display.print("PPM: ");
  display.print(ppmValue);

  display.setCursor(80, 0);
  display.print("pH: ");
  display.print(pHValue, 1);

  // Baris 2 - Suhu dan Flow
  display.setCursor(0, 10);
  display.print("T: ");
  display.print(temperatureC, 1);
  display.print("C");

  display.setCursor(80, 10);
  display.print("Flow: ");
  display.print(flowDetected ? "OK" : "X");

  // Baris 3 - Level Air
  display.setCursor(0, 20);
  display.print("Air: ");
  display.print(waterLevelCM, 1);
  display.print("cm");

  // Baris 4 - Status Singkat
  display.setCursor(0, 30);
  if (temperatureC < MIN_TEMP_C || !flowDetected || waterLevelCM > MIN_WATER_LEVEL_CM) {
    display.setTextColor(COLOR_WARN);
    display.print("!! CEK SISTEM !!");
  } else {
    display.setTextColor(COLOR_NORMAL);
    display.print("STATUS: NORMAL");
  }
}
#endif
