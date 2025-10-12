#ifndef SENSORS_H
#define SENSORS_H

#include <OneWire.h>
#include <DallasTemperature.h>

// ===== Variabel Global Sensor =====
float temperatureC = 0.0;
float pHValue = 0.0;
int ppmValue = 0;
bool flowDetected = true;
float waterLevelCM = 0.0;

unsigned long lastSensorReadTime = 0;
const unsigned long sensorReadInterval = 5000; 

// ===== Inisialisasi Sensor Suhu (DS18B20) =====
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ===== Setup Sensor =====
void setupSensors() {
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  sensors.begin();
}

// ===== Baca Sensor Suhu Air =====
void readTemperature() {
  sensors.requestTemperatures();
  temperatureC = sensors.getTempCByIndex(0);
  if (DEBUG_SERIAL) Serial.println("[Sensor] Suhu: " + String(temperatureC) + " °C");
}

// ===== Baca Sensor Ultrasonik =====
void readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30 ms timeout
  float distanceCM = duration * 0.034 / 2.0;
  waterLevelCM = distanceCM; // Bisa dikalibrasi tergantung posisi sensor

  if (DEBUG_SERIAL) Serial.println("[Sensor] Ketinggian Air: " + String(waterLevelCM) + " cm");
}

// ===== Baca Sensor Water Flow (Sederhana - pulseIn) =====
void readWaterFlow() {
  unsigned long duration = pulseIn(FLOW_SENSOR_PIN, HIGH, 1000000);  // Timeout 1 detik
  flowDetected = duration > 0 && duration < 100000;  // Jika terlalu besar, berarti tidak ada flow

  if (DEBUG_SERIAL) {
    if (flowDetected) Serial.println("[Sensor] Aliran air: OK");
    else Serial.println("[Sensor] Aliran air: TIDAK TERDETEKSI");
  }
}

// ===== Baca Sensor pH =====
void readPH() {
  int analogValue = analogRead(PH_PIN);
  float voltage = analogValue * 3.3 / 4095.0;
  
  // Formula kalibrasi — sesuaikan dengan sensor Anda
  pHValue = 7 + ((2.5 - voltage) / 0.18);

  if (DEBUG_SERIAL) Serial.println("[Sensor] pH: " + String(pHValue));
}

// ===== Baca Sensor TDS =====
void readTDS() {
  int analogValue = analogRead(TDS_PIN);
  float voltage = analogValue * 3.3 / 4095.0;

  // Formula konversi — sesuaikan dengan sensor TDS Anda
  ppmValue = (int)((133.42 * voltage * voltage * voltage) - (255.86 * voltage * voltage) + (857.39 * voltage)) * 0.5;

  if (DEBUG_SERIAL) Serial.println("[Sensor] PPM: " + String(ppmValue));
}

void readAllSensors() {
  if (millis() - lastSensorReadTime >= sensorReadInterval) {
    // Baca semua sensor
    readTemperature();
    readUltrasonic();
    readWaterFlow();
    readPH();
    readTDS();
    Serial.println("==========================");
    lastSensorReadTime = millis();  // Update waktu baca sensor
  }
}

#endif
