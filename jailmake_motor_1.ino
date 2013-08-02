#include <AFMotor.h>
 
/* 
Graccefully spin up, run, spin down and reverse a motor
- accelerate up to `maxSpd`
- hold it there for `duration` ms
- decelerate to `minsSpd`
- pause for `rest` ms
- flip the direction, rinse & repeat.
*/
 
AF_DCMotor motor(2, MOTOR12_64KHZ); // create motor #2, 64KHz pwm
 
int spd = 0;
int minSpd = 0;
int maxSpd = 255;
int acceleration = 1;
int loopDelay = 50;
int rest = 3000;
int duration = 4000;
unsigned long time;
unsigned long lastTime;
unsigned long diffTime;

int bottomSwitchPin = 3;
int topSwitchPin = 6;

//boolean running = false;
boolean running = true;

// FORWARD = 1, BACKEWARD = 2, RELEASE = 4 
int dir = BACKWARD; 
//int dir = FORWARD; 
 
void setup() {
  time = millis();
  Serial.begin(9600);           // set up Serial library at 9600 bps
  motor.setSpeed(0);            // set the speed to 200/255
  logState();
  
  pinMode(topSwitchPin, INPUT);
  pinMode(bottomSwitchPin, INPUT);
}
 
void loop() {
  if(running){
//     easy();
    getSwitches();
    
  }  
}

void getSwitches() {
  int bottomState = digitalRead(bottomSwitchPin);
  int topState = digitalRead(topSwitchPin);

  Serial.print("bot: ");
  Serial.print(bottomState);
  Serial.print(" top: ");
  Serial.print(topState);
  Serial.println();
  
  delay(500);
}

void stopPauseReverse() {
    Serial.print("Stop:     ");
    logState();
    acceleration = acceleration * -1; // flip it.
    if (dir == FORWARD){
      dir = BACKWARD;
    } else {
      dir = FORWARD;
    }
    delay(rest);
    
    motor.run(dir);
    Serial.print("Start:    ");
    logState();
}

void runAtMax(){
    spd = maxSpd;
    acceleration = acceleration * -1; // flip it.
    Serial.print("Max start:");
    logState();
    delay(duration);
    Serial.print("Max stop: ");
    logState();
}
 
void easy() {
  
  // At rest.    
  if (spd <= minSpd) {
    stopPauseReverse();
    
  // At max speed
  } else if (spd >= maxSpd) { 
    runAtMax();
    
  // accelerating / decelerating
  } else { 
    motor.setSpeed(spd);
//    motor.run(dir);
  }
  
  spd = constrain(spd + acceleration, minSpd, maxSpd);
  delay(loopDelay);
}

void logState(){
  Serial.print("Time: ");
  lastTime = time;
  time = millis();
  diffTime = time - lastTime;
  //prints time since program started
  Serial.print(diffTime);
  Serial.print(" spd:"); 
  Serial.print(spd); 
  Serial.println();
}

