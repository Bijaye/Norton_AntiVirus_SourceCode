// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APUI/VCS/uistrs.h_v   1.8   02 Mar 1998 12:03:44   DALLEE  $
//
// Description:
//      This file declares translatable strings.
//
// Contains:
//      -
//
// See Also:
//      uistrs.cpp
//
// Note:
//      UI module
//
//***************************************************************************
// $Log:   S:/APUI/VCS/uistrs.h_v  $
// 
//    Rev 1.8   02 Mar 1998 12:03:44   DALLEE
// Added success messages for Delete and Quarantine.
// 
//    Rev 1.7   20 Feb 1998 19:34:24   RStanev
// Declared uMessageBoxBottom.
//
//    Rev 1.6   19 Feb 1998 15:14:28   RStanev
// Removed unknown virus detection.  Added quarantine support.  Added
// successful action confirmation.
//
//    Rev 1.5   17 Feb 1998 20:53:58   RStanev
// Updated copyright.
//
//    Rev 1.4   28 May 1997 13:02:44   RStanev
// Added long custom alert and quarantine strings.
//
//    Rev 1.3   21 May 1997 04:34:38   RStanev
// Limited the export of strings and tables used when NAVAP's self test
// had failed to SYM_VXD only.
//
//    Rev 1.2   15 May 1997 02:35:50   RStanev
// Organized strings.
//
//    Rev 1.1   01 May 1997 14:20:14   RStanev
// Removed apvSelfCheckFailedOneHalfInMemory[].
//
//    Rev 1.0   09 Apr 1997 12:16:04   RStanev
// Initial revision.
//
//***************************************************************************

#ifndef _UISTRS_H_
#define _UISTRS_H_

//---------------------------------------------------------------------------
//
// Declare constants and macros for building text messages for
// NAVAPMessage().
//
//---------------------------------------------------------------------------
enum
    {
    _UI_END_OF_TABLE,
    _UI_FILE_IN_TROUBLE,
    _UI_DRIVE_IN_TROUBLE,
    _UI_FILE_CAUSING_TROUBLE,
    _UI_VIRUS_NAME,
    _UI_DRIVE_TO_SCAN,
    _UI_INIT_ERRORS,
    _UI_CONTROLLEDINIT_MESSAGES,
    _UI_PARAMETER_TABLE_SIZE
    };

#define UI_END_OF_TABLE             NULL, (void *)_UI_END_OF_TABLE
#define UI_FILE_IN_TROUBLE          NULL, (void *)_UI_FILE_IN_TROUBLE
#define UI_DRIVE_IN_TROUBLE         NULL, (void *)_UI_DRIVE_IN_TROUBLE
#define UI_FILE_CAUSING_TROUBLE     NULL, (void *)_UI_FILE_CAUSING_TROUBLE
#define UI_VIRUS_NAME               NULL, (void *)_UI_VIRUS_NAME
#define UI_DRIVE_TO_SCAN            NULL, (void *)_UI_DRIVE_TO_SCAN
#define UI_INIT_ERRORS              NULL, (void *)_UI_INIT_ERRORS
#define UI_CONTROLLEDINIT_MESSAGES  NULL, (void *)_UI_CONTROLLEDINIT_MESSAGES

//---------------------------------------------------------------------------
//
// Declare variables for supporting dynamic message box sizes.
//
//---------------------------------------------------------------------------
extern UINT uMessageBoxBottom;

//---------------------------------------------------------------------------
//
// Declare button strings table.
//
//---------------------------------------------------------------------------
extern TCHAR *apszButtons[];

//---------------------------------------------------------------------------
//
// Declare strings and tables displayed when infected file is found.
//
//---------------------------------------------------------------------------
extern TCHAR szAutoProtectTitle[];
extern void *apvInfectedFileFound[];
extern void *apvInfectedFileRepairSuccessful[];
extern void *apvInfectedFileRepairFailed[];
extern void *apvInfectedFileDeleteSuccessful[];
extern void *apvInfectedFileDeleteFailed[];
extern void *apvInfectedFileExcludeFailed[];
extern void *apvInfectedFileQuarantineSuccessful[];
extern void *apvInfectedFileQuarantineFailed[];
extern void *apvInfectedFileMoveFailed[];
extern void *apvInfectedFileRenameFailed[];
extern void *apvInfectedFileFoundStopNotify[];
extern void *apvInfectedFileFoundShutdownNotify[];
extern RECT rFileInfectedMessageBoxRect;

//---------------------------------------------------------------------------
//
// Scanning boot strings
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern void *apvScanningBootMessage[];
extern RECT rScanningBootMessageBoxRect;

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Known Boot Virus strings
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern void *apvInfectedBootFound[];
extern void *apvInfectedBootFoundRemoveDisk[];
extern void *apvInfectedBootRepairSuccessful[];
extern void *apvInfectedBootRepairFailed[];
extern void *apvInfectedBootRepairFailedRemoveDisk[];
extern void *apvInfectedBootFoundNotify[];
extern void *apvInfectedBootFoundNotifyRemoveDisk[];
extern void *apvInfectedBootFoundShutdownNotify[];
extern RECT rBootInfectedMessageBoxRect;

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Known Memory Virus strings
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern void *apvInfectedMemoryFoundStartupNotify[];
extern RECT rMemoryInfectedMessageBoxRect;

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Self test failure strings
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern void *apvSelfCheckFailedNoCRC[];
extern void *apvSelfCheckFailedCRCChanged[];
extern void *apvSelfCheckFailedIOError[];
extern void *apvSelfCheckFailedInternal[];
extern RECT rSelfCheckFailedMessageBoxRect;

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables displayed when an application is trying to perform
// a low-level format on a hard drive.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern void *apvHDFormat[];
extern void *apvHDFormatExcludeFailed[];
extern void *apvHDFormatStopNotify[];

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables displayed when an application is trying to write
// to the boot record of a floppy disk drive.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern void *apvFDBootWrite[];
extern void *apvFDBootWriteExcludeFailed[];
extern void *apvFDBootWriteStopNotify[];

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables displayed when an application is trying to write
// to the boot records of a hard disk drive.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern void *apvHDBootWrite[];
extern void *apvHDBootWriteExcludeFailed[];
extern void *apvHDBootWriteStopNotify[];

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables displayed when an application is trying to write
// to the master boot record of a hard disk drive.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern void *apvHDMBRWrite[];
extern void *apvHDMBRWriteExcludeFailed[];
extern void *apvHDMBRWriteStopNotify[];

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables displayed when an application is trying to write
// to a file.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern void *apvWriteToFile[];
extern void *apvWriteToFileExcludeFailed[];
extern void *apvWriteToFileStopNotify[];

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables displayed when an application is trying to change
// the RO attribute of a file.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern void *apvFileROAttrChange[];
extern void *apvFileROAttrChangeExcludeFailed[];
extern void *apvFileROAttrChangeStopNotify[];

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Declare strings and tables used for building messages displayed during
// global initialization.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern TCHAR *apszInitErrors[];

extern void *apvInitError[];

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Declare strings, tables, and message box dimensions used for building
// messages displayed during controlled initialization.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern TCHAR *apszControlledInitMessages[];

extern TCHAR szControlledInitMessageTitle[];

extern void *apvControlledInitMessage[];
extern RECT rControlledInitMessageBoxRect;

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings for notifying that the system is running out of memory
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern TCHAR szOutOfMemoryErrorTitle[];
extern TCHAR szOutOfMemoryError[];

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables displayed when unable to shut the system down or when
// the system refused to shut down.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern TCHAR szShutdownErrorMessageTitle[];
extern void *apvUnableToInitiateShutdown[];
extern void *apvShutdownAborted[];
extern RECT rShutdownErrorMessageBoxRect;

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Strings and tables used to display long custom alert.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern TCHAR szLongCustomAlertMessageTitle[];
extern RECT rLongCustomAlertMessageBoxRect;

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Declare general shared strings.
//
//---------------------------------------------------------------------------
extern TCHAR szWhatToDo[];
extern TCHAR szVirusNameError[];
extern TCHAR szSystemWillBeShutdown[];

#if defined(SYM_VXD)

extern RECT rVirusLikeActivityMessageBoxRect;

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Declare internal strings.
//
//---------------------------------------------------------------------------
extern TCHAR szButtonLeftBracket[];
extern TCHAR szButtonRightBracket[];

extern TCHAR szNewLine[];
extern TCHAR szDoubleNewLine[];
extern TCHAR szSpace[];
extern TCHAR szDoubleSpace[];

#if defined(SYM_VXD)

extern TCHAR szCurrentUserKey[];
extern TCHAR szCurrentUserKeyValueName[];

#endif // #if defined(SYM_VXD)

#endif // #ifndef _UISTRS_H_
