/* $Header:   V:/INCLUDE/VCS/PULLDOWN.H_V   1.18   16 Oct 1992 18:57:48   JOHN  $ */

/*----------------------------------------------------------------------*
 * PULLDOWN.H								*
 *									*
 * Pulldown menu constants and data structures				*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   V:/INCLUDE/VCS/PULLDOWN.H_V  $
 * 
 *    Rev 1.18   16 Oct 1992 18:57:48   JOHN
 * Added PD_CONTROL_ID_ABOUT
 * 
 *    Rev 1.17   05 Oct 1992 11:00:42   MRUSCH
 * Add symbolic value for SwitchTo option on control menu.
 * 
 *    Rev 1.16   05 May 1992 16:25:32   JOHN
 * Added (Word) cast of PD_CONTROL_... constants
 * 
 *    Rev 1.15   27 Apr 1992 00:56:40   HENRI
 * Added ID's for new built in control menus.
 * 
 *    Rev 1.14   26 Apr 1992 20:43:08   HENRI
 * Added predefined ID's for the control menu.
 * 
 *    Rev 1.13   28 Feb 1992 17:05:54   ED
 * Added some stuff from the menu code itself, and a few items of my own
 * 
 *    Rev 1.12   19 Feb 1992 14:57:22   DOUG
 * added support for title onle menu bars
 * 
 *    Rev 1.11   27 Jan 1992 22:36:00   TORSTEN
 * Changed the #include statements to use angle brackets.
 * 
 *    Rev 1.10   16 Jan 1992 13:52:34   BASIL
 * Added macros PulldownEnableMultiItemID and PulldownCheckMultiItemID
 * 
 *    Rev 1.9   07 Nov 1991 18:27:12   HENRI
 * Added "HOT_KEY_INDICATOR" definition.
 * 
 *    Rev 1.8   07 Oct 1991 15:06:28   PETERD
 * Added MENU_DIVIDER
 * 
 *    Rev 1.7   29 Jul 1991 12:16:04   MARKK
 * Added window structure for menu bar.
 * 
 *    Rev 1.6   11 Jul 1991 18:01:20   BARRY
 * Added wFlags field to MenuItemRec.
 * 
 * Added flag ITEM_IMMEDIATE which tells the Pulldown Manager NOT to highlight
 * the menu bar item upon execution.
 * 
 * Added macros PulldownEnableItemID() and PulldownCheckItemID().  These macros
 * are used to set the state of the menu item.  It sets the active/inactive
 * state and the checked/unchecked state of the menu item.
 * 
 * Added the UNCHANGED flag for PulldownSetItemState().
 * 
 *    Rev 1.5   03 Jul 1991 10:30:08   MARKK
 * Converted to large model.
 * This removed some of the prior changes.
 * 
 *    Rev 1.4   24 Jun 1991 08:22:26   MARKK
 * Changed the structures to include the formerly static data.  And
 * added an id to the menu items structure.
 * 
 *    Rev 1.3   28 Feb 1991 19:00:38   BARRY
 * No change.
 * 
 *    Rev 1.2   28 Feb 1991 17:58:36   BARRY
 * No change.
 * 
 *    Rev 1.1   28 Feb 1991 17:47:30   BARRY
 * Added MenuBarRec flag DISABLE_HELP.  This can be masked with other
 * MenuBarRec flags to disable all menu item help.
 * 
 *    Rev 1.0   29 Aug 1990 11:23:50   DAVID
 * Initial revision.
 */

#ifndef INCL_PULLDOWN
#define INCL_PULLDOWN

#ifndef INCL_TYPES
#include <types.h>
#endif

#ifndef INCL_DRAW
#include <draw.h>
#endif

#ifndef INCL_WINMGR
#include <winmgr.h>
#endif

/*----------------------------------------------------------------------*
 * The following structure contains the definitions for a single item	*
 * in a pull-down menu.							*
 *									*
 * You only need to define the first field, though it is more typical	*
 * to initialize all of them.						*
 *----------------------------------------------------------------------*/

typedef struct 
    {
    char	   *item_string;	/* String for this item		     */
    char	   *help_string;	/* Help string for this item	     */
    int		    help_entry;		/* Number of help file entry	     */
    Boolean	    item_check;		/* TRUE if it's checked		     */
    Boolean	    item_disabled;	/* TRUE if disabled		     */
    int		    item_alias;		/* Character equiv for item	     */
    Word	    item_ID;		/* The ID for this item		     */
    Word	    wFlags;		/* Record flags			     */
    struct MenuRec *heirMenu;		/* The next menu		     */
    }
    MenuItemRec;			/* BAR_ITEM                     */

#define ITEM_IMMEDIATE          1       /* Immediate action on pulldown */

/*----------------------------------------------------------------------*
 * This structure defines the data for a single pull-down menu.		*
 *									*
 * menu_string		This is the name that appears on the title bar	*
 * help_string		Status line text when this menu highlighted	*
 * help_entry		Number of NG help entry for menu (0 for index)	*
 * menu_items		Points to a list of items in the pull-down menu	*
 * auto_drop		TRUE to drop the menu whenever it's highlighted	*
 * menu_alias		Key that will cause menu to drop immediately	*
 *			(typically an ALT key, or 0 for none)		*
 *									*
 * All the other variables are filled in by the menu routines.		*
 *----------------------------------------------------------------------*/

typedef struct MenuRec
    {
    char	*menu_string;		/* Title of this menu		     */
    char	*help_string;		/* Help string of this menu	     */
    int		 help_entry;		/* Number of help file entry	     */
    MenuItemRec	*menu_items;		/* List of menu items		     */
    Byte	 auto_drop;		/* TRUE to drop when highlightd	     */
    int		 menu_alias;		/* Key to auto-drop this menu	     */
    Byte	 menu_start;		/* Start position on menu bar	     */
    Byte	 menu_end;		/* End position on menu bar	     */
    Byte	 num_items;		/* Number of items in the menu	     */
    Byte	 item_left;		/* Left side of the item	     */
    Byte	 width;			/* Width of pull-down menu	     */
    Byte	 alias_width;		/* Width of aliases		     */
    int		 num_checks;		/* Num. check marks in menu	     */
    }
    MenuRec;			        /* MENU                         */


/*----------------------------------------------------------------------*
 * This data structure defines the global characteristics for the	*
 * entire menu bar.							*
 *									*
 *	row		The line the menu bar's on			*
 *	leftCol		The left side of the                            *
 *	rightCol	The right side of the menu bar			*
 *	box		The box to use around the pull-down menu	*
 *			(use NULL for default box)			*
 *	padding		Number of spaces highlighted around title (2)	*
 *	gap		Gap between titles in the menu bar (0)		*
 *----------------------------------------------------------------------*/

typedef struct MenuBarRec
    {
    MenuRec	 *list;			/* Pointer to list of menus	     */
    Word	  flags;		/* Flags to control operation	     */
    Byte	  row;			/* Location of the menu bar	     */
    Byte	  leftCol;		/* Left side of menu bar	     */
    Byte	  rightCol;		/* Right side of menu bar	     */
    BOXES	 *box;			/* Pointer to alternate box	     */
    char	 *help_string;		/* Msg at right side of bar	     */
    Byte	  padding;		/* Number of chars around title	     */
    Byte	  gap;			/* Gap between the titles	     */
    Byte	  offset;		/* Offset of box from left side	     */
    char	 *menuTitle;		// The title of this menu
    MenuRec	 *theMenu;		// The menu currently selected
    int		  menu_down;		/* TRUE if a menu is down.	     */
    int		  menu_bar_visible;	/* TRUE when menu bar visible	     */
    int		  selected_menu;	/* Number of menu highlighted	     */
    int		  selected_item;	/* Number of item selected	     */
    int		  num_menus;		/* Number of pull-down menus	     */
    RECT	  savedRect;		/* Dimensions of current menu	     */
    int		  old_selected_menu;	/* Last menu used		     */
    int		  old_selected_item;	/* Last used item		     */
    Word	  id;			// ID of the last item selected
    struct MWindowRec far *mWindow;	// The window this menu resides in
    }
    MenuBarRec;				/* MENU_BAR			     */


#define	ESC_OUT		 0x0001  	/* ESC always exits all the way	*/
					/* out, instead of just closing	*/
					/* the current menu	*/

#define ALT_OUT		 0x0002		/* Can exit menu bar by toggling*/
					/* the Alt key	                */

#define DISABLE_HELP     0x0004         /* Disables all menu item help  */

#define MTYPE_CLOSEBOX	 0x0008		/* Menu bar type is a close box */

#define MTYPE_NOCLOSEBOX 0x0010		/* Don't draw the close box of  */
					/* a menu if it has a title.    */
					/* This should be used in 	*/
					/* conjunction with a system 	*/
					/* menu which uses		*/
					/* MTYPE_CLOSEBOX 		*/

#define MTYPE_TITLE_ONLY 0x0020		/* for those weird days when	*/
					/* all you really want is a 	*/
					/* title bar that behaves like	*/
					/* a menu bar			*/

/*----------------------------------------------------------------------*
 * The following constants are useful for initializing menu structures	*
 *----------------------------------------------------------------------*/

#define CHECKED		 1		/* For MenuItemRec.item_check	     */
#define UNCHECKED	 0

#define	ENABLED		 0		/* For MenuItemsRec.item_disabled    */
#define DISABLED	 1

#define AUTO_DROP	 1		/* For MenuRec.auto_drop	     */
#define NORMAL_DROP	 0

#define UNCHANGED	 0xFF		/* For PulldownSetItemState(...)     */

#define MENU_DIVIDER  {"-",NULL,0,UNCHECKED,DISABLED,0}


#define PulldownEnableItemID(menuBar, id, disabled)  PulldownSetMultiItemState( menuBar, id, disabled, UNCHANGED, FALSE )
#define PulldownCheckItemID(menuBar, id, checked )  PulldownSetMultiItemState( menuBar, id, UNCHANGED, checked, FALSE )

#define PulldownEnableMultiItemID(menuBar, id, disabled)  PulldownSetMultiItemState( menuBar, id, disabled, UNCHANGED, TRUE )
#define PulldownCheckMultiItemID(menuBar, id, checked )  PulldownSetMultiItemState( menuBar, id, UNCHANGED, checked, TRUE )

#define HOT_KEY_INDICATOR	'^'	/* Indicates next char as a hotkey */

#define	ALT_STAYS_IN_MENU	1

#define HEIR_MENU_POPUP_TIME	12	/* A little over half a second */

#define RIGHT_CURSOR_PRESSED	-2

#define PD_NO_ITEM              -1      /* no item selected */
#define PD_NO_MENU              -1      /* no menu selected */

#define PD_CONTROL_ID_CLOSE	    	((Word)60000)
#define PD_CONTROL_ID_MOVE	    	((Word)60001)
#define PD_CONTROL_ID_WINCLOSE		((Word)60002)
#define PD_CONTROL_ID_SWITCHTO		((Word)60003)
#define PD_CONTROL_ID_ABOUT		((Word)60004)

#endif
