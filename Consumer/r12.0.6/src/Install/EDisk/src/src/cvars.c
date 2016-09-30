/* $Header:   S:/SRC/NORTON/VCS/CVARS.C_V   1.9   03 Dec 1992 10:48:46   SKURTZ  $ */

/*----------------------------------------------------------------------*
 * CVARS.C								*
 *									*
 * This file contains global variables for NORTON.			*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/SRC/NORTON/VCS/CVARS.C_V  $
 * 
 *    Rev 1.9   03 Dec 1992 10:48:46   SKURTZ
 * Removed obsolete NUConfigrec and added NIRec and CFGBLOCKREC that are needed
 * by new configuration manager.
 *
 *    Rev 1.8   17 Oct 1992 02:11:38   JOHN
 * General cleanup and update for New Loader
 *
 *    Rev 1.7   25 Sep 1992 14:05:20   SKURTZ
 * Conversion to LARGE memory model
 *
 *    Rev 1.6   28 Aug 1992 11:05:30   SKURTZ
 * Preparations for Scorpio
 *
 *    Rev 1.5   19 Jun 1991 17:27:12   PETERD
 * Removed Advise variables (to library).
 *
 *    Rev 1.4   10 Apr 1991 17:45:24   ED
 * Removed the obsolete savedColorColorSet value.
 *
 *    Rev 1.3   18 Feb 1991 22:12:38   JOHN
 * Added <skipPwdTestForVideo> and <skipLoadingMessage>.
 * See related code in LAUNCH.C and CONFIG.C.
 *
 *    Rev 1.2   07 Feb 1991 14:53:18   ED
 * Added variables
 *
 *    Rev 1.1   18 Dec 1990 16:14:04   ED
 * Added variables to support the search feature
 *
 *    Rev 1.0   20 Aug 1990 15:19:32   DAVID
 * Initial revision.
 */

#include	<types.h>
#include	<dialog.h>
#include	<nuconfig.h>
#include	<cfg_ids.h>
#include	<symcfg.h>
#include	<main.h>
#include	<file.h>

#include	"defs.h"

/*----------------------------------------------------------------------*
 * These variables are needed by the library.				*
 *									*
 * trapping_enabled	determines if the critical error dialogs appear	*
 *----------------------------------------------------------------------*/

int		trapping_enabled	= TRUE;

/*----------------------------------------------------------------------*
 * These variables are for the startup and shutdown routines.		*
 *									*
 * NIConfigRec		holds contents of SYMCFG.BIN global config file	*
 *----------------------------------------------------------------------*/

#ifndef EMERGENCY

NIRec		NIConfigRec;

CFGBLOCKREC     cfgblockrec = 
    {
    CFGID_NU_INTEGRATOR,
    sizeof(NIRec),
    &NIConfigRec,
    CFG_GLOBAL_FIRST,
    0
    };

#endif
/*----------------------------------------------------------------------*
 * These variables provide access to the loader's memory space		*
 *----------------------------------------------------------------------*/

NULoaderDataRec far *loaderData;	/* Points to shared data space	*/

/*----------------------------------------------------------------------*
 * These variables keep track of the CommandInfoRec buffer pool		*
 * managed by CMDALLOC.C						*
 *									*
 * At startup, InitCmdBuf() is used to grab an array of up to 		*
 * MAX_COMMANDS CommandInfoRecs off the far heap.  AllocCmdBuf()	*
 * and FreeCmdBuf() are then used to take and return buffers from	*
 * this CommandInfoRec buffer pool.  This is done to overcome the	*
 * limitations of the stack-oriented FarHeap library routines.		*
 *----------------------------------------------------------------------*/

CmdInfoRec	huge *cmdBufBase;		/* ptr to  pool	*/
Word		numCmdBufs;			/* size of pool	*/
Boolean		cmdBufUsed[MAX_COMMANDS];	/* in use flags	*/

/*----------------------------------------------------------------------*
 * Other global variables						*
 *----------------------------------------------------------------------*/

Boolean		doubleClicked = FALSE;	/* Record d-clicks in main dlg	*/

Byte		descrRow;		/* Description box location	*/
Byte		descrCol;
Byte		descrWidth;
Byte		descrHeight;

DIALOG_RECORD	*mainDlgHdl;		/* Main dialog handle		*/
Boolean		openingMainDlg;		/* Modify write entry behavior	*/

Boolean		cursorOn;		/* To control TE field's cursor	*/
Boolean		passwordOK = FALSE;	/* User has supplied password?	*/
Boolean		sortByName = FALSE;	/* Command list sort order	*/
Boolean		iniSortByName;		/* Sort order from .ini file	*/
Word		totalListItems;		/* Including ones not visible	*/

CmdInfoRec	far *commands[MAX_COMMANDS]; /* All the .CMD file entries */
CmdInfoRec	tempCommandInfo; 	/* For editing current command	*/

Word		descrBuffer[DESCR_LINES] [DESCR_WIDTH + 2];  /* Editor	*/

Byte  near	fileBuf[FILE_BUF_SIZE];	/* For .CMD file I/O, etc.	*/
Byte  near	line[LINE_BUF_LEN]; 	/* General purpose line buf	*/

Boolean		commandsEdited = FALSE;	/* Need to write .CMD file?	*/
Boolean		readOnlyCmdFile;	/* Don't allow editing?		*/

Byte  near	commandFile[MAX_PATH_NAME];	/* Full name of command file */
Byte  near	commandComPath[MAX_PATH_NAME];	/* Full name of COMMAND.COM  */

Boolean		skipPwdTestForVideo;	/* Skip password test so we can	*/
					/* call NUCONFIG to set video	*/
					/* options, even if NUCONFIG is	*/
					/* password protected		*/

Boolean		skipLoadingMessage;	/* Do not display "Loading..."	*/
					/* dialog when running program.	*/
					/* Used during configuration.	*/