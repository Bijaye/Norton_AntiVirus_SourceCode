/* $Header:   S:/SRC/NORTON/VCS/CONFIG.C_V   1.7   16 Nov 1992 18:41:54   SKURTZ  $ */

/*----------------------------------------------------------------------*
 * CONFIG.C     							*
 *									*
 * This file contains routines for interfacing with the NUCONFIG	*
 * program								*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/SRC/NORTON/VCS/CONFIG.C_V  $
 * 
 *    Rev 1.7   16 Nov 1992 18:41:54   SKURTZ
 * Inserted #ifdefs for making EMERGENCY boot disk for DOS 6
 *
 *    Rev 1.6   17 Oct 1992 02:11:36   JOHN
 * General cleanup and update for New Loader
 *
 *    Rev 1.5   13 Oct 1992 09:35:14   SKURTZ
 * Changed Configuration menu to make them compatible with new NUCONFIG
 *
 *    Rev 1.4   24 Jul 1992 17:52:06   SKURTZ
 * Preparations for Scorpio. Fixed source of compiler warning messages.  Changed
 * DoExecConfig() so that it understands the new programmer-defined menu ID's
 * that are used with the dropdown menu.
 *
 *    Rev 1.3   28 Feb 1991 08:12:04   ED
 * Added the cantFindNuConfigDlg dialog to InitConfigMenu().  This
 * informs the user why their Configuration menu is being disabled.
 *
 *    Rev 1.2   18 Feb 1991 22:08:26   JOHN
 * Added code to set the <skipPwdTestForVideo> flag if doing Video setup.
 * The user must always be able to change the video setup, even if NUCONFIG
 * is password protected.
 *
 * Added code to set the <skipLoadingMessage> flag so the "Loading..."
 * message isn't displayed when running NUCONFIG.
 *
 *    Rev 1.1   11 Feb 1991 09:58:02   ED
 * Fixed DoExecConfig() to form the command line correctly, so it
 * would work correctly.
 *
 * Modified DoExecConfig() to use the switchChar when executing
 * NUCONFIG.EXE.
 *
 *    Rev 1.0   07 Feb 1991 14:55:10   ED
 * Initial revision.
 */

#include	<types.h>
#include	<dialog.h>
#include	<pulldown.h>
#include	<password.h>
#include	<lint.h>

#include	"defs.h"
#include	"proto.h"
#include	"menu.h"

Byte	configPath[128];

/*----------------------------------------------------------------------*
 * DoExecConfig
 *
 * Handles the NUCONFIG.EXE execution with the proper switches to perform
 * the action selected from the CONFIGURE menu.  Basically, the switches
 * are held in an array whose form matches the menu structure.  So all we
 * have to do is use the menu item to select the switch, then EXEC
 * NUCONFIG.EXE with the switch.  Pretty darned simple.
 * One little twist.  "item" must have VIDEO_MOUSE subtracted from it
 * to create a proper index into this array.  When the drop down menus
 * were converted to the new Passport format with programmer-defined
 * menu id's, the first item in each menu no longer could be depended on
 * to be zero.  By subtracting VIDEO_MOUSE from the "item", we are assured
 * a zero-based index value.
 *----------------------------------------------------------------------*/
#ifndef EMERGENCY
void DoExecConfig(int item)
{
    extern	Boolean		skipPwdTestForVideo;
    extern	Boolean		skipLoadingMessage;
    extern	Byte		configPath[];
    extern	Byte		*configSwitches[];
    extern	Byte		mainString[];
    extern	Byte		switchChar;

    if (*configPath)
	{
	/* Skip password test if doing Video setup */
	if (item == VIDEO_MOUSE)
	    skipPwdTestForVideo = TRUE;

	StringCopy(mainString, configPath);
	StringAppendChar(mainString, ' ');
	StringAppendChar(mainString, switchChar);
	StringAppend(mainString, configSwitches[item - VIDEO_MOUSE]);

	/* Do not display "Loading..." message */
	skipLoadingMessage = TRUE;

	Launch(mainString);
	}
}
#endif

/*----------------------------------------------------------------------*
 * InitConfigMenu
 *
 * Searches for the NUCONFIG.EXE program.  If found, the full path is
 * saved, and the menu modified if menu-editing is not allowed.
 *
 * If not found, the entire "Configure" menu is disabled
 *----------------------------------------------------------------------*/

void InitConfigMenu(void)
{
#ifndef EMERGENCY
    extern	DialogRec	cantFindNuConfigDlg;
    extern	Byte		*configProgram;		/* NUCONFIG.EXE */
    extern	Byte		configPath[];		/* path of NUCONFIG */
    extern      MenuBarRec      menuBar;
    extern	Byte		*pszProductEnvVar;

    if (!DirSearchNortonPath(pszProductEnvVar, configProgram, configPath))
	{
	PulldownSetItemState(&menuBar, VIDEO_MOUSE, 	DISABLED, UNCHANGED);
	PulldownSetItemState(&menuBar, PRINTER_CONFIG,	DISABLED, UNCHANGED);
	PulldownSetItemState(&menuBar, STARTUP_PROGS, 	DISABLED, UNCHANGED);
	PulldownSetItemState(&menuBar, ALT_NAMES, 	DISABLED, UNCHANGED);
	PulldownSetItemState(&menuBar, PASSWORDS, 	DISABLED, UNCHANGED);
	PulldownSetItemState(&menuBar, MENU_EDIT, 	DISABLED, UNCHANGED);

	*configPath = 0;
	DialogBox2(&cantFindNuConfigDlg);
	}
    else
	{			     	/* add a space for the switch */
	StringAppendChar(configPath, ' ');
	}
#endif
}