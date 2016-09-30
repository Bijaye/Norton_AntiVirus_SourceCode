/////////////////////////////////////////////////////////////////////
// TimerCBK.h
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/include/VCS/timercbk.h_v   1.0   06 Feb 1997 20:51:38   RFULLER  $
//
// This is where we will define all of our callback structures.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/include/VCS/timercbk.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:38   RFULLER
// Initial revision
// 
//    Rev 1.0   13 Jun 1996 00:34:48   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined (__TIMERCBK_H)
#define __TIMERCBK_H

/////////////////////////////////////////////////////////////////////
// typedefs

typedef BOOL (*LOGMESSAGEPROCA)(DWORD, LPSTR);
typedef BOOL (*LOGMESSAGEPROCW)(DWORD, LPWSTR);

typedef struct tagSERVERCALLBACKS
{
   LOGMESSAGEPROCA  lpfnLogMessageA;
   LOGMESSAGEPROCW  lpfnLogMessageW;
} SERVERCALLBACKS, * PSERVERCALLBACKS;


/////////////////////////////////////////////////////////////////////
// Function declarations

void SetCallbacks(
   PSERVERCALLBACKS pCallbacks);        // [in] - List of callbacks to set

#endif // !defined (__TIMERCBK_H)
