/* $Header:   S:/SRC/NORTON/VCS/EDITLIST.C_V   1.5   17 Oct 1992 02:11:44   JOHN  $ */

/*----------------------------------------------------------------------*
 * EDITLIST.C								*
 *									*
 * This file contains the routines for the editable list of DOS		*
 * commands. 								*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/SRC/NORTON/VCS/EDITLIST.C_V  $
 * 
 *    Rev 1.5   17 Oct 1992 02:11:44   JOHN
 * General cleanup and update for New Loader
 * 
 *    Rev 1.4   24 Jul 1992 17:49:52   SKURTZ
 * Preparations for Scorpio.  Fixed source of compiler warning messages.
 *
 *    Rev 1.3   25 Mar 1991 16:39:22   PETERD
 * Added a Scroll-Bar to the Add/Edit Topic list dialog.  Also expanded
 * the list to the full maximum number of topics.  This avoids having to
 * figure out how to get the scroll-bar to work in this list...
 *
 *    Rev 1.2   24 Mar 1991 09:57:08   ED
 * Made a slight change to the list box
 *
 *    Rev 1.1   14 Mar 1991 15:15:36   PETERD
 * Made changes to TERec.
 *
 *    Rev 1.0   20 Aug 1990 15:19:30   DAVID
 * Initial revision.
 */

#include	<types.h>
#include	<dialog.h>
#include	<keys.h>
#include	<internal.h>
#include	<lint.h>

#include	"defs.h"
#include	"proto.h"

/*----------------------------------------------------------------------*
 * Function prototypes							*
 *----------------------------------------------------------------------*/

void DoEditListTest(void);
void EditListWriteEntry(ListRec *list, TopicRec far *entry, int attr, Ulong itemNum);
Word EditListEventFilter(ListRec *l, Word *c);

/*----------------------------------------------------------------------*
 * This is the list data and the list description.			*
 *----------------------------------------------------------------------*/

#define STR_LEN		(TOPIC_WIDTH+1)	/* Len of list strs, incl. null	*/

#define LIST_HEIGHT	MAX_TOPICS	/* Num of entries in list array	*/
#define LIST_WIDTH	(TOPIC_WIDTH+2)

TopicRec editTopics[MAX_TOPICS];

SCROLL_BAR sBar;

ListRec editTopicList =
    {
    0, 0,				/* Row, col (set by library)	*/
    1,					/* Number of columns		*/
    LIST_WIDTH,	   			/* Width of each column		*/
    LIST_HEIGHT,     			/* Number of rows		*/
    EditListWriteEntry,			/* Procedure to display entry	*/
    NULL,				/* Procedure for mouse events	*/
    NULL,				/* Procedure to check matches	*/
    EditListEventFilter,     		/* Procedure to screen events	*/
    (void far*) editTopics,		/* Start address of entries	*/
    sizeof(TopicRec),			/* Size of each entry		*/
    THIN_LIST_BOX | MOVE_MODE
        | USE_ACTIVE,			/* Flags used for operation	*/
    &sBar,				/* Pointer to scroll bar defn	*/
    "",					/* Str to display between cols	*/
    5, 2,				/* Scroll delay values		*/
    0,					/* Number of entries in list	*/
    };

/*----------------------------------------------------------------------*
 * This is the TextEdit field used to edit the current list entry.	*
 * It uses its own char array to hold the edited string so we can	*
 * cancel editing operations (e.g. when the user hits ESC).		*
 *----------------------------------------------------------------------*/

Byte	teData[STR_LEN];

TERec te =
    {
    0,0,				/* row, col			*/
    TOPIC_WIDTH, 			/* width of edit box		*/
    teData, 				/* addr of string being edited	*/
    TOPIC_WIDTH,			/* max length of string		*/
    0, 0, 				/* insertion point, offset	*/
    NULL,				/* validation function		*/
    TE_NO_BRACKETS
    };

/*----------------------------------------------------------------------*
 * This procedure displays an entry in the editable list.		*
 *									*
 * If the list has been put into move mode, we draw arrows to the sides	*
 * of the highlighted text area.  Otherwise we put normal-colored	*
 * spaces.								*
 *----------------------------------------------------------------------*/

void EditListWriteEntry (ListRec *list, TopicRec far *entry, int attr, Ulong itemNum)
{
    extern	Byte		line[];
    auto	Byte		far *s;
    auto	Byte		leftChar;
    auto	Byte		rightChar;

    list->flags |= MOVE_MODE;		/* Make sure ALWAYS on		*/

    s = entry->name;

    if (entry == NULL)
        {
        FastWriteNChars(' ', list->column_width);
        return;
	}

    if (itemNum == list->active_entry && (list->flags & MOVE_MODE))
        {
        leftChar  = '';
	rightChar = '';
	}
    else
        leftChar = rightChar = ' ';

    FastWriteChar(leftChar);
    ColorSaveAll();
    ColorSetAttr((Word)attr);
    StringLocalize(line, s);
    FastWriteLine(line, 0, list->width - 2);
    ColorRestoreAll();
    FastWriteChar(rightChar);
}

/*----------------------------------------------------------------------*
 * This function filters dialog events for the editable list.		*
 *									*
 * When a dialog box event is given the list for processing, this	*
 * function has the option of stealing it away from the normal list	*
 * event handling code.							*
 *									*
 * The first time we get a dialog event (probably a NULL event very 	*
 * soon after the dialog box is drawn), we set up our own text-edit	*
 * field right on top of the list's active entry.  Then we process that	*
 * event as usual.  A similar thing happens for the first event 	*
 * received just after the text-edit field is "shut down".		*
 *									*
 * Null events are always passed on to the normal list code.		*
 * Mouse and key events are handled as follows:				*
 *									*
 *  editing event		Process in manner similar to the way	*
 *				the text-edit control does.		*
 *				fields.				 	*
 *  list,change field events	Shut down the text-edit field, then 	*
 *				pass event on to list code unscathed.  	*
 *  other events (bad keys)	Silently swallow them.			*
 *									*
 * Returns:	EVENT_USED 	Event was used to edit list data 	*
 *		EVENT_AVAIL	Event is passed on to normal list code	*
 *----------------------------------------------------------------------*/

Boolean     teFieldActive = FALSE;
Boolean	    editTopicFirstChar = TRUE;	/* Set when dialog opened	*/

Word EditListEventFilter(ListRec *l, Word *c)
{
    extern	TERec		te;
    extern	Byte		teData[];
    extern	Boolean    	teFieldActive;
    auto	Word		mouseRow, mouseCol;
    auto	Word		buttons;
    auto	Byte		far *s;
    auto	Byte		far *t = (Byte far *) teData;
    auto	TopicRec	far *topic;

    					/* This gets a pointer to	*/
					/* the string we want to edit.	*/
    topic = ListEntryAddress(l, l->active_entry);
    s = topic->name;

   					 /* This is where the TextEdit	*/
					 /* field is set up.		*/
    if (teFieldActive == FALSE)
        {
	te.row       = l->row + (Byte) (l->active_entry - l->top_entry);
	te.col       = (Byte)(l->col + 1);
	if (editTopicFirstChar)
	    te.insert_pt = te.offset = 0;
	StringCopyFar(t, s);		/* Copy string to TE's buffer	*/
	FastSetRowCol(te.row, te.col + te.insert_pt - te.offset);
	FastSetCursor();
	CursorUnderscore();
	teFieldActive = TRUE;
	}

					/* This is where we actually	*/
					/* process the events		*/

    if (*c == NULL)
        return (EVENT_AVAIL);		/* Don't act on null events	*/

    else if (*c == MOUSE_EVENT)
        {
					/* Track mouse until it comes	*/
					/* up or leaves TE field area.	*/

	while ((buttons = MouseButtons(&mouseRow, &mouseCol)) != 0)
	    {
	    if ( mouseRow != te.row ||
	         mouseCol < te.col || mouseCol >= te.col + te.width )
	        {
	        StringCopyFar(s, t);	/* Copy TE data back to list	*/
	        CursorOff();
	        teFieldActive = FALSE;
	        return (EVENT_AVAIL);
	        }
					/* Mouse is in TE field.  Move	*/
					/* the screen cursor under the	*/
					/* mouse cursor, or to end of	*/
					/* the string.			*/

	    te.insert_pt = mouseCol - te.col;
	    te.insert_pt = MathMin(te.insert_pt,
				    StringLength(te.string));
            FastSetRowCol(te.row, te.col + te.insert_pt);
	    FastSetCursor();

	    editTopicFirstChar = FALSE;
	    }

	return (EVENT_USED);
	}

    else				/* Must be a key event		*/
        {
					/* Check for list movement or	*/
					/* field movement keys. 	*/
	switch (*c)
	    {
	    case CR:
	    case ESC:
       	    case TAB:
	    case SHIFT_TAB:
	    case CURSOR_UP:
	    case CURSOR_DOWN:
	    case PG_UP:
	    case PG_DN:
	    case HOME:
	    case END:
            case CTRL_HOME:
            case CTRL_END:
            case CTRL_PG_UP:
            case CTRL_PG_DN:

	        if (*c == ESC)
		    ListDisplayEntry(l, l->active_entry, INVERSE);
		else
		    StringCopyFar(s, t);	/* TE data back to list	*/
		CursorOff();
	        teFieldActive = FALSE;
	        return (EVENT_AVAIL);

            default:			/* Potential editing key	*/

	    				/* If this is the first key	*/
					/* typed, erase the old string	*/

                if (editTopicFirstChar && *c >= ' ' && *c <= 255)
                    te.string[0] = '\0';

	        editTopicFirstChar = FALSE;

					/* Process one TE field key	*/
                ColorSetAttr(INVERSE);
                FastSetRowCol(te.row, core_edit(&te, *c));
		FastSetCursor();
                ColorSetAttr(NORMAL);
	        return (EVENT_USED);
	    }
	}
}
