/////////////////////////////////////////////////////////////////////
// LinkCBK.h
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/LINKCBK.H_v   1.0   10 May 1997 01:52:18   KSACKIN  $
//
// This is where we will define all of our callback structures.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/LINKCBK.H_v  $
// 
//    Rev 1.0   10 May 1997 01:52:18   KSACKIN
// Initial revision.
// 
//    Rev 1.0   06 Feb 1997 20:51:48   RFULLER
// Initial revision
// 
//    Rev 1.1   22 Jul 1996 17:01:58   DSACKIN
// Added two new callback types for sending strings.
//
//    Rev 1.0   18 Jul 1996 23:41:00   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined (__LINKCBK_H)
#define __LINKCBK_H

/////////////////////////////////////////////////////////////////////
// typedefs

typedef DWORD (*SENDCOMMANDPROC)(LPVOID);
typedef DWORD (*SENDSTRINGPROCA)(LPVOID);
typedef DWORD (*SENDSTRINGPROCW)(LPVOID);

typedef struct tagLINKCALLBACKS
{
   SENDCOMMANDPROC  lpfnSendCommandProc;
   SENDSTRINGPROCA  lpfnSendStringProcA;
   SENDSTRINGPROCW  lpfnSendStringProcW;
} LINKCALLBACKS, * PLINKCALLBACKS;


/////////////////////////////////////////////////////////////////////
// Function declarations

void SetCallbacks(
   PLINKCALLBACKS pCallbacks);          // [in] - List of callbacks to set

#endif // !defined (__LINKCBK_H)
