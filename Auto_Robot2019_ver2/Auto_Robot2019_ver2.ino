//  ****************************
//  ** Line Trace ArduinoUNO  **
//  ** Basic Control PWM      **
//  ** 08/2019 LowTech.com    **
//  ****************************

#define on_course 0b00000110
#define L2        0b00001000
#define L1        0b00000100
#define R1        0b00000010
#define R2        0b00000001
#define FWD       0
#define BACK      255
#define NO        0
#define OK        255

byte sens, all_black;
signed int std_pwm;

void setup() {
  // put your setup code here, to run once:
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);
    pinMode(9, INPUT_PULLUP);
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(12, INPUT);
    pinMode(13, INPUT);
    pinMode(14, INPUT);
    pinMode(15, INPUT);
    pinMode(16, INPUT);
    pinMode(17, INPUT);
    pinMode(18, INPUT);
    pinMode(19, INPUT);
    Serial.begin(9600);
}

void steering(byte dir){  //direction = FWD(0) or BACK(255)
  signed int pwm_L, pwm_R, p_l, p_r;

    if(dir==FWD){
      sens=(PINC>>2)&0b00001111;   //sens : 0b0000(L2)(L1)(R1)(R2)
    }else{ //BACK
      sens=(PINB>>4)&0b00000011;
      sens |=(PINC<<2)&0b00001100; //sens : 0b0000(BL2)(BL1)(BR1)(BR2)
    }
    if(sens==0){  // all White
      p_l=0; p_r=0;  
    }else if(sens==on_course){  // on course
      p_l=0; p_r=0;  
    }else if(sens==L1){
      p_l=-(std_pwm*2)/10;  p_r=(std_pwm*2)/10;   //20%
    }else if(sens==R1){
      p_l=(std_pwm*2)/10;  p_r=-(std_pwm*2)/10;   //20%
    }else if(sens==(L1|L2)){
      p_l=-(std_pwm*4)/10;  p_r=(std_pwm*4)/10;   //40%
    }else if(sens==(R1|R2)){
      p_l=(std_pwm*4)/10;  p_r=-(std_pwm*4)/10;   //40%
    }else if(sens==L2){
      p_l=-(std_pwm*7)/10;  p_r=(std_pwm*7)/10;   //70%
    }else if(sens==R2){
      p_l=(std_pwm*7)/10;  p_r=-(std_pwm*7)/10;   //70%
    }else{
      p_l=0; p_r=0;
    }
    pwm_L = std_pwm + p_l;  //Left 
    pwm_R = std_pwm + p_r;  //Right
    if(pwm_L<=0)pwm_L=0; if(pwm_R<=0)pwm_R=0;
    if(pwm_L>250)pwm_L=250; if(pwm_R>250)pwm_R=250;
    if(dir==FWD){
      OCR0A=pwm_R;
      OCR2B=pwm_L;
    }else{ //BACK
      OCR0B=pwm_R;
      OCR2A=pwm_L;
    }
}

void pwm_on(byte dir){
    TCCR0A = 0;  // Clear timer control register.
    TCCR0B = 0;  // Clear timer control register.
    TCCR2A = 0;  // Clear timer control register.
    TCCR2B = 0;  // Clear timer control register.
  if(dir==FWD){
    TCCR0A = 0b10000011;  //1000: PWM0A output, 11: Fast PWM, TOP=FF
    TCCR0B = 0b00000011;  //011: PSR=64  Period T=(1/16MHz/64)*256=1.024(ms) Freq.=1/T=976Hz
    TCCR2A = 0b00100011;  //0010: PWM2B output, 11: Fast PWM, TOP=FF
    TCCR2B = 0b00000011;  //011: PSR=64  Period T=(1/16MHz/64)*256=1.024(ms) Freq.=1/T=976Hz
  }else{
    TCCR0A = 0b00100011;  //0010: PWM0B output, 11: Fast PWM, TOP=FF
    TCCR0B = 0b00000011;  //011: PSR=64  Period T=(1/16MHz/64)*256=1.024(ms) Freq.=1/T=976Hz
    TCCR2A = 0b10000011;  //1000: PWM2A output, 11: Fast PWM, TOP=FF
    TCCR2B = 0b00000011;  //011: PSR=64  Period T=(1/16MHz/64)*256=1.024(ms) Freq.=1/T=976Hz
  }  
    OCR0A = 0;  // PWM0A=0
    OCR0B = 0;  // PWM0B=0
    OCR2A = 0;  // PWM2A=0
    OCR2B = 0;  // PWM2B=0
}

void pwm_off(){
  OCR0A=0;
  OCR0B=0;
  OCR2A=0;
  OCR2B=0;
  TCCR0A = 0;  // Clear timer control register.
  TCCR0B = 0;  // Clear timer control register.
  TCCR0A = 0b00000011;  //0000: PWM0A,0B off, 11: Fast PWM, TOP=FF
  TCCR0B = 0b00000011;  //011: PSR=64  Period T=(1/16MHz/64)*256=1.024(ms) Freq.=1/T=976Hz
  TCCR2A = 0;  // Clear timer control register.
  TCCR2B = 0;  // Clear timer control register.
  TCCR2A = 0b00000011;  //0000: PWM2A,2B off, 11: Fast PWM, TOP=FF
  TCCR2B = 0b00000011;  //011: PSR=64  Period T=(1/16MHz/64)*256=1.024(ms) Freq.=1/T=976Hz
  digitalWrite(6,LOW); 
  digitalWrite(5,LOW); 
  digitalWrite(11,LOW); 
  digitalWrite(3,LOW);
}

void go_run(byte dir){
  unsigned int blk_count, delay_stop;

  all_black=NO;     // NO(0)
  blk_count=0;
  delay_stop=800;  // ***ADJ.  delayed stop
  std_pwm=96;       // ***ADJ.  set speed
  
  pwm_on(dir);
  while(1){
    delayMicroseconds(1000);  // 1ms
    steering(dir);
    if(sens==0b00001111){   //all black?
      blk_count++;
      if(blk_count>20) all_black=OK; //***ADJ.  OK(255)
    }else{
      blk_count=0;  
    }
    if(all_black==OK){
      if(delay_stop==0)break;
      delay_stop--;
    } 
  }
  pwm_off();
}

void work(){
  delay(200);
  digitalWrite(4,HIGH);   // M1 ON(CW)
  digitalWrite(2,LOW);
  delay(1000);   // while(digitalRead(9)==0); delay(200);
//*********************************************************  
  while(digitalRead(9));  // waiting for Limit_SW = ON(0)
//*********************************************************
  digitalWrite(4,LOW);    // M1 OFF
  
  delay(200);
  digitalWrite(10,HIGH);   // M2 ON(CW)
  delay(1000);             // ***ADJ. 1000?
  digitalWrite(10,LOW);    // M2 OFF
  delay(100);

  digitalWrite(4,LOW);    // M1 ON(CCW)
  digitalWrite(2,HIGH);
  delay(1000);   // while(digitalRead(9)==0); delay(200);
//*********************************************************  
  while(digitalRead(9));  // waiting for Limit_SW = ON(0)
//*********************************************************  
  digitalWrite(2,LOW);    // M1 OFF
  delay(100);  
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long u_time;
  byte  data_num, count1, count2, count3;

  count1=0; count2=0; count3=0;
  while(1){
    while(1){
      if((PIND&bit(7))==bit(7))break;   // if pin7 is Low, waiting for High
    }
    while(1){
      if((PIND&bit(7))==0)break;        // waiting for Low 
    }
    u_time=micros();                    // get the present time 
    while(1){
      if((PIND&bit(7))==bit(7))break;   // waiting for High
    }
    u_time=micros()-u_time;             // get the time of Low level period
    
    if((u_time>1600)&&(u_time<2400)){   // 2000us ?
      count2=0; count3=0;
      count1++; 
      if(count1>=3){          // continued 3 times correct? 
        data_num=1;           // if yes, it's ok go out to nect.  
        break;
      }
    }else if((u_time>3500)&&(u_time<4500)){   // 4000us ?
      count1=0; count3=0;
      count2++; 
      if(count2>=3){
        data_num=2; 
        break;
      }
    }else if((u_time>5500)&&(u_time<6500)){   // 6000us ?
      count1=0; count2=0;
      count3++;
      if(count3>=3){
        data_num=3;
        break;
      }    
    }else{
      count1=0; count2=0; count3=0;
    }  
  }
  Serial.print(u_time);
  Serial.println(", ");
  Serial.print(data_num);
  Serial.println("\r");
  
  if(data_num==1){   //FWD run
    go_run(FWD);  
    work();
    go_run(BACK);  
  }
  if(data_num==2){   //BACK run
    go_run(BACK);  
    work();
    go_run(FWD);  
  }
  if(data_num==3){   //FWD run 2 times
    go_run(FWD);  
    go_run(FWD);  
    work();
    go_run(BACK);  
    go_run(BACK);  
  }
}
