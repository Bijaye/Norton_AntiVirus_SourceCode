// Copyright 1995-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVC/VCS/search.c_v   1.5   04 Aug 1997 22:03:58   BGERHAR  $
//
// Description:
//     Definition and option search routines
//
//************************************************************************
// $Log:   S:/NAVC/VCS/search.c_v  $
// 
//    Rev 1.5   04 Aug 1997 22:03:58   BGERHAR
// Make defs dir easy to find by finding DEFLOC.DAT and redirecting to the def dir
// 
//    Rev 1.4   09 Jun 1997 18:35:26   BGERHAR
// Add AreDefsWild() and misc. clean-up
// 
//    Rev 1.3   08 Jun 1997 23:24:24   BGERHAR
// Don't wrap for deep scan or def directories
// 
//    Rev 1.2   06 Jun 1997 18:39:46   BGERHAR
// Update options and def strings and logic
// 
//    Rev 1.1   01 Jun 1997 20:33:58   BGERHAR
// Update definition/options search logic
// 
//    Rev 1.0   06 Feb 1997 20:56:16   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:19:46   MKEATIN
// Initial revision.
// 
//    Rev 1.6   11 Dec 1996 17:45:40   JBELDEN
// set up code so navcopts is looked for and if not found navopts.dat is
// used.
// 
//    Rev 1.5   06 Dec 1996 15:20:20   JBELDEN
// added variable for detecting when virwild.dat is being used.
// 
//    Rev 1.4   06 Dec 1996 11:44:32   JBELDEN
// added code so that virwild.dat would be looked for if virscan.dat
// was not found
// 
//    Rev 1.3   14 Nov 1996 15:05:52   JBELDEN
// removed some calls to functions not used.
// 
//    Rev 1.2   16 Oct 1996 11:58:04   JBELDEN
// removed code related to sfsstr.str since stsstr is removed and
// code is unneeded.
// 
//    Rev 1.1   07 Oct 1996 15:25:24   JBELDEN
// Fixed so that it can find settings in other places and
// removed GetCancelorESCHit since the dialog isn't used.
//
//    Rev 1.0   02 Oct 1996 12:30:24   JBELDEN
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"
#include "disk.h"
#include "file.h"                       // File/Dir Find Funcs WIN32/DOS

#include "navdprot.h"
#include "navutil.h"
#include "virscan.h"

#include <stdio.h>
#include <conio.h>

#define CFG_SWITCH_NOT_FOUND    0       // Returns from ReadCFGSwitch()
#define CFG_SWITCH_FOUND        1
#define CFG_SWITCH_INVALID      (-1)

/*----------------------------- Module Defines ----------------------------*/

//***************************************************************************
// FilesExistIn() -- Return TRUE if all files in FileList exist in this dir
// 
// Parameters:
//     lpszPath -- Path to check
// 
// Return Value: 
//     TRUE -- All files needed are in the specified dir
//     FALSE -- Directory is missing one or more required files.
// 
// See Also: 
//     DefFilesExistIn()
// 
//***************************************************************************
// 02/21/1995 ELEAR   Created (as DatFilesExistIn)
// 05/26/1997 BGERHAR Rewrote and generalized for separate Options and Def dirs
//***************************************************************************

BOOL PASCAL FilesExistIn( LPSTR lpszPath, char *lpFileList[] )
{
    int  iString = 0;

    for (iString = 0; *lpFileList[iString] != EOS; iString +=1)
        {
        if (!FileExistsInDirPath(lpszPath, lpFileList[iString]))
            {
            return FALSE;
            }
        }

    return TRUE;
}

//***************************************************************************
// FileExistsInDirPath() -- Return TRUE if the file exists in the path passed
// 
//***************************************************************************
// 06/01/1997 BGERHAR Function Created (based on old ELear function)
//***************************************************************************

BOOL PASCAL FileExistsInDirPath( LPSTR lpSearchPath, LPSTR szFileName )
{
    auto    char    szTempFilePath[ SYM_MAX_PATH + 20 ];

    STRCPY( szTempFilePath, lpSearchPath );
    NameAppendFile( szTempFilePath, szFileName );
    return ( FileExists( szTempFilePath ) );
}

//***************************************************************************
// InitOptionsDir() -- Locate and initialize the options directory
// 
// Return Value: 
//    TRUE -- Options directory found and initialized.
//    FALSE -- Can't find options (terminate program)
// 
// See Also: 
//    InitDefinitionsDir()
// 
//***************************************************************************
// 05/26/1997 BGERHAR Function Created.                                 
//***************************************************************************

BOOL PASCAL InitOptionsDir (VOID)
{
    extern      CFGTEXTREC      rOptions;
    extern      CFGTEXTREC      rDefinitions;
    extern      char            *FILELIST_OPTIONS[];
    extern      char            *FILELIST_NAVC_OPTIONS[];
    extern      char            SZ_INVALID_OPTIONS_FOLDER [];
    extern      char            SZ_OPTIONS_NOT_FOUND [];
    extern      char            SZ_OPTIONS_FOUND [];
                                        // NAVXUTIL,winmisc.cpp
    extern      char            szOptionsDir [];        

    char        szShortOptionsDir[80];
    int         iSwitchResult = CFG_SWITCH_NOT_FOUND;
    BOOL        bUsingDefsDirSwitch = FALSE;

    // TODO: Replace global 'szOptionsDir' with fn to set options dir.
    //char      szOptionsDir [SYM_MAX_PATH +1];

    szOptionsDir[0] = EOS;


    iSwitchResult = ReadPathSwitch(&rOptions, szOptionsDir);

                                        // For backwards compatibility,
                                        // Revert to /CFG switch if /OPT
                                        // not used.
    if (iSwitchResult != CFG_SWITCH_FOUND)
        {
        iSwitchResult = ReadPathSwitch(&rDefinitions, szOptionsDir);
        bUsingDefsDirSwitch = TRUE;
        }

    switch (iSwitchResult)
        {
        default:
        case CFG_SWITCH_FOUND:
            if (FilesExistIn(szOptionsDir, FILELIST_OPTIONS) ||
                FilesExistIn(szOptionsDir, FILELIST_NAVC_OPTIONS))
                {
                break;
                }

            // fall through to invalid case...

        case CFG_SWITCH_INVALID:
                                        // Don't whine if /CFG used
            if (!bUsingDefsDirSwitch)
                printf(SZ_INVALID_OPTIONS_FOLDER, szOptionsDir);

            // fall through to normal search...

        case CFG_SWITCH_NOT_FOUND:
            DOSGetProgramName(szOptionsDir);
            NameStripFile(szOptionsDir);
                                        // Complain if options not found
            if ( !FilesExistIn(szOptionsDir, FILELIST_OPTIONS) &&
                 !FilesExistIn(szOptionsDir, FILELIST_NAVC_OPTIONS))
                {
                printf(SZ_OPTIONS_NOT_FOUND);
                ErrorLevelSet(ERRORLEVEL_MISSINGFILE);
                return FALSE;
                }

            break;
        }

    SYM_ASSERT( EOS != szOptionsDir[0] );

                                        // Avoid wrapping (approximate room available)
    NameShortenFileName(szOptionsDir, szShortOptionsDir, 80-strlen(SZ_OPTIONS_FOUND));
    printf (SZ_OPTIONS_FOUND, szShortOptionsDir);

    return TRUE;
}

//***************************************************************************
// InitDefinitionsDir() -- Locate and initialize the definitions directory
//
// Description:
//    1) If /CFG used, use the directory specified.
//    2) If no defs found, use the directory specified as CDefUtils dir
//    3) If no defs found in /CFG directory, check if we were launched on
//       removable media. If so, ask the user to swap disks.
//    4) If /CFG used and no defs found, complain and exit.
//    5) If no defs found, search using CDefUtils
//    6) If no defs found, check arg0 directory
//    7) If no defs found, check if we were launched on removable media.
//       If so, ask the user to swap disks.
//    8) If no defs found, complain and exit.
//    9) If defs found, record the definition set found and set the dir
//
// NOTES:
//    1) Run before InitOptionsDir() so disk swap can occur if necessary
//    2) Searches for both full and wild definition set.
// 
// Return Value: 
//    TRUE -- Options directory found and initialized.
//    FALSE -- Can't find options (terminate program)
// 
// See Also: 
//    InitOptionsDir()
// 
//***************************************************************************
// 05/26/1997 BGERHAR Function Created.                                 
//***************************************************************************

BOOL PASCAL InitDefinitionsDir (VOID)
{
    extern      BOOL GetCDefUtilsDir(char *lpszDefsDir);
    extern      CFGTEXTREC      rDefinitions;
    extern      char *          FILELIST_DEFINITIONS[];
    extern      char *          FILELIST_WILDDEFS[];
    extern      char *          FILELIST_RESQDEFS[];
    extern      char            SZ_INVALID_DEFINITIONS_FOLDER[];
    extern      char            SZ_DEFINITIONS_NOT_FOUND[];
    extern      char            SZ_DEFINITIONS_FOUND[];
    extern      char            szUsingVirWild[];

    BOOL      bDefsFound = FALSE;
    char      szDefsDir [SYM_MAX_PATH+1];
    char      szShortDefsDir [80];

    szDefsDir[0] = EOS;
    SetDefsWild(FALSE);
                                        // Find through /CFG switch

    switch (ReadPathSwitch(&rDefinitions, szDefsDir))
        {
        default:
        case CFG_SWITCH_FOUND:

            if (FilesExistIn(szDefsDir, FILELIST_DEFINITIONS) ||
                FilesExistIn(szDefsDir, FILELIST_WILDDEFS))
                {
                bDefsFound = TRUE;
                break;
                }

            // See if Hawking can make sense of the directory passed
            // (Allow use to pass NAV directory in /CFG to simplify
            // finding defs vs. the true def directory)

            if (GetCDefUtilsDir(szDefsDir))
                {
                if (FilesExistIn(szDefsDir, FILELIST_DEFINITIONS) ||
                    FilesExistIn(szDefsDir, FILELIST_WILDDEFS))
                    {
                    bDefsFound = TRUE;
                    break;
                    }
                }

                                        // Restore szDefsDir in case
                                        // GetCDefUtilsDir() changed it.
            ReadPathSwitch(&rDefinitions, szDefsDir);

            // If running from removable media, ask user to swap disks

            if (DiskIsRemovable (szDefsDir[0]) && 
                FindDefsSwapDisk(szDefsDir))
                {
                bDefsFound = TRUE;
                break;
                }

            // fall through to invalid case...

        case CFG_SWITCH_INVALID:
            printf(SZ_INVALID_DEFINITIONS_FOLDER);
            ErrorLevelSet(ERRORLEVEL_MISSINGFILE);
            return FALSE;

        case CFG_SWITCH_NOT_FOUND:
            break;
        }

                                        // Find through CDefUtils
    if (!bDefsFound)
        {
        DOSGetProgramName(szDefsDir);
        NameStripFile(szDefsDir);

        if (GetCDefUtilsDir(szDefsDir))
            {
            if (FilesExistIn(szDefsDir, FILELIST_DEFINITIONS) ||
                FilesExistIn(szDefsDir, FILELIST_WILDDEFS))
                {
                bDefsFound = TRUE;
                }
            }
        }

                                        // Find in NAVC directory
    if (!bDefsFound)
        {

        DOSGetProgramName(szDefsDir);
        NameStripFile(szDefsDir);

        if ( FilesExistIn(szDefsDir, FILELIST_DEFINITIONS) ||
             FilesExistIn(szDefsDir, FILELIST_WILDDEFS))
            {
            bDefsFound = TRUE;
            }

        // If running from removable media, ask user to swap disk

        else if (DiskIsRemovable (szDefsDir[0]) &&
                 FindDefsSwapDisk(szDefsDir))
            {
            bDefsFound = TRUE;
            }
        }
    
                                        // Complain if defs not found
    if (!bDefsFound)
        {
        printf(SZ_DEFINITIONS_NOT_FOUND);
        ErrorLevelSet(ERRORLEVEL_MISSINGFILE);
        return FALSE;
        }

    SYM_ASSERT( EOS != szDefsDir[0] );
    VirusScanSetPath ( szDefsDir );     // Tell the system where the defs are

    NameShortenFileName(szDefsDir, szShortDefsDir, 80-strlen(SZ_DEFINITIONS_FOUND));
    printf (SZ_DEFINITIONS_FOUND, szShortDefsDir);

    if (!FilesExistIn(szDefsDir, FILELIST_DEFINITIONS))
        {
        if (DiskIsRemovable(szDefsDir[0]) && FilesExistIn(szDefsDir, FILELIST_RESQDEFS ));
        else
            {
            printf( szUsingVirWild );
            SetDefsWild(TRUE);
            }
        }

    return TRUE;
}

//***************************************************************************
// FindDefsSwapDisk() -- Look for defs on diskette (ask user to swap)
// 
// Description:
//     Asks the user to swap disks and insert a valid definitions disk.
// 
// Return Value: 
//     TRUE -- Disk in drive pointed to by lpszDefsDir has defs.
//             (lpszDefsDir may point to new location
//     FALSE -- Couldn't find defs. (lpszDefsDir intact)
// 
//***************************************************************************
// 06/01/1997 BGERHAR Function Created.                                 
//***************************************************************************

BOOL PASCAL FindDefsSwapDisk (char * lpszDefsDir)
{
    extern      char *          FILELIST_DEFINITIONS[];
    extern      char *          FILELIST_WILDDEFS[];
    extern      char *          FILELIST_RESQDEFS[];
    extern      char            SZ_INSERT_DEFS_DISK[];

    char szRoot[4];

    szRoot[0] = lpszDefsDir[0];
    szRoot[1] = ':';
    szRoot[2] = '\\';
    szRoot[3] = EOS;

    for (;;)
        {
        printf (SZ_INSERT_DEFS_DISK, szRoot[0]);

        if (getch() == ESC)
            return FALSE;

        if (FilesExistIn(lpszDefsDir, FILELIST_DEFINITIONS) ||
            FilesExistIn(lpszDefsDir, FILELIST_WILDDEFS)    ||
            FilesExistIn(lpszDefsDir, FILELIST_RESQDEFS))
            {
            break;
            }

        if ( FilesExistIn(szRoot, FILELIST_DEFINITIONS) ||
             FilesExistIn(szRoot, FILELIST_WILDDEFS)    ||
             FilesExistIn(szRoot, FILELIST_RESQDEFS))
            {
            STRCPY(lpszDefsDir, szRoot);
            break;
            }

        }

    return TRUE;
}

//************************************************************************
// ReadPathSwitch()
//
// This routine checks whether the /CFG or /OPT switch was found on the 
// command line. It also returns the setting of the switch converted to 
// a full path.
//
// Parameters:
//      LPCFGTEXTREC lprSwitch [in] CFGTEXTREC of option to find
//      LPSTR  lpszPath        [out] switch setting converted to full path.
//
// Returns:
//      CFG_SWITCH_NOT_FOUND   Switch wasn't present. lpszCFGPath empty
//      CFG_SWITCH_FOUND       Switch found and lpszCFGPath set to valid folder.
//      CFG_SWITCH_INVALID     Switch found but no options in lpszCFGPath.
//************************************************************************
// 03/08/95 DALLEE, created.
// 05/26/97 BGERHAR, Changed from ReadCFGSwitch() to ReadPathSwitch() to
//                   support any option in the form /OPT:[path]
//************************************************************************

int PASCAL ReadPathSwitch (LPCFGTEXTREC lprSwitch, LPSTR lpszPath)
{
    auto    UINT        uSwitchFound;

    if ( ConfigSwitchSet(lprSwitch, NULL) )
        {
        ConfigSwitchGetText(lprSwitch, lpszPath, SYM_MAX_PATH);
        NameToFull(lpszPath, lpszPath);
        STRUPR(lpszPath);

        uSwitchFound = CFG_SWITCH_FOUND;
        }
    else
        {
        uSwitchFound = CFG_SWITCH_NOT_FOUND; //NOT_FOUND;
        }

    return (uSwitchFound);
} // ReadPathSwitch()

//***************************************************************************
// SetDefsWild()/AreDefsWild() -- Determine if wild definition set is in use
// 
//***************************************************************************
// 06/09/1997 BGERHAR Function Created.                                 
//***************************************************************************

STATIC BOOL bWildDefs = FALSE;

VOID PASCAL SetDefsWild(BOOL bAreWild)
{
    bWildDefs = bAreWild;
}

BOOL PASCAL AreDefsWild(VOID)
{
    return bWildDefs;
}


