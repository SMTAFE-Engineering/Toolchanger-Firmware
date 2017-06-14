boolean homePosition;
int currentPosition;
boolean toolExists; // possibily N/A
boolean toolInCNC;
int lastLoaded; // possibly irrelevant
boolean stock[] = {0,0,0,0,0,0,0,0); // N/A if toolExists is N/A
boolean ATCEngaged; // possibly N/A

const int motorSpdPin = a;
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
