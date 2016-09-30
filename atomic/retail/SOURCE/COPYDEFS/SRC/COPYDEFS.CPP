//************************************************************************
// Copyright 1997 Symantec Corporation
// _______________________________________________________________________
//
// $Header:   S:/COPYDEFS/VCS/copydefs.cpv   1.0   19 Feb 1998 10:11:28   mdunn  $
//
// Description: Source file for the CopyDefs applet.
//
//************************************************************************
//  $Log:   S:/COPYDEFS/VCS/copydefs.cpv  $
//
//    Rev 1.0   19 Feb 1998 10:11:28   mdunn
// Initial revision.
//
//    Rev 1.0   25 Sep 1997 19:09:18   mdunn
// Initial revision.
//************************************************************************

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <conio.h>
#include "utils.h"
#include "resource.h"
#include "defutils.h"
#include "resqloc.h"

// Local function prototypes

BOOL MustPrintHelp ( int, TCHAR** );
void PrintHelp ( TCHAR** );
BOOL LocateDefs();
BOOL PromptForDisk(TCHAR *);
//************************************************************************
// Copyright 1997 Symantec Corporation
// _______________________________________________________________________
//
// $Header:   N:/COPYDEFS/VCS/COPYDEFS.RCv   1.1   19 Feb 1998 10:21:02   mdunn  $
//
// Description: Resouce script for the CopyDefs applet.
//
//************************************************************************
//  $Log:   N:/COPYDEFS/VCS/COPYDEFS.RCv  $
//
//    Rev 1.1   19 Feb 1998 10:21:02   mdunn
// Updated version number and copyright date.
//
//    Rev 1.0   19 Feb 1998 10:15:18   mdunn
// Initial revision.
//
//    Rev 1.0   25 Sep 1997 19:11:58   mdunn
// Initial revision.
//************************************************************************

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <conio.h>
#include "utils.h"
#include "resource.h"
#include "defutils.h"

// Local function prototypes

BOOL MustPrintHelp ( int, TCHAR** );
void PrintHelp ( TCHAR** );
BOOL LocateDefs();
BOOL PromptForDisk();
BOOL CheckForEnoughDiskSpace();
BOOL CopyFiles();


// This is used during the main loop - each constant is one of the steps
// in the app.

enum {
    ACT_STARTUP_BANNER = 0,
    ACT_CHECK_FOR_HELP,
    ACT_LOCATE_DEFS,
    ACT_PROMPT_FOR_DISK,
    ACT_CHECK_DISK_SPACE,
    ACT_COPY_FILES,
    ACT_WRAP_THINGS_UP,
    ACT_LAST                            // sentinel - end of the program
};


// Global variables

TCHAR     g_szDefsDir [ MAX_PATH ];
TCHAR     g_cDestinationDrive;
TCHAR     g_szDestinationFile [ MAX_PATH ];
CDefUtils g_defs;
BOOL      g_bDefUtilsInitialized = FALSE;


int _tmain ( int argc, TCHAR** argv )
{
int nStep;

    for ( nStep = ACT_STARTUP_BANNER;
          nStep < ACT_LAST;
          nStep++ )
        {
                                        // I'm using if/else's here instead
                                        // of a switch so that a break will
                                        // take us all the way to the end of
                                        // the program.
        if ( nStep == ACT_STARTUP_BANNER )
            {
            PrintMessage ( IDS_APP_TITLE );
            }

        else if ( nStep == ACT_CHECK_FOR_HELP )
            {
            if ( MustPrintHelp ( argc, argv ) )
                {
                PrintHelp ( argv );
                break;
                }
            else
                {
                // Store the drive letter.
                g_cDestinationDrive = argv[1][0];
                }
            }

        else if ( nStep == ACT_LOCATE_DEFS )
            {
            if ( ! LocateDefs() )
                {
                break;
                }
            }

        else if ( nStep == ACT_PROMPT_FOR_DISK )
            {
/*
            if ( ! PromptForDisk() )
                {
                break;
                }
*/
            }

        else if ( nStep == ACT_CHECK_DISK_SPACE )
            {
/*
            if ( ! CheckForEnoughDiskSpace() )
                {
                break;
                }
*/
            }

        else if ( nStep == ACT_COPY_FILES )
            {
            if ( ! CopyFiles() )
                {
                break;
                }
            }

        else if ( nStep == ACT_WRAP_THINGS_UP )
            {
            PrintMessage ( IDS_COPY_SUCCESSFUL );
            }
        }   // end for


    // If we registered ourselves as a virus defs client with CDefUtils,
    // stop using the virus defs now.
    if ( g_bDefUtilsInitialized )
        {
        g_defs.StopUsingDefs();
        }

    return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    MustPrintHelp()
//
// Description:
//  Tests argc and argv and determines if help should be printed.  Help will
//  be printed if:
//      1> argc != 2; or
//      2> argv[1] begins with any of the following: "/?" "/h" "-?" "-h"; or
//      3> argv[1] is not a help switch listed in 2> and is not of the form "x:",
//         where 'x' is the drive letter.
//
// Input:
//  argc, argv: the standard parameters to main()
//
// Returns:
//  TRUE if help should be printed, or FALSE if not.
//
/////////////////////////////////////////////////////////////////////////////
//  9/20/97 Mike Dunn -- function created
/////////////////////////////////////////////////////////////////////////////

BOOL MustPrintHelp ( int argc, TCHAR** argv)
{
TCHAR szArg[3] = { 0, 0, 0 };

    // 1st case - argc is not 2 (the program name + drive letter/help switch)

    if ( argc != 2 )
        {
        return TRUE;
        }

    // A little shortcut here - a valid argv[1] must have length >= 2, so
    // we can check the length here and return TRUE if it's < 2.

    if ( _tcslen ( argv[1] ) < 2 )
        {
        return TRUE;
        }

    // Grab the first 2 characters from argv[1], since that's all we care
    // about from now on.

    _tcsncpy ( szArg, argv[1], 2 );
    _tcslwr ( szArg );

    // 2nd case - argv[1] is a help switch

    if ( ( szArg[0] == '/' || szArg[0] == '-' )  &&
         ( szArg[1] == '?' || szArg[1] == 'h' )     )
        {
        return TRUE;
        }

    // 3rd case - argv[1] is not a drive name

    szArg[0] = _totlower ( szArg[0] );

    if ( szArg[0] >= 'a'  &&  szArg[0] <= 'z'  &&  szArg[1] == ':' )
        {
        return FALSE;
        }
    else
        {
        return TRUE;
        }
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    PrintHelp()
//
// Description:
//  Prints help for the application.
//
// Input:
//  argv: the standard main() parameter.
//
// Returns:
//  Nothing.
//
/////////////////////////////////////////////////////////////////////////////
//  9/20/97 Mike Dunn -- function created
/////////////////////////////////////////////////////////////////////////////

void PrintHelp ( TCHAR** argv )
{
    PrintMessage ( IDS_HELP1 );

    _tprintf ( LLoadString ( IDS_USAGE ),
               LNameStripExtension ( LNameReturnFile ( argv[0] ) ) );

    PrintMessage ( IDS_HELP2 );
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    LocateDefs()
//
// Description:
//  Locates the directory that holds the defs using the Hawkings API,
//  and if successful, stores the dir in the global variable g_szDefsDir.
//
// Input:
//  Nothing. (uses global variables g_defs, g_szDefsDir)
//
// Returns:
//  TRUE if the defs were located, FALSE if not.
//
/////////////////////////////////////////////////////////////////////////////
//  9/20/97 Mike Dunn -- function created
/////////////////////////////////////////////////////////////////////////////

BOOL LocateDefs()
{
BOOL bRetVal;

    g_defs.InitWindowsApp ( _T("CopyDefs") );

    g_bDefUtilsInitialized = TRUE;

    bRetVal = g_defs.UseNewestDefs ( g_szDefsDir, MAX_PATH );

    if ( !bRetVal )
        {
        PrintMessage ( IDS_CANT_FIND_DEFS );
        PrintMessage ( IDS_ABORTED );
        }

    return bRetVal;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    PromptForDisk()
//
// Description:
//  If the global variable g_cDestinationDrive is a removable drive, this
//  fn prompts the user to insert a disk into the drive.  The user can
//  cancel the copy by hitting ESC.
//  If the drive is not removable, this fn does nothing.
//
// Input:
//  Nothing (reads global g_cDestinationDrive)
//
// Returns:
//  FALSE only if the user wants to cancel the copy, TRUE if the app should
//  continue on.
//
/////////////////////////////////////////////////////////////////////////////
//  9/20/97 Mike Dunn -- function created
/////////////////////////////////////////////////////////////////////////////

BOOL PromptForDisk(TCHAR *szDiskLabel)
{
TCHAR  szRoot[] = _T("x:\\");
int    nKeypress;

    szRoot[0] = g_cDestinationDrive;

    if ( DRIVE_REMOVABLE == GetDriveType ( szRoot ) )
        {
        _tprintf ( LLoadString ( IDS_INSERT_DISK ), szDiskLabel, g_cDestinationDrive );

        while ( ! _kbhit() )
            { }

        // Get the key just pressed, and bail if it was escape

        nKeypress = _getch();

        if ( nKeypress == 0x1B )        // the user hit ESC
            {
            PrintMessage ( IDS_CANCELED );

            return FALSE;
            }
        }

    return TRUE;
}

BOOL CheckForEnoughDiskSpace()
{
BOOL   bRetVal;
TCHAR  szRoot[] = _T("x:\\");
DWORD  dwSPC, dwBPS, dwNFC, dwTNC, dwFreeSpace, dwSpaceNeeded;

    szRoot[0] = g_cDestinationDrive;

    if ( 0 == GetDiskFreeSpace ( szRoot, &dwSPC, &dwBPS, &dwNFC, &dwTNC ) )
        {
        _tprintf ( LLoadString ( IDS_ERROR_READING_DRIVE ),
                   g_cDestinationDrive );

        PrintMessage ( IDS_ABORTED );

        bRetVal = FALSE;
        }
    else
        {
        dwFreeSpace = dwSPC * dwBPS * dwNFC * dwTNC;

        //&^ temp for now
        dwSpaceNeeded=0;

//        dwSpaceNeeded = CalcSpaceNeeded();

        if ( dwSpaceNeeded > dwFreeSpace )
            {
            PrintMessage ( IDS_NOT_ENOUGH_SPACE );

            PrintMessage ( IDS_ABORTED );

            bRetVal = FALSE;
            }
        else
            {
            bRetVal = TRUE;
            }
        }

    return bRetVal;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    CopyFiles()
//
// Description:
//  Copies all the necessary files to the drive specified in the global
//  variable g_cDestinationDrive.
//
// Input:
//  wDiskNum - the Definitions Disk Number (the defs now span multiple disks)
//
// Returns:
//  TRUE if all files were copied successfully, FALSE if an error occured
//  while copying.
//
/////////////////////////////////////////////////////////////////////////////
//  9/20/97 Mike Dunn -- function created
/////////////////////////////////////////////////////////////////////////////

BOOL CopyFiles()
{
    BOOL      bResult = TRUE;
    int       iNumberOfDisks = 0;
    TCHAR     szCopyingMsg     [ 64 ];
    TCHAR     szNAVDir         [ MAX_PATH ];
    TCHAR     szSourceFile     [ MAX_PATH ];
    TCHAR     szDestFile       [ MAX_PATH ];
    TCHAR     szResqLocFile    [ MAX_PATH ];
    TCHAR     szCopyDefsIniFile[ MAX_PATH ];
    TCHAR     szFilename       [MAX_PATH];
    TCHAR     szDiskLabel      [ 80 ] = "";

    // Read the NAV directory from the registry for later use.

    bResult = GetNAVDir(szNAVDir, sizeof(szNAVDir));

    if (!bResult)
    {
        // couldn't find the NAV directory!! aaaahhh!
        PrintMessage(IDS_CANT_FIND_NAV);
        PrintMessage(IDS_ABORTED);
        return FALSE;
    }

    // get the path to CopyDefs.ini

    _tcscpy(szCopyDefsIniFile, szNAVDir);
    LNameAppendFile(szCopyDefsIniFile, _T("CopyDefs.ini"));

    // First, find out how many disks there are

    for (iNumberOfDisks = 0;; ++iNumberOfDisks)
    {
        TCHAR szDiskNumber[4];
        _itoa(iNumberOfDisks + 1, szDiskNumber, 10);

        GetPrivateProfileString(RESQDISKLABELS_SECTION,
                                szDiskNumber,
                                KEY_MISSING,
                                szDiskLabel,
                                sizeof(szDiskLabel),
                                szCopyDefsIniFile);

       if (!_tcscmp(szDiskLabel, _T(KEY_MISSING)))
       {
           // no more disks

           break;
       }

    }

    // next create ResqLoc.dat

    for (int iCurrentDiskNum = 1; iCurrentDiskNum <= iNumberOfDisks; ++iCurrentDiskNum)
    {
        // get the disk label and prompt for it

        TCHAR szDiskNumber[4];
        _itoa(iCurrentDiskNum, szDiskNumber, 10);
        GetPrivateProfileString(RESQDISKLABELS_SECTION,
                                szDiskNumber,
                                KEY_MISSING,
                                szDiskLabel,
                                sizeof(szDiskLabel),
                                szCopyDefsIniFile);

        // now start write out ResqLoc.dat

        // get the path to ResqLoc.dat

        _tcscpy(szResqLocFile, szNAVDir);
        LNameAppendFile(szResqLocFile, _T(RESQLOC_INI_FILE));

        // now list file names ResqLoc.dat

        for (int iFileKey = 1;; ++iFileKey)
        {
            TCHAR szFileKey[4];
            _itoa(iFileKey, szFileKey, 10);

            GetPrivateProfileString(szDiskNumber,
                                    szFileKey,
                                    KEY_MISSING,
                                    szFilename,
                                    sizeof(szFilename),
                                    szCopyDefsIniFile);

            if (!_tcscmp(szFilename, _T(KEY_MISSING)))
            {
                // no more files to list

                break;
            }

            // now write out that label to ResqLoc.dat

            WritePrivateProfileString(RESQDEFSLOC_SECTION,
                                      szFilename,
                                      szDiskLabel,
                                      szResqLocFile);
        }
    }

    // now start copying files

    _tcscpy(szCopyingMsg, LLoadString (IDS_COPYING));

    for (iCurrentDiskNum = 1; iCurrentDiskNum <= iNumberOfDisks; ++iCurrentDiskNum)
    {
        if (!bResult)
        {
            // break out if there was an error copying a file

            break;
        }

        // get the disk label and prompt for it

        TCHAR szDiskNumber[4];
        _itoa(iCurrentDiskNum, szDiskNumber, 10);
        GetPrivateProfileString(RESQDISKLABELS_SECTION,
                                szDiskNumber,
                                KEY_MISSING,
                                szDiskLabel,
                                sizeof(szDiskLabel),
                                szCopyDefsIniFile);

        if (!PromptForDisk(szDiskLabel))
        {
            return FALSE;
        }


        // copy the def files for this disk

        for (int iFileKey = 1; bResult; ++iFileKey)
        {
            TCHAR szFileKey[4];
            _itoa(iFileKey, szFileKey, 10);

            GetPrivateProfileString(szDiskNumber,
                                    szFileKey,
                                    KEY_MISSING,
                                    szFilename,
                                    sizeof(szFilename),
                                    szCopyDefsIniFile);

            if (!_tcscmp(szFilename, _T(KEY_MISSING)))
            {
                // no more files to list

                break;
            }

            // get path to the source file - first check the defs dir

            _tcscpy(szSourceFile, g_szDefsDir);
            LNameAppendFile(szSourceFile, szFilename);

            // see if it exists

            OFSTRUCT FileInfo;
            if (OpenFile(szSourceFile, &FileInfo, OF_EXIST) == HFILE_ERROR)
            {
                // not in the defs dir...so go check the NAV dir

                _tcscpy(szSourceFile, szNAVDir);
                LNameAppendFile(szSourceFile, szFilename);

                if (OpenFile(szSourceFile, &FileInfo, OF_EXIST) == HFILE_ERROR)
                {
                    // not in the NAV dir...so go check the Windows dir

                    GetWindowsDirectory(szSourceFile, sizeof(szSourceFile));
                    LNameAppendFile(szSourceFile, szFilename);

                    if (OpenFile(szSourceFile, &FileInfo, OF_EXIST) == HFILE_ERROR)
                    {
                        // not in the Windows dir...error

                        bResult = FALSE;
                        break;
                    }
                }
            }

            // get path to the dest file

            _stprintf(szDestFile, _T("%c:\\%s"), g_cDestinationDrive, szFilename);

            // Display a status message.

            _tprintf(szCopyingMsg, szFilename);

            // now copy that file

            bResult = CopyFile(szSourceFile, szDestFile, FALSE);

            if (bResult)
            {
                PrintMessage (IDS_DONE);
            }
            else
            {
                PrintMessage (IDS_ERROR);
                break;
            }
        }

        // now copy the files from the [AllDisks] section onto this disks

        for (iFileKey = 1; bResult; ++iFileKey)
        {
            TCHAR szFileKey[4];
            _itoa(iFileKey, szFileKey, 10);

            GetPrivateProfileString(_T("AllDisks"),
                                    szFileKey,
                                    KEY_MISSING,
                                    szFilename,
                                    sizeof(szFilename),
                                    szCopyDefsIniFile);

            if (!_tcscmp(szFilename, _T(KEY_MISSING)))
            {
                // no more files to list

                break;
            }

            // get path to the source file - first check the defs dir

            _tcscpy(szSourceFile, g_szDefsDir);
            LNameAppendFile(szSourceFile, szFilename);

            // see if it exists

            OFSTRUCT FileInfo;
            if (OpenFile(szSourceFile, &FileInfo, OF_EXIST) == HFILE_ERROR)
            {
                // not in the defs dir...so go check the NAV dir

                _tcscpy(szSourceFile, szNAVDir);
                LNameAppendFile(szSourceFile, szFilename);

                if (OpenFile(szSourceFile, &FileInfo, OF_EXIST) == HFILE_ERROR)
                {
                    // not in the NAV dir...so go check the Windows dir

                    GetWindowsDirectory(szSourceFile, sizeof(szSourceFile));
                    LNameAppendFile(szSourceFile, szFilename);

                    if (OpenFile(szSourceFile, &FileInfo, OF_EXIST) == HFILE_ERROR)
                    {
                        // not in the Windows dir...error

                        bResult = FALSE;
                        break;
                    }
                }
            }

            // get path to the dest file

            _stprintf(szDestFile, _T("%c:\\%s"), g_cDestinationDrive, szFilename);

            // Display a status message.

            _tprintf(szCopyingMsg, szFilename);

            // now copy that file

            bResult = CopyFile(szSourceFile, szDestFile, FALSE);

            if (bResult)
            {
                PrintMessage (IDS_DONE);
            }
            else
            {
                PrintMessage (IDS_ERROR);
                break;
            }
        }

        // Display an error message if there was an error copying

        if ( !bResult )
        {
            _tprintf(LLoadString(IDS_ERROR_DURING_COPY), szFilename);
            PrintMessage(IDS_ABORTED);
            return bResult;
        }
    }

    return bResult;
}
