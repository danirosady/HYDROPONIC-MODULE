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
