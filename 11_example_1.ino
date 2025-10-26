#include <Servo.h>

#define PIN_LED 9
#define PIN_TRIG 12
#define PIN_ECHO 13
#define PIN_SERVO 10

#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 180.0
#define _DIST_MAX 360.0

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

#define _EMA_ALPHA 0.3

#define _TARGET_LOW 250.0
#define _TARGET_HIGH 290.0

#define _DUTY_MIN 700
#define _DUTY_NEU 1550
#define _DUTY_MAX 2400

float dist_ema, dist_prev = _DIST_MAX;
unsigned long last_sampling_time;

Servo myservo;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  myservo.attach(PIN_SERVO);
  myservo.writeMicroseconds(_DUTY_NEU);

  dist_prev = _DIST_MIN;

  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_filtered;

  if (millis() < last_sampling_time + INTERVAL)
    return;

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  if (dist_raw == 0.0 || dist_raw > _DIST_MAX) {
    dist_filtered = _DIST_MAX + 10.0;
  } else if (dist_raw < _DIST_MIN) {
    dist_filtered = dist_ema;
  } else {
    dist_filtered = dist_raw;
  }

  if (dist_ema == 0.0) {
    dist_ema = dist_filtered;
  } else {
    dist_ema = _EMA_ALPHA * dist_filtered + (1 - _EMA_ALPHA) * dist_ema;
  }

  float angle;

  if (dist_ema <= _DIST_MIN) {
    angle = 0;
  } else if (dist_ema >= _DIST_MAX) {
    angle = 180;
  } else {
    angle = (dist_ema - _DIST_MIN) * (180.0 / (_DIST_MAX - _DIST_MIN));
  }

  int duty = map(angle, 0, 180, _DUTY_MIN, _DUTY_MAX);
  myservo.writeMicroseconds(duty);

  if (dist_ema > _DIST_MIN && dist_ema < _DIST_MAX) {
    digitalWrite(PIN_LED, LOW);
  } else {
    digitalWrite(PIN_LED, HIGH);
  }

  Serial.print("Min:"); Serial.print(_DIST_MIN);
  Serial.print(",Low:"); Serial.print(_TARGET_LOW);
  Serial.print(",dist:"); Serial.print(dist_raw);
  Serial.print(",Servo:"); Serial.print(myservo.read());
  Serial.print(",High:"); Serial.print(_TARGET_HIGH);
  Serial.print(",Max:"); Serial.print(_DIST_MAX);

  Serial.println("");

  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
