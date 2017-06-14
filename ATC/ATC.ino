int currentPosition;
boolean homePosition;
boolean toolInCNC;

int lastLoaded;       // possibly irrelevant
boolean ATCEngaged;   // possibly N/A
boolean toolExists;   // possibily N/A
boolean stock[] = {0,0,0,0,0,0,0,0);

const int motorSpdPin = a;    //PWM to H-Bridge
const int motorDirPin = b;
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
