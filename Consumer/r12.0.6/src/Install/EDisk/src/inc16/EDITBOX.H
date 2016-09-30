/* $Header:   S:/INCLUDE/VCS/EDITBOX.H_V   1.2   27 Jan 1992 22:36:32   TORSTEN  $ */

/*----------------------------------------------------------------------*
 * EDITBOX.H  								*
 *									*
 * This file contains type definitions for EditBox controls		*
 *									*
 * Copyright 1991 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/INCLUDE/VCS/EDITBOX.H_V  $
 * 
 *    Rev 1.2   27 Jan 1992 22:36:32   TORSTEN
 * Changed the #include statements to use angle brackets.
 * 
 *    Rev 1.1   18 Nov 1991 12:04:16   JOHN
 * Fixed prototype of EditBoxEvent().
 * 
 *    Rev 1.0   29 Aug 1990 11:24:22   DAVID
 * Initial revision.
 */

#ifndef INCL_DIALOG
#include <dialog.h>
#endif

#ifndef INCL_KEYS
#include <keys.h>
#endif

#ifndef INCL_EDITBOX
#define INCL_EDITBOX

#define EDIT_WORDS	1		/* Edit attr/char pairs		*/

/*----------------------------------------------------------------------*
 * EditBox Field							*
 *									*
 * 	width	- Length of longest string. (Don't include NULL)	*
 *	height	- Number of rows in box.				*
 *	data	- Pointer to buffer large enough to hold 		*
 *		   (width + 2) * height * 2 ('2' is for attribute).	*
 *		   The 1st '2' is for NULL and 1 extra for wrapping.	*
 *									*
 *									*
 * Example:								*
 *									*
 * extern Word buffer[];						*
 * EditBoxRec editBox =							*
 *     {								*
 *     LINE_WIDTH,							*
 *     LINES,								*
 *     buffer,								*
 *     };								*
 *----------------------------------------------------------------------*/

typedef struct 
    {
    Byte	width;			/* Screen columns used by box	*/
    Byte	height;			/* Screen rows used by box	*/
#if EDIT_WORDS
    Word*	data;		       	/* Storage for data		*/
#else
    Byte*	data;		       	/* Storage for data		*/
#endif
    Word	cRow, cCol;		/* Cursor position		*/
    }
    EditBoxRec;

/*----------------------------------------------------------------------*
 * Function prototypes for the EditBox control.				*
 *									*
 * Pointers to the first three functions should be placed in the 	*
 * user-defined control structure.					*
 *									*
 * Example:								*
 *									*
 * UserControlRec editUCR =						*
 *     {				   				*
 *     0,				   				*
 *     EditBoxDimensions,		   				* 
 *     EditBoxWrite,							*
 *     EditBoxEvent,							*
 *     &editBox,			   				*
 *     };				   				*
 *----------------------------------------------------------------------*/

void	EditBoxDimensions(struct UCR *r, Word *pwHeight, Word *pwWidth);
void	EditBoxWrite(struct UCR *r, Boolean bFirstTime, Boolean bActive);
int	EditBoxEvent(struct UCR *r, Word *pEvent);

void	WriteCharAttrString(Word far *s, Word width);
Word 	LineLength(Word *s);

#endif
