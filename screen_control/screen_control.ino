/*
   Screen control.
   Aleksei Tsikin
   email: aleksei555@gmail.com
*/

#include "nextioninit.h"
#include "enumdefs.h"
#include <SoftwareSerial.h>

#define DEBUG

#define PW_PIN A1
#define FRQ_PIN A2
#define SAMPLES 50 //To smoothen out anlogRead fnction

const int analogPinsToGround[] = {A3, A4, A5};  //Pull this guys UP (+5) so we dont have floating pins

int currentPage = 0;
int pwNow = 0;    //Pulse width now
int frqNow = 0;   //Frequency now


int phases = threePhase;
int step = fullStep;
int rotation = cw;
int manualByte = 0x30;
int autoByte = 0xC0;
const int resetByte = 0x00;

SoftwareSerial mySerial(36, 37); //(RX pin, TX pin)

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  for (int pin = 0; pin < sizeof(analogPinsToGround); pin++)
    pinMode(analogPinsToGround[pin], INPUT_PULLUP);

  nexInit();
  pushCallback();
  endLine();
}

void loop() {
  if (currentPage == 1)
    readPots();
  nexLoop(nex_listen_list);
}

void readPots() {
  pwNow = analogRead(PW_PIN);
  frqNow = analogRead(FRQ_PIN);
  frqNow = map(frqNow, 0, 1023, 1, 30);
  switch (phases) {
    case threePhase:
      if (step == fullStep)
        pwNow = map(pwNow, 0, 1023, 1, 33);
      else
        pwNow = map(pwNow, 0, 1023, 34, 66);
      break;
    case fourPhase:
      if (step == fullStep)
        pwNow = map(pwNow, 0, 1023, 1, 25);
      else
        pwNow = map(pwNow, 0, 1023, 26, 49);
      break;
  }
  updateDisplayParameters();
}


void updateDisplayParameters() {
  Serial.print("nxfrq.val=");
  Serial.print(abs(frqNow));
  endLine();
  Serial.print("nxpw.val=");
  Serial.print(abs(pwNow));
  endLine();
}

void autoModeFunct(void *ptr) {
  currentPage = 1;
  Serial.print("page 1");
  endLine();
  Serial.print("vis 9,0");
  endLine();
}

void manualModeFunct(void *ptr) {

  manualByte = 0x30;
  Serial.print("page 2");
  endLine();
  Serial.print("vis 7,0");
  endLine();
}

void autoManualBackFunct(void *ptr) {
  currentPage = 0;
  mySerial.write(resetByte);
  Serial.flush();
  Serial.print("page 0");
  endLine();
}

void autoMetersFunct (void *ptr) {

}

void autoSetupFunct (void *ptr) {
  currentPage = 4;
  Serial.flush();
  Serial.print("page 4");
  endLine();

}

void autoStartFunct (void *ptr) {
  uint32_t number5 = 0;
  autoStart.getValue(&number5);
  if (number5 == 1){
    mySerial.write(autoByte);
    Serial.print("vis 20,0");
    endLine();
  }
  else{
    mySerial.write(resetByte);
    Serial.print("vis 20,1");
    endLine();
  }
}

void manualMetersFunct (void *ptr) {

}

void manAFunct (void *ptr) {
  manualByte ^= 1 << 3;
  mySerial.write(manualByte);
  endLine();
}

void manBFunct (void *ptr) {
  manualByte ^= 1 << 2;
  mySerial.write(manualByte);
  endLine();
}

void manCFunct (void *ptr) {
  manualByte ^= 1 << 1;
  mySerial.write(manualByte);
  endLine();
}

void manDFunct (void *ptr) {
  manualByte ^= 1 << 0;
  mySerial.write(manualByte);
  endLine();
}

void metersBackFunct (void *ptr) {
  mySerial.write(resetByte);
  endLine();

}

void stepSelectFunct (void *ptr) {
  Serial.flush();
  Serial.print("page4.step.txt=");
  Serial.print("\"");
  if (step == fullStep) {
    Serial.print("HALF");
    step = halfStep;

  }
  else {
    Serial.print("FULL");
    step = fullStep;
  }
  Serial.print("\"");
  endLine();
  autoByte ^= 1 << 1;
}

void phaseSelectFunct (void *ptr) {
  Serial.flush();
  Serial.print("page4.phase.txt=");
  Serial.print("\"");
  if (phases == threePhase) {
    Serial.print("4");
    phases = fourPhase;
  }
  else {
    Serial.print("3");
    phases = threePhase;
  }
  Serial.print("\"");
  endLine();
  autoByte ^= 1 << 3;
}

void dirSelectFunct (void *ptr) {
  Serial.flush();
  Serial.print("page4.rotation.txt=");
  Serial.print("\"");
  if (rotation == cw) {
    Serial.print("CCW");
    rotation = ccw;
  }
  else {
    Serial.print("CW");
    rotation = cw;
  }
  Serial.print("\"");
  endLine();
  autoByte ^= 1 << 2;
}

void setupBackFunct (void *ptr) {
  Serial.flush();
  Serial.print("page 1");
  endLine();
  Serial.print("vis 9,0");
  endLine();
  currentPage = 1;
}

void pushCallback() {
  autoMode.attachPush(autoModeFunct);
  manualMode.attachPush(manualModeFunct);
  autoBack.attachPush(autoManualBackFunct);
  autoMeters.attachPush(autoMetersFunct);
  autoSetup.attachPush(autoSetupFunct);
  autoStart.attachPush(autoStartFunct);
  manualBack.attachPush(autoManualBackFunct);
  manualMeters.attachPush(manualMetersFunct);
  manA.attachPush(manAFunct);
  manB.attachPush(manBFunct);
  manC.attachPush(manCFunct);
  manD.attachPush(manDFunct);
  metersBack.attachPush(metersBackFunct);
  stepSelect.attachPush(stepSelectFunct);
  phaseSelect.attachPush(phaseSelectFunct);
  dirSelect.attachPush(dirSelectFunct);
  setupBack.attachPush(setupBackFunct);
}
