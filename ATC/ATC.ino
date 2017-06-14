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

void rotateMotor(int dir, int dist){
  //dist: the required number of positions to rotate
  //dir: which direction the motor needs to turn

  int n == 0;   //just to count up

  if (dir == 1){    //clockwise
    digitalWrite(motorDirPin, HIGH)
  }
  else{             //anticlockwise
    digitalWrite(motorDirPin, LOW)
  }

  while (n != dist){
    while ( genevaZero != true){
      digitalWrite(motorSpdPin, HIGH)
      }
    digitalWrite(motorSpdPin, LOW)
    n++;
  }
}

