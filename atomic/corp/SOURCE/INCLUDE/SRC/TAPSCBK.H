/////////////////////////////////////////////////////////////////////
// TapsCBK.h
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/include/VCS/tapscbk.h_v   1.0   06 Feb 1997 20:51:46   RFULLER  $
//
// This is where we will define all of our callback structures.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/include/VCS/tapscbk.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:46   RFULLER
// Initial revision
// 
//    Rev 1.0   18 Jul 1996 18:45:52   DSACKIN
// Initial revision.
//
//    Rev 1.0   13 Jun 1996 00:34:48   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined (__TAPSCBK_H)
#define __TAPSCBK_H

/////////////////////////////////////////////////////////////////////
// typedefs

typedef DWORD (*SENDCOMMANDPROC)(LPVOID);

typedef struct tagTAPSCALLBACKS
{
   SENDCOMMANDPROC  lpfnSendCommandProc;
} TAPSCALLBACKS, * PTAPSCALLBACKS;


/////////////////////////////////////////////////////////////////////
// Function declarations

void SetCallbacks(
   PTAPSCALLBACKS pCallbacks);          // [in] - List of callbacks to set

#endif // !defined (__TAPSCBK_H)
