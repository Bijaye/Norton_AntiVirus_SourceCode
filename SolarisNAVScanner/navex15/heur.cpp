//************************************************************************
//
// $Header:   S:/NAVEX/VCS/heur.cpv   1.7   08 Jun 1998 18:38:22   AOONWAL  $
//
// Description:
//      Contains Bloodhound integration code for NAVEX-picasso
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/heur.cpv  $
// 
//    Rev 1.7   08 Jun 1998 18:38:22   AOONWAL
// Revert back to 1.5
// 
//    Rev 1.5   29 Dec 1997 17:55:54   JWILBER
// Modified during Jan98 build.
//
//    Rev 1.4   12 Dec 1997 10:38:32   CNACHEN
// Changed code to determine path to VIRSCAN5.DAT to use trailing backslash
// rather than searching for NAVC.  This is not going to be multi-byte
// compatible if the filename of the NAV executable has double byte characters
// in it. otherwise it should be fine.
//
//    Rev 1.3   06 Nov 1997 10:08:22   CNACHEN
//
//************************************************************************


#include "platform.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>

#include "ctsn.h"
#include "callback.h"
#include "navex.h"

#include "heurapi.h"
#include "navheur1.h"
#include "inifile.h"
#include "callfake.h"

// global...

NAVEX_HEUR_T        gstHeur = {0};

#if defined(SYM_DOSX)
#define APP_STRING "NAVDX"
#elif defined(SYM_WIN16)
#define APP_STRING "NAVWIN"
#endif

#define TECH_DISABLED           0
#define TECH_ENABLED            1
#define TECH_UNKNOWN            2

UINT WINAPI DDFileRead(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
{
    return(DFileRead(hHandle,lpBuffer,uBytes));
}

DWORD WINAPI DDFileSeek(HFILE hHandle, LONG dwOffset, int nFlag)
{
    return(DFileSeek(hHandle,dwOffset,nFlag));
}

EXTSTATUS FreeHeurExtensions
(
    LPNAVEX_HEUR        lpstHeur
)
{
    LPEXT_NODE          lpstNode, lpstNext;

    lpstNode = lpstHeur->lpstExtList;

    while (lpstNode != NULL)
    {
        lpstNext = lpstNode->lpstNext;

        PMemoryFree((void *)lpstNode);
        lpstNode = lpstNext;
    }

    return(EXTSTATUS_OK);
}

extern int mytoupper(int);

EXTSTATUS AddHeurExtensions
(
    LPNAVEX_HEUR        lpstHeur,
    LPSTR               lpszExtList
)
{
    char                szCurExt[4];
    int                 i, nExtIndex = 0;
    LPEXT_NODE          lpstNode;

    lpstHeur->lpstExtList = NULL;

    for (i=0;lpszExtList[i];i++)
    {
        if (lpszExtList[i] == ' ' ||
            lpszExtList[i] == '\t')
            continue;

        if (lpszExtList[i] == ',')
        {
            szCurExt[nExtIndex] = 0;
            nExtIndex = 0;

            lpstNode = (LPEXT_NODE) PMemoryAlloc(sizeof(EXT_NODE_T));
            if (lpstNode == NULL)
            {
                FreeHeurExtensions(lpstHeur);

                return(EXTSTATUS_MEM_ERROR);
            }

            lpstNode->szExt[0] = (TCHAR)szCurExt[0];
            lpstNode->szExt[1] = (TCHAR)szCurExt[1];
            lpstNode->szExt[2] = (TCHAR)szCurExt[2];
            lpstNode->szExt[3] = (TCHAR)szCurExt[3];

            lpstNode->lpstNext = lpstHeur->lpstExtList;
            lpstHeur->lpstExtList = lpstNode;
            continue;
        }

        szCurExt[nExtIndex++] = mytoupper(lpszExtList[i]);
    }

    if (nExtIndex)
    {
        szCurExt[nExtIndex] = 0;

        lpstNode = (LPEXT_NODE)PMemoryAlloc(sizeof(EXT_NODE_T));
        if (lpstNode == NULL)
        {
            FreeHeurExtensions(lpstHeur);

            return(EXTSTATUS_MEM_ERROR);
        }

        lpstNode->szExt[0] = (TCHAR)szCurExt[0];
        lpstNode->szExt[1] = (TCHAR)szCurExt[1];
        lpstNode->szExt[2] = (TCHAR)szCurExt[2];
        lpstNode->szExt[3] = (TCHAR)szCurExt[3];

        lpstNode->lpstNext = lpstHeur->lpstExtList;
        lpstHeur->lpstExtList = lpstNode;
    }

    return(EXTSTATUS_OK);
}



EXTSTATUS InitHeur
(
    LPTSTR                                  lpszNAVEXDataDir
)
{

    char        szDatFileName[SYM_MAX_PATH], szNAVEXINIFile[SYM_MAX_PATH];

    HFILE       hFile;
    int         nOut, nUseHeur;
    char        szExtList[513];

    CALLBACKREV1        stCB = {NULL,NULL,DDFileRead,NULL,DDFileSeek};

    mystrcpy(szDatFileName,lpszNAVEXDataDir);
#if defined(SYM_UNIX)
    mystrcat(szDatFileName,"VIRSCAN5.DAT");
#else
    mystrcat(szDatFileName,"virscan5.dat");
#endif

    mystrcpy(szNAVEXINIFile,lpszNAVEXDataDir);
#if defined(SYM_UNIX)
    mystrcat(szNAVEXINIFile,"heur.inf");
#else
    mystrcat(szNAVEXINIFile,"HEUR.INF");
#endif

    // only enable after we've successfully loaded

    gstHeur.bEnabled = FALSE;
    nUseHeur = TECH_UNKNOWN;

    // now check our ini file to see if we want to use heur and/or ext. list

    hFile = DFileOpen(szNAVEXINIFile,0);
    if (hFile == (HFILE)-1)
    {
        // fow now: assume Heuristics are on by default!

        nUseHeur = TECH_ENABLED;
        gstHeur.nHeurLevel = 3;

        if (AddHeurExtensions(&gstHeur,
                              "com,exe") != EXTSTATUS_OK)
        {
            return(EXTSTATUS_INIT_ERROR);
        }
    }

    // now that we've opened the file, see if heuristics are on
    // assuming we don't already know

    if (hFile != (HFILE)-1 && nUseHeur == TECH_UNKNOWN)
    {
        GetProfileInt(&stCB,
                      hFile,
                      APP_STRING,
                      "heuristics",
                      3,            // 3=on by default
                      &nOut);

        if (nOut != 0)
        {
            gstHeur.nHeurLevel = nOut;

            nUseHeur = TECH_ENABLED;
        }
        else
            nUseHeur = TECH_DISABLED;
    }

    if (hFile != (HFILE)-1 && nUseHeur == TECH_ENABLED)
    {
        // see if there's a heuristics extensions list; default is COM,EXE

        GetProfileString(&stCB,
                         hFile,
                         APP_STRING,
                         "heurextlist",
                         "com,exe",
                         szExtList,
                         512);

        if (AddHeurExtensions(&gstHeur,
                              szExtList) != EXTSTATUS_OK)
        {
            nUseHeur = TECH_DISABLED;
        }
    }

    if (nUseHeur == TECH_ENABLED)
    {
        // perform heur global init

        if (PAMGlobalInit(szDatFileName, &gstHeur.hGHeur) != PAMSTATUS_OK)
        {
            if (hFile != (HFILE)-1)
                DFileClose(hFile);

            return(EXTSTATUS_INIT_ERROR);
        }

        // now indicate that we're enabled

        gstHeur.bEnabled = TRUE;
    }

    if (hFile != (HFILE)-1)
        DFileClose(hFile);

    return ( EXTSTATUS_OK );
}

#ifdef SYM_DOS
void LoadHeuristics
(
    LPSTR               lpszProgramPath
)
{
    LPSTR               lpszPtr, lpszLast;
    char                szPath[SYM_MAX_PATH];
    DWORD               dwSize;

    // Use temporary callbacks until we get real ones from the first call
    // to EXTScanFile...

    gstHeur.lpstCallBacks = NULL;

    // set up pointers for filename processing

    lpszLast = NULL;
    strcpy(szPath,lpszProgramPath);
    strupr(szPath);
    lpszPtr = szPath;

    // find trailing backslash.

    lpszLast = strrchr(szPath,'\\');

    if (lpszLast != NULL)
    {
        lpszLast++;
        *(lpszLast) = 0;
    }

    // now open our INI file and see whats up!

    if (InitHeur(szPath) != EXTSTATUS_OK)
    {
        return;
    }
}
#endif // SYM_DOS

#ifdef SYM_WIN16
void LoadHeuristics
(
    LPVOID              lpvExtra
)
{
    char                szPath[SYM_MAX_PATH];
    DWORD               dwSize, dw;

    dwSize = GetModuleFileName((HINSTANCE)lpvExtra,
                               szPath,
                               SYM_MAX_PATH);

    if (dwSize == 0)
        return;

    // Search for the preceding backslash (no double byte since we're only
    // skipping backward over the NAVEX.DLL name which has no double byte
    // chars)

    dw = dwSize - 1;
    while (dw != 0)
    {
        if (szPath[dw] == '\\')
            break;

        --dw;
    }

    if (szPath[dw] != '\\')
        szPath[dw] = '\\';

    szPath[dw+1] = 0;

    // Use temporary callbacks until we get real ones from the first call
    // to EXTScanFile...

    gstHeur.lpstCallBacks = NULL;

    if (InitHeur(szPath) != EXTSTATUS_OK)
    {
        return;
    }
}
#endif // SYM_WIN16

void UnloadHeuristics
(
    void
)
{
    if (gstHeur.hGHeur != NULL)
        PAMGlobalClose(gstHeur.hGHeur);

    FreeHeurExtensions(&gstHeur);
}


//********************************************************************
//
// Function:
//  WORD NeedsToBeScanned()
//
// Description:
//  Determines whether the filename ends in .COM or .EXE for the time being.
//
// Returns:
//  FALSE       If the file should NOT be scanned
//  TRUE        If the file should be scanned
//
//********************************************************************

#ifndef AnsiNext
#define AnsiNext(x) (x+1)
#endif


BOOL NeedsToBeScanned
(
    LPTSTR  lpszFileName        // Name of infected file
)
{
    LPTSTR                      lptstrDOT;
    LPTSTR                      lptstrCur;
    int                         i, j;
    LPEXT_NODE                  lpstExt;
    extern NAVEX_HEUR_T         gstHeur;

    if (lpszFileName == NULL || *lpszFileName == '\0')
        return (FALSE);

    lptstrDOT = NULL;
    lptstrCur = lpszFileName;

    // find last . in filename

    while (*lptstrCur != 0)
    {
        lptstrCur = AnsiNext(lptstrCur);
        if (*lptstrCur == '.')
        {
            lptstrDOT = lptstrCur;
        }
    }

    if (lptstrDOT == NULL)
        return(FALSE);

    // advance over the period

    lptstrDOT = AnsiNext(lptstrDOT);

    // now compare the extensions

    lpstExt = gstHeur.lpstExtList;

    while (lpstExt != NULL)
    {
        for (i=0;lpstExt->szExt[i];i++)
        {
            if (!lptstrDOT[i])
            {
                for (j=i;lpstExt->szExt[j];j++)
                    if (lpstExt->szExt[j] != '?')
                        break;

                if (lpstExt->szExt[j] == 0)
                    return(TRUE);

                break;
            }

            if (lpstExt->szExt[i] != toupper(lptstrDOT[i]) &&
                lpstExt->szExt[i] != '?')
                break;
        }

        if (lpstExt->szExt[i] == 0)
        {
            return(TRUE);
        }

        lpstExt = lpstExt->lpstNext;
    }

    return (FALSE);
}


EXTSTATUS FAR WINAPI NLOADDS ScanBloodFile
(
    LPCALLBACKREV1  lpCallBack,
    LPTSTR          lpszFileName,
    HFILE           hFile,
    LPBYTE          lpbyInfectionBuffer,
    LPBYTE          lpbyWorkBuffer,
    WORD            wVersionNumber,
    LPWORD          lpwVID
)
{
    extern NAVEX_HEUR_T     gstHeur;

    (void)lpszFileName;
    (void)wVersionNumber;

    if (wVersionNumber < 3)
    {
        // not supported before picasso (nav version 3.0)

        return(EXTSTATUS_OK);
    }

    // set up callbacks!

    gstHeur.lpstCallBacks = lpCallBack;

    // use heuristics!

    if (gstHeur.bEnabled == TRUE && NeedsToBeScanned(lpszFileName))
    {
        PAMLHANDLE                      hLocal;
        BOOL                            bVirusFound;
        WORD                            wVirusID;
        PAMSTATUS                       pamStatus;

        // get instance of scanner so we can go!


        if (PAMLocalInit(gstHeur.hGHeur,&hLocal) != PAMSTATUS_OK)
        {
            return(EXTSTATUS_MEM_ERROR);
        }


        // scan that file

        pamStatus = PAMScanFileH(hLocal,
                                 hFile,
                                 gstHeur.nHeurLevel,
                                 &bVirusFound,
                                 &wVirusID);


        // kill the instance

        PAMLocalClose(hLocal);

        if (pamStatus == PAMSTATUS_OK && bVirusFound == TRUE)
        {
            *lpwVID = wVirusID;

            return(EXTSTATUS_VIRUS_FOUND);
        }

        // had to be an error or clean file, return OK anyway
    }

    return(EXTSTATUS_OK);
}
