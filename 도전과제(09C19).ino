#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

#define SND_VEL 346.0f
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 100
#define _DIST_MAX 300
#define TIMEOUT ((unsigned long)INTERVAL * 500UL)
#define SCALE (0.001f * 0.5f * SND_VEL)

#define _EMA_ALPHA 0.9f
#define N 10

float samples[N];
int sample_index = 0;
int sample_count = 0;

float dist_ema = 0.0f;
bool ema_initialized = false;

unsigned long last_sampling_time = 0;

float getMedian(float arr[], int len) {
  float temp[N];
  for (int i = 0; i < len; ++i) temp[i] = arr[i];
  for (int i = 0; i < len - 1; ++i) {
    for (int j = 0; j < len - 1 - i; ++j) {
      if (temp[j] > temp[j + 1]) {
        float t = temp[j];
        temp[j] = temp[j + 1];
        temp[j + 1] = t;
      }
    }
  }
  if (len % 2 == 1) return temp[len / 2];
  return (temp[len / 2 - 1] + temp[len / 2]) * 0.5f;
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  Serial.begin(57600);
  last_sampling_time = millis();
}

void loop() {
  unsigned long now = millis();
  if ((now - last_sampling_time) < (unsigned long)INTERVAL) return;
  last_sampling_time = now;

  float dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  samples[sample_index] = dist_raw;
  sample_index++;
  if (sample_index >= N) sample_index = 0;
  if (sample_count < N) sample_count++;

  float dist_median = getMedian(samples, sample_count);

  if (!ema_initialized) {
    dist_ema = dist_raw;
    ema_initialized = true;
  } else {
    dist_ema = _EMA_ALPHA * dist_raw + (1.0f - _EMA_ALPHA) * dist_ema;
  }

  Serial.print("raw:");  Serial.print(dist_raw);
  Serial.print(",ema:"); Serial.print(dist_ema);
  Serial.print(",median:"); Serial.println(dist_median);

  if ((dist_raw < _DIST_MIN) || (dist_raw > _DIST_MAX)) digitalWrite(PIN_LED, HIGH);
  else digitalWrite(PIN_LED, LOW);
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  unsigned long pulse = pulseIn(ECHO, HIGH, TIMEOUT);
  return (float)pulse * SCALE;
}
