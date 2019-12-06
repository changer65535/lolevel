#include "GradeSpeedMap.h"
#include <Arduino.h>



float GradeSpeedMap::metersToMiles (float meters)
{
  if (meters == 400) return 0.25;
  if (meters == 800) return 0.50;
  if (meters == 1200) return 0.75;
  if (meters == 1600) return 1.0;
  if (meters == 2000) return 1.25;
  return meters * MILE_CONVERSION_MUL;
}


float GradeSpeedMap::getSpeed(float pct,int nIndex)
{
  return pct / 100.0 * zwiftSpeeds[nIndex];
}

float GradeSpeedMap::calcSpeedFromMets(float m,float g)
{
  
  float c1,c2;
  if (true)
  {
    c1 = 0.2;
    c2 = 0.9;
  }
  float mpm = (m * 3.5 - 3.5) / (c1 + c2 * g / 100.0);
  return mpm / 26.8224;
}
float GradeSpeedMap::getIntervalSpeed(int nIndex,int nFastSlow)
{
  IntervalType *ci = &intervalData[nIndex];
  float fSpeed = ci->speed0;
  float fGrade = ci->grade0;
  if (nFastSlow == 1) 
  {
    fSpeed = ci->speed1;
    fGrade = ci->grade1;
  }
  if (ci->nSpeedMode <= SPEED_PCT_M) return getSpeed(fSpeed,ci->nSpeedMode);
  if (ci->nSpeedMode == SPEED_METS) return (calcSpeedFromMets(fSpeed,fGrade));  //in this case, fSpeed should be the mets
  if (ci->nSpeedMode == SPEED_PACE) return (paceToSpeed(fSpeed));
  if (ci->nSpeedMode == SPEED_SPEED) return (fSpeed); 
}
float GradeSpeedMap::paceToSpeed(float f)  //returns mph
{
  if (f == 0) return 0;
  return (60.0/f);
}
void GradeSpeedMap::addInterval (int nSpeedMode,int nTimingMode,float distance,float pct0,float pct1,int nRampMode,float grade0,float grade1, int reps,int lastSteps)
{
  IntervalType *ci = &intervalData[nIntervalDataIndex];
  if (nRampMode == RAMP_RANDOM)
  {
    if (abs(grade0 - grade1) < 2) nRampMode = RAMP_LINEAR; // otherwise, it will hang
  }
  ci->nSpeedMode = nSpeedMode;
  
  ci->speed0 = pct0;
  ci->speed1 = pct1;
  ci->nRampMode = nRampMode;
  ci->grade0 = grade0;
  ci->grade1 = grade1;
  
  ci->nTimingMode = nTimingMode;
  ci->dist = distance;
 
  
  
  ci->reps = reps;
  ci->originalReps = reps;  //so we can reset the reps after a loop is over
  ci->lastSteps = lastSteps;
  if (nIntervalMax == nIntervalDataIndex)
  {
    nIntervalMax++;
    nIntervalDataIndex++;
  }
  
  
  
}



void GradeSpeedMap::getPace(float spd,char *ret)
{

  
  
  if (spd == 0) 
  {
    strcpy(ret,"");
    return;
  }
  int nSecondsPerMile = round(3600.0 / spd);
  
  
  int nSecs = nSecondsPerMile % 60;
  int nMins = nSecondsPerMile / 60;
  

  char szMins[16];
  char szSecs[16];
  itoa(nSecs,szSecs,10);
  itoa(nMins,szMins,10);
  
  
  
  char buffer[64];
  strcpy(buffer,szMins);
  strcat(buffer,":");
  if (nSecs<10) strcat(buffer,"0");
  strcat(buffer,szSecs);
  strcpy (ret,buffer);
  //Serial.println(buffer);
  
  
    
}
void GradeSpeedMap::printIntervalLine (int index,char *disp)
{
    disp[0] = 0;
    IntervalType *interval = &intervalData[index];
    if (index < 0) 
    {
      disp = "";
      return;
    }
    char szIndex[6] = "  ";
    if (index == nIntervalDataIndex) strcpy(szIndex,"->");
    
    char szSpeedMode[16];
    if (interval->nSpeedMode == SPEED_PCT_1M) strcpy(szSpeedMode,"Pct 1M");
    if (interval->nSpeedMode == SPEED_PCT_5K) strcpy(szSpeedMode,"Pct 5K");
    if (interval->nSpeedMode == SPEED_PCT_10K) strcpy(szSpeedMode,"Pct 10K");
    if (interval->nSpeedMode == SPEED_PCT_HM) strcpy(szSpeedMode,"Pct HM");
    if (interval->nSpeedMode == SPEED_PCT_M) strcpy(szSpeedMode,"Pct M");
    if (interval->nSpeedMode == SPEED_METS) strcpy(szSpeedMode,"METS");
    if (interval->nSpeedMode == SPEED_PACE) strcpy(szSpeedMode,"Pace");
    if (interval->nSpeedMode == SPEED_SPEED) strcpy(szSpeedMode,"Speed");

    char szSpeed0[10];dtostrf(interval->speed0,0,2,szSpeed0);
    char szSpeed1[10];dtostrf(interval->speed1,0,2,szSpeed1);

    char szRampMode[32] = "";
    if (interval->nRampMode == RAMP_LINEAR) strcpy(szRampMode,"Linear");
    if (interval->nRampMode == RAMP_RANDOM) strcpy(szRampMode,"Random");
    if (interval->nRampMode == RAMP_V) strcpy(szRampMode,"V");

    char szGrade0[8];dtostrf(interval->grade0,0,1,szGrade0);
    char szGrade1[8];dtostrf(interval->grade1,0,1,szGrade1);

    
    char szTimingMode[8];
    if (interval->nTimingMode == TIMING_MILES) strcpy(szTimingMode,"miles");
    if (interval->nTimingMode == TIMING_METERS) strcpy(szTimingMode,"meters");
    if (interval->nTimingMode == TIMING_MINUTES) strcpy(szTimingMode,"min");
    if (interval->nTimingMode == TIMING_ELEVATION) strcpy(szTimingMode,"Elev ft");
    
    char szDist[10];
    dtostrf(interval->dist,0,2,szDist);
    
    
    
    
    char szLastSteps[32] = "";
    if (interval->lastSteps > 0) sprintf(szLastSteps," last %d steps",interval->lastSteps + 1);

    char szRepeat[32] = "";
    if (interval->reps > 1) sprintf(szRepeat,"repeat x%d",interval->reps);
    
    sprintf(disp,"%s %s %s %s %s %s %s to %s %s %s %s",szIndex,szSpeedMode,szSpeed0,szSpeed1,szRampMode,szGrade0,szGrade1,szDist,szTimingMode,szLastSteps,szRepeat);
    

   
    
    
}

void GradeSpeedMap::clearIntervals()
{
  nIntervalDataIndex = 0;
  nIntervalMax = 0;
  lambda = 0;
  nIntervalsActive = false;
}
void GradeSpeedMap::startIntervals(Treadmill *treadmill)
{
  nIntervalDataIndex = 0;
  intervalStartMins = millis() / 60000.0;
  intervalStartMiles = treadmill->fDist();
  intervalStartFeet = treadmill->fElevationGain;
  nIntervalsActive = true;
  
}

void GradeSpeedMap::initPace(int index,int hours,int mins,int secs)
{
  float fMiles = 0;
  switch (index)
  {
    case SPEED_PCT_1M:fMiles = 1;break;
    case SPEED_PCT_5K:fMiles = 3.1066;break;
    case SPEED_PCT_10K:fMiles = 6.21372;break;
    case SPEED_PCT_HM:fMiles = 13.11;break;
    case SPEED_PCT_M:fMiles = 26.21;break;
    default:return;
  }
  float fHours = (float)hours + (float)mins / 60.0 + (float)secs / 3600.0;
  float fSpeed = fMiles / fHours; 
  zwiftSpeeds[index] = fSpeed;
}
void GradeSpeedMap::init ()
{
  //////////////////PACES.  DO NOT ERASE!!//////////SAVESAVESAVE
  initPace(SPEED_PCT_1M,0,6,50);
  initPace(SPEED_PCT_5K,0,22,46);
  initPace(SPEED_PCT_10K,0,47,29);
  initPace(SPEED_PCT_HM,1,44,48);
  initPace(SPEED_PCT_M,3,35,52);
  
  ///////////////////////////As Of 5/20/2019...
  

  memset(intervalData,0,sizeof(intervalData));
  
  nIntervalMax = 0;


  ///////////WORKOUT GOES HERE

  //addZwiftRamp(1600,70,90,HM_PACE);
  //addZwiftRamp(1600*6,98,110,HM_PACE);
  //addZwiftRamp(1600,90,60,HM_PACE);
  
  
    
  
  ////////////////////////////////////////////
  
  
  
  gradeData = &volcanoClimb[0];
  landmarkData = &volcanoClimbLandmarks[0];
  nGradeSize = MAX_VOLCANO;
  
  //print();
  
  bActiveGrade = false;
  nIntervalsActive = false;
  

  
}

void GradeSpeedMap::print()
{
  Serial.println("------------GRADE: ");
  for (int i = 0;i<5;i++)
  {
    Serial.print("Dist: ");
    gradeDataType g;
    fetchGradeItem(i,&g);
    Serial.print(g.dist);
    Serial.print(" Grade: ");
    Serial.println(g.grade);
  }
  Serial.println("--------------SPEED:");
  Serial.print("nIntervalMax:");
  Serial.println(nIntervalMax);
  for (int i = 0;i<INTERVAL_MAX;i++)
  {
    
    Serial.print(" Speed0: ");
    Serial.print(intervalData[i].speed0);
    Serial.print(" Dist1: ");
    Serial.print(intervalData[i].dist);
    Serial.print(" Speed1: ");
    Serial.println(intervalData[i].speed1);
  }
  

  

}
void GradeSpeedMap::handleGrade(Treadmill *treadmill)
{

  gradeDataType g;
  fetchGradeItem(nGradeDataIndex,&g);
  //if (nGradeDataIndex >= MAX_HILLY) return;
  if (g.dist < 0) return;
  
  
  
  
  if (treadmill->fDist()  + fMapOffset < g.dist) return;
  
  //Serial.println("Changing Grade");
  //Serial.print("Dist: ");
  //Serial.print(g.dist);
  //S///erial.print(" Speed: ");
  //Serial.println(gradeData[nGradeDataIndex].grade);
  float fTempGrade = g.grade;
  
  //if (g.grade == 0) fTempGrade = 1;
  //if (g.grade == 1) fTempGrade = 1.5;
  //if (g.grade == 2) fTempGrade = 2;
  //if (g.grade >= 3) fTempGrade = (float)g.grade;
   
  //float fFinalGrade = fGradeMultiplier * fTempGrade + fGradeZero;
  //float fFinalGrade = constrain(fTempGrade,0.0,18.0);
  treadmill->setGrade(fTempGrade);
  
  nGradeDataIndex++;  //pop
  
 
  
  
}


void GradeSpeedMap::intervalDone(Treadmill *treadmill,IntervalType *ci)
{
  intervalStartMiles = treadmill->fDist();
  intervalStartMins = millis()/60000.0;
  intervalStartFeet = treadmill->fElevationGain;
  Serial.print("Reps: ");Serial.println(ci->reps);
  Serial.print("IntervalDataIndex: ");Serial.println(nIntervalDataIndex);
  ci->reps--;
  if (ci->reps <=0) //Done with the reps... Move on
  {
    Serial.println("Reps are less than or equal to zero!");
    ci->reps = ci->originalReps;
    nIntervalDataIndex++;
    if (nIntervalDataIndex >= nIntervalMax)
    {
      nIntervalsActive = false;
    }
    return;
  }
  Serial.println("MoreReps To Do!");
  //Still more reps to do
  
  nIntervalDataIndex = nIntervalDataIndex - ci->lastSteps;
  if (nIntervalDataIndex<0) nIntervalDataIndex = 0;
  Serial.println(nIntervalDataIndex);
  

}
void GradeSpeedMap::handleInterval (Treadmill *treadmill)
{
  if (nIntervalDataIndex < 0) return;//in case we back tracked too far via user error
  if (nIntervalDataIndex >= nIntervalMax) return;
  if (nIntervalDataIndex >= INTERVAL_MAX) return;
  IntervalType *ci = &intervalData[nIntervalDataIndex];//current interval
  
  if (ci->dist == 0) return;
  
  //Are we done with the interval, yet?
  
  if (ci->nTimingMode == TIMING_MILES) lambda = (treadmill->fDist() - intervalStartMiles) / (ci->dist);
  if (ci->nTimingMode == TIMING_METERS) lambda = (treadmill->fDist() - intervalStartMiles) / metersToMiles(ci->dist);
  if (ci->nTimingMode == TIMING_MINUTES) lambda = (millis()/60000.0 - intervalStartMins) / (ci->dist);
  if (ci->nTimingMode == TIMING_ELEVATION) lambda = (treadmill->fElevationGain - intervalStartFeet) / (ci->dist);
  
  if (lambda < 1)////////////////////////////////////////////////PROCESS LAMBDA///////////////
  {
    
    ////mets or not
    if (ci->nSpeedMode == SPEED_METS) treadmill->bMetsActive = true; else 
    {
      treadmill->bMetsActive = false; 
      treadmill->targetMets = 0; 
    }
     
    ///Speed
    if (ci->nSpeedMode <= SPEED_PCT_M)
    {
      treadmill->bMetsActive = false;

      float s0 = getSpeed(ci->speed0,ci->nSpeedMode);
      float s1 = getSpeed(ci->speed1,ci->nSpeedMode);
      float finalSpeed = s0;
      finalSpeed = s0 + lambda * (s1 - s0);
      treadmill->setSpeed(finalSpeed);
    }

    if (ci->nSpeedMode == SPEED_PACE)
    {
      

      float s0 = paceToSpeed(ci->speed0);
      float s1 = paceToSpeed(ci->speed1);
      float finalSpeed = s0 + lambda * (s1 - s0);
      treadmill->setSpeed(finalSpeed);
    }
    if (ci->nSpeedMode == SPEED_SPEED)
    {
      

      float s0 = ci->speed0;
      float s1 = ci->speed1;
      float finalSpeed = s0 + lambda * (s1 - s0);
      treadmill->setSpeed(finalSpeed);
    }
    
    if (ci->nSpeedMode == SPEED_METS)
    {
      treadmill->bMetsActive = true;
      float m0 = ci->speed0;
      float m1 = ci->speed1;
      
      float finalMets = m0 + lambda * (m1 - m0);
      treadmill->targetMets = finalMets;
      
    }
    

    
    ///Grade
    
    if (ci->nRampMode == RAMP_LINEAR)
    {
      float finalGrade = ci->grade0;
      if (ci->grade1 > -10) finalGrade = ci->grade0 + lambda * (ci->grade1 - ci->grade0);
      treadmill->setGrade(finalGrade);
      
    }
    if (ci->nRampMode == RAMP_V)
    {
      float lambdaV = 1 - abs(2*lambda-1);
      float finalGrade = ci->grade0;
      if (ci->grade1 > -10) finalGrade = ci->grade0 + lambdaV * (ci->grade1 - ci->grade0);
      treadmill->setGrade(finalGrade);
      
    }
    if (ci->nRampMode == RAMP_RANDOM)
    {
      static unsigned long rampDelay = millis();
      if (millis() - rampDelay<5000) return;//5 second delay
      rampDelay = millis();
      float currentGrade = treadmill->fGrade();
      do
      {
       float f = (float)random(-100,100) / 100.0;
       currentGrade +=f;
      }
      while ((currentGrade < ci->grade0) || (currentGrade > ci->grade1));
     
      treadmill->setGrade(currentGrade);
      
    }
    
    
    
    
    
  }
  if (lambda >= 1)   /////////////////////////////////////////////////////Time to move on!!
  {
    intervalDone(treadmill,ci);
    return;
    
  }
  
  
}

void GradeSpeedMap::alignLandmarks(Treadmill *treadmill)
{
  
  //Step One.  calculate fDistOffset
  landmarkDataType lm;
  fetchLandmarkItem(nLandmarkDataIndex,&lm); 
  fMapOffset = lm.dist - treadmill->fDist();
  if (strlen(lm.landmark) > 0) nLandmarkDataIndex++;
    
}
char * GradeSpeedMap::currentLandmark(Treadmill *treadmill)
{
  landmarkDataType lm;
  fetchLandmarkItem (nLandmarkDataIndex,&lm);
  char szDist[16];
  dtostrf(lm.dist - (treadmill->fDist() + fMapOffset),0,2,szDist);
  
  static char szRet[32];
  sprintf(szRet,"%s) %s",szDist,lm.landmark);
  //Serial.print(" RET: ");
  //Serial.println(szRet);
  return szRet;
  
}
void GradeSpeedMap::handle(Treadmill *treadmill)
{
  static unsigned long lastCheck = millis();
  if (millis() - lastCheck < 1000) return;
  lastCheck = millis();
  //print();
  
  if (nIntervalsActive) handleInterval(treadmill);
  
  if (bActiveGrade) handleGrade (treadmill);
}

void GradeSpeedMap::fetchGradeItem(int i,gradeDataType *gp)
{
  gradeDataType temp;
  memcpy_P (gp, &gradeData[i], sizeof (gradeDataType));
}

void GradeSpeedMap::fetchLandmarkItem(int i,landmarkDataType *lp)
{
  landmarkDataType temp;
  memcpy_P (lp, &landmarkData[i], sizeof (landmarkDataType));
}
