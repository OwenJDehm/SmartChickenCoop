// Temperature Regualtion:  COMPLETE
// Security System:         COMPLETE
// Door Control:            COMPLETE
// Feed Dispense:           COMPLETE
// Fire Alarm               COMPLETE


// Controls the temperature of the Coop depending on the mode it is set in

#include "Particle.h" // adds cloud functionality to the code

// The two modes are Incubation and AC, which are controlled by the cloud

#define Regulate 0
#define Incubate 1
#define NullMotion 6
  int mode = Regulate; // want the Coop to initially just cool and prevent overheating, Incubtaion is only required for eggs or chicks

// Door

#define Close 2
#define Open 3
  int Doormode = Close; // want the door of the coop to initially be closed

// Security

#define Arm 4
#define deArm 5
  int Securitymode = deArm; // want security initially off

// Regulation Pins:
int hatchPIN = D3;
int pos = 0;

int tempPIN = A1;
double tempC = 0.0;
int FanPIN = D0;

bool Fan_State = FALSE;

// Incubation Pins:
int motionPIN = D6;
int relayPIN = D2; // controls the heat lamp

int motionState = LOW;
int motionVal = 0;

// Door Pins:

int doorPIN = D4;

// Security Pins:

int motion2PIN = D5;
int motion2State = LOW;
int motionCounter = 0;

// Time functions to avoid delay();
unsigned long int motionCheck;    // 10.0 seconds
unsigned long int tempCheck;      // 1.0 seconds
unsigned long int airCirculation; // 1.0 seconds

unsigned long int motion2Check;   // 10.0 seconds

unsigned long int lastDebounceTime = 0;
unsigned long int debounceDelay = 50;

void setup()
{
  // Regulation Pins
  pinMode(hatchPIN, OUTPUT); // D3
  pinMode(tempPIN, INPUT); // A0 reads analog data
  pinMode(FanPIN, OUTPUT);
  digitalWrite(FanPIN, LOW); // start off
  digitalWrite(hatchPIN, LOW);

  // Incubation Pins:
  pinMode(relayPIN, OUTPUT);
  digitalWrite(relayPIN, LOW);
  pinMode(motionPIN, INPUT); // D3 reads digital data

  // Time functions
  motionCheck = millis() + 10000;     // every 10.0 seconds
  tempCheck = millis() + 1000;        // every 1.0 seconds
  motion2Check = millis() + 10000;    // every 10.0 seconds

  // Security Pins:

  pinMode(motion2PIN, INPUT);

  // Door Control Pins:
  pinMode(doorPIN, OUTPUT);
  digitalWrite(doorPIN, LOW);

  // Cloud Function
  Particle.function("temp_setMode", setTempMode);
  Particle.function("door_setMode", setDoorMode);
  Particle.function("secur_setMode", setSecurMode);

  Particle.variable("motionCounter", motionCounter);
  Particle.variable("tempC", tempC);

  Serial.begin(9600);

}

void loop()
{
  unsigned long int currentTime = millis(); // allows the avoidance of delay();

  double tempDAC = analogRead(tempPIN);
  tempC = (tempDAC-620)/12.4; // converting raw data to celsius
  Particle.publish("tempC");

  // Door Control Code

  if (Doormode == Open) {
    digitalWrite(doorPIN, HIGH);
  }
  else if (Doormode == Close) {
    digitalWrite(doorPIN, LOW);
  }

  // Security Counter

  if (currentTime > motion2Check) {
    if (Securitymode == Arm) {
      if (digitalRead(motion2PIN) == HIGH) {
        motionCounter += 1; // add one to the motion counter
      }
    }
    else if (Securitymode == deArm) {
      motionCounter = 0; // reset so it is 0, when the security is dearmed
    }
    Particle.publish("motionCounter");
    motion2Check += 10000;
  }

  // Temperature_Regulate Code

  if (mode == Regulate) {
    digitalWrite(relayPIN, LOW);
    if (currentTime > tempCheck) {
      if (tempC >= 25 && digitalRead(FanPIN) == LOW) { // turn the fan on
        digitalWrite(hatchPIN, HIGH);
        digitalWrite(FanPIN, HIGH);
      }
      else if (tempC <=23 && digitalRead(FanPIN) == HIGH) { // turn the fan off with a temperature buffer
        digitalWrite(hatchPIN, LOW);
        digitalWrite(FanPIN, LOW);
      }
      else if (tempC >= 25 && digitalRead(FanPIN) == LOW) { // keep the fan on
        digitalWrite(hatchPIN, HIGH);
        digitalWrite(FanPIN, HIGH);
      }
      else if (tempC <= 25 && digitalRead(FanPIN) == LOW) { // keep the fan off
        digitalWrite(hatchPIN, LOW);
        digitalWrite(FanPIN, LOW);
      }
      Serial.println(tempC);
      Particle.publish("tempC");

      tempCheck += 1000; // check again the next second
    }
  }
  else if (mode == Incubate) {
    if (currentTime > motionCheck) {
      if (digitalRead(motionPIN) == HIGH) {
        digitalWrite(relayPIN, HIGH);
        digitalWrite(hatchPIN, LOW);
        digitalWrite(FanPIN, LOW);
        Serial.println("Motion detected");
      }
      else if (digitalRead(motionPIN) == LOW) {
        digitalWrite(relayPIN, LOW);
        Serial.println("No motion detected");
      }
      motionCheck += 10000; // check again in 5 seconds
    }
  }
  else if (mode == NullMotion) {
    digitalWrite(hatchPIN, LOW);
    digitalWrite(FanPIN, LOW);
    digitalWrite(relayPIN, HIGH);
  }

}

int setTempMode(String inputString)
{
  if (inputString == "Regulate") {
    mode = Regulate;
    return 0;
  }
  else if (inputString == "Incubate") {
    mode = Incubate;
    return 1;
  }
  else if (inputString == "NullMotion") {
    mode = NullMotion;
    return 6;
  }
  else {
    return -1; // returning an error if HTML file should crash and output incorrect strings
  }
}

int setDoorMode(String doorString) {
  if (doorString == "Close") {
    Doormode = Close;
    return 2;
  }
  else if (doorString == "Open") {
    Doormode = Open;
    return 3;
  }
  else {
    return -1; // returning an error if HTML file should crash and output incorrect strings
  }
}

int setSecurMode(String securityString) {
  if (securityString == "Arm") {
    Securitymode = Arm;
    return 4;
  }
  else if (securityString == "deArm") {
    Securitymode = deArm;
    return 5;
  }
  else {
    return -1; // returning an error if HTML file should crash and output inccorect strings
  }
}