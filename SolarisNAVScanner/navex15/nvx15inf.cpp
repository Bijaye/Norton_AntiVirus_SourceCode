//************************************************************************
//
// $Header:   S:/NAVEX/VCS/nvx15inf.cpv   1.4   16 Dec 1998 14:40:30   DCHI  $
//
// Description:
//  Contains NAVEX15.INF reader.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/nvx15inf.cpv  $
// 
//    Rev 1.4   16 Dec 1998 14:40:30   DCHI
// Added APP_STRING for OS2.
// 
//    Rev 1.3   15 Dec 1998 13:01:56   DCHI
// 
//    Rev 1.2   15 Dec 1998 12:09:20   DCHI
// Added [PowerPointScanning] inf file checking and NAVEXInfCheck().
// 
//    Rev 1.0   08 Dec 1998 12:53:18   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#include "platform.h"
#include "n30type.h"
#include "callback.h"
#include "navex.h"

#include "inifile.h"
#include "nvx15inf.h"

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
#elif defined(SYM_OS2)
#define APP_STRING "NAVOS2"
#elif defined(SYM_UNIX)
#define APP_STRING "NAVUNIX"
#else
#define APP_STRING "NAVAP"
#endif

typedef struct tagINF15_ENTRY
{
    LPSTR           lpszKey;
    int             nDefault;
    int             nMax;
    LPINT           lpnResult;
} INF15_ENTRY_T, FAR *LPINF15_ENTRY;

extern int gnMacroHeuristicLevel;
extern int gnXL97EncryptedRepair;
extern int gnMSXEnabled;
extern int gnPP97ScanEnabled;

int gnDummy;

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

INF15_ENTRY_T FAR gastInf15Entries[] =
{
#ifndef NOMACRO
#ifdef MACROHEU
    { "macroheuristics"         , 1, 1, &gnMacroHeuristicLevel },
#endif // #ifdef MACROHEU
    { "ExcelEncryptedRepair"    , 1, 1, &gnXL97EncryptedRepair },
#ifdef PP_SCAN
    { "PowerPointScanning"      , 0, 1, &gnPP97ScanEnabled     },
#endif // #ifdef PP_SCAN
#ifdef MSX
    { "MSXEnabled"              , 1, 1, &gnMSXEnabled          },
#endif // #ifndef MSX
#endif // #ifndef NOMACRO
    { "Dummy"                   , 1, 1, &gnDummy               }
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

#define INF15_COUNT     (sizeof(gastInf15Entries) / \
                         sizeof(INF15_ENTRY_T))

//********************************************************************
//
// Function:
//  NAVEX15InfCheck()
//
// Description:
//  The function opens the INF file and loads the key values.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL NAVEX15InfCheck
(
    LPCALLBACKREV1  lpstCallBacks,
    LPTSTR          lpszNAVEXINIFile
)
{
    HFILE           hFile;
    int             i;
    LPINF15_ENTRY   lpstEntry;

    // Initialize all results to their default values

    lpstEntry = gastInf15Entries;
    for (i=0;i<INF15_COUNT;i++)
    {
        *(lpstEntry->lpnResult) = lpstEntry->nDefault;
        ++lpstEntry;
    }

    // Open the file

    hFile = lpstCallBacks->FileOpen(lpszNAVEXINIFile,0);

    if (hFile != (HFILE)-1)
    {
        lpstEntry = gastInf15Entries;
        for (i=0;i<INF15_COUNT;i++)
        {
            GetProfileInt(lpstCallBacks,
                          hFile,
                          APP_STRING,
                          lpstEntry->lpszKey,
                          lpstEntry->nDefault,
                          lpstEntry->lpnResult);

            ++lpstEntry;
        }

        lpstCallBacks->FileClose(hFile);
    }

    // Limit level to maximum

    lpstEntry = gastInf15Entries;
    for (i=0;i<INF15_COUNT;i++)
    {
        if (*(lpstEntry->lpnResult) < 0 ||
            *(lpstEntry->lpnResult) > lpstEntry->nMax)
            *(lpstEntry->lpnResult) = lpstEntry->nDefault;

        ++lpstEntry;
    }

    return(TRUE);
}


char gszStartUpDir[256];

#if !defined(NAVEX15) && (defined(SYM_DOSX) || defined(SYM_WIN16))

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

    if (NAVEX15InfCheck(lpstCallBacks,
                        szNAVEXINIFile) == FALSE)
        return(FALSE);

    return(TRUE);
}


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

    if (NAVEX15InfCheck(lpstCallBacks,
                        szNAVEXINIFile) == FALSE)
        return(FALSE);

    return(TRUE);
}


#endif // #if !defined(NAVEX15) && (defined(SYM_DOSX) || defined(SYM_WIN16))

#if !defined(NAVEX15) && defined(SYM_WIN)

HFILE WINAPI NAVEXInfFileOpen(LPTSTR lpName, UINT uOpenMode)
{
    HFILE       hFile;

    if (uOpenMode != 0)
        return((HFILE)-1);

    hFile = _lopen(lpName,OF_READ | OF_SHARE_COMPAT);
    if (hFile == HFILE_ERROR)
        return((HFILE)-1);

    return(hFile);
}

HFILE WINAPI NAVEXInfFileClose(HFILE hHandle)
{
    HFILE       hFile;

    hFile = _lclose(hHandle);
    if (hFile == HFILE_ERROR)
        return((HFILE)-1);

    return(0);
}

UINT  WINAPI NAVEXInfFileRead(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
{
    UINT        uResult;

    uResult = _lread(hHandle,lpBuffer,uBytes);
    if (uResult == HFILE_ERROR)
        return((UINT)-1);

    return(uResult);
}

UINT  WINAPI NAVEXInfFileWrite(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
{
    (void)hHandle;
    (void)lpBuffer;
    (void)uBytes;

    return((UINT)-1);
}

DWORD WINAPI NAVEXInfFileSeek(HFILE hHandle, LONG dwOffset, int nFlag)
{
    LONG        lResult;

    lResult = _llseek(hHandle,dwOffset,nFlag);
    if (lResult == HFILE_ERROR)
        return((DWORD)-1);

    return((DWORD)lResult);
}

DWORD WINAPI NAVEXInfFileSize(HFILE hHandle)
{
    (void)hHandle;

    // Not implemented

    return((DWORD)-1);
}

CALLBACKREV1 gstNAVEXInfWinCBs =
{
    NAVEXInfFileOpen,
    NAVEXInfFileClose,
    NAVEXInfFileRead,
    NAVEXInfFileWrite,
    NAVEXInfFileSeek,
    NAVEXInfFileSize
};

//********************************************************************
//
// Function:
//  BOOL NAVEXInfCheck()
//
// Description:
//  Sets gbCheckedNAVEXInf to TRUE and then
//  looks in NAVEX.INF to see what is specified.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL gbCheckedNAVEXInf = FALSE;

BOOL NAVEXInfCheck
(
    void
)
{
    char            szNAVEXINIFile[280];
    int             i, j;

#ifdef SYM_WIN16
    GetMacroHeurEnableState(&gstNAVEXInfWinCBs);
    GetXL97EncRepEnableState(&gstNAVEXInfWinCBs);
#endif // #ifdef SYM_WIN16

    gbCheckedNAVEXInf = TRUE;

    for (i=0;i<256;i++)
    {
        if ((szNAVEXINIFile[i] = gszStartUpDir[i]) == 0)
            break;
    }

    for (j=0;j<10;j++)
        szNAVEXINIFile[i++] = "NAVEX.INF"[j];

    // Copy the path

    if (NAVEX15InfCheck(&gstNAVEXInfWinCBs,
                        szNAVEXINIFile) == FALSE)
        return(FALSE);

    return(TRUE);
}

#endif // #if !defined(NAVEX15) && defined(SYM_WIN)


