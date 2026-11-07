#include <MPU6050.h>
#include <Wire.h>

MPU6050 mpu(0x69);
const int buzzer = 8;  // Pin na kojem je buzzer

bool movementDetected = false;
unsigned long firstMovementTime = 0;
bool alertActive = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 nije povezan!");
    while (1);
  }
  Serial.println("MPU6050 inicijaliziran.");
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Izračunaj ukupno ubrzanje
  float accelMagnitude = sqrt(ax * ax + ay * ay + az * az);

  static float lastAccelMagnitude = accelMagnitude;
  float delta = abs(accelMagnitude - lastAccelMagnitude);

  if (delta > 1000) {  // Prag za detekciju pokreta
    if (!movementDetected) {
      // Prvi pokret: kratko upozorenje
      movementDetected = true;
      firstMovementTime = millis();
      Serial.println("Pokret detektiran! Piši 1 sekundu.");
      digitalWrite(buzzer, HIGH);
      delay(1000);
      digitalWrite(buzzer, LOW);
    } else if (millis() - firstMovementTime < 10000 && !alertActive) {
      // Drugi pokret u roku od 10 s: alarm
      Serial.println("Drugi pokret u roku od 10s! Piši 20 sekundi.");
      digitalWrite(buzzer, HIGH);
      delay(20000);
      digitalWrite(buzzer, LOW);
      alertActive = true;
    }
  }

  // Reset nakon 10 s bez novog pokreta
  if (millis() - firstMovementTime > 10000) {
    movementDetected = false;
    alertActive = false;
  }

  lastAccelMagnitude = accelMagnitude;
  delay(100);
}
