/*
   BYJ48 Stepper motor code
   Connect :
   IN1 >> D8
   IN2 >> D9
   IN3 >> D10
   IN4 >> D11
   VCC ... 5V Prefer to use external 5V Source
   Gnd

  */

#define IN1  8
#define IN2  9
#define IN3  10
#define IN4  11
int Steps = 0;
boolean Direction = true;
unsigned long last_time;
unsigned long currentMillis ;
int constStep = 1000;
int closeGateSteps=constStep;
int openGateSteps=constStep;
long time;

//Proximity Sensor
const int proximitySensor = 7;

//Infra sensor
#define sensor A0 // Sharp IR GP2Y0A41SK0F (4-30cm, analog)


//
boolean carIsNear = false;
boolean firstTimeStart = true;
void setup()
{
Serial.begin(115200);
pinMode(IN1, OUTPUT); 
pinMode(IN2, OUTPUT); 
pinMode(IN3, OUTPUT); 
pinMode(IN4, OUTPUT); 
pinMode(13, OUTPUT);
pinMode(12, OUTPUT);

}
void loop()
{
  if (firstTimeStart) {
    delay(4000);
    firstTimeStart = false;
  }
  
  boolean carIsNear = isCarNear();
  if (carIsNear ) {
    //Turn on red light
    Serial.println("Car is near");
    digitalWrite(13,HIGH);
    delay(100);
    //Open the gate
    gateControl(openGateSteps);
    openGateSteps = 0;
    closeGateSteps = constStep;
    Direction=!Direction;
  } else {
    //Turn off the red light
    digitalWrite(13,LOW);
    //Check if car is in the park == close to the infra sensor
    boolean carInThePark = isCarInThePark();
    if (carInThePark) {
      //Turn on the green light
      digitalWrite(12,HIGH);
      //Close the gate
          delay(100);
      gateControl(closeGateSteps);
      closeGateSteps = 0;
      openGateSteps = constStep;
      Direction=!Direction;
      
    } else {
      //Turn off the green light
      digitalWrite(12,LOW);
    }
    
  }

}

void stepper(int xw){
  for (int x=0;x<xw;x++){
switch(Steps){
   case 0:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, HIGH);
   break; 
   case 1:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, HIGH);
   break; 
   case 2:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, LOW);
   break; 
   case 3:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, LOW);
   break; 
   case 4:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   break; 
   case 5:
     digitalWrite(IN1, HIGH); 
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   break; 
     case 6:
     digitalWrite(IN1, HIGH); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   break; 
   case 7:
     digitalWrite(IN1, HIGH); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, HIGH);
   break; 
   default:
     digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);
   break; 
}
SetDirection();
}
} 
void SetDirection(){
if(Direction==1){ Steps++;}
if(Direction==0){ Steps--; }
if(Steps>7){Steps=0;}
if(Steps<0){Steps=7; }
}

long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are 73.746
  // microseconds per inch (i.e. sound travels at 1130 feet per second).
  // This gives the distance travelled by the ping, outbound and return,
  // so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}



// My functions

boolean isCarNear() {
    //Proximity sensor
  long duration, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a ensorshort LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(proximitySensor, OUTPUT);
  digitalWrite(proximitySensor, LOW);
  delayMicroseconds(2);
  digitalWrite(proximitySensor, HIGH);
  delayMicroseconds(5);
  digitalWrite(proximitySensor, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH pulse
  // whose duration is the time (in microseconds) from the sending of the ping
  // to the reception of its echo off of an object.
  pinMode(proximitySensor, INPUT);
  duration = pulseIn(proximitySensor, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);


  delay(100);
  Serial.println("distance from car to proximity sensor");
    Serial.println(cm);
  Serial.println("cm");
  if (cm < 3 ) {
    return true;  
  }
  
  return false;
}

boolean isCarInThePark() {
  // Infra sensor
 // 5v
  float volts = analogRead(sensor)*0.0048828125;  // value from sensor * (5/1024)
  int distance = 60.374*pow(volts, -1.16); // worked out from datasheet graph
  delay(100); // slow down serial port 
  
  if (distance > 30){
    return false;
    Serial.println("Car is far away from the gate");
    Serial.println(distance);   // print the distance
  } else {
    Serial.println("Should not close the gate");
    Serial.println(distance);   // print the distance
  return true;
  }
}

void gateControl(int steps_left) {
        while (steps_left>0) {
         currentMillis = micros();
      if(currentMillis-last_time>=1000){
        stepper(1); 
        time=time+micros()-last_time;
        last_time=micros();
        steps_left--;
      }
     }
}

