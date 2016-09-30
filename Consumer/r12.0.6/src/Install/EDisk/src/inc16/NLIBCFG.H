/* $Header:   I:/INCLUDE/VCS/NLIBCFG.H_V   1.14   02 Dec 1992 12:17:18   CRAIG  $ */

/*----------------------------------------------------------------------*
 * NLIBCFG.H								*
 *									*
 * Structure used to store Norton Library configuration in NORTON.INI	*
 *									*
 * This file was split off from NUCONFIG.H				*
 *									*
 * Copyright 1991 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   I:/INCLUDE/VCS/NLIBCFG.H_V  $
 * 
 *    Rev 1.14   02 Dec 1992 12:17:18   CRAIG
 * Removed version bytes; Quake doesn't use them.
 *
 *    Rev 1.13   02 Dec 1992 11:44:42   CRAIG
 * Changes for new Config Manager.
 *
 *    Rev 1.12   30 Mar 1992 22:44:02   PETERD
 * Added blockCursor to nlib configuration
 *
 *    Rev 1.11   27 Jan 1992 22:36:48   TORSTEN
 * Changed the #include statements to use angle brackets.
 *
 *    Rev 1.10   19 Jan 1992 19:18:18   HENRI
 * Modified mouse click #defines.
 *
 *    Rev 1.9   14 Jan 1992 18:52:40   JOHN
 * Added standardMouseCursor
 *
 *    Rev 1.8   12 Nov 1991 11:12:54   PETERD
 * Added TempDirs and NLibConfigWrite()
 *
 *    Rev 1.7   08 Nov 1991 10:41:44   PETERD
 * Changed double-click values for mouse.
 *
 *    Rev 1.6   29 Oct 1991 16:35:52   PETERD
 * Added product variable name to NLibConfigOpen.
 *
 *    Rev 1.5   01 Oct 1991 18:05:08   PETERD
 * Added default mouse values.
 *
 *    Rev 1.4   20 Sep 1991 15:49:38   PETERD
 * Added displayLines and mouse features.
 *
 *    Rev 1.3   04 Sep 1991 13:45:22   MARKK
 *
 *    Rev 1.2   10 May 1991 14:33:32   PETERD
 * Removed the intenseBackground field (created a "reserved" byte) since
 * it is no longer used.
 *
 *    Rev 1.1   23 Apr 1991 00:16:16   JOHN
 * Added <altColorBaseSet> field.  This field stores the color set on
 * which the custom colors are based.  It is used in NUCONFIG when the
 * user wants to go back to the default attribute.
 *
 *    Rev 1.0   26 Mar 1991 15:12:14   JOHN
 * Initial revision.
 */

#ifndef INCL_NLIBCFG
#define	INCL_NLIBCFG

#ifndef INCL_DRAW
#include <draw.h>
#endif

#ifndef INCL_INIFILE
#include <inifile.h>
#endif

/*----------------------------------------------------------------------*
 * Format of NLibConfigRec						*
 *									*
 * If you make changes here, be sure to update NLibConfigReset().	*
 *----------------------------------------------------------------------*/

typedef struct
    {
    Byte	colorSet;		/* See VIDEO\COLORS.C		*/
    Byte	graphicsLevel;		/* See GMODE_... in DRAW.H	*/
    ColorSet	altColorSet;
    Boolean	leftHandedMouse;
    Boolean	fastMouseReset;
    Boolean	noZoomDialogs;
    Boolean	plainDeskTop;
    Boolean	ctrlEnterAccepts;	/* Does ^Enter accept dialog?	   */
    Boolean	arrowsOnButtons;	/* Do buttons have arrow pointers? */
    Byte	displayLines;      	/* Number of display lines         */
    Byte	altColorBaseSet;

    Byte	mouseAcceleration;	// 0 - 100
    Byte	mouseSensitivity;	// 0 - 100
    Byte	mouseDoubleClicks;	// tenth seconds
    Boolean	standardMouseCursor;	// Use standard mouse driver cursor

    Boolean	blockCursor;		// Use a block cursor

					/* When you add fields to this	*/
					/* record, take the space from	*/
					/* the reserved bytes, so that	*/
					/* a recompile by everyone is	*/
					/* not needed immediately.	*/
    Byte        reserved[ 32
			- sizeof(Byte)	 // altColorBaseSet
			- sizeof(Byte)	 // mouseAcceleration
			- sizeof(Byte)	 // mouseSensitivity
			- sizeof(Byte)	 // mouseDoubleClicks
			- sizeof(Boolean) // standardMouseCursor
			- sizeof(Boolean) // blockCursor
			];
    } NLibConfigRec;


/*----------------------------------------------------------------------*
 * The following paths are for directories used for temporary entities	*
 *----------------------------------------------------------------------*/

typedef struct NLibTempDirsRec
    {
    Byte	szVMMSwapDir[MAX_PATH_NAME];
    Byte	szVMMSaveDir[MAX_PATH_NAME];
    Byte	szOvlSwapDir[MAX_PATH_NAME];
    Byte	szTempDir[MAX_PATH_NAME];
    Byte	szResveredPath[MAX_PATH_NAME];
    Byte	byReserved[32];
    } NLibTempDirsRec;

/*----------------------------------------------------------------------*
 * Standard values							*
 *----------------------------------------------------------------------*/

#define	MOUSE_DEFAULT_VALUE	255

#define	MOUSE_NO_ACCEL		100
#define	MOUSE_MEDIUM_ACCEL	50
#define	MOUSE_FAST_ACCEL	10

#define	MOUSE_SLOW		20
#define	MOUSE_MEDIUM		50
#define	MOUSE_FAST		80

#define	MOUSE_SLOW_CLICK	15
#define	MOUSE_MEDIUM_CLICK	9
#define	MOUSE_FAST_CLICK	5

/*----------------------------------------------------------------------*
 * External data declarations						*
 *----------------------------------------------------------------------*/

extern NLibConfigRec near nlibConfig;		/* Library configuration*/
extern NLibTempDirsRec near nlibTempDirs;	/* Library configuration*/

extern Byte  nortonIniFileSTR[];	/* NORTON.INI file name		*/

/*----------------------------------------------------------------------*
 * Function prototypes							*
 *----------------------------------------------------------------------*/

int PASCAL NLibConfigRead(void);
Word PASCAL NLibConfigWrite(void);
void PASCAL NLibConfigReset(void);
void PASCAL NLibConfigResetPaths(void);
void PASCAL NLibFastInit(void);
void PASCAL NLibFastRestore(void);

#endif
