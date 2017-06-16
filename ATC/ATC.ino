#define LIFT_CYLINDER_DELAY 5000
#define PIVOT_CYLINDER_DELAY 10000

#define HOME_SENSE_PIN 12
#define GENEVA_SENSE_PIN 14
#define MOTOR_SPEED_PIN 1           //PWM to H-Bridge
#define MOTOR_DIRECTION_PIN 2       //High = Clockwise
#define PIVOT_CYLINDER_PIN 3
#define LIFT_CYLINDER_PIN 4

#define TEST_PIN_A 10
#define TEST_PIN_B 13
int incomingByte;
int testModeActive = 0;

int currentPosition;
boolean homed = false;
boolean toolInCNC = false;
boolean genevaLocked = true;

int lastLoaded;       // possibly irrelevant
boolean ATCEngaged;   // possibly N/A
boolean toolExists;   // possibily N/A
boolean stock[] = {0,0,0,0,0,0,0,0};

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_SPEED_PIN, OUTPUT);
  pinMode(MOTOR_DIRECTION_PIN, OUTPUT);
  pinMode(PIVOT_CYLINDER_PIN, OUTPUT);
  pinMode(LIFT_CYLINDER_PIN, OUTPUT);
  
  pinMode(TEST_PIN_A, OUTPUT);
  pinMode(TEST_PIN_B, OUTPUT);
  digitalWrite(TEST_PIN_B, LOW);
  
  //toolInCNC = isCNCLoaded();
  if (toolInCNC = true){
    //insert error message that pauses the software until dealt with
  }
  //homing();
}
void loop() {
  //awaits user input to switch a tool
  //valid inputs are: 0, 1 through 8, and T
  //0 just causes an unload
  //1 through 8 requests a tool which may require unloading first
  //T which sends the user into Test Mode allowing further commands

  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();

    int requestedToolID = 0;

    if (testModeActive == 0){
      //default menu
      switch(incomingByte){
        case 'T':{
          testModeActive = 1;
          digitalWrite(TEST_PIN_B, HIGH);
          break;
        }
        case '0':{
          requestedToolID = 0;
          break;
        }
        case '1':{
          requestedToolID = 1;
          break;
        }
        case '2':{
          requestedToolID = 2;
          break;
        }
        case '3':{
          requestedToolID = 3;
          break;
        }
        case '4':{
          requestedToolID = 4;
          break;
        }
        case '5':{
          requestedToolID = 5;
          break;
        }
        case '6':{
          requestedToolID = 6;
          break;
        }
        case '7':{
          requestedToolID = 7;
          break;
        }
        case '8':{
          requestedToolID = 8;
          break;
        }
      }
    }
    else{
      //test menu
      switch(incomingByte){
        case 'X':{
          testModeActive = 0;
          digitalWrite(TEST_PIN_B, LOW);
          break;
        }
        case 'U':{
          digitalWrite(LIFT_CYLINDER_PIN, HIGH);
          delay(LIFT_CYLINDER_DELAY);
          break;
        }
        case 'D':{
          digitalWrite(LIFT_CYLINDER_PIN, LOW);
          delay(LIFT_CYLINDER_DELAY);
           break;
        }
        case 'I':{
          digitalWrite(PIVOT_CYLINDER_PIN, HIGH);
          delay(PIVOT_CYLINDER_DELAY);
          break;
          }
        case 'O':{
          digitalWrite(PIVOT_CYLINDER_PIN, LOW);
          delay(PIVOT_CYLINDER_DELAY);
          break;
        }
        case 'R':{
          rotateGenevaClockwiseOnce();
          break;
        }
        case 'L':{
          rotateGenevaAnticlockwiseOnce();
          break;
        }
      }
    }
    switchTool(requestedToolID);
  }
}

void testMode(){
  //await user input
  //X returns the user to prior menu
  //U commands the atc to lift the current tool UP
  //D commands the atc to lower the current tool DOWN
  //I commands the atc to move IN and engage with the CNC
  //O commands the atc to move OUT and disengage with the CNC
  //R commands the geneva to rotate clockwise once
  //L commands the geneva to rotate anti-clockwise once
}

void homing(){
  while (homed != true){
    rotateGenevaClockwiseOnce();
    homed = digitalRead(HOME_SENSE_PIN);
  }
  currentPosition = 1;
}

void rotateGenevaClockwiseOnce(){
  rotateMotor(1,1); 
  //(clockwise = 1, distance = 1)
}

void rotateGenevaAnticlockwiseOnce(){
  rotateMotor(0,1);
  //(anticlockwise = 0, distance = 1)
}

void switchTool(int reqTool){
  //if the CNC has a tool in, then the CNC needs to be unloaded before a new tool can be loaded up
  if (toolInCNC == true){
    unloadTool(reqTool);
  }
  loadTool(reqTool);
}

void unloadTool(int reqTool){
  //ask cnc if it's in position and if not make it so
  digitalWrite(LIFT_CYLINDER_PIN, HIGH);     //ATC:LIFT
  delay(LIFT_CYLINDER_DELAY);     
  digitalWrite(PIVOT_CYLINDER_PIN, HIGH);    //ATC:IN
  delay(PIVOT_CYLINDER_DELAY);
  digitalWrite(LIFT_CYLINDER_PIN, LOW);     //ATC:LOWER
  //small delay here?
  //tell cnc to engage the drawbar          //DRAWBAR:ON
  delay(LIFT_CYLINDER_DELAY);       
  //tell cnc to disengage the drawbar       //DRAWBAR:OFF

  //if required tool is ID 0 then only an unload is required, thus the ATC will not want to remain engaged
  if(reqTool == 0){
    digitalWrite(PIVOT_CYLINDER_PIN, LOW);
    delay(PIVOT_CYLINDER_DELAY);            //ATC:OUT
  }
}

void loadTool(int reqTool){
   //ask cnc if it's in position and if not make it so
   digitalWrite(PIVOT_CYLINDER_PIN, HIGH);     //ATC:IN
   delay(PIVOT_CYLINDER_DELAY);
   rotatePath(reqTool);                        //ATC:ROTATE
   //tell cnc to engage the drawbar            //DRAWBAR:ON
   digitalWrite(LIFT_CYLINDER_PIN, HIGH);      //ATC:LIFT
   delay(LIFT_CYLINDER_DELAY);
   //tell cnc to disengage the drawbar         //DRAWBAR:OFF
   digitalWrite(PIVOT_CYLINDER_PIN, LOW);      //ATC:OUT
   delay(PIVOT_CYLINDER_DELAY);
   digitalWrite(LIFT_CYLINDER_PIN, LOW);       //ATC:LOWER
   delay(LIFT_CYLINDER_DELAY);
}

void rotatePath(int endPos){
  //using the current and desired position, the shortest path needs to be calculated
  
  int startPos = currentPosition;
  int down;
  int up;
  int rotDir;
  int rotDist;

  //tool positions are between 1 and 8 and the shortest path needs to be found
  if (startPos > endPos){
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

  if (rotDir == 1){    //clockwise
    digitalWrite(MOTOR_DIRECTION_PIN, HIGH);
  }
  else{             //anticlockwise
    digitalWrite(MOTOR_DIRECTION_PIN, LOW);
  }

  digitalWrite(MOTOR_SPEED_PIN, HIGH);
  delay(1000);
  //this delay prevents the upcoming code from prematurely counting a successful loop due to detecting the sensor immediately

  int n = 0;
  while (n != rotDist){
    if (genevaLocked == true){
      n++;
      if (rotDir == 1){ //if Direction clockwise position up 1
        currentPosition++;
      }
      else{ //rotDir != 1: if direction anticlockwise position down 1
        currentPosition--;
      }
      
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
  
  digitalWrite(MOTOR_SPEED_PIN, LOW);
}

