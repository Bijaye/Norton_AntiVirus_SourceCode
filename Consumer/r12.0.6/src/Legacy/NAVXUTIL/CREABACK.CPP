// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/creaback.cpv   1.8   24 Jun 1998 12:36:06   jtaylor  $
//
// Description:
//      This is the function to create a backup file.
//
// Contains:
//      CreateBackup()
//************************************************************************
// $Log:   S:/NAVXUTIL/VCS/creaback.cpv  $
// 
//    Rev 1.8   24 Jun 1998 12:36:06   jtaylor
// Moved the personality dll include file to the Windows only section.
//
//    Rev 1.7   24 Jun 1998 12:32:44   jtaylor
// No change.
//
//    Rev 1.6   23 Jun 1998 21:03:32   jtaylor
// Changed OEM versions to use .vir files.
//
//    Rev 1.5   28 May 1998 21:34:46   TCASHIN
// Make sure our IQuarantineItem pointer is NULL if AddFileToQuarantine()
// fails.
//
//    Rev 1.4   19 May 1998 22:59:12   tcashin
// Fixed the FileCopy() parameter for the DVX platform to use the OEM path
// in the repair structure.
//
//    Rev 1.3   18 May 1998 21:30:08   tcashin
// Changed CreateBackup() to get the strings it needs from the REPAIRFILESTRUCT
// and fixed some ANSI/OEM problems. Removed COM init/uninit calls since they
// are now in the NAVW startup & shutdown code.
//
//    Rev 1.2   29 Apr 1998 05:49:10   tcashin
// The createbackup() function still has to be there for the
// DOS platform. I got the old version backup and #ifdef'd the
// appropriate places.
//
//    Rev 1.0   06 Feb 1997 21:05:10   RFULLER
// Initial revision
//
//    Rev 1.8   26 Oct 1996 17:03:28   MKEATIN
// Modified UpdateExtention()
//
//    Rev 1.7   14 Oct 1996 16:34:46   JBRENNA
// Change UpdateExtension(). If the last character of the current extension
// passed in is a DBCS char, that 2 byte character is replaced with a
// 1 byte version of uCount.
//
//    Rev 1.6   14 Oct 1996 15:06:56   JBRENNA
// Create UpdateExtension(). Changed CreateBackup() to DBCS enable the
// file already exists logic. If a DBCS extension ends in a DBCS character
// and that file w/ that extension already exists, we replace the last
// DBCS character (2 bytes) with "0x". The '0' replaces the DBCS lead byte
// and the 'x' is the current count.
//
//    Rev 1.5   13 Sep 1996 15:31:34   JBRENNA
// Win95 needs CreateBackup() to set the APIs to OEM before calling FileCopy.
// If we do not expicitly change the APIs, repairing an Inoc file fails to
// create .VIR file on translatable ANSI filenames. I am not sure what the
// story is for WinNT, PVENKAT seem to go out of his way to remove setting
// the APIs in revision 1.2. I don't know why.
//
//    Rev 1.4   09 Sep 1996 17:35:26   DJesch
// Fixed STS #61062 -- permissions of a backup file were not the same as the
// original file.
//
//    Rev 1.3   07 Aug 1996 13:51:28   YVANG
// Ported changes from LuigiPlus.
//
//    Rev 1.2   30 Jul 1996 09:52:12   PVENKAT
// #60322 fixed.  Avoided setting File Apis to OEM on WIN32.
//
//    Rev 1.1   28 Jun 1996 12:02:40   MKEATIN
// Ported Changes from LuigiPlus
//
//    Rev 1.0   30 Jan 1996 15:56:26   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:51:28   BARRY
// Initial revision.
//
//    Rev 1.0   16 Dec 1994 10:36:56   MFALLEN
// Initial revision.
//
//    Rev 1.1   15 Dec 1994 20:13:20   MFALLEN
// From trunk.
//
//    Rev 1.0   12 Aug 1993 11:54:00   MFALLEN
// Initial Revision
//
//    Rev 1.6   21 Jul 1993 17:46:10   MFALLEN
// Modified the way we treat unsuccessful repairs and backup creations.
//
//    Rev 1.5   19 Jul 1993 21:39:54   MFALLEN
// Enhanced the activity log code to write log entries for system integrity
// events. Also a new function BuildWhatScanned() has been added to NAVUTIL
// so both the DOS and Windows can use it.
//
//    Rev 1.4   19 Jul 1993 10:08:58   MFALLEN
// Enhanced create backup files function
//
//    Rev 1.3   09 Jun 1993 18:50:34   DALLEE
// Do not attempt to create backups with the same name as the original.
//
//    Rev 1.2   05 Apr 1993 13:15:42   DALLEE
// Changed FIL4_COPY to FileCopy().
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "navutil.h"
#include "dbcs.h"

#ifdef SYM_WIN32
    #include "iquaran.h"
    #include "quar32.h"
    #include "n32behav.h" // For personality DLL
#endif


//****************************************************************************
// UpdateExtension()
//
// If original extention (when uCount is zero) is less than 3 bytes long,
// uCount is Appended.  Otherwise, the last character is replaced with uCount.
//
// This function correctly processes DBCS characters.  Assume that the
// extension is a  DBCS string and the second (last) character is a DBCS
// character (say .ABC, where BC is the DBCS character). The ".ABC" becomes
// ".Ax". Where 'x' is the current uCount setting.
//
// Parameters:
//      lpExt       [in/out]   Extension string to update
//      uCount      [in]       Current count.
//****************************************************************************
// 10/14/96 JBRENNA - Function created.
// 10/26/96 MKEATIN - Function majorly modified.
//****************************************************************************
static void UpdateExtension (LPSTR lpszExtention, UINT uCount)
{
    auto   TCHAR  szCount[2];
    auto   WORD   wStrLen  = STRLEN (lpszExtention);

    SYM_ASSERT (lpszExtention);
                                        // This routine only processes
    uCount = uCount > 9 ? 9 : uCount;   // 0-9 settings.

    szCount[0] = uCount + '0';          // Convert uCount to string szCount
    szCount[1] = EOS;

    if ((wStrLen < 3) && (uCount == 0)) // Only append if uCount is 0
        STRCAT (lpszExtention, szCount);
    else                                // Replace the last char with szCount.
    {
        if (DBCSGetByteType(&lpszExtention[0],
                            &lpszExtention[wStrLen-1]) == _MBC_TRAIL)
        {
            lpszExtention[wStrLen-2] = szCount[0];
            lpszExtention[wStrLen-1] = EOS;
        }
        else
        {
            lpszExtention[wStrLen-1] = szCount[0];
            lpszExtention[wStrLen]   = EOS;
        }
    }
}

//************************************************************************
// CreateBackup()
//
// For SYM_DOS or installs with Qurantine dissabled:
//      This routine creates a backup file in the same directory as the
//      source file using the given backup extension.
// For systems with Quarantine
//      This routine creates a backup file in Quarantine.
//
// Parameters:
//      LPREPAIRFILESTRUCT lpRepair     Repair info
//
// Returns:
//      TRUE                            Backup file successfully created.
//      FALSE                           Error creating file.
//************************************************************************
// 3/5/93  DALLEE modified.
// 6/9/93  DALLEE do not copy a file to itself.
// 6/23/98  JTAYLOR -- Changed the OEM versions to use ".vir" files.
//************************************************************************

BOOL SYM_EXPORT WINAPI CreateBackup (LPREPAIRFILESTRUCT lpRepair)
{
    auto    BOOL    bStatus = FALSE;

// do not attempt to use Quarantine from DOS.
#ifndef SYM_DOS
    auto    BOOL    bOEM = NAV_IS_SCANONLY();

    // Find out from the personality DLL if the install is the OEM version
    // If not OEM then use Quarantine for a backup mechanism.
    if( !bOEM )
    {
        auto    IQuarantineDLL* p;
        auto    HRESULT         hr;
        auto    BOOL            bFileApisAnsi;

        lpRepair->pQuarBackupItem = NULL;
        
        bFileApisAnsi = AreFileApisANSI();

        if (!bFileApisAnsi)
            SetFileApisToANSI ();

        hr = CoCreateInstance( CLSID_QuarantineDLL,
                                NULL,
                                CLSCTX_INPROC_SERVER,
                                IID_QuarantineDLL,
                                (LPVOID*)&p);

        if( FAILED( hr ) )
            bStatus = FALSE;
        else
        {

            hr = p->Initialize();

            if( FAILED( hr ) )
                bStatus = FALSE;
            else
            {
                hr = p->AddFileToQuarantine(lpRepair->szFullPath,
                                            QFILE_TYPE_NORMAL,
                                            QFILE_STATUS_BACKUP_FILE,
                                            &lpRepair->pQuarBackupItem);

                if( FAILED( hr ) )
                {
                    bStatus = FALSE;
                    lpRepair->pQuarBackupItem = NULL;
                }
                else
                {
                    bStatus = TRUE;
                }

                p->Release();
            }

        }

        if (bFileApisAnsi)
            SetFileApisToANSI ();
        else
            SetFileApisToOEM ();
    }
    if (!bStatus)
#endif // SYM_DOS
    // If we are in DOS mode, or are an OEM version, or if Quarantine failed,
    // then create a ".vir" file.
    {
        auto    char    szBack     [ SYM_MAX_PATH+1 ],
                        szBackExtOem  [ SYM_MAX_PATH+1 ];

        auto    UINT    i = 0;
        auto    BOOL    bApisWereAnsi = ARE_FILE_APIS_ANSI;

        bStatus = TRUE;
        
        STRCPY (szBack, lpRepair->szFullOemPath);

        NameStripExtension (szBack);

        STRCAT (szBack, ".");
        NavAnsiToOem (lpRepair->szBackExt, szBackExtOem);
        STRCAT (szBack, szBackExtOem);
                                            // -------------------------------
                                            // Do not attempt FileCopy a file
                                            // to itself - causes Sector
                                            // Not Found Error (in DOS), and
                                            // deletes the file.
                                            // -------------------------------
        while (!STRICMP(szBack, lpRepair->szFullOemPath) || FileExists (szBack))
            {
            if (i == 10)
                {
                bStatus = FALSE;
                break;
                }

            UpdateExtension (szBackExtOem, i);
            ++i;

            NameStripExtension (szBack);
            STRCAT (szBack, ".");
            STRCAT (szBack, szBackExtOem);
            }

        if (bStatus == TRUE)
        {
            // JBRENNA rev 1.5: Actually Win95 needs the APIs in OEM mode to
            // correctly create an ANSI repair file (Ex. Çommand.VIR) for a Inoc
            // Repair. I do not know why WinNT needs the APIs unchanged ... ask
            // PVENKAT.

            // PVENKAT rev 1.2: On NT/95 ANSI is supported only on DOS/WIN we need OEM
#if !defined(NAVWNT)
            if (bApisWereAnsi)
                SET_FILE_APIS_TO_OEM;
#endif
            bStatus = !FileCopy (lpRepair->szFullOemPath, szBack, NULL);
#if !defined(NAVWNT)
            if (bApisWereAnsi)
                SET_FILE_APIS_TO_ANSI;
#endif
        }

#if defined(SYM_WINNT)
                                            // If the copy succeeds, we will need
                                            // to copy the security from the old
                                            // file to the new file.
            FileCopySecurityInformation(
            lpFullOemPath,
            szBack,
            DACL_SECURITY_INFORMATION);

#endif
    }

    return (bStatus);
} // End CreateBackup()
