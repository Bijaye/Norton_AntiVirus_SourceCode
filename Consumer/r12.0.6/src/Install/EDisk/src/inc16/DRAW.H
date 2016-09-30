/* $Header:   S:/INCLUDE/VCS/DRAW.H_V   1.20   01 Mar 1992 18:25:24   JOHN  $ */

/*----------------------------------------------------------------------*
 * This file contains type definitions for high level display objects,	*
 * including dialog boxes and their controls but excluding pull-downs.	*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/INCLUDE/VCS/DRAW.H_V  $
 * 
 *    Rev 1.20   01 Mar 1992 18:25:24   JOHN
 * Updated for new graphic font classification system
 * 
 *    Rev 1.19   27 Jan 1992 22:36:10   TORSTEN
 * Changed the #include statements to use angle brackets.
 * 
 *    Rev 1.18   14 Jan 1992 18:51:08   JOHN
 * Added graphicMouseMode and associated macros
 * 
 *    Rev 1.17   14 Jan 1992 15:24:28   PETERD
 * Added one-line scroll bar icon
 * 
 *    Rev 1.16   10 Jan 1992 17:38:56   JOHN
 * Added "plusMinus" to character table record.
 * 
 *    Rev 1.15   26 Nov 1991 11:36:30   ED
 * Added ICON_CDROM
 * 
 *    Rev 1.14   29 Oct 1991 16:36:36   PETERD
 * Added left and right brackets ([]) to char table.
 * 
 *    Rev 1.13   22 Oct 1991 18:16:42   PETERD
 * Added saveShape to the font record.  This was needed because we now
 * have the ability to change between 14 and 16 pixel character fonts on
 * VGA, so we can't go changing the actual font image.
 * 
 *    Rev 1.12   04 Sep 1991 13:44:54   MARKK
 * 
 *    Rev 1.11   13 Aug 1991 11:20:24   HENRI
 * Added new definitions for ICON_FILE, ICON_EXE, ICON_RESIZE.
 * 
 *    Rev 1.10   25 Jul 1991 12:23:20   ED
 * Added threeStateIcon and threeStateIconG0
 * 
 *    Rev 1.9   10 Jul 1991 09:10:12   MARKK
 * Added VideoGetMaxCols
 * 
 *    Rev 1.8   03 Jul 1991 10:30:54   MARKK
 * Converted to large model.
 * 
 *    Rev 1.7   19 Jun 1991 19:59:50   HENRI
 * Fixed the ICON_MAXIMIZE definition.
 * 
 *    Rev 1.6   19 Jun 1991 19:31:50   HENRI
 * Added the ICON_MAXIMIZE and ICON_MINIMIZE definitions.
 * 
 *    Rev 1.5   26 Apr 1991 18:08:04   JOHN
 * Added <deleteLines1> and <deleteLines2> to FontChar structure.
 * These are used to delete scan lines from the font on EGA displays.
 * 
 *    Rev 1.4   30 Mar 1991 17:50:20   PETERD
 * [Peter] Added videoBlinkState variable.
 * [John]  Added define's for scroll bar colors.
 * 
 *    Rev 1.3   26 Mar 1991 11:45:54   HENRI
 * Added the "FastIsFullGraphicsMode()" macro.
 * Added the GMODE_FULL_GRAPHICS definition.
 * Added two characters to the CHAR_TABLE_RECORD.
 * 
 *    Rev 1.2   01 Mar 1991 15:39:08   BARRY
 * Removed keyBarColors from ColorSet, didn't know that it would break
 * config stuff.
 * 
 *    Rev 1.1   28 Feb 1991 19:29:46   BARRY
 * Added keyBarColors to ColorSet.
 * 
 *    Rev 1.0   29 Aug 1990 11:24:30   DAVID
 * Initial revision.
 */

#ifndef INCL_DRAW
#define INCL_DRAW

#ifndef INCL_TYPES
#include <types.h>
#endif

/*----------------------------------------------------------------------*
 * "OLD" Font redefinition modes					*
 *----------------------------------------------------------------------*/

#define GMODE_NO_GRAPHICS	0	// No redefs
#define GMODE_NO_MOUSE		1	// Only controls
#define GMODE_DEFINE_ALL	2	// Controls and mouse
#define GMODE_FULL_GRAPHICS	3	// Full graphical interface

#if 0
// These are now obsolete
extern Byte near graphicCharMode;
#define FastIsFullGraphicsMode()  (graphicCharMode==GMODE_FULL_GRAPHICS)
#endif

/*----------------------------------------------------------------------*
 * New font redefinition modes						*
 *----------------------------------------------------------------------*/

extern Boolean near bBasicFontsRedefined;
extern Boolean near bGraphicMouse;
extern Boolean near bGraphicControls;
extern Boolean near bGraphicMinMax;
extern Boolean near bGraphicWindows;
extern Boolean near bGraphicScrollBars;
extern Boolean near bGraphicIcons;

#define MouseIsGraphic()	(bGraphicMouse==TRUE)
#define MouseIsStandard()	(bGraphicMouse==FALSE)

/*----------------------------------------------------------------------*
 * This structure describes how to put patterns together to build	*
 * boxes.								*
 *----------------------------------------------------------------------*/

struct PATTERN {
	char c;
	char count;
	};
typedef struct PATTERN PATTERN;

struct BOXES
	{
	PATTERN *pattern;	/* Pointer to a pattern.		*/
	int  repeat;		/* How many lines of this pattern	*/
	};
typedef struct BOXES BOXES;

/*----------------------------------------------------------------------*
 * The following structure is used to define a color set.  Color sets	*
 * are used to define all the attributes for pop-up windows, and for	*
 * other parts of the screen.						*
 *----------------------------------------------------------------------*/

struct COLORS
	{
	char	normal;
	char	inverse;
	char	bright;
	char	bright_inverse;
        char	disabled;
	char	active_title;
	char	inactive_title;
	char	close_box;
	};
typedef struct COLORS COLORS;

/*----------------------------------------------------------------------*
 * This structure is a complete set of COLORS for a given display 	*
 * type/mode.  An array of these structures is defined in COLORS.C.	*
 * The COLOR variables used by the program are initialized by   	*
 * copying values from the appropriate array entry.			*
 *----------------------------------------------------------------------*/

typedef struct
    {
    COLORS	panelColors;		/* Colors in the background	*/
    COLORS	menuBarColors;		/* Colors in pull-down menu bar	*/
    COLORS	menuColors;		/* Colors in pull-down menus	*/
    COLORS	dialogColors;		/* Colors in primary dialogs	*/
    COLORS	listBoxColors;		/* Colors in secondary dialogs	*/
    COLORS	alertColors;		/* Colors in alert boxes	*/
    COLORS	helpColors;		/* Colors in help boxes		*/
    COLORS	extraColors;		/* Extra Colors for anything	*/
    } ColorSet;

extern Byte  	charAttribute;
extern Byte  	clearAttr;

extern int near	normal_attr;
extern int near inverse_attr;
extern int near bright_attr;
extern int near bright_inverse_attr;
extern int near disabled_attr;
extern int near	active_title_attr;
extern int near	inactive_title_attr;
extern int near	close_box_attr;

#define NORMAL		normal_attr	/* Values for attrName	*/
#define INVERSE		inverse_attr
#define	REVERSE		INVERSE
#define BRIGHT		bright_attr
#define BOLD		BRIGHT
#define BRIGHT_INVERSE	bright_inverse_attr
#define GREY		disabled_attr
#define ACTIVE_TITLE	active_title_attr
#define INACTIVE_TITLE	inactive_title_attr
#define CLOSE_BOX	close_box_attr

#define	SCROLL_ARROW_ATTR	extra_colors.disabled
#define	SCROLL_THUMB_ATTR	extra_colors.disabled
#define	SCROLL_BAR_ATTR		extra_colors.close_box

#define ColorGetOldAttr()		(originalAttr)
#define ColorSetOldAttr(value)		(originalAttr = (Byte) (value))
#define ColorSetAttr(attrName)		(charAttribute = (Byte) (attrName))
#define ColorGetAttr()			(charAttribute)
#define ColorGetAttrValue(attrName)	(attrName)
#define ColorSetClearAttr(attrName)	(clearAttr = (Byte) (attrName))
#define ColorGetClearAttr()	        (clearAttr)
#define ColorGetCurrentSet()		(colorSet)

extern Byte near videoBlinkState;

extern Word near displayLines;
extern Word near displayColumns;

#define VideoGetMaxRows()		(displayLines)
#define VideoGetMaxCols()               (displayColumns-1)

/*----------------------------------------------------------------------*
 * Used in FastPrintJustified()						*
 *----------------------------------------------------------------------*/

#define	CENTER 		0
#define RIGHT		1

/*----------------------------------------------------------------------*
 * Here is the definition for a rectangle.  I use rectangles mainly	*
 * for mouse support.							*
 *----------------------------------------------------------------------*/

struct RECT
	{
	Byte top;
	Byte left;
	Byte bottom;
	Byte right;
	};
typedef struct RECT RECT;
typedef RECT RectRec;

/*----------------------------------------------------------------------*
 * This structure is used for maintaining the dimensions (height and    *
 * width) of anything such as controls, dialogs, etc.			*
 *----------------------------------------------------------------------*/

typedef struct
    {
    Byte height;
    Byte width;
    } DimensionRec;

/*----------------------------------------------------------------------*
 * These are the definitions for graphics characters that I used to	*
 * make boxes, etc.							*
 *----------------------------------------------------------------------*/

#define		UPPER_LEFT		201		/* É		*/
#define		UPPER_T			209		/*   Ñ		*/
#define		UPPER_MIDDLE		203		/* Ë		*/
#define		UPPER_RIGHT		187		/*   »		*/
#define		LOWER_LEFT		200		/* È		*/
#define		LOWER_T			207		/*   Ï		*/
#define		LOWER_MIDDLE		202		/* Ê		*/
#define		LOWER_RIGHT		188		/*   ¼		*/
#define		HORIZONTAL		205		/* Í		*/
#define		VERTICAL		186		/*   º		*/
#define		LEFT_T			199		/* Ç		*/
#define		LEFT_MIDDLE		204		/*   Ì		*/
#define		RIGHT_T			182		/* ¶		*/
#define		RIGHT_MIDDLE		185		/*   ¹		*/

#define		THIN_VERTICAL		179		/* ³		*/
#define		THIN_HORIZONTAL		196		/*   Ä		*/
#define		THIN_LOWER_T		193		/* Á		*/
#define		THIN_UPPER_T		194		/*   Â		*/
#define		THIN_LEFT_T		195		/* Ã		*/
#define		THIN_RIGHT_T		180		/*   ´		*/
#define		THIN_UPPER_LEFT		218		/* Ú		*/
#define		THIN_UPPER_RIGHT	191		/*   ¿		*/
#define		THIN_LOWER_LEFT		192		/* À		*/
#define		THIN_LOWER_RIGHT	217		/*   Ù		*/

#define		UPPER_RIGHT_MIXED	183		/* ·		*/
#define		RIGHT_UPPER_MIXED	184		/*   ¸		*/
#define		LOWER_RIGHT_MIXED	189		/* ½		*/
#define		RIGHT_LOWER_MIXED	190		/*   ¾		*/

#define		UPPER_LEFT_MIXED	214		/* Ö		*/
#define		LEFT_UPPER_MIXED	213		/*   Õ		*/
#define		LOWER_LEFT_MIXED	211		/* Ó		*/
#define		LEFT_LOWER_MIXED	212		/*   Ô		*/

/*----------------------------------------------------------------------*
 * This record contains the mouse icons.				*
 *----------------------------------------------------------------------*/

typedef struct
        {
  	Byte normal;
	Byte check;
	Byte cancel;
	Byte up;
	Byte down;
	Byte left;
	Byte right;
	} MouseIconRec;

/*----------------------------------------------------------------------*
 * This structure is used for Check Boxes, Radio Buttons, and Buttons   *
 * indicating the left side of the control, the right and the icon      *
 * indicating on.  Example:  '(' = left, ')' = right, 'o' = on          *
 *----------------------------------------------------------------------*/

typedef struct
        {
	Byte left;
	Byte right;
	Byte on;
	Byte off;
	} ControlIconRec;

/*----------------------------------------------------------------------*
 * This structure is used by scroll bars for determining its icons	*
 *----------------------------------------------------------------------*/
typedef struct
        {
	Byte upArrow;
	Byte downArrow;
	Byte filler;
	Byte pointer;
	} ScrollBarIconRec;

/*--------------------------------------------------------------------------*/
/* Character table for printing equivalent characters for other video modes */
/* Add new variables to end of table to prevent record clashes with other   */
/* object files which use this structure.				    */
/*--------------------------------------------------------------------------*/

typedef struct
    {
    Byte  singleVertical;
    Byte  singleHorizontal;
    Byte  singleLowerT;
    Byte  singleUpperT;
    Byte  singleLeftT;
    Byte  singleRightT;
    Byte  singleUpperLeft;
    Byte  singleUpperRight;
    Byte  singleLowerLeft;
    Byte  singleLowerRight;
    Byte  singleCross;
    Byte  singleVerticalCross;

    Byte  doubleUpperLeft;
    Byte  doubleUpperT;
    Byte  doubleUpperMiddle;
    Byte  doubleUpperRight;
    Byte  doubleLowerLeft;
    Byte  doubleLowerT;
    Byte  doubleLowerMiddle;
    Byte  doubleLowerRight;
    Byte  doubleHorizontal;
    Byte  doubleVertical;
    Byte  doubleLeftT;
    Byte  doubleLeftMiddle;
    Byte  doubleRightT;
    Byte  doubleRightMiddle;
    Byte  doubleCross;
    Byte  doubleVerticalCross;

    Byte  smallRectangle;
    Byte  fullShadedBlock;
    Byte  leftPointTriangle;
    Byte  rightPointTriangle;
    Byte  upPointTriangle;
    Byte  downPointTriangle;
    Byte  leftDoubleArrows;
    Byte  rightDoubleArrows;

		/* New for NU 5.0	*/
    Byte  leftHalfBlock;
    Byte  rightHalfBlock;
    Byte  smallDot;

    Byte  upperRightMixed;
    Byte  leftLowerMixed;
    Byte  checkMark;

		/* New for NU 6.0	*/
    Byte  lowerHalfBlock;
    Byte  upperHalfBlock;
    
		/* New for ND		*/
    Byte  upDownArrow;
    Byte  leftRightArrow;
    Byte  leftBracket;
    Byte  rightBracket;
    Byte  plusMinus;
    Byte  textEditFiller;
    } CHAR_TABLE_RECORD;

extern CHAR_TABLE_RECORD near charTable;

extern Byte threeStateIcon;
extern Byte threeStateIconG0;
extern MouseIconRec mouseIcon;
extern ControlIconRec checkBoxIcon;
extern ControlIconRec radioIcon;
extern ControlIconRec gCheckBoxIcon;
extern ControlIconRec gRadioIcon;
extern ControlIconRec buttonIcon;
extern ScrollBarIconRec scrollIcon;
extern Byte pullDownCheck;

/*----------------------------------------------------------------------*
 * This structure is used for storing font redefinition characters      *
 *----------------------------------------------------------------------*/

#define _DeleteScanLines(start, end) ((start << 4) | end)

typedef struct
    {
    char ch;                            /* The char we want to replace  */
    Byte deleteLines1;			/* Scan lines to delete		*/
    Byte deleteLines2;			/* Scan lines to delete		*/
    char shape[16];                     /* New shape of the char        */
    char saveShape[16];			/* Save the old shape 		*/
    } FontChar;

#endif
