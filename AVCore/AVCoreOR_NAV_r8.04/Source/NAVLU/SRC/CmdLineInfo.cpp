// Copyright 1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/cmdlineinfo.cpv   1.7   28 Aug 1998 16:14:28   JBRENNA  $
//
// Description:
//   Provide the implementation for the Command Line arguments.
//
//****************************************************************************
// $Log:   S:/NAVLU/VCS/cmdlineinfo.cpv  $
// 
//    Rev 1.7   28 Aug 1998 16:14:28   JBRENNA
// Port changed from QAKN branch to Trunk:
// 1. Rev 1.6.1.2   10 Jul 1998 17:20:18   TCASHIN
//    Added support for the /SAGESET command line switch and fixed a minor
//    problem in /SAGERUN handling to schedlued LiveUpdate is silent.
// 2. Rev 1.6.1.1   29 Jun 1998 14:24:46   tcashin
//    Added the /setup command line switch.
// 
//    Rev 1.6   26 May 1998 14:38:12   TCASHIN
// Win98 Sage has a slightly different syntax for/sagerun (now /sagerun:x).
// 
//    Rev 1.5   11 Sep 1997 13:16:24   MKEATIN
// Ported changes from branch G.
// 
//    Rev 1.4.1.1   16 Aug 1997 19:30:16   JBRENNA
// Add support for /SageRun switch ... runs in silent mode.
// 
//    Rev 1.4.1.0   12 Aug 1997 22:57:42   DLEWIS
// Branch base for version QAKG
// 
//    Rev 1.4   06 Aug 1997 10:27:50   JBRENNA
// Add support for /silent switch.
// 
//    Rev 1.3   18 Jun 1997 14:01:16   JBRENNA
// Add PretendSuccessReboot cmd line option ... for DEBUG only.
// 
//    Rev 1.2   03 Jun 1997 13:07:04   JBRENNA
// 1. Change IDS_CMDOPT_* resource ids to IDS_CMDLINE_OPT_*.
// 2. Add a /PretendHostBusy switch that is only available for _DEBUG builds.
// 
//    Rev 1.1   01 Jun 1997 18:48:12   JBRENNA
// Add new command line options: /Prompt and /InternalRetry.
// 
//    Rev 1.0   23 May 1997 21:26:28   JBRENNA
// Initial revision.
// 
//****************************************************************************

#include "StdAfx.h"
#include "CmdLineInfo.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////////
// Description:
//   Process the command line arguments. The following are the currently
//   understood arguments:
//
//      /scheduled  sets m_bScheduled to TRUE.
//////////////////////////////////////////////////////////////////////////
void CNavLuCmdLineInfo::ParseParam (LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
    if (!m_bScheduled)
    {
        CString strParam;
        strParam.LoadString (IDS_CMDLINE_OPT_SCHEDULED);
        
        if (strParam.CompareNoCase (lpszParam) == 0)
        {
            m_bSilent    = TRUE;        // Scheduled switch implies silent LU mode.
            m_bScheduled = TRUE;
            return;
        }
    }

    if (!m_bPrompt)
    {
        CString strParam;
        strParam.LoadString (IDS_CMDLINE_OPT_PROMPT);
        
        if (strParam.CompareNoCase (lpszParam) == 0)
        {
            m_bPrompt = TRUE;
            return;
        }
    }

    if (!m_bInternalRetry)
    {
        CString strParam;
        strParam.LoadString (IDS_CMDLINE_OPT_INTERNALRETRY);

        if (strParam.CompareNoCase (lpszParam) == 0)
        {
            m_bInternalRetry = TRUE;
            return;
        }
    }

    if (!m_bSilent)
    {
        CString strParam;
        strParam.LoadString (IDS_CMDLINE_OPT_SILENT);
        
        if (strParam.CompareNoCase (lpszParam) == 0)
        {
            m_bSilent = TRUE;
            return;
        }
    }

    if (!m_bSageRun)
    {
        CString strCmdLineParam(lpszParam);

        CString strParam;
        strParam.LoadString (IDS_CMDLINE_OPT_SAGERUN);
        strParam.MakeUpper ();
        
        if (strCmdLineParam.Find (strParam) != -1)
        {
            m_bSilent  = TRUE;      // "/SageRun" implies silent mode.
            m_bSageRun = TRUE;
            return;
        }
    }

    if (!m_bSetup)
    {
        CString strParam;
        strParam.LoadString (IDS_CMDLINE_OPT_SETUP);

        if (strParam.CompareNoCase (lpszParam) == 0)
        {
            m_bSetup = TRUE;
            return;
        }
    }

    if (!m_bSageSet)
    {
        CString strCmdLineParam(lpszParam);

        CString strParam;
        strParam.LoadString (IDS_CMDLINE_OPT_SAGESET);
        strParam.MakeUpper ();
        
        if (strCmdLineParam.Find (strParam) != -1)
        {
            m_bSageSet = TRUE;
            return;
        }
    }

#ifdef _DEBUG
    if (!m_bPretendHostBusy)
    {
        CString strParam;
        strParam.LoadString (IDS_CMDLINE_OPT_PRETENDHOSTBUSY);

        if (strParam.CompareNoCase (lpszParam) == 0)
        {
            m_bPretendHostBusy = TRUE;
            return;
        }
    }

    if (!m_bPretendSuccessReboot)
    {
        CString strParam;
        strParam.LoadString (IDS_CMDLINE_OPT_PRETENDSUCCESSREBOOT);

        if (strParam.CompareNoCase (lpszParam) == 0)
        {
            m_bPretendSuccessReboot = TRUE;
            return;
        }
    }
#endif // _DEBUG
}

