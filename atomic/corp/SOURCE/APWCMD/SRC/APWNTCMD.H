// Copyright 1997 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/APWCMD/VCS/ApwNtCmd.h_v   1.2   29 Jun 1998 22:21:02   RStanev  $
//
// Description:
//   Contains the NT specific class definition for ApwNtCmd.
//
//***************************************************************************
// $Log:   S:/APWCMD/VCS/ApwNtCmd.h_v  $
// 
//    Rev 1.2   29 Jun 1998 22:21:02   RStanev
// Added new functionality to support asynchronous processing of tasks
// triggered by driver notifications. This is a workaround for a problem
// with RPC in Win9x.
//
//    Rev 1.1   01 Jun 1997 12:39:32   DALLEE
// Changed naming scheme to be more readable - "APW" in classes and
// functions is now "Apw".
//
//    Rev 1.0   31 May 1997 19:56:38   JBRENNA
// Initial revision.
//
//***************************************************************************

#ifndef __APWNTCMD_H
#define __APWNTCMD_H

#ifndef __APWCMD_H
#include "ApwCmd.h"                     // For CAPWCmd class.
#endif
#ifndef __SVCCTRL_H
#include "SvcCtrl.h"                    // For Service Communications
#endif
#ifndef __APSCLI_H
#include "ApsCli.h"                     // For communications with NAVAPSVC
#endif


//************************************************************************
// class CApwNtCmd
//
// NT version of the CAPWCmd class.

class CApwNtCmd : public CApwCmd
{
// Operations
public:
    virtual BOOL     Init( HWND hParent );
    virtual BOOL     DeInit( VOID );

    virtual BOOL     Options( HWND hParent );
    virtual BOOL     StartDriver( VOID );
    virtual BOOL     StopDriver( VOID );
    virtual BOOL     WaitForStartupPending( VOID );
    virtual BOOL     EnableDriver( VOID );
    virtual BOOL     DisableDriver( VOID );
    virtual AP_STATE GetStatusOfDriver( VOID );
    virtual BOOL     ShutdownScan( HWND hParent );
    virtual VOID     ConfirmApwComTasksReceived( VOID );

// Attributes
public:

// Implementation
public:
    ~CApwNtCmd();

// Private Functions
private:
    void GetNavwExePath (LPSTR lpszNavw, LPSTR lpszDir);

// Private Data.
private:
    CNAVAPSVCClient   m_TalkAPService;
};

#endif // __APWNTCMD_H
