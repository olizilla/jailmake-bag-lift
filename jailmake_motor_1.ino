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
 
AF_DCMotor motor(2, MOTOR12_64KHZ); // create motor #2, 64KHz pwm
 
// FORWARD = 1, BACKEWARD = 2, RELEASE = 4 
const int UP = 1;
const int DOWN = 2;
const int STOP = 4;
 
int minSpd = 26;
int maxSpd = 255;
int spd = minSpd + 1;
int acceleration = 1;
int loopDelay = 50;
int pause = 3000;
unsigned long duration = 4000;
unsigned long time;
unsigned long lastTime;
unsigned long diffTime;
unsigned long maxStarted;

int topSwitchPin = 13;   // NOTE: we cannot use any old pins, most are used by the sheild.
int bottomSwitchPin = 11;
int lastSwitch;          // which pin fired last.

// INITIAL DIRECTION AND SHOULD WE BE RUNNING. TODO: we need a switch to stop and reset.
int dir = FORWARD; 
boolean running = true;

void setup() {
  time = millis();
  Serial.begin(9600);           // set up Serial library at 9600 bps
  motor.setSpeed(0);            // set the speed to 200/255
  
  pinMode(topSwitchPin, INPUT);
  pinMode(bottomSwitchPin, INPUT);
  
  delay(2000);
  Serial.print("Setup: ");
  logState();
}
 
void loop() {

  if(running){
    
    checkSwitches();
    
    if (spd <= minSpd) {          // At rest.    
      pauseThenReverse();         // Time to pause and reverse.
      
    } else if (spd >= maxSpd) {   // At max speed.
       runAtMax();
       
    } else {                      // accelerating / decelerating      
      changeSpeed();
      delay(loopDelay);
    }
  }  
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
}

// Switch helper method to reduce the boilerplate
void checkSwitch(int pin, int newDirection) {
  int state = digitalRead(pin);

  if (state == 0){
    
//    if (pin == lastSwitch){ 
//      return; // ignore multiple fires of the same pin...
//    } 

    motor.run(STOP);
    
    lastSwitch = pin;
    
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
  motor.run(STOP);
  
  maxStarted = 0; // clean up just in case.
  
  Serial.print("Stop:     ");
  logState();
  
  dir = newDirection;
  acceleration = acceleration * -1; // flip accel to decel.
  
  delay(pause);
  
  changeSpeed();
  motor.run(dir);
  
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
  
  unsigned long currentMillis = millis();

  if (currentMillis - maxStarted >= duration){  // time to start slowing down.
      maxStarted = 0;
      acceleration = acceleration * -1;         // flip accel to decel.
      changeSpeed();

      Serial.print("Max stop: ");
      logState();          
  }
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
  Serial.print(" dir:"); 
  Serial.print(dir);  
  Serial.println();
}

