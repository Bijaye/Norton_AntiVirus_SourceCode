//************************************************************************
//
// $Header:   S:/NAVEX/VCS/xl97eren.cpv   1.0   08 Sep 1998 18:42:02   DCHI  $
//
// Description:
//  Contains XL97 encrypted repair checking.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/xl97eren.cpv  $
// 
//    Rev 1.0   08 Sep 1998 18:42:02   DCHI
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
//  InitXL97EncRepEnableState()
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

extern int gnXL97EncryptedRepair;

EXTSTATUS InitXL97EncRepEnableState
(
    LPCALLBACKREV1  lpstCallBacks,
    LPTSTR          lpszNAVEXINIFile
)
{
    HFILE       hFile;

    // For now, we just have 0 and 1, assume on by default

    gnXL97EncryptedRepair = 1;

    // For now, we will only be checking the INI file

    hFile = lpstCallBacks->FileOpen(lpszNAVEXINIFile,0);

    // now that we've opened the file, see if XL97 encrypted
    //  repair is on assuming we don't already know

    if (hFile != (HFILE)-1)
    {
        GetProfileInt(lpstCallBacks,
                      hFile,
                      APP_STRING,
                      "ExcelEncryptedRepair",
                      1,            // 1=on by default
                      &gnXL97EncryptedRepair);

        lpstCallBacks->FileClose(hFile);
    }

    // Limit to either 0 or 1

    if (gnXL97EncryptedRepair < 0 || gnXL97EncryptedRepair > 1)
        gnXL97EncryptedRepair = 1;

    return(EXTSTATUS_OK);
}

#if defined(SYM_DOSX) || defined(SYM_WIN16)

//********************************************************************
//
// Function:
//  BOOL GetXL97EncRepEnableState()
//
// Description:
//  Sets gnXL97EncryptedRepair to TRUE and then
//  looks in XLENCREP.INF to see if XL97 encrypted repair
//  is enabled or not.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

extern char gszStartUpDir[256];

BOOL gbCheckedXL97EncRepEnableState = FALSE;

BOOL GetXL97EncRepEnableState
(
    LPCALLBACKREV1  lpstCallBacks
)
{
    char            szNAVEXINIFile[280];
    int             i, j;

    gbCheckedXL97EncRepEnableState = TRUE;

    for (i=0;i<256;i++)
    {
        if ((szNAVEXINIFile[i] = gszStartUpDir[i]) == 0)
            break;
    }

    for (j=0;j<13;j++)
        szNAVEXINIFile[i++] = "XLENCREP.INF"[j];

    // Copy the path

    if (InitXL97EncRepEnableState(lpstCallBacks,
                                  szNAVEXINIFile) != EXTSTATUS_OK)
        return(FALSE);

    return(TRUE);
}

#endif // #if defined(SYM_DOSX) || defined(SYM_WIN16)

