#ifndef RCDDEAPP_H
#define RCDDEAPP_H

HDDEDATA CALLBACK DDECallbackProc(
              UINT uType,     // transaction type
              UINT uFmt,      // clipboard data format
              HCONV hconv,    // handle to the conversation
              HSZ hsz1,       // handle to a string
              HSZ hsz2,       // handle to a string
              HDDEDATA hdata, // handle to a global memory object
              DWORD dwData1,  // transaction-specific data
              DWORD dwData2   // transaction-specific data
      );
class RCDdeApp : RCProcess
{
    private:
      RCError Error;
	  RCKeys Keys;
	  RCCommon *Common;
      HWND AppHwnd;
      PROCESS_INFORMATION Apppi;
      DWORD idInst;
      HSZ hszService, hszTopic, hszItem;
      HCONV hConv;
      CString AppName;
      CString MainAppWinName;
	  CString sourceapp;

	  int FExit(int IPCmode);
    
   public:
	   RCDdeApp(RCCommon *Common, CString AppName, int AppVersion);
      void Start(CString FirstFile);
      void Close(int IPCmode);
	  int Execute(CString dde_command);
	  HANDLE GetProcessHandle();
	  HWND GetApplicationHandle();
	  BOOL isAppRunning();
	
       
};
#endif
