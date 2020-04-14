/*
   Driver control.
   Aleksei Tsikin
   email: aleksei555@gmail.com
*/

#include "commands.h"
#include <SoftwareSerial.h>

#define pwPot A1
#define frqPot A2
#define phA 38
#define phB 39
#define phC 40
#define phD 44

SoftwareSerial mySerial(51, 50); //(RX, TX) pins
ExtractCommand myCommand;

int phases = threePhase;
int step = fullStep;
int rotation = cw;

bool runMotor = false;


double PW, frq, ramp, frqold, PWx;
double T, PW25, PW33, PWtime, Overlap, Pulsetime;
double phasecorrection = 0.600;              //Time im ms to execute readdata() function.
double PWcorr = Pulsetime - phasecorrection;



void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop() {
  if (mySerial.available() > 0) {
    int inByte = mySerial.read();
    Serial.println(inByte, HEX);
    int mode = myCommand.extractMode(inByte);
    switch (mode) {
      case 0xC0: runMotor = true;
        setParameters(inByte);
        break;
      case 0x30: myCommand.extractManualCommand(inByte);
        break;
      case 0x00: myCommand.allLow();
        runMotor = false;
        break;
      default: return;
    }
  }
  if (runMotor == true)
    autoModeFunct();
}

void setParameters(int inByte) {
  phases = myCommand.setPhase(inByte);
  rotation = myCommand.setRotation(inByte);
  step = myCommand.setStep(inByte);
}

void autoModeFunct() {
  //Serial.println("Running");
  readData();
  calcDelays();
  autoRun();
}

void readData() {
  frq = map(analogRead(frqPot), 0, 1023, 1, 30);
  if (phases == fourPhase) {
    if (step == halfStep)
      PW = map(analogRead(pwPot), 0, 1023, 26, 49);
    else
      PW = map(analogRead(pwPot), 0, 1023, 1, 25);
  } else {
    if (step == halfStep)
      PW = map(analogRead(pwPot), 0, 1023, 34, 66);
    else
      PW = map(analogRead(pwPot), 0, 1023, 1, 33);
  }
  //Serial.println("frq : " + String(frq) + " PW: " + String(PW));
}

void calcDelays() {
  T = (1 / frq) * 1000;                       //Period time in ms.
  PWtime = T * (PW / 100);
  if (phases == fourPhase) {           //4 phase is active
    PW25 = T * 0.25;                          //25% pulse width
    if (step == halfStep) {     //half step is active
      Overlap = PWtime - PW25;
      PWx = PW25 - Overlap;
    } else                          //full step is active
      PWx = PW25 - PWtime;
  } else {                                    //3 phase is active
    PW33 = T * 0.3333;                          //33.3% pulse width
    if (step == halfStep) {      //half step is active
      Overlap = PWtime - PW33;
      PWx = PW33 - Overlap;
    } else                               //full step is active
      PWx = PW33 - PWtime;
  }
  //Serial.println("Calc Delays Done");
}

void autoRun() {
  //Serial.println("AutoRun Funct");
  float PWxcorr = PWx - phasecorrection;
  if (phases == fourPhase) {           //4 Phase is active  HIGH
    //Serial.println("4 Phase");
    if (rotation == cw) {      //CW rotation
      phaseA();
      delay(PWx);
      phaseB();
      delay(PWx);
      phaseC();
      delay(PWx);
      phaseD();
      delay(PWx);
    } else {                                  //CCW rotation
      phaseA();
      delay(PWx);
      phaseD();
      delay(PWx);
      phaseC();
      delay(PWx);
      phaseB();
      delay(PWx);
    }
  } else {                                      //3 Phase is active
     //Serial.println("3 Phase");
    if (rotation == cw) {     //CW rotation
      phaseA();
      delay(PWx);
      phaseB();
      delay(PWx);
      phaseC();
      delay(PWx);
    } else {                                  //CCW rotation
      phaseA();
      delay(PWx);
      phaseC();
      delay(PWx);
      phaseB();
      delay(PWx);
    }
  }
}

void phaseA() {
  //Serial.println("PHASE A");
  digitalWrite(phA, HIGH);
  if (step == halfStep) {
    delay(Overlap);
    if (phases == fourPhase) {
      if (rotation == cw)
        digitalWrite(phD, LOW);
      else
        digitalWrite(phB, LOW);
    } else {
      if (rotation == cw)
        digitalWrite(phC, LOW);
      else
        digitalWrite(phB, LOW);
    }
  } else {
    delay(PWtime);
    digitalWrite(phA, LOW);
  }
}

void phaseB() {
  //Serial.println("PHASE B");
  digitalWrite(phB, HIGH);
  if (step == halfStep) {
    delay(Overlap);
    if (phases == fourPhase) {
      if (rotation == cw)
        digitalWrite(phA, LOW);
      else
        digitalWrite(phC, LOW);
    } else {
      if (rotation == cw)
        digitalWrite(phA, LOW);
      else
        digitalWrite(phC, LOW);
    }
  } else {
    delay(PWtime);
    digitalWrite(phB, LOW);
  }
}

void phaseC() {
  //Serial.println("PHASE C");
  digitalWrite(phC, HIGH);
  if (step == halfStep) {
    delay(Overlap);
    if (phases == fourPhase) {
      if (rotation == cw)
        digitalWrite(phB, LOW);
      else
        digitalWrite(phD, LOW);
    } else {
      if (rotation == cw)
        digitalWrite(phB, LOW);
      else
        digitalWrite(phA, LOW);
    }
  } else {
    delay(PWtime);
    digitalWrite(phC, LOW);
  }
}

void phaseD() {
  //Serial.println("PHASE D");
  digitalWrite(phD, HIGH);
  if (step == halfStep) {
    delay(Overlap);
    if (rotation == cw)
      digitalWrite(phC, LOW);
    else
      digitalWrite(phA, LOW);
  } else {
    delay(PWtime);
    digitalWrite(phD, LOW);
  }
}
