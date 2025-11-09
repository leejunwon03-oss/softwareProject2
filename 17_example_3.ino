#include <Servo.h>


#define PIN_IR     0           
#define PIN_LED    9
#define PIN_SERVO 10

#define _DUTY_MIN 700   
#define _DUTY_NEU 1550  
#define _DUTY_MAX 2400 

#define _DIST_MIN  100.0   
#define _DIST_MAX  250.0   


#define EMA_ALPHA  0.01      

#define LOOP_INTERVAL 20   

Servo myservo;
unsigned long last_loop_time;    

float dist_prev = _DIST_MIN; 
float dist_ema = _DIST_MIN;

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  
  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_NEU);
  

  Serial.begin(1000000);    
  last_loop_time = millis();
}

void loop()
{
  unsigned long time_curr = millis();
  int duty;
  float a_value, dist_raw;

 
  if (time_curr < (last_loop_time + LOOP_INTERVAL))
    return;
  last_loop_time += LOOP_INTERVAL;

  a_value = analogRead(PIN_IR);
 
  dist_raw = ((6762.0 / (a_value - 9.0)) - 4.0) * 10.0;

  
  if (dist_raw >= _DIST_MIN && dist_raw <= _DIST_MAX) {
    digitalWrite(PIN_LED, HIGH); 
    dist_prev = dist_raw;        
  } else {
    digitalWrite(PIN_LED, LOW);  
    
  }
 
 
  dist_ema = EMA_ALPHA * dist_prev + (1.0 - EMA_ALPHA) * dist_ema;
      
  
  float duty_f = _DUTY_MIN + (dist_ema - _DIST_MIN) * (_DUTY_MAX - _DUTY_MIN) / (_DIST_MAX - _DIST_MIN);
  duty = (int)duty_f;


  if (duty < _DUTY_MIN) duty = _DUTY_MIN;
  if (duty > _DUTY_MAX) duty = _DUTY_MAX;
 
  myservo.writeMicroseconds(duty);


  Serial.print("_DUTY_MIN:");  Serial.print(_DUTY_MIN);
  Serial.print(",_DIST_MIN:");  Serial.print(_DIST_MIN);
  Serial.print(",IR:");        Serial.print(a_value);
  Serial.print(",dist_raw:");  Serial.print(dist_raw);
  Serial.print(",ema:");        Serial.print(dist_ema);
  Serial.print(",servo:");      Serial.print(duty);
  Serial.print(",_DIST_MAX:"); Serial.print(_DIST_MAX);
  Serial.print(",_DUTY_MAX:"); Serial.print(_DUTY_MAX);
  Serial.println("");
}
