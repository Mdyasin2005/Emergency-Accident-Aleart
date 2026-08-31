
#include <Wire.h>
#include <math.h>

#define MPU6050_ADDR 0x68
#define BUZZER D7
#define BUTTON D3

#define IMPACT_THRESHOLD 12.0

bool impactDetected = false;

void setup() {

  Serial.begin(115200);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  pinMode(BUTTON, INPUT_PULLUP);

  Wire.begin(D2, D1);
  Wire.setClock(100000);

  Serial.println();
  Serial.println("==============================");
  Serial.println("    ACCIDENT IMPACT TEST");
  Serial.println("==============================");

  Wire.beginTransmission(MPU6050_ADDR);
  byte error = Wire.endTransmission();

  if (error != 0) {

    Serial.print("MPU6050 NOT FOUND - I2C Error: ");
    Serial.println(error);

    while (true) {
      delay(1000);
    }
  }

  // Wake MPU6050
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  // ±8g
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission();

  Serial.println("MPU6050 READY");
  Serial.println("Impact threshold: 12 m/s²");
  Serial.println();
}

void loop() {

  // Read MPU6050

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B);

  if (Wire.endTransmission(false) != 0) {
    Serial.println("MPU READ ERROR");
    delay(100);
    return;
  }

  Wire.requestFrom(MPU6050_ADDR, 6);

  if (Wire.available() != 6) {
    Serial.println("MPU READ FAILED");
    delay(100);
    return;
  }

  int16_t ax = (Wire.read() << 8) | Wire.read();
  int16_t ay = (Wire.read() << 8) | Wire.read();
  int16_t az = (Wire.read() << 8) | Wire.read();

  float X = (ax / 4096.0) * 9.80665;
  float Y = (ay / 4096.0) * 9.80665;
  float Z = (az / 4096.0) * 9.80665;

  float total = sqrt(
    X * X +
    Y * Y +
    Z * Z
  );

  Serial.print("X: ");
  Serial.print(X, 2);

  Serial.print(" | Y: ");
  Serial.print(Y, 2);

  Serial.print(" | Z: ");
  Serial.print(Z, 2);

  Serial.print(" | TOTAL: ");
  Serial.print(total, 2);

  Serial.print(" m/s² | ");


  // Impact detection

  if (total >= IMPACT_THRESHOLD) {

    Serial.println(">>> IMPACT DETECTED <<<");

    digitalWrite(BUZZER, HIGH);

    impactDetected = true;

    Serial.println(">>> BUZZER ON <<<");
    Serial.println("Press button to turn buzzer OFF.");

  } else {

    Serial.println("NORMAL");
  }


  // Button turns buzzer OFF

  if (impactDetected &&
      digitalRead(BUTTON) == LOW) {

    digitalWrite(BUZZER, LOW);

    impactDetected = false;

    Serial.println(">>> BUTTON PRESSED <<<");
    Serial.println(">>> BUZZER OFF <<<");

    delay(500);
  }

  delay(50);
}
