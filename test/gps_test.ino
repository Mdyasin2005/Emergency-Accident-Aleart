
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

#define GPS_RX D5
#define GPS_TX D6

TinyGPSPlus gps;
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);

void setup() {

  Serial.begin(115200);
  gpsSerial.begin(9600);

  Serial.println();
  Serial.println("==============================");
  Serial.println("          GPS TEST");
  Serial.println("==============================");
  Serial.println("Waiting for GPS data...");
}

void loop() {

  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  // Location
  if (gps.location.isUpdated()) {

    Serial.println();
    Serial.println("GPS DATA");

    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);

    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);

    Serial.print("Satellites: ");
    Serial.println(gps.satellites.value());
  }

  // Date
  if (gps.date.isUpdated()) {

    Serial.print("Date: ");

    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.println(gps.date.year());
  }

  // Time
  if (gps.time.isUpdated()) {

    Serial.print("GPS UTC Time: ");

    if (gps.time.hour() < 10)
      Serial.print("0");

    Serial.print(gps.time.hour());
    Serial.print(":");

    if (gps.time.minute() < 10)
      Serial.print("0");

    Serial.print(gps.time.minute());
    Serial.print(":");

    if (gps.time.second() < 10)
      Serial.print("0");

    Serial.println(gps.time.second());
  }

  delay(10);
}
