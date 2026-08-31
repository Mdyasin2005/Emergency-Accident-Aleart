
#include <Wire.h>

#define MPU6050_ADDR 0x68

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("==============================");
  Serial.println("       MPU6050 TEST");
  Serial.println("==============================");

  Wire.begin(D2, D1);
  Wire.setClock(100000);

  // Check MPU6050
  Wire.beginTransmission(MPU6050_ADDR);
  byte error = Wire.endTransmission();

  if (error == 0) {
    Serial.println("MPU6050 FOUND at 0x68");
  } else {
    Serial.print("MPU6050 NOT FOUND - I2C Error: ");
    Serial.println(error);
    return;
  }

  // Wake up MPU6050
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  // Set accelerometer range to +/- 8g
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission();

  Serial.println("MPU6050 READY");
  Serial.println();
}

void loop() {

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B);

  if (Wire.endTransmission(false) != 0) {
    Serial.println("MPU6050 READ ERROR");
    delay(500);
    return;
  }

  Wire.requestFrom(MPU6050_ADDR, 6);

  if (Wire.available() != 6) {
    Serial.println("READ FAILED");
    delay(500);
    return;
  }

  int16_t ax = (Wire.read() << 8) | Wire.read();
  int16_t ay = (Wire.read() << 8) | Wire.read();
  int16_t az = (Wire.read() << 8) | Wire.read();

  float X = (ax / 4096.0) * 9.80665;
  float Y = (ay / 4096.0) * 9.80665;
  float Z = (az / 4096.0) * 9.80665;

  float total = sqrt(X * X + Y * Y + Z * Z);

  Serial.print("X: ");
  Serial.print(X, 2);

  Serial.print(" m/s² | Y: ");
  Serial.print(Y, 2);

  Serial.print(" m/s² | Z: ");
  Serial.print(Z, 2);

  Serial.print(" m/s² | TOTAL: ");
  Serial.print(total, 2);

  Serial.println(" m/s²");

  delay(200);
}
