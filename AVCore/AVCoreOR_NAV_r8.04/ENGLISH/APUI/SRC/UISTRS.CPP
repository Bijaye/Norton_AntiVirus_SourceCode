// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   N:/APUI/VCS/uistrs.cpv   1.13   27 Apr 1998 16:59:32   jbrenna  $
//
// Description:
//      This file defines translatable strings.
//
// Contains:
//      -
//
// See Also:
//      uistrs.h, navap.h, actions.h
//
//***************************************************************************
// $Log:   N:/APUI/VCS/uistrs.cpv  $
// 
//    Rev 1.13   27 Apr 1998 16:59:32   jbrenna
// Removed 'known' from 'known virus'.
// 
//    Rev 1.12   02 Mar 1998 12:05:28   DALLEE
// Added success messages for Delete and Quarantine.
// 
//    Rev 1.11   20 Feb 1998 19:34:58   RStanev
// Added uMessageBoxBottom.
//
//    Rev 1.10   20 Feb 1998 17:16:18   RStanev
// Added dynamic adjustment of message boxes.
//
//    Rev 1.9   19 Feb 1998 15:09:42   RStanev
// Removed unknown virus detection.  Added quarantine support.  Added
// successful action confirmation.
//
//    Rev 1.8   17 Feb 1998 20:53:16   RStanev
// Updated copyright.
//
//    Rev 1.7   13 Sep 1997 13:37:42   RStanev
// Synchronized apszInitErrors[] with UIINITERROR.
//
//    Rev 1.6   08 Jul 1997 13:04:48   ROB
// Fixed label.
//
//    Rev 1.5   08 Jul 1997 13:01:30   ROB
// Added test labels for common alert strings.
//
//
//    Rev 1.4   28 May 1997 13:04:56   RStanev
// Added long custom alert and quarantine strings.  Added string for
// MSG_Unable_To_Locate_Scan_Directory.
//
//    Rev 1.3   21 May 1997 04:32:34   RStanev
// Updated apszInitErrors[] and apszControlledInitMessages[].  Limited
// the definition of strings and tables used when NAVAP's self test had
// failed to SYM_VXD only.
//
//    Rev 1.2   15 May 1997 02:34:48   RStanev
// Organized strings.
//
//    Rev 1.1   01 May 1997 14:19:32   RStanev
// Removed apvSelfCheckFailedOneHalfInMemory[].
//
//    Rev 1.0   09 Apr 1997 12:17:06   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "navap.h"
#include "uistrs.h"

//---------------------------------------------------------------------------
//
// The RECT_* macros are used to define the dimensions of NAVAP's dialogs.
//
// These macros should be used as follows:
//
// RECT_*(name_of_symbol,left_border,top_border,right_border,bottom_border);
//
// When translating the strings, you can change from one type of RECT_*
// message box to a different one in order to achieve desired results.
//
// RECT_FIXED defines a fixed message box, whose coordinates will never be
// changed.  If the text in the message box is too long to fit, it will be
// truncated (or however SYMEvent's AsyncMsgBox() handles it).
//
// RECT_DYNAMIC_B will define a message box whose bottom border will be
// stretched if the text does not fit in the requested box.  Note, that if
// the text for the message box fits, and there is a lot of empty space
// (let's say, you have given top_border of 5, bottom_border of 15, and you
// have asked for just one line of text) the box will not be shrunk!
//
// RECT_DYNAMIC_TB will define a message box whose top and bottom borders
// will be stretched if the text is too large to be displayed.  The way the
// borders are stretched, is by adding half of the number of extra lines
// needed to the bottom border, and subtracting half of the number of extra
// lines needed from the top border.  Note, that just like RECT_DYNAMIC_B,
// the message box will not be shrunk under any circumstance.
//
// The variable uMessageBoxBottom defines the highest value that the
// bottom_border parameter of the dynamic RECT macros can be adjusted to.
// Usually the text mode that SYMEvent switches to when displaying a message
// is 80x25, so unless you know that the particular hardware on the machine
// supports more lines, don't change its value.
//
//---------------------------------------------------------------------------
#define RECT_FIXED(n,l,t,r,b)       RECT n = {l,t,r,b}
#define RECT_DYNAMIC_B(n,l,t,r,b)   RECT n = {l,t,r,b | 0x8000L}
#define RECT_DYNAMIC_TB(n,l,t,r,b)  RECT n = {l,t | 0x8000L,r,b | 0x8000L}

UINT uMessageBoxBottom = 25;

//---------------------------------------------------------------------------
//
// Buttons displayed in a standard NAV Auto Protect dialog box.
//
// Notes:
//      These strings should be OEM.
//      Keep this table synchronized with AVACTION in actions.h(INCLUDE).
//
//---------------------------------------------------------------------------
TCHAR *apszButtons[] =
    {
    NULL,
    _T("&OK"),
    _T("&Stop"),
    _T("&Continue"),
    _T("&Repair"),
    _T("&Delete"),
    _T("&Exclude"),
    _T("&Quarantine"),
    _T("&Move"),
    _T("Re&name"),
    NULL,
    NULL
    };

//---------------------------------------------------------------------------
//
// Strings and tables displayed when an infected file is found.
//
// Note:
//      The strings should be OEM.
//
//---------------------------------------------------------------------------
TCHAR szAutoProtectTitle[] = _T("Norton AntiVirus Auto-Protect");

void *apvInfectedFileFound[] =
    {
    _T("VIRUS FOUND: The file "),
    UI_FILE_IN_TROUBLE,
    _T(" is infected with the "),
    UI_VIRUS_NAME,
    _T(" virus."),
    szDoubleNewLine,
    szWhatToDo,
    UI_END_OF_TABLE
    };

void *apvInfectedFileRepairSuccessful[] =
    {
    _T("The virus "),
    UI_VIRUS_NAME,
    _T(" was successfully removed from "),
    UI_FILE_IN_TROUBLE,
    _T(".  It is now safe to access this file."),
    UI_END_OF_TABLE
    };

void *apvInfectedFileRepairFailed[] =
    {
    _T("Unable to repair the file "),
    UI_FILE_IN_TROUBLE,
    _T(".  It is still infected with the "),
    UI_VIRUS_NAME,
    _T(" virus."),
    szDoubleNewLine,
    szWhatToDo,
    UI_END_OF_TABLE
    };

void *apvInfectedFileDeleteSuccessful[] =
    {
    _T("The infected file "),
    UI_FILE_IN_TROUBLE,
    _T(" was successfully deleted.  You should replace this file with an ") \
    _T("uninfected copy of the original."),
    UI_END_OF_TABLE
    };

void *apvInfectedFileDeleteFailed[] =
    {
    _T("Unable to delete the file "),
    UI_FILE_IN_TROUBLE,
    _T(".  It is still infected with the "),
    UI_VIRUS_NAME,
    _T(" virus."),
    szDoubleNewLine,
    szWhatToDo,
    UI_END_OF_TABLE
    };

void *apvInfectedFileExcludeFailed[] =
    {
    _T("Unable to exclude the file "),
    UI_FILE_IN_TROUBLE,
    _T(" from virus detection.  It is still infected with the "),
    UI_VIRUS_NAME,
    _T(" virus."),
    szDoubleNewLine,
    szWhatToDo,
    UI_END_OF_TABLE
    };

void *apvInfectedFileQuarantineSuccessful[] =
    {
    _T("The infected file "),
    UI_FILE_IN_TROUBLE,
    _T(" was successfully quarantined."),
    UI_END_OF_TABLE
    };

void *apvInfectedFileQuarantineFailed[] =
    {
    _T("Unable to quarantine the file "),
    UI_FILE_IN_TROUBLE,
    _T(".  It is still infected with the "),
    UI_VIRUS_NAME,
    _T(" virus."),
    szDoubleNewLine,
    szWhatToDo,
    UI_END_OF_TABLE
    };

void *apvInfectedFileMoveFailed[] =
    {
    _T("Unable to move the file "),
    UI_FILE_IN_TROUBLE,
    _T(".  It is still infected with the "),
    UI_VIRUS_NAME,
    _T(" virus."),
    szDoubleNewLine,
    szWhatToDo,
    UI_END_OF_TABLE
    };

void *apvInfectedFileRenameFailed[] =
    {
    _T("Unable to rename the file "),
    UI_FILE_IN_TROUBLE,
    _T(".  It is still infected with the "),
    UI_VIRUS_NAME,
    _T(" virus."),
    szDoubleNewLine,
    szWhatToDo,
    UI_END_OF_TABLE
    };

void *apvInfectedFileFoundStopNotify[] =
    {
    _T("The file "),
    UI_FILE_IN_TROUBLE,
    _T(" is infected with the "),
    UI_VIRUS_NAME,
    _T(" virus.  Access to this file will be denied."),
    UI_END_OF_TABLE
    };

void *apvInfectedFileFoundShutdownNotify[] =
    {
    _T("The file "),
    UI_FILE_IN_TROUBLE,
    _T(" is infected with the "),
    UI_VIRUS_NAME,
    _T(" virus."),
    szDoubleSpace,
    szSystemWillBeShutdown,
    UI_END_OF_TABLE
    };

RECT_DYNAMIC_TB ( rFileInfectedMessageBoxRect, 5, 10, 75, 15 );

//---------------------------------------------------------------------------
//
// Strings and tables displayed while scanning boot records.
//
// Note:
//      The strings should be OEM.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

void *apvScanningBootMessage[] =
    {
    _T("Scanning the boot record of drive "),
    UI_DRIVE_TO_SCAN,
    _T(" for viruses."),
    UI_END_OF_TABLE
    };

RECT_FIXED ( rScanningBootMessageBoxRect, 5, 9, 75, 17 );

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables displayed when a boot sector has been infected with a
// known virus.
//
// Note:
//      The strings should be OEM.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

void *apvInfectedBootFound[] =
    {
    _T("VIRUS FOUND: The boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T(" is infected with the "),
    UI_VIRUS_NAME,
    _T(" virus."),
    szDoubleNewLine,
    szWhatToDo,
    UI_END_OF_TABLE
    };

void *apvInfectedBootFoundRemoveDisk[] =
    {
    _T("The boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T(" is infected with the "),
    UI_VIRUS_NAME,
    _T(" virus.  You must repair the infected disk or remove it from the drive."),
    szDoubleNewLine,
    szWhatToDo,
    UI_END_OF_TABLE
    };

void *apvInfectedBootRepairSuccessful[] =
    {
    _T("The virus "),
    UI_VIRUS_NAME,
    _T(" was successfully removed from the boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T("."),
    UI_END_OF_TABLE
    };

void *apvInfectedBootRepairFailed[] =
    {
    _T("Unable to repair the boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T(".  It is still infected with the "),
    UI_VIRUS_NAME,
    _T(" virus."),
    UI_END_OF_TABLE
    };

void *apvInfectedBootRepairFailedRemoveDisk[] =
    {
    _T("Unable to repair the boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T(".  It is still infected with the "),
    UI_VIRUS_NAME,
    _T(" virus.  You must remove the infected disk from the drive."),
    UI_END_OF_TABLE
    };

void *apvInfectedBootFoundNotify[] =
    {
    _T("The boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T(" is infected with the "),
    UI_VIRUS_NAME,
    _T(" virus."),
    UI_END_OF_TABLE
    };

void *apvInfectedBootFoundNotifyRemoveDisk[] =
    {
    _T("The boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T(" is infected with the "),
    UI_VIRUS_NAME,
    _T(" virus.  You must remove the infected disk from the drive."),
    UI_END_OF_TABLE
    };

void *apvInfectedBootFoundShutdownNotify[] =
    {
    _T("The boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T(" is infected with the "),
    UI_VIRUS_NAME,
    _T(" virus."),
    szDoubleNewLine,
    szSystemWillBeShutdown,
    UI_END_OF_TABLE
    };

RECT_DYNAMIC_TB ( rBootInfectedMessageBoxRect, 5, 10, 75, 15 );

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables displayed when memory has been infected with a
// known virus.
//
// Note:
//      The strings should be OEM.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

void *apvInfectedMemoryFoundStartupNotify[] =
    {
    _T("VIRUS IN MEMORY: The "),
    UI_VIRUS_NAME,
    _T(" virus was found in memory."),
    szDoubleNewLine,
    szSystemWillBeShutdown,
    UI_END_OF_TABLE
    };

RECT_FIXED ( rMemoryInfectedMessageBoxRect, 5, 8, 75, 18 );

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables used when NAVAP's self test had failed.
//
// Note:
//      The strings should be OEM.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

TCHAR szSelfTestShutdown[] = _T(" Please wait for the system to shut down. ") \
                             _T("Then restart your computer from your NAV Rescue ") \
                             _T("Disk and scan for viruses. If no virus is ") \
                             _T("found, try reinstalling Norton AntiVirus ") \
                             _T("from your original disks.");

void *apvSelfCheckFailedNoCRC[] =
    {
    _T("The file NAVAP.VXD has been altered. This may be due to virus activity."),
    szSelfTestShutdown,
    UI_END_OF_TABLE
    };

void *apvSelfCheckFailedCRCChanged[] =
    {
    _T("The file NAVAP.VXD has been altered. This may be due to virus activity."),
    szSelfTestShutdown,
    UI_END_OF_TABLE
    };

void *apvSelfCheckFailedIOError[] =
    {
    _T("I/O error while self-testing NAV Auto-Protect."),
    szDoubleSpace,
    szSelfTestShutdown,
    UI_END_OF_TABLE
    };

void *apvSelfCheckFailedInternal[] =
    {
    _T("Internal error while self-testing NAV Auto-Protect."),
    szDoubleSpace,
    szSelfTestShutdown,
    UI_END_OF_TABLE
    };

RECT_FIXED ( rSelfCheckFailedMessageBoxRect, 5, 8, 75, 18 );

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables displayed when an application is trying to perform
// a low-level format on a hard drive.
//
// Note:
//      The strings should be OEM.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

void *apvHDFormat[] =
    {
    _T("VIRUS-LIKE ACTIVITY: The application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(" is attempting to format drive "),
    UI_DRIVE_IN_TROUBLE,
    _T("."),
    szDoubleNewLine,
    szWhatToDo,
    UI_END_OF_TABLE
    };

void *apvHDFormatExcludeFailed[] =
    {
    _T("Unable to exclude the application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(".  Formatting of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T(" will be allowed."),
    UI_END_OF_TABLE
    };

void *apvHDFormatStopNotify[] =
    {
    _T("The application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(" is attempting to format drive "),
    UI_DRIVE_IN_TROUBLE,
    _T(".  This action will not be allowed."),
    UI_END_OF_TABLE
    };

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables displayed when an application is trying to write
// to the boot record of a floppy disk drive.
//
// Note:
//      The strings should be OEM.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

void *apvFDBootWrite[] =
    {
    _T("VIRUS-LIKE ACTIVITY: The application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(" is attempting write to the boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T("."),
    szDoubleNewLine,
    szWhatToDo,
    UI_END_OF_TABLE
    };

void *apvFDBootWriteExcludeFailed[] =
    {
    _T("Unable to exclude the application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(".  Writing to the boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T(" will be allowed."),
    UI_END_OF_TABLE
    };

void *apvFDBootWriteStopNotify[] =
    {
    _T("The application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(" is attempting write to the boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T(".  This action will not be allowed."),
    UI_END_OF_TABLE
    };

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables displayed when an application is trying to write
// to the boot records of a hard disk drive.
//
// Note:
//      The strings should be OEM.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

void *apvHDBootWrite[] =
    {
    _T("VIRUS-LIKE ACTIVITY: The application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(" is attempting write to the boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T("."),
    szDoubleNewLine,
    szWhatToDo,
    UI_END_OF_TABLE
    };

void *apvHDBootWriteExcludeFailed[] =
    {
    _T("Unable to exclude the application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(".  Writing to the boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T(" will be allowed."),
    UI_END_OF_TABLE
    };

void *apvHDBootWriteStopNotify[] =
    {
    _T("The application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(" is attempting write to the boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T(".  This action will not be allowed."),
    UI_END_OF_TABLE
    };

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables displayed when an application is trying to write
// to the MBR of a hard disk drive.
//
// Note:
//      The strings should be OEM.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

void *apvHDMBRWrite[] =
    {
    _T("VIRUS-LIKE ACTIVITY: The application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(" is attempting write to the master boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T("."),
    szDoubleNewLine,
    szWhatToDo,
    UI_END_OF_TABLE
    };

void *apvHDMBRWriteExcludeFailed[] =
    {
    _T("Unable to exclude the application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(".  Writing to the master boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T(" will be allowed."),
    UI_END_OF_TABLE
    };

void *apvHDMBRWriteStopNotify[] =
    {
    _T("The application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(" is attempting write to the master boot record of drive "),
    UI_DRIVE_IN_TROUBLE,
    _T(".  This action will not be allowed."),
    UI_END_OF_TABLE
    };

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables displayed when an application is trying to write
// to a file.
//
// Note:
//      The strings should be OEM.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

void *apvWriteToFile[] =
    {
    _T("VIRUS-LIKE ACTIVITY: The application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(" is attempting to write to the file "),
    UI_FILE_IN_TROUBLE,
    _T("."),
    szDoubleNewLine,
    szWhatToDo,
    UI_END_OF_TABLE
    };

void *apvWriteToFileExcludeFailed[] =
    {
    _T("Unable to exclude the application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(".  Writing to the file "),
    UI_FILE_IN_TROUBLE,
    _T(" will be allowed."),
    UI_END_OF_TABLE
    };

void *apvWriteToFileStopNotify[] =
    {
    _T("The application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(" is attempting write to the file "),
    UI_FILE_IN_TROUBLE,
    _T(".  This action will not be allowed."),
    UI_END_OF_TABLE
    };

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables displayed when an application is trying to change
// the RO attribute of a file.
//
// Note:
//      The strings should be OEM.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

void *apvFileROAttrChange[] =
    {
    _T("VIRUS-LIKE ACTIVITY: The application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(" is attempting to clear the Read-Only attribute of the file "),
    UI_FILE_IN_TROUBLE,
    _T("."),
    szDoubleNewLine,
    szWhatToDo,
    UI_END_OF_TABLE
    };

void *apvFileROAttrChangeExcludeFailed[] =
    {
    _T("Unable to exclude the application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(".  Clearing the Read-Only attribute of the file "),
    UI_FILE_IN_TROUBLE,
    _T(" will be allowed."),
    UI_END_OF_TABLE
    };

void *apvFileROAttrChangeStopNotify[] =
    {
    _T("The application "),
    UI_FILE_CAUSING_TROUBLE,
    _T(" is attempting to clear the Read-Only attribute of the file "),
    UI_FILE_IN_TROUBLE,
    _T(".  This action will not be allowed."),
    UI_END_OF_TABLE
    };

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables used for building messages displayed during global
// initialization.  These strings and tables will be discarded after the
// initialization is complete.
//
// The file NAVAPNT.CPP in NAVAP contains translation tables which convert
// convert the platform independent MSG_ enumerators to message IDs specific
// to SYM_NTK defined in this file.  This file contains tables which convert
// the platform independent MSG_ enumerators to the SYM_VXD specific message
// strings defined in the same file.
//
// Please keep all messages synchronized!
//
// Note:
//      All strings should be OEM.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

#define DRIVER_INIT_DATA_SEGMENT
#include "drvseg.h"

TCHAR *apszInitErrors[] =
    {
    _T("SYMEVNT.386 is not loaded."),
    _T("Incompatible version of SYMEVNT.386 is loaded."),
    _T("Unable to initialize internal data."),
    _T("Unable to save persistent data."),
    _T("Unable to allocate thread data."),
    _T("Unable to initialize user mode communication."),
    _T("Unable to register user mode communication callbacks."),
    _T("Unable to allocate open file data."),
    _T("Unable to initialize file caching."),
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    _T("Error disabling DOS Auto-Protect agent."),
    _T("Unable to initialize V86 APIs for communicating with the DOS scanner."),
    _T("Unable to install critical error handler."),
    _T("Unable to install ^C / ^Break handler.")
    };

void *apvInitError[] =
    {
    _T("Error: NAV Auto-Protect is unable to start!"),
    szDoubleNewLine,
    UI_INIT_ERRORS,
    szDoubleSpace,
    _T("You may need to reinstall Norton AntiVirus to correct the problem."),
    szDoubleNewLine,
    _T("Press any key to continue..."),
    UI_END_OF_TABLE
    };

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings, tables and message box dimensions used for building messages
// displayed during controlled initialization.
//
// The file NAVAPNT.CPP in NAVAP contains translation tables which convert
// convert the platform independent MSG_ enumerators to message IDs specific
// to SYM_NTK defined in this file.  This file contains tables which convert
// the platform independent MSG_ enumerators to the SYM_VXD specific message
// strings defined in the same file.
//
// Please keep all messages synchronized!
//
// Note:
//      The strings should be OEM.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

TCHAR *apszControlledInitMessages[] =
    {
    _T("Unable to synchronize configuration loading."),
    _T("Unable to determine the location of the configuration files."),
    _T("Unable to determine the location of the virus definition files."),
    _T("Unable to determine the Auto-Protect startup name."),
    _T("Bad path to the configuration files specified."),
    _T("Unable to read the configuration files."),
    _T("The configuration files were not found. Using default configuration."),
    _T("Unable to initialize the exclusion database."),
    _T("Unable to initialize the virus scanning engine."),
    _T("Unable to initialize the virus scanning engine database files."),
    _T("The system was not protected during startup.  Please verify your configuration."),
    _T("Unable to load the Auto-Protect Windows agent. Logging and network alerting will be unavailable."),
    _T("Unable to create list of devices."),
    _T("Unable to install event hooks.")
    };

TCHAR szControlledInitMessageTitle[] = _T("NAV Auto-Protect");

void *apvControlledInitMessage[] =
    {
    UI_CONTROLLEDINIT_MESSAGES,
    UI_END_OF_TABLE
    };

RECT_FIXED ( rControlledInitMessageBoxRect, 5, 8, 75, 17 );

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings for notifying that the system is running out of memory
//
// Note:
//      The strings should be OEM.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

TCHAR szOutOfMemoryErrorTitle[] = _T("Warning!");
TCHAR szOutOfMemoryError[]      = _T("\nSystem memory is running very low.  Norton AntiVirus Auto-Protect may not be able to function properly.\n\n\n");

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Message boxes displayed when unable to shut the system down or when the
// system refused to shut down.
//
// Note:
//      The strings should be OEM.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

TCHAR szShutdownErrorMessageTitle[] = _T("NAV Auto-Protect Warning");

void *apvUnableToInitiateShutdown[] =
    {
    _T("Unable to initiate shutdown."),
    UI_END_OF_TABLE
    };

void *apvShutdownAborted[] =
    {
    _T("An application that you are running has refused to terminate.  Please close this application and then shut down the computer."),
    UI_END_OF_TABLE
    };

RECT_FIXED ( rShutdownErrorMessageBoxRect, 5, 8, 75, 17 );

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Message box used to display long custom alert.
//
// Note:
//      The strings should be OEM.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

TCHAR szLongCustomAlertMessageTitle[] = _T("NAV Auto-Protect Alert");

RECT_DYNAMIC_TB ( rLongCustomAlertMessageBoxRect, 5, 10, 75, 15 );

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// General strings and structures shared between different messages.
//
// Notes:
//      The strings should be OEM.
//      szVirusNameError should not be longer than 20 characters.
//
//---------------------------------------------------------------------------
TCHAR szWhatToDo[]               = _T("What would you like to do?");
TCHAR szVirusNameError[]         = _T("(no virus name)");
TCHAR szSystemWillBeShutdown[]   = _T("The system will be shut down.");

#if defined(SYM_VXD)

RECT_DYNAMIC_TB ( rVirusLikeActivityMessageBoxRect, 5, 10, 75, 15 );

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Internal strings.  These strings shouldn't be changed without consulting
// a developer.
//
//---------------------------------------------------------------------------
TCHAR szButtonLeftBracket[]       = _T("[");
TCHAR szButtonRightBracket[]      = _T("]");

TCHAR szNewLine[]                 = _T("\n");
TCHAR szDoubleNewLine[]           = _T("\n\n");
TCHAR szSpace[]                   = _T(" ");
TCHAR szDoubleSpace[]             = _T("  ");

#if defined(SYM_VXD)

TCHAR szCurrentUserKey[]          = _T("System\\CurrentControlSet\\Control");
TCHAR szCurrentUserKeyValueName[] = _T("Current User");

#endif // #if defined(SYM_VXD)
