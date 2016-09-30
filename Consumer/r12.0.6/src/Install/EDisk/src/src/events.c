/* $Header:   S:/SRC/NORTON/VCS/EVENTS.C_V   1.13   21 Apr 1993 16:45:38   SKURTZ  $ */

/*----------------------------------------------------------------------*
 * EVENTS.C      							*
 *									*
 * This file contains the event handling procedures and the dispatcher  *
 * for menu bar selections.                                     	*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/SRC/NORTON/VCS/EVENTS.C_V  $
 * 
 *    Rev 1.13   21 Apr 1993 16:45:38   SKURTZ
 * Fixes ptr 8332. Moving window around using keyboard caused cursor confusion
 *
 *    Rev 1.12   05 Jan 1993 15:44:26   SKURTZ
 * Additions for bringing ADVISE feature online
 *
 *    Rev 1.11   23 Dec 1992 09:33:20   SKURTZ
 * Fixed ptr NU05824. Now allows grey+ and grey- keys to be used.
 *
 *    Rev 1.10   16 Nov 1992 18:41:56   SKURTZ
 * Inserted #ifdefs for making EMERGENCY boot disk for DOS 6
 *
 *    Rev 1.9   17 Oct 1992 02:11:38   JOHN
 * General cleanup and update for New Loader
 *
 *    Rev 1.8   13 Oct 1992 09:35:14   SKURTZ
 * Changed Configuration menu to make them compatible with new NUCONFIG
 *
 *    Rev 1.7   28 Aug 1992 17:24:32   SKURTZ
 * Fixed ADVISE menu to work with new hypertext helpsystem
 *
 *    Rev 1.6   24 Jul 1992 17:58:16   SKURTZ
 * Preparations for Scorpio.  Fixed source of compiler warning messages.
 * Fixed MenuItem() to understand new ID numbers returned by PullDownEvent().
 * Changed Keyevent and MouseEvent to use PullDownEvent().  Note: The ADVISExxxx
 * routines will have to be updated before this module is finished.
 *
 *    Rev 1.5   19 Jun 1991 17:27:50   PETERD
 * Changed the Advise menu items to library function calls.
 *
 *    Rev 1.4   26 Apr 1991 16:59:40   ED
 * Modified KeyEvent() to convert CR events to CTRL_ENTER events,
 * so we don't have to deal with the "Ctrl-Enter Accepts" setting.
 *
 *    Rev 1.3   22 Mar 1991 15:03:42   ED
 * Added detection for the close box to MouseEvent()
 *
 *    Rev 1.2   07 Feb 1991 14:53:54   ED
 * Modified MenuItem() to work with the new Configuration menu
 *
 *    Rev 1.1   18 Dec 1990 16:11:14   ED
 * Added support for the "Search" and "Application Errors" menu
 * items in the MenuItem() function.
 *
 *    Rev 1.0   20 Aug 1990 15:19:28   DAVID
 * Initial revision.
 */

#include	<types.h>
#include	<dialog.h>
#include	<pulldown.h>
#include	<keys.h>
#include	<stdabout.h>
#include        <hyper.h>
#include	<lint.h>

#include	"defs.h"
#include	"proto.h"
#include	"menu.h"

/*----------------------------------------------------------------------*
 * Local function prototypes						*
 *----------------------------------------------------------------------*/

int	LOCAL AltToggleEvent(void);
int	LOCAL MouseEvent(DIALOG_RECORD *mainDlgHdl);
int	LOCAL MenuItem(int menu);
int	LOCAL KeyEvent(DIALOG_RECORD *mainDlgHdl, int key);

/*----------------------------------------------------------------------*
 * External data							*
 *----------------------------------------------------------------------*/
#ifndef EMERGENCY
extern  MenuBarRec menuBar;
#endif
/*----------------------------------------------------------------------*
 * This procedure is the main control loop which calls an appropriate  	*
 * routine to handle each event.					*
 *----------------------------------------------------------------------*/

void EventLoop(void)
{
    extern	DIALOG_RECORD	*mainDlgHdl; /* Main list dialog handle	*/
    auto	int		event;	    /* Type of event	 	*/
    auto	int		done;	    /* Ready to quit program?	*/
#ifdef EMERGENCY
    extern 	Byte		*CantExitFromProgram[];
#endif

    mainDlgHdl = OpenMainList();

    done = FALSE;
    while (!done)
        {
        event = DialogGetEvent();

        switch (event)
            {
            case MOUSE_EVENT:			/* A button was pressed	*/
                done = MouseEvent(mainDlgHdl);	/* Handle button push 	*/
                break;

	    case ALT_TOGGLE:		/* Alt key pressed and released	*/
	    	done = AltToggleEvent();
		break;

	    case NULL:			/* No mouse or key input ready	*/
                MainFilter(mainDlgHdl, NULL);
	        break;

	    default:			            /* A key was hit 	*/
                done = KeyEvent(mainDlgHdl, event); /* Handle key code 	*/
                break;
            }
#ifdef EMERGENCY
	if (done)
	    {
	    StdDlgError(CantExitFromProgram);
	    done = FALSE;
	    }
#endif
        }

    CloseMainList(mainDlgHdl);
}

/*----------------------------------------------------------------------*
 * This procedure activates the menu bar after the user presses and	*
 * releases the Alt key.						*
 *									*
 * Returns:	TRUE   if time to quit program (due to Quit command)	*
 * 		FALSE  otherwise 				    	*
 *----------------------------------------------------------------------*/

int LOCAL AltToggleEvent(void)
{
#ifndef EMERGENCY

    auto	int		menu; 	/* Number of pull-down menu	*/
    auto	int		done; 	/* Return value			*/

    MenuDisableItems();

    if ((menu = PulldownEvent(&menuBar,ALT_KEY_EVENT)) == PD_CONTROL_ID_CLOSE)
	done = TRUE;
    else
	done = MenuItem(menu);

    RedrawMenuBar();			/* Remove menu name highlight	*/
    return(done);



#else
    return(FALSE);
#endif

}

/*----------------------------------------------------------------------*
 * This function takes care of all mouse events.		 	*
 *									*
 * MouseUp() and MouseDown() are used to maintain a finite-state	*
 * machine which detects double clicks; they update the library		*
 * variable mouse_clicks.						*
 *									*
 * Returns:	TRUE   if time to quit program (due to Quit command)	*
 * 		FALSE  otherwise 				    	*
 *----------------------------------------------------------------------*/

int LOCAL MouseEvent(DIALOG_RECORD *mainDlgHdl)
{
    extern      OutputFieldRec	mainField;
    extern	TERec		mainTE;
    auto	int		row, col;   /* Mouse position   	*/
    auto	int		buttons;    /* Mouse button flags	*/
#ifndef EMERGENCY
    auto	int		menu;	    /* Pull-down menu number	*/
#endif
    auto	int		done;	    /* Return value	       	*/
    auto	Word		wDialogResponse;

    MenuDisableItems();
    buttons = MouseButtons(&row, &col);

    if (buttons == 0)			/* No buttons pressed		*/
        {
	MouseUp();			/* Update state machine		*/
	return (FALSE);			/* We ignore button releases	*/
	}
    else
	MouseDown(row, col);		/* Update state machine		*/

#ifndef EMERGENCY

    if ((unsigned)row == menuBar.row)
        {
	done = FALSE;

	if ((menu = PulldownEvent(&menuBar,MOUSE_EVENT)) == PD_CONTROL_ID_CLOSE)
	    done = TRUE;
	else
	    done = MenuItem(menu);

	RedrawMenuBar();		/* Remove menu name highlight	*/
	MouseUp();			/* Update state machine		*/
        return(done);
	}
    else
#endif
        {
        if (MainFilter(mainDlgHdl, MOUSE_EVENT) == EVENT_AVAIL)
	    {
	    wDialogResponse = DialogProcessEvent(mainDlgHdl, MOUSE_EVENT);
	    switch (wDialogResponse)
		{
		case ACCEPT_DIALOG :
		    DoMainListCommand();
		    break;
		case ABORT_DIALOG:
		    return (TRUE);
		    break;
		}
	    }

    	/* update state of edit field in case window moved */
    	mainTE.row = mainField.row;
    	mainTE.col = mainField.col;
	done = FALSE;
	}

    return(done);
}

/*----------------------------------------------------------------------*
 * This function takes care of all key events.				*
 *									*
 * Returns:	TRUE   if time to quit program (due to Quit command)	*
 * 		FALSE  otherwise 				    	*
 *----------------------------------------------------------------------*/

int LOCAL KeyEvent(DIALOG_RECORD *mainDlgHdl, int key)
{
    extern      OutputFieldRec	mainField;
    extern	TERec		mainTE;
    auto     	int		result;
#ifndef EMERGENCY
    auto     	int		done;
    auto     	int		nID;

    done = FALSE;  			/* Assume we won't quit		*/


    MenuDisableItems();
    switch (key)
	{
	case F1:
	    HyperHelpDisplay();
	    return(FALSE);
	case GREY_MINUS:		// allow use of grey- and grey+
	    key = '-';			// keys.
	    break;
	case GREY_PLUS:
	    key = '+';
	    break;
	}


    nID = PulldownEvent(&menuBar, key);

    if (nID == PD_CONTROL_ID_CLOSE)
	return(TRUE);

    if (nID != PD_NO_MENU)
	{
        done = MenuItem(nID);
        RedrawMenuBar();
        return(done);
        }

#else

    switch (key)			// even if in EMERGENCY configuration,
	{				// still have to translate grey- and grey+
	case GREY_MINUS:		// keys.
	    key = '-';
	    break;
	case GREY_PLUS:
	    key = '+';
	    break;
	}
#endif


    if (key == CR)			// convert CRs so we don't have to
   	key = CTRL_ENTER;		// deal with "Ctrl-Enter accepts"

    if (MainFilter(mainDlgHdl, key) == EVENT_AVAIL)
	{
        result = DialogProcessEvent(mainDlgHdl, key);
        if (result == ACCEPT_DIALOG)
	    DoMainListCommand();
	else if (result == ABORT_DIALOG)
	    return(TRUE);

	else if (result == EVENT_USED)
	    {
    	    mainTE.row = mainField.row;
    	    mainTE.col = mainField.col;
	    }
	}

    return(FALSE);
}

/*----------------------------------------------------------------------*
 * This function takes a menu and item number from the main menu bar 	*
 * and calls the corresponding procedure.			 	*
 *									*
 * Returns:	TRUE   if time to quit program (due to Quit command)	*
 * 		FALSE  otherwise 				    	*
 *----------------------------------------------------------------------*/

int LOCAL MenuItem(int menu)
{
#ifndef EMERGENCY
    extern	Byte		*pszProductEnvVar;
    extern	Boolean		cursorOn;
    extern	Word		wHelpMenuTopics[];
    auto	int		done;

    /*
     * We turn off the hardware cursor so it isn't left in a weird
     * place outside the textedit field by drawing commands done
     * by the selected menu item.  It will be moved back into place
     * and turned on again when MainFilter() gets a NULL event.
     */

    CursorOff();
    cursorOn = FALSE;

    done = FALSE;			/* Assume we won't quit		*/

    switch (menu)
        {
	case SORT_BY_NAME:
	    DoSortCommands(TRUE);
	    break;

	case SORT_BY_TOPIC:
	    DoSortCommands(FALSE);
	    break;

	case ADD_CMD:
	    DoAddCmd();
	    break;

	case EDIT_CMD:
	    DoEditCmd();
	    break;

	case DELETE_CMD:
	    DoDeleteCmd();
	    break;

	case QUIT:
            done = TRUE;
	    break;

	case VIDEO_MOUSE:
	case PRINTER_CONFIG:
	case STARTUP_PROGS:
	case ALT_NAMES:
	case PASSWORDS:
	case MENU_EDIT:
	    DoExecConfig(menu);
            break;

	case IDM_ADVISE:
	    DoAdvise();
	    break;

	case IDM_HELP_INDEX:
	case IDM_KEYBOARD:
	case IDM_MOUSE:
	case IDM_DIALOG:
	case IDM_PULLDOWN:
	case IDM_USING_HELP:
	    HyperHelpDisplay();
	    break;

	case IDM_ABOUT:
	    StdDlgAboutBox();
	    break;
        }

    return(done);

#else
    return(FALSE);
#endif
}
