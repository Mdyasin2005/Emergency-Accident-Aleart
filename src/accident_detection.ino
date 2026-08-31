#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <math.h>

// =====================================================
// PIN DEFINITIONS
// =====================================================

#define MPU_ADDR 0x68

#define MPU_SDA D2
#define MPU_SCL D1

#define BUZZER D7
#define BUTTON D3

// GPS
#define GPS_RX D5
#define GPS_TX D6


// =====================================================
// WIFI
// =====================================================

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

#define BOT_TOKEN "YOUR_BOT_TOKEN"
#define CHAT_ID "YOUR_CHAT_ID"
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);


// =====================================================
// GPS
// =====================================================

TinyGPSPlus gps;
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);


// =====================================================
// MPU6050
// =====================================================

// ±8g
const float ACCEL_SCALE = 4096.0;
const float GRAVITY = 9.80665;

// Accident threshold
const float IMPACT_THRESHOLD = 12.0;


// =====================================================
// ACCIDENT SETTINGS
// =====================================================

const unsigned long CANCEL_TIME = 10000;

bool accidentDetected = false;
bool alertSent = false;

unsigned long accidentStartTime = 0;


// =====================================================
// SAVED ACCIDENT DATE & TIME
// =====================================================

int savedDay = 0;
int savedMonth = 0;
int savedYear = 0;

int savedHour = 0;
int savedMinute = 0;
int savedSecond = 0;

bool savedTimeValid = false;


// =====================================================
// FUNCTION DECLARATIONS
// =====================================================

bool readAcceleration(
  float &X,
  float &Y,
  float &Z,
  float &total
);

void sendAccidentAlert();


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println("    ACCIDENT DETECTION SYSTEM");
  Serial.println("================================");


  // ---------------------------------------------------
  // BUZZER
  // ---------------------------------------------------

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);


  // ---------------------------------------------------
  // BUTTON
  // ---------------------------------------------------

  pinMode(BUTTON, INPUT_PULLUP);


  // ---------------------------------------------------
  // MPU6050
  // ---------------------------------------------------

  Wire.begin(MPU_SDA, MPU_SCL);
  Wire.setClock(100000);

  Wire.beginTransmission(MPU_ADDR);

  byte error = Wire.endTransmission();

  if (error != 0) {

    Serial.println("ERROR: MPU6050 NOT FOUND!");
    Serial.print("I2C Error: ");
    Serial.println(error);

  } else {

    Serial.println("MPU6050 FOUND at 0x68");


    // Wake MPU6050

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission();


    // Set accelerometer to ±8g

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x1C);
    Wire.write(0x10);
    Wire.endTransmission();


    Serial.println("MPU6050 READY");
  }


  // ---------------------------------------------------
  // GPS
  // ---------------------------------------------------

  gpsSerial.begin(9600);

  Serial.println("GPS STARTED");


  // ---------------------------------------------------
  // WIFI
  // ---------------------------------------------------

  Serial.println();
  Serial.print("Connecting to WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi CONNECTED");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());


  // ---------------------------------------------------
  // TELEGRAM
  // ---------------------------------------------------

  client.setInsecure();

  Serial.println("Telegram ready.");

  Serial.println();
  Serial.println("================================");
  Serial.println("SYSTEM READY");
  Serial.println("Impact threshold: 12 m/s^2");
  Serial.println("Cancel window: 10 seconds");
  Serial.println("================================");
  Serial.println();
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop() {


  // ===================================================
  // READ GPS CONTINUOUSLY
  // ===================================================

  while (gpsSerial.available()) {

    gps.encode(gpsSerial.read());
  }


  // ===================================================
  // ACCIDENT ALREADY DETECTED
  // ===================================================

  if (accidentDetected) {


    // -------------------------------------------------
    // BUTTON PRESSED
    // -------------------------------------------------

    if (digitalRead(BUTTON) == LOW) {

      delay(30);

      if (digitalRead(BUTTON) == LOW) {

        digitalWrite(BUZZER, LOW);

        accidentDetected = false;

        Serial.println();
        Serial.println("================================");
        Serial.println("     ACCIDENT CANCELLED");
        Serial.println("     BUZZER OFF");
        Serial.println("================================");
        Serial.println();


        // Wait for button release

        while (digitalRead(BUTTON) == LOW) {

          delay(10);
        }

        return;
      }
    }


    // -------------------------------------------------
    // 10 SECOND TIMER
    // -------------------------------------------------

    unsigned long elapsed =
      millis() - accidentStartTime;


    Serial.print("Cancel window: ");

    Serial.print(
      (CANCEL_TIME - elapsed) / 1000
    );

    Serial.println(" seconds");


    // -------------------------------------------------
    // 10 SECONDS COMPLETED
    // -------------------------------------------------

    if (elapsed >= CANCEL_TIME) {

      digitalWrite(BUZZER, LOW);

      accidentDetected = false;

      Serial.println();
      Serial.println("================================");
      Serial.println("     ACCIDENT CONFIRMED");
      Serial.println("     NO CANCELLATION");
      Serial.println("================================");
      Serial.println();


      // Send Telegram

      if (!alertSent) {

        sendAccidentAlert();

        alertSent = true;
      }

      return;
    }


    delay(100);

    return;
  }


  // ===================================================
  // READ ACCELERATION
  // ===================================================

  float X;
  float Y;
  float Z;
  float totalAcceleration;


  if (!readAcceleration(
        X,
        Y,
        Z,
        totalAcceleration
      )) {

    Serial.println("MPU6050 READ ERROR");

    delay(100);

    return;
  }


  // ===================================================
  // PRINT ACCELERATION
  // ===================================================

  Serial.print("X: ");
  Serial.print(X, 2);

  Serial.print(" | Y: ");
  Serial.print(Y, 2);

  Serial.print(" | Z: ");
  Serial.print(Z, 2);

  Serial.print(" | TOTAL: ");
  Serial.print(totalAcceleration, 2);

  Serial.print(" m/s^2 | ");


  // ===================================================
  // IMPACT DETECTION
  // ===================================================

  if (totalAcceleration >= IMPACT_THRESHOLD) {


    // -------------------------------------------------
    // IMPACT DETECTED
    // -------------------------------------------------

    accidentDetected = true;
    alertSent = false;

    accidentStartTime = millis();


    // -------------------------------------------------
    // SAVE GPS DATE & TIME
    // -------------------------------------------------

    if (
      gps.date.isValid() &&
      gps.time.isValid()
    ) {

      savedDay = gps.date.day();
      savedMonth = gps.date.month();
      savedYear = gps.date.year();

      savedHour = gps.time.hour();
      savedMinute = gps.time.minute();
      savedSecond = gps.time.second();

      // -----------------------------------------------
      // UTC → IST
      // -----------------------------------------------

      savedMinute += 30;

      if (savedMinute >= 60) {

        savedMinute -= 60;
        savedHour++;
      }

      savedHour += 5;

      if (savedHour >= 24) {

        savedHour -= 24;
      }

      savedTimeValid = true;

    } else {

      savedTimeValid = false;
    }


    // -------------------------------------------------
    // BUZZER ON
    // -------------------------------------------------

    digitalWrite(BUZZER, HIGH);


    Serial.println(">>> IMPACT DETECTED <<<");
    Serial.println(">>> BUZZER ON <<<");
    Serial.println("Press button within 10 seconds to cancel.");


    // -------------------------------------------------
    // DISPLAY SAVED TIME
    // -------------------------------------------------

    if (savedTimeValid) {

      Serial.print("Accident Date: ");

      Serial.print(savedDay);
      Serial.print("/");
      Serial.print(savedMonth);
      Serial.print("/");
      Serial.println(savedYear);


      Serial.print("Accident Time: ");

      if (savedHour < 10)
        Serial.print("0");

      Serial.print(savedHour);
      Serial.print(":");


      if (savedMinute < 10)
        Serial.print("0");

      Serial.print(savedMinute);
      Serial.print(":");


      if (savedSecond < 10)
        Serial.print("0");

      Serial.print(savedSecond);

      Serial.println(" IST");
    }


  } else {

    digitalWrite(BUZZER, LOW);

    Serial.println("NORMAL");
  }


  delay(20);
}


// =====================================================
// READ MPU6050
// =====================================================

bool readAcceleration(
  float &X,
  float &Y,
  float &Z,
  float &total
) {


  Wire.beginTransmission(MPU_ADDR);

  Wire.write(0x3B);

  byte error =
    Wire.endTransmission(false);


  if (error != 0) {

    return false;
  }


  Wire.requestFrom(
    MPU_ADDR,
    6
  );


  if (Wire.available() != 6) {

    return false;
  }


  int16_t ax =
    (Wire.read() << 8) |
     Wire.read();


  int16_t ay =
    (Wire.read() << 8) |
     Wire.read();


  int16_t az =
    (Wire.read() << 8) |
     Wire.read();


  // ---------------------------------------------------
  // Convert raw values to m/s²
  // ---------------------------------------------------

  X =
    (ax / ACCEL_SCALE) *
    GRAVITY;


  Y =
    (ay / ACCEL_SCALE) *
    GRAVITY;


  Z =
    (az / ACCEL_SCALE) *
    GRAVITY;


  // ---------------------------------------------------
  // Total acceleration
  // ---------------------------------------------------

  total =
    sqrt(
      X * X +
      Y * Y +
      Z * Z
    );


  return true;
}


// =====================================================
// SEND TELEGRAM ACCIDENT ALERT
// =====================================================

void sendAccidentAlert() {


  Serial.println("Preparing Telegram alert...");


  String message = "";


  // ---------------------------------------------------
  // HEADER
  // ---------------------------------------------------

  message += "🚨 ACCIDENT DETECTED 🚨\n\n";


  message +=
    "The accident detection system has confirmed an impact.\n\n";


  // ---------------------------------------------------
  // DATE & TIME
  // ---------------------------------------------------

  if (savedTimeValid) {

    message += "📅 Date: ";

    if (savedDay < 10)
      message += "0";

    message += String(savedDay);

    message += "/";

    if (savedMonth < 10)
      message += "0";

    message += String(savedMonth);

    message += "/";

    message += String(savedYear);

    message += "\n";


    message += "⏰ Accident Time: ";

    if (savedHour < 10)
      message += "0";

    message += String(savedHour);

    message += ":";


    if (savedMinute < 10)
      message += "0";

    message += String(savedMinute);

    message += ":";


    if (savedSecond < 10)
      message += "0";

    message += String(savedSecond);

    message += " IST\n\n";

  } else {

    message +=
      "⏰ Accident time unavailable.\n\n";
  }


  // ---------------------------------------------------
  // GPS LOCATION
  // ---------------------------------------------------

  if (gps.location.isValid()) {


    double latitude =
      gps.location.lat();


    double longitude =
      gps.location.lng();


    message += "📍 Location:\n";


    message += "Latitude: ";

    message +=
      String(latitude, 6);

    message += "\n";


    message += "Longitude: ";

    message +=
      String(longitude, 6);

    message += "\n\n";


    // Google Maps

    message += "🗺 Google Maps:\n";

    message +=
      "https://maps.google.com/?q=";


    message +=
      String(latitude, 6);


    message += ",";


    message +=
      String(longitude, 6);


    message += "\n\n";


  } else {

    message +=
      "📍 GPS location unavailable.\n\n";
  }


  // ---------------------------------------------------
  // SEND MESSAGE
  // ---------------------------------------------------

  Serial.println("Sending Telegram alert...");


  bool result =
    bot.sendMessage(
      CHAT_ID,
      message,
      ""
    );


  if (result) {

    Serial.println(
      "Telegram alert SENT successfully!"
    );

  } else {

    Serial.println(
      "Telegram alert FAILED!"
    );
  }
}
