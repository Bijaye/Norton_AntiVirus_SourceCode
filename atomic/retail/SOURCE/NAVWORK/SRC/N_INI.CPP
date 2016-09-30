// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_ini.cpv   1.2   06 Oct 1998 14:27:12   tcashin  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_ini.cpv  $
// 
//    Rev 1.2   06 Oct 1998 14:27:12   tcashin
// Read the navoptx.dat file for new extended options.
// 
//    Rev 1.1   02 May 1997 07:37:16   TIVANOV
// new entry for the Quarantine initilization
//
//    Rev 1.0   06 Feb 1997 21:02:50   RFULLER
// Initial revision
//
//    Rev 1.0   30 Jan 1996 15:55:28   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:30   BARRY
// Initial revision.
//
//    Rev 1.5   09 May 1995 10:30:32   KEITH
// Correct some nouns
//
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "symcfg.h"
#include "navutil.h"
#include "options.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#include "stub.h"
#include "nonav.h"
#include "nunavstb.h"
#include "cmbstr.h"
#include "navoptx.h"

BOOL LOCAL GetPath(char* szTempW, int size);

VOID WINAPI ReadIniScanner (HINSTANCE hInstance, LPNONAV lpNav,
                            LPNAVOPTIONS lpNavOptions)
{

    auto TCHAR   szTempPath[SYM_MAX_PATH*2] = {0};


   // *************************** //
   // Obtain the Scanner options  //
   // *************************** //

   MEMCPY (&lpNav->Opt, &lpNavOptions->scanner, sizeof(SCANOPT));

   // *************************** //
   // Obtain the General options  //
   // *************************** //

   MEMCPY (&lpNav->Gen, &lpNavOptions->general, sizeof(GENOPT));

   // ************************* //
   // Obtain the Alert options  //
   // ************************* //

   MEMCPY (&lpNav->Alert, &lpNavOptions->alert, sizeof(ALERTOPT));

   // **************************** //
   // Obtain the Activity options  //
   // **************************** //

   MEMCPY (&lpNav->Act, &lpNavOptions->activity, sizeof(ACTOPT));

   // ******************************* //
   // Obtain the Inoculation options  //
   // ******************************* //

   MEMCPY (&lpNav->Inoc, &lpNavOptions->inoc, sizeof(INOCOPT));

   // ******************************* //
   // Obtain the Inoculation options  //
   // ******************************* //

   MEMCPY (&lpNav->Quarantineset, &lpNavOptions->quarantineset, sizeof(QUARANTINESET));

   // ***********************************************
   // In-line Kludge Alert!
   //
   // We can't change NAVOPTS.DAT. Instead, read the 
   // extended options file, NAVOPTX.DAT, for the 
   // access denied option. 
   // 
   // Make sure this option gets added to NAVOPTS.DAT 
   // in the next rev!
   // ***********************************************

   if ( GetPath(szTempPath, sizeof(szTempPath) / sizeof(szTempPath[0])) )
   {
       _tcscat(szTempPath, NAVOPTX_FILE);

       lpNav->ExtendedOpts.bLogAccessDenied = 
              GetPrivateProfileInt(NAVOPTX_SECT_ACTLOG,
                                   NAVOPTX_ACTLOG_ACCESSDENIED,
                                   0,
                                   szTempPath);
   }
   else
       lpNav->ExtendedOpts.bLogAccessDenied = FALSE;

   // ***********************************************
   // End In-line Kludge Alert!
   // ***********************************************

}


//************************************************************************
// GetPath()
//
// Get the path of the NAV directory.
//
// Returns:
//      TRUE        on success.
//      FALSE       failure
//************************************************************************
// 10/5/1998 TCASHIN
//************************************************************************
BOOL LOCAL GetPath(TCHAR * szTemp, int size)
{
    auto HKEY   hKey = 0;
    auto BOOL   bRet = FALSE;

    MEMSET (szTemp, 0, size);

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Symantec\\InstalledApps"), 0, KEY_READ, &hKey))
    {
#ifdef NAVWNT
        if (ERROR_SUCCESS == RegQueryValueEx(hKey, TEXT("NAVNT"), 0, 0, (LPBYTE)szTemp, (LPDWORD)&size))
#else
        if (ERROR_SUCCESS == RegQueryValueEx(hKey, TEXT("NAV95"), 0, 0, (LPBYTE)szTemp, (LPDWORD)&size))
#endif
        {
            bRet = TRUE;
            RegCloseKey(hKey);
        }
    }

    return ( bRet );
}
