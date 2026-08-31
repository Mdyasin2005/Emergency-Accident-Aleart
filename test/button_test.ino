
#define BUTTON D3

void setup() {

  Serial.begin(115200);

  pinMode(BUTTON, INPUT_PULLUP);

  Serial.println();
  Serial.println("==============================");
  Serial.println("       BUTTON TEST");
  Serial.println("==============================");
  Serial.println("Press the button...");
}

void loop() {

  if (digitalRead(BUTTON) == LOW) {

    Serial.println("BUTTON PRESSED");

    delay(300);

  } else {

    Serial.println("BUTTON RELEASED");

    delay(300);
  }
}
