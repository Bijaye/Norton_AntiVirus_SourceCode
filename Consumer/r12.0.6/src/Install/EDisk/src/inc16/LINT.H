/* $Header:   V:/INCLUDE/VCS/LINT.H_V   1.302   15 Dec 1993 11:30:12   BARRY  $ */

/************************************************************************
 * LINT.H								*
 *                                                                      *
 * Description:                                                         *
 *  This file contains function prototypes for most public library 	*
 *  routines. 								*
 *									*
 *  All LOCAL library functions should have their prototypes in a	*
 *  separate module-specific header file used only by the library	*
 *  source files.							*
 ************************************************************************
 *
 * $Log:   V:/INCLUDE/VCS/LINT.H_V  $
 * 
 *    Rev 1.302   15 Dec 1993 11:30:12   BARRY
 * Merge changes from NLIB 4.0
 * 
 *    Rev 1.302   16 Nov 1993 12:12:40   BARRY
 * Added FileCmpDateTime()
 * 
 *    Rev 1.301   05 Nov 1993 17:45:52   BARRY
 * Add StringNAppend (=StringNCat)
 * 
 *    Rev 1.300   05 Nov 1993 13:49:52   BARRY
 * Added STREMPTY
 * 
 *    Rev 1.299   05 Nov 1993 13:24:34   BARRY
 * Map StringNLocalize (new fn) to StringNCopyFar
 * 
 *    Rev 1.298   21 Oct 1993 00:18:10   BARRY
 * Added prototype for FindWindowsDir()
 * 
 *    Rev 1.297   17 Mar 1993 16:12:30   BASIL
 * Added DOSGetStartupDrive()
 * 
 *    Rev 1.296   15 Mar 1993 14:54:16   BASIL
 * Added prototype for DOSGetHostDrive() and DOSGetRealBootDrive()
 * 
 *    Rev 1.295   09 Mar 1993 19:21:04   REESE
 * Added CheckForEnoughMemory().
 * 
 *    Rev 1.294   09 Feb 1993 15:15:00   TORSTEN
 * 
 *    Rev 1.293   09 Feb 1993 15:06:42   TORSTEN
 * Changed the NovellGetConnectionInfo() connection number to a Word.
 * 
 *    Rev 1.292   09 Feb 1993 10:36:28   DAVID
 * Cleaned up prototype for StdDlgChangeDir2().
 * 
 *    Rev 1.291   01 Feb 1993 21:28:54   JOHN
 * Ported from Quake:
 *    DiskIOCTL()
 *    DiskIOCTLError()
 *    DiskGetSuperStorSwapDrive()
 *    DiskGetSuperStorHostDrive()
 * 
 *    Rev 1.290   20 Jan 1993 17:13:46   CRAIG
 * Added VideoPageCalcAddress() and VideoPageGetSize().
 *
 *    Rev 1.289   15 Jan 1993 12:56:38   BASIL
 * Added VideoIsAnsiInstalled() to detect the presence of ansi.sys.
 *
 *    Rev 1.288   14 Jan 1993 21:09:32   DAVID
 * Added FATGetPrev(), FATBackGetPrev()
 *
 *    Rev 1.287   11 Dec 1992 12:28:08   CRAIG
 * Added VideoPageCopy().
 *
 *    Rev 1.286   09 Dec 1992 16:57:02   BASIL
 * Added FileCopyWithProgress()
 *
 *    Rev 1.285   04 Dec 1992 15:26:52   JOHN
 * Removed bHeadHack parameter from BIOSInstallInt13Handler()
 *
 *    Rev 1.284   04 Dec 1992 10:02:48   JOEP
 * MouseInit and MouseInitSW now return an INT before they were VOID
 *                 -1	Mouse installed
 *                 0	No mouse Installed
 *
 *    Rev 1.283   30 Nov 1992 17:16:30   CRAIG
 * Added StringGetEnd().
 *
 *    Rev 1.282   19 Nov 1992 11:11:28   CRAIG
 * Added VideoGetPageOffset prototype.
 *
 *    Rev 1.281   11 Nov 1992 17:47:22   JOHN
 * Added DiskIsSpeedStor(). Changed BIOSInstallInt13Handler().
 *
 *    Rev 1.280   09 Nov 1992 13:37:32   JOHN
 * Added DiskIsDoubleSpace() and DiskIsCompressed()
 *
 *    Rev 1.279   31 Oct 1992 01:09:00   JOHN
 * Fixed prototype of NameToPacked()
 *
 *    Rev 1.278   30 Oct 1992 07:49:04   MARKK
 * Window and VM Manager changes
 *
 *    Rev 1.277   19 Oct 1992 10:37:42   DAVID
 * Added a case sensitive version of DOSGetEnvironment().
 *
 *    Rev 1.276   16 Oct 1992 18:59:18   JOHN
 * [John] Changed parameters to StdDlgGetUserName().
 * [John] Added FileScanForSignature().
 * [????] Added FATReadSpecify().
 *
 *    Rev 1.275   13 Oct 1992 08:42:52   CRAIG
 * Added parameter to StdDlgGetSearchKey().
 *
 *    Rev 1.274   11 Oct 1992 14:37:26   BASIL
 * Added ImageSave()
 *
 *    Rev 1.273   29 Sep 1992 16:11:20   JOHN
 * Protected MakeDiskBootable() prototype with #ifdef.
 *
 *    Rev 1.272   24 Sep 1992 19:50:02   BASIL
 * Added FATClear()
 *
 *    Rev 1.271   23 Sep 1992 19:25:26   BASIL
 * added MakeDiskBootable()
 *
 *    Rev 1.270   23 Sep 1992 11:00:52   CRAIG
 * Added StdDlgHexCalculator()
 *
 *    Rev 1.269   23 Sep 1992 10:58:26   CRAIG
 * Added _ConvertStringToLong()
 *
 *    Rev 1.268   14 Sep 1992 12:56:22   CRAIG
 * Added VMMAvailWithPurge().
 *
 *    Rev 1.267   14 Sep 1992 12:00:50   CRAIG
 * Corrected VMMAvail() prototype (wrong sequence!)
 *
 *    Rev 1.266   10 Sep 1992 21:59:44   JOHN
 * Changed RawDir...() functions to use VMM
 *
 *    Rev 1.265   10 Sep 1992 17:09:44   JOHN
 * Removed obsolete MM Heap stuff
 *
 *    Rev 1.264   09 Sep 1992 19:06:04   JOHN
 * Removed special VMM versions of volume label functions.
 *
 *    Rev 1.263   09 Sep 1992 17:28:28   JOHN
 * Removed the VMMDiskType() group of functions.
 * The normal DiskType() now uses VMM for its buffer.
 *
 *    Rev 1.262   08 Sep 1992 18:44:20   SKURTZ
 * Added BiosKBD*() prototypes
 *
 *    Rev 1.261   08 Sep 1992 17:44:22   SKURTZ
 * Add mini vid*() routines that can be called without of library overhead.
 *
 *    Rev 1.260   01 Sep 1992 19:08:14   JOHN
 * Added _DiskAllocDMABuffer() and _DiskFreeDMABuffer()
 *
 *    Rev 1.259   27 Aug 1992 16:46:04   JOHN
 * Added VMMAllocFixed() and VMMFreeFixed()
 *
 *    Rev 1.258   21 Aug 1992 11:20:58   CRAIG
 * Added DOSWriteStringArray().
 *
 *    Rev 1.257   21 Aug 1992 11:19:10   HENRI
 *
 *    Rev 1.256   13 Aug 1992 16:30:34   JOHN
 * Updated FAT functions
 *
 *    Rev 1.255   13 Aug 1992 10:46:18   TORSTEN
 * Added the prototypes for the joystick routines.
 *
 *    Rev 1.254   13 Aug 1992 10:44:00   HENRI
 * A couple of more changes for Quake compatibility.
 *
 *    Rev 1.253   12 Aug 1992 23:10:26   JOHN
 * Fixed incorrect #ifdef in Date...() functions
 *
 *    Rev 1.252   12 Aug 1992 22:59:34   JOHN
 * Put NameToPacked() and NameToUnpackedFar() back in
 *
 *    Rev 1.251   12 Aug 1992 18:05:46   HENRI
 * Only define FileRead and FileWrite if they are not
 * already defined (Quake).
 *
 *    Rev 1.250   12 Aug 1992 12:50:06   HENRI
 * Removed duplicate definitions of NameToPacked and NameToUnpacked.
 *
 *    Rev 1.249   11 Aug 1992 13:38:14   HENRI
 * Fixed some #define problems that were
 * incurred in last revision.
 *
 *    Rev 1.248   11 Aug 1992 13:29:00   HENRI
 * Pulled out routines defined in Quake library
 * and put them between "#ifndef <identifier> / #endif"
 * statements.  The following files from the
 * Quake library were checked for duplicates:
 * NDSK.H,  NFAT.H,  NFOP.H,  NNAM.H,  NPRM.H
 *
 *    Rev 1.247   10 Aug 1992 09:58:14   CRAIG
 * Added StringCopyDigitsOnly().
 *
 *    Rev 1.246   10 Aug 1992 09:55:30   JOHN
 * Various changes for Scorpio.
 *
 *    Rev 1.245   03 Aug 1992 15:37:32   JOHN
 * Changed DiskGetDMABuffer() and a few others things too
 *
 *
 * ... Revisions prior to Scorpio have been deleted to save space ...
 *
 *
 *    Rev 1.0   29 Aug 1990 11:24:52   DAVID
 * Initial revision.
 *
 ************************************************************************/

#ifndef INCL_LINT

#define INCL_LINT

#ifndef INCL_TYPES
#include <Types.h>
#endif

#ifndef INCL_WINGLUE
#include <WinGlue.h>
#endif

#ifndef INCL_PULLDOWN
#include <Pulldown.h>
#endif

/*----------------------------------------------------------------------*
 * Bar Module								*
 *----------------------------------------------------------------------*/

#ifdef INCL_BAR

void	BarIconPosition (BarRecord *bar, Word *row, Word *col);
int	BarPointLocation (BarRecord *bar, Word row, Word col);
int	BarHandle( UserControlRec *ucr, Word *c );
void	BarInternalDraw( BarRecord *bar );
void	BarDraw( UserControlRec *ucr, Boolean firstTime, Boolean active );
void	BarSize( UserControlRec *ucr, Word *height, Word *width );
void	BarMoveDown( BarRecord *bar, Byte increments );
void	BarMoveUp( BarRecord *bar, Byte increments );
void	BarSet( BarRecord *bar, Byte increments );

#endif

/*----------------------------------------------------------------------*
 * Sound Module								*
 *----------------------------------------------------------------------*/

void	Beep (void);
void	PASCAL	Blip (void);
void	PASCAL  Sound (Word frequency, Word duration);
void	PASCAL	BeepDone (void);
void	PASCAL	BeepError (void);
void	PASCAL	NAVSound (void);

/*----------------------------------------------------------------------*
 * Alternate Buffered File Module  					*
 *----------------------------------------------------------------------*/

#ifdef INCL_DEFNS

FILE_INFO NEAR *BFOpen (Byte *fileName, Byte NEAR *buffer, Word bufSize, Word mode);
void		BFClose (FILE_INFO NEAR *fptr);
void		BFGetDate (FILE_INFO NEAR *fptr, Word *date, Word *time);
void		BFSetDate (FILE_INFO NEAR *fptr, Word date, Word time);
int		BFGetC (FILE_INFO NEAR *fptr);
int		BFRGetC (FILE_INFO NEAR *fptr);
void		BFUngetC (FILE_INFO NEAR *fptr);
int		BFGetLine (FILE_INFO NEAR *fptr, Byte *string, Word maxChars);
int		BFSetPtr (FILE_INFO NEAR *fptr, Ulong newPtr);
Ulong		BFGetPtr (FILE_INFO NEAR *fptr);
Ulong		BFSize (FILE_INFO NEAR *fptr);

#endif

/*----------------------------------------------------------------------*
 * BIOS Module								*
 *----------------------------------------------------------------------*/

void    BIOSRestorePrintChar(void);
void    BIOSInstallPrintChar(void);

Byte 	CMOSRead( Byte address );
void 	CMOSWrite( Byte address, Byte data );

#ifdef INCL_DISK
int     BIOSInstallInt13Handler(Int13Record NEAR *int13PackagePtr);
void    BIOSUninstallInt13Handler(void);
#endif

/*----------------------------------------------------------------------*
 * Buffer module                    					*
 *----------------------------------------------------------------------*/

void	PASCAL	BufferToUpper(Word buff_seg, Word buff_off, Word buff_size);

/*----------------------------------------------------------------------*
 * Calibrate Recovery module						*
 *----------------------------------------------------------------------*/

void 	CheckRecoveryInfo(void);

/*----------------------------------------------------------------------*
 * Character Case Module						*
 *----------------------------------------------------------------------*/

Byte	PASCAL CharToUpper (Word c);
Byte	PASCAL CharToLower (Word c);
Boolean	PASCAL CharIsUpper (Byte c);
Boolean PASCAL CharIsLower (Byte c);
Boolean	PASCAL CharIsAlpha (Byte ch);
Boolean	PASCAL CharIsNumeric (Byte ch);
Boolean PASCAL CharIsAlphaNumeric (Byte ch);
Boolean PASCAL CharIsHexNumeric (Byte ch);
Boolean PASCAL CharIsWhite(Byte ch);
Byte	PASCAL CharToSortValue(Word ch);

/*----------------------------------------------------------------------*
 * Screen Color Manipulation Module 					*
 *----------------------------------------------------------------------*/

#ifdef INCL_DRAW

void		ColorSaveAll (void);
void		ColorRestoreAll (void);
void 	PASCAL	ColorSetSet (COLORS *colors);
void		ColorRestoreSet (void);
void	PASCAL 	ColorChangeSet (COLORS *colors);
Byte    PASCAL  ColorCreateShadow(Byte color);
Byte	PASCAL 	ColorCreateCompatible (Byte dialogColor, Byte shadowColor, Byte sourceColor);
Byte	PASCAL	ColorGetDisabled (Boolean *bMonoColor);

extern Byte near colorSet;
#define ColorGetColorSet() 	(colorSet)

#if 0
//	These are macros in DRAW.H:
void	ColorSetAttr (Word attrName);	     // Use NORMAL, BOLD, etc.
int	ColorGetAttr (void);
int	ColorGetAttrValue (Word attrName);
void	ColorSetClearAttr (Word attrName);
Byte	ColorGetOldAttr (void);
Byte	ColorGetCurrentSet(void);
#endif

#endif

/*----------------------------------------------------------------------*
 * COMPAQ Computer Module      						*
 *----------------------------------------------------------------------*/

Word 	CDECL	CompaqGetSystemSpeed( Word *speed );
void	CDECL	CompaqSetSystemSpeed ( Word speedType, Word speedValue);
Word 	CDECL 	CompaqMemoryCacheStatus(void);

/*----------------------------------------------------------------------*
 * Conversion Module							*
 *----------------------------------------------------------------------*/

Ulong   PASCAL  SwapUlong (Ulong num);
Byte *	PASCAL	ConvertSizeToShortString(Ulong size, Byte *string);
Byte *	PASCAL	ConvertSizeToFullString(Ulong size, Byte *string);
Word 	PASCAL  ConvertWordToBCD( Word value);
Byte 	PASCAL	ConvertBCDToByte( Byte number );
Byte * 	PASCAL 	_ConvertLongToString (Ulong num, Byte *str, Word rdx, Byte thou_char);
Byte *	PASCAL 	_ConvertWordToString (Word num, Byte *str, Word rdx, Byte thou_char);
Ulong   PASCAL 	_ConvertStringToLong( Byte *szString, Word wRadix );

Byte * 	PASCAL 	ConvertEncodedDateToString ( Byte *dateString, Word date);
void    PASCAL  ConvertEncodedTimeToString (Byte *TimeString, Word Time);
Byte * 	PASCAL 	ConvertDateToString( Byte *dateString, Word month, Word day, Word year,
			     Boolean shortFormat );
Byte * 	PASCAL 	_ConvertDateToString( Byte *dateString, Word month, Word day, Word year,
			     Boolean shortFormat, Boolean bWeekday);
Byte * 	PASCAL 	ConvertTimeToString( Byte *string, Word hour, Word minute, Boolean padded );
void	PASCAL	ConvertSecondsToDateTime( Ulong ulSeconds, Word *pwYear, Word *pwMonth,
			Word *pwDate, Word *pwHour, Word *pwMinute, Word *pwSecond );
Boolean PASCAL  ConvertStringToDate(Byte * szDateString, Word * pwMonth, Word * pwDay, Word * pwYear);
Boolean PASCAL  ConvertStringToTime(Byte * szTimeString, Word * pwHours, Word * pwMinutes, Word * pwSeconds);

Byte    PASCAL 	ConvertAscToEbc (Byte value);
Byte    PASCAL 	ConvertEbcToAsc (Byte value);

Word    PASCAL 	ConvertTicksToHMS(Ulong ticks, Word *hour, Word *minute, Word *seconds);
Byte * 	PASCAL 	ConvertElapsedTimeToString( Byte *string, Word hour, Word minute,
			             Word second, Boolean leadingZero );
Byte * PASCAL 	ConvertElapsedTimeToLongString ( Byte *string, int hour, int minute, int second);
Byte *  PASCAL  ConvertLongToAbbreviation( Ulong number, Byte *string );
Byte 	PASCAL 	ConvertHexDigitToDecimal (Byte hex);
Word  	PASCAL	ConvertHexStringToWord (Byte *string);
Ulong  	PASCAL	ConvertHexStringToLong (Byte *string);

Word PASCAL ConvertStringToEncodedDate (Byte *string, Byte dateChar);
Word PASCAL ConvertStringToEncodedTime(Byte *timeSting);

void	PASCAL	ConvertFlagsToString( Byte *szBuffer, Byte **aszStrings, Ulong ulFlags );

#ifndef _INCL_NPRM
Word	PASCAL 	ConvertTimeToEncoded (Word hour, Word minute, Word seconds);
void    PASCAL 	ConvertEncodedTimeToNormal (Word time, int *hour, int *minute, int *seconds);
Word    PASCAL 	ConvertDateToEncoded (Word month, Word day, Word year);
void    PASCAL 	ConvertEncodedDateToNormal (Word date, int *month, int *day, int *year);
Byte *	PASCAL	ConvertLongToString (Ulong num, Byte *string);
Byte * 	PASCAL 	ConvertWordToString (Word num, Byte *string);

Ulong   PASCAL 	ConvertStringToLong (Byte *string);
Word    PASCAL 	ConvertStringToWord (Byte *string);
#endif

#ifdef INCL_DISK
#ifndef INCL_NDSK
Ulong	PASCAL	ConvertClusterToSector (DiskRecord *diskPtr, Word cln);
Word	PASCAL	ConvertSectorToCluster (DiskRecord *diskPtr, Ulong sector);
Word  	PASCAL 	ConvertSizeToCluster (DiskRecord *diskPtr, Ulong size);
Ulong   PASCAL  ConvertPhySectorToLogSector( AbsDiskRecord *absDisk, DiskRecord *disk,
                         Byte head, Word track, Byte sector );
Ulong   PASCAL  ConvertPhySectorToLong( AbsDiskRecord *absDisk,
                         Byte head, Word track, Byte sector );
void    PASCAL  ConvertLongToPhySector( AbsDiskRecord *absDisk, Ulong absSector,
                         Byte *head, Word *track, Byte *sector );
#endif
#endif

/*----------------------------------------------------------------------*
 * Hardware Cursor Module						*
 *----------------------------------------------------------------------*/

void	CursorHome (void);
void	CursorSetRowCol (Word row, Word col);
void	CursorGetRowCol (Word *row, Word *col);
void	CursorSetShape (Word shape);
Word	CursorGetShape (void);
void	CursorOff (void);
void	CursorOn (void);
void	CursorPop (void);
void	CursorPush (void);
void	CursorBlock (void);
void	CursorUnderscore (void);
Word	CursorGetVisible(void);

void	PASCAL PositionCursor(Word wRow, Word wCol);

/*----------------------------------------------------------------------*
 * Date/Time Module							*
 *----------------------------------------------------------------------*/

Byte *	PASCAL	DateGetMonthName (Word month);
Byte *	PASCAL	DateGetWeekDayName ( Word dayNumber );
Byte *	PASCAL	DateGetCurrentDateString( Byte *string );
Byte *  PASCAL  DateGetCurrentTimeString( Byte *string , Boolean padded);
Word    PASCAL	DateGetDayOfWeek ( Word month, Word day, Word year );
void	PASCAL	DateGetCurrentDateTime(Word *date, Word *time);
Boolean	PASCAL	DateIsDOSLegal (Word year, Word month, Word day);
Boolean	PASCAL	DateIsDOSLegalTime (Word hour, Word minute, Word second, Boolean military);

#ifndef _INCL_NPRM
Byte *	PASCAL	DateGetCurrentDateTimeString( Byte *string );
#endif

// ----------------------------------------------------------------------
// International Date/Time functions from Othello
// ----------------------------------------------------------------------

#ifdef INCL_DIALOG
int PASCAL 	NGetDlgIntlDate (DIALOGREC drDialog, void *pItem,
			LPWORD lpwMonth, LPWORD lpwDay, LPWORD lpwYear);
void PASCAL 	NSetDlgIntlDate (DIALOGREC drDialog, void *pItem,
			WORD wMonth, WORD wDay, WORD wYear,
			BOOL bExcludeCentury);
int PASCAL	NGetDlgIntlTime (DIALOGREC drDialog, void *pItem,
			LPWORD lpwHours, LPWORD lpwMinutes,
			LPWORD lpwSeconds);
void PASCAL 	NSetDlgIntlTime (DIALOGREC drDialog, void *pItem,
			WORD wHours, WORD wMinutes, WORD wSeconds,
			BOOL bDispSeconds, BOOL bDispAMPM);
#endif

void PASCAL 	NSetIntlLongDate (Byte *pBuffer, WORD wMonth, WORD wDay,
			WORD wYear, BOOL bWeekday);
void PASCAL 	NSetIntlDate (Byte *pBuffer, WORD wMonth, WORD wDay,
			WORD wYear);
void PASCAL	NSetIntlTime (Byte *pBuffer, WORD wHours, WORD wMinutes,
			WORD wSeconds, Boolean bDispSeconds,
			Boolean bDispAMPM);

/*----------------------------------------------------------------------*
 * Desqview Module						        *
 *----------------------------------------------------------------------*/

Boolean PASCAL  DesqviewInstall(void);

/*----------------------------------------------------------------------*
 * Dialog Box Module							*
 *----------------------------------------------------------------------*/

#ifdef INCL_DIALOG

Word	PASCAL	DialogArgument( Byte *arg, int *type, int *width,
				int *arg_num, ArgumentFlagsRec *flagsRec,
				Byte **group );
Word		DialogBoxInternal( DIALOG *dialog, COLORS *colors,
			           DialogFilter eventFilter, char *args[] );
void 	PASCAL 	DialogDrawBox( int row1, int col1, int row2, int col2,
			       DIALOG_RECORD *dr );

COLORS* CDECL  DialogGetColor(void);
Word	CDECL	DialogBox (DIALOG *dialog, ...);
Word	CDECL	DialogBox2 (DIALOG *dialog, ...);
Word	CDECL   DialogHelpBox (DIALOG *dialog, ...);
Word	CDECL	DialogAlertBox (DIALOG *dialog, ...);
Word	CDECL	DialogColorBox (DIALOG *dialog, COLORS *colors, ...);
Word	CDECL	DialogBoxHooked (DIALOG *dialog, DialogFilter eventFilter, ...);
Word 	PASCAL	DialogCheckHelp(MenuBarRec *menuBar, Word event);

Boolean         DialogIsOpen (void);

DIALOG_RECORD  *DialogOpen (DIALOG *dialog, ...);
DIALOG_RECORD  *DialogOpen2 (DIALOG *dialog, ...);
DIALOG_RECORD  *DialogOpenC (DIALOG *dialog, COLORS *colors, ...);
Word	PASCAL	DialogClose (DIALOG_RECORD *dr, Word eventResult);

Word	PASCAL	DialogLoop (DIALOG_RECORD *dr, DialogFilter eventFilter );
Word	PASCAL	DialogTextListEventLoop (DIALOG_RECORD *dr, DialogFilter eventFilter);
Word	PASCAL	DialogTextListEvent (ListRec *list, Word c);
Word	PASCAL 	DialogGetEvent (void);
Word	PASCAL	DialogProcessEvent (DIALOG_RECORD *dr, Word c);

void	PASCAL	DialogOutput (OutputFieldRec *outField, void *value);
void	CDECL	DialogPrintOutput (OutputFieldRec *outField, Byte *s, ...);

Word 	PASCAL	DialogGetUniqueButtonNumber (DIALOG_RECORD *dr);
WORD	PASCAL  DialogGetButtonNumber(ButtonsRec *lpButton);
void	PASCAL	DialogUpdateTitle (DIALOG_RECORD *dr, Byte *s, Boolean active);
void	PASCAL	DialogChangeButtons (DIALOG_RECORD *dr, ButtonsRec *b, int align);

void	PASCAL	DialogOpenDummy (void);
void	PASCAL	DialogCloseDummy (void);
void	PASCAL	DialogOpenMenuHelpDummy (void);
void	PASCAL	DialogCloseMenuHelpDummy (void);

Boolean PASCAL 	DialogMouseInCloseBox (DIALOG_RECORD *dr);

void	PASCAL DialogSetItemInt (DIALOG_RECORD *drDialog, void *pItem,
				Word wValue, Boolean bSigned);
void	PASCAL DialogSetItemLong (DIALOG_RECORD *drDialog, void *pItem,
				Ulong dwValue, Boolean bSigned);
void	PASCAL DialogSetItemText (DIALOG_RECORD *drDialog, void *pItem,
				Byte *pString);
Word 	PASCAL DialogGetItemInt (DIALOG_RECORD *drDialog, void *pItem,
			   	Boolean *pbTranslated, Boolean bSigned);
Ulong   PASCAL DialogGetItemLong (DIALOG_RECORD *drDialog, void *pItem,
			   	Boolean *pbTranslated, Boolean bSigned);
int 	PASCAL DialogGetItemText (DIALOG_RECORD *drDialog, void *pItem,
			   	Byte *pString, int nMaxCount);
void 	PASCAL DialogUpdateControl (DIALOG_RECORD *drDialog,
				void *pDialogItem, Boolean bActive);
void 	PASCAL DialogCheckButton (DIALOG_RECORD *drDialog,
				CheckBoxRec *pItem, Word wCheck);
Word 	PASCAL DialogIsButtonChecked (DIALOG_RECORD *drDialog, CheckBoxRec *pItem);
void 	PASCAL DialogCheckRadioButton (DIALOG_RECORD *drDialog, RadioButtonsRec *pItem,
				int nIDCheckButton);
void	PASCAL DialogExpand (DIALOGREC drDialog, Word wPivotRow, Boolean bExpand);
Word	PASCAL DialogGetItemNumber (DIALOGREC drDialog, void *pItem);

DIALOG_ITEM *PASCAL DialogGetNthItem (DIALOG_RECORD *drDialog, int nItem);
DIALOG_ITEM *PASCAL DialogGetItem (DIALOG_RECORD *drDialog, void *pItem);
DIALOG_ITEM *PASCAL DialogGetFocus (DIALOG_RECORD *drDialog);
void 	PASCAL DialogSetFocus (DIALOG_RECORD *drDialog, void *pItem);
void PASCAL DialogKBMoveDlg (DIALOG_RECORD *dr, Word c);

/*----------------------------------------------------------------------*
 * Standard Dialogs							*
 *----------------------------------------------------------------------*/

int StdEventLoop(
    MenuBarRec    *menuBar,
    DIALOG_RECORD *dr,
    int (*ButtonFunc)(DIALOG_RECORD *dr),
    int (*MenuFunc)(int menu, int item, int id),
    void (*PreFunc)(DIALOG_RECORD *dr, int *event),
    void (*PostFunc)(DIALOG_RECORD *dr));

Word	PASCAL 	StdDlgViewAs (Word *viewerNum);

Word	PASCAL 	StdDlgSaveChanges (Word wValue);

Boolean PASCAL StdDlgNetUsers (
  Byte 		*byServer,
  Boolean 	*pbSelected,
  Handle 	*hUserMem,
  Word          *wMaxUsers,
  void          (*pfSelect)( Handle hUserMem, Word wMaxUsers, Boolean *pbSelected),
  Byte		*pszTitle);

Boolean PASCAL StdDlgConnectedNetUsers (
  Byte 		*byServer,
  Boolean 	*pbSelected,
  Handle 	*hUserMem,
  Word          *wMaxUsers,
  void          (*pfSelect)( Handle hUserMem, Word wMaxUsers, Boolean *pbSelected),
  Byte		*pszTitle);

Boolean PASCAL StdDlgConnectedNetUser (
  Byte 		*pbyServer,
  Handle 	*hUserMem,
  Word          *wMaxUsers,
  Byte		*pszTitle,
  Ulong         ulSelectID);

Boolean PASCAL StdDlgNetGroups (
  Byte 		*pbyServer,
  Boolean 	*pbSelected,
  Handle 	*hGroupMem,
  Word          *wMaxGroups,
  void          (*pfSelect) (Handle hGroupMem, Word wMaxGroups, Boolean *pbSelected),
  Byte		*pszTitle);

Boolean PASCAL StdDlgConnectedNBUser (
  Handle        *hUserMem,
  Word          *wMaxUsers,
  Byte          *pszTitle);

Boolean PASCAL StdDlgConnectedNBUsers (
  Boolean 	*pbSelected,
  Handle 	*hUserMem,
  Word          *wMaxUsers,
  void          (*pfSelect)( Handle hUserMem, Word wMaxUsers, Boolean *pbSelected),
  Byte		*pszTitle);

WORD PASCAL StdDlgSelectLocDriveList(
    BYTE        *pszTitle,
    BYTE        *pszSelectedList);

#ifdef INCL_PRINTMAN
void PASCAL StdDlgDefaultPrinterConfig(PRINT_SETUP_REC *pConfig);
int PASCAL StdDlgPrinterConfig(PRINT_SETUP_REC *pConfig);
#endif

Boolean PASCAL	StdDlgCheckPrinter(Byte *outputName);

Word	PASCAL	StdDlgGetUserName(Byte *product, Byte *user, Byte *company,
				  Byte *serialNumber, Boolean bCanEscape);

Boolean PASCAL	StdDlgGuiltScreen(Byte *programName, Byte *programVersion);

#ifdef INCL_DISK
#ifndef INCL_NDSK
Boolean	PASCAL	StdDlgPromptForFloppyFormat(DiskRec *diskPtr, Byte driveType, int *BPBIndexPtr);
#endif
#endif

FileHandle PASCAL StdDlgFileCreate( Byte *returnFileName, Byte *message );
FileHandle PASCAL StdDlgOpenNewFile (Byte *fileName, Word flags );

Ulong   CDECL	StdDlgGetDateTime(Byte *headOfDialog[], ...);
Word    CDECL	StdDlgGetAttributes(char *stringsHead[],...);
Byte 	PASCAL	StdDlgAttrDecodeBits( Byte bitMap );
Ulong   CDECL	StdDlgSelectFileSize(Word numberOfFiles);

Boolean CDECL StdDlgSelectFile(
    char        *fileName,
    char        *initFileSpec,
    char far    *buffer,
    int         numberOfFiles,
    int         flags,
    char        *title);

void 		StdDlgEstimateTime(OutputFieldRec *estimatedTime,
			Word current, Word total);
DIALOG_RECORD	*StdDlgInformUser (Byte *strs[]);
Ulong  PASCAL	StdDlgGetCurrentDriveEntry( Byte dl, Byte *driveLetters );
int     CDECL	StdGetDriveList (Byte driveLetters[], Byte filter,
			      Boolean (*driveFilterProc)(Byte dl) );
Byte	CDECL	StdDlgSelectDrive( Byte *dl, Byte filter, Byte *title, Byte *message, Boolean (*driveFilterProc)(Byte dl) );
Byte	PASCAL	StdDlgJustGetDrive( Byte *dl, Byte *driveLetters, Word numDrives, Byte *title, Byte *message );
Byte    PASCAL	StdDlgSelectPhysicalDrive( Byte *driveNumber, Byte *title );
Byte	PASCAL	StdDlgSelectDriveAndType(Byte drive, Byte *title );
Word 	PASCAL	StdDlgTreeCurrentEntry( Byte *currentPath );
int 	PASCAL	StdDlgNCDTree (Byte *currentPath, Boolean cancelOnSameDir );
void 	CDECL	StdDlgError( Byte *strings[], ...);
void 	CDECL	StdDlgMessage( Byte *strings[], ... );
int             StdDldFloppyType(int num, Byte names[], Byte types[]);

Boolean PASCAL	StdDlgCheckDisk( Byte dl, Byte *title, Word *errorCode );
Boolean PASCAL	StdDlgChangeDir ( Byte *path );
Boolean PASCAL	StdDlgChangeDir2 ( Byte *path, Byte driveFilter,
                                Boolean (*driveFilterProc)(Byte dl), Byte *subTitle,
                                CheckBoxRec *checkBox );
Boolean PASCAL	StdDlgSelectDir ( Byte *path, Byte *title, Byte *subTitle,
                                CheckBoxRec *checkBox );

Boolean CDECL	StdDlgTaskSwitch(void);

Boolean CDECL	StdDlgMultitasking(Word warnEnv, Word errorEnv);
Boolean CDECL	StdDlgFastOpen(Boolean warnOnly);
Boolean	CDECL	StdDlgLantastic( Boolean bWarnOnly );
Boolean PASCAL	StdDlgTreeSize(Byte *path, Ulong *treeSizeRet, Ulong *clustersRet,
                        Word clusterSize, Boolean pause);

typedef	struct
    {
    Boolean	bAllNet;		// all network drives selected
    Boolean	bAllLocal;		// all local drives selected
    Boolean	bAllFloppy;		// all floppy drives selected
    Boolean	bDisableNet;
    Boolean	bDisableLocal;
    Boolean	bDisableFloppy;
    Byte	*pszDriveTextList;	// return buffer for the drive string
    Byte	*pszSelectedDrives;	// used to set selections on entry
    } SelectDriveInfo;

Byte PASCAL	StdDlgSelectDriveList (
    Byte byDefaultDrive, 		// default drive to start with
    Byte *pszTitle,			// title of the dialog box
    Byte byDefaultDriveType, 		// default drive type to start with
    Byte *pszSelectedList,		// returned list of drives
    SelectDriveInfo *pInfoRec,		// info record (see STDDLG.H)
    Word wFlags );			// flags - see STDDLG.H

Byte PASCAL	StdDlgSelectDriveList2 (
    Byte byDefaultDrive, 		// default drive to start with
    Byte *pszTitle,			// title of the dialog box
    Byte byDefaultDriveType, 		// default drive type to start with
    Byte *pszSelectedList,		// returned list of drives
    SelectDriveInfo *pInfoRec,		// info record (see STDDLG.H)
    CheckBoxRec *pCheckBox,		// additional check box
    Word wFlags );			// flags - see STDDLG.H

void PASCAL StdDlgFormatDriveStr(Byte *pszUnformatted, Byte *pszFormatList);

#ifdef INCL_DISK
#ifndef INCL_NDSK

Boolean PASCAL	StdDlgGetPhysicalSectorRange (COLORS *colors,
					     Byte *title,
					     AbsDiskRec *absDisk,
					     Word *head,
					     Word *track,
					     Word *sector,
					     Ulong *range,
					     Boolean (*rangeCheck)(AbsDiskRec *, Word *, Word *, Word *, Ulong *),
					     Byte flags );
Boolean PASCAL	StdDlgViewDiskMap (DiskRec *disk, Byte *name, Word mapRows, Word mapCols, COLORS *colors, Word scn, Word endCluster );
Word    PASCAL	DiskMapRatio (DiskRec *disk, Word mapSize);
void    PASCAL	DiskMap (DiskRec *disk, Word mapRow, Word mapCol,
                        Word numRows, Word numCols, Byte far *map, Byte *mapChars, Word scn, Word endCluster );
Boolean PASCAL	StdDlgGetClusterRange (COLORS *colors,
				      Byte *title,
				      DiskRec *disk,
				      Word *low,
				      Word *high,
				      Boolean (*rangeCheck)(DiskRec *, Word *, Word *),
				      Byte flags );
void PASCAL StdDlgGetClusterUpdateTEs (Word low, Word high, Byte flags );
Boolean PASCAL StdDlgGetSectorRange(COLORS *colors,
				    Byte *title,
				    DiskRec *disk,
				    Ulong *low,
				    Ulong *high,
				    Boolean (*rangeCheck)(DiskRec *, Ulong *, Ulong *),
				    Byte flags );
void PASCAL StdDlgGetSectorUpdateTEs (Ulong low, Ulong high, Byte flags);

#endif
#endif  /* INCL_DISK */

typedef  Boolean (PASCAL *CRASHPROC)(void);
extern CRASHPROC  crashProc;

VOID PASCAL StdDlgCrashAndBurn(
    Byte        *pszMessage,            /* Message to display           */
    Boolean     bShutdown);             /* Shutdown application         */

Boolean PASCAL StdDlgGetSearchKey (COLORS *colors, Byte *title, Byte *key, Word *keyLen, Boolean *ignoreCase, Boolean *keyChanged, int *pnOffset);

Boolean PASCAL StdDlgUpdateFillBar (OutputFieldRec *fillBarField, Ulong currentSize, Ulong maxSize);
Boolean PASCAL StdDlgUpdatePercent (OutputFieldRec *percentField, Ulong currentSize, Ulong maxSize);
Boolean PASCAL StdDlgUpdateTime( OutputFieldRec    *elapsedField, Ulong currentSize, Ulong maxSize, Ulong timeStart );

Boolean PASCAL StdDlgStopOp (void);
Boolean CDECL StdDlgBreakCheck (void);

int     CDECL StdDlgConfirmDeleteDir(Byte *path, Boolean dirsOK);

#ifdef INCL_DRAW
Boolean StdDlgCustomColor(MenuBarRec *menuBar, ColorSet *altColors, ColorSet *defaultColors);
#endif

Boolean PASCAL  StdDlgOk ( Byte *pStrings[] ) ;

#endif /* INCL_DIALOG */

#ifdef	INCL_VDISKREC
Word PASCAL StdDlgBrowse (
  Byte		*pszFileName,
  Byte 		*pszInitialFileSpec,
  Word		wFlags,
  Byte 		*pTitle,
  Word		wMaxFiles,
  VDISKREC	*pVDisk);
#endif

Boolean  PASCAL  StdDlgStartWait(void);
void	 PASCAL  StdDlgEndWait(void);

void StdDlgHexCalculator( void );

/*----------------------------------------------------------------------*
 * Directory Module							*
 *----------------------------------------------------------------------*/

#ifndef _INCL_NFOP_
int	CDECL	DirRemove (Byte *path);
int	CDECL	DirMake (Byte *path);
Word 	PASCAL	DirParsePath(Byte *path);
#endif

Boolean	PASCAL	DirSearchPath (Byte *foundPath, Byte *fileName);
int	CDECL	DirGetName (Byte driveLetter, Byte *path);
int	CDECL	DirChange (Byte *path);
int	PASCAL	DirCreate (Byte *path);
void	CDECL	DirTruncateName (Byte *path);
Boolean PASCAL  DirSearchNortonPath (Byte *pszVar, Byte *file, Byte *fullName);
void	PASCAL	DirNewNortonPath (Byte *pszVar, Byte *pszSearch, Byte *fullName, Byte *name);
Boolean PASCAL  DirSearchProductPath (Byte *pszVar, Byte *file, Byte *fullName);
void	PASCAL	DirNewProductPath (Byte *pszVar, Byte *pszSearch, Byte *fullName, Byte *name);
Boolean PASCAL  DirSearchProgramPath (Byte *pszProductVar, Byte *file, Byte *fullName);
Boolean PASCAL  FindWindowsDir (Byte *szReturnPath);

int PASCAL DirRemoveTree(Byte *path, void (*displayFunc)(Byte *), int (*breakFunc)(void));
int DirCopyTree( Byte *source, Byte *dest, void (*displayFunc)(Byte *),
                Boolean (*breakFunc)(void));
int DirMoveTree( Byte *source, Byte *dest, void (*displayFunc)(Byte *),
                Boolean (*breakFunc)(void));
Ulong DirSize(Byte *path, Ulong *clusters, Word clusterSize);
Ulong DirTreeSize(Byte *path, Ulong *clusters, Word clusterSize);
Word PASCAL DirGetLongestPath (Byte *sourceDir, Byte *destDir);

/*----------------------------------------------------------------------*
 * Disk I/O Module							*
 *----------------------------------------------------------------------*/

#ifdef INCL_DISK
#ifndef INCL_NDSK	/* If not using Quake definitions */

BOOL	PASCAL	DiskGetVolumeLabel(BYTE byDriveLetter, BYTE *pszVolumeLabel);
BOOL	PASCAL	DiskSetVolumeLabel(BYTE byDrive, BYTE *pszVolumeLabel);
void	PASCAL	DiskSetBPBVolumeLabel(BYTE byDrive, BYTE *pszVolumeLabel);

void 	PASCAL  DiskBPBtoDiskRec(int BPBIndex, BPBRec *BPBPtr, DiskRec *diskPtr, AbsDiskRec *absDiskPtr);
Boolean PASCAL  DiskRecToFloppyAbsDiskRec(DiskRec *diskPtr, AbsDiskRec *absDiskPtr);
Word 	PASCAL	DiskGetBPBVersion(Byte *oemSignature, Word *BPBSizePtr);
Boolean PASCAL 	DiskRecalcPartEntry(PartEntryRec *partPtr, AbsDiskRec *absDiskPtr);
Ulong	PASCAL 	DiskGetSectorStartOffset(AbsDiskRec *absDiskRecPtr, PartEntryRec *partPtr);
Ulong	PASCAL 	DiskGetSectorEndOffset(AbsDiskRec *absDiskRecPtr, PartEntryRec *partPtr);
void 	CDECL 	DiskGetLockedTable(register Byte *lockedTablePtr);
void 	CDECL 	DiskSetLockedTable(register Byte *lockedTablePtr);
void	CDECL	DiskDumpCachedWrites(void);
Word	CDECL	DiskIsNCACHEInstalled(void);
Boolean CDECL	DiskIsSmartdriveInstalled(void);
void	CDECL	DiskFlushSmartdrive(void);
void	CDECL	DiskSuspendSmartdrive(void);
void	CDECL	DiskResumeSmartdrive(void);
void	CDECL	DiskDisableMonitor(void);
void	CDECL	DiskEnableMonitor(void);
void	CDECL	DiskDisableEraseProtect(void);
void	CDECL	DiskEnableEraseProtect(void);
void	CDECL	DiskFastOpenFlush(Byte driveLetter);
Boolean	CDECL	_DiskFastOpenInstalled(void);
Ulong	CDECL	DiskCalcPhysicalSectors(BPBRec *BPBPtr);
void	PASCAL	DiskFillInBPB(Ulong BPBhiddenSectors, BPBRecord *BPBPtr,
		    	      DiskRecord *diskPtr, AbsDiskRecord *absDiskPtr);
Boolean	PASCAL	DiskIsValidDiskRec(DiskRec *diskPtr);
int	PASCAL	DiskGetType(DiskRec *diskPtr, AbsDiskRec *absDiskPtr,
				Byte far *buffer);
int	PASCAL	NUDiskGetType(Byte dl);
Word	PASCAL	DiskGetDASDType(Byte dn, Byte *typePtr);
Byte	PASCAL	DiskGetAccessFlag(Byte dl);
Boolean	PASCAL	DiskSetAccessFlag(Byte dl, Byte accessType);
Boolean PASCAL	DiskGetHDParamTable(Byte dn, HDParamTable *driveParamTable);

Boolean PASCAL	DiskIsReady(Byte dl);

Byte	PASCAL  DiskGetBootDrive (void);
Boolean PASCAL	DiskGetDriverTable (Byte dl, DiskRecord *diskPtr);
Boolean PASCAL	DiskGetDriverTablePtr (Byte dl, DiskRecord far **drivePtrPtr);
DiskRecord far * PASCAL DiskFindDPB(Byte dl);
Boolean	PASCAL	DiskAllocDMABuffer(Byte far **buffer);
void    PASCAL  DiskFreeDMABuffer(void);
Boolean	PASCAL	_DiskAllocDMABuffer(Byte far **buffer, Word nSectors);
void    PASCAL  _DiskFreeDMABuffer(Byte far *lpBuffer);
Byte far * PASCAL DiskGetDMABuffer(void);
Word	PASCAL	DiskGetDMABufferSize(void);
Word	PASCAL	_DiskGetFreeSpaceInfo (Byte driveLetter, Word *bytesSectorPtr,
				      Word *sectorsClusterPtr,
				      Word *clustersPtr);
Boolean PASCAL	DiskGetHDParams (DiskRecord *diskPtr,
			 	AbsDiskRecord  *absDiskPtr,
			 	BPBRecord	*BPBPtr,
			 	Byte far	*buffer);

Boolean	CDECL	DiskGetInfo ( Byte dl, DiskRec *diskRecPtr);
Ulong	CDECL	DiskGetSerialNumber (void);
void		DiskReset(void);
void    PASCAL	DiskResetDOS (Byte dl);
Boolean	PASCAL	DiskResetAccessFlag(Byte dl);
Boolean PASCAL	DiskSetFATPtr (Byte dl, Word cluster);
Byte	PASCAL	DiskSetDASDType (Byte disk,Byte type);
Boolean		DiskSingleFloppy (void);
void	PASCAL	DiskUnscrambleBIOSCoding (Byte *encdptr, Word *cylPtr, Byte *sctPtr);
Word 	PASCAL 	DiskScrambleBIOSCoding (Word track, Byte sector);
Byte	CDECL 	DiskSeek(Byte driveNum, Word track);
Byte	CDECL	DiskReady(Byte driveNum);
void	PASCAL	DiskLogSectorToPhySector (Ulong sector, DiskRecord *diskPtr,
				AbsDiskRecord *absDiskPtr);

void	PASCAL	DiskMapLogToPhyParams (Ulong sector, Byte dl, Byte far *buffer,
				Int13Rec NEAR *int13PackagePtr);

void		HandleDiskErrors(void);	/* Pass addr to DiskErrorOn() */
Byte    PASCAL	DiskAbsOperation(Word command, AbsDiskRec *absDiskRecPtr);
Byte	PASCAL	DiskGetPhysicalInfo(AbsDiskRecord *absDiskPtr);
Byte	PASCAL	_DiskGetPhysicalInfo(AbsDiskRecord *absDiskPtr);
Word	PASCAL	DiskGenericIOCTL(Word command, Byte dl, void far *paramBlock);
Word	PASCAL	DiskIOCTL(Byte byCommand, Word wDriveOrHandle,
			  void far *lpBuffer, Word wBufferSize);
Word	PASCAL	DiskIOCTLError(void);
int	PASCAL	DiskParseFilename(Byte *commandLineToParse, Byte *unopenedFCB);
Byte    	DiskGetMaxDrive(void);
Word	PASCAL	DiskIsAssigned(Byte drv);
Word	CDECL	_DiskIsAssigned(Byte *source, Byte *dest );
Boolean PASCAL 	DiskIsBlockDevice (Byte dl);
Word	PASCAL	DiskIsFixed(Byte drv);
Byte    PASCAL  DiskIsPhantom(Byte dl);
Byte	PASCAL	DiskIsLogical(Byte dl);
Word	PASCAL	DiskIsNetwork(Byte drv);
Word	PASCAL	DiskIsCDROM(Byte drv);
Word	PASCAL	DiskIsSubst(Byte drv);
Ulong   PASCAL	DiskIsEncrypted(Byte dl);
Boolean PASCAL	DiskIsRAMDrive(Byte dl);
Boolean PASCAL	DiskIsTrueDrive(Byte dl, Byte filter, Boolean (*driveFilterProc)(Byte dl) );
Boolean	PASCAL	DiskIsValidBPB(BPBRec *BPBPtr);
Boolean	PASCAL	DiskIsSpeedStor(Byte byDriveLetter);  // Storage Dimensions partitions
Boolean PASCAL	DiskIsCompressed(Byte dl);
Boolean PASCAL	_DiskIsStackerLoaded(void);
Boolean PASCAL	DiskIsStacker(Byte dl);
Boolean	PASCAL	DiskIsSuperStor(Byte byDriveLetter);  // Compression driver
Byte	PASCAL	DiskGetSuperStorSwapDrive(Byte bySuperStorDrive);
Byte	PASCAL	DiskGetSuperStorHostDrive(Byte bySuperStorDrive);
Boolean PASCAL	DiskIsDoubleSpace(Byte driveLetter);
Boolean PASCAL  _DiskIsDoubleSpace(Byte driveLetter, Byte *pbyHostDriveLetter,
				   Boolean *pbSwapped, Byte *pbySequence);
Word 	PASCAL 	DiskType(Byte dl);
Word    PASCAL  DiskGetDriveType(DiskRecord *diskPtr, AbsDiskRecord *absDiskPtr);
void    PASCAL  DiskTypeResetTable(void);
void    PASCAL  DiskTypeResetEntry(Byte dl);
int	PASCAL	DiskIsBig(Byte dl, DiskRecord *diskPtr);
Word		_DiskIsBig(Byte drv_num,Word buff_seg,Word buff_off);
Boolean PASCAL	DiskIsBigTable (DiskRecord *diskPtr, Word *startOfRootPtr,
                       		Word *sectorsPerFatPtr, Boolean *okPtr);
Boolean	PASCAL	DiskIsLantasticServer(void);
Word	CDECL	_DiskBlockDevice (int command, Byte isBigFooted,
			Byte dn, Word sectorsToRead,
			Ulong startingSector, Word bufferSeg, Word bufferOff);
Word	PASCAL	DiskBlockDevice(int command, DiskRec *diskPtr);
Word	PASCAL	DiskSetFormatType ( AbsDiskRecord *absDiskPtr,
				DisketteParamRecord *paramPtr);
Word	PASCAL	DiskSetVerifyState (Word newState);
Boolean CDECL	DiskGetVerifyState (void);
Ulong   PASCAL  DiskComputeDriveSize ( DiskRecord *diskPtr );
Byte  * PASCAL  DiskGetPartitionSystemString (Byte systemID);
void	CDECL	DiskGetLowLevelInt13(void);
Byte    CDECL	DiskLowLevelInt13 (Word command, AbsDiskRec *absDiskRecPtr);
Boolean PASCAL  DiskCheckFatIds (DiskRec *diskPtr, Byte far *buffer);
int 	PASCAL 	DiskFATCheck( DiskRec *diskPtr );
Byte	CDECL	DiskCheckController(Byte driveNumber);
Byte	CDECL	DiskCheckControllerRAM(Byte driveNumber);
int     CDECL   DiskCheckBPB(Byte dl);

void	PASCAL	DiskDiskRecToBPB (Byte dl, LPDISKREC diskPtr, LPBPBREC BPBPtr);
Word	PASCAL	DiskBPBTable (Word wDiskMinorType,
				Byte far *lpbyBPBTable, Word far *lpwCount);
Word	PASCAL	DiskBPBFromIndex (Word wBPBIndex, LPBPBREC lpBPBRec);

void 	PASCAL 	DiskKeepRunning(Byte byClockTicks);
Byte 	PASCAL 	DiskMotorOffGet(void);
Byte 	PASCAL 	DiskMotorOffSet(Byte byClockTicks);
Byte 	PASCAL 	DiskSettleTimeGet(void);
Byte 	PASCAL 	DiskSettleTimeSet(Byte byMilliseconds);
Byte 	PASCAL 	DiskStartupGet(void);
Byte 	PASCAL 	DiskStartupSet(Byte byEightsOfSec);

#endif
#endif

Boolean	CDECL	DiskFastOpenInstalled(Byte dl);

#ifndef _INCL_NFOP_
Word    PASCAL	DiskSet (Byte driveLetter);
Byte		DiskGet (void);
Boolean	PASCAL	DiskGetAllocInfo(Byte dl, Byte *mediaDescriptor,
				Byte *SectorsPerCluster, Word *totalClusters,
				Word *bytesPerSector);
Byte		DiskGetEquipmentDrives(void);
void    PASCAL	DiskSetToSingleDisk (Byte dl);
Word    PASCAL	DiskSetLogical (Byte dl);
Word    PASCAL  DiskGetFreeSpaceInfo(Byte byDriveLetter, Word *pwBytesPerSector, Word *pwSectorsPerCluster, Word *pwTotalClusters);
Word	PASCAL	DiskGetClusterSize(Byte dl);
#endif

#ifndef _INCL_NPRM
Ulong	CDECL	DiskGetCacheSize(Word *pwBlockSize, Word *pwCurrentBlocks,
				 Word *pwDosBlocks, Word *pwWinBlocks);
void	CDECL	DiskDisableCache(void);
void	CDECL	DiskEnableCache(void);
Boolean	CDECL	DiskCacheInstalled(void);
#endif

#ifdef	INCL_VDISKREC
Ulong	PASCAL	VDiskGetDriveList (VDISKREC *pVDisk, Byte *pszDriveList,
					Word *wListIndex);
#endif

/*----------------------------------------------------------------------*
 * DOS Module								*
 *----------------------------------------------------------------------*/

#define exit() DOSExit()

void	PASCAL	DOSExit(void);
int	PASCAL	DOSExec(Byte *file, Byte *commandLine);
void    PASCAL	DOSPrepExec(Byte *commandLine);
Word	CDECL   DOSErrorLevel(void);

void	PASCAL	DOSGetInternationalInfo(void);
Word	CDECL   DOSGetCodePage(void);
Word	CDECL   DOSSetCountryCode(Word country);

Byte far * PASCAL DOSGetCollatingTable(void);  // Found in STRING\COLLATE.ASM

void	CDECL	DOSBreakOn(void);
void	CDECL	DOSBreakOff(void);
void	CDECL	DOSInstallBreakHandler(void);
void	CDECL	DOSUnInstallBreakHandler(void);
void	CDECL	DOSInstallInt02Handler(void);
void	CDECL	DOSUnInstallInt02Handler(void);
void	CDECL	DOSInstallInt09Handler(void);
void	CDECL	DOSUnInstallInt09Handler(void);
Boolean	CDECL	DOSRebootCheck(void);
void	CDECL	DOSInstallInt1CHandler(void);
void	CDECL	DOSUnInstallInt1CHandler(void);
void	CDECL	_WaitForTick(void);
Boolean	CDECL	DOSBreakCheck(void);
void	PASCAL  DOSGetProgName(Byte *p);
void	CDECL   DOSReboot(void);
Boolean	CDECL	DOSStdOutRedirected(void);
Byte    CDECL	DOSGetBootDisk(void);
BYTE    PASCAL  DOSGetRealBootDrive(void);
BYTE    PASCAL  DOSGetStartupDrive(void);
WORD    PASCAL  DOSGetHostDrive(LPBYTE lpbyDrive);
Ulong	CDECL	DOSReadCountFromCounter(void);
Boolean CDECL	DOSIsAppendLoaded(void);
void	CDECL	DOSDisableAppend(void);
void	CDECL	DOSEnableAppend(void);
Word	CDECL	DOSGetAppendState(void);
void	CDECL	DOSSetAppendState(Word state);
Word	PASCAL	DOSGetProcessFileHandleCount(void);
Word	PASCAL	DOSGetSystemFileHandleCount(void);
Word	PASCAL	DOSGetOpenFileCount(Byte byDriveLetter, Boolean bOnlyIfWritten);
Boolean	PASCAL	DOSGetOpenFileName(int nType, Word wHandle, Byte *pszName);
Boolean	PASCAL	DOSIsFileOpen(Byte byDriveLetter, Word wStartCluster);
Word	PASCAL	DOSGetSystemFileHandle(Word wProcessHandle);
#ifdef INCL_DOS
Boolean	PASCAL	DOSGetOpenFileInfo(int nType, Word wHandle, OpenFileInfoRec far *lpInfo);
Word	PASCAL	DOSGetInfoOnAllOpenFiles(Word wMaxFiles, OpenFileInfoRec far *lpInfo);
SFTEntryRec far * PASCAL DOSGetSystemFileTableEntry(Word wHandle);
DeviceDriverRec	far * DOSGetFirstDriverAddress(void);
void 	PASCAL	DOSGetCommandLine(PSPRecord far *psp, Byte *commandLine);
#endif

#ifndef _INCL_NPRM
Boolean	PASCAL	DOSGetInternalVariables(BYTE FAR * FAR *buffer);
void	PASCAL  DOSGetEnvironment(Byte *name,Byte *value,int length);
void	PASCAL  DOSGetEnvironmentCase(Byte *name,Byte *value,int length);
Byte	PASCAL	DOSGetDate(Word *month, Word *day, Word *year);
void	PASCAL  DOSGetTime(Word *hour, Word *min, Word *sec, Word *hund);
Byte	CDECL	DOSOEMNumber(void);
void	CDECL   DOSSetBreakStatus (Boolean flag);
Boolean	CDECL   DOSGetBreakStatus (void);
void    CDECL   DOSGetIntHandler(Word intNum, void far * *handler);
void	CDECL   DOSSetIntHandler(Word intNum, void far *handler);
void	PASCAL	DOSSetDate(Word month, Word day, Word year);
void	PASCAL  DOSSetTime(Word hour, Word min, Word sec, Word hund);
Word	CDECL   DOSGetVersion(void);
#endif

/* The next two functions are located in \LIBSRC\DISK\FAST.C */
void		DOSGetProgramNameOfPSP(Word pspSeg, Byte *path, Byte *name, Byte *commandLine);
Boolean		DOSTSRSearch(Byte *tsrName, Byte *commandLine);

/*----------------------------------------------------------------------*
 * DOS Standard Screen I/O Module					*
 *----------------------------------------------------------------------*/

void	PASCAL	DOSWriteChar(Byte c);
void	PASCAL	DOSWriteNChars(Byte c, Word count);
void	PASCAL	DOSWriteString(Byte *string);
void    PASCAL  DOSWriteStringArray( Byte *aszStringArray[] );
void	PASCAL	DOSWriteUnsignedDecimal(Word value);
void	CDECL	DOSWriteLong(Ulong value);
void	PASCAL	DOSWriteStderr(Byte *string);
void    PASCAL  DOSWriteStdaux(Byte *byString);
void    PASCAL  _DOSWriteStdaux(Byte *byString);
void	CDECL   DOSWriteCRLF(void);
void	CDECL	DOSPrint(Byte *format, ...);
void	CDECL	DOSPrintStdaux(BYTE *pszFormat, ...);
void	PASCAL	DOSPrintTimeStdaux(Byte *message);

/*----------------------------------------------------------------------*
 * Draw  Module								*
 *----------------------------------------------------------------------*/

#ifdef INCL_DRAW

void	PASCAL	DrawBox(BOXES *box, int row, int col);
void	PASCAL	DrawDoubleBox( Word top, Word left, Word height, Word width );
void	PASCAL	DrawSingleBox( Word top, Word left, Word height, Word width );
void    PASCAL  DrawGraphicBox( Word top, Word left, Word height, Word width );
void    PASCAL  DrawOutlineSingleBox( Word top, Word left, Word height, Word width );
void    PASCAL  DrawOutlineDoubleBox( Word top, Word left, Word height, Word width );
void    PASCAL  DrawOutlineBox( Word top, Word left, Word height, Word width );
void    PASCAL  DrawButtonBox( Word top, Word left, Word height, Word width );
void    PASCAL  DrawOutlineGraphicBox( Word top, Word left, Word height, Word width );
void 	PASCAL	DrawZoomingBox( Word top, Word left, Word bottom, Word right,
		     			Boolean border, Boolean thin );
void	PASCAL	DrawGraphicalListBox( Word top, Word left, Word height, Word width );
void    PASCAL  DrawGroupBox( Word top, Word left, Word height, Word width );
void	PASCAL	DrawDesktopArea (Boolean plainDeskTop, int topRow );
void	PASCAL	DrawTitleBar(Byte *pszTitle, Boolean bCloseBox);

#endif

/*----------------------------------------------------------------------*
 * Error Module								*
 *----------------------------------------------------------------------*/

typedef	Word (*ERRORTRAPPROC)(Word errorInfo, Word errorCode, Byte *deviceName);

Word	ErrorTrap (Word errorInfo, Word errorCode, Byte *deviceName);
void	DiskErrorOn (ERRORTRAPPROC errorTrap);
void	DiskErrorOff (void);

#ifdef INCL_DISK
#ifndef INCL_NDSK
Word	Error2526(DiskRec *diskPtr, Word errorCode);
Word	Error13(AbsDiskRec *diskPtr, Word errorCode);
#endif
#endif

/*----------------------------------------------------------------------*
 * Idle Hook Module                                                     *
 *----------------------------------------------------------------------*/

Boolean	PASCAL	IdleHookRegister(void (*HookFunc)(void), Word wFrequency);
void	PASCAL	IdleHookUnregister(void (*HookFunc)(void));
void	PASCAL	IdleHookCall(void);
Boolean PASCAL	IdleHooksDisable(void);
Boolean PASCAL	IdleHooksEnable(void);

/*----------------------------------------------------------------------*
 * Event Handling Module						*
 *----------------------------------------------------------------------*/

Word	PASCAL	EventGetNext(Word *message);
void	PASCAL	EventPost(Word event, Word message);
Word 	PASCAL	EventAvail(Word *message);
Word		EventGetKB(void);

/*----------------------------------------------------------------------*
 * Function prototypes for far heap module  				*
 *----------------------------------------------------------------------*/

Boolean		FarHeapInit(void);
Boolean		FarHeapInitSize(Word size);
void far * PASCAL FarHeapAlloc(Ulong size);
void		PASCAL FarHeapFree(void far *buffer);
void		FarHeapFreeAll(void);
Ulong		FarHeapAvail(void);
Boolean 	FarHeapTerminate(void);
Boolean 	FarHeapShrink(void);
Boolean 	FarHeapUnshrink(void);
void far *	FarHeapMoveLastBlockUp(void far *blockStart);
void far *	FarHeapMoveLastBlockDown(void);
void PASCAL	FarHeapShrinkPara(Word paragraphsToFree);
Boolean PASCAL	FarHeapGrowPara(Word paragraphsToGrow);

/*----------------------------------------------------------------------*
 * Direct Screen I/O Module						*
 *----------------------------------------------------------------------*/

void		FastInitScreen3(Byte defaultColorSet, Byte maxGraphicCharMode,
				Boolean fastMouseReset, Boolean graphicalMouse,
				Byte displayLines);
void		FastRestoreScreen3(void);
void	        FastInitGraphicChars(Word maxGraphicCharMode, Boolean graphicalMouse);
void            FastRestoreGraphicChars(void);
void	PASCAL	FastSpecialInitFonts(void);
void 	PASCAL	FastSaveState(Boolean boolRestoreFonts);
Word 	PASCAL	FastRestoreState(void);
Boolean 	FastCanFontsBeRedefined(void);
void		FastGetCharShape(char c, char shape[16]);
void		FastSetCharShape(char c, char shape[16]);
void	PASCAL	FastAccessA000(void);
void	PASCAL	FastProtectA000(void);
void	PASCAL	FastClearFilterTable(void);
void	PASCAL	FastAddCharToFilterTable(Byte chr);
Byte	PASCAL	FastFilterSpecialChars(Byte c);
void    PASCAL	FastFilterString(Byte *string);
void    PASCAL	FastFilterBuffer(void *pBuffer, Word wBufferSize);
void    	FastInitDisplayModule(void);
void		FastFlush(void);
void	PASCAL	FastWriteRawChar(Byte c);
void 	PASCAL 	FastWriteChar(Byte c);
Byte		FastReadAttr(void);
void            FastSetAttr(Word wAttr);
Byte		FastReadChar(void);
void 	PASCAL 	FastWriteString(Byte *string);
void 	PASCAL 	FastWriteRawString(Byte *string);
Byte *	PASCAL 	FastWriteLine(Byte *s, Word offset, Word width);
Byte *	PASCAL 	FastWriteLineFar(Byte far *s, Word offset, Word width);
Byte *  PASCAL  FastWriteMarkedLine(Byte *s, Word offset, Word width, Word bytesBefore, Word bytesOn, int markedAttr);
void 	PASCAL 	FastWriteUnsignedDecimal(Word num);
void 	PASCAL 	FastWriteLong(Ulong num, Word width, Byte comma);
void 	PASCAL 	FastWriteNChars(Byte c, Word n);
void 	PASCAL 	FastWriteSpaces(Word n);
void		FastWriteCRLF(void);
void	PASCAL	FastWriteHotkeyString(Byte *string);
void	PASCAL	FastWriteHotkeyColorString(Byte *string, int attr);
void 	PASCAL 	FastSetRowCol(Word row, Word col);
void 	PASCAL 	FastGetRowCol(Word *row, Word *col);
void		FastSetCursor(void);
void 	PASCAL 	FastWriteFileName(Byte *name, Word width);
void		FastPrint(Byte *format, ...);
void    	FastPrintJustified(int controlCode, Byte *format, ...);
Byte		*FastStringPrint(Byte *returnString, Byte *format, ...);
void    	FastPrintStrings(Byte **formatStrings, ...);
void	PASCAL	FastWriteHex(Byte c);

void    PASCAL   FastFlashWordAtRowCol(Word row, Word startCol,
                                       Word endCol, Word flashes, Word rate);
Byte * CDECL    FastGetWordAtRowCol(Word row, Word column, Word *start, Word *end);

void	CDECL	CalcDisplayOffset(void);

#define		FastSetLeftMargin(l)	(leftMargin = (Byte) (l))
#define 	FastSetRightMargin(r)	(rightMargin = (Byte) (r))
#define		FastGetLeftMargin()	(leftMargin)
#define 	FastClearMargins()	(leftMargin = (Byte) 0, rightMargin = (Word) MAX_COL)
Word		FastGetRow(void);
Word		FastGetCol(void);

extern 	Byte NEAR	leftMargin;
extern  Word NEAR	rightMargin;
extern	Word NEAR	screenY;
extern	Word NEAR	screenX;
extern  Byte NEAR	originalAttr;

Byte 	PASCAL	FastScrollWindowLeft(Word top, Word left,
				Word height, Word width, Word distance);
Byte	PASCAL	FastScrollWindowRight(Word top, Word left,
				Word height, Word width, Word distance);

/*----------------------------------------------------------------------*
 * File Allocation Table Module	     					*
 *----------------------------------------------------------------------*/

#ifdef INCL_DISK
#ifndef _INCL_NFAT_

Boolean	PASCAL	FATAlloc(DiskRec *diskPtr);
void	PASCAL	FATFree(void);
int	PASCAL	FATRead(void);
int 	PASCAL  FATReadSpecify(Byte byWhichFAT);
Boolean PASCAL	FATWrite(void);
Word	PASCAL	FATGet(Word wCluster);
void	PASCAL	FATPut(Word wCluster, Word wNewValue);
Word	PASCAL	FATSize(DiskRec *d);
Word    PASCAL	FATFindSCN(DiskRec *diskPtr, Word cluster);
Word    PASCAL  FATChainCount(DiskRec *disk, Word scn);
Word 	PASCAL 	FATFragmentCount(DiskRec *disk, Word scn);
Word 	PASCAL 	FATGetNthLink(DiskRec *disk, Word scn, Word n);
Boolean PASCAL  FATClear(void);
Word	PASCAL	FATGetPrev (Word wCluster);

Boolean	PASCAL	FATBackAlloc(DiskRec *diskPtr);
void	PASCAL	FATBackFree(void);
Word	PASCAL	FATBackGet(Word wCluster);
void	PASCAL	FATBackPut(Word wCluster, Word wNewValue);
Word	PASCAL	FATBackGetPrev (Word wCluster);

Byte far * PASCAL FATBitAlloc(DiskRec *diskPtr);
void	PASCAL	FATBitFree(Byte far *lpBitmap);
Boolean PASCAL 	FATBitGet(Word cluster, Byte far *lpBitmap);
void 	PASCAL	FATBitPutOn(Word cluster, Byte far *lpBitmap);
void 	PASCAL	FATBitPutOff(Word cluster, Byte far *lpBitmap);

/* The next two functions are intended for internal library use */
void far * PASCAL FATLock(Word wCluster, Word *pwClusterOffset, Boolean bDirty);
void	PASCAL	FATUnlock(void);

#endif
#endif

/*----------------------------------------------------------------------*
 * FCB Module								*
 *----------------------------------------------------------------------*/

int	FCBParse(Byte *fileName, void *FCBPtr);

/*----------------------------------------------------------------------*
 * File Module								*
 *----------------------------------------------------------------------*/

extern	Word NEAR	error;

int	CDECL	FileFarRead(Word handle, Word bufferSeg, Word bufferOff, Word size);
int	CDECL	FileFarWrite(Word handle, Word bufferSeg, Word bufferOff, Word size);
Boolean	CDECL	FilePrint(Word wHandle, Byte *pszFormat, ...);
Ulong	CDECL	FileSize(Word handle);
Ulong	CDECL	FileGetPtr(Word handle);
int	CDECL	FileCreateUnique(Byte *fileName, Word attributes);
int	CDECL	FileDeleteFCB(void *FCBPtr);
Ulong   PASCAL  FileSpecSize(Byte *filespec);
int 	PASCAL	FileCopy(Byte *fromFile, Byte *toFile, Boolean (*breakCheck)(void));
int     PASCAL  FileCopyWithProgress( Byte *fromFile,
                                      Byte *toFile,
                                      Boolean (*breakCheck)(void),
                                      void (*ProgressProc)(DWORD dwCopied,
                                                           DWORD dwTotal));
int 	PASCAL	FileAppend(Byte *fromFile, Byte *toFile, Boolean (*breakCheck)(void));
void 	PASCAL	FileCreateUniqueName(Byte *pszFileName, Byte *pszPrefix,
				     Byte *pszExt);
Word    PASCAL  FileGetIllegalChars(Byte *pBuffer, Boolean bPathChars);
void	PASCAL	FileTrueName(Byte *pSrc, Byte *pDst);
Boolean	PASCAL	FileGetTextLine(int nFileHandle, Byte * szLineBuffer, Word wMaxLength);
Ulong	PASCAL	FileScanForSignature(Word wFileHandle, Ulong lStartOffset,
				     void far *lpDiskBuffer, Word wDiskBufferSize,
				     void *pSignature, Word wSignatureLen);

#ifndef _INCL_NFOP_
int	CDECL	FileDelete(Byte *fileName);
int	CDECL	FileRename(Byte *oldName, Byte *newName);
Word 	PASCAL	FileFlushHandle(Word hFile);
int	CDECL	FileFindFirst(Byte *fileName, Word attr, void *infoPtr);
int	CDECL	FileFindNext(void *infoPtr);
int	CDECL	FileGetAttr(Byte *fileName);
int	CDECL	FileSetAttr(Byte *fileName, Word attributes);
int	CDECL	FileGetDateTime(Word handle, Word *date, Word *time);
int	CDECL	FileSetDateTime(Word handle, Word date, Word time);
int     PASCAL  FileCmpDateTime(LPSTR lpszFile1, LPSTR lpszFile2);
Word 	PASCAL	FileCommit(Word hFile);
Word 	PASCAL	FileDup(Word far *lphDupFile, Word hFile);
int	CDECL	FileOpen(Byte *fileName, Word openMode);
int	CDECL	FileClose(Word handle);
Ulong	CDECL	FileSeek(Word handle, Ulong offset, Word method);
int	CDECL	FileCreate(Byte *fileName, Word attributes);
#endif

#ifndef FileRead
int	CDECL	FileRead(Word handle, void *buffer, Word size);
#endif

#ifndef FileWrite
int	CDECL	FileWrite(Word handle, void *buffer, Word size);
#endif

/*----------------------------------------------------------------------*
 * Standard Buffered File Module 					*
 *----------------------------------------------------------------------*/

void	CDECL	FileOpenBuffer (Word handle, Byte NEAR *buffer, Word maxSize);
int	CDECL	FileGetC (Byte NEAR *buffer);
Word	CDECL	FilePutC (Word c, Byte NEAR *buffer);
void	CDECL	FileClear (Byte NEAR *buffer);
Word	PASCAL	FilePutS (Byte *string, Byte NEAR *buffer);
int	PASCAL	FileGetLine (Byte NEAR *buffer, Byte *line, int maxLine);

#ifndef _INCL_NFOP_
Word	CDECL	FileFlush (Byte NEAR *buffer);
#endif

/*----------------------------------------------------------------------*
 * Help Module								*
 *----------------------------------------------------------------------*/

Word 	PASCAL  HelpInit(Word method, Byte *pszhelpFileName, Byte *pszProductVar);
Word	PASCAL	HelpClose (void);
Word	PASCAL	HelpSet (Word topic);
void	PASCAL	Help (void);

void 	PASCAL 	HelpLine (Byte *helpText);
Word            HelpLineGetMouseEvent(void);
void	PASCAL  HelpWriteProgramName (void);
void		HelpWriteLine (Byte *line, Word width);
void    PASCAL  HelpLinePush(void);
void    PASCAL  HelpLinePop(void);

#ifdef INCL_DIALOG
int 	CDECL	HelpHandleHyperTextMouse (ListRec *list, Ulong entryNum,
			   RECT *entryRect, Word buttons, int mouseClicks);
#endif

Boolean PASCAL	HelpInitHyperText( Word (*h)( Byte *str ) );

#ifdef INCL_HELP
Word	PASCAL	HelpParseTitles (HelpFileInfoRec *h, Byte far* lines[], Word maxTitles, Byte far* buf);
Word	PASCAL	HelpParseTextLines (HelpFileInfoRec *h, Byte far* lines[], Word maxLines, Word topic, Byte far* buf);

Word 	PASCAL HelpFileInit( HelpFileInfoRec *h, Byte *fileName );
Boolean PASCAL HelpFileClose( HelpFileInfoRec *h );
Boolean PASCAL HelpRead( HelpFileInfoRec *h, Ulong offset,
                         Word size, Byte far *buffer );
Word	PASCAL HelpGetOffsets( HelpFileInfoRec *h, Word index, HelpFileOffsetRec *offset );
Boolean PASCAL HelpGetText( HelpFileInfoRec *h, Word index, Byte far *buffer );
Boolean PASCAL HelpGetTitle( HelpFileInfoRec *h, Word index, Byte far *buffer );
#endif

/*----------------------------------------------------------------------*
 * Hardware Module							*
 *----------------------------------------------------------------------*/

#ifdef INCL_HARDWARE
HWEquipRec HWGetEquipmentList(void);
#endif

Word	PASCAL	HWWhichVGA( void );
Word	PASCAL	HWVGAMemSize( void );
void	PASCAL HWSetRTCDate(Word month, Word day, Word year);
void	PASCAL HWSetRTCTime(Word hour, Word minutes, Word secondes);
void	HWWarmReboot(void);
void	HWColdReboot(void);
Byte	HWSetPrinterTimeout(Byte newValue);
Boolean HWIsExpansionBox(void);
Word    HWFindMachine(Word workSeg, Word workOff, Byte *class);
Byte    *HWMachineName(Word machine);
Boolean HWMachineTypes(Byte *model, Byte *subModel);
Boolean HWIs3270(void);
Boolean _HWIs3270(void);
Byte	HWIsEGA(void);
Boolean _HWIsHercules(void);
Boolean _HWIsVGA(void);
Word    HWIsXGA(void);
Byte 	HWGetDisplayAdapterType(Word *displayMemory);
Boolean HWIsPS2(Word machine);
Word    HWGetDisplayCombinationCode(void);
Boolean _HWHasCMOS(void);
Boolean HWIsVectra(void);
Boolean HWIs6300Plus(void);
Boolean HWIsA20On(void);

WORD PASCAL JoystickCount(void);
BYTE PASCAL JoystickButtons(void);
void PASCAL JoystickPositions(WORD far * wJoystick1X, WORD far * wJoystick1Y,
                            WORD far * wJoystick2X, WORD far * wJoystick2Y);

#ifndef _INCL_NPRM
Boolean HWIsMicroChannel(void);
Boolean HWIsEISA(void);
Boolean HWHasCMOS(void);
#endif

/*----------------------------------------------------------------------*
 * Icon Module								*
 *----------------------------------------------------------------------*/

void PASCAL IconGet(Byte *pathName, Byte *iconLeft, Byte *iconRight);
void PASCAL IconGetPacked(Byte *pathName, Byte *iconLeft, Byte *iconRight);
void PASCAL IconGetFromExtension(Byte *pszExtension, Byte *iconLeft, Byte *iconRight);
void PASCAL IconGetDisk(Byte dl, Byte *iconPieces);
void PASCAL IconGetDiskType(Word driveType, Byte *iconPieces);

#ifdef	INCL_VDISKREC
void PASCAL VDiskIconGetDisk(VDISKREC *pVDisk, Byte dl, Byte *iconPieces);
#endif

/*----------------------------------------------------------------------*
 * Image Module                                                         *
 *----------------------------------------------------------------------*/

Byte  PASCAL  ImageSave (Byte byDrive, Boolean bSingleCopy);

/*----------------------------------------------------------------------*
 * Keyboard I/O Module							*
 *----------------------------------------------------------------------*/


Word		KBRead(void);
Word		KBReadScan(void);
Word            KBWrite(Word keyExtKey);
void            KBSetTypematicRate(Word rate, Word delay);
void	PASCAL 	KBSetRate (Word rate);
Word		KBScanToExtended(Word key);
void		KBFlush(void);
int		KBHit(void);			/* -1 if no key ready	*/
int		_KBHit(void);
Word	PASCAL	KBTranslate(Word);
Word		KBShiftFlags(void);
Word		KBSendCommand(Byte command);
Boolean	PASCAL 	KBProgrammable(void);
Boolean	PASCAL 	KBIsProgrammable(void);
Boolean         KBPseudoNeeded(void);
void            KBInstallPseudoHandler(void);
void            KBUnInstallPseudoHandler(void);
Boolean		KBGetInternational(Byte *pszCountyName, Word *pwSubType);
Boolean         KBPlaybackInit(VOID);
Boolean	PASCAL	KBBind(Word key, Boolean (*proc)(Word key));
Byte	PASCAL	KBConvertAlt(Word key);
void	PASCAL	KBKeyName(Byte *name, Word key);
Boolean CDECL	KBGetLine(Byte *string, Word maxLength);
Boolean PASCAL  KBKeyIsAltAlphaNumeric(Word c);
Boolean PASCAL  KBMacroInit(void);
void    	EventRecord(Word scancode, Word wRow, Word wCol, Word wButtons);
Word    	EventPlayback(Byte type);


/*----------------------------------------------------------------------*
 * Smaller keyboard routines that do not require a lot of library overhead.
 *-----------------------------------------------------------------------*/

int  		BiosKBHit(void);
Word 		BiosKBRead(void);
void  		BiosKBFlush(void);


/*----------------------------------------------------------------------*
 * LAN Manager Module							*
 *----------------------------------------------------------------------*/

#ifdef INCL_LANMAN

Boolean	PASCAL	LanManIsInstalled( void );

int	PASCAL	LanManServerEnum2( short sLevel, char far *pbBuffer, unsigned short cbBuffer,
			unsigned short far *pcEntriesRead, unsigned short far *pcTotalAvail,
			unsigned long flServerType, char far *pszDomain );

int	PASCAL	LanManUserGetInfo11( char far *pszServer, char far *pszUserName,
			short sLevel, char far *pbBuffer, unsigned short cbBuffer,
			unsigned short far *pcbTotalAvail );

Boolean	PASCAL	LanManUserGetID( char *pszUserName, unsigned short *pwAccountType );

int	PASCAL	LanManWkstaGetInfo( short sLevel, char far *pbBuffer, unsigned short cbBuffer );

#endif /* INCL_LANMAN */

/*----------------------------------------------------------------------*
 * Lantastic Module							*
 *----------------------------------------------------------------------*/

#ifdef INCL_LANTAST

Boolean	CDECL	LtCancelRedirection( Byte *pszDosName );
VOID	CDECL	LtFlushPrinter( void );
Word	CDECL	LtGetAccount( LtUserAccount *pUserAccount, Byte *pszServer );
Word	CDECL	LtGetActiveUserInfo( Word *pwIndex,
			LtActiveUserEntry *pActiveUserEntry,
			Byte *pszServer );
Word	CDECL	LtGetInactiveServerEntry( Word *pwIndex, Byte *pszServer,
			Word *pwAdapter );
Word	CDECL	LtGetLoginEntry( Word *pwIndex, Byte *pszServer,
			Word *pwAdapter );
Word	CDECL	LtGetLoginUserEntry( Word *pwIndex, Byte *pszServer,
			Byte *pszUserName, Word *pwAdapter );
Word	CDECL	LtGetMachineName( Byte *pszNetName, Byte *pcNetID );
Boolean	CDECL	LtGetRedirection( Word wFlag, Byte *pszDosName,
			Byte *pszNetName, Byte *pcDevType );
Boolean	CDECL	LtGetRedirectionEntry( Word wIndex, Byte *pszDosName,
			Byte *pszNetName, Byte *pcDevType );
Word	CDECL	LtGetSharedDirectoryInfo( Byte *pszDefinition,
			Byte *pszResource, Word *pwAccess );
Word	CDECL	LtGetUserAccount( Word *pwIndex, Byte *pszUser,
			Byte *pszServer );
Word	CDECL	LtGetUserName( Byte *pszName, Word *pwPrivilege,
			Byte *pszServer );
Word	CDECL	LtGetUserNameEntry( Word *pwIndex, Byte *pszName,
			Word *pwAdapter );
Word	CDECL	LtGetVersion( void );
Word	CDECL	LtInstallationCheck( void );
Boolean	CDECL	LtInstallMessageService( void );
Boolean	CDECL	LtLanpupIsInstalled( void );
Word	CDECL	LtLogin( Byte *pszzLoginData, Word wAdapter );
Word	CDECL	LtLogout( Byte *pszServer );
Word	CDECL	LtLogoutAll( void );
Boolean	CDECL	LtPollMessage( Byte *pbyMsgType, Byte *pszOriginator, Byte *pszText );
Boolean	CDECL	LtRedirectDevice( Byte *pszDosName, Byte *pszNetName,
			Byte cDevType );
Boolean	CDECL	LtRedirectorIsInstalled( void );
Boolean	CDECL	LtRemoveMessageService( void );
Word	CDECL	LtSendMessage( Byte *pszMachine, Byte *pszServer, Byte *pszUser, Byte *pszText );
Boolean	CDECL	LtServerIsInstalled( void );

#endif /* INCL_LANTAST */

/*----------------------------------------------------------------------*
 * List Module								*
 *----------------------------------------------------------------------*/

#ifdef INCL_DIALOG

void	PASCAL 	ListInit (LIST *list);
void    PASCAL	ListInitScrollRange (LIST *list);
void	PASCAL	ListInitWidth (LIST *list);
Ulong	PASCAL	ListSearch (LIST *list, Ulong start, Word what );
Boolean	PASCAL 	ListKey (LIST *list, int key);
void	PASCAL  ListMoveUp (LIST *list);
void	PASCAL	ListMoveDown (LIST *list);
Boolean	PASCAL 	ListMoveLeft (LIST *list);
Boolean	PASCAL  ListMoveRight (LIST *list);
void	PASCAL  ListHome (LIST *list);
void	PASCAL  ListEnd (LIST *list);
void	PASCAL  ListPageUp (LIST *list);
void	PASCAL  ListPageDown (LIST *list);
void	PASCAL  ListHighlight (LIST *list, Ulong i, int attr);
void	PASCAL  ListDisplay (LIST *list);
void    PASCAL  ListDisplayHighlighted (LIST *list);
void	PASCAL  ListDisplayRow (LIST *list, Ulong start);
void	PASCAL  ListDisplayEntry (LIST *list, Ulong i, int attr);
void	PASCAL  ListEntryRowCol (LIST *list, Ulong i, int *row, int *col);
void far * PASCAL ListEntryAddress (LIST *list, Ulong i);
void	PASCAL  ListEraseArea (LIST *list);
void	PASCAL  ListCopyEntries (LIST *list, Ulong startItem, Ulong numItems, Ulong dest);
void	PASCAL  ListMoveEntries (LIST *l, Ulong entry1, Ulong entry2);
void 	PASCAL  ListWrite (LIST *list, Boolean firstTime, Boolean active);
void 	PASCAL  ListResetScrollBar(LIST *list);
void 	PASCAL  ListDimensions( LIST *list, Word *height, Word *width );

void	PASCAL 	ListInsertEntry (LIST *list, void far *newEntry);
void	PASCAL 	ListDeleteEntry (LIST *list);

int	PASCAL 	ListMouse (LIST *list, int row, int col);
Ulong	PASCAL 	ListCalcFile (LIST *list, RECT *rect, int row, int col);
void	PASCAL 	ListIdle (LIST *list);
void	PASCAL 	ListScrollDelay (LIST *list, Word eventCounter);

void	PASCAL 	ListAdjustActiveEntry (LIST *list);
void	PASCAL 	ListAdjustTopEntry (LIST *list);

Word	PASCAL  ListMovement( LIST *list, Word *c, Boolean skipSearch );
Word 	PASCAL 	ListEvent (DIALOG_ITEM *field, Word *c);
void 	PASCAL 	ListScrollBarMouseEvent (LIST *list, Word row, Word col);
int	PASCAL	ListScrollBarMouseArea (ScrollBarRec *bar, Word row);
void 	PASCAL 	ListScrollBarMouseThumbEvent (LIST *list);
void 	PASCAL	ListScrollBarMouseScrollEvent (register LIST *list, Word row, Boolean up);
void	PASCAL	ListScrollBarMousePageEvent (register LIST *list, int area);
void	PASCAL	ListScrollBarEnable (DIALOGREC drDialog, ListRec *pList, Boolean bEnabled);

void	PASCAL	ListSizeToScreen (ListRec *pList, Byte byNormalHeight);
void	PASCAL	ListEmptyMessage (ListRec *pList, Byte *pszEmptyMessage);
void 	PASCAL	ListDrawTitle (register ListRec *list, Byte *title);
void	PASCAL	ListSetScrollBar (ListRec *list);

void	PASCAL	ScrollBarInit (register ScrollBarRec *bar, Ulong numValues, Word pageSize);
void	PASCAL	ScrollBarDraw (register ScrollBarRec *bar);
void	PASCAL	ScrollBarSetValue (register ScrollBarRec *bar, Ulong value);
Word	PASCAL	ScrollBarThumbPos (register ScrollBarRec *bar);
Ulong	PASCAL	ScrollBarValueForThumb (register ScrollBarRec *bar, Word thumbPos);
Boolean PASCAL	ScrollBarRectPointIn (register ScrollBarRec *bar, int row, int col);
void	PASCAL	ScrollBarEnable (ScrollBarRec *pBar, Boolean bEnable);

#endif /* INCL_DIALOG */

/*----------------------------------------------------------------------*
 * MKBOOT.lib: make disk bootable                                       *
 *----------------------------------------------------------------------*/

#ifdef INCL_MAKEBOOT

int     PASCAL  MakeDiskBootable(Byte dl, Word wFlags);

#endif

/*----------------------------------------------------------------------*
 * Main Functions Module						*
 *----------------------------------------------------------------------*/

#ifdef INCL_MAIN

void	PASCAL	MainCheckHelp (Byte *helpScreen[]);
void	PASCAL	MainPrintBanner (void);
void    PASCAL  MainProcessCmdLine (Byte *commandLine);
int     PASCAL  MainParamCount (void);
int     PASCAL  MainSwitchCount (void);
Byte *  PASCAL  MainParamGet (Word paramNumber);
void	PASCAL	MainParamToUpper (void);
int	PASCAL	MainSwitchGetText (SwitchRec *switchVar, Byte *string, Word maxSize);
int	PASCAL	MainSwitchGetTextSpecial (SwitchRec *switchVar, Byte *string, Word maxSize);
int	PASCAL	MainSwitchGetNumber (SwitchRec *switchVar, Word *valuePtr);
int	PASCAL	MainSwitchSet (SwitchRec *switchVar);
void 	PASCAL 	MainFixupCmdLineDate (Byte *args, SwitchRec *sw, Byte replace);

#endif

/*----------------------------------------------------------------------*
 * Math Functions Module						*
 *----------------------------------------------------------------------*/

int	PASCAL	MathMax (Word a, Word b);
int	PASCAL	MathMin (Word a, Word b);
int	PASCAL	MathAbs (int a);
Ulong   PASCAL  MathLongRandom( void );
void    PASCAL  MathSeedLongRandom( Ulong ulNewSeed );
Word	PASCAL	MathRandom (void);
void	PASCAL	MathSeedRandom (Word n);
Word	PASCAL	percent (Word size, Ulong value, Ulong range);
Ulong 	PASCAL 	MathLongSwap (Ulong a);
Word 	PASCAL 	MathWordSwap (Word a);
Word 	PASCAL 	MathWordRotateLeft(Word number, Word bitsToRotate);
Word 	CDECL	MathDivUlongAndRound(Ulong dividend, Word divisor);

#ifndef _INCL_NPRM
Boolean	PASCAL	MathIsPower2 (register Word num);
Word	PASCAL	MathDiff (Word number1,Word number2);
Ulong	PASCAL	MathLongDiff (Ulong a, Ulong b);
#endif

/*----------------------------------------------------------------------*
 * Memory Module							*
 *----------------------------------------------------------------------*/

void            MemoryFarMove(void far *d, void far *s, Word count);
void            MemoryNearMove(void near *d, void near *s, Word count);
void            MemoryShellSort(void huge *base, Word itemSize,
                     Word numItems, int (*compare)(void far *i1, void far *i2));

void	PASCAL	MemoryMove(Word sourceSeg, Word sourceOff,
		    	   Word targetSeg, Word targetOff, Word length);

		/* Internal move routines: */
void	PASCAL	MemoryMoveFar(void far *source,
			      void far *target,
		 	      Ulong numBytes, Boolean reverse);
void	PASCAL	MemoryMoveNear(register Byte near *target,
                              register Byte near *source,
		              Word numBytes, Boolean reverse);

Word	   PASCAL DOSAlloc(Ulong far *reqSize, void far *retPtr);
void far * PASCAL DOSAllocParagraphs(Word wNumParagraphs);
void	   PASCAL DOSFree(void far *addr);
Word	   PASCAL DOSGetFreeMem(void);

Word	CDECL	MemoryFarAllocate(Word paragraphs, Byte huge **ptr);
Word	CDECL	MemoryFarDeallocate(Byte huge *ptr);
Boolean 	MemoryRelease(void);
Boolean PASCAL	MemoryShrink(Word segment, Word newLength);

Byte huge * PASCAL MemoryMSCAlloc(Ulong size);
void 	    PASCAL MemoryMSCFree(Byte huge *ptr, Ulong size);

int     PASCAL  MemoryCompareChar(void far *buf, Byte val, Word count);
Word 	PASCAL	MemoryCompareBlocks(Word Seg1, Word Off1,
			            Word Seg2, Word Off2, Word blockLength);
Boolean PASCAL	MemoryCompareBlockWithChar(Word Seg, Word Off,
			                   Word length, Byte value);
void	PASCAL	MemoryFill(Word Seg, Word Off, Word length, Byte value);
void		MemorySet(void far *d, Byte v, Word count);

Byte far * PASCAL MemoryScan(Byte far *buffer, Word scanLength,
			     Byte *match, Word length);
Word	PASCAL	MemoryScanForNonValue(Word segbuffer, Word offbuffer,
				      Word bufferSize, Word value);

void	PASCAL	MemoryFastSearchInit(Byte *key, Word keyLength, Boolean ignoreCase, Boolean endMatchesOK);
Boolean PASCAL	MemoryFastSearch(Byte far *buffer, Word bufferSize, Word *offset);

Word		MemoryTest(void);
Boolean PASCAL	MemoryIsRAM(Word p);
Word		MemoryGetSP(void);
Word		MemoryGetExtendedSize(void);
Word		EMSTest(void);

Ulong   PASCAL	MemoryAddrToLong(void far *ptr);

Ulong           MemoryUMBTotalAvail(void);
Ulong           MemoryUMBLargestAvail(void);
void far *      MemoryUMBAlloc(Ulong size);
void            MemoryUMBFree(void far *ptr);

unsigned short  MemoryGetCRC16(signed char *pcData, unsigned short wLength);
void* PASCAL    MemoryReverse(void *pMemory, Word max);

extern BOOL PASCAL CheckForEnoughMemory ( DWORD dwSize );
Boolean PASCAL  MemoryCheckMinimum(Ulong dwSize); // Use CheckForEnoughMemory instead
Ulong   PASCAL  MemoryGetMySize(void);

#ifndef _INCL_NPRM
Boolean PASCAL	MemorySearch(Word BuffSeg, Word BuffOff, Word length,
			     Byte *string, Word StrLngth);
void            MemoryHugeMove(void huge *d, void huge *s, Ulong bytes);
void far * PASCAL MemoryNormalizeAddress(void far *address);
#endif

/*----------------------------------------------------------------------*
 * Mouse Module								*
 *----------------------------------------------------------------------*/

#ifdef INCL_DRAW

Word	PASCAL 	MouseButtons(Word *row, Word *col);
#define MouseGetPosition(a,b) MouseButtons(a,b)

void	PASCAL	MouseGetSensitivity(Word *horiz, Word *verti, Word *dClick);
void	PASCAL	MouseSetBuffering(Word buffState);
void	PASCAL	MouseClearBuffer(void );
void	PASCAL	MouseSetSensitivity(Word horiz, Word verti, Word dClick);
void	PASCAL	MouseSetLimits(int nTopRow, int nLeftCol, int nBottomRow, int nRightCol);
void	PASCAL	MouseSetMinMaxVertical(Word min, Word max);
Byte		MouseGetShape(void);
void	PASCAL	MouseSetShape(Word shape);
int	PASCAL	MouseGetGraphicShape(void);
int	PASCAL	MouseSetGraphicShape(int nCursorType);
void	PASCAL	MouseSetPosition(Word row, Word col);
void	PASCAL	MouseSyncPositions(void);
Word	PASCAL	MouseGetState(void *stateBuffer, Word stateSize);
void	PASCAL	MouseSetState(void *stateBuffer);
int		MouseInit(void);
int		MouseInitSW(void);

void		MouseDisable(void);
void		MouseDisableDriver(void);
void		MouseEnableDriver(void);

void	PASCAL	MouseCheck(void);
void	PASCAL	MouseDown(Word row, Word col);
void	PASCAL	MouseClearState(void);
void	PASCAL	MouseUp(void);
Word	PASCAL	MouseTrackControl(RECT *rect, Byte leftC, Byte rightC, Byte bothC);
Word	PASCAL	MouseGeneralTrackControl(RECT *rect);
Word	PASCAL	MouseTrackOutside(RECT *rect);
Word	PASCAL	MouseTrackCancel(void);
void    PASCAL  MouseType(Byte *mouseVerHigh, Byte *mouseVerLow, Byte *mouseType, Byte *IRQ);
void	PASCAL	MouseBufferingOff(void);
void	PASCAL	MouseBufferingOn(void);

/* These routines are obsolete and no longer used */
#define		MouseHideCursor()
#define		MouseUnhideCursor()

void    	MouseHC(void);			/* Mouse hide cursor	*/
void    	MouseUHC(void);			/* Mouse Unhide cursor	*/

/* Macros to set graphic mouse cursor shape */
#define MouseNormal()	StdDlgEndWait()
#define MouseWait()	StdDlgStartWait()
Byte    PASCAL  StdDlgGetWaitDlgState(void);

/* The following should only be called from within the library */
void		MouseSpecialInitialization(Boolean bGraphicMouse);
void		MouseSpecialRestore(Boolean bGraphicMouse);
void		MouseInitInterrupt(void);
void		MouseRestoreInterrupt(void);

#endif

/*----------------------------------------------------------------------*
 * Name Module								*
 *----------------------------------------------------------------------*/

Word	PASCAL	NameIsPrinter (Byte *s);
Boolean	PASCAL	NameIsWildcard (Byte *name);
void	PASCAL	NameStripExtension (Byte *name);
void	PASCAL	NameRemoveDriveLetter (Byte *name);
Byte	PASCAL	NameExtractDrive (Byte *name);
void	PASCAL	NamePrintFileName (int outHandle, Byte *pathName, int width);
int	PASCAL	NameSameFile (Byte *name1, Byte *name2);
int	PASCAL	NameToFullName (Byte *d, Byte *s);
int	PASCAL	NameGetFullPath	(Byte *pathName);
Boolean	PASCAL	NameIsLocal (Byte *name);
//Boolean	PASCAL	NameIsFull (Byte *name);
Byte	PASCAL	NameDirPath (Byte *s, Byte *t, Boolean followsDIRRules);
int	PASCAL	NameInfo(Byte *pszName, Byte *pbyDrive, Byte *pszDir,
		     Byte *pszFile, Byte *pszFullPath, Word wFlags);

Word    PASCAL  NameValidatePath (Byte *pathname);
void    PASCAL  NameParse (Byte *szInputPath, Byte *pbyDriveLetter,
                        Byte *pszDirectory, Byte *pszFilename);

#ifndef INCL_NNAME
Boolean	PASCAL	NameReturnExtension (Byte *fileName, Byte *extension);
void	PASCAL	NameReturnFile (Byte *fileName, Byte *pathName);
void	PASCAL	NameStripFile (Byte *name);
void	PASCAL	NameReturnPath (Byte *pathOnlyName, Byte *pathName);
void	PASCAL	NameAppendFile (Byte *pathName, Byte *fileName);
void    PASCAL  NameToUnpacked(char *dst, char *src);
void    PASCAL  NameToPacked(char *dst, char *src);
void    PASCAL  NameToUnpackedFar(char far *dst, char far *src);
void    PASCAL  NameToPackedFar(char far *dst, char far *src);
void	PASCAL	NameConvertPath (Byte *fullPathName, Byte *relativePathName);
void	PASCAL	NameCompact (Byte *pathName);
void    PASCAL	NameUnWildcard ( Byte *original, Byte *new );
int	PASCAL	NameWildcardToName (Byte *fullWildcardedName, Byte *guideName);
Boolean	PASCAL	NameIsDevice (Byte *name);
Boolean PASCAL	NameIsValidFileChar (Byte ch, Boolean anyChar);
int	PASCAL	NameToFull(char *d, char *s);

// Macro replacements in used in NNAM.H
int	PASCAL	NameIsDirectory (Byte *name);
int	PASCAL	NameIsFile (Byte *name);
Boolean PASCAL	NameIsRoot (Byte *name);

#endif

WORD    PASCAL  FileIsWindowsEXE (Byte *pszPathname);

#ifdef	INCL_VDISKREC
int	PASCAL	VDiskNameToFull (VDISKREC *pVDisk, Byte *d, Byte *s);
int	PASCAL	VDiskNameIsDirectory (VDISKREC *pVDisk, Byte *name);
int	PASCAL	VDiskNameIsFile (VDISKREC *pVDisk, Byte *name);
int	PASCAL	VDiskNameSameFile (VDISKREC *pVDisk, Byte *s, Byte *d);
#endif

/*----------------------------------------------------------------------*
 * Novell Network Functions						*
 *----------------------------------------------------------------------*/

#ifdef INCL_NOVELL

Word    PASCAL  NovellGetConnectionNumber(void);
Word 	PASCAL 	NovellShellRequest( Byte functionNumber, void far *sendPacket,
			 void far *receivePacket);
Word 	PASCAL	NovellGetConnectionInfo( Word connectionNumber, Byte *objectName,
			      Word *objectType, Ulong *objectID,
			      Byte *loginTime);
Word    PASCAL  NovellGetBinderyAccessLevel( Byte *AccessLevel, Ulong *objectID);
Word 	PASCAL 	NovellGetServerInfo( ServerInfoRec *server );
Word 	PASCAL 	NovellGetServerStrings( Byte *name, Byte *revision,
				    Byte *date, Byte *copyright );
Word	PASCAL  NovellGetDirectoryHandle(Word dl);
Word    PASCAL  NovellGetDirectoryPath( Word dl, Byte *path );
Word 	PASCAL	NovellWorkStationRequest( Word regAX,
			 		  Word near *returnES,
			         	  Word near *returnSI );
Word 	PASCAL 	NovellGetFileServerName( Word dl, Byte *name );
Word            NovellFindFiles(char *fileName, char attributes,
                        NovellFindFileRequestRec *staticTable,
                        NovellFileInfo *infoBuffer);
Word            NovellGetFileInfo(char *fileName, char attributes,
                        NovellFileInfo *infoBuffer);
Word            NovellGetObjectName(long objectID, char name[48], Word *type);
Word            NovellGetObjectID(Word type, char *name, long *ID);
Word            NovellRenameDir(char *newName, char *oldName);
Boolean PASCAL	NovellGetID (Ulong *id, Boolean *supervisor);

WORD	PASCAL	NovellGetUserList (Byte byServer, USERNAMECALLBACK UserNameProc, DWORD dwParam);
Word	PASCAL	NovellEnumUserList (USERNAMECALLBACK UserNameProc, Ulong dwParam);
Word	PASCAL	NovellSetPreferredConnection (Byte byConn);
Byte	PASCAL	NovellGetPreferredConnection (void);
Word	PASCAL	NovellCheckOwner (Byte far *szFilename, Ulong dwUserID);
Word	PASCAL	NovellGetSpecificUserInfo (Ulong far *lpdwUserID,
				Byte far * szUser, Byte byDrive);
Word	PASCAL	NovellGetUserInfo (Byte byDrive, Ulong far *lpdwUserId,
				Boolean far *lpbSupervisor);
Word	PASCAL	NovellNameGetDevice (Byte far *szFilename, Byte far *szResult);
Word	PASCAL	NovellGetServer (Byte dl, Byte *name);

Byte far *PASCAL NovellGetDriveTable (void);
#endif

/*----------------------------------------------------------------------*
 * Function Key Bar Module						*
 *----------------------------------------------------------------------*/

VOID PASCAL KeyBarDisplay(
		BYTE	*pszKeyBar[]);  /* Pointer to keybar array      */

WORD PASCAL KeyBarGetFKey(
		WORD	wFlags,		/* Buttons flags                */
		WORD	wCol);          /* Column of the pressed key    */

VOID PASCAL KeyBarDestroy(VOID);
void PASCAL KeyBarPromote(void);

/*----------------------------------------------------------------------*
 * Pull-down Menu Module						*
 *----------------------------------------------------------------------*/

#ifdef INCL_PULLDOWN

WORD    PASCAL PulldownDrawBar(MenuBarRec *menuBar);
void    PASCAL PulldownHideBar (MenuBarRec *menuBar);
void    PASCAL PulldownDrawMenuTitle (MenuBarRec *menuBar, MenuRec *theMenu, int attr );
void    PASCAL PulldownDrawBox( int row1, int col1, int row2, int col2, RECT *savedRect )	;
void    PASCAL PulldownDropMenu (MenuBarRec *menuBar);
void    PASCAL PulldownMenuDimensions (MenuRec *theMenu);
void    PASCAL PulldownDrawItem (MenuRec *menu, int item, int highlight, int row );
Boolean PASCAL PulldownItemAlias (MenuBarRec *menuBar, int key, int *menu, int *item);
Boolean PASCAL PulldownMenuAlias (MenuBarRec *menuBar, int key, int *menu);
Boolean PASCAL PulldownKeySelect (MenuBarRec *menuBar, int *menu, int *item, int startMenu, int startItem);
int     PASCAL PulldownKeyLoop (MenuBarRec *menuBar, int key, int *menu, int *item);
int     PASCAL PulldownMenuDownLoop (MenuBarRec *menuBar, int *menu, int *item);
Boolean PASCAL PulldownItemHasHeir( MenuBarRec *menuBar, int menu, int item );
int     PASCAL PulldownShowHeirMenu( MenuBarRec *menuBar );
Boolean PASCAL PulldownMouseSelect (MenuBarRec *menuBar, int *menu, int *item);
void    PASCAL PulldownMouseMenuEvent (MenuBarRec *menuBar, int *menu, int *item);
int     PASCAL PulldownMenuID( MenuBarRec *menuBar, int menu, int item );
int     PASCAL PulldownEvent( MenuBarRec *menuBar, Word event );
void    PASCAL PulldownMoveVertical (MenuBarRec *menuBar, int item, int direction);
WORD    PASCAL PulldownSetBar (MenuBarRec *menuBar);
void    PASCAL PulldownCloseMenu (MenuBarRec *menuBar);
Boolean PASCAL PulldownMenuIsEmpty (MenuBarRec *menuBar);
int     PASCAL PulldownGetMenuHelpID (MenuBarRec *menuBar, int menu, int item);
void    PASCAL PulldownCheckItem (MenuBarRec *menuBar, int menu, int item, Boolean check);
void    PASCAL PulldownChangeItem(MenuBarRec *menuBar, int nMenu, int nItem, Byte *szItemStr);
void    PASCAL PulldownEnableItem (MenuBarRec *menuBar, int menu, int item, Boolean flag);
Boolean PASCAL PulldownSetMultiItemState(MenuBarRec *menuBar, int id, Boolean disabled, Boolean checked,Boolean bMulti );
Boolean PASCAL PulldownSetItemState(MenuBarRec *menuBar, int id, Boolean disabled, Boolean checked);
void    PASCAL PulldownHighlightMenu (MenuBarRec *menuBar, int menuNum);
void    PASCAL PulldownHighlightItem (MenuBarRec *menuBar, int item);
Boolean PASCAL PulldownMouseHelpRect (MenuBarRec *menuBar, RECT *rect);
void    PASCAL PulldownCheckHelp (MenuBarRec *menuBar);
Boolean PASCAL PulldownCloseBoxEvent (MenuBarRec *menuBar);
Boolean PASCAL PulldownCloseBoxEvent2 (MenuBarRec *menuBar);
void    PASCAL PulldownEnableMenu (MenuBarRec *pMenuBar, Boolean bEnable);
void	PASCAL PulldownShortDrawBar (MenuBarRec *menuBar);
int     PASCAL PulldownMapMouseToMenu( MenuBarRec *menuBar, int col);
void           PulldownWindowCallback(struct MWindowRec far *window, Word command);
int 	PASCAL PulldownKeyDropMenu (MenuBarRec *menuBar, int *menu, int *item, int key, int startItem);
int     PASCAL PulldownComputeFirstRow( MenuBarRec *menuBar );

#endif /* INCL_PULLDOWN */

/*----------------------------------------------------------------------*
 * Raw Directory Module							*
 *----------------------------------------------------------------------*/

#ifdef INCL_DIR

int 	PASCAL	RawDirRead(DiskRec *diskPtr, Word scn, Handle *phDir);
int 	PASCAL	RawDirWrite(DiskRec *diskPtr, Word scn, Handle hDir);
int 	PASCAL	RawDirReadPartial(DiskRec *diskPtr, Word scn, Handle *phDir);
int 	PASCAL	RawDirWritePartial(DiskRec *diskPtr, Word scn, Handle hDir);
void 	PASCAL	RawDirFree(Handle hDir);
Word    PASCAL  RawDirTrace(DiskRec *disk, Byte *fullPath, Word *dirSCN);
int 	PASCAL	RawDirInDirectory(Byte c, Byte *efile, Handle hDir, Boolean weird);
int 	PASCAL	RawDirIsEntry(DirEntryRec far *dirPtr, DiskRec *diskPtr);
int 	PASCAL	RawDirPrepareList(DirEntryRec huge **p, int cnt, int *countPtr, Word scn);
void	PASCAL	RawDirSwap(DirEntryRec huge *a, DirEntryRec huge *b);
Boolean PASCAL  RawDirIsSystemFile(Byte *filename);
Boolean PASCAL  RawDirIsSystemFileFar(Byte far *filename);
int		RawDirMoveTree(Byte *source, Byte *dest);
DirEntryRec far * PASCAL RawDirLock(Handle hDir, Word *pwCount);
void	PASCAL	RawDirUnlock(Handle hDir);

#ifndef _INCL_NFAT_
Boolean PASCAL 	RawDirSearch(DiskRec *disk, Word dirScn,
			     RawDirSearchKeyRec *search,
			     DirEntryRec *dirPtr);
#endif /* _INCL_NFAT */

#endif /* INCL_DIR */

/*----------------------------------------------------------------------*
 * Rectangular Coordinates Module 					*
 *----------------------------------------------------------------------*/

#ifdef INCL_DRAW

void	PASCAL	RectSet(RECT *rect, Word top, Word left, Word bottom, Word right);
void    PASCAL  RectSetFar(RECT far *rRect, Word wTop, Word wLeft,
                           Word wBottom, Word wRight);
Boolean PASCAL	RectPointIn(RECT *rect, Word row, Word col);
Boolean PASCAL  RectPointInFar(RECT far *rRect, Word wRow, Word wCol);

#endif

/*----------------------------------------------------------------------*
 * Registers Module 							*
 *----------------------------------------------------------------------*/

Word    CDECL	RegisterGetDS(void);
Word    CDECL	RegisterGetES(void);
Word    CDECL	RegisterGetCS(void);
Word    CDECL	RegisterSetES(Word value);
Word    CDECL	RegisterSetDS(Word value);

/*----------------------------------------------------------------------*
 * Report Module 							*
 *----------------------------------------------------------------------*/

extern FileHandle NEAR reportHandle;

#define ReportInit(h)	(reportHandle = h)
#define ReportDone()	/* reserved for future use */

Boolean PASCAL	ReportVerifyOutput(Byte *pOutputName);
Boolean		ReportPrint(Byte *format, ...);
Boolean		ReportPrintCentered(Byte *format, ...);
Boolean PASCAL	ReportString(Byte *s);
Boolean PASCAL	ReportNewLine(void);
Boolean PASCAL	ReportNewPage(void);
Boolean PASCAL	ReportCenteredString(Byte *string);
Boolean PASCAL	ReportErrorTitle(Byte *string);
Boolean PASCAL	ReportDividingLine(void);
Boolean PASCAL	ReportBoxedTitle(Byte *title);
Boolean PASCAL	ReportCheckPrinter(Byte *pOutputName);
Boolean PASCAL	ReportTitle(Byte *title);
Boolean PASCAL	ReportHeader(Byte *progTitle, Byte *fullVer, Byte *reportTitle);
void PASCAL	DateAndTimeString(Byte *string);

/*----------------------------------------------------------------------*
 * String Manipulation Module						*
 *----------------------------------------------------------------------*/

#define STREMPTY(szStr)    (*(szStr)==0)
void 	PASCAL 	StringReturnFormattedFileName(Byte *name1, Byte *formattedStr, Word width);
int 	PASCAL 	StringSubCompare(Byte *s1, Byte *s2, Boolean matchCase);
int  	PASCAL 	StringCompare(Byte *s1, Byte *s2, Boolean matchCase);
int  	PASCAL 	StringCompareFar(Byte far *s1, Byte far *s2, Boolean matchCase);
Byte * 	PASCAL 	StringSearch(Byte *buffer, Byte *key, int bufferSize);
Byte *	PASCAL	StringScan(Byte *pszString, Byte byValue);
Word 	PASCAL 	StringLength(Byte *s);
Byte *  PASCAL  StringGetEnd(Byte *s);
Word    PASCAL  StringWithHotkeyLength(Byte *string);
Word 	PASCAL 	StringFarLength(Byte huge *s);
void 	PASCAL 	StringCopy(Byte *d, Byte *s);
Byte far * PASCAL  StringCopyFar(Byte far *d, Byte far *s);
void    PASCAL  StringCopyDigitsOnly( Byte *szDest, Byte *szSrc );
int  	PASCAL 	StringCmp(Byte *d, Byte *s);
int     PASCAL  StringNCmp(Byte *d1, Byte *s1, Word max);
int  	PASCAL 	StringFarCmp(Byte far *d1, Byte far *s1);
int  	PASCAL 	StringFarNCmp(Byte far *d1, Byte far *s1, Word max);
int  	PASCAL 	StringFarNICmp(Byte far * szString1, Byte far * szString2, Word wMaxCompare);
int	PASCAL	StringIntlCmp(Byte *pszString1, Byte *pszString2);
int	PASCAL	StringIntlFarCmp(Byte far *lpszString1, Byte far *lpszString2);
void 	PASCAL 	StringNCat(Byte *d, Byte *s, int n);
Byte    *PASCAL StringNCopy(Byte *string1, Byte *string2, int max);
Byte far *PASCAL StringNCopyFar(Byte far *string1, Byte far *string2, int max);
Word 	PASCAL 	StringNLength(Byte *s, int n);
#define StringNAppend(dest,source,max)  (StringNCat(dest,source,max))
Byte *	PASCAL  StringAppend(Byte *d, Byte *s);
Byte *	PASCAL  StringAppendChar(Byte *d, Byte ch);
void 	PASCAL 	StringToLower(Byte *s);
void 	PASCAL 	StringToLowerFar(Byte far *s);
void 	PASCAL 	StringToUpper(Byte *s);
void 	PASCAL 	StringToUpperFar(Byte far *s);
void 	PASCAL 	StringStripSpaces(Byte *s);
Byte *	PASCAL  StringCheckPlural(int num, Byte *strings[]);
Word 	PASCAL 	StringLengthFar(Byte far *string);
Boolean	PASCAL	StringMatchFar(Byte *string1, Byte far *string2);
Byte far *PASCAL StringAppendFar(Byte far *destination, Byte far *source);
Byte *	PASCAL  StringLocalize(Byte *nearString, Byte far *farString);
#define StringNLocalize(dest,source,max) (StringNCopyFar((Byte far *)(dest),source,max))
Byte *	PASCAL  StringPad(Byte *string, signed int number, Byte ch);
Byte *	        StringPrint(Byte *buffer, Byte *formatString, ...);
Byte    PASCAL  StringFirstUpperChar(Byte *string);
Byte *  PASCAL	StringOrdinalSuffix(Word i);
Byte * 	PASCAL 	InternalStringPrint(Byte *buffer, Byte *s, Byte *args);

Byte far * FAR 	StringMatchLast(const Byte far *lpString1, int nChar);
Word FAR	StringFindSub(const Byte far *lpString1, const Byte far *lpString2);
Byte far * FAR	StringFindFirstChar(const Byte far *lpString1, const Byte far *lpString2);

Byte    PASCAL  StringGetHotkey(Byte *string);
VOID FAR PASCAL StringPrintFileName(BYTE *pszName, BYTE *pszBuffer, WORD wWidth);
void    PASCAL  StringRemoveWhiteSpace(Byte *output, Byte *input);
void	PASCAL	StringStripBuildNumber(Byte *pszOutBuffer, Byte *pszVersion);
void    PASCAL  BufferXOR(Byte *pBuffer, Word wLen, Byte byMask);

#ifndef _INCL_NPRM
void	PASCAL	StringCompress(Byte *compressedStr, Byte *string);
void	PASCAL	StringUncompress(Byte far *string, Byte far *compressedStr);
Word	PASCAL	StringMaxLength(Byte **s);
Byte *	PASCAL  StringSkipWhite(Byte *s);
Byte *	PASCAL  StringSkipToWhite(Byte *s);
LPSTR   PASCAL  StringSkipIniDelim(LPSTR lpString);
LPSTR   PASCAL  StringSkipToIniDelim(LPSTR lpString);
Boolean PASCAL  CharIsIniDelim(Byte ch);
Byte *	PASCAL  StringReverse(Byte *string);
#endif

/*----------------------------------------------------------------------*
 * Text Edit Filters							*
 *----------------------------------------------------------------------*/

#ifdef INCL_DIALOG

Word TEDigitsOnlyFilter(TERec *te, Word *c);
Word TELowerFileCharsOnlyFilter(TERec *te, Word *c);
Word TEUpperFileCharsOnlyFilter(TERec *te, Word *c);
Word TEUpperPathCharsOnlyFilter(TERec *te, Word *c);
Word TEUpperFileNAMECharsFilter(TERec *te, Word *c);
Word TEHexDigitsOnlyFilter(TERec *te, Word *c);
Word TEUpperCaseOnlyConvertFilter(TERec *te, Word *c);
Word TEUpperFilePatCharsOnlyFilter(TERec *te, Word *c);
Word TELowerFilePatCharsOnlyFilter(TERec *te, Word *c);
Word TEUpperPathPatCharsOnlyFilter(TERec *te, Word *c);
Word TELowerPathCharsOnlyFilter (TERec *te, Word *c);
Word TELowerPathPatCharsOnlyFilter(TERec *te, Word *c);
Word TENoLeadingSpaceFilter(TERec *te, Word *c);

#endif

/*----------------------------------------------------------------------*
 * Timer Module								*
 *----------------------------------------------------------------------*/

void	PASCAL	TimerDelay(Word duration);
Ulong		TimerTicks(void);
Ulong   PASCAL	TimerElapsedTicks(Ulong start);
Ulong	CDECL	Clock(Byte command);
void 	PASCAL	TimerDelayBegin(Ulong *startTimePtr);
void 	PASCAL	TimerDelayEnd(Word seconds, Ulong startTime);

/*----------------------------------------------------------------------*
 * TSR Control Module							*
 *----------------------------------------------------------------------*/

Word    CDECL	TSRInstalled( Byte *id );
Word    CDECL	TSRGetPSP( Byte *id );
Boolean	CDECL	TSRIsLoaded( Byte *id );
Boolean CDECL	TSRIsEnabled( Byte *id );
Boolean CDECL	TSREnable( Byte *id );
Boolean CDECL	TSRDisable( Byte *id );
Boolean CDECL	TSRSuspend( Byte *id );  /* Use only with NCACHE! */
Boolean CDECL	TSRUpdateConfig( Byte *id, void far *dataPtr );
Boolean CDECL	TSRIs2FSet(void);
Boolean CDECL	TSRWinInEnhancedMode(void);
Boolean CDECL	TSRWin286Active(void);
Boolean CDECL	TSRWin386Active(void);
Boolean CDECL	TSRWin31Active(void);
Word	CDECL	TSRMultiTaskEnvironment(void);
void far *CDECL TSRGetDataPtr( Byte *id );
void far *CDECL TSRGetCtrlPtr( Byte *id );
Boolean CDECL	TSRIsPCAnywhereInstalled(void);
Boolean CDECL	TSRIsPCAnywhereInHostMode(void);
void    CDECL   TSRLaunchSaver (void);

/*----------------------------------------------------------------------*
 * Video BIOS Module							*
 *----------------------------------------------------------------------*/

void PASCAL VideoClearWindow (Word row1, Word col1, Word row2, Word col2);
void PASCAL VideoScrollUp    (Word row1, Word col1, Word row2, Word col2, Word n);
void PASCAL VideoScrollDown  (Word row1, Word col1, Word row2, Word col2, Word n);
void	VideoSetMode (Word mode);
void	VideoSetBlink (Word highlightOrBlink);
void	VideoSetBorder (Word border);
void	VideoSetRows (Byte rows);
Word	VideoGetMode (void);
Word	VideoGetRows (void);
Word	VideoGetCharHeight(void);
Word	VideoGetColumns (void);
void	VideoGetPalette ( Byte *v );
void	VideoSetPalette ( Byte *v );
void    VideoSetColorRegister(Byte registerNum, Byte value);
Byte    VideoGetColorRegister(Byte registerNum);
Word	VideoGetType (void);
Word    VideoGetPageOffset( void );
void	VideoSetEGAEmulation (void);
void	VideoRestoreEGAEmulation (void);
void	VideoClearScreen (void);
void	VideoClearMonoScreen (void);
void    VideoInitColors (Byte defaultSet);
Boolean VideoGetStateInfo(Word tableSeg, Word tableOff );
Boolean PASCAL VideoIsUltravision(void);
Boolean PASCAL VideoIsAX2(void);
BOOL    PASCAL VideoIsAnsiInstalled(void);
WORD    VideoPageGetSize( VOID );
LPVOID  VideoPageCalcAddress( WORD wPage );
BOOL    VideoPageCopy( WORD wPageDest, WORD wPageSrc );

// Please read the library source code before using Video DAC functions!
void	VideoGetDAC(Word nDACReg, Byte *pbyRed, Byte *pbyGreen, Byte *pbyBlue);
void	VideoSetDAC(Word nDACReg, Byte byRed, Byte byGreen, Byte byBlue);
void	VideoSaveDAC(Byte *pSaveBuf);
void	VideoRestoreDAC(Byte *pSaveBuf);
void	VideoFadeOut(Byte *pWorkBuf, Word nFadeUnit);
void	VideoFadeIn(Byte *pSaveBuf, Byte *pWorkBuf, Word nFadeUnit);

/*----------------------------------------------------------------------*
 * mini video routines that don't need a lot of library overhead	*									*
 *----------------------------------------------------------------------*/
void  	VidSetRowCol(Word wRow, Word wCol);
Byte  	VidReadAttr (void);
Byte  	VidReadChar (void);
void  	VidWriteNChars (Byte byCharToUse, Word wCount);
void  	VidWriteCRLF(void);
void  	VidWriteChar(Byte bCharacter);
void  	VidSetCursRowCol( Word row, Word col);
void  	VidGetCursRowCol( int  * row, int  * col);
void  	VidBlankScrnArea(Byte row, Byte col, Word width, Word height,Byte Attr);
void  	VidClsWithAttrib(Byte Attrib);

/*----------------------------------------------------------------------*
 * Vines (Banyan) Module                                                *
 *----------------------------------------------------------------------*/

#ifdef INCL_VINES

Boolean	CDECL	VinesCancelAssignment( Byte *pszDosName );
Word	CDECL	VinesGetAccessRights( VnsAccessCtrlBlk *pAccessCtrlBlk );
Boolean	CDECL	VinesGetAssignListEntry( Word wIndex, Byte *pszDosName,
					 Byte *pszNetName, Byte *pcDevType );
Boolean	CDECL	VinesGetAssignment( Word wFlag, Byte *pszDosName,
				    Byte *pszNetName, Byte *pcDevType );
Boolean	CDECL	VinesGetErrorString( Word wErrorCode, Byte *pszBuffer );
Byte	CDECL	VinesGetInterruptNumber( void );
Boolean	CDECL	VinesGetUserName( Byte *pszBuffer );
Word	CDECL	VinesGetVersion( void );
Boolean	CDECL	VinesIsInstalled( void );
Boolean	CDECL	VinesPollMessage( Byte *szBuffer );
VOID	CDECL	VinesReleaseMessages( void );
Boolean	CDECL	VinesServiceIsInstalled( Byte byService );
Word	CDECL	VinesSetAssignment( Byte *pszDosName, Byte *pszNetName,
				    Byte cDevType );
Word	CDECL	VinesStEnumNames( VnsEnumCtrlBlk *pEnumCtrlBlk );
Boolean	CDECL	VinesStIsInstalled( void );
Word	CDECL	VinesStNiceName( VnsNiceNameBlk *pNiceNameBlk );
Word	CDECL	VinesTrapMessages( void );

#endif /* INCL_VINES */

/*----------------------------------------------------------------------*
 * Window Module							*
 *----------------------------------------------------------------------*/

Byte far * CDECL WindowSave(Byte far *buffer, Word top, Word left, Word bottom, Word right);
Byte far * CDECL WindowRestore(Byte far *buffer, Word top, Word left, Word bottom, Word right);

void	WindowSetAttr (Byte row, Byte col, Word width, Word height, Byte attr);

Boolean PASCAL WindowPush (Word top, Word left, Word bottom, Word right, Boolean shadow);
void    PASCAL WindowPop (Word top, Word left, Word bottom, Word right);

/*----------------------------------------------------------------------*
 * Advise Module							*
 *----------------------------------------------------------------------*/

void AdviseSearch (Byte *fileBuffer, Word bufferSize, void (*launchProc)(Byte *command));
void AdviseApplErrors (Byte *fileBuffer, Word bufferSize, void (*launchProc)(Byte *command));
void AdviseDiskProblems (Byte *fileBuffer, Word bufferSize, void (*launchProc)(Byte *command));
void AdviseDOSErrors (Byte *fileBuffer, Word bufferSize, void (*launchProc)(Byte *command));
void AdviseCHKDSKErrors (Byte *fileBuffer, Word bufferSize, void (*launchProc)(Byte *command));

/*----------------------------------------------------------------------*
 * Scheduler section                                                    *
 *----------------------------------------------------------------------*/

Word    PASCAL  StdDlgSchedule ( void ) ;

/* Function definitions in zipvdisk.c */

/*----------------------------------------------------------------------*
 * ZIP Module								*
 *----------------------------------------------------------------------*/

#ifdef	INCL_VDISKREC
LPVDISKREC PASCAL SpecifyZip(Byte *filename);
void  PASCAL CloseZip  (LPVDISKREC lpVDisk);
void PASCAL ZipGetName(LPVDISKREC lpVDisk, Byte *lpName);
void PASCAL ZipGetFullName(LPVDISKREC lpVDisk, Byte *lpName);
Word PASCAL ZipAddEntry	   (VDISKREC *pVDisk, void far *pFListIn, LPBYTE lpPath);
void PASCAL ZipDeleteEntry   (VDISKREC *pVDisk, char *pPath);
#endif

        // THE FOLLOWING DIALOGS ARE NOT FOR PUBLIC APPLICATIONS

void PASCAL StdDlgColorSet (void);
void PASCAL StdDlgCharacterTable (void);

#endif /* INCL_LINT */
