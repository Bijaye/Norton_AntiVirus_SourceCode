// Copyright 1994 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/navxutil/VCS/backup.cpv   1.0   06 Feb 1997 21:05:16   RFULLER  $
//
// Description:
//      This is the function to create a backup file.
//
// Contains:
//      BackupInfectedFile()
//***************************************************************************
// $Log:   S:/navxutil/VCS/backup.cpv  $
// 
//    Rev 1.0   06 Feb 1997 21:05:16   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:56:32   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:51:34   BARRY
// Initial revision.
// 
//    Rev 1.1   17 Apr 1995 15:46:40   MFALLEN
// sym_max_path+1
// 
//    Rev 1.0   16 Dec 1994 09:32:12   MARKL
// Initial revision.
// 
//    Rev 1.1   14 Dec 1994 18:03:20   BRAD
// Fixed bug which would continue to add 123.. to end of extension
// 
//    Rev 1.0   12 Dec 1994 15:38:30   RSTANEV
// Initial revision.
//
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "navutil.h"

//---------------------------------------------------------------------------
//
// BOOL WINAPI BackupInfectedFile (
//      LPSTR   lpszInfectedFile,       // Infected file (Input)
//      LPSTR   lpszBackupExtension,    // Extension for backup file (Input)
//      LPSTR   lpszBackupFile          // Name of backup file (Output)
//      );
//
// This function backs up an infected file, applying the extension
// specified by the user.
//
// Entry:
//      lpszInfectedFile
//      lpszBackupExtension
//      lpszBackupFile
//
// Exit:
//      TRUE  if the backup was successful.
//      FALSE if the backup was not successful.
//
//---------------------------------------------------------------------------
BOOL SYM_EXPORT WINAPI BackupInfectedFile (LPSTR lpszInfectedFile,
                                LPSTR lpszBackupExtension,
                                LPSTR lpszBackupFile)
    {
    BOOL    bStatus = TRUE;
    UINT    i = 0;
    int     nExtLen, nCountLen;
    char    szCount[10];
    char    szNewExtension[SYM_MAX_PATH+1];


    STRCPY(lpszBackupFile, lpszInfectedFile);
    NameStripExtension(lpszBackupFile);
    NameAppendExtension(lpszBackupFile, lpszBackupExtension);

                                        // Need to get a unique name for the
                                        // backup.
    nExtLen = STRLEN(lpszBackupExtension);
    while (FileExists(lpszBackupFile))
        {
        if (i > 999)
            {
            bStatus = FALSE;
            break;
            }

        _ConvertWordToString (i++, szCount, 10, 0);
        nCountLen = STRLEN(szCount);
        if (nCountLen + nExtLen == 3)
            {
            STRCPY(szNewExtension, lpszBackupExtension);
            }
        else if (nCountLen + nExtLen > 3)
            {
                                        // Make sure we use count portion
            STRNCPY(szNewExtension, lpszBackupExtension, 3 - nCountLen);
            szNewExtension [3 - nCountLen] = EOS;
            }
        else // nCountLen + nExtLen < 3
            {
            STRCPY(szNewExtension, lpszBackupExtension);
            StringAppendNChars(szNewExtension, '0', 3 - (nCountLen + nExtLen));
            }
        STRCAT(szNewExtension, szCount);

        NameStripExtension (lpszBackupFile);
        NameAppendExtension(lpszBackupFile, szNewExtension);
        }

    if (bStatus)
        bStatus = !FileCopy(lpszInfectedFile, lpszBackupFile, NULL);

    return(bStatus);
    }
