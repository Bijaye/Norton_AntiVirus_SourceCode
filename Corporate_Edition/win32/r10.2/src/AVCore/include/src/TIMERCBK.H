/////////////////////////////////////////////////////////////////////
// TimerCBK.h
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/timercbk.h_v   1.0   11 Dec 1996 16:17:36   DSACKIN  $
//
// This is where we will define all of our callback structures.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/timercbk.h_v  $
// 
//    Rev 1.0   11 Dec 1996 16:17:36   DSACKIN
// Initial revision.
// 
//    Rev 1.0   08 Nov 1996 11:35:40   DKEEFER
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
