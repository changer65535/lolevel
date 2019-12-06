#include "NexDisplay.h"


void NexDisplay::bufferError (char *szMessage)
{
  Serial.print("Nex BUFFER ERROR: ");
  Serial.println (szMessage);
  //tone(TONE_PIN, 610);
}
void NexDisplay::sendCommand(const char *szData) 
{ 
  
  
  
  /*for (int i = 0; i < strlen(szData); i++)
  {
    nexSerial.write(szData[i]);   // Push each char 1 by 1 on each loop pass  
  }
  */
  nexSerial.write(szData);

  nexSerial.write(0xff); //We need to write the 3 ending bits to the Nextion as well
  nexSerial.write(0xff); //it will tell the Nextion that this is the end of what we want to send.
  nexSerial.write(0xff);

  
}
void NexDisplay::addToGraph(byte nWaveform, byte nChannel,byte nValue)
{
  char buffer[64] = "";
  sprintf(buffer,"add %d,%d,%d",nWaveform,nChannel,nValue);
  sendCommand(buffer);

} 


int NexDisplay::getVal(char *controlID)
{
  char commandBuffer[64];
  
  sprintf(commandBuffer,"get %s.val",controlID);
  sendCommand(commandBuffer);
  byte buffer[8];
  //int bnRet = nexSerial.readBytes(buffer,8);
  int bnRet = listen(buffer,8);
  /*Serial.print(bnRet);
  Serial.print("---");
  for (int i = 0;i<=sizeof(buffer);i++)
  {
    Serial.print(buffer[i]);
    Serial.print(",");
  }
  Serial.print("---");
  
  */
  if (bnRet != 5) 
  {
    char szError[64];
    sprintf(szError,"Byte Return Error getVal: %s",controlID);
    
    bufferError(szError);
    return -1;
  }
  if (buffer[0] != 113) 
  {
    bufferError("Bad Value Error getVal");
    return -1;   //variable le not found
  }
  int nRet = (byte)buffer[1] + (byte)buffer[2] * 256;
  //Serial.println(nRet);
  return (nRet);
}

int NexDisplay::getID(char *controlID)
{
  char commandBuffer[64];
  sprintf(commandBuffer,"get %s.id",controlID);
  sendCommand(commandBuffer);
  byte buffer[8];
  int bnRet = listen(buffer,8);
  
  if (bnRet != 5) 
  {
    Serial.print("BNRET: ");
    Serial.println(bnRet);
    Serial.print(buffer[0],HEX);
    Serial.print(",");
    Serial.print(buffer[1],HEX);
    Serial.print(",");
    Serial.print(buffer[2],HEX);
    Serial.print(",");
    Serial.print(buffer[3],HEX);
    Serial.println(",");
     
    
    bufferError("Byte Return Error in getID");
    return -1;
  }
  if (buffer[0] != 113) 
  {
    
    bufferError("bad Value Error in getID");
    Serial.println(buffer[0],HEX);
    return (-1);
  }
  
  return (byte)buffer[1] + (byte)buffer[2] * 256;
}
void NexDisplay::flush()
{
  while (nexSerial.available() > 0) 
  {
    char ch = nexSerial.read();
  }
}

int NexDisplay::listen(char *buffer,int bufferSize)
{
  
  long startTime = millis();
  int nBufferIndex = 0;
  while (true)
  {
    if (millis() - startTime > 250) 
    {
      bufferError("NEX DISPLAY TIMEOUT DURING LISTEN");
      Serial.print("BufferIndex: ");
      Serial.println(nBufferIndex);
      Serial.println("---------------");
      for (int i = 0;i<nBufferIndex;i++)
      {
        Serial.print(buffer[i] & 0xff,HEX);
        Serial.print(" ");
      }
      Serial.println();
      return(0);
    }
    if (nexSerial.available() > 0)
    {
      
      buffer[nBufferIndex] = nexSerial.read();
      
      /*for (int i = 0;i<=nBufferIndex;i++)
      {
        Serial.print((byte)buffer[i],HEX);
        Serial.print(" ");
      }
      Serial.println();
     */
      
      if (nBufferIndex > 2)
      {
        if (((byte)buffer[nBufferIndex-2] == 0xff) &&
        ((byte)buffer[nBufferIndex-1] == 0xff) &&
        ((byte)buffer[nBufferIndex] == 0xff))
        {
          
           buffer[nBufferIndex - 2] = 0x00;
           return nBufferIndex - 2;
           
        }
      }
      nBufferIndex++;
      if (nBufferIndex>=bufferSize) 
      {
        for (int i = 0;i<bufferSize;i++) 
        {
          Serial.print(buffer[i] & 0xff,HEX);
          Serial.print(",");
        }
        char szError[64];
        sprintf(szError,"Buffer OverRun in Listen.  Size: %d",bufferSize);
        bufferError(szError);
      }
      
    }
  }
}

int NexDisplay::getPage()
{
 sendCommand("sendme");
 char buffer[5];
 int nResult = listen(buffer,sizeof(buffer));
 if (nResult == 0)
 {
   
   return (-1);
   
 }
 if (buffer[0] != 0x66)
 {
   return (-1);
 }
 /*Serial.println("success in getPage");
 Serial.println(buffer[0],HEX);
 Serial.println(buffer[1],HEX);
 Serial.println(buffer[2],HEX);
 */
 return (buffer[1]);
}

void NexDisplay::waitForPage()
{
  unsigned long startTime = millis();
  clearComms();
  sendCommand("sendme");
  char buffer[5];
  nexSerial.readBytes(buffer,5);
  Serial.print("waitforPage TIME: ");
  Serial.println(millis() - startTime);
  /*for (int i = 0;i<5;i++) 
  {
    Serial.print((byte)buffer[i],HEX);
    Serial.print(",");
  }
  Serial.print("--");
  Serial.println("Leaving waitforPage");
  */
  
  
}
float NexDisplay::getFloat(char *controlID,float defaultVal)
{
  char buffer[64];
  sprintf(buffer,"get %s.txt",controlID);
  sendCommand(buffer);
  
  char szReturn[64];
  int nRet = listen(szReturn,sizeof(szReturn));
  if (szReturn[0] != 0x70) 
  {
    char szError[64];
    sprintf(szError,"bad Value Error in getFloat: %s  Buffer is...",controlID);
    bufferError(szError);
    Serial.print("nRet: ");
    Serial.println(nRet);
    for (int i = 0;i<10;i++) 
    {
      Serial.print(szReturn[i] & 0xff,HEX);
      Serial.print(",");
    }
    Serial.println();
    
    return (-1);
  }
  
  
  //Serial.print("Length szReturn");
  //Serial.println(strlen(szReturn));
  if (strlen(szReturn) == 1) return defaultVal;//remember, the first byte is 0x70
  return (atof(&szReturn[1]));
}

float NexDisplay::getPace(char *controlID,float defaultVal = 0.0)
{
  //Serial.println("Entering getPace");
  char buffer[64];
  sprintf(buffer,"get %s.txt",controlID);
  sendCommand(buffer);
  
  char szReturn[64];
  int nRet = listen(szReturn,sizeof(szReturn));
  if (szReturn[0] != 0x70) 
  {
    char szError[64];
    sprintf(szError,"bad Value Error in getPace: %s  Buffer is...",controlID);
    bufferError(szError);
    Serial.print("nRet: ");
    Serial.println(nRet);
    for (int i = 0;i<10;i++) 
    {
      Serial.print(szReturn[i] & 0xff,HEX);
      Serial.print(",");
    }
    Serial.println();
    
    return (-1);
  }
  //Serial.print("Length szReturn");
  //Serial.println(strlen(szReturn));
  if (strlen(szReturn) == 1) return defaultVal;//remember, the first byte is 0x70
  char *szRet1 = &szReturn[1];
  //Serial.print("szReturn: ");
  //Serial.println(szRet1);
  int nIndex = strcspn(szRet1,":");if (nIndex == strlen(szRet1)) nIndex = -1;
  if (nIndex == -1) return atof(szRet1);
  return atof(szRet1) + atof(&szRet1[nIndex+1]) / 60.0;
  
  
  
}

int NexDisplay::getInt(char *controlID,int defaultVal)
{
  return (int)getFloat(controlID,defaultVal);
}



int NexDisplay::setVal(char *controlID,int v)
{
  
  char buffer[64];
  sprintf(buffer,"%s.val=%d",controlID,v);
  
  sendCommand(buffer);
}
void NexDisplay::setTxt(char *controlID,char *txt)
{
  
  char buffer[512];
  sprintf(buffer,"%s.txt=\"%s\"",controlID,txt);
  sendCommand(buffer);
  
}

void NexDisplay::setTxt(char *controlID,int n)
{
  
  char buffer[64];
  sprintf(buffer,"%s.txt=\"%d\"",controlID,n);
  
  sendCommand(buffer);
}
void NexDisplay::setTxt(char *controlID,float f,int decPlaces = 1)
{
  char szFloat[10];
  dtostrf(f,0,decPlaces,szFloat);
  char buffer[64];
  strcpy(buffer,controlID);
  strcat(buffer,".txt=");
  strcat(buffer,"\"");
  strcat(buffer,szFloat);
  strcat(buffer,"\"");
  sendCommand(buffer);
  
  
}


void NexDisplay::addCallback (byte nPage,byte nID,byte pushPop,FunctionPtr f)
{
  //char buffer[64];
  //Serial.println("Looking...");
  //char buffer[64];
  //sprintf(buffer,"Looking for %d %d %d",nPage,nID,pushPop);
  //Serial.println(buffer);
  //Serial.print("ncbIndex: ");
  //Serial.println(ncbIndex);
  for (int i = 0;i<ncbIndex;i++)
  {
    
    //sprintf(buffer,"i: %d-- %d %d %d",i,ncb[i].nPage,ncb[i].nID,ncb[i].pushPop);
    //Serial.println(buffer);
    if ((ncb[i].nPage == nPage) && (ncb[i].nID == nID) && (ncb[i].pushPop == pushPop))
    {
      //Serial.println("Already Found");
      return;//return, because the button already exists...
    }
    
  }
  ncb[ncbIndex].nPage   = nPage;
  ncb[ncbIndex].nID     = nID;
  ncb[ncbIndex].pushPop = pushPop;
  ncb[ncbIndex].f       = f;
  ncbIndex++;
  //Serial.println("Added");
}

void NexDisplay::addCallback (byte nPage,char *text,byte pushPop,FunctionPtr f)
{
  //Serial.print("Looking up: ");
  //Serial.println(text);
  
  byte nID = getID(text);
  //Serial.print("ID is ");
  //Serial.println(nID);
  addCallback(nPage,nID,pushPop,f);
  
}
void NexDisplay::nexListen()
{
  
  
  
  if (!nexSerial.available()) return;
  //Serial.println("NEXLISTEN");
  
  
  byte buffer[64];
  memcpy(buffer,0,sizeof(buffer));
  int ret = listen(buffer,sizeof(buffer));
  
  
  if (buffer[0] == 1) return;
  
  if ((byte)buffer[0] == NEX_TOUCH_EVENT)
  {
    
    byte nPage = (byte)buffer[1];
    byte nID = (byte)buffer[2];
    byte pushPop = (byte)buffer[3];
    //Serial.println("touch!");
    //Serial.println(nPage,HEX);
    //Serial.println(nID,HEX);
    //Serial.println(pushPop,HEX);
    
    
    for (int i = 0;i<ncbIndex;i++)
    {
      if ((ncb[i].nPage == nPage) && (ncb[i].pushPop == pushPop) && (ncb[i].nID == nID))
      {
        //Serial.println("Found Function");
        ncb[i].f();
      }
    }
    
    
      
  }
  
}
void NexDisplay::clearComms()
{
  while (nexSerial.available()) char ch = nexSerial.read();
  nexSerial.flush();
}
bool NexDisplay::nexInit()
{
  //nexInit();
  //return;
    bool ret1 = false;
    bool ret2 = false;
    
    
    nexSerial.begin(9600);
    nexSerial.flush();
    
    
    clearComms();
    sendCommand("");
    //sendCommand("bkcmd=0");
    //delay(100);
    sendCommand("bkcmd=0");
    
    //delay(100);
    //while (nexSerial.available()) char ch = nexSerial.read();
    //nexSerial.flush();

    sendCommand("baud=57600");
    delay(200);
    nexSerial.begin(57600);
    delay(200);
    //sendCommand("Hello");
    
    //ret1 = listen();
    sendCommand("page 0");
    //ret2 = recvRetCommandFinished();
    //return ret1 && ret2;
}
