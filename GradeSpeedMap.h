#ifndef __GRADESPEEDMAP__
#define __GRADESPEEDMAP__

#include <Arduino.h>
#include "Treadmill.h"
#include "GradeData.h"
#include <HardwareSerial.h>


 
class GradeSpeedMap
{
  public:
  

  #define MILE_CONVERSION_MUL 0.000621371
  
  float zwiftSpeeds[5] = 
  {
    8.5918854429,
    8.023454806,
    7.697657261,
    7.360573924,
    7.039916437
  };
  
  
  float fDistOffset = 0;
  float fMapOffset = 0;
  //////////////GRADE STUFF/////////
  float fGradeMultiplier = 1;
  float fGradeZero = 0;
  bool bActiveGrade = false;
  int nGradeSize = -1;
  float lambda = 0;
  
  
  gradeDataType *gradeData;
  
  int nGradeDataIndex = 0;
  
  int nGradeMapIndex = 0;

  ///////////////LANDMARK STUFF/////////////////////
  int nLandmarkDataIndex = 0;
  
  
  landmarkDataType *landmarkData;
  
  
  
  



////////////SPEED AND SPEED RAMP///////////////
bool nIntervalsActive = false;
float intervalStartMiles = 0;
float intervalStartMins = 0;
float intervalStartFeet = 0;

int nIntervalDataIndex = 0;
int nIntervalMax = 0;
#define INTERVAL_MAX 10


#define SPEED_PCT_1M 0
#define SPEED_PCT_5K 1
#define SPEED_PCT_10K 2
#define SPEED_PCT_HM 3
#define SPEED_PCT_M 4
#define SPEED_METS 5
#define SPEED_PACE 6
#define SPEED_SPEED 7

#define TIMING_MILES 0
#define TIMING_METERS 1
#define TIMING_MINUTES 2
#define TIMING_ELEVATION 3

#define RAMP_LINEAR 0
#define RAMP_RANDOM 1
#define RAMP_V 2

typedef struct
{
  int nTimingMode;  //0 = change speed and grade corresponding with distance (in miles)
                    //1 = change speed and grade corresponding with distance (in meters)
                    //2 = change speed and grade corresponding with time (in minutes)
                    //3 = change speed and grade corresponding with elevation (in feet)
            
  int nSpeedMode;   //See SPEED_PCT constants
  int nRampMode;    //0 = linear ramp between grade0 and grade1
                    //1 = random numbers between grade0 and grade1
                    //2 = V.  Linear up to lambda 0,.5 and back down again
  float dist;       //in miles meters minutes or elevation gain depending on nTimingMode
  float speed0;     //either mph or mets depending on nSpeedMode
  float speed1;     //either mph or mets depending on nSpeedMode
  float grade0;     //given as a percentage * 10.  E.G.  10.5 percent grade is 10.5
  float grade1;     //given as a percentage * 10.  E.G.  10.5 percent grade is 10.5
  int reps;
  int originalReps;
  int lastSteps;
}IntervalType;
IntervalType intervalData[INTERVAL_MAX];
  
  
  
  public:
  void init();
  void print();
  void initPace(int index,int hours,int mins,int secs);
  float metersToMiles (float meters);
  float calcSpeedFromMets(float m,float g);
  float getSpeed(float pct,int nIndex);
  float getIntervalSpeed(int nIndex,int nFastSlow);
  
  
 
  
  float paceToSpeed(float f);
  void clearIntervals();
  void addInterval (int nSpeedMode,int nTimingMode, float distance,float pct0,float pct1,int nRampMode = RAMP_LINEAR,float grade0 = 1,float grade1 = 1,int reps = 1,int lastSteps = 0);
  void getPace(float spd,char *ret);
  void printIntervalLine (int index,char *disp);
  
  void intervalDone(Treadmill *treadmill,IntervalType *ci);
  
  
  void handleInterval (Treadmill *treadmill);
  void handleGrade(Treadmill *treadmill);
  
  
 
  void handle (Treadmill *treadmill);
  char *currentLandmark(Treadmill *treadmill);
  void alignLandmarks(Treadmill *treadmill);
  void fetchGradeItem(int i,gradeDataType *gp);
  void fetchLandmarkItem(int i,landmarkDataType *lp);
  void startIntervals(Treadmill *treadmill);
  
  
  
};
#endif
