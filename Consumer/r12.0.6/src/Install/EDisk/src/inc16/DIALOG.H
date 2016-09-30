/* Copyright 1991 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   V:/INCLUDE/VCS/DIALOG.H_V   1.73   31 Dec 1992 11:36:10   REESE  $ *
 *                                                                      *
 * Description:                                                         *
 *  Constants and data structures used by dialog manager.               *
 *                                                                      *
 * See Also:                                                            *
 *  None.                                                               *
 *                                                                      *
 ************************************************************************ 
 *
 * $Log:   V:/INCLUDE/VCS/DIALOG.H_V  $
 * 
 *    Rev 1.73   31 Dec 1992 11:36:10   REESE
 * Added arg_num extern from dlgvars.
 * 
 *    Rev 1.72   16 Oct 1992 18:58:28   JOHN
 * Added ABOUT_ON_SYS_MENU flag and lpfnSysMenuAboutHook extern
 * 
 *    Rev 1.71   03 Sep 1992 10:20:30   CRAIG
 * Added MULT_SELECT_LIST flag.
 * 
 *    Rev 1.70   06 Aug 1992 10:46:44   CRAIG
 * Added TextEditIsEnabled() macro.
 * 
 *    Rev 1.69   22 May 1992 14:58:50   KEVIN
 * TE_NO_GREY, to prevent greying out if disabled
 * 
 *    Rev 1.68   04 May 1992 17:29:56   ED
 * Added TE_USER_DEF
 * 
 *    Rev 1.67   31 Mar 1992 18:04:54   ED
 * SB_NO_ADJUST
 * 
 *    Rev 1.66   10 Mar 1992 10:41:36   ED
 * Removed the "disabled" field from the ComboBoxRec and replaced its
 * purpose with the CBS_DISABLED flag.  The "disabled" field is now
 * "matchKey" for owner-draw key matching purposes.
 * 
 * Added the COMBO_BOX_LIST flag to the set of list flags.  This is set
 * automatically by the combo box code to indicate to whoever needs to
 * know that the list is in a combo.
 * 
 *    Rev 1.65   28 Feb 1992 12:51:32   DAVIDH
 * Added the constant NOT_A_BUTTON.
 * 
 *    Rev 1.64   26 Feb 1992 10:59:30   ED
 * Added the ClearSkipFlag, SetSkipFlag, ClearHotkeyFlag, and
 * SetHotkeyFlag macros.
 * 
 *    Rev 1.63   17 Feb 1992 13:14:56   HENRI
 * Added DIF_TE_EXPANDED.
 * 
 *    Rev 1.62   14 Feb 1992 15:09:46   HENRI
 * Added some prototypes.
 * 
 *    Rev 1.61   10 Feb 1992 14:27:04   ED
 * Added RadioButtonsFieldEnable
 * 
 * 
 *    Rev 1.60   30 Jan 1992 18:08:22   ED
 * Modified the button record and added the ACCEPT_CURRENT flag
 * 
 *    Rev 1.59   29 Jan 1992 14:31:10   PETERD
 * Increased dialog stack to 15.
 * 
 *    Rev 1.58   27 Jan 1992 22:36:38   TORSTEN
 * Changed the #include statements to use angle brackets.
 * 
 *    Rev 1.57   12 Jan 1992 15:20:24   HENRI
 * Added SB_FORCE_THIN_TO_RIGHT.
 * 
 *    Rev 1.56   30 Dec 1991 20:13:58   HENRI
 * Added the hotkey field to DIALOG_ITEM record.
 * 
 *    Rev 1.55   30 Dec 1991 18:15:58   HENRI
 * Added Hotkey field to DIALOG_ITEM.
 * 
 *    Rev 1.54   13 Dec 1991 15:40:08   BARRY
 * Added the SB_FORCE_THIN flag.  This will force the scroll bar to
 * be thin, even in full graphics mode.
 * 
 *    Rev 1.53   10 Dec 1991 10:35:56   ED
 * Added LIST_ADDED_... and the NO_ZERO macro, which were both removed
 * from the List Manager code
 * 
 *    Rev 1.52   01 Dec 1991 12:51:52   ED
 * Added typedefs for all pointer procedures.  These are enabled by
 * #defining PROC_TYPEDEFS before including this file.
 * 
 *    Rev 1.51   08 Nov 1991 12:29:06   HENRI
 * Added OWNER_WIDTH_SET flag definition for the list record.
 * 
 *    Rev 1.50   29 Oct 1991 16:27:14   ED
 * Added NORMAL_DIALOG_FLAGS
 * 
 *    Rev 1.49   10 Oct 1991 13:14:10   MARKK
 * Added HAS_MENU_BAR flag
 * 
 *    Rev 1.48   10 Oct 1991 12:45:30   ED
 * Added LIST_DBLCLK as a return value from DialogGetUniqueButtonNumber().
 * 
 *    Rev 1.47   26 Sep 1991 10:17:38   ED
 * Added the FAR_STRING_FIELD format specifier
 * 
 *    Rev 1.46   20 Sep 1991 16:01:46   DOUG
 * added ButtonFieldEnable
 * 
 * 
 *    Rev 1.45   09 Sep 1991 17:34:52   BASIL
 * Added prototype for ButtonEnableWithCheck() (..\controls\buttons.c)
 * 
 *    Rev 1.44   06 Sep 1991 07:48:12   ED
 * Added a definition for okCancelVertButtons
 * 
 *    Rev 1.43   04 Sep 1991 13:09:40   ED
 * Locking this puppy up.  Who knows what I did....
 * 
 *    Rev 1.42   28 Aug 1991 20:31:26   ED
 * Changed ComboBoxInitHistory to ComboBoxHistoryInit
 * 
 *    Rev 1.41   28 Aug 1991 09:59:44   ED
 * Lots of modifications.  The last being the modification of the Scroll
 * Bar record with a disabled field.
 * 
 *    Rev 1.40   16 Aug 1991 07:30:40   ED
 * Added a couple of new button functions
 * 
 *    Rev 1.39   15 Aug 1991 10:20:26   ED
 * Added the CBS_WAS_DROPPED flag
 * 
 *    Rev 1.38   12 Aug 1991 11:28:28   BARRY
 * Added the format specifier skip fields.
 * 
 *    Rev 1.37   12 Aug 1991 07:41:48   ED
 * Added SKIP_FLAGS and INACTIVE
 * 
 *    Rev 1.36   07 Aug 1991 19:31:22   PETERD
 * Added CENTER_HORIZONTAL flag to NORMAL_DIALOG_FLAGS.
 * 
 *    Rev 1.35   06 Aug 1991 08:42:30   ED
 * Added TextEditEnable(), ButtonEnable(), RadioButtonsEnable() and some
 * other assorted gems.
 * 
 *    Rev 1.34   05 Aug 1991 11:09:22   ED
 * Changed "Byte far *" in the list prototypes to be "void far *".
 * 
 *    Rev 1.33   04 Aug 1991 13:22:52   MARKK
 * Added prototypes for list callbacks.
 * 
 *    Rev 1.32   02 Aug 1991 16:08:56   MARKK
 * Added POP_UP_BOX dialog flag to prevent the previous dialog from
 * being drawn inactive.
 * 
 *    Rev 1.31   02 Aug 1991 10:01:42   ED
 * Added "disabled" and "reserved" fields to most control records.
 * 
 * Added ButtonEnable() as a function prototype
 * 
 *    Rev 1.30   02 Aug 1991 08:47:32   MARKK
 * Added a window pointer to the scroll bar record to facilitate
 * placing a scroll bar in its own window.
 * 
 *    Rev 1.29   01 Aug 1991 12:21:08   MARKK
 * Added DialogMoveWindow prototype.
 * 
 * 
 *    Rev 1.28   31 Jul 1991 19:44:46   JOHN
 * Added DIALOG_CONDITIONAL_SKIP.  This skips the line in graphical mode
 * and inserts a blank line in non-graphical.  This should help us fine
 * tune the look of certain dialogs.
 * 
 *    Rev 1.27   31 Jul 1991 19:29:42   JOHN
 * Added support for text edit field validation functions.
 * 
 *    Rev 1.26   24 Jul 1991 18:16:34   HENRI
 * Added the CB_DISABLED flag for CheckBoxRec.
 * 
 *    Rev 1.25   24 Jul 1991 16:05:34   ED
 * Added the getString field to the ComboBoxRec
 * 
 * Added prototypes for ComboBoxGetSelection() and 
 * ComboBoxSetSelection().
 *  
 * 
 *    Rev 1.24   24 Jul 1991 16:03:04   MARKK
 * Fixed prototypes of callback functions.
 * 
 *    Rev 1.23   10 Jul 1991 14:02:58   HENRI
 * Changed combo box flags so they don't conflict with USE_VALUE and USE_PAD 
 * flags.
 * 
 *    Rev 1.22   09 Jul 1991 15:43:48   MARKK
 * Changed the data field in UCR be void far *.
 * 
 *    Rev 1.21   03 Jul 1991 10:31:36   MARKK
 * Converted to large model.
 * 
 *    Rev 1.20   21 Jun 1991 12:59:14   HENRI
 * Added the ComboBoxRec and its flags.
 * 
 *    Rev 1.19   25 Apr 1991 11:35:44   PETERD
 * Added NO_TASK_SWITCH flag to dialog flags.
 * 
 *    Rev 1.18   02 Apr 1991 13:44:42   ED
 * Commented out the duplicate prototypes inserted in the last
 * modification.  The originals are located in INTERNAL.H
 * 
 *    Rev 1.17   01 Apr 1991 14:30:44   DOUG
 * more function prototypes
 * 
 *    Rev 1.16   01 Apr 1991 14:27:48   DOUG
 * added missing BUTTON.C function prototypes
 * 
 *    Rev 1.15   28 Mar 1991 12:02:24   PETERD
 * Added a new dialog flag (NO_UPDATE_BORDER) to prevent the border
 * type updating used to indicate active/inactive dialogs.
 * 
 *    Rev 1.14   26 Mar 1991 11:34:12   ED
 * Added the CLEAR_SCROLL_BAR_COL flag to the list box flags
 * 
 *    Rev 1.13   26 Mar 1991 09:56:20   ED
 * Added the SB_NO_GRAPHICS flag for scroll bars
 * 
 *    Rev 1.12   25 Mar 1991 16:47:32   ED
 * Added the NO_CLOSE_BOX flag.
 * 
 * Added the openColors field in DIALOG_RECORD.  This tracks the colors
 * that a dialog was opened with.
 * 
 *    Rev 1.11   22 Mar 1991 11:41:18   ED
 * Added TITLE_LEFT flag to ListRec.
 * 
 *    Rev 1.10   21 Mar 1991 18:50:20   HENRI
 * Added a title field (Byte *) to the List Record.
 * 
 *    Rev 1.9   21 Mar 1991 07:10:36   ED
 * Added SCROLL_BAR_WIDTH
 * 
 *    Rev 1.8   27 Feb 1991 16:22:04   HENRI
 * Added flags for the new text edit fields: TE_NO_BRACKETS & TE_NO_DOTS.
 * Added #define for CONTINUE_DIALOG.
 * 
 *    Rev 1.7   21 Feb 1991 12:08:42   ED
 * Added the TE_CONVERT_CR flag.
 * 
 *    Rev 1.6   20 Feb 1991 12:02:18   JOHN
 * Added SCROLL_LIST flag for ListRec.
 * 
 *    Rev 1.5   07 Feb 1991 13:51:08   ED
 * Modified ButtonsEvent() to take an additional int argument
 * 
 *    Rev 1.4   07 Feb 1991 13:25:44   ED
 * Changed description of TWO_LINES button flag.
 * Added function prototypes for GETITEM.C functions.
 * Added extern for DIALOG_4COLS[].
 * 
 *    Rev 1.3   09 Jan 1991 17:23:28   ED
 * Added the ACCEPT_SPACE, NO_SPACEBAR, and TWO_LINES flags as new
 * button flags.
 * 
 *    Rev 1.2   20 Dec 1990 08:23:54   ED
 * Added SHOW_HOT_KEYS flag to the button section.  This is to
 * support displaying the hot keys under the new 6.0 button scheme.
 * USE_BRACKETS was removed and its value assumed by SHOW_HOT_KEYS.
 * 
 *    Rev 1.1   12 Nov 1990 10:53:34   HENRI
 * Added the function prototype definition for the
 * ListHighlight() hook routine called _ListHighlight().
 * See the revision comments in source file LIST.C for
 * information.
 * 
 *    Rev 1.0   29 Aug 1990 11:24:50   DAVID
 * Initial revision.
 ************************************************************************/

#ifndef INCL_DIALOG
#define INCL_DIALOG

#ifndef INCL_TYPES
#include <types.h>
#endif

#ifndef INCL_DRAW
#include <draw.h>
#endif

#ifndef INCL_VMM
#include <vmm.h>
#endif

#define NO_ZERO(pNum)    { if (!(*(pNum))) *(pNum) = 1; }

#define LIST_ADDED_COL       1       // add extra column for new scroll bar
#define LIST_ADDED_ROW       1       // add extra row for new scroll bar

#define ClearSkipFlag(i)        ((i)->flags &= ~SKIP)
#define SetSkipFlag(i)          ((i)->flags |= SKIP)
#define ClearHotkeyFlag(i)      ((i)->hotkey &= 0x7F)
#define SetHotkeyFlag(i)        ((i)->hotkey |= 0x80)

/*----------------------------------------------------------------------*
 * These are constants used when filtering out drives to be displayed	*
 * in the StdDlgSelectDrive() routine.  				*
 *----------------------------------------------------------------------*/
#define	FILTER_ALL_DRIVES	0	/* All (logical) drives 	*/
#define	FILTER_LOGICAL_ONLY	1	/* No net, assigned, subst-ed	*/
#define FILTER_SPECIAL		2	/* LOGICAL_ONLY & filter proc	*/
#define FILTER_CUSTOM		3	/* ALL_DRIVES & filter proc	*/

/*----------------------------------------------------------------------*
 * These are return constants, after returning from dialogs.		*
 *----------------------------------------------------------------------*/

#define		DIALOG_YES		0
#define		DIALOG_NO		1

#define		OK			0
#define		CANCEL			1

/*----------------------------------------------------------------------*
 * These flags are used by Check Boxes, Radio Buttons, and Buttons      *
 *----------------------------------------------------------------------*/

#define		USE_VALUE	1	/* Don't reset value to 0	*/
#define		USE_PAD		2	/* Use existing pad and gap	*/

/*----------------------------------------------------------------------*
 * Check box record							*
 * Used for turning a flag on and off.  It looks like this:		*
 *									*
 *		     (row, col)						*
 *		       \						*
 *			[ ]   Text goes here				*
 *			   <->						*
 *			    pad						*
 *----------------------------------------------------------------------*/

typedef struct
    {
    Byte	*name;		/* The text to right of the check box	*/
    Word	 flags;		/* Reserved for future use		*/
    Boolean	 value;	  	/* State of check box; TRUE = checked	*/
    Byte	 pad;	  	/* Spacing between the box and text	*/
    Byte	 row, col;	/* Internal use - coordinates of box	*/
    } CheckBoxRec;

#define CB_USE_VALUE	0x01	/* Use preset value			*/
#define CB_USE_PAD	0x02	/* Use PAD value			*/
#define CB_DISABLED	0x04	/* Disabled check box control		*/
#define CB_THREE_STATE	0x08	/* Three state check box control	*/

#define	CB_STATE_OFF		0	// Checkbox values
#define	CB_STATE_ON		1
#define	CB_STATE_DONT_CARE	2	// (three-state only)
#define	CB_STATE_IGNORE		2	// (three-state only)

/*----------------------------------------------------------------------*
 * Radio button record							*
 * Used for selecting one item out of list.  Here's how it looks:	*
 *									*
 *	     (row, col)	     						*
 *	       \		     					*
 * 		(*)    First Label					*
 * 		( )    Second Label					*
 * 		( )    Third Label					*
 * 		( )    Fourth Label					*
 *		   <-->							*
 *		    pad							*
 *----------------------------------------------------------------------*/

typedef struct
    {
    Byte      **names;		/* List of button labels		*/
    Word	flags;	       	/* Bit flags to control operation	*/
    Byte	value;	       	/* Active button number (0 for first)	*/
    Byte	pad;	       	/* Spacing between button and label	*/
    Byte	gap;	       	/* Spacing between buttons		*/
    Byte	numButtons;    	/* Internal use - number of buttons	*/
    Byte	row, col;      	/* Internal use - coordinates		*/
    Byte	cursorPos;	/* Internal use - HW cursor position  	*/
    Byte	disabled;	/* internal use - disabled bitflags	*/
    Word	reserved;	/* for future use			*/
    } RadioButtonsRec;

#define	ARRANGE_HORIZONTAL	4	// do horizontal buttons

/*----------------------------------------------------------------------*
 * User-defined control record						*
 * Used for creating customized controls.				*
 *----------------------------------------------------------------------*/

typedef struct UCR
    {
    Word	flags;
    void	(*initControl) (struct UCR *r, Word *height, Word *width);
    void	(*drawControl) (struct UCR *r, Boolean firstTime, Boolean active);
    int 	(*handleEvent) (struct UCR *r, Word *c);
    void	FAR *data;
    Byte	row, col;
    RECT	rect;
    } UserControlRec;

#ifdef	PROC_TYPEDEFS
typedef	void (*INITCONTROLPROC)(struct UCR *r, Word *height, Word *width);
typedef	void (*DRAWCONTROLPROC)(struct UCR *r, Boolean firstTime, Boolean active);
typedef	int  (*HANDLEEVENTPROC)(struct UCR *r, Word *c);
#endif

/*----------------------------------------------------------------------*
 * Text-edit record							*
 * Used to create fields where text can be entered.			*
 *----------------------------------------------------------------------*/

typedef struct TERec
    {
    Byte	row, col;	     	/* Left side of the edit box	*/
    Byte 	width;			/* Width of the edit box	*/
    char	*string;	     	/* The string we're editing	*/
    int		max_len;	     	/* Maximum length of the string	*/
    int		insert_pt;	     	/* Insertion point		*/
    int		offset;			/* First char visible in box	*/
    Word	(*validate)(struct TERec *te, Word *c);
    Word	flags;	   	    	/* Bit flags			*/
    Word	(*checkField)(struct TERec *pTE);
    Word	reserved;		/* for future use		*/
    } TERec;

typedef TERec TE_RECORD;		/* Retain old structure name	*/

#ifdef	PROC_TYPEDEFS
typedef	Word (*VALIDATEPROC)(struct TERec *te, Word *c);
typedef	Word (*CHECKFIELDPROC)(struct TERec *pTE);
#endif

#define		TE_PSTRING	0x0001	/* Edit Pascal-style string	*/
#define		TE_NO_ECHO	0x0010  /* Echo stars instead of chars  */
#define		TE_NO_STRIP	0x0020  /* Don't strip spaces           */
#define		TE_CONVERT_CR	0x0040  /* Convert ENTER to CTRL-ENTER	*/
#define		TE_NO_DOTS	0x0080  /* Use spaces instead of dots	*/
#define		TE_NO_BRACKETS	0x0100  /* Disables brackets around text*/
#define		TE_DISABLED	0x0200  /* Disables the edit control	*/
#define		TE_NO_GREY	0x0400  /* Not greyed even if disabled	*/

		/* The following flags are used internally:		*/
#define		TE_UPDATED	0x4000  /* Set when field is changed	*/
#define         TE_USER_DEF     0x8000  /* reserved for temporary flags */

		/* These flags are used by text_edit(), 		*/
		/* which is essentially obsolete			*/
#define		TE_CONTINUE	0x0001	/* Don't reset cursor to start	*/
#define		TE_CURSOR	0x0002	/* Can move between fields	*/
#define		TE_FIRST_TIME	0x0004	/* First time in field flag	*/
#define 	TE_CHANGED	0x0008	/* text edit field dirty bit	*/

#define TextEditIsEnabled(t) ((Boolean) (((t)->flags & TE_DISABLED) == 0))

/*----------------------------------------------------------------------*
 * Output field record							*
 * Used to create a dialog field that can easily be updated by the 	*
 * program.  								*
 *----------------------------------------------------------------------*/

typedef struct
    {
    Byte	type;		/* Substitution letter: c,s,d,l,x,m,p	*/
    Word	flags;		/* Flags to control operation		*/
    Byte	maxWidth;	/* Maximum size of field		*/
    void	*data;		/* Data, like that passed to DialogBox()*/
    Boolean	update;		/* Set to have DialogBox() redraw field	*/
    Byte	row, col;	/* Internal use - coordinates of field	*/
    Word	reserved;	/* for future use			*/
    } OutputFieldRec;

#define LEFT_FIELD	     1	/* Left justify text within field	*/
#define RIGHT_FIELD	     2	/* Right justify text within field	*/
#define BRIGHT_FIELD	     4	/* Draw output text in bright attribute	*/
#define NO_FIRST_DRAW	     8	/* Don't draw when dlg is first put up	*/ 
#define REVERSE_VIDEO_FIELD 16	/* Display output in reverse video	*/
#define HOTKEY_FIELD	    32  /* Draw first capital letter bright	*/ 

/*----------------------------------------------------------------------*
 * Buttons record							*
 * Used to create a row of buttons, usually at the bottom of the dialog.*
 *									*
 * Here is a description of the information that you need to fill in:  	*
 *									*
 *	strings		Pointer to a list of button names.  The last	*
 *			name must be a null string: "".			*
 *									*
 *	result		A null-terminated list of characters to return	*
 *			for each of the buttons in the list.		*
 *									*
 *			If this list is empty, returns CR for the first	*
 *			item and ESC for the last item.			*
 *									*
 *	value		The initial value to use for the button		*
 *									*
 * The rest of the data is internal, and the dialog routines fill it	*
 * in automatically.							*
 *----------------------------------------------------------------------*/

typedef struct BUTTONS
    {
    Byte	**strings;	/* A list of the buttons	        */
    int		flags;		/* Flags that control buttons	        */
    Byte	value;		/* Current button selected	        */
    Byte	row, col;	/* Screen location of button	        */
    Byte 	pad, gap;	/* Spacing around each button	        */
    Byte	disabled;	/* internal use - disabled bitflags     */
    Byte	defaultValue;	/* internal use - default button        */
    Byte        reserved;	/* for future use		        */
    } ButtonsRec;

typedef ButtonsRec BUTTONS;		/* Retain old structure name	*/

#define		USE_ARROWS	    0	// obsolete

#define		ACCEPT_SPACE	    0	// used to enable space bar, but that
					// is now the default action.  use
					// NO_SPACEBAR to disable the spacebar

#define		SHOW_HOT_KEYS	    8	/* show hot keys in buttons */
#define		BOUNCE_BACK	   16	/* Back to old item if mouse is	*/
					/* dragged off button & released*/
#define		UNBOXED	           32	/* Don't surround each button	*/
					/* with single-line box		*/
#define		ARRANGE_VERTICAL   64	/* Stack on top of each other	*/
#define		NO_WRAP		  128	/* No wrap around end of list	*/
#define		NO_DEFAULT	  256	/* For multiple button sets	*/

#define		NO_SPACEBAR       512	// space bar presses the button

#define		NO_ARROWS	 1024	// show arrows instead of color change
					// to show default button

#define		TWO_LINES	 2048	// uses a single line between vertical
					// buttons (instead of double)

#define         ACCEPT_CURRENT   4096   // CR will cause an ACCEPT_DIALOG,
                                        // using the currently selected button

#define NORMAL_BUTTON_FLAGS	SHOW_HOT_KEYS


#define	NOT_A_BUTTON	(Word)(-1)      // A DialogGetUniqueButtonNumber()
                                        // return value

/*----------------------------------------------------------------------*
 * Here is the definition for a scroll bar.				*
 *									*
 * blank_char is the character used for a blank scroll bar.  		*
 * For example, this could be a double vertical line, or a space.  	*
 * Blank scroll bars are drawn whenever everything fits onto one page.	*
 *									*
 * NOTE: blank_char is no longer used					*
 *----------------------------------------------------------------------*/

typedef struct 
	{
	Byte	col;			/* X-coordinate of scroll bar	*/
	Byte	top;			/* Top of the scroll bar	*/
	Byte	bottom;			/* Bottom of the scroll bar	*/
	Word    flags; 			/* Bit flags to modify behavior	*/
	char	blank_char;		/* Char to use when no bar	*/

	Ulong	numPageValues;		/* == (numValues - pageSize)  	*/
	Ulong	value;			/* The current value		*/
	Word	thumbSize;		/* Rows used by elevator box	*/
	Word	thumbPosition;		/* Top row of elevator box	*/
	Word    numThumbPositions; 	/* 0 if fills entire bar, etc	*/
	struct MWindowRec *pWindow;	// The window containing this scroll
					// bar
	Byte	disabled;		/* disabled flag		*/
	Byte	reserved;		/* for future use		*/
	} ScrollBarRec;

typedef ScrollBarRec SCROLL_BAR;	/* Retain old structure name	*/

#define	SCROLL_BAR_WIDTH	2	/* width in characters		*/

#define SB_NEEDS_REDRAW		1	/* For redraw			*/
#define	SB_NO_GRAPHICS		2	/* don't draw graphical bars	*/
#define SB_FORCE_THIN           4       /* Force a thin scroll bar      */
#define SB_FORCE_THIN_TO_LEFT	8	/* Force thin scroll bar	*/
					/* without adjusting it by 1 col*/
#define SB_NO_ADJUST            16      /* don't adjust the column      */

/*----------------------------------------------------------------------*
 * This data structure is used by the list manager routines.		*
 *									*
 * All of the fields in the second group are filled in by the list	*
 * routines, and are for internal use only.				*
 *									*
 *	row, col	Top, left side of the list			*
 *	num_columns	Number of columns in the list			*
 *	column_width	Width of each column				*
 *	height		Height of the list				*
 *	write_entry()	The procedure that writes a single entry on	*
 *			the screen.					*
 *	entries		This is a pointer to an array of entries.	*
 *	entry_size	Number of bytes for each element of the entries	*
 *			array.						*
 *	scroll_bar	Pointer to the scroll bar, or NULL if no bar	*
 *									*
 * The list routines call write_entry() to display a single entry.  The	*
 * calling convention is:						*
 *									*
 *	write_entry(list, entry, attr);					*
 *  where								*
 *	list		Pointer to the list we're working with		*
 *	entry		Pointer to entry to write			*
 *	attr		Attribute to use when we write to screen	*
 *									*
 * The list routine list_mouse() calls do_mouse() with the following	*
 * parameters whenever there is a mouse down event in a list item:	*
 *									*
 *	do_mouse(list, entry_num, rect, buttons, clicks);		*
 *  where								*
 *	list		Pointer to the list we're working with		*
 *	entry_num	Pointer to the entry mouse was on		*
 *	rect		Pointer to rectangle around the entry		*
 *	buttons		Which mouse buttons are down			*
 *	clicks		Number of mouse clicks (1 or 2)			*
 *  returns								*
 *	0		Handle (or didn't) this mouse event		*
 *	1		Selected entry with left button			*
 *	2		Selected entry with right button		*
 *	3		Canceled selection				*
 *----------------------------------------------------------------------*/

typedef struct ListRec
	{
	Byte 	row, col;		/* Top, left of the list	*/
	Byte 	num_columns;		/* Number of columns in list	*/
	Byte 	column_width;		/* Width of each column		*/
	Byte 	height;			/* Height of the list		*/
	void	(*write_entry)(struct ListRec *, void far *, int, Ulong);
					/* Routine to write an entry	     */
	int	(*do_mouse)(struct ListRec *, Ulong, RECT *, Word, int);
					/* Handle a mouse event		     */
	int	(*check_match)(struct ListRec *, void far *, Word, Ulong);
					/* Checks match for list_search	     */
	Word	(*event_filter)(struct ListRec *, Word *);
					/* Screen dialog events		     */
	void 	far *entries;		/* Pointer to array of entries	*/
	int	entry_size;		/* Number of bytes per entry	*/
	int	flags;			/* Flags to control operation	*/
	SCROLL_BAR *scroll_bar;		/* NULL if no scroll bar	*/
	char	*separator;		/* String between columns	*/
	Byte    delay1, delay2;		/* Tick delays between scrolls	*/

	Ulong	num;			/* Number of list entries	*/
	Ulong	top_entry;		/* First entry visible in list	*/
	Ulong	active_entry;		/* The highlighted entry	*/
	Byte 	separator_width;	/* Width of column separator	*/
	Byte   *title;			/* List title (optional)	*/
	void far *userData;		/* generic pointer for user	*/
	Byte    width;			/* Width of the entire list	*/
	RECT	rect;			/* Rectangle around the list	*/
	Byte	itemHeight;		// The height in lines of each item
	Word	reserved;		/* for future use		*/
	} ListRec;

#ifdef	PROC_TYPEDEFS
typedef	void (*WRITEENTRYPROC)(struct ListRec *, void far *, int, Ulong);
typedef	int  (*DOMOUSEPROC)(struct ListRec *, Ulong, RECT *, Word, int);
typedef	int  (*CHECKMATCHPROC)(struct ListRec *, void far *, Word, Ulong);
typedef	Word (*EVENTFILTERPROC)(struct ListRec *, Word *);
#endif

typedef ListRec LIST;			/* Retain old structure name	*/

#define	LIST_WRAP	        0x0001	/* Wrap at the ends of list	*/

#define	MATCH_TO_CR	        0x0002	/* Treats match in list_box	*/
					/* as CR 			*/ 

#define	THIN_LIST_BOX	        0x0004	/* In dialogs: draw a single 	*/
					/* line box around the list	*/

#define	ACCEPT_DCLICKS	        0x0008	/* In dialogs: accept dialog if	*/
					/* do_mouse() returns 1 or 2;	*/
					/* for the default do_mouse()	*/
					/* this means double left or	*/
					/* single right click.		*/

#define	FIT_DIALOG	        0x0010  /* In dialogs: kludge to force	*/
					/* lists w/scroll bars to fit	*/
					/* inside dialog box exactly, 	*/
					/* w/scroll bar on frame.	*/

#define	USE_ACTIVE	        0x0020	/* Don't reset top or active	*/
					/* entries when initialized	*/

#define	MOVE_MODE	        0x0040 	/* Physically move active entry	*/
					/* in array when user moves the	*/
					/* selection bar		*/

#define	SPACE_TOGGLES_MODE      0x0080	/* Spacebar toggles move mode	*/
					/* (also certain mouse clicks)	*/

#define	SCROLL_LIST	        0x0100	/* List scrolls up and down,	*/
					/* does not have item bar	*/

#define	CR_ACCEPTS	        0x0100	/* OBSOLETE -- DO NOT USE	*/

#define	TITLE_LEFT	        0x0200	/* left-justifys the title 	*/
					/* rather than centering it 	*/

#define	CLEAR_SCROLL_BAR_COL	0x0400	/* clears an extra column for 	*/
					/* dialogs that don't have	*/
					/* scroll bars			*/

#define	POPUP_LIST	        0x0800	/* defines a popup list		*/

#define	OWNER_WIDTH_SET	        0x1000	/* Indicates that the width of	*/
					/* of the list is preset in the	*/
					/* list structure		*/

#define COMBO_BOX_LIST          0x2000  /* List is part of a combo box  */

#define MULT_SELECT_LIST        0x4000  // A multiple-selection list
                                        // (used by StdListMulti...)

	// Some constants for list boxes

#define	LIST_TOP_ENTRY		(0L)	// first entry in a list box
#define	LIST_NO_SELECTION	(-1L)	// used to indicate no selection

#define	LIST_SINGLE_HEIGHT	1	// itemHeight default value
#define	LIST_DOUBLE_HEIGHT	2	// list with two lines per entry

			// return value from DialogGetUniqueButtonNumber
#define	LIST_DBLCLK		((Word)(-1))

/*----------------------------------------------------------------------*
 * Combo Box control							*
 *----------------------------------------------------------------------*/

typedef struct ComboBoxRec
    {
    Word	 flags;		/* Reserved for future use		*/
    Byte 	 **names;	/* List of combo box labels (optional)	*/
    Byte	 *string;	/* Text Edit string			*/
    Byte	 width;		/* width of text edit.			*/
    Byte 	 height;	/* Height of list			*/
    Ulong	 value;		/* value of combo box (list number)	*/
    TERec	 te;		/* Text Edit Rec			*/
    ListRec	 list;		/* ListRecord				*/
    ScrollBarRec scrollBar;	/* Scroll bar for list record		*/
    Byte	 row, col;	/* Internal use - coordinates of box	*/
				/* Routine to create the edit box entry	*/
    Byte	(*ownerDraw)	(Word message, struct ComboBoxRec *pCombo);
    Byte	matchKey;	/* key to match (owner draw only)       */
    Word	reserved;	/* for future use			*/
    } ComboBoxRec;


#ifdef	PROC_TYPEDEFS
typedef	Byte (*OWNERDRAWPROC)(Word message, struct ComboBoxRec *pCombo);
#endif
	// Some constants for combo boxes

#define	COMBO_TOP_ENTRY		(0L)	// first entry in the combo
#define	COMBO_NO_SELECTION	(-1L)	// used to indicate no selection

// ---------------------------------------------------------------
// Owner-Draw Combo Box Messages
// ---------------------------------------------------------------

				// return code for "no match" after 
				// processing CBN_CHECKMATCH
#define	CBN_NO_MATCH		((Byte)(-1))

#define	CBN_DROPDOWN		100	// notification of a drop down
#define	CBN_EDITCHANGE		101	// notification of an edit update
#define	CBN_SELCHANGE		102	// notification of any change
#define	CBN_CHECKMATCH		103	// notification of a match check

// ---------------------------------------------------------------
// Combo Box Flags
// ---------------------------------------------------------------

#define	CBS_HISTORY_BOX		(CBS_OWNER_DRAW_FIXED | CBS_OWNER_SET_COUNT)
#define CBS_OWNER_DRAW          (CBS_OWNER_DRAW_FIXED | CBS_OWNER_SET_COUNT | CBS_NOTIFY)

#define CBS_OWNER_DRAW_FIXED	0x0004 /* Use your own list write entry   */

#define	CBS_NOEDIT		0x0008 /* Don't allow text editing	  */

#define CBS_OWNER_SET_COUNT	0x0010 /* Set the list.num field yourself */

#define	CBS_NOTIFY		0x0020	/* notify owner on dropdown 	*/
					
#define	CBS_WAS_DROPPED		0x0040	/* list box was dropped down	*/
					/* (this is an internal flag - */
					/* it cannot be set manually)	*/

#define CBS_DISABLED            0x0080  /* combo is disabled            */

/*----------------------------------------------------------------------*
 * Here are the flags for the dialog box structure.			*
 *----------------------------------------------------------------------*/

#define	ALIGN_LEFT	  (1L << 0)	/* Aligns strings along left	*/
#define	ALIGN_CENTER	  (1L << 1)	/* Center the strings		*/
#define	DIALOG_BEEP	  (1L << 2)	/* Beep on entry		*/
#define	CLEAR_BUFFER	  (1L << 3)	/* Clear the keyboard buffer	*/
#define	STRIP_SPACES	  (1L << 4)	/* Del leading & traling spaces	*/
#define	NO_BORDER	  (1L << 5)	/* Don't clr border around dlog	*/
#define	CENTER_VERTICAL	  (1L << 6)	/* Center the dialog box	*/
#define	NO_SHADOW	  (1L << 7)	/* To disable shadows		*/
#define	NO_ZOOM		  (1L << 8)	/* To disable zooming box	*/
#define	RETURN_VALUE	  (1L << 9)	/* Control DialogBox() ret value*/
#define	ALL_ACTIVE	  (1L << 10)	/* All controls active at once	*/
#define	NO_CLEAR	  (1L << 11)	/* Don't clear box interior	*/
#define	NO_BOX		  (1L << 12)	/* Just draw interior of dialog	*/
#define	NO_SAVE		  (1L << 13)	/* Don't save or restore screen area	*/
#define	THIN_BORDER	  (1L << 14)	/* Thin border (FALSE = thick)  */
#define	NO_HELP		  (1L << 15)	/* Don't invoke help system	*/
#define	BOLD_TITLE	  (1L << 16) 	/* Put title in BOLD video	*/
#define	DF_SINGLE_LINE    (1L << 17) 	/* Draw single line box   	*/
#define	NO_CLOSE_BOX	  (1L << 18) 	/* Don't draw a close box 	*/
#define	NO_UPDATE_BORDER  (1L << 19) 	/* Don't redraw border          */
#define NO_TASK_SWITCH    (1L << 20)    /* Don't allow task-switching   */
#define	CENTER_HORIZONTAL (1L << 21)	/* Center the dialog box	*/
#define POP_UP_BOX	  (1L << 22)	// Don't make parent dialog inactive
#define	HAS_MENU_BAR	  (1L << 23)	// This dialog has a menubar, use the
					// menubar colors for dialog title.
#define	NO_STATUS_LINE	  (1L << 24)	// Don't draw status line
#define	NO_CAPTION	  (1L << 25)	// Don't draw title line
#define ABOUT_ON_SYS_MENU (1L << 26)	// Add "About" item to system menu

/*----------------------------------------------------------------------*
 * Default flags for dialog boxes 					*
 *----------------------------------------------------------------------*/

#define NORMAL_DIALOG_FLAGS (ALIGN_CENTER | CENTER_VERTICAL | \
        		     THIN_BORDER | RETURN_VALUE |     \
			     BOLD_TITLE | CENTER_HORIZONTAL)

/*----------------------------------------------------------------------*
 * Here are the types of fields that we can have in dialog boxes.	*
 *----------------------------------------------------------------------*/

#define		ATTR_FIELD	 'a'	/* Attribute modifier		*/
#define		FILL_CHAR_FIELD	 'f'	/* Repeats a character		*/
#define		FILL_LINE_FIELD	 'F'	/* Repeats blank lines		*/
#define		LITERAL_FIELD	 '%'	/* Literal % as in "...%%...\n" */

#define		CHAR_FIELD	 'c'	/* Character field		*/
#define		INT_FIELD	 'd'	/* Integer field		*/
#define		LONG_FIELD	 'l'	/* Long integer field		*/
#define		HEX_FIELD	 'x'	/* Hex number field		*/
#define		LONG_HEX_FIELD	 'm'	/* This is a menu field		*/
#define		PATH_FIELD	 'p'	/* Prints path name		*/
#define		STRING_FIELD	 's'	/* String field			*/
#define		FAR_STRING_FIELD 'e'	/* far String field		*/
#define		GROUP_FIELD	 'g'	/* For rectangle (group)	*/

#define		SPECIAL_TE_FIELD 't'	/* This is a text edit field	*/
#define		RECT_FIELD	 'r'	/* A rectangle field		*/

#define         SKIP_WORD_FIELD  'S'    /* Skip word fields             */
#define         SKIP_PTR_FIELD   'P'    /* Skip pointer fields, ie:     */
                                        /* STRINGS AND LONGS            */

#define		SLIDE_BAR_FIELD	 'A'	/* Slide bar field		*/
#define		BUTTON_FIELD	 'B'	/* This is a button field	*/
#define		CHECK_FIELD      'C'	/* Check box field		*/
#define		LIST_FIELD	 'L'	/* List field			*/
#define		MENU_FIELD	 'M'	/* This is a menu field		*/
#define		OUTPUT_FIELD	 'O'	/* Output field			*/
#define		RADIO_FIELD	 'R'	/* Radio button field		*/
#define		TE_FIELD	 'T'	/* This is a text edit field	*/
#define		USER_FIELD	 'U'	/* User-defined field		*/
#define		SCROLL_BAR_FIELD 'V'	/* Scroll bar field		*/
#define		COMBO_BOX_FIELD  'X'	/* Combe box field		*/

/*----------------------------------------------------------------------*
 * Dialog Formatting Commands						*
 *----------------------------------------------------------------------*/

#define		TOP_ALIGNED	  'T'	/* Align columns to top line    */
#define		MIDDLE_ALIGNED	  'M'	/* Center columns top/bottom    */
#define		BOTTOM_ALIGNED	  'B'	/* Align columns to bottom line */
#define		END_COL_SPLIT	  '1'	/* End a column split		*/
#define		NEXT_COLUMN	  'N'	/* Define strings for next col. */
#define		LEFT_JUSTIFY	  'L'	/* Left justify strings in col. */
#define		RIGHT_JUSTIFY	  'R'	/* Right justify in column	*/
#define		CENTER_JUSTIFY	  'C'	/* Center justify in column	*/
#define		SKIP_LINE	  'S'	/* Skip the current line	*/
#define		CONDITIONAL_SKIP  'G'	/* Either skip or empty line	*/
#define		SINGLE_LINE	  '-'	/* Draw a single horizontal line*/
#define		DOUBLE_LINE	  '='	/* Draw a double horizontal line*/
#define		LITERAL_FORMAT	  '#'	/* Literal # as in "##..."      */
#define	        BEGIN_HELP	  'H'	/* Indicate help line start	*/

/*----------------------------------------------------------------------*
 * This data structure contains the information that we need to work	*
 * with a field in a dialog box.					*
 *----------------------------------------------------------------------*/

struct DIALOG_ITEM
	{
	Byte row,col;			/* Location of TE field		*/
	Byte type;			/* Type of this field		*/
	Byte height;			/* Height of the field		*/
	Byte width;			/* Width of the field		*/
	int  flags;			/* Flags about this field	*/
	void *item;			/* Pointer to item for field	*/
	Byte hotkey;			/* A...Z, 0...9			*/
	};
typedef struct DIALOG_ITEM DIALOG_ITEM;

typedef	DIALOG_ITEM	*DIALOGITEM;

#define		SKIP		1	/* Skip over this field		*/

					// inactive is a way of applying 
					// the SKIP flag without modifying
					// the SKIP flag itself.  They are
					// absolute equivalents otherwise.
#define		INACTIVE	8	/* Skip over this field		*/

					// combine the two skip-flags
					// into one mask
#define		DIF_TE_EXPANDED	16	// This flag is only used in
					// conjunction with the special
					// text edit field.  It is used to
					// to indicate a special text edit 
					// field whose width is actually
					// 2 more than the string length.
					// this adjusts for the brackets.

#define		SKIP_FLAGS	(SKIP | INACTIVE)

#if 0
#define		CENTER_ITEM	2	/* Center item in the field	*/
#define		RIGHT_ITEM	4	/* Right-justify item in field	*/
#endif

/*----------------------------------------------------------------------*
 * This data structure defines dialog boxes.  Fill in this structure	*
 * before you make a call to the procedures that display a dialog box	*
 *----------------------------------------------------------------------*/

typedef struct 
	{
	Byte 	row,col;		/* Location of top, center	*/
	Ulong	flags;			/* Control various attributes	*/
	char	*title;			/* Title of dialog box		*/
	char	**strings;		/* Strings of dialog box	*/
	BUTTONS *buttons;		/* The list of buttons		*/
	Ulong   dwUserData;		// The user data area
	} DialogRec;

typedef DialogRec DIALOG;		/* Retain old structure name	*/

/*----------------------------------------------------------------------*
 * The offset array for formatted strings				*
 *----------------------------------------------------------------------*/

#define	MAX_FORMAT_ARGS	64

/*----------------------------------------------------------------------*
 * Here is the internal data structure that my window libraries use to	*
 * keep track of all dialog boxes on the screen.			*
 *----------------------------------------------------------------------*/

struct DIALOG_RECORD
	{
	DIALOG		d;			// Original information

	int		num_items;		// Number of items in dialog
	int		first_num;		// First item in dialog box
	Byte 		height;			// Height of dialog box
	Byte 		width;			// Width of dialog box
	Byte 		cursorRow, cursorCol;	// Old cursor position
	Word		cursorShape;		// Shape of old cursor

	int		item_num;		// Current item number
	int		old_num;		// The previous item
	DIALOG_ITEM	*item;			// Pointer to current item
	DIALOG_ITEM	*first_item;		// First item in the dialog
        RECT    	saveRect;		// Saved region dimensions
	
	BUTTONS 	*resetButtonSet;	// Set w/ initial deflt	button
	Byte		resetButtonNum;		// Num of deflt button in set
	int		bounceBackItem;		// Saved item, for button use

	COLORS		*openColors;		// Colors it was opened with

	Byte		formatArgOffsets[MAX_FORMAT_ARGS];
	struct MWindowRec far *lpWindow;// The window containing this dialog
	};

typedef struct DIALOG_RECORD DIALOG_RECORD;

typedef DIALOG_RECORD *DIALOGREC;

#define MAX_DIALOGS	15	/* Maximum dialogs active at once	*/
#define MAX_ITEMS	50	/* Total items in all active dialogs	*/

#define EVENT_AVAIL	0	
#define EVENT_USED	1
#define CONTINUE_DIALOG	0	/* Only used by StdEventLoop()		*/
#define ACCEPT_DIALOG	2
#define ABORT_DIALOG	3

#define DLG_TASK_SWITCH 0xF000

/*----------------------------------------------------------------------*
 * Used by DialogArgument						*
 *----------------------------------------------------------------------*/

typedef struct
    {
    unsigned reverse	   : 1;
    unsigned leadingZeros  : 1;
    unsigned activeControl : 1;
    unsigned       	   : 5;
    } ArgumentFlagsRec;

/*----------------------------------------------------------------------*
 * Predefined strings for dialog controls				*
 *----------------------------------------------------------------------*/

extern Byte near DIALOG_1COL[];
extern Byte near DIALOG_2COLS[];
extern Byte near DIALOG_3COLS[];
extern Byte near DIALOG_4COLS[];
extern Byte near DIALOG_NEXT_COL[];
extern Byte near DIALOG_LINE[];
extern Byte near DIALOG_DLINE[];
extern Byte near DIALOG_LEFT[];
extern Byte near DIALOG_RIGHT[];
extern Byte near DIALOG_CENTER[];
extern Byte near DIALOG_SKIP_LINE[];
extern Byte near DIALOG_EMPTY_LINE[];
extern Byte near DIALOG_CONDITIONAL_SKIP[];
extern Byte near DIALOG_HELP[];
extern Byte near DIALOG_END[];

/*----------------------------------------------------------------------*
 * Predefined strings for dialog controls.				*
 * These structures are defined in the language dependent files		* 
 *----------------------------------------------------------------------*/

extern BUTTONS yesNoButton;
extern BUTTONS okButton;
extern BUTTONS okCancelButton;
extern BUTTONS okCancelVertButtons;

#ifdef	PROC_TYPEDEFS
typedef	void (*BUTTONHELPPROC)(DIALOG_RECORD *dialogTop, Word i);
extern	BUTTONHELPPROC buttonHelpProc;
#else
extern void (*buttonHelpProc)(DIALOG_RECORD *dialogTop, Word i);
#endif

#define ButtonsEnableHelp(helpProc)	(buttonHelpProc = helpProc)
#define ButtonsDisableHelp()		(buttonHelpProc = NULL)


/*----------------------------------------------------------------------*
 * Function prototypes for dialog module.				*
 *----------------------------------------------------------------------*/

					/* DialogFilter is a pointer 	*/
					/* to a function returning a	*/
					/* Word.			*/
#ifdef	PROC_TYPEDEFS
typedef Word (*DIALOGFILTERPROC)(DIALOG_RECORD *dr, Word *c);
#endif

typedef Word (*DialogFilter)(DIALOG_RECORD *dr, Word *c);

					/* Function called when user	*/
					/* selects About from sys menu.	*/

extern	void	(far * near lpfnSysMenuAboutHook)(void);

					/* DLOG_LP.C functions		*/

Word	PASCAL	DialogItemEvent (DIALOG_RECORD *dr, DIALOG_ITEM *item, Word *c);
Word	PASCAL	DialogChangeFieldEvent (DIALOG_RECORD *dr, Word c);
void	PASCAL	DialogChangeField (DIALOG_RECORD *dr, Word fieldNum);
void    PASCAL	DialogResetDefaultButton (DIALOG_RECORD *dr);
Word	PASCAL	DialogNextItemNum (DIALOG_RECORD *dr, int direction);
Boolean PASCAL	DialogItemCloser (int origRow, int origCol, int rowDif, int colDif, Word c, int *besttDist);
Word	PASCAL	DialogTrackMouseIntoItem ( Word old_field );
int	PASCAL	DialogMouseItemNum (void);
Boolean PASCAL	DialogMouseInItem (DIALOG_ITEM *field);
Word	PASCAL	DialogMouseCR (Word c);

					/* DLOG_ADD.C functions		*/

DIALOG_RECORD * PASCAL DialogAllocateDRecord (DIALOG *dialog);
void		PASCAL DialogDisposeDRecord (DIALOG_RECORD *drecord);
void		PASCAL DialogAddField (char *item, int type, int height, int width, int flags, Byte hotkey); 
void		PASCAL DialogWriteField (Word field_num, Boolean firstTime, Boolean active); 

					/* DLOG_STR.C functions		*/

Word PASCAL	DialogInternalStringLength (char *s, Byte *args, int *height, int *width, Byte *offsets);
void		DialogInternalWriteString (char *s, void *argsIn, Byte *offsets);
Word PASCAL 	DialogArgWidth (Word *arg, int type, Word *height, Word *width); 
Word PASCAL 	DialogArgument (Byte *arg, int *type, int *width, int *arg_num, ArgumentFlagsRec *flagsRec, Byte **group );

					/* DLGCLOSE.C functions		*/

void PASCAL	DialogCloseAll (void);

					/* DIALOGI.C functions (some)	*/

DIALOG_RECORD *	DialogInternalOpen (DIALOG *dialog, void *args);

					/* GETITEM.C functions */

DIALOG_ITEM *	DialogGetItemPtr(DIALOG_RECORD *dr, void *control);
int		DialogGetItemNum(DIALOG_RECORD *dr, void *control);

					// dlog_mov.c

void PASCAL	DialogMoveItems(DIALOG_RECORD *dialog, int deltaY, int deltaX);
void PASCAL	DialogMoveWindow(DIALOG_RECORD *dr, int newRow, int newCol);

					/* DLGVARS.C variables (some) */
extern int near	arg_num;	/* Dialog's argument number	*/

					/* CONTROLS functions		*/

void    PASCAL  CheckBoxToggle (CheckBoxRec *cbox);
void 	PASCAL 	CheckBoxWrite( CheckBoxRec *checkBox, Boolean firstTime, Boolean active );
Word 	PASCAL 	CheckBoxWidth( CheckBoxRec *ptr );
Word	PASCAL  CheckBoxEvent (DIALOG_ITEM *field, Word *c);
void	PASCAL	CheckBoxEnable (DIALOG_RECORD *dr, CheckBoxRec *cbox, Boolean bEnable);

void	PASCAL  RadioButtonsWrite( RadioButtonsRec *rb, Boolean firstTime, Boolean active );
void	PASCAL  RadioButtonsDimensions( RadioButtonsRec *ptr, Word *height, Word *width );
Word	PASCAL  RadioButtonsEvent (DIALOG_ITEM *field, Word *c);
void	PASCAL	RadioButtonsSetValue (RadioButtonsRec *radio, Byte value);
void	PASCAL  RadioButtonsAdjustCursor (RadioButtonsRec *radio, Word row);
void	PASCAL	RadioButtonsEnable (register RadioButtonsRec *radio, 
				Word button, Boolean bEnabled);
void    PASCAL  RadioButtonsFieldEnable (DIALOGREC drDialog, RadioButtonsRec *radio, Boolean bEnable);

void	PASCAL  UserDefinedWrite( UserControlRec *uc, Boolean firstTime, Boolean active );
void	PASCAL  SizeUserDefined( UserControlRec *uc, Word *height, Word *width );
Word	PASCAL  UserDefinedEvent (DIALOG_ITEM *field, Word *c);

void    PASCAL  ButtonSetColors(void);
void	PASCAL	ButtonsDimensions( BUTTONS *b, Word *height, Word *width );
void	PASCAL	ButtonsWrite( BUTTONS *b, Boolean firstTime, Boolean active, int singleButton );
Word	PASCAL  ButtonsEvent (BUTTONS *b, int items, Word *c);
void	PASCAL	ButtonEnable (BUTTONS *b, Word button, Boolean bEnabled);
void	PASCAL	ButtonEnableWithCheck (BUTTONS *b, Word button, Boolean bEnabled);
void	PASCAL	FlashButton(BUTTONS *b);
Word	PASCAL	ButtonsMouse(BUTTONS *b);
Byte	PASCAL	ButtonSetDefault (register BUTTONS *pButtons, 
				Byte byDefaultButton, Boolean bActive);
Boolean	PASCAL	ButtonIsDisabled (BUTTONS *pButtons, Byte byButton);

void    PASCAL  ButtonFieldEnable (DIALOGREC drDialog, BUTTONS *button, Boolean bEnable);
void    PASCAL  ButtonsArea(BUTTONS *b, Word i, Word *row, Word *col, Word *height, Word *width);
void    PASCAL  ButtonsAdjustValue (BUTTONS *b, Word origRow, Word origCol);


void    PASCAL  _TextEditWrite (TERec *te, Boolean firstTime, Boolean active );
void	PASCAL	InternalTextEditWrite (TERec *te,
				       Boolean firstTime,
				       Boolean active,
				       int normalAttr,
				       int inverseAttr);
void	PASCAL 	TextEditWrite (DIALOG_ITEM *field, Boolean firstTime, Boolean active);
Word	PASCAL	TextEditEvent (DIALOG_ITEM *field, Word *c);
void	PASCAL  TextEditUpdate(TERec *te, int offset);
void 	PASCAL	TextEditEnable (DIALOG_RECORD *drDialog, TERec *teRec, Boolean bEnable);
int	PASCAL  core_edit(TE_RECORD *te_record, Word key);
void	PASCAL  update_text(int row, int col, int width, char *string);
Word    PASCAL  TEStringLength (TERec *te);
void	PASCAL	TESetLength (TERec *te, Word length);

#ifdef	PROC_TYPEDEFS
typedef	Boolean (*LISTHIGHLIGHTPROC)(LIST *list, Ulong i, int attr);
extern LISTHIGHLIGHTPROC _ListHighlight;
#else
extern Boolean (*_ListHighlight)(LIST *list, Ulong i, int attr);
#endif

void PASCAL	ComboBoxWrite (ComboBoxRec *cb, Boolean bFirstTime, Boolean bActive );
void PASCAL	ComboBoxDimensions(ComboBoxRec *cb, Word *wHeight, Word *wWidth );
Word PASCAL	ComboBoxEvent (DIALOG_ITEM *pDialogField, int *pnEvent);
Ulong PASCAL	ComboBoxGetSelection (ComboBoxRec *cb);
Ulong PASCAL	ComboBoxSetSelection (ComboBoxRec *cb, Ulong value);
void PASCAL 	ComboBoxEnable (DIALOGREC drDialog, ComboBoxRec *cb, Boolean bEnable);
Word PASCAL	ComboBoxPopDown (ComboBoxRec *cb);
Word PASCAL	InternalComboBoxEvent (ComboBoxRec *cb, DIALOG_ITEM *pDialogField, int *pnEvent);

#ifdef	INCL_VMM
Handle PASCAL 	ComboBoxHistoryInit (ComboBoxRec *cbsCombo, Word wEntrySize,
				Word wMaxEntries, Handle hBlock);
#endif

void PASCAL 	ComboBoxHistoryTerminate (ComboBoxRec *cbsCombo);
Word PASCAL 	ComboBoxHistoryGet (ComboBoxRec *cbsCombo, Word wEntry, 
				Byte *pszEntry);
Word PASCAL 	ComboBoxHistoryPut (ComboBoxRec *cbsCombo);
Word PASCAL	ComboBoxHistoryInsert (ComboBoxRec *cbsCombo, Byte *pString);
void 		ComboBoxUpdateStr (ComboBoxRec *cb);

void PASCAL	InitFormatWidths(Byte *);
void PASCAL	CheckFormatWidth(Byte *s, Byte *offsets);
void PASCAL	ComputeFormatWidths(Byte *);

#endif
