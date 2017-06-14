int currentPosition;
boolean homePosition;
boolean toolInCNC;
boolean genevaZero;   // needs a better name

int lastLoaded;       // possibly irrelevant
boolean ATCEngaged;   // possibly N/A
boolean toolExists;   // possibily N/A
boolean stock[] = {0,0,0,0,0,0,0,0);

const int motorSpdPin = a;    //PWM to H-Bridge
const int motorDirPin = b;    //High/On is Clockwise
const int pivotCylPin = c;
const int liftCylPin = d;

void setup() {
  Serial.begin(9600);
  pinMode(motorSpdPin, OUTPUT);
  pinMode(motorDirPin, OUTPUT);
  pinMode(pivotCylPin, OUTPUT);
  pinMode(liftCylPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void homing(){
  while (homePosition != true){
    rotateMotor(1,1); //clockiwise once
  }

  currentPosition = 1;
}

void rotatePath(int endPos){
  //using the current and desired position, the shortest path needs to be calculated
  
  int startPos = currentPosition;
  int down;
  int up;
  int rotDir;
  int rotDist;

  //tool positions are between 1 and 8 and the shortest path needs to be found
  if (startPos > EndPos ){
    down = startPos - endPos;
    up = endPos + 8 - startPos;
  }
  else{ //ie endPos > startPos
    up = endPos - startPos;
    down = startPos + 8 - endPos;
  }

  //up equates to clockwise and thus the smaller determines the ideal direction
  if (up < down){
    rotDir = 1; //clockwise
    rotDist = up;
  }
  else{ //ie down < up
    rotDir = 0; //anticlock
    rotDist = down;
  }
  
  rotateMotor(rotDir, rotDist);
}

void rotateMotor(int rotDir, int rotDist){
  //this function commands the motor to move the geneva in the desired direction, the desired number of rotations
  //rotDist: the required number of positions to rotate on the geneva
  //rotDir: which direction the motor needs to turn the geneva

  int n == 0;   //just to count up

  if (rotDir == 1){    //clockwise
    digitalWrite(motorDirPin, HIGH)
  }
  else{             //anticlockwise
    digitalWrite(motorDirPin, LOW)
  }

  digitalWrite(motorSpdPin, HIGH);
  delay(1000);
  //this delay prevents the upcoming code from prematurely counting a successful loop due to detecting the sensor immediately
  
  while (n != rotDist){
    if (genevaZero == true){
      n++;
      currentPosition = currentPosition + (rotDir * 2 - 1);
      //rotDir is 1 or 0 for clockwise or anticlockwise and the currentposition must move +1 or -1
      //by having rotDir doubled minus one, the values 1 and 0 become 1 and -1 respectively
     
      if (currentPosition == 9){
      currentPosition = 1; 
      }
      if (currentPosition == 0){
      currentPosition = 8;
      }
      //loop constraint, when position becomes 0 or 9 it is correctly adjusted to 8 or 1
      
      if (n != rotDist){
        delay(1000);
      }
      //delay is to prevent counting more than once each time the sensor is detected
      //the if statement prevents the motor overshooting on the final loop because the motor stop is outside the loop
    }
  }
  
  digitalWrite(motorSpdPin, LOW);
}

