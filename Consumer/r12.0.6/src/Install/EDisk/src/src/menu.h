/* $Header:   S:/SRC/NORTON/VCS/MENU.H_V   1.7   17 Oct 1992 02:11:46   JOHN  $ */

/*----------------------------------------------------------------------*
 * MENU.H								*
 *									*
 * This file contains constants for pull-down menus.			*
 * These must be kept consistent with the actual menu bar definitions.	*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/SRC/NORTON/VCS/MENU.H_V  $
 * 
 *    Rev 1.7   17 Oct 1992 02:11:46   JOHN
 * General cleanup and update for New Loader
 * 
 *    Rev 1.6   13 Oct 1992 09:35:14   SKURTZ
 * Changed Configuration menu to make them compatible with new NUCONFIG
 *
 *    Rev 1.5   07 Oct 1992 12:08:10   SKURTZ
 * Changed CUTOFF_POINT to the CUT_FROM and CUT_TO defines.  Since leftmost
 * menu now has a QUIT command, this line has to be moved up if ability to edit
 * the menu is removed.
 *
 *
 *    Rev 1.4   28 Aug 1992 17:24:32   SKURTZ
 * Fixed ADVISE menu to work with new hypertext helpsystem
 *
 *    Rev 1.3   24 Jul 1992 18:09:18   SKURTZ
 * Preparations for Scorpio.  Redid menu ID numbers for support of new Passport
 * library.
 *
 *    Rev 1.2   07 Feb 1991 15:00:12   ED
 * Rearranged menu ID's
 *
 *    Rev 1.1   18 Dec 1990 16:10:38   ED
 * Added menu definitions for the "Search" and "Application errors"
 * menu items.
 *
 *    Rev 1.0   20 Aug 1990 15:19:44   DAVID
 * Initial revision.
 */

#define	CUT_FROM    	7			/* menu could end here!	*/
#define CUT_TO		3

#define SORT_BY_NAME	0			/* MENU menu    	*/
#define SORT_BY_TOPIC	1
#define ADD_CMD		2
#define EDIT_CMD	3
#define DELETE_CMD	4
#define QUIT		5

#define	VIDEO_MOUSE	6			/* CONFIGURE menu	*/
#define PRINTER_CONFIG  7
#define STARTUP_PROGS   8
#define	ALT_NAMES	9 
#define	PASSWORDS	10
#define	MENU_EDIT	11

#define IDM_ADVISE	12			/* HELP menu		*/
#define IDM_HELP_INDEX	13
#define IDM_KEYBOARD	14
#define IDM_MOUSE	15
#define IDM_DIALOG	16
#define IDM_PULLDOWN	17
#define IDM_USING_HELP	18
#define IDM_ABOUT	19

