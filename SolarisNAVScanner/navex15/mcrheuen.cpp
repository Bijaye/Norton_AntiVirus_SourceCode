//************************************************************************
//
// $Header:   S:/NAVEX/VCS/MCRHEUEN.CPv   1.0   05 Nov 1997 12:00:42   DCHI  $
//
// Description:
//  Contains macro heuristics enablement checking.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/MCRHEUEN.CPv  $
// 
//    Rev 1.0   05 Nov 1997 12:00:42   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "n30type.h"
#include "callback.h"
#include "navex.h"

#include "inifile.h"

/////////////////////////////////////////////////////////////////////////////
// Get heuristics enablement flag
/////////////////////////////////////////////////////////////////////////////

#ifdef SYM_WIN32
#define APP_STRING "NAVW32"
#elif defined(SYM_DOSX)
#define APP_STRING "NAVDX"
#elif defined(SYM_NLM)
#define APP_STRING "NAVNLM"
#elif defined(SYM_WIN16)
#define APP_STRING "NAVWIN"
#elif defined(SYM_UNIX)
#define APP_STRING "NAVUNIX"
#else
#define APP_STRING "NAVAP"
#endif

//********************************************************************
//
// Function:
//  InitMacroHeurEnableState()
//
// Description:
//  The function first checks the app's "heur" value to determine
//  whether the app has enabled/disabled heuristics.  If either,
//  then the function uses the value.  Otherwise, the function
//  checks the appropriate function in the ini file.
//
// Returns:
//  EXTSTATUS_OK        Always
//
//********************************************************************

extern int gnMacroHeuristicLevel;

EXTSTATUS InitMacroHeurEnableState
(
    LPCALLBACKREV1  lpstCallBacks,
    LPTSTR          lpszNAVEXINIFile
)
{
    HFILE       hFile;
//    char        szOptionValue[128];
//    BOOL        bImplemented;

    // For now, we just have 0 and 1, assume on by default

    gnMacroHeuristicLevel = 1;

    // For now, we will only be checking the INI file
/*
    // first check our config callbacks to see if we want to use heuristics

    // precedence:
    // 1. command line option or built in option
    // 2. ini file
    // 3. default is on

    lpstCallBacks->GetConfigInfo(_T("heur"),
                                 (LPVOID)szOptionValue,
                                 sizeof(szOptionValue)-1,
                                 &bImplemented,
                                 NULL);

    if (bImplemented == TRUE)
    {
        // "/heur" was definitely on the command line or an option; get the
        // string.

        gnMacroHeuristicLevel = szOptionValue[0] - '0';
    }
    else
*/
    {
        // now check our ini file to see if we want to use heur if
        //  the app did not know

        hFile = lpstCallBacks->FileOpen(lpszNAVEXINIFile,0);

        // now that we've opened the file, see if heuristics are on
        // assuming we don't already know

        if (hFile != (HFILE)-1)
        {
            GetProfileInt(lpstCallBacks,
                          hFile,
                          APP_STRING,
                          "macroheuristics",
                          3,            // 3=on by default
                          &gnMacroHeuristicLevel);

            lpstCallBacks->FileClose(hFile);
        }
    }

    // Limit level to either 0 or 1

    if (gnMacroHeuristicLevel < 0 || gnMacroHeuristicLevel > 1)
        gnMacroHeuristicLevel = 1;

    return ( EXTSTATUS_OK );
}

#if defined(SYM_DOSX) || defined(SYM_WIN16)

//********************************************************************
//
// Function:
//  BOOL GetMacroHeurEnableState()
//
// Description:
//  Sets gbCheckedMacroHeurEnableState to TRUE and then
//  looks in HEUR.INF to see if macro heuristics are enabled or not.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

char gszStartUpDir[256];

BOOL gbCheckedMacroHeurEnableState = FALSE;

BOOL GetMacroHeurEnableState
(
    LPCALLBACKREV1  lpstCallBacks
)
{
    char            szNAVEXINIFile[280];
    int             i, j;

    gbCheckedMacroHeurEnableState = TRUE;

    for (i=0;i<256;i++)
    {
        if ((szNAVEXINIFile[i] = gszStartUpDir[i]) == 0)
            break;
    }

    for (j=0;j<9;j++)
        szNAVEXINIFile[i++] = "HEUR.INF"[j];

    // Copy the path

    if (InitMacroHeurEnableState(lpstCallBacks,
                                 szNAVEXINIFile) != EXTSTATUS_OK)
        return(FALSE);

    return(TRUE);
}

#endif // #if defined(SYM_DOSX) || defined(SYM_WIN16)

