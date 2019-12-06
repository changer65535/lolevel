#ifndef __NEXDISPLAY__
#define __NEXDISPLAY__
#define NEX_PUSH 1
#define NEX_POP 0
#define NEX_TOUCH_EVENT 0x65
#define nexSerial  Serial3
#include <Arduino.h>
//#include "Nextion.h"
class NexDisplay
{public:
  typedef void (*FunctionPtr) ();
  struct NCBStruct
  {
     byte nPage;
     byte nID;
     byte pushPop;
     FunctionPtr f;
     
  };
  
  NCBStruct ncb[64*sizeof(NCBStruct)];
  int ncbIndex = 0;
  void bufferError (char *szMessage);
  void sendCommand(const char *szData) ;
  int NexDisplay::listen(char *buffer, int bufferSize);
  int getID(char *controlID);
  int getVal(char *controlID);
  int setVal(char *controlID,int v);
  void setTxt(char *controlID,char *txt);
  void setTxt(char *controlID,int n);
  void setTxt(char *controlID,float f,int decPlaces = 1);
  float getFloat(char *controlID,float defaultVal = 0.0);
  float getPace(char *controlID,float defaultVal = 0.0);
  int getInt(char *controlID,int defaultVal = 0);
  void addCallback (byte nPage,byte nID,byte pushPop,FunctionPtr f);
  void addCallback (byte nPage,char *text,byte pushPop,FunctionPtr f);
  void nexListen();
  bool nexInit();
  void addToGraph(byte nWaveform, byte nChannel,byte nValue);
  int getPage();
  void waitForPage ();
  void flush();
  void clearComms();
  
 
  
};
#endif
