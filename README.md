# 🚨 ESP8266 IoT Accident Detection System

An IoT-based accident detection prototype that detects a sudden high-impact event using an **MPU6050 accelerometer** and sends the accident location and time through **Telegram** using an **ESP8266 NodeMCU** and GPS.

## 📌 Project Overview

The system continuously monitors acceleration using the MPU6050.

When the total acceleration crosses the configured accident threshold of:

**12 m/s²**

the system:

1. Detects a possible accident.
2. Activates the buzzer.
3. Starts a **10-second cancellation window**.
4. Allows the user to cancel the alert using a push button.
5. If the alert is not cancelled, the accident is confirmed.
6. GPS coordinates are obtained.
7. The accident date and time are recorded.
8. The accident time is converted to **IST (UTC + 5:30)**.
9. A Telegram emergency notification is sent.
10. A Google Maps location link is included in the message.

---

## ⚙️ Main Features

* 🚗 Accident impact detection
* 📈 Real-time acceleration measurement
* ⚡ 12 m/s² impact threshold
* 🔊 Buzzer warning
* 🔘 10-second cancellation button
* 📍 GPS location tracking
* 📅 Accident date recording
* ⏰ Accident time in IST
* 📱 Telegram emergency notification
* 🗺️ Google Maps location link
* 📡 Wi-Fi communication through ESP8266

---

## 🧩 Hardware Components

| Component       | Purpose                       |
| --------------- | ----------------------------- |
| ESP8266 NodeMCU | Main controller and Wi-Fi     |
| MPU6050         | Acceleration/impact detection |
| GPS Module      | Location and date/time        |
| Buzzer          | Accident warning              |
| Push Button     | Cancel false accident alert   |
| Jumper Wires    | Connections                   |
| Power Supply    | System power                  |

---

## 🔌 Pin Connections

### MPU6050

| MPU6050 | NodeMCU |
| ------- | ------- |
| VCC     | 3.3V    |
| GND     | GND     |
| SDA     | D2      |
| SCL     | D1      |

I²C address used:

```text
0x68
```

### GPS

| GPS | NodeMCU                |
| --- | ---------------------- |
| TX  | D5                     |
| RX  | D6                     |
| GND | GND                    |
| VCC | Appropriate GPS supply |

### Buzzer

| Buzzer | NodeMCU |
| ------ | ------- |
| +      | D7      |
| -      | GND     |

### Push Button

| Button     | NodeMCU |
| ---------- | ------- |
| Terminal 1 | D3      |
| Terminal 2 | GND     |

The button uses the NodeMCU's internal pull-up resistor.

---

## 🧠 Accident Detection Logic

The MPU6050 provides acceleration on three axes:

```text
X
Y
Z
```

The total acceleration is calculated as:

```text
Total = √(X² + Y² + Z²)
```

The result is expressed in:

```text
m/s²
```

The accident threshold is:

```text
12 m/s²
```

### Detection

```text
Total acceleration < 12 m/s²
        ↓
      NORMAL
```

```text
Total acceleration ≥ 12 m/s²
        ↓
 IMPACT DETECTED
        ↓
    BUZZER ON
        ↓
  10 second timer
        ↓
   Push button?
    ↙       ↘
  YES        NO
   ↓          ↓
CANCEL     CONFIRM
              ↓
        Send Telegram
```

---

## ⏰ Accident Time

The system saves the GPS date and time **when the impact is detected**.

GPS normally provides UTC time.

The system converts it to Indian Standard Time:

```text
IST = UTC + 5 hours 30 minutes
```

Therefore, the Telegram alert reports the **accident detection time**, rather than the time at which the Telegram message happens to be sent.

---

## 📱 Telegram Alert

A typical alert contains:

```text
🚨 ACCIDENT DETECTED 🚨

The accident detection system has confirmed an impact.

📅 Date: 31/08/2026
⏰ Accident Time: 17:15:20 IST

📍 Location:
Latitude: 22.xxxxxx
Longitude: 88.xxxxxx

🗺 Google Maps:
https://maps.google.com/?q=22.xxxxxx,88.xxxxxx
```

---

## 🔔 False Accident Cancellation

To reduce false alerts, the buzzer does not immediately result in a Telegram emergency notification.

After an impact:

```text
Impact detected
      ↓
Buzzer ON
      ↓
10-second cancellation period
```

If the push button is pressed:

```text
Button pressed
      ↓
Buzzer OFF
      ↓
Accident cancelled
      ↓
No Telegram alert
```

If the button is not pressed:

```text
10 seconds completed
      ↓
Accident confirmed
      ↓
Telegram alert sent
```

---

## 💻 Software Requirements

Install the Arduino IDE and configure it for the ESP8266 board.

Required libraries:

```text
ESP8266WiFi
WiFiClientSecure
UniversalTelegramBot
TinyGPSPlus
SoftwareSerial
Wire
```

Select the board:

```text
NodeMCU 1.0 (ESP-12E Module)
```

Select the correct COM port for your NodeMCU.

---

## 🔐 Configuration

Before uploading the program, enter your own Wi-Fi and Telegram credentials:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

#define BOT_TOKEN "YOUR_BOT_TOKEN"
#define CHAT_ID "YOUR_CHAT_ID"
```

### ⚠️ Security

**Do not upload real Wi-Fi passwords or Telegram bot tokens to GitHub.**

Keep credentials as placeholders in the public repository.

---

## 🧪 Module Testing

Each component was tested individually before integration.

### MPU6050

The I²C scanner should detect:

```text
I2C device found at address 0x68
```

### GPS

The GPS test verifies:

* Latitude
* Longitude
* Date
* Time
* Satellite information

### Buzzer

The buzzer test verifies:

```text
Buzzer ON
Buzzer OFF
```

### Push Button

The button test verifies button press and release detection.

### Accident Detection

The final test verifies that an acceleration of at least:

```text
12 m/s²
```

triggers the impact detection system.

---

## 📁 Repository Structure

```text
ESP8266-Accident-Detection-System/
│
├── README.md
│
├── src/
│   └── accident_detection.ino
│
├── test/
│   ├── mpu6050_test.ino
│   ├── gps_test.ino
│   ├── buzzer_test.ino
│   └── button_test.ino
│
├── docs/
│   ├── circuit-connections.md
│   ├── circuit-diagram.png
│   └── system-block-diagram.png
│
└── LICENSE
```

---

## 🚀 Future Improvements

Possible improvements for a real vehicle implementation include:

* 4G/LTE communication instead of phone hotspot Wi-Fi
* Dedicated automotive power supply
* Better impact classification
* Gyroscope-based orientation analysis
* Vehicle speed information
* Cloud dashboard
* Emergency contact notification
* Multiple emergency contacts
* Battery backup
* Automotive-grade sensors
* Improved false-positive filtering
* Direct communication with emergency services

---

## ⚠️ Prototype Disclaimer

This project is an **academic/prototype accident detection system**.

The 12 m/s² threshold is a prototype detection criterion and should not be treated as a validated automotive accident-detection standard.

A production vehicle system would require extensive testing, calibration, automotive-grade hardware, safety validation, and regulatory compliance.

---

## 👨‍💻 Project

**ESP8266 IoT Accident Detection System**

Built using:

```text
ESP8266
+
MPU6050
+
GPS
+
Buzzer
+
Push Button
+
Telegram
```

---

## 📜 License

This project is intended for educational and research purposes.
