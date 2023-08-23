#include <Servo.h>

Servo myservo;
Servo hatchservo;

void setup() {
  myservo.attach(9);
  pinMode(8, INPUT);
  hatchservo.attach(11);
  pinMode(10, INPUT);
}

void loop() {
  if (digitalRead(8) == HIGH) {
    myservo.write(90);
  }
  else if (digitalRead(8) == LOW) {
    myservo.write(0);
  }

  if (digitalRead(10) == LOW) {
    hatchservo.write(90);
  }
  else if (digitalRead(10) == HIGH) {
    hatchservo.write(30);
  }
}