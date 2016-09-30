/* $Header:   S:/SRC/NORTON/VCS/DEFS.H_V   1.13   05 Jan 1993 15:44:28   SKURTZ  $ */

/*----------------------------------------------------------------------*
 * Norton Integrator		      				 DEFS.H	*
 *									*
 * This file contains constants, structures, and type definitions.	*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/SRC/NORTON/VCS/DEFS.H_V  $
 * 
 *    Rev 1.13   05 Jan 1993 15:44:28   SKURTZ
 * Additions for bringing ADVISE feature online
 *
 *    Rev 1.12   11 Nov 1992 10:30:32   SKURTZ
 * Increased NUM_EXE_FILES for DUPDISK program
 *
 *    Rev 1.11   29 Oct 1992 17:54:44   JOHN
 * Increased NUM_EXE_FILES for RESCUE program
 *
 *    Rev 1.10   19 Oct 1992 18:24:10   JOHN
 * Added byPasswordID to NIFileInfoRec
 *
 *    Rev 1.9   17 Oct 1992 02:11:48   JOHN
 * General cleanup and update for New Loader
 *
 *    Rev 1.8   28 Aug 1992 11:05:04   SKURTZ
 * Preparations for Scorpio
 *
 *    Rev 1.7   25 Jun 1991 11:11:16   PETERD
 * Moved Advise menu items and related functions to library.
 *
 *    Rev 1.6   08 May 1991 17:48:04   JOHN
 * Increased NUM_EXEs value to account for addition of LP.
 *
 *    Rev 1.5   25 Apr 1991 09:57:28   ED
 * Removed the DEBUG variable, placing it in its own file, DEBUG.H
 *
 *    Rev 1.4   05 Apr 1991 14:24:28   ED
 * Increased the NUM_EXEs value to account for the addition of
 * Directory Sort
 *
 *    Rev 1.3   11 Feb 1991 13:01:48   JOHN
 * Turned off debugging.
 *
 *    Rev 1.2   07 Feb 1991 14:59:36   ED
 * Changed items to handle the new Configuration menu
 *
 *    Rev 1.1   18 Dec 1990 16:09:46   ED
 * Added definitions for use with the search option
 *
 *    Rev 1.0   20 Aug 1990 15:19:46   DAVID
 * Initial revision.
 */

/*----------------------------------------------------------------------*
 * Generally useful macros and constants.				*
 *----------------------------------------------------------------------*/

#include <minmax.h>

#ifndef INCL_MAIN
#include <main.h>
#endif


#define isspace(c)	(c == ' ' || c == '\t')

#ifndef ESC
#define ESC 27
#endif

#define ListGrow(list)    (list.height += (Byte) (VideoGetMaxRows() - 25))
#define ListShrink(list)  (list.height -= (Byte) (VideoGetMaxRows() - 25))

/*----------------------------------------------------------------------*
 * Various program limits.						*
 *									*
 * FILE_BUF_SIZE needs to be larger than the biggest Advise text entry.	*
 *----------------------------------------------------------------------*/

#define FILE_BUF_SIZE		(5*1024)

#define ERROR_LINE_WIDTH	50
#define ERROR_LINE_WIDTH2	58
#define ERROR_LINES		10

#define SHOW_ERROR_LINE_WIDTH	60
#define SHOW_ERROR_LINES 	10

#define MAX_FILE_LINE		(3 * DESCR_WIDTH + 1)
#define LINE_BUF_LEN		max(150, MAX_FILE_LINE)

#define	KEY_LENGTH		40
#define	MAX_ERRORS		100
#define	MAX_KEYS		10

/*----------------------------------------------------------------------*
 * Types and constants related to the main dialog.	 		*
 *									*
 * I should make these names more consistent.				*
 *----------------------------------------------------------------------*/

#define MAX_COMMANDS		128
#define MAX_TOPICS		10	/* Must be small for radio btns	*/

#define COMMAND_LINES		15		/* height of list	*/
#define COMMAND_WIDTH		16		/* excluding null	*/
#define TOPIC_WIDTH           (COMMAND_WIDTH-2)	/* excluding null	*/
#define COMMAND_LIST_WIDTH	20		/* list box width	*/

#define DESCR_LINES		14		/* height of box	*/
#define DESCR_WIDTH		40		/* excluding null	*/

#define DOS_CMD_LINES		1  /* 5 */	/* per command		*/
#define DOS_CMD_WIDTH		127		/* excluding null	*/

#define TYPE_NONE	0		/* Types for CmdInfoRec		*/
#define TYPE_TOPIC	1
#define TYPE_COMMAND	2
#define TYPE_BLANK	3

typedef struct
    {
    Byte		type;
    Byte		topic;
    Byte		name [COMMAND_WIDTH + 1];
    Byte		dosCmds [DOS_CMD_LINES] [DOS_CMD_WIDTH + 1];
    Word		descr [DESCR_LINES] [DESCR_WIDTH + 1];
    }
    CmdInfoRec;

typedef struct			/* A mini CmdInfoRec for blank lines	*/
    {
    Byte		type;		/* Always TYPE_BLANK	*/
    Byte		topic;
    }
    BlankRec;

/*----------------------------------------------------------------------*
 *									*
 *----------------------------------------------------------------------*/

typedef struct
    {
    Byte		origTopic;
    Byte		name [TOPIC_WIDTH + 1];
    }
    TopicRec;

/*----------------------------------------------------------------------*
 *									*
 *----------------------------------------------------------------------*/

typedef struct
    {
    Byte*	title;
    Word	titleHelpTopic;
    Word	helpTopic;
    }
    ErrorInfoRec;

typedef struct
    {
    Byte*	helpToken;
    Byte*	buttonName;
    Byte*	command;
    }
    ButtonInfoRec;


#define DISK_PROB_LINE_WIDTH	60
#define DISK_PROB_LINES		4
#define DISK_PROB_SOL_LINES	5

#define	KEY_LENGTH		40
#define	MAX_ERRORS		100
#define	MAX_KEYS		10

#define ERROR_LINE_WIDTH	50
#define ERROR_LINE_WIDTH2	58
#define ERROR_LINES		10

#define SHOW_ERROR_LINE_WIDTH	60
#define SHOW_ERROR_LINES 	10

#ifndef ESC
#define ESC 27
#endif

#define MAX_TOPICS		10	/* Must be small for radio btns	*/


/*----------------------------------------------------------------------*
 * Norton Integrator EXE file information table description.		*
 *----------------------------------------------------------------------*/

//#define NUM_EXE_FILES	       32
#define NUM_EXE_FILES	       5

//#ifdef INCL_MAIN

#define TTY_SWITCH_NONE		0	/* Values for ttySwitches	*/
#define TTY_SWITCH_ANY		1
#define TTY_SWITCH_LISTED	2

#define	TTY_ARG_NONE		0	/* Values for ttyArgs		*/
#define TTY_ARG_ANY		1
#define TTY_ARG_WILD		2

#define MAX_SWITCHES	       20	/* Max num of special switches	*/
//#endif

typedef struct
    {
    Byte*	name;		    /* Normal EXE filename ("SFORMAT.EXE")	*/
    Byte*	alias;		    /* Substitute filename ("SF.EXE")	*/
    Word	helpID;		    /* Help topic for this program		*/
    Byte    byPasswordID;   /* Password ID (see PASSWORD.H)         */
	Byte    byDiskNum;
    Boolean	tty;		    /* Always produces TTY output?		*/
    Byte	ttyArgs;	    /* What args cause TTY output?		*/
    Byte	ttySwitches;	/* What switches cause TTY output?	*/
    SwitchRec*	switches[MAX_SWITCHES];	/* Specific switches to check	*/
					                    /* if TTY_SWITCH_SOME		*/
    }
    NIFileInfoRec;


/*----------------------------------------------------------------------*
 * Extern declarations for program strings.				*
 *----------------------------------------------------------------------*/

extern Byte NULL_STR[];
extern Byte *HELP_SCREEN[];
