#include "Treadmill.h"
volatile unsigned long Treadmill::counter = 0;
void Treadmill::printBytes(byte *b,int len)
{
  for (int i = 0;i<len;i++)
  {
    Serial.print(b[i],HEX);
    Serial.print(" ");
  }
  Serial.println();
}
void Treadmill::getResult(bool waitForever = false)
{
  if (waitForever)
  {
    if (debugMode) 
    {
      Serial.println();
      Serial.println("getting after wait");
    }
    
  }
  byte retBytes[64];
  memset (retBytes,0,sizeof(retBytes));
  
  
  byte returnedBytes = SerialTreadmill.readBytes(retBytes,5);
  if (debugMode)
  {
    //Serial.print("returnedBytes: ");
    //Serial.println(returnedBytes);
  }
  
  if (returnedBytes < 5)
  {
    if (debugMode)
    {
      Serial.println("Not enough bytes returned");
    
      Serial.print("returnedBytes: ");
      Serial.println(returnedBytes);
      for (int i = 0;i<5;i++)
      {
        Serial.print(retBytes[i],HEX);
        Serial.print(" ");
      }
    }
  }
  if (retBytes[4] == 0)
  {
    if (debugMode) Serial.println("no data (ok with 0x20)");
    
  }
  if (debugMode) Serial.print("Ret Bytes: ");
  if (debugMode) printBytes(retBytes,5);
  byte data[64];
  memset(data,0,sizeof(data));
  
  SerialTreadmill.readBytes(data,retBytes[4]+1);
  //if (debugMode) Serial.print("Result: ");
  //if (debugMode) printBytes(data,retBytes[4]+1);
  
  
}
void Treadmill::sendCommand (char *szDebugText,byte *b,int len,bool waitForever = false)
{
    
    //Serial.println();
    
    if (debugMode)
    {
      Serial.print("===");
      Serial.println(szDebugText);
      
      Serial.print("Sending: ");
      printBytes(b,len);

    }
    SerialTreadmill.write(b,len);
    
    if (waitForever) 
    {
      Serial.println("***waiting***");
      do 
      {
        //if (debugMode) Serial.print(".");
      }
      while (!SerialTreadmill.available());
    }
    getResult(waitForever);
    
}

byte Treadmill::doCRC1 (byte *message,int length)
{
  byte poly = 0x8d & 0xff;
  //build the message as an array of bits
  byte augMessage[length+1];   //length + 1
  for (int byteIndex = 0;byteIndex<length;byteIndex++) augMessage[byteIndex] = message[byteIndex] & 0xff;
  augMessage[length] = 0x00;


  word r = 0;
  for (int byteIndex = 0;byteIndex<length+1;byteIndex++)
  {
      for (int bitIndex = 0; bitIndex < 8; bitIndex++)
      {

          r = r << 1;
          byte shiftByte = (augMessage[byteIndex] >> (7 - bitIndex) & 0xff) & 0x01;
          r += shiftByte;
          if ((r & 0x100) == 0x100) //check for carry
          {
              r = r ^ poly;

          }
          r = r & 0xff;
      }
  }
  return r;

};

void Treadmill::sendCommandCRC (char *szCommand,byte *command,byte s)
{
  byte c2[s+1];
  memcpy(c2, command, s);
  byte crc = doCRC1 (command,s);
  c2[s] = crc;
  sendCommand(szCommand,c2,sizeof(c2));
  
}

void Treadmill::setRawGrade (word wFinal)
{
  
  //wFinal += 6976;
  byte command[] = {0x00,0xff,0xff,0x35,0x02,((wFinal >> 8) & 0xff),wFinal & 0xff};
  sendCommandCRC("rawGrade",command,sizeof(command));
  
  
}
void Treadmill::setGrade (float fGrade)
{
  
  //Serial.print("setGrade: ");
  //Serial.println(fGrade);
  fGrade = constrain (fGrade,MIN_GRADE,MAX_GRADE);
  fTargetGrade = fGrade;

 
  
  
  static word lastWord = 0;
  
  
  float fConv = fGrade * GRADE_MUL + GRADE_ZERO;
  word wFinal = int(fConv);
  
  if (wFinal == lastWord) return;
  lastWord = wFinal;
  
  
  byte command[] = {0x00,0xff,0xff,0x35,0x02,((wFinal >> 8) & 0xff),wFinal & 0xff};
  sendCommandCRC("Grade 1",command,sizeof(command));
  
}


void Treadmill::setSpeed (float fSpeed)
{
  fSpeed = constrain (fSpeed,0,12);
  fTargetSpeed = fSpeed;
  fTargetSpeedK = fSpeed * 1.60934;
  
  
  static word lastMul = 0;
  
  ///used to be 0xb0c / 9.0
  word mul = int(fSpeed * convMPH + yInt);
  if (lastMul == mul) return;
  lastMul = mul;
  
  if (fSpeed < 0.25) mul = 0;

   //Serial.print("setSpeed ");
   //Serial.println(fSpeed);
  
  byte command[] = {0x00,0xff,0xff,0x2f,0x02,((mul >> 8) & 0xff),mul & 0xff};
  sendCommandCRC("Speed1",command,sizeof(command));
        
  
}
void Treadmill::setSpeedK (float fSpeed)
{
  setSpeed(fSpeed / 1.60934);
}

void Treadmill::integrateStuff()
{
  static unsigned long lastCheck = millis();
  //if (millis() - lastCheck < 1000) return;
  float fDeltaSecs = (millis() - lastCheck) / 1000.0;//seconds
  lastCheck = millis();
  float deltaCalories = fDeltaSecs * fMets() * fWeight / 3600.0;
  //Serial.println(deltaCalories);
  if (fSpeed() >= 0.5) fTotalCalories += deltaCalories;
  fElevationGain = fElevationGain + (fGrade() / 100 * 5280.0 * fDeltaSecs * fSpeed() / 3600.0);
  fIntegratedDistance += (fDeltaSecs * fSpeed() / 3600.0);
  //Serial.println(fDeltaSecs);
  //Serial.print(fIntegratedDistance);
  //Serial.print(" ");
  //Serial.println(fDist());
  
}
void Treadmill::checkStatus()
{
  static unsigned long lastTime = millis();
  if (millis() - lastTime < 200) return;
  lastTime = millis();
  //0 FF FF 45 0 29
  byte command[6] = {0x00,0xff,0xff,0x2e,0x0,0x20};
  //manually send
  SerialTreadmill.write(command,sizeof(command));
  byte status[12];
  SerialTreadmill.readBytes(status,sizeof(status));
  
  speedWord = status[5] * 256 + status[6];
  gradeWord = status[7] * 256 + status[8];
  statusWord = status[1] * 256 + status[2];
  statusWord2 = status[3] * 256 + status[4];
  statusWord3 = status[9] * 256 + status[10];
  
  moving = statusWord & 0x20;
  error = statusWord & 0x200;
  if (error > 0) Serial.println("CheckStatus Error");
  integrateStuff();
 
}

float Treadmill::fGrade()
{
  return constrain ((gradeWord - GRADE_ZERO) / GRADE_MUL,MIN_GRADE,MAX_GRADE);
}

float Treadmill::fSpeed()
{
  float fRet;
  fRet = constrain(((float)speedWord - yInt) / convMPH,0,30);
  if (fRet<0.25) fRet = 0;
  return fRet; 
}
float Treadmill::fSpeedK()
{
  return (fSpeed() * 1.609344);
}

float Treadmill::fMets (float speed,float grade)
{
  if (targetMetsMode == WALKING_METS)
  {
    float mpm = speed * 26.8224;
    return (0.1 * mpm + 1.8 * mpm * grade/100 + 3.5)/3.5;
  }
  if (targetMetsMode == RUNNING_METS)
  {
    float mpm = speed * 26.8224;
    return (0.2 * mpm + 0.9 * mpm * grade/100 + 3.5)/3.5;
  }
  return (-1);
}
float Treadmill::fWalkingMets()
{
    float mpm = fSpeed() * 26.8224;
    return (0.1 * mpm + 1.8 * mpm * fGrade()/100 + 3.5)/3.5;
}
float Treadmill::fRunningMets()
{
    float mpm = fSpeed() * 26.8224;
    return (0.2 * mpm + 0.9 * mpm * fGrade()/100 + 3.5)/3.5;
}

float Treadmill::fMets()
{
  if (targetMetsMode == WALKING_METS) return fWalkingMets();
  if (targetMetsMode == RUNNING_METS) return fRunningMets();
  return (-1);
}

void Treadmill::getPace(float spd,char *ret)
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
void Treadmill::getPace(char *ret)

{
  return getPace(fSpeed(),ret);
}
void Treadmill::getPaceK(char *ret)
{
  return getPaceK(fSpeedK(),ret);
}
void Treadmill::getPaceK(float spd,char *ret)
{
  if (spd == 0) 
  {
    strcpy(ret,"");
    return;
  }
  int nSecondsPerK = round(3600.0 / spd);
  
  
  int nSecs = nSecondsPerK % 60;
  int nMins = nSecondsPerK / 60;
  

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
float Treadmill::calcSpeedFromMets ()
{
  
  //WVO= 0.1 * Speed + 0.018 * form.Grade.value * Speed + NG * 1
  //        RVO= 0.2 * Speed + 0.009 * form.Grade.value * Speed + NG * 1

  float g = fGrade();
  
  float c1,c2;
  if (targetMetsMode == WALKING_METS)
  {
    c1 = 0.1;
    c2 = 1.8;
  }
  if (targetMetsMode == RUNNING_METS)
  {
    c1 = 0.2;
    c2 = 0.9;
  }
  float mpm = (targetMets * 3.5 - 3.5) / (c1 + c2 * g / 100.0);
  return mpm / 26.8224;
}

float Treadmill::calcSpeedFromMets(float m,float g)
{
  
  float c1,c2;
  if (targetMetsMode == WALKING_METS)
  {
    c1 = 0.1;
    c2 = 1.8;
  }
  if (targetMetsMode == RUNNING_METS)
  {
    c1 = 0.2;
    c2 = 0.9;
  }
  float mpm = (m * 3.5 - 3.5) / (c1 + c2 * g / 100.0);
  return mpm / 26.8224;
}

void Treadmill::delayWithYield(int ms)
{
  unsigned int currentTime = millis();
  while (millis() - currentTime < ms)
  {
    yield();
  }
}
void Treadmill::resetVariables()
{
  bMetsActive = false;
  targetMets = 0;
  targetHR = 0;
  targetMetsMode = RUNNING_METS;
  randomGradeMin = 0;
  randomGradeMax = 4;
  randomGradeInterval = 0;
  counter = 0;
  fIntegratedDistance = 0;
  
  savedTargetMets = targetMets;
  
  
  fTotalCalories = 0;
  fElevationGain = 0;
  
  
  
 
  
}
void Treadmill::init()
{
  /////////////FAN///////////////////////////////////
  pinMode (FAN_PIN,OUTPUT);
  
  pinMode(ENCODER_PIN,INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), Treadmill::intProc, RISING);
  resetVariables();
  Serial.println("sending out lots of status checks");
  
  
  SerialTreadmill.flush();

  
  
  
  
  //Serial.println("Sending out initialization commands");
  
  byte initThing[] = {0xfc,0x00,0xff,0xff,0x45,0x00,0x29};////FC 0 FF FF 45 0 29
  sendCommand("Init Thing",initThing,sizeof(initThing));
  delayWithYield(400);
  byte secondThing[] = {0x00,0xff,0xff,0x29,0x00,0x93};////0 FF FF 29 0 93
  sendCommand("-----2:",secondThing,sizeof(secondThing));
  checkStatus();
  Serial.println();
  byte thirdThing[] = {0x00,0xff,0xff,0x45,0x00,0x29};//0 FF FF 45 0 29
  sendCommand("-----3",thirdThing,sizeof(thirdThing));
  checkStatus();

  delayWithYield(400);
  
  
  byte fourthThing[] = {0x00,0xff,0xff,0x20,0x00,0xcb};//0 FF FF 20 0 CB
  sendCommand("-----4",fourthThing,sizeof(fourthThing),true);

  byte crc = doCRC1(fourthThing,5);
  fourthThing[5] = crc;
  Serial.print("CRC: ");
  Serial.println(crc);
  checkStatus();
  delayWithYield(400);
  
  Serial.println();
  byte fifthThing[] = {0x00,0xff,0xff,0x36,0x01,0x01,0x1a};//0 FF FF 36 1 1 1A
  sendCommand("-----5",fifthThing,sizeof(fifthThing));

  
  checkStatus();
  checkStatus();
  setGrade(0);
  digitalWrite(FAN_PIN,0);
   
}
void Treadmill::updateRandomGrade(bool forceStart = false)
{
  static float currentGrade = 1.0;
  static unsigned long lastChecked = millis();
  if (randomGradeInterval == 0) return;
  if ((forceStart == false) && (millis() - lastChecked < randomGradeInterval * 1000)) return;
  lastChecked = millis();
  
  if (randomGradeMode == 0)
  {
     do
     {
      float f = (float)random(-100,100) / 100.0;
      currentGrade +=f;
     }
     while ((currentGrade < randomGradeMin) || (currentGrade > randomGradeMax));
     
     setGrade(currentGrade);
     return;
     
  }
  if (randomGradeMode == 1)
  {
    currentGrade = fGrade() + 0.25;
    if (currentGrade > randomGradeMax) currentGrade = randomGradeMin; 
    //Serial.println("Random Grade" + (String)randomGrade);
    setGrade(currentGrade);
    
    return;
  }
  
}

void Treadmill::handleMets()
{
  if (bMetsActive == false) return;
  //Serial.print(targetMets);
  static long lastCheck = millis();
  static float lastSpeed = 0;
  if (targetMets == 0) return;
  if (millis() - lastCheck < 1000) return;
  lastCheck = millis();
  //Serial.println();
  float s = calcSpeedFromMets();
  
  
  if (lastSpeed == s) return;
  //Serial.println ("Target Mets: " + (String)targetMets);
  //Serial.println("Speed From Mets: " + (String)s);
  lastSpeed = s;
  if (s < 0) s = 0;
  if (s > 10) s = 10;
  setSpeed(s);
}
//////////////;/SOUND FUNCTIONS//////////////////





byte Treadmill::fan ()
{
  return fanMode;
}
static void Treadmill::intProc()
{
  
  counter++;
  Serial.print(".");
  
}
float Treadmill::fDist ()
{

  //belt distance is 130 12/16 inches around (130.75)
  //counters per revolution is 944
  //ticks per inch is 944/130.75 = 7.219885 = 457451.931166 ticks per mile or .13850635593 inches per tick / 12 = 0.01154219632 feet per tick
  //return (counter * 0.01169823861 / 5280);

  //There are 58.2 ticks per revolution
  //meaning 58.2 ticks per 130.75 inches
  //meaning 0.445124282982792 ticks per inch
  //meaning 5.341491395793499 ticks per fool
  //meaning 28203.074569789674952 ticks per mile
  return fIntegratedDistance;
  //return (counter / 28203.074569789674952);
}

void Treadmill::clearOdometer()
{
  fIntegratedDistance = 0;
}
void Treadmill::fan (byte mode)
{
  fanMode = mode;
  digitalWrite(FAN_PIN,fanMode);
  
}
