/* $Header:   S:/SRC/NORTON/VCS/MAINLIST.C_V   1.11   16 Nov 1992 18:41:56   SKURTZ  $ */

/*----------------------------------------------------------------------*
 * MAINLIST.C								*
 *									*
 * This file contains routines for managing the main dialog.		*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/SRC/NORTON/VCS/MAINLIST.C_V  $
 * 
 *    Rev 1.11   16 Nov 1992 18:41:56   SKURTZ
 * Inserted #ifdefs for making EMERGENCY boot disk for DOS 6
 *
 *    Rev 1.10   29 Oct 1992 15:56:30   SKURTZ
 * Updated DestroyWindow to WinDistroyWindow()
 *
 *    Rev 1.9   17 Oct 1992 02:11:36   JOHN
 * General cleanup and update for New Loader
 *
 *    Rev 1.8   25 Sep 1992 14:05:18   SKURTZ
 * Conversion to LARGE memory model
 *
 *    Rev 1.7   24 Jul 1992 17:34:34   SKURTZ
 * Preparations for Scorpio.  Fixed source of compiler warning messages.
 *
 *    Rev 1.6   18 May 1991 19:47:04   JOHN
 * Additional fix for the topic help problem.
 *
 *    Rev 1.5   18 May 1991 18:51:56   PETERD
 * TOPICs now get the right help topic.
 *
 *    Rev 1.4   15 May 1991 09:35:30   ED
 * Added the HelpSetCommand() function to SetCommandHelp() to set
 * help according to the full name or the alias (short name) of
 * the programs.  Fixes PTR #NU2984.
 *
 *    Rev 1.3   24 Mar 1991 09:57:28   ED
 * Modified MainListWriteEntry() to display TOPICs using BRIGHT and
 * BRIGHT_INVERSE attributes, instead of the old method of drawing a
 * HORIZONTAL line.
 *
 *    Rev 1.2   22 Mar 1991 21:06:16   JOHN
 * Changed <config.plainDeskTop> to <nlibConfig.plainDeskTop>
 *
 *    Rev 1.1   28 Feb 1991 08:11:20   ED
 * Added the TE_CONVERT_CR and TE_NO_BRACKETS flags to the main text
 * edit field.
 *
 *    Rev 1.0   20 Aug 1990 15:19:36   DAVID
 * Initial revision.
 */

#include	<types.h>
#include	<dialog.h>
#include	<draw.h>
#include	<editbox.h>
#include	<nuconfig.h>
#include	<main.h>
#include	<password.h>
#include	<hyper.h>
#include	<lint.h>

#include	"nort-hlp.h"
#include	"defs.h"
#include	"proto.h"
#include	"menu.h"

/*----------------------------------------------------------------------*
 * Local function prototypes						*
 *----------------------------------------------------------------------*/

void		DialogAttachMenubar(DIALOG_RECORD *dr, MenuBarRec *pMenuBar);
Boolean LOCAL	HelpSetCommand(Byte *name, Byte far *fileName, Word helpID);
Boolean	LOCAL	MouseInTE(TERec *te, Word row, Word col);
void	LOCAL	InitEditField(Byte far *s);
Boolean		SkipItem(Ulong itemNum);

/*----------------------------------------------------------------------*
 * External data							*
 *----------------------------------------------------------------------*/

extern DIALOG_RECORD	*mainDlgHdl;
extern OutputFieldRec	mainField;
#ifndef EMERGENCY
extern MenuBarRec	menuBar;
#endif
/*----------------------------------------------------------------------*
 * This routine is called just before the main event loop to open	*
 * the main dialog box.							*
 *									*
 * Returns:	dialog record handle, to manage the opened dialog 	*
 *----------------------------------------------------------------------*/

DIALOG_RECORD * OpenMainList(void)
{
    extern	Byte		descrRow, descrCol;
    extern	Byte		descrWidth, descrHeight;
    extern	Byte		DESCRIPTION_STR[];
    extern	Byte		COMMAND_STR[];
    extern	DialogRec	mainDlg;
    extern	ListRec   	mainList;
    extern	Boolean		openingMainDlg;
    auto	DIALOG_RECORD	*dr;

    WinSuspendUpdates();

    ListGrow(mainList);

    openingMainDlg = TRUE;		/* affects WriteEntry routine	*/
    dr = DialogOpen(&mainDlg,
    		    &mainList,
		    DESCR_LINES + 3 + VideoGetMaxRows() - 25,
		    &mainField);
    openingMainDlg = FALSE;

    /* Save screen area of description box interior.
     * This includes a space of padding on either side. */

    descrRow    = (Byte)(mainList.row + 1);
    descrCol    = (Byte)(mainList.col + mainList.width + 4);
    descrWidth  = DESCR_WIDTH + 2;
    descrHeight = (Byte) (DESCR_LINES + VideoGetMaxRows() - 25);

    /* Draw box frames into dialog.
     * Remember that dimensions passed to
     * box routines are for box interiors. */

    DrawListBox(COMMAND_STR,
                mainList.row, mainList.col,
                mainList.height, mainList.width, TRUE);

    DrawListBox(DESCRIPTION_STR,
    		descrRow - 1, descrCol,
		descrHeight + 1, descrWidth, FALSE);

    /* Highlight the active list entry again.
     * This time we are not opening the dialog box
     * so the description and output field will be drawn. */

    ListDisplayEntry(&mainList, mainList.active_entry, INVERSE);

#ifndef EMERGENCY
    DialogAttachMenubar(dr, &menuBar);

    RedrawMenuBar();	// It seems like you don't need this, but you do!
#endif
    WinResumeUpdates();

    return(dr);
}

/*----------------------------------------------------------------------*
 * This routine is the counterpart to OpenMainList().			*
 * It is called after the main event loop has terminated.		*
 *									*
 * This used to be more exciting in a previous version of NI.		*
 *----------------------------------------------------------------------*/

void CloseMainList(DIALOG_RECORD *dr)
{
    extern	ListRec   	mainList;

    WinSuspendUpdates();

#ifndef EMERGENCY
    if (menuBar.mWindow)
        {
        WinDestroyWindow(menuBar.mWindow);
        menuBar.mWindow = NULL;
        }
#endif

    DialogClose(dr, EVENT_USED);

    WinResumeUpdates();

    ListShrink(mainList);
}

/*----------------------------------------------------------------------*
 * DialogAttachMenubar()						*
 *----------------------------------------------------------------------*/
#ifndef EMERGENCY

void DialogAttachMenubar(DIALOG_RECORD *dr, MenuBarRec *pMenuBar)
{
    // map the menu bar to the dialog

    pMenuBar->leftCol  = dr->saveRect.left;
    pMenuBar->rightCol = (Byte) (pMenuBar->leftCol + dr->width + 3);

    // if in /G0 mode, shrink the menu to fit

    if (!bGraphicControls)
        {
        pMenuBar->leftCol++;
        pMenuBar->rightCol--;
        }

    // place the bar under the dialog title

    pMenuBar->row = (Byte) (dr->saveRect.top + 1);

    // draw the menu bar

    PulldownDrawBar(pMenuBar);

    // link with the dialog's window so that they will move together

    WinSetLink(dr->lpWindow, pMenuBar->mWindow);
    WinSetWindow(dr->lpWindow);
}

#endif

/*----------------------------------------------------------------------*
 * This is the text edit field in the main dialog box.			*
 *----------------------------------------------------------------------*/

Byte	mainString[130];

TERec	mainTE =
    {
    0, 0,		/* row, col	*/
    66,			/* width	*/
    mainString,		/* string	*/
    127,		/* max_len	*/
    0,			/* insert_pt	*/
    0,			/* offset	*/
    NULL,		/* validate	*/
    TE_CONVERT_CR | TE_NO_BRACKETS,		   	/* flags	*/
    };

/*----------------------------------------------------------------------*
 * This routine sets up the text edit field in the main dialog box	*
 * to display the string 's'.						*
 *----------------------------------------------------------------------*/

void LOCAL InitEditField (Byte far *s)
{
    extern	TERec		mainTE;
    extern	Byte		mainString[];
    extern	Boolean		cursorOn;

    mainTE.row = mainField.row;
    mainTE.col = mainField.col;

    if (s == NULL || s[0] == '\0')
        mainString[0] = '\0';
    else
        {
        StringLocalize(mainString, s);
        StringStripSpaces(mainString);
        StringAppend(mainString, " ");
        }
    mainTE.insert_pt = min(StringLength(mainString), (unsigned)mainTE.max_len - 1);

    if ((unsigned)mainTE.insert_pt >= mainTE.width)
        mainTE.offset = mainTE.insert_pt - mainTE.width + 1;
    else
        mainTE.offset = 0;

    ColorSetAttr(INVERSE);
    core_edit(&mainTE, -1);
    ColorSetAttr(NORMAL);
    CursorSetRowCol(mainTE.row, mainTE.col + mainTE.insert_pt - mainTE.offset);
    if (cursorOn)
        CursorUnderscore();
}

/*----------------------------------------------------------------------*
 * This is the event filter for the main dialog box.			*
 * The basic idea is to implement a dialog box with all items active.	*
 * We cannot simply use the library "ALL_ACTIVE" flag, since the 	*
 * library does not properly manage the hardware cursor.		*
 *----------------------------------------------------------------------*/

Word MainFilter(DIALOG_RECORD *dr, Word event)
{
    extern	ListRec   	mainList;
    extern	TERec		mainTE;
    extern	Boolean		cursorOn;
    auto	Word		buttons, mouseRow, mouseCol;
    auto	Word		oldRow, oldCol;

    switch (event)
        {
	case NULL:
	    if (!cursorOn)
	        {
    		CursorSetRowCol(mainTE.row, mainTE.col + mainTE.insert_pt - mainTE.offset);
                CursorUnderscore();
	        cursorOn = TRUE;
		}
	    return (EVENT_AVAIL);

        case CR:
	case CTRL_ENTER:
        case ESC:
        case TAB:
        case SHIFT_TAB:
	case CTRL_HOME:
	case CTRL_END:
        case CTRL_BREAK:
	case F1:
        case F10:
	case ALT_F4:
	case ALT_SPACE:
	     CursorOff();
	     cursorOn = FALSE;
	     return (EVENT_AVAIL);

        case PG_UP:
        case CURSOR_UP:
	case HOME:
	    if (mainList.active_entry == 0)
	        return (EVENT_USED);
	    else
	        {
	        CursorOff();
	        cursorOn = FALSE;
	        return (EVENT_AVAIL);
		}

        case PG_DN:
        case CURSOR_DOWN:
	case END:
	    if (mainList.active_entry == mainList.num - 1)
		return (EVENT_USED);
	    else
	        {
	        CursorOff();
	        cursorOn = FALSE;
	        return (EVENT_AVAIL);
		}

	    return (EVENT_AVAIL);
	}

    if (event == MOUSE_EVENT)
        {
	MouseButtons(&mouseRow, &mouseCol);

	if (MouseInTE(&mainTE, mouseRow, mouseCol))
	    {
	    oldRow = oldCol = 0xFFFF;
	    do  {
	        if ( (oldRow != mouseRow || oldCol != mouseCol) &&
	             MouseInTE(&mainTE, mouseRow, mouseCol) )
		    {
                    mainTE.insert_pt = mouseCol - mainTE.col;
		    mainTE.insert_pt = MathMin(mainTE.insert_pt, StringLength(mainTE.string));
                    FastSetRowCol(mainTE.row, mainTE.col + mainTE.insert_pt);
		    oldRow = mouseRow;
		    oldCol = mouseCol;
		    }
                }
                while ((buttons = MouseButtons(&mouseRow, &mouseCol)) != 0);
	    return (EVENT_USED);
	    }
	else
            {
            CursorOff();
	    cursorOn = FALSE;
	    return (EVENT_AVAIL);
	    }
	}

    ColorSetAttr(INVERSE);
    CursorOff();
    core_edit(&mainTE, event);
    ColorSetAttr(NORMAL);

    CursorSetRowCol(mainTE.row, mainTE.col + mainTE.insert_pt - mainTE.offset);
    CursorUnderscore();
    cursorOn = TRUE;

    return (EVENT_USED);
}

/*----------------------------------------------------------------------*
 * Tests if a given mouse position is in a TE record, with enough	*
 * typecasts to get MSC to shut up about conversion problems.		*
 *----------------------------------------------------------------------*/

Boolean LOCAL MouseInTE (TERec *te, Word row, Word col)
{
    if (row == (Word) mainTE.row &&
	 (Word) mainTE.col <= col && col < (Word) mainTE.col + mainTE.width)
        return (TRUE);
    return (FALSE);
}

/*----------------------------------------------------------------------*
 * This function is the do_mouse routine pointed to by the mainList 	*
 * ListRec structure.  It is called only by the library.		*
 *									*
 * If the user double-clicks on one of the list entries, the global	*
 * variable doubleClicked will be set.  The other global variables	*
 * are internal to the library.						*
 *									*
 * Returns:	1 or 2		If mouse double-clicked on list item	*
 *		3		If both buttons pressed and released	*
 *		0		Otherwise		  		*
 *----------------------------------------------------------------------*/

int MainListDoMouse(ListRec *list, Ulong entryNum,
		    RECT *entryRect, Word buttons, int mouseClicks)
{
    extern	Boolean		doubleClicked;
    extern	int		firstRow;
    extern	int		firstCol;
    extern	int		mouse_clicks;
    auto	int	  	result;

    if (mouseClicks == 2 && RectPointIn(entryRect, firstRow, firstCol))
        {
	result = MouseGeneralTrackControl(entryRect);
	mouse_clicks = 3;
	if (result == 1 || result == 2)
	    doubleClicked = TRUE;
	}
    else
        result = 0;

    return (result);
}

/*----------------------------------------------------------------------*
 * This routine processes the list event as usual, then forces the 	*
 * list highlight bar off of any "skip" item (if possible).		*
 *----------------------------------------------------------------------*/

Word MainListEventFilter(ListRec *list, Word *c)
{
    auto	Ulong		oldActiveEntry;
    auto	Ulong		newActiveEntry;
    auto	Word		result;

    oldActiveEntry = list->active_entry;

    result = ListMovement(list, c, TRUE);

    if (result == EVENT_USED)
        {
        if (SkipItem(list->active_entry))
	    {
	    newActiveEntry = list->active_entry;
	    if (oldActiveEntry > list->active_entry)	/* moved up	*/
	        {
		if (SkipUp(&newActiveEntry) == FALSE)
		    if (SkipDown(&newActiveEntry, list->num) == FALSE)
		        newActiveEntry = oldActiveEntry;
		}
	    else					/* moved down	*/
	        {
		if (SkipDown(&newActiveEntry, list->num) == FALSE)
		    if (SkipUp(&newActiveEntry) == FALSE)
		        newActiveEntry = oldActiveEntry;
		}
            ListHighlight(list, newActiveEntry, INVERSE);
	    }
	}

    return (result);
}

/*----------------------------------------------------------------------*
 * This is a utility routine for moving a list highlight bar off of 	*
 * "skip" items.  It tries to find the first available non-skip list	*
 * item in the up direction, starting at item "*entry".			*
 *----------------------------------------------------------------------*/

Boolean SkipUp(Ulong *entry)
{
    auto	Ulong	e;

    e = *entry;
    while (e > 0 && SkipItem(e))
	e -= 1;
    if (e == 0 && SkipItem(e))
	return (FALSE);
    else
        {
        *entry = e;
	return (TRUE);
	}
}

/*----------------------------------------------------------------------*
 * This is a utility routine for moving a list highlight bar off of 	*
 * "skip" items.  It tries to find the first available non-skip list	*
 * item in the down direction, starting at item "*entry". 		*
 *----------------------------------------------------------------------*/

Boolean SkipDown(Ulong *entry, Ulong num)
{
    auto	Ulong	e;

    e = *entry;
    while (e < num-1 && SkipItem(e))
	e += 1;
    if (e == num-1 && SkipItem(e))
	return (FALSE);
    else
        {
        *entry = e;
	return (TRUE);
	}
}

/*----------------------------------------------------------------------*
 * This routine tests if the indicated item in the command list is	*
 * a separator that the highlight bar should not rest on.		*
 *----------------------------------------------------------------------*/

Boolean SkipItem(Ulong itemNum)
{
    extern	CmdInfoRec far*	commands[MAX_COMMANDS];

    return (Boolean) (commands[itemNum]->type == TYPE_BLANK);
}

/*----------------------------------------------------------------------*
 * This procedure is the write_entry routine pointed to by the mainList	*
 * ListRec structure.  It is called only by the library.		*
 *----------------------------------------------------------------------*/

void MainListWriteEntry(ListRec *list, CmdInfoRec far* far* entry, int attr, Ulong itemNum)
{
    extern	Boolean		openingMainDlg;
    extern	Byte	  	descrRow, descrCol;
    extern	ListRec   	mainList;
    extern	Byte	  	descrWidth, descrHeight;
    extern	Byte	  	line[];
    auto	Word	  	i;
    auto	Word		far *s;
    auto	Byte	  	leftChar, rightChar;
    auto	Word		useAttr;
    auto	CmdInfoRec	far *r;
    auto	Word		effectiveWidth;
    auto	Word		leftPad, rightPad;

    HyperHelpTopicSet(H_NI_MAIN);	/* In case SetCommandHelp fails	*/

    descrRow    = (Byte)(mainList.row + 1);
    descrCol    = (Byte)(mainList.col + mainList.width + 4);

    if (entry == NULL)			/* Nothing to draw		*/
        {
        FastWriteSpaces(list->width);

	/* If the list is empty, display something reasonable in the
	 * description area.  */

	if (!openingMainDlg && itemNum == 0)
	    {
            VideoClearWindow(descrRow, descrCol,
	    		     descrRow + descrHeight - 1,
			     descrCol + descrWidth - 1);
            InitEditField(NULL);
	    }
        return;
	}

    r = *entry;

    useAttr = (unsigned)(SkipItem(itemNum)) ? NORMAL : attr;

    if (itemNum == list->active_entry && (list->flags & MOVE_MODE))
        {
        leftChar  = '';
	rightChar = '';
	}
    else
        leftChar = rightChar = ' ';

    FastWriteChar(leftChar);		/* Selection bar left gap	*/
    ColorSaveAll();
    ColorSetAttr(useAttr);
    FastWriteChar(' ');			/* Selection bar left padding	*/

    effectiveWidth = list->width - 4;

    if (r->type == TYPE_BLANK)
        FastWriteSpaces(effectiveWidth);
    else if (r->type == TYPE_TOPIC)
        {
	ColorSetAttr ( (useAttr == (unsigned)INVERSE) ? BRIGHT_INVERSE : BRIGHT);

        StringLocalize(line, r->name);
	i = StringLength(line) + 2;
	leftPad = (effectiveWidth - i) / 2;
	rightPad = effectiveWidth - i - leftPad;
	FastWriteSpaces(leftPad + 1);
        FastWriteString(line);
	FastWriteSpaces(rightPad + 1);
	ColorSetAttr(useAttr);
	}
    else
        {
        StringLocalize(line, r->name);
        FastWriteLine(line, 0, effectiveWidth);
	}

    FastWriteChar(' ');			/* Selection bar right padding	*/
    ColorRestoreAll();			/* Switch to original color	*/
    FastWriteChar(rightChar);	 	/* Selection bar right gap	*/

    /* We don't want to draw the description and command line
     * if we are in the process of opening the main dialog box,
     * since they will just be erased as the rest of the box is drawn */

    /* Get the row/col of the window that we're drawn in. */

    if (!openingMainDlg && attr == INVERSE)
        {
	if (SkipItem(itemNum))
	    {
            VideoClearWindow(descrRow, descrCol,
	    		     descrRow + descrHeight - 1,
			     descrCol + descrWidth - 1);
            InitEditField(NULL);
	    }
	else
	    {
            for (i = 0; i < DESCR_LINES; i++)
                {
	        s = r->descr[i];

		FastSetRowCol(descrRow + i, descrCol + 1);
	        if (s == NULL)
                    FastWriteSpaces(DESCR_WIDTH);
	        else
 		    WriteCharAttrString(s, DESCR_WIDTH);
  	        }
	    InitEditField(r->dosCmds[0]);

            if (r->type != TYPE_TOPIC)
	        SetCommandHelp();
	    }
	}
}

/*----------------------------------------------------------------------*
 * This procedure is called when the user selects a button from the	*
 * main list dialog.							*
 *----------------------------------------------------------------------*/

void DoMainListCommand (void)
{
    extern	Boolean		doubleClicked;
    extern	ListRec 	mainList;
    extern	Byte		mainString[];

    if (doubleClicked)
        doubleClicked = FALSE;

    if (SkipItem(mainList.active_entry))
        return;

    Launch(mainString);			/* May not return		*/
}

/*----------------------------------------------------------------------*
 * This routine returns a pointer to the current CmdInfoRec.		*
 *									*
 * This used to be more exciting in a previous version of NI.		*
 *----------------------------------------------------------------------*/

CmdInfoRec far *GetCurrentCommand (void)
{
    extern	CmdInfoRec far*	commands[MAX_COMMANDS];
    extern	ListRec		mainList;

    return (commands[mainList.active_entry]);
}

/*----------------------------------------------------------------------*
 * This routine checks if the specified command is a Norton program,	*
 * and if so, sets the current help topic to the topic that describes	*
 * the program.  This was thrown together using parts copied from	*
 * launch.c.								*
 *----------------------------------------------------------------------*/

void SetCommandHelp (void)
{
#ifndef EMERGENCY
    extern	CmdInfoRec	far *commands[MAX_COMMANDS];
    extern	ListRec		mainList;
    extern	NIFileInfoRec	fileInfo[NUM_EXE_FILES];
    auto	Byte		progName[129];
    auto	CmdInfoRec	far *r;
    auto	Byte		far *p;
    auto	Byte		*s;
    auto	Byte		name[13];
    auto	Word		i;

    if (mainList.num == 0)
        return;
    r = commands[mainList.active_entry];

    if (r->type == TYPE_TOPIC)
	{
	HyperHelpTopicSet(H_NI_MAIN);
	return;
	}

    /* Move p to start of command. */
    p = r->dosCmds[0];
    while (isspace(*p))
        p++;

    /* Copy command to buffer. */
    s = progName;
    while (CharIsFile(*p) || *p == ':' || *p == '\\')
        *s++ = *p++;
    *s = '\0';

    NameReturnFile(name, progName);
    NameStripExtension(name);
    StringToUpper(name);

    for (i = 0; i < NUM_EXE_FILES; i++)
        {
	if (HelpSetCommand(name, fileInfo[i].name, fileInfo[i].helpID))
	    return;

	if (HelpSetCommand(name, fileInfo[i].alias, fileInfo[i].helpID))
	    return;
	}

    if (r->type == TYPE_COMMAND)
        HyperHelpTopicSet(H_NI_USER_CMD);
#endif
}

Boolean LOCAL HelpSetCommand(Byte *name, Byte far *fileName, Word helpID)
{
    auto	Byte		cmdName[COMMAND_WIDTH + 1];
    auto	Boolean		success = FALSE;

    StringCopyFar(cmdName, fileName);
    StringToUpper(cmdName);

    if (StringNCmp(name, cmdName, 8) == 0)
	{
	HyperHelpTopicSet(helpID);
	success = TRUE;
	}

   return (success);
}