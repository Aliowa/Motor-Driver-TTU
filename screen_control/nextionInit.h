#include <Arduino.h>
#include <Nextion.h>

//Home Page
NexButton autoMode = NexButton(0, 1, "nxauto");
NexButton manualMode = NexButton(0, 2, "nxmanual");


//Auto Mode Page
NexButton autoBack = NexButton(1, 1, "nxautohome");
NexButton autoMeters = NexButton(1, 9, "nxautometer");
NexButton autoSetup = NexButton(1, 20, "nxautosetup");
NexDSButton autoStart = NexDSButton(1, 19, "xstart");


//Manual Mode Page
NexButton manualBack = NexButton(2, 1, "nxmanhome");
NexButton manualMeters = NexButton(2, 7, "nxmanmeters");
NexDSButton manA = NexDSButton(2, 3, "mana");
NexDSButton manB = NexDSButton(2, 4, "manb");
NexDSButton manC = NexDSButton(2, 5, "manc");
NexDSButton manD = NexDSButton(2, 6, "mand");


//Meters Page
NexButton metersBack = NexButton(3, 1, "nxmeterhome");

//Setup Page
NexButton stepSelect = NexButton(4, 10, "setupstep");
NexButton phaseSelect = NexButton(4, 9, "setupphase");
NexButton dirSelect = NexButton(4, 11, "setupdir");
NexButton setupBack = NexButton(4, 1, "nxsetuphome");


NexTouch *nex_listen_list[] = {
  &autoMode,
  &manualMode,
  &autoBack,
  &autoMeters,
  &autoSetup,
  &autoStart,
  &manualBack,
  &manualMeters,
  &manA,
  &manB,
  &manC,
  &manD,
  &metersBack,
  &stepSelect,
  &phaseSelect,
  &dirSelect,
  &setupBack,
  NULL
};


void endLine() {
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);
}
