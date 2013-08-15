#include <AFMotor.h>
 
/* 
Graccefully spin up, run, spin down and reverse a motor
- accelerate up to `maxSpd`
- hold it there for `duration` ms
- decelerate to `minsSpd`
- pause for `rest` ms
- flip the direction, rinse & repeat.
- Motor speed 26 is unloaded min spd... no motion at 26.
*/

// create motor #2, 64KHz pwm
//AF_DCMotor motor(2, MOTOR12_64KHZ); 
AF_DCMotor motor(2, MOTOR12_1KHZ); 

int minSpd = 25;
int maxSpd = 255;
int spd = minSpd + 1;
int acceleration = 1;
int loopDelay = 50;
int pause = 10000;
unsigned long duration = 1000;
unsigned long accelDuration = 5000;
unsigned long accelStart;
unsigned long time;
unsigned long lastTime;
unsigned long lastLog;
unsigned long diffTime;
unsigned long maxStarted;

/*
A0 = digital pin 14
A1 = digital pin 15
A2 = digital pin 16
A3 = digital pin 17
A4 = digital pin 18
A5 = digital pin 19
*/
int topSwitchPin = 19;   // NOTE: we cannot use any old pins, most are used by the sheild.
int bottomSwitchPin = 17;

int lastSwitch;               // which pin fired last.
unsigned long lastSwitchTime = 0; // when did it fire?

// Initial direction and should we be running. TODO: we need a switch to stop and reset.
boolean running = true;
// FORWARD/UP = 1, BACWARD/DOWN = 2, RELEASE = 4 
int dir = FORWARD; 

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  motor.setSpeed(0);            // set the speed to 200/255
  
  pinMode(topSwitchPin, INPUT);
  pinMode(bottomSwitchPin, INPUT);
  
  delay(1000);
  Serial.print("Setup: ");
  logState();
}
 
void loop() {

  if(running){
    
    time = millis();
    
    checkSwitches();
    
    if (spd <= minSpd) {          // At rest.    
      pauseThenReverse();         // Time to pause and reverse.
      
    } else if (spd >= maxSpd) {   // At max speed.
       runAtMax();
       
    } else {                      // accelerating / decelerating      
//      changeSpeed();
//      delay(loopDelay);
      accelerate(accelStart, time);
      if (lastLog == 0 || time - lastLog >= 1000) {
        lastLog = time;
        Serial.print("new speed:" );
        Serial.print(spd);
        Serial.println();
      }      
    }
  }  
}

void accelerate(long start, long now) {
  long t = constrain(now - start, 0, accelDuration);
  if (acceleration == 1){
      spd = map(t, 0, accelDuration, minSpd +1 , maxSpd);
  } else {
      spd = map(t, 0, accelDuration, maxSpd - 1, minSpd);
  }  
  motor.setSpeed(spd);
  motor.run(dir);
}

void changeSpeed(){
  spd = constrain(spd + acceleration, minSpd, maxSpd);
  motor.setSpeed(spd);
  motor.run(dir);
}

// Switches signal that we've gone as far as we can and so force a pause and reverse.
void checkSwitches(){
  checkSwitch(topSwitchPin, BACKWARD);
  checkSwitch(bottomSwitchPin, FORWARD);
  
  int wait = pause + 5000;
  
  if(lastSwitchTime > 0 && time - lastSwitchTime >= wait){
    lastSwitch = 0; // clear the switch guard.
  }
}

// Switch helper method to reduce the boilerplate
void checkSwitch(int pin, int newDirection) {
  int state = digitalRead(pin);

  if (state == 0){
    
    if (pin == lastSwitch){ 
      return; // ignore multiple fires of the same pin...
    } 

    motor.run(RELEASE);
    
    lastSwitch = pin;
    lastSwitchTime = millis();
    
    Serial.print("Switching pin: ");
    Serial.print(pin);
    Serial.print(" new direction: ");
    Serial.print(newDirection);
    Serial.print(" ");
    logState();
    
    pauseThenGo(newDirection); // this delays for `pause`
  }
}

void pauseThenReverse() {
    if (dir == FORWARD) {
      pauseThenGo(BACKWARD);
    } else {
      pauseThenGo(FORWARD);
    }
}

void pauseThenGo(int newDirection) {
  motor.run(RELEASE);
  spd = minSpd;
  
  maxStarted = 0; // clean up just in case.
  
  Serial.print("Stop:     ");
  logState();
  
  dir = newDirection;
  acceleration = 1; // flip to accel.
  
  delay(pause);
  
  changeSpeed();
  
  accelStart = millis();
  
  Serial.print("Go:       ");
  logState();
}

void runAtMax(){
  spd = maxSpd;    
  
  if(maxStarted == 0){     // We just got to maxSpeed so record the startTime.
    maxStarted = millis();
    
    Serial.print("Max start:");
    logState();
  }

  if (time - maxStarted >= duration){  // time to start slowing down.
      maxStarted = 0;
      acceleration = -1;         // flip accel to decel.
      changeSpeed();
      accelStart = time;
      
      Serial.print("Max stop: ");
      logState();          
  }
}

void logState(){
  Serial.print("Time: ");
  lastTime = time;
  diffTime = time - lastTime;
  //prints time since program started
  Serial.print(diffTime);
  Serial.print(" spd:"); 
  Serial.print(spd); 
  Serial.print(" dir:"); 
  Serial.print(dir);  
  Serial.println();
}

