#include <Servo.h>
#include <math.h>

#define PIN_TRIG 12
#define PIN_ECHO 13
#define PIN_SERVO 10

#define SND_VEL 346.0
#define DETECT_DISTANCE 150
#define INTERVAL 50
#define MOVING_TIME 3000

Servo myServo;
unsigned long lastSamplingTime = 0;
unsigned long moveStartTime = 0;
bool carDetected = false;
int openAngle = 90;
int closeAngle = 30;

float measureDistance();
float sigmoid(float x);

void setup() {
  Serial.begin(9600);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  myServo.attach(PIN_SERVO);
  myServo.write(closeAngle);
  delay(500);
}

void loop() {
  unsigned long now = millis();

  if (now - lastSamplingTime >= INTERVAL) {
    lastSamplingTime = now;

    float distance = measureDistance();

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" mm");

    if (distance < DETECT_DISTANCE && !carDetected) {
      carDetected = true;
      moveStartTime = millis();
    }
    else if (distance >= DETECT_DISTANCE + 50 && carDetected) {
      carDetected = false;
      moveStartTime = millis();
    }
  }

  unsigned long progress = millis() - moveStartTime;
  float t = (float)progress / MOVING_TIME;

  if (t > 1.0) t = 1.0;

  float smooth = sigmoid((t - 0.5) * 10);

  int targetAngle;
  if (carDetected) {
    targetAngle = closeAngle + (openAngle - closeAngle) * smooth;
  } else {
    targetAngle = openAngle - (openAngle - closeAngle) * smooth;
  }

  myServo.write(targetAngle);
}

float measureDistance() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long duration = pulseIn(PIN_ECHO, HIGH);
  float distance = (duration * 0.000001 * SND_VEL / 2.0) * 1000.0;
  return distance;
}

float sigmoid(float x) {
  return 1.0 / (1.0 + exp(-x));
}
