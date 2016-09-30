/* $Header:   S:/SRC/NORTON/VCS/UTIL.C_V   1.6   17 Oct 1992 02:11:40   JOHN  $ */

/*----------------------------------------------------------------------*
 * UTIL.C     								*
 *									*
 * This file contains miscellaneous utility routines.			*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/SRC/NORTON/VCS/UTIL.C_V  $
 * 
 *    Rev 1.6   17 Oct 1992 02:11:40   JOHN
 * General cleanup and update for New Loader
 * 
 *    Rev 1.5   24 Jul 1992 17:45:44   SKURTZ
 * Preparations for Scorpio. Fixed source of compiler warning messages.
 * Changed draw box routine to use bGraphicWindow as its video test. Based
 * on this it now gets the characters from either charTable or WindowFontDefs.
 * Before, the characters were hard-coded in.
 *
 *    Rev 1.4   19 Jun 1991 17:27:40   PETERD
 * Removed FarToNear().
 *
 *    Rev 1.3   10 Apr 1991 14:50:48   ED
 * Fixed the scroll bar misalignment (actually, the list box
 * misalignment) on the main command list.  This only happened
 * in non-graphics mode.
 *
 *    Rev 1.2   27 Mar 1991 10:24:50   ED
 * Changed the list box colors to be the same as the dialog title
 *
 *    Rev 1.1   24 Mar 1991 09:59:00   ED
 * Modified DrawListBox() to draw a graphical list box and title if the
 * full graphics mode option is on.
 *
 *    Rev 1.0   20 Aug 1990 15:19:36   DAVID
 * Initial revision.
 */

#include	<types.h>
#include	<dialog.h>
#include	<draw.h>
#include	<lint.h>

#include	"defs.h"
#include	"proto.h"

#define	FLUSH_ME

#ifdef	FLUSH_ME
#define	FlushIt()	FastFlush()
#else
#define	FlushIt()	{ }
#endif

extern CHAR_TABLE_RECORD near charTable;

/*----------------------------------------------------------------------*
 * This routine draws a box as shown (without erasing the interior).	*
 * The gap flag prevents clobbering of a list scroll bar.		*
 *									*
 *	ÚÄÄÄÄÄtitleÄÄÄÄÄ¿	   					*
 *	³top               ³	   					*
 *	³                  height					*
 *	³           left   ³ 	   					*
 *	ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ						*
 *	 ÄÄÄÄÄwidthÄÄÄÄÄ						*
 *									*
 *----------------------------------------------------------------------*/

void PASCAL DrawListBox(Byte *title, Word top, Word left, Word height, Word width, Boolean gap)
{
    extern	PATTERN		list_center[];
    extern	PATTERN		list_bottom[];
    auto	Word		titleLen;
    auto	Word		leftPad;
    auto	Word		right;
    auto	Word		i;

    right = left + width - 1;
    titleLen = StringLength(title);
    leftPad = (width - titleLen) / 2;

    if (!bGraphicWindows)
	{
	FastSetRowCol(top - 1, left - 1);
	FastWriteRawChar(charTable.singleUpperLeft);
	FlushIt();
	FastWriteNChars(charTable.singleHorizontal, leftPad);
	FlushIt();

	ColorSaveAll();
	ColorSetAttr(BRIGHT);
	FastWriteString(title);
	FlushIt();
	ColorRestoreAll();

	FastWriteNChars(charTable.singleHorizontal, width - leftPad - titleLen);

	if (gap)
	    FastWriteRawChar(charTable.singleHorizontal);
	FlushIt();
	FastWriteRawChar(charTable.singleUpperRight);
	FlushIt();

	/* Draw middle rows */
	for (i = 0; i < height; i++)
	    {
	    FastSetRowCol(top + i, left - 1);
	    FastWriteRawChar(charTable.singleVertical);
	    FlushIt();
	    if (gap == FALSE)
	        {
	        FastSetRowCol(top + i, right + 1);
	        FastWriteRawChar(charTable.singleVertical);
		FlushIt();
	        }
	    }

	/* Draw bottom line */
	FastSetRowCol(top + height, left - 1);
	FastWriteRawChar(charTable.singleLowerLeft);
	FlushIt();
	FastWriteNChars(charTable.singleHorizontal, width);

	if (gap)
	    FastWriteRawChar(charTable.singleHorizontal);

	FastWriteRawChar(charTable.singleLowerRight);

	FastFlush();
	}
    else
	{
	ColorSaveAll();
	ColorSetAttr(ACTIVE_TITLE);

	FastSetRowCol(top - 1, left - 1);
	FastWriteSpaces(leftPad);
	FastWriteString(title);
	FastWriteSpaces(width - leftPad - titleLen + 2);
	if (gap == TRUE)
	    FastWriteChar(' ');

	ColorRestoreAll();

	/* Draw middle rows */
	for (i = 0; i < height; i++)
	    {
	    FastSetRowCol(top + i, left - 1);
	    FastWriteRawChar(list_center[0].c);  // Left
	    if (gap == FALSE)
	        {
	        FastSetRowCol(top + i, right + 1);
	        FastWriteRawChar(list_center[2].c);  // Right
	        }
	    }

	/* Draw bottom line */
	FastSetRowCol(top + height, left - 1);
	FastWriteNChars(list_bottom[1].c, width + 2);
	if (gap == TRUE)
	    FastWriteChar(list_bottom[1].c);

	FastFlush();
	}
}

/*----------------------------------------------------------------------*
 * This routine tests if a character permitted in a pathname.		*
 *----------------------------------------------------------------------*/

Boolean	CharIsFile (Byte c)
{
    if (c <= ' ')
        return (FALSE);

    switch (c)
        {
	case '"' :
	case '/' :
	case '[' :
	case ']' :
	case '|' :
	case '<' :
	case '>' :
	case '+' :
	case '=' :
	case ';' :
	case ',' :
	case '?' :
	case '*' :
	    return (FALSE);
	}

    return (TRUE);
}

/*----------------------------------------------------------------------*
 * This routine compacts the last component of a path name, if it has	*
 * more than 8 letters in its name or more than 3 in its extension.	*
 *									*
 * Examples:								*
 *	"NNNNNNNNxxxxxxxx"			--> "NNNNNNNN"		*
 *	"xxxx\xxxxxx\NNNNNNNNxxxxxx.EEExxxxxx"	--> "NNNNNNNN.EEE" 	*
 *	"x:NNNNNNNNxxxxxxxx.E"			--> "NNNNNNNN.E"	*
 *----------------------------------------------------------------------*/

void NameFixEnd (Byte *name)
{
    auto	Byte	ext[4];
    auto	Byte*	s;
    auto	Word	len;

    if (*name == '\0')
        return;

    *ext = '\0';
    len = StringLength(name);
    s = name + len - 1;

    while (len > 1 && *s != '.' && *s != '\\' && *s != ':')
        {
	len--;
        s--;
	}

    if (*s == '.')			/* If there is an exension	*/
        StringNCopy(ext, s + 1, 3);	/* Save first three chars of it	*/

    while (len > 1 && *s != '\\' && *s != ':')
        {
	len--;
	s--;
	}

    if (*s == '\\' || *s == ':') 	/* Make sure at true name start	*/
        s++;

    if (StringLength(s) > 8)		/* Chop long name 		*/
        s[8] = '\0';
    NameStripExtension(s);

    if (*ext != '\0')			/* Add saved extension, if any	*/
        {
        StringAppend(s, ".");
        StringAppend(s, ext);
	}
}

/*----------------------------------------------------------------------*
 * This routine converts a string with attribute escape sequences	*
 * into a string of character/attribute-index pairs.			*
 *									*
 * 'n' is the largest number of pairs (excluding null) that will be	*
 * placed in the target string.						*
 *									*
 * The index represents one of the fields of a COLORS record.		*
 * The new string is terminated with a zero word.			*
 * The new string can be up to twice as long (in bytes) as the old 	*
 * string.								*
 * 									*
 * Example:   "Normal ^Bbold"  -->  N0 o0 r0 m0 a0 l0  0 b2 o2 l2 d2 00	*
 *----------------------------------------------------------------------*/

void ConvertDescrStrToCharAttr (Word far *d, Byte *s, Word n)
{
    auto	Byte		c;
    auto	Byte		attrIndex;

    attrIndex = 0;

    while (n > 0 && (c = *s++) != '\0')
        {
	if (c == '^')
	    {
	    switch (*s++)
	        {
		case 'n':
		case 'N':
		    attrIndex = 0;
		    break;

		case 'r':
		case 'R':
		    attrIndex = 1;
		    break;

	        case 'b':
		case 'B':
		    attrIndex = 2;
		    break;

	        case 'u':
		case 'U':
		    attrIndex = 3;
		    break;
		}
	    continue;
	    }

	*d++ = c + (attrIndex << 8);
	n--;
	}

    *d++ = 0;
}

/*----------------------------------------------------------------------*
 * This routine converts a string of character/attribute-index pairs	*
 * into a normal string with attribute escape sequences.		*
 *									*
 * 'n' is the largest number of characters (excluding null) that will	*
 * be placed in the target string.	  				*
 *									*
 * The new string can be up to 50% longer (in bytes) than the old 	*
 * string.								*
 * 									*
 * Example:   H0 i0  0 B1 y1 e1  0 !2 00  -->  "Hi ^Rbye^B!"		*
 *----------------------------------------------------------------------*/

void ConvertCharAttrToDescrStr (Byte *d, Word far *s, Word n)
{
    auto	Byte		c;
    auto	Byte		attrIndex;
    auto	Byte		oldAttrIndex;
    auto	Byte far*	ss = (Byte far *) s;


    oldAttrIndex = 0;

    while (n > 0)
        {
	c         = *ss++;
	attrIndex = *ss++;

	if (c == '\0')
	    break;

	if (attrIndex != oldAttrIndex)
	    {
	    *d++ = '^';
	    switch (attrIndex)
	        {
		case 0:
		default:
		    *d++ = 'N';
		    break;

		case 1:
		    *d++ = 'R';
		    break;

	        case 2:
		    *d++ = 'B';
		    break;

	        case 3:
		    *d++ = 'U';
		    break;
		}
	    oldAttrIndex = attrIndex;
	    }

	*d++ = c;
	n--;
	}

    *d = '\0';
}

/*----------------------------------------------------------------------*
 * This routine is a stripped down version of StringStripSpaces().	*
 *----------------------------------------------------------------------*/

void PASCAL StringStripTrailingSpaces (Byte *string)
{
    auto	Byte*		s;

    s = string + StringLength(string);
    while (s > string && (*(s-1) == ' ' || *(s-1) == '\t'))
        s--;
    *s = '\0';
}
