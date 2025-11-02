#include <Servo.h> 

#define PIN_SERVO 10     
#define _DUTY_MIN 700     // 서보의 0도 위치에 해당
#define _DUTY_NEU 1550    // 서보의 90도 위치에 해당
#define _DUTY_MAX 2400    // 서보의 180도 위치에 해당
#define _POS_START (_DUTY_MIN + 100) // 서보 구동 시작 위치 (0도 근처)
#define _POS_END    (_DUTY_MAX - 100) // 서보 구동 최종 위치 (180도 근처)
#define INTERVAL 20     // loop() 함수의 업데이트 주기 간격 


unsigned long last_sampling_time; // 마지막으로 서보를 업데이트한 시각을 저장

Servo myservo; // 서보 모터를 제어하기 위한 Servo 객체를 생성

float duty_change_per_interval; // 20ms 간격 동안 듀티가 변화할 양
float duty_target; // 서보가 도달해야 할 목표 펄스 폭입니다.
float duty_curr;   // 서보의 현재 펄스 폭입니다. 

float current_servo_speed = 0.0; // 현재 설정된 서보의 각속도 값

int experiment_state = 0;           // 현재 실험의 상태 (1:실험1, 2:실험2, 3:완료)를 추적
unsigned long experiment_start_time = 0; // 각 실험이 시작된 시각을 저장하여 경과 시간을 측정



void setup() {
  myservo.attach(PIN_SERVO); 
  
  duty_target = duty_curr = _POS_START; // 목표와 현재 듀티를 시작 위치로 초기화
  myservo.writeMicroseconds((int)duty_curr); // 서보를 초기 위치로 이동
  


  // [실험 1 시작]
  experiment_state = 1;  // 실험 상태를 1로 설정
  experiment_start_time = millis();  // 현재 시간을 실험 1의 시작 시간으로 기록
  
  current_servo_speed = 3.0;  // 실험 1의 속도를 3.0로 설정
  
  // duty_change_per_interval 계산 (등속 이동을 위한 20ms당 듀티 변화량 계산)
 
  duty_change_per_interval = 
    (float)(_DUTY_MAX - _DUTY_MIN) * (current_servo_speed / 180.0) * (INTERVAL / 1000.0);
  
  duty_target = _POS_END; // 실험 1의 목표를 _POS_END로 설정
  
  last_sampling_time = 0; // 업데이트 시간을 0으로 초기화
  
 
}



void loop() {
  
  if (millis() < (last_sampling_time + INTERVAL)) // 현재 시간이 다음 업데이트 시간보다 작으면
    return; // loop를 즉시 종료하고 대기
    
  // 등속 이동 
  if (duty_target > duty_curr) { // 목표 듀티가 현재 듀티보다 크면 
    duty_curr += duty_change_per_interval; // 계산된 변화량만큼 듀티를 증가
    if (duty_curr > duty_target)
      duty_curr = duty_target; // 목표 듀티를 초과하지 않도록 보정
  } else { // 목표 듀티가 현재 듀티보다 작으면 (역방향 회전)
    duty_curr -= duty_change_per_interval; // 계산된 변화량만큼 듀티를 감소
    if (duty_curr < duty_target)
      duty_curr = duty_target; // 목표 듀티보다 작아지지 않도록 보정
  }

  // 서보 위치 업데이트
  myservo.writeMicroseconds((int)duty_curr); 

  // [실험 상태] 
  if (experiment_state == 1) { // 현재 실험 1 진행 중인 경우
    
    if (millis() - experiment_start_time >= 60000) { // 60초가 경과했는지 확인
      
      // 실험 2로 전환
      experiment_state = 2; // 상태를 2로 변경
      experiment_start_time = millis(); // 실험 2의 시작 시간을 현재 시각으로 리셋

      // 실험 2 조건 
      current_servo_speed = 0.3; // 속도를 0.3으로 설정
      
      // duty_change_per_interval 재계산 
      duty_change_per_interval = 
        (float)(_DUTY_MAX - _DUTY_MIN) * (current_servo_speed / 180.0) * (INTERVAL / 1000.0);

      duty_target = _DUTY_NEU; // 목표를 중립 위치(_DUTY_NEU)로 변경(90도)
      
    
    }
  } else if (experiment_state == 2) { // 현재 실험 2 진행 중인 경우
    // 실험 2 종료 조건
    if (millis() - experiment_start_time >= 300000) { // 300초(300000ms)가 경과했는지 확인
      
      // 모든 실험 완료
      experiment_state = 3; // 상태를 3(완료)로 변경
      
     
    }
  }

  

 
  last_sampling_time += INTERVAL; // 다음 업데이트 시점을 20ms 후로 설정
}
