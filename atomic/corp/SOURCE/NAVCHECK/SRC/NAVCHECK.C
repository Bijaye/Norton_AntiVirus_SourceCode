// Copyright 1992-1996 Symantec, Peter Norton Product Group
//***********************************************************************
//
// $Header:   S:/NAVCHECK/VCS/navcheck.c_v   1.1   20 Aug 1997 13:42:42   TCASHIN  $
//
// Description:
//
//      NAVCHECK determines the current platform and start the
//      appropriate NAV update program from the server.
//
// See Also:
//***********************************************************************
// $Log:   S:/NAVCHECK/VCS/navcheck.c_v  $
// 
//    Rev 1.1   20 Aug 1997 13:42:42   TCASHIN
// If /NOUPDATE is used, there is no need to verify that the NNS 
// and Exe files exist.
// 
//    Rev 1.0   18 Aug 1997 21:15:06   TCASHIN
// Initial revision.
//***********************************************************************

#include <platform.h>
#include <xapi.h>
#include <stdio.h>
#include <string.h>
#include <stddos.h>
#include <file.h>
#include <windows.h>
#include "navcheck.h"


//************************************************************************
// Local prototypes
//************************************************************************
void    BuildFileNames(VOID);
int     GetCurrentPlatform(VOID);
void    ParseNavcheckCmdLine(int argc, char *lpargv[]);
int     VerifyStartInfo(WORD);


//************************************************************************
// External Data
//************************************************************************
extern const unsigned char *NAVCHK_HELP_SCREEN[];
extern const unsigned char *NAVCHK_ERR_NO16[];
extern const unsigned char *NAVCHK_ERR_NO32[];
extern const unsigned char *NAVCHK_ERR_NO16EXE[];
extern const unsigned char *NAVCHK_ERR_NO32EXE[];
extern const unsigned char *NAVCHK_ERR_NO16NNS[];
extern const unsigned char *NAVCHK_ERR_NO32NNS[];

extern char NAVCHK_W16[];
extern char NAVCHK_W32[];
extern char NAVCHK_HELP[];
extern char NAVCHK_NOUPDATE[];

extern char NAVCHK_W16EXE[];
extern char NAVCHK_W16NNS[];
extern char NAVCHK_W32EXE[];
extern char NAVCHK_W32NNS[];


//************************************************************************
// Global Data
//************************************************************************
EXESTARTINFO  StartInfo;


//************************************************************************
// Main()
//************************************************************************
// 8/17/97 TCASHIN   Function created.
//************************************************************************

int main( int argc, char *argv[])
{
    int     returnCode;
    int     wPlatform;

    ParseNavcheckCmdLine(argc, argv);

    if (StartInfo.byHelp || argc == 1 )
    {
        DOSWriteStringArray(NAVCHK_HELP_SCREEN);
        return (0);
    }

    BuildFileNames();

    wPlatform = GetCurrentPlatform();

    if (StartInfo.byNoupdate == FALSE)
        {
        returnCode = VerifyStartInfo(wPlatform);
        if (returnCode != 0)
           return (returnCode);
        }

    switch (wPlatform)
    {
    auto char szNewCmd[512] = {0};

    case NAVCHK_16BIT:
        if (StartInfo.byNoupdate)
        {
            returnCode = NAVCHK_PLAT_W16;
            break;
        }
        else
        {            
            /* Start the specified 16bit program ... */
            STRCPY(szNewCmd, StartInfo.szW16ExeFile);
            STRCAT(szNewCmd, " ");
            STRCAT(szNewCmd, StartInfo.szW16NNSFile);
            system(szNewCmd);
        }

        returnCode = 0;
        break;

    case NAVCHK_32BIT:
        if (StartInfo.byNoupdate)
        {
            returnCode = NAVCHK_PLAT_W32;
            break;
        }
        else
        {            
            /* Start the specified 32bit program ... */
            STRCPY(szNewCmd, StartInfo.szW32ExeFile);
            STRCAT(szNewCmd, " ");
            STRCAT(szNewCmd, StartInfo.szW32NNSFile);
            system(szNewCmd);
        }

        returnCode = 0;
        break;

    default:
        /* Unknown platform ... */
        printf(" ... unknown platform\n");
        returnCode = 0;
        break;
    };

    return (returnCode);
}



//************************************************************************
// BuildFileNames()
//
//  Create the file names for the EXE and NNS files for both 
//  16 and 32 bit plaforms.
//
// Parameters:
//  none
//
// Returns:
//  none
//
//************************************************************************
// 8/17/97 TCASHIN   Function created.
//************************************************************************
void BuildFileNames(VOID)
{
    UINT uFlags;
    int  len;

    //*******************************************************************
    // Create the 16bit file names ...
    //*******************************************************************

    uFlags = NameGetType(StartInfo.szW16CmdLine);

    STRCPY(StartInfo.szW16NNSFile, StartInfo.szW16CmdLine);

    if (!(uFlags & NGT_IS_FILE))
        {
        len = STRLEN(StartInfo.szW16NNSFile);

        if ((uFlags & NGT_IS_DIR) && 
            (uFlags &~ NGT_IS_ROOT) &&
            (StartInfo.szW16NNSFile[len-1] != '\\') )

            STRCAT(StartInfo.szW16NNSFile,"\\");

        STRCAT(StartInfo.szW16NNSFile, NAVCHK_W16NNS);
        }

    STRCPY(StartInfo.szW16ExeFile, StartInfo.szW16NNSFile);

    NameStripFile(StartInfo.szW16ExeFile);

    len = STRLEN(StartInfo.szW16ExeFile);

    if (StartInfo.szW16ExeFile[len-1] != '\\')
            STRCAT(StartInfo.szW16ExeFile, "\\");

    STRCAT(StartInfo.szW16ExeFile, NAVCHK_W16EXE);

    
    //*******************************************************************
    // Create the 32bit file names ...
    //*******************************************************************
    
    uFlags = NameGetType(StartInfo.szW32CmdLine);

    STRCPY(StartInfo.szW32NNSFile, StartInfo.szW32CmdLine);

    if (!(uFlags & NGT_IS_FILE))
        {
        len = STRLEN(StartInfo.szW32NNSFile);

        if ((uFlags & NGT_IS_DIR) && 
            (uFlags &~ NGT_IS_ROOT) &&
            (StartInfo.szW32NNSFile[len-1] != '\\') )

            STRCAT(StartInfo.szW32NNSFile,"\\");

        STRCAT(StartInfo.szW32NNSFile, NAVCHK_W32NNS);
        }

    STRCPY(StartInfo.szW32ExeFile, StartInfo.szW32NNSFile);

    NameStripFile(StartInfo.szW32ExeFile);

    len = STRLEN(StartInfo.szW32ExeFile);

    if (StartInfo.szW32ExeFile[len-1] != '\\')
            STRCAT(StartInfo.szW32ExeFile, "\\");

    STRCAT(StartInfo.szW32ExeFile, NAVCHK_W32EXE);
}

//************************************************************************
// GetCurrentPlatform()
//
//  Determine the current platform.
//
// Parameters:
//  none
//
// Returns:
//  NAVCHK_16BIT   - For DOS, Win3.1, and Win32S
//  NAVCHK_32BIT   - For Win95 and WinNT
//  NAVCHK_UNKNOWN - Anything else.
//
//************************************************************************
// 8/17/97 TCASHIN   Function created.
//************************************************************************
int GetCurrentPlatform(VOID)
{

    WORD    wPlatform;

    wPlatform = SystemGetWindowsType();

    if ((wPlatform == SYM_SYSTEM_WIN_32S) || 
        (wPlatform == SYM_SYSTEM_WIN_31)  ||
        (wPlatform == 0) )
        return (NAVCHK_16BIT);
    else if ((wPlatform == SYM_SYSTEM_WIN_95) || 
        (wPlatform == SYM_SYSTEM_WIN_NT))
        return (NAVCHK_32BIT);
    else
        return (NAVCHK_UNKNOWN);

}


//************************************************************************
// ParseNavcheckCmdLine()
//
//  Parse the command line.
//
// Parameters:
//  argc - number of arguments
//  argv - the normal array for strings
//
// Returns:
//  none
//
//************************************************************************
// 8/17/97 TCASHIN   Function created.
//************************************************************************
void ParseNavcheckCmdLine(int argc, char *lpargv[])
{
    int      i;
    LPBYTE   lpTempPtr;

    for (i=1;i<argc;i++)
    {
        if (STRCMP(STRUPR(lpargv[i]), NAVCHK_HELP) == 0)
        {
            StartInfo.byHelp = TRUE;
            continue;
        }

        if (STRCMP(STRUPR(lpargv[i]), NAVCHK_NOUPDATE) == 0)
        {
            StartInfo.byNoupdate = TRUE;
            continue;
        }

        if (STRSTR(STRUPR(lpargv[i]), NAVCHK_W16) != NULL)
        {
            lpTempPtr = lpargv[i];
            lpTempPtr += STRLEN(NAVCHK_W16);
            STRCPY(StartInfo.szW16CmdLine, lpTempPtr);
            continue;
        }

        if (STRSTR(STRUPR(lpargv[i]), NAVCHK_W32) != NULL)
        {
            lpTempPtr = lpargv[i];
            lpTempPtr += STRLEN(NAVCHK_W32);
            STRCPY(StartInfo.szW32CmdLine, lpTempPtr);
            continue;
        }
    }
}


//************************************************************************
// VerifyStartInfo()
//
//  Verify that the EXE and NNS files exist for the platform
//  we are running on.
//
//  For a given platform, the option *MUST* be specified on
//  the command line, and the NNS and EXE files must exist.
//
//  An error message will be printed here before we return.
//
// Parameters:
//  wPlatform - Either NAVCHK_16BIT or NAVCHK_32BIT
//
// Returns:
//  Success: 0
//  Failure: See NAVCHK_ERR_* in NAVCHECK.H
//  
//************************************************************************
// 8/17/97 TCASHIN   Function created.
//************************************************************************
int VerifyStartInfo(WORD wPlatform) 
{

    if (wPlatform == NAVCHK_16BIT)
    {
        if (STRLEN(StartInfo.szW16ExeFile) == 0)
        {
            // No W16 directory specified ...
            DOSWriteStringArray(NAVCHK_ERR_NO16);
            return(NAVCHK_ERR_CMD);
        }
        else
        {
            if (!FileExists(StartInfo.szW16ExeFile))
            {
                // Can't find W16 exe file
                DOSWriteStringArray(NAVCHK_ERR_NO16EXE);
                return(NAVCHK_ERR_W16);
            }

            if (!FileExists(StartInfo.szW16NNSFile))
            {
                // Can't find W16 exe file
                DOSWriteStringArray(NAVCHK_ERR_NO16NNS);
                return(NAVCHK_ERR_W16);
            }

        }
    }

    if (wPlatform == NAVCHK_32BIT)
    {
        if (STRLEN(StartInfo.szW32ExeFile) == 0)
        {
            // No W32 directory specified ...
            DOSWriteStringArray(NAVCHK_ERR_NO32);
            return(NAVCHK_ERR_CMD);
        }
        else
        {
            if (!FileExists(StartInfo.szW32ExeFile))
            {
                // Can't find W32 exe file
                DOSWriteStringArray(NAVCHK_ERR_NO32EXE);
                return(NAVCHK_ERR_W32);
            }

            if (!FileExists(StartInfo.szW32NNSFile))
            {
                // Can't find W32 exe file
                DOSWriteStringArray(NAVCHK_ERR_NO32NNS);
                return(NAVCHK_ERR_W32);
            }
        }
    }

    return(0);
}
