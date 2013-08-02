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

int spd = 1;
int minSpd = 0;
int maxSpd = 255;
int acceleration = 1;
int loopDelay = 30;
int rest = 3000;
int duration = 4000;
int dir = FORWARD;              // FORWARD = 1, BACKEWARD = 2, RELEASE = 4 

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  motor.setSpeed(0);            // set the speed to 200/255
}

void loop() {
  easy();
}

void easy() {
  
  // At rest.    
  if (spd <= minSpd) { 
    acceleration = acceleration * -1; // flip it.
    if (dir == FORWARD){
      dir = BACKWARD;
    } else {
      dir = FORWARD;
    }
    Serial.print("...Rest...");
    Serial.println(spd);
    Serial.println(dir);
    delay(rest);
    
  // At max speed
  } else if (spd >= maxSpd) { 
    spd = maxSpd;
    acceleration = acceleration * -1; // flip it.
    Serial.print("...Max...");
    Serial.println(spd);
    Serial.println(dir);
    delay(duration);
    
  // accelerating / decelerating
  } else { 
    motor.setSpeed(spd);
    motor.run(dir);
  }
  
  spd = constrain(spd + acceleration, minSpd, maxSpd);
  delay(loopDelay);
}

