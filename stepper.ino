
#define dirPin 2
#define stepPin 3
#define btnPin 7
#define stepsPerRevolution 200

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
   pinMode(btnPin, INPUT); 
}

void loop() {
  digitalWrite(dirPin, HIGH);
  int state = digitalRead(btnPin);
    if (state == LOW){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(3000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(3000);
    }
}
