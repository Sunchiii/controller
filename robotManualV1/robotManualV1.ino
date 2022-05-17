#include <Arduino.h>
#include <PS2X_lib.h>  //for v1.6
#include <Servo.h>

int Time = 10;
int n = Time;
int Time2 = 65;
int n2 = Time2;

#define PS2RCVR 30
// left wheel
#define ENA 2
#define OUT1 3
#define OUT2 4

// right wheel
#define ENB 5
#define OUT3 7
#define OUT4 6

// left && right
#define EN 8
#define L 24
#define R 26


/******************************************************************
 * set pins connected to PS2 controller:
 *   - 1e column: original 
 *   - 2e colmun: Stef?
 * replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT        13
#define PS2_CMD        11
#define PS2_SEL        10
#define PS2_CLK        12

/******************************************************************
 * select modes of PS2 controller:
 *   - pressures = analog reading of push-butttons 
 *   - rumble    = motor rumbling
 * uncomment 1 of the lines for each mode selection
 ******************************************************************/

#define pressures   false
#define rumble      false

int pos;// variable to rotate the servo
int add;

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning 
//you must always either restart your Arduino after you connect the controller, 
//or call config_gamepad(pins) again after connecting the controller.

int error = 0;
byte type = 0;
byte vibrate = 0;

void setup() {
  
  Serial.begin(57600);
  pinMode(PS2RCVR, OUTPUT);
  pinMode(OUT1, OUTPUT);
  pinMode(OUT2, OUTPUT);
  pinMode(OUT3, OUTPUT);
  pinMode(OUT4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(L, OUTPUT);
  pinMode(R, OUTPUT);
  pinMode(EN, OUTPUT);

  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
  analogWrite(EN, 200);

  digitalWrite(PS2RCVR, LOW);

  delay(100);

  digitalWrite(PS2RCVR, HIGH);
  
  delay(5000);  //added delay to give wireless ps2 module some time to startup, before configuring it

  //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************
  
  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  
  if(error == 0){
    Serial.print("Found Controller, configured successful ");
    Serial.print("pressures = ");
  if (pressures)
    Serial.println("true ");
  else
    Serial.println("false");
  Serial.print("rumble = ");
  if (rumble)
    Serial.println("true)");
  else
    Serial.println("false");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("Note: Go to www.billporter.info for updates and to report bugs.");
  }  
  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
  
  
}

void loop() {

  /* You must Read Gamepad to get new values and set vibration values
     ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
     if you don't enable the rumble, use ps2x.read_gamepad(); with no values
     You should call this at least once a second
   */  
  if(error == 1) //skip loop if no controller found
    return;

  ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed



    if(ps2x.Button(PSB_START)) {
        Serial.println("START");
    }

    if(ps2x.Button(PSB_SELECT)) {
        Serial.println("SELECT");
    }

    if(ps2x.Button(PSB_PAD_UP)) {
        Serial.println("UP");
        moveForward();
    }else if(ps2x.Button(PSB_PAD_DOWN)) {
        Serial.println("DOWN");
        moveBackward();
    }else if(ps2x.Button(PSB_PAD_LEFT)) {
        Serial.println("LEFT");
        turnLeft();
    }else if(ps2x.Button(PSB_PAD_RIGHT) ) {
        Serial.println("RIGHT");
        turnRight();

    }else if(ps2x.Button(PSB_SQUARE)) {
        Serial.println("[]");
        Left();
    }else if(ps2x.Button(PSB_CIRCLE)) {
        Serial.println("O");
        Right();
        
    }else{
      moveStop();
    }

//============================
    
  delay(15);

}


void turnLeft() {
  digitalWrite(OUT1, LOW);
  digitalWrite(OUT2, HIGH);

  digitalWrite(OUT3, HIGH);
  digitalWrite(OUT4, LOW);
}

void turnRight() {
  digitalWrite(OUT1, HIGH);
  digitalWrite(OUT2, LOW);

  digitalWrite(OUT3, LOW);
  digitalWrite(OUT4, HIGH);
}

void moveForward() {
  digitalWrite(OUT1, LOW);
  digitalWrite(OUT2, HIGH);

  digitalWrite(OUT3, LOW);
  digitalWrite(OUT4, HIGH);
}

void moveBackward() {
  digitalWrite(OUT1, HIGH);
  digitalWrite(OUT2, LOW);

  digitalWrite(OUT3, HIGH);
  digitalWrite(OUT4, LOW);
}

void moveStop() {
  digitalWrite(OUT1, LOW);
  digitalWrite(OUT2, LOW);
  
  digitalWrite(OUT3, LOW);
  digitalWrite(OUT4, LOW);

  digitalWrite(L, LOW);
  digitalWrite(R, LOW);
}

void Left() {
  digitalWrite(L, LOW);
  digitalWrite(R, HIGH);
}

void Right() {
  digitalWrite(L, HIGH);
  digitalWrite(R, LOW);
}
