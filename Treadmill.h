//#include <ArduinoJson.h>

#ifndef __TREADMILL__
#define __TREADMILL__
#include <Arduino.h>
//#include "Tonez.h"

////////////////////////ODOMETER COUNTER ENCODER PINƒ
#define ENCODER_PIN 2

//5280 feet per mile
//63360 inches per mile
//1 mph = 63360 inches per hour
//63360 inches per hour = 17.6 inches per second
//17.6 inches per second == 0.0176 inches per msec

//1 minute at 4 mph yields 30090 ticks
//1 minute at 5280 * 4 feet per hour is 30090 ticks
//1 minute at 5280 * 4 feet per 60 minutes is 1 * 5280 * 4 / 60 = 352 feet.
//352 feet at 30090 ticks is 0.01169823861 feet per tick.
//////////////////////////SPEED CONSTANTS///////////////////////////////
#define convMPH 320.5273041
#define convKPH 199.1669281
#define yInt -62.2555619
/////////////////////////GRADE CONSTANTS////////////////////////////////
//#define GRADE_ZERO 7040.0
//#define GRADE_ZERO 13952.0
//#define GRADE_MUL 43008.0
#define GRADE_ZERO 12921.67829
#define GRADE_MUL 2503.607251

#define MIN_GRADE -3.0
#define MAX_GRADE 18.0

////////////////////////FAN//////////////////////
#define FAN_PIN 38

//////////////////////THE ONE AND ONLY TREADMILL PORT...////////////////
#define SerialTreadmill Serial1


#define noMode 0
#define NO_METS 0
#define WALKING_METS 1
#define RUNNING_METS 2
#define INTERVAL_MAX 15

class Treadmill
{
  private:
  ///////////////////FAN////////////////////
  bool fanMode = 0;  //0 is off 1 is on
  
  
  public:
  bool debugMode = false;
  static volatile unsigned long counter;
  
  

  
  volatile word speedWord = 0;
  volatile word gradeWord = 0;
  volatile word statusWord = 0;
  volatile word statusWord2 = 0;
  volatile word statusWord3 = 0;
  volatile bool moving = false;
  volatile bool error = false;
  ///////////METS//////////////////////////

  volatile bool bMetsActive = false;
  volatile float targetMets = 0.0;
  float savedTargetMets = 0.0;   //used to restore after stopp pressed;
  volatile int targetMetsMode = RUNNING_METS;
  volatile int targetHR = -1;

  volatile float fTargetSpeed = 0;
  volatile float fTargetSpeedK = 0;
  volatile float fTargetGrade = 0;
  //////////////////////RANDOM GRADE GLOBAL VARIABLES
  volatile long randomGradeInterval = 30;
  volatile int randomGradeMin = 0;
  volatile int randomGradeMax = 4;
  int randomGradeMode = 0;//0 = random, 1 = ramp up
  
  /////////////////////////////////ZONES
  int nAge = 51;
  int nMaxHR = 220 - nAge;
  int nHealthyZone = 0.5 * (float)nMaxHR;
  int nFitnessZone = 0.65 * (float)nMaxHR;
  int nAerobicZone = 0.75 * (float)nMaxHR;
  int nAnerobicZone = 0.85 * (float)nMaxHR;
  int nRedLineZone = 0.95 * (float)nMaxHR;
  //Tonez tonez;

  
  
  
   
  
////////////////INTEGRATION VALUES//////////////////////
  float fTotalCalories = 0;
  float fWeight = 70.0;
  float fElevationGain = 0;
  float fIntegratedDistance = 0;

  
  
  
  void printBytes(byte *b,int len);
  void getResult(bool waitForever = false);
  void sendCommand (char * szDebugText,byte *b,int len,bool waitForever = false);
  byte doCRC1 (byte *message,int length);
  void sendCommandCRC (char *szCommand,byte *command,byte s);
  void setGrade (float fGrade);
  void setRawGrade (word wFinal);
  void setSpeed (float fSpeed);
  void setSpeedK (float fSpeed);
  void checkStatus();
  float fGrade();
  float fSpeed();
  float fSpeedK();
  float fWalkingMets();
  float fRunningMets();
  float fMets (float speed,float grade);
  float fMets();
  void getPace(float spd,char *ret);
  void getPace(char *ret);
  void getPaceK(float spd,char *ret);
  void getPaceK(char *ret);
  float calcSpeedFromMets ();
  float calcSpeedFromMets(float m,float g);
  void resetVariables();
  void init();
  void updateRandomGrade(bool forceStart = false);
  void handleMets();
  void delayWithYield(int ms);

  
  byte fan ();
  void fan (byte mode);

  /////////INTERRUPT
  static void intProc();
  float fDist();
  void integrateStuff();
  void clearOdometer();

  
  
  
  
};
#endif
