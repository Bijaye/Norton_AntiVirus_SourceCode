//===========================================================================
//
// $Header:   S:/ISNAVNT/VCS/iscommon.h_v   1.14   26 Oct 1998 13:51:14   mdunn  $
//
// Description:  Header file for 9x and NT common routines.
//
//===========================================================================
// $Log:   S:/ISNAVNT/VCS/iscommon.h_v  $
// 
//    Rev 1.14   26 Oct 1998 13:51:14   mdunn
// Added CompareFileVersions()
//
//    Rev 1.13   15 Oct 1998 21:42:14   mdunn
// Nuked CheckIfSettingsCanBeKept()
//
//    Rev 1.12   28 Sep 1998 18:05:06   mdunn
// Added logic so that install will:
//   overwrite 5.00 options
//   prompt before overwriting options from 5.01+
//   ask 'are you sure?' when installing over a newer version of NAV.
//
//    Rev 1.11   13 Aug 1998 12:04:26   mdunn
// Added ReadTrialLengthSwitch()
//
//    Rev 1.10   30 Jun 1998 11:13:26   mdunn
// Fixed 117289 - worked around IS buggyness which was always overwriting the
// settings files.
//
//    Rev 1.9   29 Jun 1998 18:26:44   mdunn
// Added DeleteOldSequenceNumbers().
//
//    Rev 1.8   10 Jun 1998 18:58:40   mdunn
// little tweaks
//
//    Rev 1.7   10 Jun 1998 16:43:16   mdunn
// Resurrected AddFolderInternetShortcut().
//
//    Rev 1.6   21 May 1998 22:02:52   mdunn
// Added SetDialogBitmap().
//
//    Rev 1.5   07 May 1998 19:17:18   mdunn
// #ifdef'ed out AddFolderInternetShortcut()
//
//    Rev 1.4   12 Mar 1998 15:56:18   mdunn
// Added background bitmap
//
//    Rev 1.3   09 Mar 1998 12:11:18   mdunn
// More 95 and NT merging
//
//    Rev 1.2   02 Mar 1998 17:28:10   mdunn
// Moved UpdateFilesFromArchive() out of the common RUL and H files - the merged version was choking and not extracting the virus defs correctly.
//
//    Rev 1.1   02 Mar 1998 15:27:02   mdunn
// Added SetLUProductName()
//
//    Rev 1.0   26 Feb 1998 13:05:42   mdunn
// Initial revision.
//
//    Rev 1.0   26 Feb 1998 12:56:02   mdunn
// Initial revision.
//===========================================================================

prototype AddFolderInternetShortcut(STRING, STRING, STRING, STRING, STRING, STRING, NUMBER );
prototype BuildVersionString ( BYREF STRING, NUMBER, NUMBER, NUMBER );
prototype ComposeGroupItemExceptionList(STRING, BYREF LIST);
prototype CompareVersionNumbers(STRING, STRING, NUMBER);
prototype CompareFileVersions ( STRING, STRING );
prototype DecrementLiveUpdateUsageCounts(STRING);
prototype DeleteOldSequenceNumbers();
prototype DialogShowADVOPanel1();
prototype DialogShowADVOPanel2();
prototype DialogShowSdLicense();
prototype DialogShowSdStartCopy();
prototype DialogShowSdWelcome();
prototype GetSkuNumber(BYREF STRING);
prototype IsPrevVerWithLU(STRING);
prototype NetscapeInstalled();
prototype ReleaseCustomDialogs();
prototype RemoveFolderContentsExceptList(STRING, BYREF LIST);
prototype SaveSettingsFilesBeforeISBlowsThemAway();
prototype RestoreBlownAwaySettingsFiles();
prototype SetDialogBitmap ( NUMBER );
prototype SetLUProductName ( STRING, BYREF STRING );
prototype SetupVirusDefs();

#ifdef NAV_TRIAL_VERSION
prototype ReadTrialLengthSwitch ( BYREF STRING );
#endif

prototype CheckValidOS ( NUMBER );
prototype DialogShowKeepSettings();
prototype OnlineRegistration();
prototype SetupScreen();
prototype ShowPostCopyDialogs();
prototype UpdateFilesFromArchive(STRING, STRING, STRING);
prototype UpdateLiveUpdate();
prototype UpdateNavOpts();
prototype UpdateVirusDefs();

#define NAV_LU_CMDLINES_KEY     "SOFTWARE\\Symantec\\Norton AntiVirus\\LiveUpdate\\CmdLines"
#define SYM_LU_SEQUENCES_KEY    "SOFTWARE\\Symantec\\LiveUpdate\\Sequences"
#define NAV_LU_PRODUCT_NAME     "ProductName"

#define IDB_BACKGROUND_BITMAP   20000





