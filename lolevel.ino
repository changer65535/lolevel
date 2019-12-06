////////TODO...
//Check speedRamp--set speed (during handlespeedramp) should works while in mets mode!! (To adjust speed during workouts)
//See of Grade Offset Works (It is now seperate from Speed Offset)




//#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
/////////RELAY/////////////////////



/////////NEOPIXEL/////////////////
#define LED_PIN    36
#define LED_COUNT 8

/////TODO////////////////////


//#include <ArduinoJson.h>
#include <Keypad.h>

#include "NexDisplay.h"
#include "Treadmill.h"
#include <SoftwareSerial.h>
#include "GradeSpeedMap.h"
#include <Arduino.h>
#include <Wire.h>
//#include <VL53L0X.h>
//#include <Adafruit_LIS3DH.h>
//#include <Adafruit_Sensor.h>

/////////////////ACCELEROMETER///////////////////
//Adafruit_LIS3DH lis = Adafruit_LIS3DH();
float fThump = 0;
float fMaxThump = 0;

/////////////////////LIDAR///////////////////////
//VL53L0X lidar;
bool lidarActive = false;

////////////////////////////////////////////////
bool bDrawGrade = false,bDrawSpeed = false,bDrawHR = false,bDrawMets = false;
NexDisplay nex;
int nexActivePage = 0;
#define NEX_UPDATE_INTERVAL 100

float fRange = 0;


///////////////KEYPAD////////////////////////////////////////////////////////
const byte ROWS = 3; //two rows
const byte COLS = 3; //two columns
byte rowPins[ROWS] = {44, 42, 40}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {48, 46}; //connect to the column pinouts of the keypad
//52 -->1
//50 -->2
//48 -->3
//46 -->4 on the rs232

char keys[ROWS][COLS] = 
{
  {'d','g'},
  {'u','s'},
  {'r','f'} 
};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
/////////////NEOPIXEL//////////////
//Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
//////////////////////GLOBALS///////////////////////////
bool speakerActive = true;

Treadmill treadmill;
#define SerialWiFi Serial2
//SoftwareSerial SerialBT(51,53);  //rx tx
SoftwareSerial SerialBT(13,12);  //rx tx

GradeSpeedMap gsm;

/////////ACCEL////
float magnitude = 0;





//////////////////////////////////////////////



volatile int btHR = 0;
char currentCommand[64];
char currentCommandBT[64];

bool debugMode = false;
char szMessage[80] = "";
///////////////////////////Chirps and Tones/////////////////
void chirp()
{
  SerialBT.println("c:2");
}
void upchirp()
{
  SerialBT.println("c:1");
}
void doubleTone()
{
  SerialBT.println("c:3");
}

void quickTone()
{
  SerialBT.println("c:4");
}
void quickToneHigh()
{
  SerialBT.println("c:5");
}
void drawMessage (char * szMessage)
{
  nex.setTxt("g0",szMessage);
}
void incSpeed (float delta)
{
  quickToneHigh();
  if (gsm.nIntervalsActive)
  {
    gsm.intervalData[gsm.nIntervalDataIndex].speed0+=delta;
    gsm.intervalData[gsm.nIntervalDataIndex].speed1+=delta;
    return;
  }
  if (treadmill.bMetsActive) 
  {
    treadmill.targetMets +=delta;
    if (treadmill.targetMets > 16) treadmill.targetMets = 16.0;
    if (treadmill.targetMets < 1) treadmill.targetMets = 1;
  }
  else 
  { 
    if (treadmill.fSpeedK() < 1) treadmill.setSpeedK(1);
    treadmill.setSpeedK(treadmill.fTargetSpeedK + delta);
  }
}


void changeGrade( float delta)
{
  quickToneHigh();
  if (gsm.nIntervalsActive)
  {
    gsm.intervalData[gsm.nIntervalDataIndex].grade0 += delta;
    gsm.intervalData[gsm.nIntervalDataIndex].grade1 += delta;
    return;
  }
  treadmill.setGrade(treadmill.fTargetGrade + delta);
}
///////////////////////////Declaration for the nextion CALLBACKS///////////////////////////////////////
void gradePlusCallback(void *ptr)
{
  changeGrade(0.25);
}
void gradeMinusCallback(void *ptr)
{
 changeGrade (-0.25);
}
void gradePlusPlusCallback(void *ptr)
{
  changeGrade(1);
}

void gradeMinusMinusCallback(void *ptr)
{
 changeGrade(-1);
}
void speedPlusCallback(void *ptr)
{
  incSpeed(0.1);  
}
void speedPlusPlusCallback(void *ptr)
{
 incSpeed(1);  
}

void speedMinusCallback(void *ptr)
{
  incSpeed(-0.1);
}
void speedMinusMinusCallback(void *ptr)
{
  incSpeed(-1);
}

void stopButtonPopCallback(void *ptr)
{
  upchirp();
  treadmill.resetVariables();
  //gsm.clearIntervals();
  gsm.nIntervalsActive = false;
  gsm.nGradeDataIndex = 0;
  gsm.bActiveGrade = false;
  gsm.nLandmarkDataIndex = 0;
  
  gsm.fDistOffset = 0;
  gsm.fMapOffset = 0;
  treadmill.setSpeed(0);
  lidarActive = false;
  SerialBT.println("r");//reset
  
  
}
void pauseButtonPopCallback(void *ptr)
{
  static float fLastSpeed = 0;
  if (fLastSpeed == 0)
  {
    upchirp();
    treadmill.setSpeed(0);
    fLastSpeed = treadmill.fSpeed();
    nex.setTxt("pauseButton","Go");
  }
  else
  {
    upchirp();
    treadmill.setSpeed(fLastSpeed);
    fLastSpeed = 0;
    nex.setTxt("pauseButton","Pause");
  }
  
}
void lidarButtonPopCallback(void *ptr)
{
  upchirp();
  lidarActive = !lidarActive;
  
  
}
void speakerButtonPopCallback(void *ptr)
{
  upchirp();
  int nCadence = nex.getInt("cadenceVal");
  if (nCadence > 200)
  {
    nCadence = 200;
    nex.setTxt("cadenceVal",nCadence);
  }
  if (nCadence <30)
  {
    nCadence = 30;
    nex.setTxt("cadenceVal",nCadence);
  }
  speakerActive = !speakerActive;
  
  if (speakerActive) 
  {
    char szCadence[16];
    sprintf(szCadence,"p:%d",nCadence);
    SerialBT.println(szCadence);
  }
  else SerialBT.println("p:0");
  
  
}
void fanButtonPopCallback(void *ptr)
{
  
  treadmill.fan(!treadmill.fan());
  quickToneHigh();
  if (treadmill.fan() == 1) 
  {
   drawMessage("Fan On");
    
    
  }
  else 
  {
    drawMessage("Fan Off");
    
  }
}
void graphButtonPopCallback(void *ptr)
{
  
  initPage2();
  
  
}


void mapPopCallback(void *ptr)
{
  
  //chirp();
  nex.sendCommand("page page6");
  initPage7();
}
void metsModeButtonPopCallback (void *ptr)
{
  upchirp();
  if (nex.getVal("metsMode") == 0) treadmill.targetMetsMode = WALKING_METS;
  if (nex.getVal("metsMode") == 1) treadmill.targetMetsMode = RUNNING_METS;
  
}
void metsOffPopCallback (void *ptr)
{
  upchirp();
  treadmill.targetMetsMode = RUNNING_METS;
  treadmill.bMetsActive = false;
  treadmill.randomGradeInterval = 0;
  //homeButtonPopCallback(0);
  
  
}


void startRandomButtonPopCallback (void *ptr)
{
  
  treadmill.bMetsActive = true;
  treadmill.targetMets = nex.getFloat("page1.targetMets");
  treadmill.savedTargetMets = treadmill.targetMets;
  treadmill.randomGradeMin = nex.getFloat("page1.minGrade");
  treadmill.randomGradeMax = nex.getFloat("page1.maxGrade");
  
  treadmill.randomGradeInterval = nex.getVal("page1.n0");
  
  treadmill.targetMetsMode = nex.getVal("page1.metsMode") + 1;
  
  
  
  treadmill.setGrade(treadmill.randomGradeMin);

  if (nex.getVal("c0") == 1)
  {
    Serial.println("Random HR!!");
    treadmill.targetHR = nex.getFloat("t1");
  }
  else
  {
    treadmill.targetHR = 0;
  }
  treadmill.updateRandomGrade(true);//force start
 
  
  homeButtonPopCallback(0);
  
}

void gradeChangePopCallback ()
{
  float fNewGrade = (float)nex.getVal("h0") / 10.0;
  treadmill.setGrade(fNewGrade);
  nex.sendCommand("page page0");
  delay(100);
  upchirp();
}
void gradeMinusPopCallback ()
{
  treadmill.setGrade(treadmill.fGrade() - 0.5);
}


void startSpeedmapPopCallback (void *ptr)
{
 
  upchirp();
  
  if (nex.getVal("bt0") == 1) 
  {
    treadmill.clearOdometer();
    gsm.nIntervalsActive = true;
    Serial.println("Active Speed Activated");
  }
  else gsm.nIntervalsActive = false;
}
void startGrademapPopCallback (void *ptr)
{
  upchirp();
  if (nex.getVal("bt1") == 1) gsm.bActiveGrade = true;else gsm.bActiveGrade = false;
}
void offsetMinusPopCallback (void *ptr)
{
  //calculate one second at current speed
  float fOffset = treadmill.fSpeed() / 3600.0;
  gsm.fDistOffset +=fOffset;
  //upchirp();
}
void offsetPlusPopCallback (void *ptr)
{
  float fOffset = treadmill.fSpeed() / 3600.0;
  gsm.fDistOffset -= fOffset;
  upchirp();
}
void alignPopCallback (void *ptr)
{
  upchirp();
  gsm.alignLandmarks(&treadmill);
  
}
void skipButtonPopCallback (void *ptr)
{
  upchirp();
  landmarkDataType lm;
  gsm.fetchLandmarkItem(gsm.nLandmarkDataIndex,&lm);
  if (strlen(lm.landmark) > 0) gsm.nLandmarkDataIndex++;
  
  gsm.nLandmarkDataIndex++;
  
}
void setGradeMap(gradeDataType *gp,landmarkDataType *lp)
{
  upchirp();
  gsm.gradeData = gp;
  gsm.landmarkData = lp;
  nex.sendCommand("page page6");
  //start grade a goin'
  gsm.bActiveGrade = true;
  
  initPage7();
}
void grade0PopCallback (void *ptr)
{
  setGradeMap(fiveKRoute,fiveKLandmarks);
}

void grade1PopCallback (void *ptr)
{
  setGradeMap(hillyRun,hillyRunLandmarks);
}
void grade2PopCallback (void *ptr)
{
  setGradeMap(volcanoClimb,volcanoClimbLandmarks);
}
void grade3PopCallback (void *ptr)
{
 setGradeMap(elevenOcean,elevenOceanLandmarks);
}
void grade4PopCallback (void *ptr)
{
 setGradeMap(mountainRoute,mountainRouteLandmarks);
}
void customInterval1PopCallBack (void *ptr)
{
 setGradeMap(mountainRoute,mountainRouteLandmarks);
}


void clearButtonPopCallback(void *ptr)
{
  nex.setTxt("t2","");
 
 
 
  if (gsm.nIntervalMax > 0) 
  {
    
    
    gsm.nIntervalMax--;
    if (gsm.nIntervalDataIndex > gsm.nIntervalMax) gsm.nIntervalDataIndex = gsm.nIntervalMax;
    
    
    memset(&gsm.intervalData[gsm.nIntervalMax],0,sizeof(gsm.intervalData[0]));
    drawSpeedMap("t2");
    quickToneHigh(); 
  }
  if (gsm.nIntervalMax == 0)
  {
    gsm.nIntervalsActive = false;
  }
  
  
}

void startButtonPopCallback(void *ptr)
{
  gsm.startIntervals(&treadmill);
  quickToneHigh();
}

void drawPreview ()
{
  char szPreview[128] = "";
  sprintf(szPreview,"%f-%f mph",gsm.getIntervalSpeed(gsm.nIntervalDataIndex,0),gsm.getIntervalSpeed(gsm.nIntervalDataIndex,1));
  nex.setTxt("preview",szPreview);
  
}
void previewPopCallback(void *ptr)
{
  quickToneHigh();
  drawPreview();
  
}

void hill1Callback (void *ptr)
{
  //void GradeSpeedMap::addInterval (int nSpeedMode,int nTimingMode,float distance,float pct0,float pct1,int nRampMode,float grade0,float grade1, int reps,int lastSteps)
  gsm.addInterval(SPEED_PACE,TIMING_MINUTES,15,10,10,RAMP_RANDOM,0,4);
  gsm.addInterval(SPEED_PACE,TIMING_MINUTES,5,10,8,RAMP_LINEAR);
  gsm.addInterval(SPEED_PACE,TIMING_MINUTES,2,10,10,RAMP_LINEAR);
  gsm.addInterval(SPEED_PACE,TIMING_MINUTES,2,8,8,RAMP_LINEAR,15,15);
  gsm.addInterval(SPEED_PACE,TIMING_MINUTES,2,11,11,RAMP_LINEAR,0,0,6,1);
  
  
}
void addButtonPopCallback (void *ptr)
{
  Serial.println(gsm.nIntervalDataIndex);
  Serial.println(gsm.nIntervalMax);
  int nSpeedMode = nex.getVal("speedMode");                   //0 = Based on mph
                                                              //1 = Based on Mets
                                                              //2 = Based on Pace
  float speed_slow,speed_fast;
  
  if (nSpeedMode == SPEED_PACE)
  {
    
    speed_slow = nex.getPace("speed_slow",30);           //default to 30:00 minute per mile pace if field is empty
    speed_fast = nex.getPace("speed_fast",speed_slow);   //default to speed1_slow if field is empty.  E.G.  No ramp
  }    
  else
  {
    speed_slow = nex.getFloat("speed_slow");
    speed_fast = nex.getFloat("speed_fast",speed_slow);                                                        
  }
  
  float grade_slow = nex.getFloat("grade_slow",1);//defaults to 1
  float grade_fast = nex.getFloat("grade_fast",grade_slow);//defauts to initial grade
  float speed_dist = nex.getFloat("distance");
  
  int nTimingVal = nex.getVal("timingVal");                   //0 = change speed and grade corresponding with distance (in miles)
                                                              //1 = change speed and grade corresponding with distance (in meters)
                                                              //2 = change speed and grade corresponding with time (in minutes)
 
  //Serial.print("grade_fast: ");Serial.println(grade_fast);
  
  
  int nReps = nex.getInt("reps",1);
  int nLastSteps = nex.getInt("lastSteps",0);                 //How many steps backwards to skip  (last step would -> 1).  No skips would -> 0
  int nRampMode = nex.getVal("rampMode");                     //0 = linear ramp between grade0 and grade1
                                                              //1 = random numbers between grade0 and grade1
                                                              //2 = V mode.  Ramps to half way and back down
                                                
  
          
  
  gsm.addInterval(nSpeedMode,nTimingVal,speed_dist,speed_slow,speed_fast,nRampMode,grade_slow,grade_fast,nReps,nLastSteps);
  
  drawSpeedMap("t2");
  
  quickToneHigh();
}

void nextButtonPopCallback (void *ptr)
{
  quickToneHigh();
  GradeSpeedMap::IntervalType *ci = &gsm.intervalData[gsm.nIntervalDataIndex];
  gsm.intervalDone(&treadmill,ci);
}
void drawIntervalControls()
{
  GradeSpeedMap::IntervalType *ci = &gsm.intervalData[gsm.nIntervalDataIndex];
  nex.setVal("speedMode",ci->nSpeedMode);
  switch (ci->nSpeedMode)
  {
    case SPEED_PCT_1M:nex.setTxt("speedTypeText","Pct 1M");break;
    case SPEED_PCT_5K:nex.setTxt("speedTypeText","Pct 5K");break;
    case SPEED_PCT_10K:nex.setTxt("speedTypeText","Pct 10K");break;
    case SPEED_PCT_HM:nex.setTxt("speedTypeText","Pct HM");break;
    case SPEED_PCT_M:nex.setTxt("speedTypeText","Pct M");break;
    case SPEED_METS:nex.setTxt("speedTypeText","Mets");break;
    case SPEED_PACE:nex.setTxt("speedTypeText","Pace");break;
    case SPEED_SPEED:nex.setTxt("speedTypeText","Speed");break;
  }
  nex.setVal("rampMode",ci->nRampMode);
  switch (ci->nRampMode)
  {
    case RAMP_LINEAR:nex.setTxt("rampText","Linear %");break;
    case RAMP_RANDOM:nex.setTxt("rampText","Random %");break;
    case RAMP_V:nex.setTxt("rampText","V Shaped %");break;
    
  }
  nex.setVal("timingVal",ci->nTimingMode);
  switch (ci->nTimingMode)
  {
    case TIMING_MILES:nex.setTxt("timingTxt","Miles");break;
    case TIMING_METERS:nex.setTxt("timingTxt","Meters");break;
    case TIMING_MINUTES:nex.setTxt("timingTxt","Minutes");break;
    case TIMING_ELEVATION:nex.setTxt("timingTxt","Elevation");break;
  }
  nex.setTxt("distance",ci->dist);
  
  
  nex.setTxt("speed_slow",ci->speed0);
  if (ci->speed1 == ci->speed0) nex.setTxt("speed_fast","");else nex.setTxt("speed_fast",ci->speed1);
  nex.setTxt("grade_slow",ci->grade0);
  if (ci->grade1 == ci->grade0) nex.setTxt("grade_fast","");else nex.setTxt("grade_fast",ci->grade1);
  nex.setTxt("distance",ci->dist);

  nex.setTxt("lastSteps",ci->lastSteps);
  nex.setTxt("reps",ci->reps);
  drawSpeedMap("t2");
  drawPreview();
}
void upButtonPopCallback (void *ptr)
{
  gsm.nIntervalDataIndex--;
  if (gsm.nIntervalDataIndex <0) gsm.nIntervalDataIndex = 0;
  drawIntervalControls();
  
  quickToneHigh();
  
}
void downButtonPopCallback (void *ptr)
{
  gsm.nIntervalDataIndex++;
  if (gsm.nIntervalDataIndex >gsm.nIntervalMax)gsm.nIntervalDataIndex = gsm.nIntervalMax;
  drawIntervalControls();
  quickToneHigh();
  
}
void customButtonPopCallback (void *ptr)
{
 
  quickToneHigh();
  initPage8();
  
}


void gradeMapButtonPopCallback (void *ptr)
{
  
  initPage8();
  
  
}
void speedMapButtonPopCallback (void *ptr)
{
  upchirp();
  
  
  
  
}

void initPage0()
{
  Serial.println("initPage0");
  nexActivePage = 0;
  nex.waitForPage();
 
  
  //Serial.print("Mets: ");
  Serial.println(treadmill.targetMetsMode);
  
  nex.addCallback(6,4,NEX_POP,&gradeChangePopCallback);
  nex.addCallback(0,"b6",NEX_POP,&mapPopCallback);
  nex.addCallback(0,"homeButton",NEX_POP,&homeButtonPopCallback);
  nex.addCallback(0,"b1",NEX_POP,&intervalButtonPopCallback);

  nex.setVal("metsMode",treadmill.targetMetsMode - 1);
  nex.setVal("bt1",treadmill.fan());
  nex.setVal("speakerButton",speakerActive);
  
  Serial.println("added Callback");
}

void initPage1()
{ 
  nexActivePage = 1;
  
  Serial.println("METS Page pressed");
  delayWithStatusCheck(1000);
  nex.waitForPage();
  upchirp();
 
  if (treadmill.targetMets == 0) nex.setTxt("targetMets",treadmill.fMets());
  else nex.setTxt("targetMets",treadmill.targetMets);
  
  nex.setTxt("minGrade",treadmill.randomGradeMin);
  nex.setTxt("maxGrade",treadmill.randomGradeMax);
  
  if (treadmill.randomGradeInterval == 0) nex.setVal("n0",5);
  else nex.setVal("n0",treadmill.randomGradeInterval);
  
  nex.setVal("t1",treadmill.targetHR);
  if (treadmill.targetHR > 0) nex.setVal("c0",1); else nex.setVal("c0",0);
  
  nex.setVal("metsMode",treadmill.targetMetsMode-1);
  nex.addCallback(1,"startRamp",NEX_POP,&startRampCallback);
  nex.addCallback(1,"gsmButton",NEX_POP,&gsmPopCallback);
  nex.addCallback(1,"metsOff",NEX_POP,&metsOffPopCallback);//check me!!
  
}


void initPage2()
{
  nexActivePage = 2;
  delayWithStatusCheck(1000);
  nex.waitForPage();
  upchirp();
  nex.setVal("gradeToggle",bDrawGrade);
  nex.setVal("speedToggle",bDrawSpeed);
  nex.setVal("hrToggle",bDrawHR);
  nex.setVal("metToggle",bDrawMets);
  nex.addCallback(2,"homeButton",NEX_POP,&homeButtonPopCallback);
  nex.addCallback(2,"gradeToggle",NEX_POP,&gradeTogglePopCallback);
  nex.addCallback(2,"speedToggle",NEX_POP,&speedTogglePopCallback);
  nex.addCallback(2,"hrToggle",NEX_POP,&hrTogglePopCallback);
  nex.addCallback(2,"metToggle",NEX_POP,&metTogglePopCallback);
  
}
void initPage3()
{
  nexActivePage = 3;
  delayWithStatusCheck(1000);
  nex.waitForPage();
  
  
  
  nex.addCallback(3,"addButton",NEX_POP,&addButtonPopCallback);
  nex.addCallback(3,"homeButton",NEX_POP,&homeButtonPopCallback);
  nex.addCallback(3,"clearButton",NEX_POP,&clearButtonPopCallback);
  nex.addCallback(3,"startButton",NEX_POP,&startButtonPopCallback);
  nex.addCallback(3,"preview",NEX_POP,&previewPopCallback);
  nex.addCallback(3,"nextButton",NEX_POP,&nextButtonPopCallback);
  nex.addCallback(3,"upButton",NEX_POP,&upButtonPopCallback);
  nex.addCallback(3,"downButton",NEX_POP,&downButtonPopCallback);
  nex.addCallback(3,"customButton",NEX_POP,&customButtonPopCallback);
  drawSpeedMap("t2");
  
  upchirp();
}


void initPage7()
{
  Serial.println("Page6 pressed");
  nexActivePage = 7;
  delayWithStatusCheck(1000);
  nex.waitForPage();
  nex.addCallback(7,"bt0",NEX_POP,&startSpeedmapPopCallback);
  nex.addCallback(7,"bt1",NEX_POP,&startGrademapPopCallback);
  nex.addCallback(7,"b0",NEX_POP,&offsetMinusPopCallback);
  nex.addCallback(7,"b1",NEX_POP,&offsetPlusPopCallback);
  nex.addCallback(7,"alignButton",NEX_POP,&alignPopCallback);
  nex.addCallback(7,"homeButton",NEX_POP,&homeButtonPopCallback);
  nex.addCallback(7,"skipButton",NEX_POP,&skipButtonPopCallback);
  nex.addCallback(7,"mapButton",NEX_POP,&gradeMapButtonPopCallback);
  nex.addCallback(7,"speedButton",NEX_POP,&speedMapButtonPopCallback);
  
  if (gsm.nIntervalsActive == true) nex.setVal("bt0",1); else nex.setVal("bt0",0);
  if (gsm.bActiveGrade == true) nex.setVal("bt1",1); else nex.setVal("bt1",0);
  
}
void addButton (int nPage, char *szButtonID, char *szTitle, void *fPtr)
{
  nex.setTxt(szButtonID,szTitle);
  nex.addCallback(nPage,szButtonID,NEX_POP,fPtr);
}
void initPage8()
{
  Serial.println("Page7 pressed");
  nexActivePage = 8;
  delayWithStatusCheck(1000);
  nex.waitForPage();
  
  upchirp();
  nex.setTxt("b0","5K Run");
  nex.setTxt("b1","Hilly Run");
  nex.setTxt("b2","Volcano KOM Climb");
  nex.setTxt("b3","11 Ocean Blvd");
  nex.setTxt("b4","Mountain Run");
  addButton(8,"b5","Hill Custom 1",&hill1Callback);
  
  
}
void randButtonPopCallback (void *ptr)
{
  //nex.sendCommand("page page1");
  initPage1();
}
void intervalButtonPopCallback (void *ptr)
{
  
  initPage3();
}
void homeButtonPopCallback (void *ptr)
{
  upchirp();
  
  Serial.println("homeButtonPopCallback");
  initPage0();
}

void gradeTogglePopCallback (void *ptr)
{
  quickToneHigh();
  bDrawGrade = nex.getVal("gradeToggle");
  Serial.print("Grade: ");
  Serial.println(bDrawGrade);
}
void speedTogglePopCallback (void *ptr)
{
   quickToneHigh();
  bDrawSpeed = nex.getVal("speedToggle");
  Serial.print("Speed: ");
  Serial.println(bDrawSpeed);
}
void hrTogglePopCallback (void *ptr)
{
  quickToneHigh();
  bDrawHR = nex.getVal("hrToggle");
}
void metTogglePopCallback (void *ptr)
{
  quickToneHigh();
  bDrawMets = nex.getVal("metToggle");
}






void drawNex()
{
  
  

  static long lastNexTime = millis();
  if (millis() - lastNexTime < NEX_UPDATE_INTERVAL) return;
  lastNexTime = millis();
  
  if (nexActivePage == 0)
  {
    
    nex.setTxt("t6",treadmill.fGrade());
    
    nex.setTxt("totalCalories",int(treadmill.fTotalCalories));
    /////////////////////////METS///////////////////////////
    if (!treadmill.bMetsActive) nex.setTxt("metsVal",treadmill.fMets(),2);
    else
    {
      char szMets[8];
      dtostrf(treadmill.targetMets,0,1,szMets);
      strcat(szMets,"*");
      nex.setTxt("metsVal",szMets);
    }
    nex.setTxt("hrVal",btHR);
    nex.setTxt("distVal",treadmill.fDist() + gsm.fDistOffset,2);
    char szPace[16];
    treadmill.getPaceK(szPace);
    char szSpeed[16];
    dtostrf(treadmill.fSpeedK(),0,1,szSpeed);
    char szCombo[64];
    sprintf(szCombo,"%s KPH\r\n%s",szSpeed,szPace);
    
    nex.setTxt("paceVal",szPace);
    
    nex.setTxt("t7",szCombo);
    nex.setTxt("elevVal",treadmill.fElevationGain,0);
    if (gsm.nIntervalsActive == true)
    {
      //Serial.println("SPEED ACTIVE!!");
      //Only once per second, please
      static long activeMillis = millis();
      if (millis() - activeMillis < 1000) return;
      activeMillis = millis();
      nex.setVal("progress",gsm.lambda * 100);
      char disp[120];
      gsm.printIntervalLine(gsm.nIntervalDataIndex,disp);
      //Serial.println(disp);
      drawMessage(disp);
    }
    
   
  }
  if (nexActivePage == 2)
  {
    static long graphInterval = millis();
    if (millis() - graphInterval < 500) return;
    graphInterval = millis();
    byte bGrade = int(255.0 * treadmill.fGrade() / 18.0);
    byte bSpeed = int(255.0 * treadmill.fSpeed() / 10.0);
    byte bHR = constrain(map(btHR,40,170,0,255),0,255);
    
  
    
    byte bMets = int(255.0 * treadmill.fMets() / 15.0);
    //Serial.println("Speed: " + (String)bSpeed);
    //Serial.println("Grade: " + (String)bGrade);
  
    
    
    if (bDrawGrade) nex.addToGraph(2,0,bGrade);
    if (bDrawSpeed) nex.addToGraph(2,1,bSpeed);
    if (bDrawHR)    nex.addToGraph(2,2,bHR);
    if (bDrawMets)  nex.addToGraph(2,3,bMets);
    nex.setTxt("speedText",treadmill.fSpeed());
    nex.setTxt("gradeText",treadmill.fGrade());
    nex.setVal("j0",gsm.lambda * 100);
    char disp[120];
    
 
    drawMessage(disp);
    
  }
  if (nexActivePage == 3)
  {
    if (gsm.nIntervalsActive) drawSpeedMap("t2");
    //nex.setVal("progress",gsm.lambda * 100);  
    return;
  }
  if (nexActivePage == 7)
  {
   
    drawSpeedMap("t7");
    drawGradeMap("t18");
    
    nex.setTxt("t6",int(gsm.fDistOffset * 1000));
    
    char szLandmark[32];
    strcpy(szLandmark,gsm.currentLandmark(& treadmill));
    //Serial.print("Ret: ");
    //Serial.println(szLandmark);
    char shortText[10];
    strncpy(shortText,szLandmark,sizeof(shortText));
    nex.setTxt("alignButton",szLandmark);
    nex.setTxt("bt0",treadmill.fSpeed());
    nex.setTxt("bt1",treadmill.fGrade());
    
  }
  
 
}

 
void startRampCallback(void *ptr)
{
  upchirp();
  treadmill.bMetsActive = true;
  treadmill.targetMets = nex.getFloat("page1.targetMets");
  treadmill.savedTargetMets = treadmill.targetMets;
  treadmill.randomGradeMin = nex.getFloat("minGrade");
  treadmill.randomGradeMax = nex.getFloat("maxGrade");
  
  int  interval = nex.getVal("n0");
  if (interval == 0) interval = 5;
  treadmill.randomGradeInterval = interval;
 
  
  
  treadmill.targetMetsMode = nex.getVal("page1.metsMode") + 1;
  
  
  treadmill.setGrade(treadmill.randomGradeMin);

  if (nex.getVal("c0") == 1)
  {
    //Serial.println("Random HR!!");
    treadmill.targetHR = nex.getFloat("t1");
  }
  else
  {
    treadmill.targetHR = 0;
  }
  treadmill.randomGradeMode = 1;
  treadmill.updateRandomGrade(true);//force start
 
  Serial.println((String)"Target Mets: " + (String)treadmill.targetMets);
  Serial.println((String)"Random Grade Min: " + (String)treadmill.randomGradeMin);
  Serial.println((String)"Random Grade Max: " + (String)treadmill.randomGradeMax);
  Serial.println((String)"Random Grade Interval: " + (String)treadmill.randomGradeInterval);
  Serial.println((String)"Target Mets Mode: " + (String)treadmill.targetMetsMode);
  Serial.println((String)"Target HR: " + (String)treadmill.targetHR);
  //nex.sendCommand("page page0");
  //initPage0();
}
void gsmPopCallback(void *ptr)
{
  upchirp();
  treadmill.bMetsActive = true;
  treadmill.targetMets = nex.getFloat("targetMets");
  treadmill.targetMetsMode = nex.getVal("metsMode") + 1;  //walking or running
  nex.sendCommand("page page0");
  initPage0();
}

void setupNex()
{
  
  nex.nexInit();
  delayWithStatusCheck(1000);
  ////////////////////////PAGE0/////////////////////////

  nex.addCallback(0,17,NEX_POP,&randButtonPopCallback);
  nex.addCallback(0,19,NEX_POP,&graphButtonPopCallback);
  
  
  nex.addCallback(0,29,NEX_PUSH,&gradePlusPlusCallback);
  nex.addCallback(0,30,NEX_PUSH,&gradePlusCallback);//sorry about the capital G
  nex.addCallback(0,4,NEX_PUSH,&gradeMinusCallback);
  nex.addCallback(0,22,NEX_PUSH,&gradeMinusMinusCallback);

  nex.addCallback(0,18,NEX_POP,&metsModeButtonPopCallback);
  nex.addCallback(0,"bt1",NEX_POP,&fanButtonPopCallback);
  nex.addCallback(0,9,NEX_POP,&stopButtonPopCallback);
  nex.addCallback(0,"pauseButton",NEX_POP,&pauseButtonPopCallback);
  nex.addCallback(0,"lidarButton",NEX_POP,&lidarButtonPopCallback);
  nex.addCallback(0,"speakerButton",NEX_POP,&speakerButtonPopCallback);
  
  nex.addCallback(0,5,NEX_PUSH,&speedPlusPlusCallback);
  nex.addCallback(0,25,NEX_PUSH,&speedPlusCallback);
  nex.addCallback(0,6,NEX_PUSH,&speedMinusCallback);
  nex.addCallback(0,26,NEX_PUSH,&speedMinusMinusCallback);
  
  
  ////////////////////////PAGE1/////////////////////////

  nex.addCallback(1,8,NEX_POP,&startRandomButtonPopCallback);
  nex.addCallback(1,2,NEX_POP,&homeButtonPopCallback);
  
  
  
 
  
  
  ///////////////////PAGE6////////////////////////
  nex.addCallback(6,4,NEX_POP,&gradeChangePopCallback);
  nex.addCallback(6,3,NEX_POP,&gradeMinusCallback);
  nex.addCallback(6,4,NEX_POP,&gradePlusCallback);
  /////////////////START THEM UP!/////////////////////
  nex.sendCommand("page page0");
  
}

void flushWiFi()
{

  while(SerialWiFi.available() > 0) 
  {
    char t = SerialWiFi.read();
  }
  SerialWiFi.flush();
}  


////////////////////////////POLL SERIAL AND HANDLE COMMAND////////////////////////
void handleNewCommand(String command)
{
  long startMillis;
  Serial.print("Command was: ");
  Serial.println(command);
  
  if (command == "start")
  {
    Serial.println("STARTING!!");
    treadmill.setSpeed(1.0);
    digitalWrite(FAN_PIN,1);
    return;
    
  }
  if (command == "stop")
  {
   treadmill.bMetsActive = false;
   treadmill.targetMets = 0;
   treadmill.setSpeed(0);
   treadmill.setGrade(0);
   digitalWrite(FAN_PIN,0);
   
   return;
  }
  if (command == "reset") 
  {
    
    treadmill.init();
    flushWiFi();
    Serial.flush();
    SerialWiFi.flush();
    
   
    
    return;
    
  }
  if (command.startsWith("rawCommand="))
  {
    Serial.println("RAWCOMMAND");
    int beg = command.indexOf("[");
    int ending = command.indexOf("]");
    
    String c = command.substring(beg,ending + 1);
    
    
    
    ///JSON IT
    StaticJsonBuffer<200> jsonBuffer;
    JsonArray& array2 = jsonBuffer.parseArray(c);
    //Serial.println("--------");
    //Serial.println(beg);
    //Serial.println(ending);
    ////Serial.println(c);
    //array2.printTo(Serial);
    byte command[16];
    int cSize = 0;
    for (cSize = 0;cSize<array2.size();cSize++) command[cSize] = (byte)array2[cSize];
    treadmill.sendCommand("rawCommand: ",command,cSize);
   
    upchirp();
    return;
  }
  if (command.startsWith("speed="))
  {
    //clear targetMets
    treadmill.targetMets = 0;
    treadmill.bMetsActive = false;
    String speed = command.substring(6);
    treadmill.setSpeed(speed.toFloat());
    upchirp();
    return;
    
  }
  if (command.startsWith("grade="))
  {
    Serial.println("success");
    String grade = command.substring(6);
    treadmill.setGrade(grade.toFloat());
    upchirp();
    return;
    
  }
  if (command.startsWith("rgr="))
  {
    Serial.println("success");
    String rgr = command.substring(4);
    treadmill.setRawGrade(rgr.toInt());
    upchirp();
    return;
    
  }
  if (command.startsWith("cal"))
  {
    int rawGrade = 0;
    Serial.println("success");
    treadmill.setRawGrade(rawGrade);
    treadmill.checkStatus();
    delay(100);
    while (treadmill.moving)
    {
      treadmill.checkStatus();
      delay(100);
    }
    while (rawGrade < 32767)
    {
      rawGrade += 64;
      treadmill.setRawGrade(rawGrade);
      treadmill.checkStatus();
      delay(100);
      while (treadmill.moving)
      {
        treadmill.checkStatus();
        delay(100);
      }
      Serial.print(rawGrade);
      Serial.print(",");
      Serial.print(treadmill.gradeWord);
      Serial.print(",");
      Serial.print(treadmill.fGrade());
      Serial.print(",");
      Serial.print(treadmill.statusWord,BIN);
      Serial.print(",");
      Serial.print(treadmill.statusWord2,BIN);
      Serial.print(",");
      Serial.println(treadmill.statusWord3,BIN);
      
      
      
    }
    
  }
  if (command.startsWith("="))
  {
    int startGrade = treadmill.gradeWord;
    int inc = 0;
    while (!treadmill.moving)
    {
      Serial.print(".");
      Serial.println(inc);
      treadmill.setRawGrade(startGrade + inc);
      delay(150);
      treadmill.checkStatus();
      inc+=128;
      
      
      
      
    }
    //treadmill.setRawGrade(treadmill.gradeWord + 128);
    for (int i = 0;i<10;i++)
    {
      treadmill.checkStatus();
      Serial.println(treadmill.gradeWord);
      delay(150);
    }
  }

  if (command.startsWith("setTargetMets="))
  {
    Serial.println("success");
    treadmill.bMetsActive = true;
    String szTargetMets = command.substring(14);
    treadmill.targetMets = szTargetMets.toFloat();
    //Serial.println("TargetMets: " + szTargetMets);
    //treadmill.doubleChirp();
    return;
    
  }
  if (command.startsWith("setTargetMetsMode="))
  {
    Serial.println("success");
    String szTargetMetsMode = command.substring(18);
    treadmill.targetMetsMode = szTargetMetsMode.toFloat();
    //Serial.println("TargetMetsMode: " + szTargetMetsMode);
    doubleTone();
    return;
    
  }
  if (command.startsWith("setTargetHR="))
  {
    treadmill.bMetsActive =true;
    if (treadmill.targetMets == 0) treadmill.targetMets = treadmill.fMets();
    //Serial.println("HR=");
    String szTargetHR = command.substring(12);
    treadmill.targetHR = szTargetHR.toInt();
    //Serial.println("targetHR: " + treadmill.targetHR);
    //treadmill.doubleTone();
    return;
    
  }
  if (command.startsWith("fan="))
  {
    
    String szFan = command.substring(4);
    if (szFan == "1") treadmill.fan(1);else treadmill.fan(0);
    return;
    
  }
  
  if (command == "infoStr")
  {
    startMillis = millis();
    //Serial.println("InfoStr request received");
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["speed"] = treadmill.fSpeed();
    root["grade"] = treadmill.fGrade();
    root["status"] = treadmill.statusWord;
    root["moving"] = treadmill.moving;
    root["error"] = treadmill.error;
    root["targetMets"] = treadmill.targetMets;
    
    String ret;
    root.printTo(ret);
    
    //Serial.print("returning: ");
    //Serial.println (ret);
    SerialWiFi.println(ret);
    long totalMillis = millis() - startMillis;
    //Serial.print("Latency msec: ");
    //Serial.println(totalMillis);
    return;
    
  }
  
  ///error unrecognized command
  String ret = "{\"error\":\"" + command + "\"}";
  
     
  SerialWiFi.println(ret);
  
  
  
}
void pollSerial()
{
 static int HRIndex = 0;
 static int cIndex = 0;
 static int cIndexBT = 0;
 static char szHR[8];
  while (Serial.available() > 0)
  {
    char ch = Serial.read();
    currentCommand[cIndex] = ch;
    cIndex++;
    if (cIndex>=sizeof(currentCommand)) bufferError("Serial Port");
    if (ch == '\r') 
    {
      currentCommand[cIndex - 1] = 0;
      handleNewCommand((String)currentCommand);
      
      cIndex = 0;
    }
  }
  while (SerialWiFi.available() > 0)
  {
    char ch = SerialWiFi.read();
    currentCommand[cIndex] = ch;
    cIndex++;
    //Serial.println(currentCommand);
    if (cIndex>=sizeof(currentCommand)) 
    {
      cIndex = 0;
      return;
      //bufferError("Serial WiFi");
    }
    if (ch == '\r') 
    {
      //printBytes(currentCommand,sizeof(currentCommand));
      currentCommand[cIndex - 1] = 0;
      
      handleNewCommand((String)currentCommand);
      
      cIndex = 0;
    }
  }
  while (SerialBT.available() > 0)
  {
    
    char ch = SerialBT.read();
    if (ch == 0x0a) return; //ignore LF
    currentCommandBT[cIndexBT] = ch;
    cIndexBT++;
    //Serial.println(currentCommand);
    if (cIndexBT>=sizeof(currentCommandBT)) 
    {
      cIndexBT = 0;
      return;
      //bufferError("Serial BT");
    }
    if (ch == '\r') 
    {
      //printBytes(currentCommand,sizeof(currentCommand));
      for (int i = 0;i<cIndexBT;i++)
      {
        Serial.print(currentCommandBT[i],HEX);
        Serial.print(" ");
      }
      Serial.println();
      currentCommandBT[cIndexBT - 1] = 0;
      
      handleNewCommand((String)currentCommandBT);
      
      cIndexBT = 0;
    }
  }
  
}

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
/*
void rainbow() 
{
   strip.setBrightness(32);
   float f = fmap(fRange,0,100,0,7);
   if (f >=7)
   {
     strip.setBrightness(64);
     strip.fill(strip.gamma32(strip.Color(0,255,0)));
     strip.show();
     return;
      
   }
   if (f <=0.25)
   {
     strip.setBrightness(64);
     strip.fill(strip.gamma32(strip.Color(255,0,0)));
     strip.show();
     return;
      
   }
   
   int nVal = int(f);
   
   float  fFrac = f - int(f);
   int nFrac = int(fFrac * 255.0);
  
   {
    for(int i=0; i<strip.numPixels(); i++) 
    { 
      int nHue = 0 + (i * 65536L / strip.numPixels());
      int nSaturation = 255;
      int nValue = 255;
     
      if (i<nVal)    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(nHue,nSaturation,nValue)));
      if (i == nVal) strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(nHue,nSaturation,nFrac)));
      if (i > nVal)  strip.setPixelColor(i,strip.gamma32(strip.Color(0, 0, 0)));
    }
    strip.show(); // Update strip with new contents
   
  }
}
*/
void setup(void)
{
  //////////////////NEOPIXELS//////////////////////////////
  //strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  //strip.show();            // Turn OFF all pixels ASAP
  //strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  
  setupNex();
  
  Serial.begin(115200);
  
  delay(1);
  gsm.init();
  

  ////////////////////LIDAR////////////////////////////////
  //Wire.begin();
  //Serial.println("Starting lidar...");
  //lidar.init();
  //Serial.println("Lidar Started...");
  //lidar.setTimeout(500);
  //lidar.startContinuous();
  //lidar.setMeasurementTimingBudget(30000);
  /////////////////////TREADMILL/////////////////////
  SerialTreadmill.begin(9600);              //MOTOR IN
  SerialWiFi.begin(19200);
  SerialBT.begin(57600);
  
  Serial.println("Starting Treadmill");
  treadmill.init();

  
  
  ////////////////////ACCELEROMETER////////////////////////////////
  //Serial.println("Starting Accelerometer...");
  //if (! lis.begin(0x18)) 
  //{   // change this to 0x19 for alternative i2c address
  //  Serial.println("Couldnt start");
  //  while (1);
 // }
 //  lis.setRange(LIS3DH_RANGE_4_G);
  
  
  
  
  
  chirp();

  initPage0();//goto the firtst page
  //rainbow();
  
  
  
  
  
  
 
  
}

void pollKeypad()
{
  char key = keypad.getKey();
  if (key != NO_KEY) 
  {
    Serial.print("KEYPAD: ");
    Serial.println(key);
    
    if (key == 'f')//faster
    {
      
      if (treadmill.bMetsActive > 0) 
      {
        treadmill.targetMets +=0.1;
        if (treadmill.targetMets > 16) treadmill.targetMets = 16.0;
        
      }
      
      else 
      {
        if (treadmill.fSpeed() < 0.5) 
        {
          treadmill.setSpeed(0.5);
          
          return;
        }
        treadmill.setSpeed(treadmill.fSpeed() + 0.25);
      }
      chirp();
     
      
            
    }
    if (key == 's')//slower
    {
       if (treadmill.bMetsActive > 0) 
       {
         treadmill.targetMets -= 0.1;
         if (treadmill.targetMets < 0) treadmill.targetMets = 0.0;
       }
       else treadmill.setSpeed(treadmill.fSpeed() - 0.25);
       chirp();
      
    }
    
    if (key == 'r')//reset
    {
      chirp();
      treadmill.setGrade(0);
      treadmill.setSpeed(0);
      
      treadmill.resetVariables();
      
     
    }
    if (key == 'u') 
    {
      
      treadmill.setGrade (treadmill.fTargetGrade + 1);
      chirp();
    }
    if (key == 'd') 
    {
      
      treadmill.setGrade (treadmill.fTargetGrade - 1.0);
      chirp();
      
    }
    
  }
}





void updateTargetHR()
{
  static int intervalSecs = 5;
  static long intervalMillis = intervalSecs * 1000;
  
  static long lastCheck = millis();
  static int lastHR = btHR;
  if ((millis() - lastCheck) < intervalMillis) return;
  lastCheck = millis();
  
  
  if (treadmill.targetHR <= 0) return;
  
  //Serial.println("HR CHECK: ");
  
  if (btHR <= 40) return;
  float deltaHRPerSec = ((float)btHR - (float)lastHR) / (float) intervalSecs;
  int extrapolatedHR = 15 * deltaHRPerSec + btHR;
  
  
  float hrPctDiff = (float)treadmill.targetHR / (float)extrapolatedHR;
  if (hrPctDiff > 1.10) hrPctDiff = 1.10;
  if (hrPctDiff < 0.90) hrPctDiff = 0.90;
  
  treadmill.targetMets = treadmill.targetMets * hrPctDiff;
  treadmill.targetMets = constrain(treadmill.targetMets,1,13.5);
  //Serial.print("BTHR: ");
  //Serial.println(btHR);
  //Serial.println("Extrapolated HR" + (String)extrapolatedHR);
  //Serial.println("targetHR: " + (String)treadmill.targetHR);
  //Serial.println ("pctDiff: " + (String)hrPctDiff);
  //Serial.println("Updated Target Mets: " + (String)treadmill.targetMets);
  lastHR = btHR;
  
}





void drawSpeedMap(char *button)
{
 
  char final[512] = "";
  for (int i = 0;i<min(gsm.nIntervalMax,INTERVAL_MAX);i++)
  {
    char disp[80];
    gsm.printIntervalLine(i,disp);
    
    strcat(final,disp);
    strcat(final,"\r\n");
  }
  if (gsm.nIntervalMax == gsm.nIntervalDataIndex) strcat(final,"->\r\n");
  
  
  nex.setTxt(button,final);
  
  
}
void drawGradeMap(char *button)
{
  unsigned long startTime = millis();
  char szFinal[512] = "";
  for (int i = gsm.nGradeDataIndex;i<min(gsm.nGradeDataIndex + 5,gsm.nGradeSize);i++)
  {
    
    char szDist[10];
    char szGrade[10];
    gradeDataType g;
    gsm.fetchGradeItem(i,&g);
    if (g.dist>=0)
    {
      dtostrf(g.dist - (treadmill.fDist() + gsm.fMapOffset),0,2,szDist);
      dtostrf(g.grade,0,2,szGrade);
    
      
      strcat(szFinal,szDist);
      strcat(szFinal,",");
      strcat(szFinal,szGrade);
     
      
      strcat(szFinal,"\r\n");
    }
    
    
  }
  
  nex.setTxt(button,szFinal);
  //Serial.println(szFinal);
}
void drawMapPage()
{
  
  
  
  
  
}





void bufferError (char *szMessage)
{
  Serial.print("LoLevel BUFFER ERROR: ");
  Serial.println (szMessage);
  drawMessage(szMessage);
  chirp();
}

void updateBTSpeed()
{
  static unsigned long lastUpdate = millis();
  static float lastSpeed = 0;
  static float lastGrade = 0;
 
  if (millis() - lastUpdate<1000) return;
  lastUpdate = millis();
  if (treadmill.fSpeed() != lastSpeed)
  {
    lastSpeed = treadmill.fSpeed();
    char szSpeed[8];
    dtostrf(lastSpeed,0,2,szSpeed);
    SerialBT.print("s:");
    SerialBT.println(szSpeed);
  }
  if (treadmill.fGrade() != lastGrade)
  {
    lastGrade = treadmill.fGrade();
    char szGrade[8];
    dtostrf(lastGrade,0,2,szGrade);
    SerialBT.print("g:");
    SerialBT.println(szGrade);
  }
}

void updateLidar ()
{
  /*#define AVERAGE_NUM 20
  #define LIDAR_NEAR_DIST 220
  #define LIDAR_FAR_DIST 600
  static int ranges[AVERAGE_NUM] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  static int nRangeIndex = 0;

 
  
  
 
  
  if (treadmill.fTargetSpeed < 0.5) return;
  
   
  static unsigned long lastUpdated = millis();
  static int speedCount = 0;
  
  
  int i = nRangeIndex % AVERAGE_NUM;
  int r = lidar.readRangeContinuousMillimeters();
  //Serial.println(r);
  ranges[i] = r;
  nRangeIndex++;
  
  int nAverageRange = 0;
  for (int i = 0;i<AVERAGE_NUM;i++) nAverageRange += ranges[i];
  int nRange = nAverageRange / AVERAGE_NUM;
  
  
  
  fRange = constrain(map(nRange,LIDAR_NEAR_DIST,LIDAR_FAR_DIST,100,0),0,100);
  
  rainbow();
  if (millis() - lastUpdated < 1500) return;
  if ((nRange < LIDAR_NEAR_DIST) && (nRange > -1))
  {
    lastUpdated = millis();
    
    speedCount++;
    if (speedCount>4) 
    {
      treadmill.setSpeed(treadmill.fTargetSpeed + 1.0);
      quickToneHigh();
      return;
    }
    incSpeed();
    return;
    
    
    
  }
  if ((nRange > LIDAR_FAR_DIST) && (nRange < 1000) && (treadmill.fSpeed() > 0.5)) 
  {
    lastUpdated = millis();
    speedCount++;
    if (speedCount > 4)
    {
      treadmill.setSpeed(treadmill.fTargetSpeed - 1.0);
      quickToneHigh();
      return;
    }
    else
    {
      decSpeed();
      return;  
    }
    
  }
  
  //Neither too small or too great
  speedCount = 0;
  
  if (lidar.timeoutOccurred()) 
  { 
    drawMessage("LIDAR TIMEOUT ERROR");
  }
  

  */
}
void updateAccel()
{
  return;/*
  float fMin,fMax;
  unsigned long startMillis = millis();
  sensors_event_t event;
  int i = 0;
  while (millis() - startMillis < 400)
  {
    
    lis.getEvent(&event);
    float x = event.acceleration.x;
    float y = event.acceleration.y;
    float z = event.acceleration.z;
    //normalize
    fThump = sqrt(x*x + y*y + z*z);
    if (i == 0)
    {
      fMin = fThump;
      fMax = fThump;
      i = 1;
    }
    if (fThump>fMax) fMax = fThump;
    if (fThump<fMin) fMin = fThump;
  }
  fMaxThump = fMax - fMin;
  nex.setTxt("g0",fMaxThump);
  
  //Serial.println(fMaxThump);
  
  */
  
  
}


void delayWithStatusCheck (unsigned long st)
{
   unsigned long stopTimer = millis() + st;
   while (millis() < stopTimer)
   {
     treadmill.checkStatus();
   }
}
void loop(void)
{
  
  
  updateBTSpeed();
  updateLidar();
  updateAccel();
  
  
  treadmill.checkStatus();
  
  
  pollSerial();
  pollKeypad();
  treadmill.handleMets();
  updateTargetHR();
  
  nex.nexListen();
  drawNex();
  
  treadmill.updateRandomGrade();
  
  
  
  gsm.handle(&treadmill);
  
  
  
 
}
