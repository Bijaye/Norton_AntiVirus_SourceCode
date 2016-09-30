// Copyright 1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/cmdlineinfo.h_v   1.5   28 Aug 1998 16:15:34   JBRENNA  $
//
// Description:
//   Provide the class for parsing the Command Line arguments.
//
//****************************************************************************
// $Log:   S:/NAVLU/VCS/cmdlineinfo.h_v  $
// 
//    Rev 1.5   28 Aug 1998 16:15:34   JBRENNA
// Port changed from QAKN to trunk:
// 1. Rev 1.4.1.2   10 Jul 1998 17:21:54   TCASHIN
//    Added support /SAGESET.
// 2. Rev 1.4.1.1   29 Jun 1998 14:24:22   tcashin
//    Added the /setup command line switch.
// 
//    Rev 1.4   11 Sep 1997 13:20:32   MKEATIN
// Ported changed from branch G.
// 
//    Rev 1.3.1.1   16 Aug 1997 19:30:18   JBRENNA
// Add support for /SageRun switch ... runs in silent mode.
// 
//    Rev 1.3.1.0   12 Aug 1997 22:57:34   DLEWIS
// Branch base for version QAKG
// 
//    Rev 1.3   18 Jun 1997 14:01:38   JBRENNA
// Add PretendSuccessReboot cmd line option ... for DEBUG only.
// 
//    Rev 1.2   03 Jun 1997 13:09:02   JBRENNA
// Added m_bPretendHostBusy.
// 
//    Rev 1.1   01 Jun 1997 18:48:30   JBRENNA
// Add new command line options: /Prompt and /InternalRetry.
// 
//    Rev 1.0   23 May 1997 21:26:30   JBRENNA
// Initial revision.
// 
//****************************************************************************

#ifndef __CMDLINEINFO_H
#define __CMDLINEINFO_H

class CNavLuCmdLineInfo : public CCommandLineInfo
{
// public functions
public:
    CNavLuCmdLineInfo();

    virtual void ParseParam (LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);

// public member variables
public:
    BOOL m_bScheduled;
    BOOL m_bSilent;
    BOOL m_bPrompt;
    BOOL m_bSageRun;
    BOOL m_bSageSet;
    BOOL m_bInternalRetry;
    BOOL m_bPretendHostBusy;
    BOOL m_bPretendSuccessReboot;
    BOOL m_bSetup;
};

// inline functions

inline CNavLuCmdLineInfo::CNavLuCmdLineInfo()
{
    m_bScheduled            = FALSE;
    m_bSilent               = FALSE;
    m_bPrompt               = FALSE;
    m_bSageRun              = FALSE;
    m_bSageSet              = FALSE;
    m_bInternalRetry        = FALSE;
    m_bPretendHostBusy      = FALSE;
    m_bPretendSuccessReboot = FALSE;
    m_bSetup                = FALSE;
}

#endif // __CMDLINEINFO_H

