/* $Header:   N:/NORTON/VCS/NISTR.C_V   1.59   29 Jun 1995 14:33:12   GEORGEL  $ */

/*----------------------------------------------------------------------*
 * Norton Integrator		     				NISTR.C	*
 *									*
 * This file contains text strings.					*
 *									*
 * Copyright 1991 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   N:/NORTON/VCS/NISTR.C_V  $
 * 
 *    Rev 1.59   29 Jun 1995 14:33:12   GEORGEL
 * Fixed 38820
 * 
 *    Rev 1.58   28 Jun 1995 03:21:46   TMAGILL
 * Change NU Preview to NU for Win 95
 * 
 *    Rev 1.57   02 Mar 1995 03:03:04   TMAGILL
 * Remove 'The' from title
 * 
 *    Rev 1.56   02 Mar 1995 02:39:04   TMAGILL
 * Update title for NU Preview
 * 
 *    Rev 1.55   17 Feb 1994 02:45:42   TMAGILL
 * Remove special about box string
 * 
 *    Rev 1.54   16 Feb 1994 14:34:30   JOHN
 * Changed version in main window title to 8.0
 * 
 *    Rev 1.53   01 Feb 1994 21:03:04   TMAGILL
 * Updated to version 8.0
 * 
 *    Rev 1.52   21 Apr 1993 10:11:48   KEVIN
 * button hot key fix
 * 
 *    Rev 1.51   18 Mar 1993 13:18:22   JOEP
 * Added new programmers name
 * 
 *    Rev 1.50   17 Mar 1993 11:52:38   JOEP
 * More Strings
 * 
 *    Rev 1.49   15 Mar 1993 12:19:36   SKURTZ
 * Added the duplicate name for DUPDISK.EXE (DD.EXE)
 *
 *    Rev 1.48   09 Feb 1993 09:58:22   BRIANF
 * Fixed annotations.
 *
 *    Rev 1.47   06 Feb 1993 00:22:38   DAVID
 * UnErase now has a /LIST switch which makes it TTY; fileInfo table updated.
 *
 *    Rev 1.46   26 Jan 1993 17:59:16   SKURTZ
 * Added help topics to menu titles
 *
 *    Rev 1.45   05 Jan 1993 15:45:58   SKURTZ
 * Removal of obsolete AdviseLaunch[] list.
 *
 *    Rev 1.44   04 Jan 1993 18:00:26   SKURTZ
 * Updated names of some DEFINEs that are created by the help system
 *
 *    Rev 1.43   23 Dec 1992 10:14:46   SKURTZ
 * Fixed PTR NU06703 (Changed text in editColorNames[] from "Underlined" to
 * a more appropriate "Bold Reverse").
 *
 *    Rev 1.42   17 Dec 1992 10:05:32   SKURTZ
 * nothing done.
 *
 *    Rev 1.41   17 Dec 1992 09:58:22   SKURTZ
 * Changed 'printer configuration' to 'printer setup' in pulldown menu
 *
 *    Rev 1.40   14 Dec 1992 15:13:50   SKURTZ
 * EMERGENCY configuration only: Added askEmergencyDisk2 dialog. Added szPauseMsg
 *
 *    Rev 1.39   30 Nov 1992 17:12:36   JOHN
 * Added szEmergencyExeName[]
 *
 *    Rev 1.38   18 Nov 1992 16:24:04   SKURTZ
 * Changed 'MEMMAX' to 'MMSETUP' and changed 'NCACHE' to 'NCACHE2'
 *
 *    Rev 1.37   16 Nov 1992 18:44:06   SKURTZ
 * Inserted #ifdefs for making EMERGENCY boot disk for DOS 6
 *
 *    Rev 1.36   11 Nov 1992 10:31:22   SKURTZ
 * Added DUPDISK program
 *
 *    Rev 1.35   05 Nov 1992 13:05:52   BRUCEH
 * Added TxMog Annotations
 *
 *    Rev 1.34   29 Oct 1992 17:54:56   JOHN
 * Added RESCUE program
 *
 *    Rev 1.33   20 Oct 1992 16:05:02   JOHN
 * Reduced menu spacing
 *
 *    Rev 1.32   19 Oct 1992 18:24:50   JOHN
 * Added byPasswordID field to fileInfo[]
 *
 *    Rev 1.31   17 Oct 1992 02:13:16   JOHN
 * General cleanup and update for New Loader
 *
 *    Rev 1.30   13 Oct 1992 09:36:06   SKURTZ
 * Changed Configuration menu to make them compatible with new NUCONFIG
 *
 *    Rev 1.29   07 Oct 1992 12:13:34   SKURTZ
 * Added warning comment about adding menu items to menuItems[].
 *
 *    Rev 1.28   28 Aug 1992 17:25:10   SKURTZ
 * Fixed ADVISE menu to work with new hypertext helpsystem
 *
 *    Rev 1.27   24 Jul 1992 18:04:54   SKURTZ
 * Preparations for Scorpio.  Inserted HotKeys.  Fixed MenuItemRec's to use
 * new ID numbers returned by PulldownEvent().  Removed all help lines that
 * would appear along bottom of screen. Fixed MenuBarRec to use the new Pass-
 * port look&feel.
 *
 *    Rev 1.26   02 Jun 1992 17:29:18   JOHN
 * Changed version number to 6.01
 *
 *    Rev 1.25   06 Aug 1991 13:15:20   PETERD
 * Changed the program version number to "6.0", so that it can be used
 * as a generic heading ("Norton Utilities 6.0").
 *
 *    Rev 1.24   19 Jun 1991 17:25:34   PETERD
 * Removed all Advise messages to library.
 *
 *    Rev 1.23   17 May 1991 22:49:52   JOHN
 * Removed H_GLOBAL_README.
 *
 *    Rev 1.22   16 May 1991 10:21:02   ED
 * No change.
 *
 *    Rev 1.21   08 May 1991 18:15:56   JOHN
 * Minor cleanup of some advise messages.
 *
 *    Rev 1.20   08 May 1991 18:01:34   JOHN
 * Added /G2 to TTY help message.
 *
 *    Rev 1.19   08 May 1991 17:48:40   JOHN
 * Added /DI to list of SYSINFO switches that cause TTY output.
 *
 *    Rev 1.18   07 May 1991 15:32:40   ED
 * Added "LOADHI" and "LH" as internal dos commands
 *
 * Added H_NI_LP as a help item
 *
 * Added Line Print to the fileInfo structure.
 *
 * Added dBASE messages
 *
 *    Rev 1.17   25 Apr 1991 10:42:30   ED
 * Fixed up the Lotus error message IDs and the program help IDs, and
 * added missing commands to the fileInfo[] structure.
 *
 *    Rev 1.16   10 Apr 1991 14:55:10   ED
 * Added a scroll bar to the dos problems text list box.
 *
 *    Rev 1.15   05 Apr 1991 14:24:02   ED
 * Added Directory Sort to the list of Norton programs
 *
 *    Rev 1.14   25 Mar 1991 16:44:06   PETERD
 * Changed the Add/Edit Menu Topic dialog to work with the new graphical
 * interface.
 *
 *    Rev 1.13   24 Mar 1991 09:59:56   ED
 * Modified strings to work with the new-style graphic list boxes.
 *
 *    Rev 1.12   14 Mar 1991 15:15:56   PETERD
 * Made changes to TERec's.
 *
 *    Rev 1.11   13 Feb 1991 19:00:46   PETERD
 * Changed TTY Help text to DOS 5.0 format
 *
 *    Rev 1.10   11 Feb 1991 13:14:50   JOHN
 * Added H_GLOBAL_README help topic.
 *
 *    Rev 1.9   11 Feb 1991 13:02:42   JOHN
 * Updated help topics to reflect changes to help file.
 *
 *    Rev 1.8   28 Jan 1991 10:33:18   ED
 * Updated to the latest 6.0 strings.
 *
 *    Rev 1.5   26 Nov 1990 09:52:24   ED
 * Removed SW_RECONSTRUCT from the fileInfo table entry for NCD.  If
 * NCD /R was entered from the Norton program command-line, NCD would
 * go full-screen to scan the directory structure, but when NCD was
 * exited, the "Press any key to continue" prompt would appear at the
 * bottom of the screen.  Fixes PTR #NU1850.
 *
 *    Rev 1.4   21 Nov 1990 11:04:50   ED
 * Inserted a "to" in the string "Press any key continue"
 *
 *    Rev 1.3   14 Sep 1990 17:26:04   DAVID
 * Fixed a typo.
 *
 *    Rev 1.2   14 Sep 1990 16:53:42   DAVID
 * More revisions to fileInfo table.  Got rid of all TTY_SWITCH_ANY
 * entries and replaced them with TTY_SWITCH_LISTED.  Without this
 * change, any invalid switch will cause the Norton program to assume
 * the utility will run in TTY mode, which means the "Press any key"
 * message will be drawn on top of the utility's full screen output.
 * Had to increase number of switch pointers in NIFileInfoRec.
 *
 *    Rev 1.1   27 Aug 1990 15:05:30   DAVID
 * Fixed mistakes in fileInfo table: changed DiskMon, FileSave, and
 * NCC from TTY_ARG_ANY to TTY_ARG_NONE.
 *
 *    Rev 1.0   20 Aug 1990 15:34:52   DAVID
 * Initial revision.
 */

#include	<dialog.h>
#include	<main.h>
#include	<pulldown.h>
#include	<keys.h>
#include	<nuconfig.h>
#include	<password.h>
#include	<editbox.h>

#include	"nort-hlp.h"
#include 	"menu.h"
#include        "defs.h"
#include	"proto.h"

/*----------------------------------------------------------------------*
 * These are miscellaneous strings.					*
 *----------------------------------------------------------------------*/

extern Byte errorTitle[];

#ifdef EMERGENCY
Byte	szEmergencyExeName[] = "STARTUP.EXE";	/* Name of Emergency shell */	//@Type:TextString
#endif

#ifdef EMERGENCY
Byte	CMD_FILE[]	= "EMERGNCY.CMD";     /* Menu definition file	*/        //@Type:TextString
#else
Byte	CMD_FILE[]	= "NORTON.CMD";     /* Menu definition file	*/        //@Type:TextString
#endif

Byte	COMSPEC[] 	= "COMSPEC";	    /* Environment var name	*/        //@Type:TextString
Byte	COMMAND_COM[]	= "COMMAND.COM";    /* Used if no COMSPEC	*/    //@Type:TextString

Byte	BAT_EXT[]	= "BAT";	    /* DOS batch file extension	*/        //@Type:TextString

Byte	PATH_STR[]	= "PATH";	    /* Environment var name	*/            //@Type:TextString

Byte	BAT_STR[]	= ".BAT";	    /* Standard DOS extensions	*/        //@Type:TextString
Byte	EXE_STR[]	= ".EXE";                                             //@Type:TextString
Byte	COM_STR[]	= ".COM";                                             //@Type:TextString

//@Rem
/*----------------------------------------------------------------------*
 * Command file tokens; these must begin with '!'			*
 *----------------------------------------------------------------------*/
//@EndRem

Byte COMMAND_TOKEN[]	= "!command";                                     //@Type:TextString
Byte TOPIC_TOKEN[]	= "!topic";                                           //@Type:TextString
Byte DOS_CMD_TOKEN[]	= "!dos-cmd";                                     //@Type:TextString

/*----------------------------------------------------------------------*
 * This record contains the program name as it appears on the 		*
 * DOS command line and status line.					*
 *----------------------------------------------------------------------*/

//@Block Type:TextString
VersionRec PROGRAM_NAME[] =
    {
    PROG_VERSION_FLAG,
    "Norton Utilities",                                                   //@ID:NortonUtils
    "9.0",                                                                //@ID:70
    };
//@EndBlock

/*----------------------------------------------------------------------*
 * This is the switch description text that appears when the user 	*
 * requests help from the command line using the "?" parameter.		*
 *									*
 * The switches listed here actually work for all programs.  Since	*
 * this program has no switches of its own, this is a good place to	*
 * document the generic switches.					*
 *----------------------------------------------------------------------*/

Byte *HELP_SCREEN[] =
    {
//@Block Type:TextString
    "Run the Norton Utilities (and other programs) from a menu.\n",       //@ID:Runthe
    "\n",
    "NORTON [/BW|/LCD] [/G0] [/M0] [/NOZOOM]\n",                          //@ID:CommandParams
//@EndBlock
    "\n",
//@Block Type:TextString Left
    "  /BW      Forces the use of the Black and White color set.\n",      //@ID:Bw
    "  /LCD     For laptop displays.  Forces the use of the LCD color set.\n", //@ID:Lcd
    "  /G0      Disable font redefinitions and graphic mouse.\n",         //@ID:G0
    "  /M0      Disable graphical mouse.\n",                              //@ID:M0
    "  /NOZOOM  Disables dialog box zooming.\n",                          //@ID:Nozoom
//@EndBlock
    NULL_STR,
    };

/*----------------------------------------------------------------------*
 * This is the table of internal DOS commands.				*
 *									*
 * These names were found by checking the COMMAND.COM file for IBM DOS	*
 * versions 3.3 and 4.0.  Their order in this list is irrelevent, as	*
 * is the length of the list.					  	*
 *----------------------------------------------------------------------*/

//@Block Type:TextString
Byte*	internalDOSCmds[] =		/* Names of internal commands	*/
    {
    "dir",                                                                //@ID:b1
    "call",                                                               //@ID:b2
    "chcp",                                                               //@ID:b3
    "rename",                                                             //@ID:b4
    "ren",                                                                //@ID:b5
    "erase",                                                              //@ID:b6
    "del",                                                                //@ID:b7
    "type",                                                               //@ID:b8
    "rem",                                                                //@ID:b9
    "copy",                                                               //@ID:b10
    "pause",                                                              //@ID:b11
    "date",                                                               //@ID:b12
    "time",                                                               //@ID:b13
    "ver",                                                                //@ID:b14
    "vol",                                                                //@ID:b15
    "cd",                                                                 //@ID:b16
    "chdir",                                                              //@ID:b17
    "md",                                                                 //@ID:b18
    "mkdir",                                                              //@ID:b19
    "rd",                                                                 //@ID:b20
    "rmdir",                                                              //@ID:b21
    "break",                                                              //@ID:b22
    "verify",                                                             //@ID:b23
    "set",                                                                //@ID:b24
    "prompt",                                                             //@ID:b25
    "path",                                                               //@ID:b26
    "exit",                                                               //@ID:b27
    "ctty"                                                                //@ID:b28
    "echo",                                                               //@ID:b29
    "goto",                                                               //@ID:b30
    "shift",                                                              //@ID:b31
    "if",                                                                 //@ID:b32
    "for",                                                                //@ID:b33
    "cls",                                                                //@ID:b34
    "truename",                                                           //@ID:b35
    "loadhigh",                                                           //@ID:b36
    "lh",                                                                 //@ID:b37
    NULL,				/* Terminates the list		*/
    "        ",				/* Spares for patching		*/
    "        ",
    NULL,
    };
//@EndBlock

/*----------------------------------------------------------------------*
 * These strings must be consisten with the NUCONFIG program		*
 *----------------------------------------------------------------------*/

Byte	*configProgram = "NUCONFIG.EXE";                                  //@Type:TextString

//@Rem
 	/* Theses strings must match the switches in the 	*/
 	/* NUCONFIG program, and their order must match the	*/
	/* menu layout of the CONFIGURE menu			*/
//@EndRem

//@Block Type:TextString
Byte	*configSwitches[] =
    {
    "$VI",		// Video and Mouse                                        //@ID:Configvi
    "$PT",		// Printer configuration                                  //@ID:Configpt
    "$ST",		// Startup programs                                       //@ID:Configst
    "$AN",		// Alternate names                                        //@ID:Configan
    "$PA",		// Passwords                                              //@ID:Configpa
    "$ME",		// Menu editing                                           //@ID:Configme
    };
//@EndBlock

/*----------------------------------------------------------------------*
 * Switch to disable the guilt screen					*
 *----------------------------------------------------------------------*/

//@Block Type:TextString
SwitchRec SW_NO_GUILT = { "$NG", EXACT };                                 //@ID:sr1

/*----------------------------------------------------------------------*
 * Table of Norton Utilities programs.					*
 *									*
 * If a program is found in this table, and the "always TTY" flag is	*
 * FALSE (meaning it supports a full-screen interface) it receives the 	*
 * following special treatment:						*
 *									*
 * 1) The Integrator checks a second table to see if the program has 	*
 * a password.  If so, it performs the password request itself, if it 	*
 * has not yet done so during the course of the current Integrator 	*
 * session.								*
 *									*
 * 2) The following switches are passed along to the program if they 	*
 * were typed on the command line when NORTON.EXE was first started:  	*
 *	SWITCH_BW							*
 *	SWITCH_LCD							*
 *	SWITCH_HERC							*
 *	SWITCH_NO_ZOOM							*
 *	SWITCH_GRAPHIC_MODE_0						*
 *	SWITCH_GRAPHIC_MODE_1						*
 *	SWITCH_GRAPHIC_MODE_2						*
 *	SWITCH_MOUSE_MODE_0						*
 *	SWITCH_MOUSE_MODE_1						*
 *									*
 * 3) The program is given the special SWITCH_INTEGRATOR switch,	*
 *    which means:							*
 *    (a) do not clear your screen on entry or exit			*
 *    (b) suppress password checking					*
 *									*
 * 4) The command line being sent to the program is checked against	*
 * the flags listed in the table to predict if the program will 	*
 * produce TTY output.  If not, the Integrator suppresses its own	*
 * screen clears when launching the program.				*
 *									*
 * Note that the cache programs are not able to handle extra Norton	*
 * switches like SWITCH_BW and SWITCH_INTEGRATOR.			*
 *									*
 *----------------------------------------------------------------------*/

	/* The strings in these switches must match the strings
	 * in the same switches in the SysInfo program. */

SwitchRec SW_TSR      = { "TSR",    EXACT };                              //@ID:sr2
SwitchRec SW_DISKINFO = { "DI",     EXACT };                              //@ID:sr3
SwitchRec SW_SUMMARY  = { "SU",     AT_LEAST };                           //@ID:sr4

	/* The strings in these switches must match the strings
	 * in the same switches in the Diskreet program. */

SwitchRec SW_SHOW    = { "SHOW",   EXACT };                               //@ID:sr5
SwitchRec SW_HIDE    = { "HIDE",   EXACT };                               //@ID:sr6
SwitchRec SW_CLOSE   = { "CLOSE",  EXACT };                               //@ID:sr7
SwitchRec SW_ON      = { "ON",     EXACT };                               //@ID:sr8
SwitchRec SW_OFF     = { "OFF",    EXACT };                               //@ID:sr9

	/* The strings in these switches must match the strings
	 * in the same switches in the FileSave program. */

SwitchRec ON_EP		= { "ON",	EXACT };                                  //@ID:sr10
SwitchRec OFF_EP	= { "OFF",	EXACT };                                  //@ID:sr11
SwitchRec UNINSTALL_EP	= { "UN",	AT_LEAST };                           //@ID:sr12
SwitchRec STATUS_EP	= { "ST",	AT_LEAST };                               //@ID:sr13

	/* The strings in these switches must match the strings
	 * in the same switches in the DiskMon program. */

SwitchRec SW_DISKLIGHT	= { "L",	AT_LEAST    };                        //@ID:sr14
SwitchRec SW_PROTECT	= { "PRO",	AT_LEAST    };                        //@ID:sr15
SwitchRec SW_PARK	= { "PA",	AT_LEAST    };                            //@ID:sr15
SwitchRec SW_STATUS	= { "S",	AT_LEAST    };                            //@ID:sr16
SwitchRec SW_UNINSTALL	= { "UN",	AT_LEAST    };                        //@ID:sr17

	/* The strings in these switches must match the strings
	 * in the same switches in the NCC program. */

SwitchRec
 SW_BW80 =        {"BW8",  AT_LEAST },                                    //@ID:sr18
 SW_CO80 =        {"CO",   AT_LEAST },                                    //@ID:sr19
 SW_50_LINES =    {"50",   EXACT },                                       //@ID:sr20
 SW_40_LINES =    {"40",   EXACT },                                       //@ID:sr21
 SW_43_LINES =    {"43",   EXACT },                                       //@ID:sr22
 SW_35_LINES =    {"35",   EXACT },                                       //@ID:sr23
 SW_25_LINES =    {"25",   EXACT },                                       //@ID:sr24
 SW_FAST_KEY =    {"FA",   AT_LEAST },                                    //@ID:sr25
 SW_LEFT =        {"L",    EXACT },                                       //@ID:sr26
 SW_COMMENT =     {"C",    EXACT } ,                                      //@ID:sr27
 SW_SET_ALL =     {"SET",  AT_LEAST },                                    //@ID:sr28
 SW_CURSOR =      {"CU",   AT_LEAST },                                    //@ID:sr29
 SW_PALETTE =     {"PA",   AT_LEAST },                                    //@ID:sr30
 SW_DOSCOLOR =    {"DO",   AT_LEAST },                                    //@ID:sr31
 SW_KEYRATE =     {"KEY",  AT_LEAST },                                    //@ID:sr32
 SW_NO_DATE =     {"N",    EXACT },                                       //@ID:sr33
 SW_START =       {"STA",  AT_LEAST},                                     //@ID:sr34
 SW_STOP =        {"STO",  AT_LEAST},                                     //@ID:sr35
 SW_DISPLAY =     {"DI",   AT_LEAST};                                     //@ID:sr36

	/* The strings in these switches must match the strings
	 * in the same switches in the NCD program. */

SwitchRec NCD_SW_RECONSTRUCT	= { "R", AT_LEAST };                      //@ID:sr37
SwitchRec NCD_SW_NOWRITE	= { "N", AT_LEAST };                          //@ID:sr38
SwitchRec NCD_SW_VOLUME_LABEL	= { "V", EXACT };                         //@ID:sr39

	/* The strings in these switches must match the strings
	 * in the same switches in the UNERASE program. */

SwitchRec LIST_FILES		= { "L", AT_LEAST };                      //@ID:sr40

//@EndBlock
#if 0
NIFileInfoRec fileInfo[NUM_EXE_FILES] =
  {
/*    Standard		Alternate				Password                Always	Which args	Which switches					*/
/*    EXE name		EXE name	Help Topic ID		ID                      TTY?	cause TTY?	cause TTY?		Specific switches	*/
/*    ----------    	----------	--------------		-----                   ------	------------	---------------		------------------	*/
    { "BE",		    NULL_STR,	H_NI_BE,		0,                      TRUE,						},
    { "CALIBRAT",	NULL_STR,    	H_NI_CALIBRATE,		PWD_PROG_CALIBRAT,      FALSE,	TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "DISKEDIT",	"DE",	     	H_NI_DISKEDITOR,	PWD_PROG_DISKEDIT,      FALSE,	TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "DISKMON", 	NULL_STR,    	H_NI_DISKMON,		0,                      FALSE,	TTY_ARG_NONE,	TTY_SWITCH_LISTED,	&SW_DISKLIGHT, &SW_PROTECT,
      /* cont. */												                        &SW_PARK, &SW_STATUS,
      /* cont. */                                                                                                                       &SW_UNINSTALL },
    { "DISKREET",	NULL_STR,    	H_NI_DISKREET,		0,                      FALSE,	TTY_ARG_NONE,	TTY_SWITCH_LISTED,	&SW_SHOW, &SW_HIDE, &SW_CLOSE, &SW_ON, &SW_OFF },
    { "DISKTOOL",	NULL_STR,    	H_NI_DISKTOOL,		PWD_PROG_DISKTOOL,      FALSE,	TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "DS",		NULL_STR,	H_NI_DIRECTORY_SORT,	0,                      FALSE,	TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "SMARTCAN",	NULL_STR,    	H_NI_SC,		0,                      FALSE,	TTY_ARG_NONE,	TTY_SWITCH_LISTED,	&ON_EP, &OFF_EP, &UNINSTALL_EP, &STATUS_EP },
    { "FA",		NULL_STR,	H_NI_FA,		0,                      TRUE,						},
    { "FD",		NULL_STR,	H_NI_FD,		0,                      TRUE,						},
    { "FILEFIND",	"FF",		H_NI_FILEFIND,		0,                      FALSE,	TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "FILEFIX", 	NULL_STR,    	H_NI_FILEFIX,		PWD_PROG_FILEFIX,       FALSE,	TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "FL",		NULL_STR,	H_NI_FL,		0,                      TRUE,						},
    { "FS",		NULL_STR,	H_NI_FS,		0,                      TRUE,						},
    { "IMAGE",		NULL_STR,    	H_NI_IMAGE,		0,                      TRUE,						},
    { "LP",		NULL_STR,	H_NI_LP,		0,                      TRUE,						},
//    { "MMSETUP",	NULL_STR,	H_NI_MEMMAX,		0,                      FALSE,	TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "NCACHE2",	NULL_STR,    	H_NI_CACHE,		0,                      TRUE,						},
    { "NCC",     	NULL_STR,    	H_NI_CONTROL_CENTER,	0,                      FALSE,	TTY_ARG_NONE,	TTY_SWITCH_LISTED,	&SW_BW80, &SW_CO80, &SW_50_LINES, &SW_40_LINES,
      /* cont. */												                        &SW_43_LINES, &SW_35_LINES, &SW_25_LINES,
      /* cont. */												                        &SW_FAST_KEY, &SW_LEFT, &SW_COMMENT,
      /* cont. */												                        &SW_SET_ALL, &SW_CURSOR, &SW_PALETTE, &SW_DOSCOLOR,
      /* cont. */												                        &SW_KEYRATE, &SW_NO_DATE, &SW_START, &SW_STOP,
      /* cont. */												                        &SW_DISPLAY },
    { "NCD",     	NULL_STR,    	H_NI_NCD,		0,                      FALSE,	TTY_ARG_ANY,	TTY_SWITCH_LISTED,	&NCD_SW_NOWRITE, &NCD_SW_VOLUME_LABEL },
    { "NUCONFIG",	NULL_STR,	H_NI_NUCONFIG,		PWD_PROG_NUCONFIG,      FALSE,	TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "NDD",     	NULL_STR,    	H_NI_NDD, 		PWD_PROG_NDD,           FALSE,	TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "NDIAGS",		NULL_STR,	H_NI_NDIAGS,		PWD_PROG_NDIAG,         FALSE,	TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "RESCUE",		NULL_STR,    	H_NI_RESCUE,		PWD_PROG_RESCUE,        FALSE,	TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "DUPDISK",        "DD",       	H_NI_DD,	        0,                      FALSE,  TTY_ARG_NONE,   TTY_SWITCH_NONE, },
    { "SFORMAT", 	"SF",	     	H_NI_SF,		PWD_PROG_SFORMAT,       FALSE,	TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "SPEEDISK",	"SD",	     	H_NI_SD,		PWD_PROG_SPEEDISK,      FALSE,	TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "SYSINFO", 	"SI",	     	H_NI_SI,		0,                      FALSE,	TTY_ARG_NONE,	TTY_SWITCH_LISTED,	&SW_TSR, &SW_SUMMARY, &SW_DISKINFO },
    { "TS",		NULL_STR,	H_NI_TS,		0,                      TRUE,						},
    { "UNERASE", 	NULL_STR,    	H_NI_UNERASE,		PWD_PROG_UNERASE,       FALSE,	TTY_ARG_WILD,	TTY_SWITCH_LISTED,	&LIST_FILES },
    { "UNFORMAT",	NULL_STR,    	H_NI_UNFORMAT,		PWD_PROG_UNFORMAT,      FALSE,	TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "WIPEINFO",	NULL_STR,    	H_NI_WIPEINFO,		PWD_PROG_WIPEINFO,      FALSE,	TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
  };
#else
/*    Standard          Alternate                       Password                         Always	        Which args	     Which switches		         			*/
/*    EXE name          EXE name    Help Topic ID		   ID                   Disk #   TTY?	        cause TTY?	     cause TTY?		    Specific switches	*/
/*    ----------    	----------	--------------		-----------------       ------	------          ------	         ---------------	-----------------	*/
NIFileInfoRec fileInfo[NUM_EXE_FILES] =   {
    { "DISKEDIT",	"DE",	     	H_NI_DISKEDITOR,	PWD_PROG_DISKEDIT,      2,      FALSE,	        TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "NAVDX",     	NULL_STR,    	H_NI_NAV, 		    PWD_PROG_NAV,           2,      FALSE,	        },
    { "NDD",     	NULL_STR,    	H_NI_NDD, 		    PWD_PROG_NDD,           2,      FALSE,	        TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
    { "UNERASE", 	NULL_STR,    	H_NI_UNERASE,		PWD_PROG_UNERASE,       4,      FALSE,	        TTY_ARG_WILD,	TTY_SWITCH_LISTED,	&LIST_FILES },
    { "UNFORMAT",	NULL_STR,    	H_NI_UNFORMAT,		PWD_PROG_UNFORMAT,      1,      FALSE,          TTY_ARG_NONE,	TTY_SWITCH_NONE,	},
  };
#endif

  /*----------------------------------------------------------------------*
 * This is the dialog box for the opening "guilt screen"		*
 *----------------------------------------------------------------------*/

//@Block Type:DialogString
Byte *guiltStrings[] =				/* Dialog strings	*/
    {
    DIALOG_EMPTY_LINE,
    "%s1",					/* product name		*/
    "Version %s2",    				/* version string	*/                //@ID:Version
    DIALOG_EMPTY_LINE,
    " A Product of Symantec Corporation ",                                //@ID:Aproduct
    DIALOG_EMPTY_LINE,
    " %3a%s3%1a ",				/* user name		*/
    " %3a%s4%1a ",				/* company name		*/
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };
//@EndBlock

DialogRec guiltDlg =				/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS,			/* flags		*/
    NULL,		      			/* title		*/
    guiltStrings,				/* strings		*/
    };

Byte *cantUpdateRTLFile[] =
    {
    DIALOG_EMPTY_LINE,
    " Can't update %s1! ",                                                //@ID:Cantupdate Type:DialogString
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };

/*----------------------------------------------------------------------*
 * These are the components of the main dialog box.			*
 *----------------------------------------------------------------------*/

//@Block Type:DialogTitle
Byte COMMAND_STR[]	= "Commands";
Byte DESCRIPTION_STR[]	= "Description";
//@EndBlock

ScrollBarRec mainListScrollBar;

extern CmdInfoRec far* commands [MAX_COMMANDS];

ListRec mainList =		       	/* List	description		*/
    {
    0, 0,				/* row, col (set by library)	*/
    1,					/* number of columns		*/
    COMMAND_LIST_WIDTH,	     		/* width of each column		*/
    COMMAND_LINES,			/* number of rows		*/
    MainListWriteEntry,  		/* procedure to display entry	*/
    MainListDoMouse,			/* procedure for mouse events	*/
    NULL,				/* procedure to check matches	*/
    MainListEventFilter,		/* procedure to filter events	*/
    (void far*) commands,   	   	/* start address of entries	*/
    sizeof(CmdInfoRec far*),    	/* size of each entry		*/
    					/* flags used for operation	*/
    ACCEPT_DCLICKS | USE_ACTIVE,
    &mainListScrollBar,  		/* pointer to scroll bar defn	*/
    NULL_STR,	       			/* str to display between cols	*/
    5, 2,				/* scroll delay values		*/
    0,					/* number of entries in list	*/
    0,					/* top entry			*/
    0,					/* active entry			*/
    0,					/* separator width		*/
    COMMAND_STR,			/* list box title		*/
    };

						/* Used as placeholder	*/
OutputFieldRec mainField = {'s', NO_FIRST_DRAW, 0, NULL};

Byte *mainStrings[] =  				/* Dialog strings	*/
    {
#ifndef EMERGENCY
    DIALOG_EMPTY_LINE,
#endif
    "%69f ",
    DIALOG_2COLS,
    DIALOG_LEFT,
    DIALOG_EMPTY_LINE,
    "  %L1",
    DIALOG_EMPTY_LINE,
    DIALOG_NEXT_COL,
    "%F2",
    DIALOG_1COL,
    "  %O3",
    DIALOG_END
    };

DialogRec mainDlg =				/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
#ifndef EMERGENCY
    NORMAL_DIALOG_FLAGS | HAS_MENU_BAR,		/* flags		*/
#else
    NORMAL_DIALOG_FLAGS | ABOUT_ON_SYS_MENU,	/* flags		*/
#endif
    " Norton AntiVirus Professional ",     		/* title		*/            //@ID:NortUtils Type:DialogTitle
    mainStrings,				/* strings		*/
    };

/*----------------------------------------------------------------------*
 * This dialog box appears if the program runs out of memory when 	*
 * reading the command definition file, or if the user tries to create	*
 * a new command and there is no memory left.				*
 *----------------------------------------------------------------------*/

Byte OUT_OF_MEMORY[] = "Out of memory";                                   //@ID:Outofmem Type:DialogString

Byte *noMemStrings[] =				/* Dialog strings 	*/
    {
    DIALOG_EMPTY_LINE,
    OUT_OF_MEMORY,
    DIALOG_EMPTY_LINE,
    "%B1",
    DIALOG_END
    };

DialogRec noMemDlg =				/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS, 			/* flags		*/
    errorTitle,					/* title		*/
    noMemStrings,				/* strings		*/
    };

#ifdef EMERGENCY
/*----------------------------------------------------------------------*
 * This dialog box appears if the program that needs to be executed  	*
 * cannot be found on emergency disk number 1.  It prompts for second   *
 *----------------------------------------------------------------------*/
Byte *askEmergencyDisk2[] =
    {
    DIALOG_EMPTY_LINE,
    "Insert Emergency Disk 2",                                                       //@ID:InsertDisk2 Type:DialogString Center
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };

Byte *askEmergencyDisk3[] =
    {
    DIALOG_EMPTY_LINE,
    "Insert Emergency Disk 3",                                                       //@ID:InsertDisk3 Type:DialogString Center
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };

Byte *askEmergencyDisk4[] =
    {
    DIALOG_EMPTY_LINE,
    "Insert Emergency Disk 4",                                                       //@ID:InsertDisk4 Type:DialogString Center
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };

Byte *askEmergencyDiskErr[] =
    {
    DIALOG_EMPTY_LINE,
    "Invalid executable name",                                                       //@ID:InsertDiskErr Type:DialogString Center
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };

Byte  szPauseMsg[] = "Reinsert Emergency Disk 1. Press a key when ready.";
#endif
/*----------------------------------------------------------------------*
 * This dialog box appears when a program is launched (actually, just	*
 * before we start searching the path for the file to EXEC).		*
 *----------------------------------------------------------------------*/

Byte *loadingStrings[] =     			/* Dialog strings 	*/
    {
    DIALOG_EMPTY_LINE,
    " Loading... ",                                                       //@ID:loading Type:DialogString Center  Margin1
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };

DialogRec loadingDlg =				/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS | POP_UP_BOX,		/* flags		*/
    NULL_STR,					/* title		*/
    loadingStrings,				/* strings		*/
    };

/*----------------------------------------------------------------------*
 * This dialog box appears if we cannot find the executable file	*
 * for a particular command as specified in the menu definition file.	*
 *----------------------------------------------------------------------*/

Byte *badCmdStrings[] =				/* Dialog strings 	*/
    {
    DIALOG_EMPTY_LINE,
    " Unable to locate the command ",                                     //@ID:Unable Type:DialogString
    " %3a\"%s1\"%1a ",
    DIALOG_EMPTY_LINE,
    "%B2",
    DIALOG_END
    };

DialogRec badCmdDlg =				/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS, 			/* flags		*/
    errorTitle,					/* title		*/
    badCmdStrings,				/* strings		*/
    };

/*----------------------------------------------------------------------*
 * This dialog is only used if compiled with the EMERGENCY switch set. 	*
 * It appears if the user attempts to leave the program. Since this	*
 * program operates as the command interpreter, there is nowhere to     *
 * exit to.								*
 *----------------------------------------------------------------------*/

#ifdef EMERGENCY

Byte *CantExitFromProgram[] =
    {
    DIALOG_EMPTY_LINE,
    "You cannot exit from this program.",
    "You must remove the floppy diskette",
    "from drive A: and reboot the computer.",
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };

#endif

/*----------------------------------------------------------------------*
 * This is similar to the DialogRec badPasswordDlg in pwstring.c in the	*
 * language library, but it's phrasing is slightly different since	*
 * the norton program won't exit if the password is incorrect.		*
 *----------------------------------------------------------------------*/

Byte *myBadPasswordStrings[] =	   		/* Dialog strings	*/
    {
    DIALOG_EMPTY_LINE,
    "%3aIncorrect%1a",                                                    //@ID:Incorrect Type:DialogString
    " Press any key to continue ",                                        //@ID:Pressany Type:DialogString
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };

DialogRec myBadPasswordDlg =	   		/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS, 			/* flags		*/
    errorTitle,      				/* title		*/
    myBadPasswordStrings,			/* strings		*/
    };

/*----------------------------------------------------------------------*
 * Confirmation dialog box for "Delete menu item"			*
 *----------------------------------------------------------------------*/

extern Byte *yesNoButtonStrings[];

ButtonsRec deleteConfirmButtons =   		/* Dialog buttons	*/
    {
    yesNoButtonStrings,				/* strings		*/
    NORMAL_BUTTON_FLAGS | USE_PAD | USE_VALUE, 	/* flags		*/
    0,						/* value		*/
    0, 0,					/* row, col		*/
    1, 2,       				/* pad, gap 		*/
    };

Byte *deleteConfirmStrings[] =			/* Dialog strings 	*/
    {
    DIALOG_EMPTY_LINE,
    " Do you really want to remove ",                                     //@ID:Doyou Type:DialogString
    " %3a\"%s1\"%1a? ",
    DIALOG_EMPTY_LINE,
    " %B2 ",
    DIALOG_END
    };

DialogRec deleteConfirmDlg =			/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS, 			/* flags		*/
    " Delete Menu Item ",			/* title		*/                    //@ID:Deletemenu Type:DialogTitle
    deleteConfirmStrings,			/* strings		*/
    };

/*----------------------------------------------------------------------*
 * Dialog box for Add/Edit Menu Item command 				*
 *----------------------------------------------------------------------*/

//@Block Type:DialogTitle
Byte addCommandTitle[]  = " Add Menu Item ";
Byte editCommandTitle[] = " Edit Menu Item ";
//@EndBlock

TERec editCommandNameTE = 			/* Text-edit field	*/
    {
    0, 0,					/* row, col		*/
    COMMAND_WIDTH + 2, 	 			/* width		*/
    NULL,					/* string		*/
    COMMAND_WIDTH,    				/* max length		*/
    0, 0,    					/* insert pt, offset	*/
    NULL,    					/* validation function	*/
    0,	  	   				/* flags		*/
    };

TERec editCommandDosTE =  			/* Text-edit field	*/
    {
    0, 0,					/* row, col		*/
    40,	    					/* width		*/
    NULL,					/* string		*/
    DOS_CMD_WIDTH,	    			/* max length		*/
    0, 0,    					/* insert pt, offset	*/
    NULL,    					/* validation function	*/
    0,	  	   				/* flags		*/
    };

Byte *editCommandRadioStrings[11];  		/* Patched at run time	*/

RadioButtonsRec	editCommandRadios =
    {
    editCommandRadioStrings,			/* strings		*/
    USE_VALUE | USE_PAD,			/* flags		*/
    0,						/* value		*/
    1, 0,					/* pad, gap		*/
    };

//@Group:Norton.nistr.editcommandbut
//@Block Type:DialogCommand
Byte *editCommandButtonStrings[] = 		/* Button strings	*/
    {
    "  ^OK  ",                                                            //@ID:ok
    "De^scription...",                                                    //@ID:descrip
    "^Cancel",                                                            //@ID:cancel
    NULL_STR,
    };
//@EndBlock
//@EndGroup

ButtonsRec editCommandButtons =			/* Dialog buttons	*/
    {
    editCommandButtonStrings, 			/* strings		*/
    NORMAL_BUTTON_FLAGS | USE_PAD,	 	/* flags		*/
    0,						/* value		*/
    0, 0,					/* row, col		*/
    1, 2,       				/* pad, gap 		*/
    };

//@Group:Norton.nistr.editCommandStrings
//@Block Type:DialogPopup
Byte *editCommandStrings[] =			/* Dialog strings 	*/
    {
    DIALOG_EMPTY_LINE,
    DIALOG_LEFT,
    " ^Name in menu: %T1 ",                                               //@ID:Nameinmenu
    DIALOG_EMPTY_LINE,
    " ^DOS command:  %T2 ",                                               //@ID:Doscommand
    DIALOG_EMPTY_LINE,
    DIALOG_CENTER,
    "%22gTopic",                                                          //@ID:Topic
    " %R3 ",
    "%g",
    DIALOG_EMPTY_LINE,
    " %B4 ",
    DIALOG_END
    };
//@EndBlock
//@EndGroup

DialogRec editCommandDlg =			/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS, 			/* flags		*/
    editCommandTitle,				/* title		*/
    editCommandStrings,				/* strings		*/
    };

/*----------------------------------------------------------------------*
 * This dialog box appears if the user has sorted the commands by	*
 * topic and chooses the Add Menu Item command.				*
 *----------------------------------------------------------------------*/

//@Group:Norton.nistr.addwhatbuttons
//@Block Type:DialogCommand Center=
Byte *addWhatButtonStrings[] = 		/* Button strings	*/
    {
    "C^ommand",
    " ^Topic ",
    "^Cancel ",
    NULL_STR,
    };
//@EndBlock
//@EndGroup

ButtonsRec addWhatButtons =			/* Dialog buttons	*/
    {
    addWhatButtonStrings, 			/* strings		*/
    NORMAL_BUTTON_FLAGS | USE_PAD, 	 	/* flags		*/
    0,						/* value		*/
    0, 0,					/* row, col		*/
    1, 2,       				/* pad, gap 		*/
    };

Byte *addWhatStrings[] =			/* Dialog strings 	*/
    {
    DIALOG_EMPTY_LINE,
    " What do you want to add? ",                                         //@ID:whatdo Type:DialogString
    DIALOG_EMPTY_LINE,
    " %B1 ",
    DIALOG_END
    };

DialogRec addWhatDlg =				/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS, 			/* flags		*/
    addCommandTitle,				/* title		*/
    addWhatStrings,				/* strings		*/
    };

/*----------------------------------------------------------------------*
 * This dialog box appears if the user tries to create too many topics.	*
 *----------------------------------------------------------------------*/

Byte *tooManyTopicsStrings[] =			/* Dialog strings 	*/
    {
    DIALOG_EMPTY_LINE,
    " There is a maximum of %d1 topics. ",                                //@ID:Thereis Type:DialogString
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };

DialogRec tooManyTopicsDlg = 			/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS, 			/* flags		*/
    errorTitle,      				/* title		*/
    tooManyTopicsStrings,  			/* strings		*/
    };

/*----------------------------------------------------------------------*
 * Dialog box for Edit Menu Item command, when on a command topic.	*
 *----------------------------------------------------------------------*/

//@Group:Norton.nistr.edittopicbutton
//@Block Type:DialogCommand Center=
Byte *editTopicButtonStrings[] = 		/* Button strings	*/
    {
    "      ^OK      ",                                                    //@ID:ok
    "^Description...",                                                    //@ID:descrip
    "    ^Cancel    ",                                                    //@ID:cancel
    NULL_STR,
    };
//@EndBlock
//@EndGroup

ButtonsRec editTopicButtons =			/* Dialog buttons	*/
    {
    editTopicButtonStrings, 			/* strings		*/
    NORMAL_BUTTON_FLAGS | USE_PAD | 	 	/* flags		*/
        ARRANGE_VERTICAL,
    0,						/* value		*/
    0, 0,					/* row, col		*/
    1, 1,       				/* pad, gap 		*/
    };

Byte *editTopicStrings[] =			/* Dialog strings 	*/
    {
    DIALOG_EMPTY_LINE,
//@Paragraph ID:edittopicstrings Type:DialogString
    " Enter the topic name and choose ",
    " a position for this topic. ",
//@EndParagraph
    DIALOG_EMPTY_LINE,
    DIALOG_2COLS,
    " %L1  ",
    DIALOG_NEXT_COL,
    DIALOG_EMPTY_LINE,
    DIALOG_EMPTY_LINE,
    DIALOG_EMPTY_LINE,
    "  %B2 ",
    DIALOG_1COL,
    DIALOG_END
    };

DialogRec editTopicDlg =			/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS, 			/* flags		*/
    editCommandTitle,				/* title		*/
    editTopicStrings,				/* strings		*/
    };

/*----------------------------------------------------------------------*
 * Command description mini-editor dialog box.				*
 *----------------------------------------------------------------------*/

extern	Word descrBuffer[];

Word	descrBoxOffset = 33;		/* Adjust as needed to move box	*/

EditBoxRec descrEditBox =
    {
    DESCR_WIDTH,				/* width		*/
    DESCR_LINES,				/* height		*/
    descrBuffer,				/* data			*/
    };

UserControlRec descrEditUCR =
    {
    0,						/* flags		*/
    EditBoxDimensions,				/* initControl routine	*/
    EditBoxWrite,				/* drawControl routine	*/
    EditBoxEvent,				/* handleEvent routine	*/
    &descrEditBox,				/* data			*/
    };

OutputFieldRec descrEditField =
    {
    's', 					/* type			*/
    LEFT_FIELD | NO_FIRST_DRAW,        		/* flags		*/
    13,						/* maxWidth		*/
    NULL,					/* data			*/
    };

//@Block Type:DialogString
Byte *editColorNames[] =
    {
    "Normal",                                                             //@ID:norm
    "Reverse",                                                            //@ID:reverse
    "Bold",                                                               //@ID:bold
    "Bold Reverse",                                                          //@ID:underline
    };
//@EndBlock

Byte *editDescrStrings[] =			/* Dialog strings 	*/
    {
    DIALOG_2COLS,
    DIALOG_EMPTY_LINE,
    DIALOG_EMPTY_LINE,
    DIALOG_EMPTY_LINE,
    "  %U1   ",
    DIALOG_NEXT_COL,
    DIALOG_LEFT,
    DIALOG_EMPTY_LINE,
    DIALOG_EMPTY_LINE,
    DIALOG_EMPTY_LINE,
//@Paragraph ID:Changetext Type:DialogString
    " Press F2 to ",
    " change text ",
    " attribute.  ",
    DIALOG_EMPTY_LINE,
    " Current attribute ",
    " is %O2 ",
    DIALOG_EMPTY_LINE,
    " Press %3aTab%1a to move ",
    " between controls. ",
//@EndParagraph
    DIALOG_EMPTY_LINE,
    DIALOG_EMPTY_LINE,
    DIALOG_CENTER,
    " %B3 ",
    DIALOG_EMPTY_LINE,
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };

DialogRec editDescrDlg =			/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS, 			/* flags		*/
    editCommandTitle,				/* title		*/
    editDescrStrings,				/* strings		*/
    };

/*----------------------------------------------------------------------*
 * 									*
 *----------------------------------------------------------------------*/

Byte *cantFindNuConfigStrs[] =
    {
    DIALOG_EMPTY_LINE,
    "Could not find",                                                     //@ID:Couldnot Type:DialogString
    "%3aNUCONFIG.EXE%1a",
    DIALOG_EMPTY_LINE,
//@Paragraph ID:TheConfig Type:DialogString
    "The %3aConfiguration%1a menu",
    "will be disabled.",
//@EndParagraph
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };

DialogRec cantFindNuConfigDlg =			/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS, 			/* flags		*/
    NULL,					/* title		*/
    cantFindNuConfigStrs,			/* strings		*/
    &okButton
    };

/*----------------------------------------------------------------------*
 * Dialog boxes for reporting file I/O errors. 				*
 * The appropritate strings are patched into the DialogRec at run-time.	*
 *----------------------------------------------------------------------*/

Byte *cantFindFileStrings[] =			/* Dialog strings 	*/
    {
    DIALOG_EMPTY_LINE,
    " Unable to locate the file %3a%-30p1%1a ",                           //@ID:Unable1 Type:DialogString
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };

Byte *cantOpenFileStrings[] =			/* Dialog strings 	*/
    {
    DIALOG_EMPTY_LINE,
    " Unable to open the file %3a%-30p1%1a ",                             //@ID:Unable2 Type:DialogString
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };

Byte *cantWriteFileStrings[] =			/* Dialog strings 	*/
    {
    DIALOG_EMPTY_LINE,
    " Error writing the the file %3a%-30p1%1a ",                          //@ID:ErrorWrite Type:DialogString
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };

Byte *corruptedFileStrings[] =			/* Dialog strings 	*/
    {
    DIALOG_EMPTY_LINE,
    " The file %3a%-30p1%1a is corrupted ",                               //@ID:Thefile Type:DialogString
    DIALOG_EMPTY_LINE,
    DIALOG_END
    };

DialogRec fileErrorDlg = 			/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS, 			/* flags		*/
    errorTitle,					/* title		*/
    NULL,					/* strings		*/
    &okButton,					/* buttons		*/
    };

/*----------------------------------------------------------------------*
 * These are the pull-down menu definitions.				*
 *----------------------------------------------------------------------*/

#ifndef EMERGENCY

/*  WARNING:
 *  IF MORE OPTIONS ARE ADDED TO menuItems[] THEN YOU MUST MODIFY "CUT_FROM"
 *  AND "CUT_TO" TO REFLECT THE NEW LOCATIONS OF THE "Add menu item" AND
 *  "Quit".  THE LATTER IS PUT INTO THE FORMER WHEN NUCONFIG IS USED
 *  TO LOCK OUT EDITING OF THE LIST.
 *  "CUT_FROM" AND "CUT_TO" ARE LOCATED IN THE MENU.H FILE
 */

//@Group:Norton.nistr.menuitems
//@Block Type:MenuCommand
MenuItemRec menuItems[] =		/* Contents of MAIN menu	*/
  {
    { "Sort by ^name",       	   NULL,	  H_NI_MENU_SORT_NAME,        //@ID:sortname
                                UNCHECKED,   ENABLED,  ALT_N,SORT_BY_NAME},
    { "Sort by ^topic",            NULL,	  H_NI_MENU_SORT_TOPIC,       //@ID:sorttopic
    				CHECKED, ENABLED,  ALT_T,SORT_BY_TOPIC},
    { "-",			   NULL,	  0,
    				UNCHECKED, DISABLED, 0,0},
    { "^Add menu item...",          NULL,	  H_NI_MENU_ADD_MENU_ITEM,    //@ID:addmenu
    				UNCHECKED, ENABLED, 0,ADD_CMD},
    { "^Edit menu item...",         NULL,	  H_NI_MENU_EDIT_MENU_ITEM,   //@ID:editmenu
    				UNCHECKED, ENABLED, 0,EDIT_CMD},
    { "^Delete menu item...",       NULL,	  H_NI_MENU_DEL_MENU_ITEM,    //@ID:deletemenu
    				UNCHECKED, DISABLED, 0,DELETE_CMD},
    { "-",			   NULL,	  0,
    				UNCHECKED, DISABLED, 0,0},
    { "E^xit",			   NULL,	  H_NI_MENU_EXIT,             //@ID:exit
    				UNCHECKED, ENABLED, ALT_X,QUIT	},
    { NULL_STR }
  };
//@EndBlock
//@EndGroup

//@Group:Norton.nistr.configureitems
//@Block Type:MenuCommand
MenuItemRec configureItems[] =		/* Contents of CONFIGURE menu	*/
  {
    { "^Video and mouse...",	   NULL, H_NI_CONF_VIDEO_MOUSE,           //@ID:vidmouse
    				UNCHECKED, ENABLED,  0,VIDEO_MOUSE},
    { "P^rinter Setup...", 	NULL,  H_NI_CONF_PRINTER,              //@ID:prntconfig
    				UNCHECKED, ENABLED,  0,PRINTER_CONFIG},
    { "^Startup Programs...",	   NULL,  H_NI_CONF_STARTUP_PROGS,         //@ID:startup
    				UNCHECKED, ENABLED,  0,STARTUP_PROGS},
    { "^Alternate names...",	   NULL,  H_NI_CONF_ALT_NAMES,            //@ID:altnames
    				UNCHECKED, ENABLED,  0,ALT_NAMES},
    { "^Passwords...",	    	   NULL,  H_NI_CONF_PASSWORDS,            //@ID:passwords
    				UNCHECKED, ENABLED,  0,PASSWORDS},
    { "^Menu editing...",	   NULL,  H_NI_CONF_MENU_EDIT,                //@ID:menuedit
    				UNCHECKED, ENABLED,  0,MENU_EDIT},
    { NULL_STR }
  };
//@EndBlock
//@EndGroup

//@Group:Norton.nistr.helpitems
//@Block Type:MenuCommand
MenuItemRec helpItems[] =		/* Contents of HELP menu	*/
  {
    { "The Norton Ad^visor",	    NULL,  H_NI_HELP_ADVISOR,                   //@ID:advisor
    				UNCHECKED, ENABLED,  0, IDM_ADVISE},
    { "-",			   NULL,   0,
    				UNCHECKED, DISABLED, 0,0},
    { "^Index",			    NULL,  H_NI_HELP_INDEX,                        //@ID:index
    				UNCHECKED, ENABLED,  0, IDM_HELP_INDEX},
    { "^Keyboard",		    NULL,  H_GLOBAL_USING_KEYB,                   //@ID:keyboard
    				UNCHECKED, ENABLED,  0, IDM_KEYBOARD},
    { "^Mouse",			    NULL,  H_GLOBAL_USING_MOUSE,                  //@ID:mouse
    				UNCHECKED, ENABLED,  0, IDM_MOUSE},
    { "^Dialog boxes",		    NULL,  H_GLOBAL_USING_DIALOGS,            //@ID:dlgboxes
    				UNCHECKED, ENABLED,  0, IDM_DIALOG},
    { "^Pulldown menus",	    NULL,  H_GLOBAL_USING_MENUS,              //@ID:pulldown
    				UNCHECKED, ENABLED,  0, IDM_PULLDOWN},
    { "^Using Help",		    NULL,  H_GLOBAL_HELP_ON_HELP,             //@ID:usinghelp
    				UNCHECKED, ENABLED,  0, IDM_USING_HELP},
    { "-",			   NULL,   0,
    				UNCHECKED, DISABLED, 0,0},
    { "^About...",		    NULL,  H_NI_HELP_ABOUT,                        //@ID:about
    				UNCHECKED, ENABLED,  0, IDM_ABOUT},
    { NULL_STR }
  };
//@EndBlock
//@EndGroup

//@Group:Norton.nistr.menulist
//@Block Type:MenuCommand
MenuRec menuList[] =			/* Contents of the menu bar	*/
  {
    { "^Menu",      	NULL,  0, 	menuItems,	NORMAL_DROP, ALT_M },     //@ID:menu
    { "^Configuration", NULL,  0, 	configureItems, NORMAL_DROP, ALT_C }, //@ID:configuration
    { "^Help",		NULL,  0, 	helpItems,	NORMAL_DROP, ALT_H },         //@ID:help
    { NULL_STR }
  };
//@EndBlock
//@EndGroup

MenuBarRec menuBar =			/* Appearance of the menu bar	*/
    {
    menuList,           	   	/* Menus in bar                 */
    ESC_OUT | ALT_OUT | MTYPE_NOCLOSEBOX, /* Operation bit flags	*/
    1,					/* Row of menu bar		*/
    0, 0,				/* Left and right sides 	*/
    NULL,				/* Ptr to alternate menu box	*/
    NULL_STR,				/* Msg at right end of bar	*/
    1,					/* Hilite padding around titles	*/
    0,					/* Gap between the titles	*/
    0,					/* Offset of box from left side */
    NULL				/* Application title		*/
    };


#endif // EMERGENCY

/**/
