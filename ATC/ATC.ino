#define HOME_SENSE_PIN 12
#define GENEVA_SENSE_PIN 13
#define LIFT_CYLINDER_DELAY 5000
#define PIVOT_CYLINDER_DELAY 10000
#define MOTOR_SPEED_PIN 1           //PWM to H-Bridge
#define MOTOR_DIRECTION_PIN 2       //High = Clockwise
#define PIVOT_CYLINDER_PIN 3
#define LIFT_CYLINDER_PIN 4

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

  //toolInCNC = isCNCLoaded();
  if (toolInCNC = true){
    //insert error message that pauses the software until accepted
  }
  homing();
}
void loop() {
  // put your main code here, to run repeatedly:

}

void homing(){
  while (homed != true){
    rotateMotor(1,1); //clockiwise once
    homed = digitalRead(HOME_SENSE_PIN);
  }

  currentPosition = 1;
}

void unloadTool(int reqTool){
  //ask cnc if it's in position and if not make it so
  digitalWrite(LIFT_CYLINDER_PIN, HIGH);       //ATC:LIFT
  delay(LIFT_CYLINDER_DELAY);     
  digitalWrite(PIVOT_CYLINDER_PIN, HIGH);      //ATC:IN
  delay(PIVOT_CYLINDER_DELAY);
  digitalWrite(LIFT_CYLINDER_PIN, LOW);        //ATC:LOWER
  //small delay here?
  //tell cnc to engage the drawbar      //DRAWBAR:ON
  delay(LIFT_CYLINDER_DELAY);       
  //tell cnc to disengage the drawbar   //DRAWBAR:OFF
  
  if(reqTool != 0){
    loadTool(reqTool);
  }
  else{ //if reqTool is 0 then there will be no unload, and ATC is finished
    digitalWrite(PIVOT_CYLINDER_PIN, LOW);
    delay(PIVOT_CYLINDER_DELAY);        //ATC:OUT
  }
}

void loadTool(int reqTool){
   //ask cnc if it's in position and if not make it so
   digitalWrite(PIVOT_CYLINDER_PIN, HIGH);     //ATC:IN
   delay(PIVOT_CYLINDER_DELAY);
   rotatePath(reqTool);                 //ATC:ROTATE
   //tell cnc to engage the drawbar     //DRAWBAR:ON
   digitalWrite(LIFT_CYLINDER_PIN, HIGH);      //ATC:LIFT
   delay(LIFT_CYLINDER_DELAY);
   //tell cnc to disengage the drawbar  //DRAWBAR:OFF
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

