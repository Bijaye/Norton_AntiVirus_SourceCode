// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVINOC/VCS/inocsmrt.cpv   1.1   20 May 1997 17:43:14   BILL  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVINOC/VCS/inocsmrt.cpv  $
// 
//    Rev 1.1   20 May 1997 17:43:14   BILL
// Remove intel only calls (certlibn) and fail all calls from NT clients.
// 
//    Rev 1.0   06 Feb 1997 20:58:18   RFULLER
// Initial revision
// 
//    Rev 1.4   01 Jul 1996 15:51:34   DALLEE
// Changed comments on SmartInocOpen() to reflect parameters should be
// in ANSI mode, not OEM (as they were a few revs back).
// 
//    Rev 1.3   27 Jun 1996 10:21:40   GWESTER
// Ported in fixes from Luigi Plus.
// 
//    Rev 1.2   22 Mar 1996 10:09:38   jworden
// DBCS Enable
// 
//    Rev 1.1   14 Feb 1996 17:08:28   JWORDEN
// Process internal file names in OEM mode to allow for the innoculation
// of file names that include certain non-ANSI translatable characters
// 
//    Rev 1.0   30 Jan 1996 15:53:16   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 13:22:22   BARRY
// Initial revision.
// 
//    Rev 1.7   23 Mar 1995 16:59:52   SZIADEH
// changed CHAR to char
// 
//    Rev 1.6   23 Mar 1995 14:48:54   SZIADEH
// added UNC support.
//************************************************************************
#include "platform.h"
#include "xapi.h"
#include "ctl3d.h"                  // Microsoft 3D library
#include "cmbstr.h"
#include "options.h"
#include "inoc.h"

typedef struct tagSMARTINOC
{
    UINT    uInocOpenOK;
    BOOL    bReadWrite;
    BOOL    bLongFile;
    BYTE    byDrive;
    TCHAR   szUNCName[SYM_MAX_PATH+1];
} SMARTINOC, FAR * LPSMARTINOC;


static SMARTINOC Smart =
{
    SMARTOPEN_CLOSED,
    FALSE, FALSE, 0, ""
};

//------------------------------------------------------------------------
// Function: SmartInocOpen
//
// Purpose:
// Notes:   File APIs are expected to be in ANSI mode.
//
// Input:    lpszFile      // Full path in ANSI format
//           bReadWrite    //
//
//------------------------------------------------------------------------
UINT SYM_EXPORT WINAPI SmartInocOpen
(
    LPSTR lpszFile,                     // Full path in ANSI format
    BOOL bReadWrite                     //
)
{
#ifdef _ALPHA_
	return FALSE;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return FALSE;

    auto    BYTE    byDrive=0;
    auto    BOOL    bLongFile;
    auto    BOOL    bUNC_OK=FALSE;
    auto    TCHAR   szBuffer [SYM_MAX_PATH+1];
    auto    TCHAR   szUNCName[SYM_MAX_PATH+1];

    NameReturnShortName(lpszFile, szBuffer);
    bLongFile = (BOOL) (STRLEN(szBuffer) > 144);

    SYM_ASSERT (lpszFile );
    if ( IsUNCName ( lpszFile ) )
        {
        UNC_GetServerVolumeName (lpszFile, szUNCName );
        bUNC_OK =  TRUE;
        }
    else
        {
        byDrive = lpszFile[0];
        }

    if (byDrive != Smart.byDrive ||
        STRICMP ( szUNCName, Smart.szUNCName) ||
        bReadWrite != Smart.bReadWrite ||
        bLongFile != Smart.bLongFile ||
        Smart.uInocOpenOK == SMARTOPEN_CLOSED ||
        Smart.uInocOpenOK == SMARTOPEN_ERROR)
        {
        SmartInocClose();

        Smart.byDrive    = byDrive;
        Smart.bLongFile  = bLongFile;
        Smart.bReadWrite = bReadWrite;

        BuildInocDBName (szBuffer, bLongFile);

        if ( bUNC_OK )
            {
            STRCPY ( Smart.szUNCName, szUNCName );
            STRCAT ( szUNCName, szBuffer+2 );
            STRCPY ( szBuffer, szUNCName );
            }
        else
            szBuffer[0] = byDrive;
        
        if (InocOpen (szBuffer, bReadWrite) == TRUE)
            {
            if (bReadWrite == TRUE)
                Smart.uInocOpenOK = SMARTOPEN_RW;
            else 
                Smart.uInocOpenOK = SMARTOPEN_RO;
            }
        else
            Smart.uInocOpenOK = SMARTOPEN_ERROR;
        }

    return (Smart.uInocOpenOK);
#endif    
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
VOID SYM_EXPORT WINAPI SmartInocClose(VOID)
{
#ifdef _ALPHA_
	return;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return;

    if (Smart.uInocOpenOK == SMARTOPEN_RW ||
        Smart.uInocOpenOK == SMARTOPEN_RO)
        {
        InocClose();
        Smart.uInocOpenOK = SMARTOPEN_CLOSED;
        }
#endif
}

UINT SYM_EXPORT WINAPI SmartInocGetStatus(VOID)
{
#ifdef _ALPHA_
	return 0;
#else	
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    	return 0;

    return (Smart.uInocOpenOK);
#endif    
}
