#include <Arduino.h>

enum Defs {
  threePhase,
  fourPhase,
  halfStep,
  fullStep,
  cw,
  ccw,
};

class ExtractCommand {
  public:
    int extractMode(int);
    void extractManualCommand(int);
    void allLow(void);

    int setPhase(int);
    int setRotation(int);
    int setStep(int);

  private:
    const int modeMask = 0xF0;      //11110000 in BIN
    const int commandMask[4] = {0x01, 0x02, 0x04, 0x08}; //00000001, 00000010, 00000100, 00001000
    const int driverPins[4] = {40, 41, 42, 43};
    const int c[4] = {4, 3, 2, 1};
};

int ExtractCommand::extractMode(int inByte) {
  return inByte & modeMask;
}

void ExtractCommand::extractManualCommand(int inByte) {
  for (int i = 0; i < 4; i++) {
    int testByte = inByte & commandMask[i];
    if (testByte == commandMask[i])
      digitalWrite(driverPins[i], HIGH);
    //Serial.println("Phase " + String(c[i]) + " HIGH");
    else
      digitalWrite(driverPins[i], LOW);
    //Serial.println("Phase " + String(c[i]) + " LOW");
  }
}

void  ExtractCommand::allLow(void) {
  for (int i = 0; i < sizeof(driverPins); i++)
    digitalWrite(driverPins[i], LOW);
}

int ExtractCommand::setPhase(int inByte) {
  int testByte = inByte & commandMask[3];
  if (testByte == commandMask[3]) {
    //Serial.println("4 phase");
    return fourPhase;
  }
  else {
    //Serial.println("3 phase");
    return threePhase;
  }
}

int ExtractCommand::setRotation(int inByte) {
  int testByte = inByte & commandMask[2];
  if (testByte == commandMask[2]) {
    //Serial.println("ccw");
    return ccw;
  }
  else {
    //Serial.println("cw");
    return cw;
  }
}

int ExtractCommand::setStep(int inByte) {
  int testByte = inByte & commandMask[1];
  if (testByte == commandMask[1]) {
    //Serial.println("Half step");
    return halfStep;
  }
  else {
    //Serial.println("full Step");
    return fullStep;
  }
}
