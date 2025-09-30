int led = 7;
int duty = 0;
int period = 100;
int repeat;


void set_duty(int d){
  duty = d;
}

void pwm(){
  int on = period*duty/100;
  int off = period - on;

  digitalWrite(led,HIGH);
  delayMicroseconds(on);

  digitalWrite(led,LOW);
  delayMicroseconds(off);
}

void setup(){
  pinMode(led,OUTPUT);
  
  if(period==10000){
    repeat=5;
  }
  else if(period==1000){
    repeat=50;
  }
  else if(period==100){
    repeat=500;
  } 
}

void loop(){
  
  for(int d=0; d<=100; d++){
    set_duty(d);
    for(int j=0;j<repeat;j++){
      pwm();
    }
  }

  for(int d=100; d>=0; d--){
    set_duty(d);
    for(int j=0;j<repeat;j++){
      pwm();
    }
  }
}
