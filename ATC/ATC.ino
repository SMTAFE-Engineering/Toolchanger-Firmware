#define LIFT_CYLINDER_DELAY 5000
#define PIVOT_CYLINDER_DELAY 10000
#define WITH_DRAWBAR true
#define WITHOUT_DRAWBAR false

#define TOOL_UP_MESSAGE "Lifting tool"
#define TOOL_DOWN_MESSAGE "Lowering tool"
#define ATC_IN_MESSAGE "Engaging ATC"
#define ATC_OUT_MESSAGE "Disengaging ATC"
#define DRAWBAR_ON_MESSAGE "Drawbar Active"
#define DRAWBAR_OFF_MESSAGE "Drawbar Inactive"
#define STEP_DONE_MESSAGE "Step Complete"

#define HOME_SENSE_PIN 12
#define GENEVA_SENSE_PIN 14
#define MOTOR_SPEED_PIN 9           //PWM to H-Bridge
#define MOTOR_DIRECTION_PIN 8       //High = Clockwise
#define PIVOT_CYLINDER_PIN 7
#define LIFT_CYLINDER_PIN 6
#define CNC_DRAWBAR_PIN 4
#define CNC_LOADED_PIN 3

#define TEST_PIN 13
int incomingByte;
int testModeActive = 0;

int currentPosition;
boolean homed = false;
boolean toolInCNC = false;
boolean genevaLocked = false;
boolean automateDrawbar = true;
int moveClockwise = 0;
int moveAnticlockwise = 1;

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
  pinMode(TEST_PIN, OUTPUT);
  pinMode(CNC_DRAWBAR_PIN, OUTPUT);
  pinMode(GENEVA_SENSE_PIN, INPUT);
  pinMode(CNC_LOADED_PIN, INPUT);
  Serial.println("Starting Tool Changer");

  boolean toolInCNC = false;
  toolInCNC = digitalRead(CNC_LOADED_PIN);
  if (toolInCNC = true){
    //insert error message that pauses the software until dealt with
  }
  //homing();
  Serial.println("Enter Desired Tool");
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

    int requestedToolID = 99;

    if (testModeActive == 0){
      //default menu options
      switch(incomingByte){
        case 'T':{
          testModeActive = 1;
          digitalWrite(TEST_PIN, HIGH);
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
      //test menu options
      switch(incomingByte){
        case 'X':{
          testModeActive = 0;
          digitalWrite(TEST_PIN, LOW);
          break;
        }
        case 'U':{
          if (automateDrawbar == true){
            moveToolUp(WITH_DRAWBAR);
          }
          else{
            moveToolUp(WITHOUT_DRAWBAR);
          }
          break;
        }
        case 'D':{
          if (automateDrawbar == true){
            moveToolDown(WITH_DRAWBAR);
          }
          else{
            moveToolDown(WITHOUT_DRAWBAR);
          }
          break;
        }
        case 'I':{
          moveATCIn();
          break;
          }
        case 'O':{
          moveATCOut();
          break;
        }
        case 'R':{
          rotateGenevaClockwiseOnce();
          Serial.println(STEP_DONE_MESSAGE);
          break;
        }
        case 'L':{
          rotateGenevaAnticlockwiseOnce();
          Serial.println(STEP_DONE_MESSAGE);
          break;
        }
        case '1':{
          currentPosition = 1;
          Serial.print("Tool Position ");
          Serial.println(currentPosition);
          break;
        }
        case 'M':{
          if (automateDrawbar == true){
            automateDrawbar = false;
            Serial.println("Drawbar is now set to manual");
          }
          else{
            automateDrawbar = true;
            Serial.println("Drawbar is now set to automatic");
          }
          Serial.println("Drawbar manual only works during testing. Drawbar is always automatic during normal operations.");
        }
      }
    }
    if (requestedToolID >= 0 && requestedToolID <= 8){
      if (requestedToolID != 0){
        Serial.print("Tool ID");
        Serial.print(requestedToolID);
        Serial.println(" requested...");
      }
      else{
        Serial.println("Tool Unload requested...");
      }
      switchTool(requestedToolID);
    }
    //here is the last thing to be done as a result of input, I should be able to put the menu spit here
    if (testModeActive == 0){
      Serial.println("Enter Desired Tool");
    }
    else{ //ie test mode menu
      Serial.println("Test Menu");
      Serial.println("1 - Set Current Position to 1");
      Serial.println("U / D - Move Lift Cylinder Up or Down");
      Serial.println("I / O - Move ATC In or Out");
      Serial.println("R / L - Rotate Tool Clockwise or Anticlockwise Once");
      Serial.print("M - Toggle drawbar automation for testing, currently: ");
      if (automateDrawbar == true){
        Serial.println("on");
      }
      else{
        Serial.println("off");
      }
      Serial.println("X - Leave Test Menu");
    }
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
  Serial.println("ATC Homeded to Tool Position 1");
}

void rotateGenevaClockwiseOnce(){
  rotateMotor(moveClockwise,1); 
  //moveClockwise is a global int variable
}

void rotateGenevaAnticlockwiseOnce(){
  rotateMotor(moveAnticlockwise,1);
  //moveAnticlockwise is a global int variable
}

void switchTool(int reqTool){
  //if the CNC has a tool in, then the CNC needs to be unloaded before a new tool can be loaded up
  if (toolInCNC == true){
    unloadTool(reqTool);
  }
  if (reqTool != 0){
    loadTool(reqTool);
    Serial.print("Tool ID");
    Serial.print(reqTool);
    Serial.println(" is now Loaded");
  }
}

void unloadTool(int reqTool){
  //ask cnc if it's in position and if not make it so
  Serial.print("UNLOADING TOOL ");
  Serial.println(currentPosition);
  moveToolUp(WITHOUT_DRAWBAR);      //lift tool slot up (no drawbar needed)
  moveATCIn();                      //pivot atc machine in to engage
  moveToolDown(WITH_DRAWBAR);       //lower tool slot down using the drawbar to unload the tool
  if(reqTool == 0){                 //if required tool is ID 0 then only an unload is required
    moveATCOut();                   //thus pivot atc machine out to disengage
  }
  toolInCNC = digitalRead(CNC_LOADED_PIN);
  toolInCNC = false; //manual write for testing
  Serial.println("TOOL UNLOAD COMPLETE");
}

void loadTool(int reqTool){
  //ask cnc if it's in position and if not make it so
  Serial.print("LOADING TOOL ");
  Serial.println(reqTool);
  //code missing: if statement to check if PIVOT_CYCLINDER_PIN is already High
  moveATCIn();                    //pivot atc machine in to engage (ideally this will be inside an if statement)
  rotatePath(reqTool);            //rotoate atc via the optimal path to the desired tool
  moveToolUp(WITH_DRAWBAR);       //lift tool slot up using the drawbar to load the tool
  moveATCOut();                   //pivot atc machine out to disengage
  moveToolDown(WITHOUT_DRAWBAR);  //lower tool slot back down (no drawbar needed)
  toolInCNC = digitalRead(CNC_LOADED_PIN);
  toolInCNC = true; //manual write for testing
}

void rotatePath(int endPos){
  //using the current and desired position, the shortest path needs to be calculated
  
  int startPos = currentPosition;
  int down;
  int up;
  int rotDir;
  int rotDist;

  //tool positions are between 1 and 8 and the shortest path needs to be found
  //up equates to clockwise and down equates to anticlockwise
  if (startPos > endPos){
    down = startPos - endPos;
    up = endPos + 8 - startPos;
  }
  else{ //ie endPos > startPos
    up = endPos - startPos;
    down = startPos + 8 - endPos;
  }
  //whichever of up or down is smaller corresponds to the ideal direction to rotate
  if (up < down){
    rotDir = moveClockwise; //global int variable
    rotDist = up;
  }
  else{ //ie down < up
    rotDir = moveAnticlockwise; //global int variable
    rotDist = down;
  }
  
  rotateMotor(rotDir, rotDist);
}

void rotateMotor(int rotDir, int rotDist){
  //this function commands the motor to move the geneva in the desired direction, the desired number of rotations
  //rotDist: the required number of positions to rotate on the geneva
  //rotDir: which direction the motor needs to turn the geneva

  if (rotDir == moveClockwise){
    digitalWrite(MOTOR_DIRECTION_PIN, HIGH);
  }
  else{    //ie anticlockwise
    digitalWrite(MOTOR_DIRECTION_PIN, LOW);
  }
  Serial.println("Rotating ATC");
  analogWrite(MOTOR_SPEED_PIN, 128);                        //turn motor on
  boolean seenPinOnFirstLap = false; //used to ignore the first time the sensor sees the pin at the start and only once
  int n = 0;
  while (n != rotDist){                                       //loop until n = rotDir
    genevaLocked = digitalRead(GENEVA_SENSE_PIN);
    if (genevaLocked != true){                                //IF THE GENEVA SENSOR PIN CAN BE SEEN
      if (seenPinOnFirstLap == false){                        //&& first time seeing pin (which is on startup)
        while (genevaLocked != true){                         //loop for as long as the sensor pin can be seen
          genevaLocked = digitalRead(GENEVA_SENSE_PIN);
        }                                                     //once sensor can't see pin
        seenPinOnFirstLap = true;                             //acknowledge that the first pass is done
        Serial.println("First Pass");
      }
      else{                                                   //&& not the first time seeing pin (this is now the lap ticks)
        while (genevaLocked != true && (n + 1) != rotDist){   //loop for as long as the sensor pin can be seen
          genevaLocked = digitalRead(GENEVA_SENSE_PIN);       //HOWEVER not if this where we are about to stop: ie [n+1=rotDist]
          Serial.println("Waiting to clear sensor");
        }                                                     //once sensor can't see pin OR once seen on last lap ready to stop
        n++;                                                  //increment the loop counter, which will cause a stop on last lap
        incrementCurrentPosition(rotDir);                     //increment the current position as per the direction rotated
        Serial.println("Lap Complete");
      }
    }
  }
  digitalWrite(MOTOR_SPEED_PIN, LOW);                         //turn motor off
}

void incrementCurrentPosition(int dir){
  if (dir == moveClockwise){
    currentPosition++;
  }
  else{ //ie anticlockwise
    currentPosition--;
  }
  if (currentPosition == 9){
    currentPosition = 1;
  }
  if (currentPosition == 0){
    currentPosition = 8;
  }
  Serial.print("Tool Position ");
  Serial.println(currentPosition);
  //loop constraint, when position becomes 0 or 9 it is correctly adjusted to 8 or 1
}

void moveToolUp(boolean drawbarRequired){
  if (drawbarRequired == true){
    digitalWrite(CNC_DRAWBAR_PIN, HIGH);
    Serial.println(DRAWBAR_ON_MESSAGE);
  }
  Serial.println(TOOL_UP_MESSAGE);
  digitalWrite(LIFT_CYLINDER_PIN, HIGH);
  delay(LIFT_CYLINDER_DELAY);
  if (drawbarRequired == true){
    digitalWrite(CNC_DRAWBAR_PIN, LOW);
    Serial.println(DRAWBAR_OFF_MESSAGE);
  }
  Serial.println(STEP_DONE_MESSAGE);  
}

void moveToolDown(boolean drawbarRequired){
  Serial.println(TOOL_DOWN_MESSAGE);
  digitalWrite(LIFT_CYLINDER_PIN, LOW);
  if (drawbarRequired == true){
    delay(1000);
    digitalWrite(CNC_DRAWBAR_PIN, HIGH);
    Serial.println(DRAWBAR_ON_MESSAGE);
  }
  delay(LIFT_CYLINDER_DELAY);
  if (drawbarRequired == true){
    digitalWrite(CNC_DRAWBAR_PIN, LOW);
    Serial.println(DRAWBAR_OFF_MESSAGE);
  }
  Serial.println(STEP_DONE_MESSAGE);
}

void moveATCIn(){
  Serial.println(ATC_IN_MESSAGE);
  digitalWrite(PIVOT_CYLINDER_PIN, HIGH);
  delay(PIVOT_CYLINDER_DELAY);
  Serial.println(STEP_DONE_MESSAGE);
}

void moveATCOut(){
  Serial.println(ATC_OUT_MESSAGE);
  digitalWrite(PIVOT_CYLINDER_PIN, LOW);
  delay(PIVOT_CYLINDER_DELAY);
  Serial.println(STEP_DONE_MESSAGE);
}
