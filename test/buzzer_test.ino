
#define BUZZER D7

void setup() {

  Serial.begin(115200);

  pinMode(BUZZER, OUTPUT);

  digitalWrite(BUZZER, LOW);

  Serial.println();
  Serial.println("==============================");
  Serial.println("        BUZZER TEST");
  Serial.println("==============================");
}

void loop() {

  Serial.println("BUZZER ON");

  digitalWrite(BUZZER, HIGH);

  delay(2000);

  Serial.println("BUZZER OFF");

  digitalWrite(BUZZER, LOW);

  delay(2000);
}
