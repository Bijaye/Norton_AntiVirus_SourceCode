/* $Header:   S:/NORTON/VCS/NORTON.C_V   1.26   23 Feb 1995 14:20:46   MARKK  $ */

/*----------------------------------------------------------------------*
 * NORTON.C									*
 *									*
 * Norton Integrator main module					*
 *									*
 * Copyright 1991 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/NORTON/VCS/NORTON.C_V  $
 * 
 *    Rev 1.26   23 Feb 1995 14:20:46   MARKK
 * Allocate an environment if none exists
 * 
 *    Rev 1.25   17 Feb 1994 02:44:06   TMAGILL
 * Remove special about box handling
 * 
 *    Rev 1.24   17 Mar 1993 11:04:40   JOEP
 * new about box
 * 
 *    Rev 1.23   10 Mar 1993 10:29:24   SKURTZ
 * Changed to CheckForEnoughMemory() for testing initial memory threshold
 *
 *    Rev 1.22   08 Mar 1993 16:35:00   SKURTZ
 * Added low memory TTY error message.
 *
 *    Rev 1.21   04 Mar 1993 19:13:20   SKURTZ
 * Allow VMM to use EMS and XMS, too.
 *
 *    Rev 1.20   23 Dec 1992 09:33:30   SKURTZ
 * Fixed ptr NU6313 (Password checking now functional).  Fixed ptr NU6657 (checks
 * for minimum free available memory and exits with a message that makes sense)
 *
 *    Rev 1.19   03 Dec 1992 10:50:02   SKURTZ
 * Upgrade to new configuration manager.
 *
 *    Rev 1.18   16 Nov 1992 18:41:52   SKURTZ
 * Inserted #ifdefs for making EMERGENCY boot disk for DOS 6
 *
 *    Rev 1.17   29 Oct 1992 15:57:14   SKURTZ
 * Updated MWindowInit() to WinInitManager(); Updated VMMInit to use handle tables
 *
 *    Rev 1.16   17 Oct 1992 02:11:42   JOHN
 * General cleanup and update for New Loader
 *
 *    Rev 1.15   07 Oct 1992 12:11:30   SKURTZ
 * Quit command of leftmost pulldown menu is moved up to replace Edit command if
 * NUCONFIG is used to remove the ability to edit the menus.
 *
 *    Rev 1.14   25 Sep 1992 14:05:18   SKURTZ
 * Conversion to LARGE memory model
 *
 *    Rev 1.13   10 Sep 1992 16:49:58   BASIL
 * Uncommented and updated the Diskreet and MenuEdit flag tests.
 * Updated GuiltScreen routine to work with new patch scheme
 *
 *    Rev 1.12   24 Jul 1992 17:36:04   SKURTZ
 * Preparations for Scorpio.  Adjustments to work with Passport Library.  Fixed
 * source of compiler warning messages.
 *
 *    Rev 1.11   05 Aug 1991 19:00:56   PETERD
 * Changed to using the PROGRAM_NAME structure for the version label,
 * instead of the PRODUCT_NAME, so the product can have version '6.01'
 *
 *    Rev 1.10   24 May 1991 16:39:10   ED
 * Added a null-termination of the company name and user name strings
 * so they wouldn't trash the screen at maximum length;
 *
 *    Rev 1.9   17 May 1991 22:44:18   JOHN
 * Added DO_GUILT_SCREEN flag.  This will let us make in-house versions
 * of the Integrator that don't display the guilt screen.
 *
 *    Rev 1.8   29 Apr 1991 12:44:12   JOHN
 * When doing the guilt screen, copied user and company name to a
 * near memory buffer before passing them to the dialog manager.
 *
 *    Rev 1.7   25 Apr 1991 09:55:48   ED
 * Added the guilt dialog back to the code.  In all the confusion
 * with getting the user name stuff working, I left out the main
 * reason for doing it in the first place!
 *
 *    Rev 1.6   25 Apr 1991 08:22:46   ED
 * Put all user name/company name stuff in the DoGuiltScreen()
 * function, and made it work.  I have to use MemoryFarMove()
 * instead of StringLocalize() or the stack gets trashed.
 *
 *    Rev 1.5   24 Apr 1991 16:03:06   ED
 * Added code to support the new StdUserName() dialog box, as well
 * as patching the NORTON.OVL file with the user name and company
 * name.
 *
 *    Rev 1.4   10 Apr 1991 17:45:44   ED
 * Removed the savedColorColorSet variable, which was obsolete
 *
 *    Rev 1.3   22 Mar 1991 21:06:14   JOHN
 * Changed <config.plainDeskTop> to <nlibConfig.plainDeskTop>
 *
 *    Rev 1.2   07 Feb 1991 14:56:40   ED
 * Slighly modified John's niPatchTable stuff for cutting off the
 * Configuration menu.  John cut it off one line too low.
 *
 *    Rev 1.1   16 Jan 1991 13:58:34   JOHN
 * Changed call to exit() to DOSExit()
 *
 *    Rev 1.0   20 Aug 1990 15:19:40   DAVID
 * Initial revision.
 */

#include	<pulldown.h>
#include	<dialog.h>
#include	<keys.h>
#include	<file.h>
#include	<main.h>
#include	<disk.h>
#include	<password.h>
#include	<nuconfig.h>
#include	<symcfg.h>
#include	<hyper.h>
#include	<nloader.h>
#include	<lint.h>
#include	<tkextrns.h>
#include	<mscstr.h>
#include	"nort-hlp.h"
#include	"defs.h"
#include	"proto.h"
#include	"menu.h"

/*----------------------------------------------------------------------*
 * Local constants							*
 *----------------------------------------------------------------------*/

#define VMM_HANDLE_TABLES  (64/20)+1

/* this number derived purely empirically  */
#define MIN_FREE_MEMORY      350*1024UL

Boolean 	gbOutOfMemory = FALSE;

/*----------------------------------------------------------------------*
 * Patch area externals							*
 *----------------------------------------------------------------------*/

extern PasswordPatchRec   far *lpPwdPatchArea;
extern UserPatchRec       far *lpUserPatchArea;
extern IntegratorPatchRec far *lpIntPatchArea;

/* Function definitions in norton.c */

void	LOCAL	  HideDiskreet		(void);;
void		  main			(char *args);
Boolean LOCAL	  InitMemory		(void);
void		  FinishUp		(void);
Boolean LOCAL	  ProcessCommandLine	(char *args);
void		  ProcessLauncherCmdLine(void);
void		  InitError		(void);
void		  RestoreError		(void);
void		  RedrawMenuBar		(void);
void		  SaveVarsToLoader	(void);
void		  RestoreVarsFromLoader	(void);
void LOCAL	  HideDiskreet 		(void);
Boolean LOCAL	  DoGuiltScreen		(void);
void LOCAL PASCAL CheckPSP		(void);


#if 0
void	LOCAL	HideDiskreet(void);
#endif

/*----------------------------------------------------------------------*
 * This procedure is the main entry point.				*
 *----------------------------------------------------------------------*/

void main(char *args)
{
    extern	Byte		*pszProductEnvVar;
    extern	Boolean		integrator;		/* library var	*/
    extern	Boolean		passwordOK;
    extern	Boolean		sortByName;
    extern	Boolean		iniSortByName;
#ifndef EMERGENCY
    extern	MenuItemRec	menuItems[];
    extern      NIRec		NIConfigRec;
    extern      CFGBLOCKREC     cfgblockrec;
#endif
    extern	ColorSet	colorSets[];		/* library var	*/
    extern	Boolean		commandsEdited;
    extern	Byte		commandComPath[];
    extern	Byte		line[];
    extern	NULoaderDataRec far *loaderData;
    extern	Byte		OUT_OF_MEMORY[];
    extern	Boolean		bClearDOSScreen;
    extern	Boolean		bNoHelpLine;

    auto	Word		err;
    auto	Boolean		firstLoad;

    bNoHelpLine	= TRUE;



    MemoryRelease();			/* Deallocate near heap		*/


    if (CheckForEnoughMemory(MIN_FREE_MEMORY))
	DOSExit();

    CheckPSP();
    
#if 0
    /* If Install has told us we are an International version,
     * hide the help topic for Diskreet. */

    if (!FlagTest(lpIntPatchArea->wFlags, INT_PATCH_FLAG_DISKREET))
        HideDiskreet();
#endif

#ifndef EMERGENCY
    /* If menu editing is not allowed,
     * chop off the editing commands from the "Menu" menu. */

    if (!FlagTest(lpIntPatchArea->wFlags, INT_PATCH_FLAG_MENU_EDIT))
	{
	menuItems[CUT_TO] = menuItems[CUT_FROM];
	menuItems[CUT_TO + 1 ].item_string[0] = '\0';
	}
#endif


    firstLoad = ProcessCommandLine(args);

    if (!InitMemory())
        {
	MainPrintBanner();
	DOSPrint("%s1\n", OUT_OF_MEMORY);
	DOSExit();
        }

    InitError();			/* Set up error handling	*/

    if (!firstLoad)
        integrator = TRUE;

#if 0
    // Kludge until the password stuff starts working
    passwordOK = loaderData->passwordOK = TRUE;
    loaderData->skipPassword = TRUE;
#endif

    // If we thought that last program was one of ours, but it
    // turns out that it wasn't, we need to clear the screen.
    if (!firstLoad && loaderData->wChildLibraryVersion == 0x0001)
	{
	loaderData->bScreenWasCleared = TRUE;
	bClearDOSScreen = TRUE;
	CursorSetRowCol(1, 0);
	}
    else
	bClearDOSScreen = FALSE;

    err = NUFastInitScreen();		/* Read .INI, init screen/mouse	*/

#ifndef EMERGENCY
	ConfigInit(0);
	ConfigBlockGet(&cfgblockrec);
#endif

    if (!firstLoad)
	MouseSetPosition(loaderData->mouseRow, loaderData->mouseCol);

    if (WinInitManager(10, TRUE, 0) != MWI_NO_ERROR)
	{
	gbOutOfMemory = TRUE;
	FinishUp();
	DOSExit();
	}

    /*
     * We did a RestoreVarsFromLoader() in ProcessCommandLine(),
     * so NUFastInitScreen() would have the flags it needed.
     * But NUFastInitScreen() changes the originalAttr and dosCursorSize
     * library vars.  We need to get those values from the loader again,
     * since we want the original orignalAttr, and also since NCC may have
     * changed those values for us.
     */

    if (!firstLoad)
        RestoreVarsFromLoader();

#ifdef EMERGENCY
    iniSortByName = TRUE;
#else
    iniSortByName = NIConfigRec.sortByName;
#endif




#ifndef EMERGENCY

    if (firstLoad)
        {
	sortByName = iniSortByName;

	/* Display the "guilt screen", or create it */
      	if (DoGuiltScreen() == FALSE)
	    {
	    // User doesn't want to tell us their name
	    FinishUp();
	    DOSExit();
	    }
	}

    if (firstLoad)
        NUConfigError(err, TRUE);	/* Report config err if any	*/


    HelpVMMInit(pszProductEnvVar, HELPFILE_NAME, HELPFILE_VERSION, TRUE);
    HyperHelpTopicSet(H_NI_MAIN);
#endif

    InitCmdBuf();
    InitCommandComPath(commandComPath);

    InitConfigMenu();

    ReadCommands(firstLoad);

    EventLoop(); 			/* Enter the main event loop 	*/

    integrator = FALSE;			/* Do normal screen clearing	*/

    /*
     * If the current sort order differs from what is saved
     * in the configuration file, then update the file.
     * We don't bother reporting any errors, but it probably
     * wouldn't be a bad idea (there is already a dialog box
     * defined for this).
     */
#ifndef EMERGENCY
    if (sortByName != iniSortByName)
        {
        NIConfigRec.sortByName = sortByName;
	ConfigBlockSet(&cfgblockrec);
	ConfigTerminate(0);
	}
#endif

    if (commandsEdited)
        WriteCommands();

    FinishUp();

    DOSExit();
}

/************************************************************************
 *                                                                      *
 * Description:  Initialize the VMM system				*
 *                                                                      *
 * Parameters:   NONE                                                   *
 *                                                                      *
 * Return Value: TRUE If successfull, FALSE otherwize                   *
 *                                                                      *
 ************************************************************************/

Boolean LOCAL InitMemory(void)
{
    auto        Ulong           lConv;
    auto        Ulong           lEMS;
    auto        Ulong           lXMS;
    auto        Ulong           lDisk;
    auto        Boolean		bResult;

#ifndef EMERGENCY
    lEMS = lXMS = lConv = ALL_MEMORY;
    lDisk = 0;
#else
    lConv = ALL_MEMORY;
    lEMS = lXMS = lDisk = 0;
#endif

    bResult = (Boolean)VMMInit(VMM_HANDLE_TABLES, &lConv, &lEMS, &lXMS, &lDisk);


    return(bResult);
}

/*----------------------------------------------------------------------*
 * This routine is responsible for general shutdown of memory and screen*
 * settings.  It then exits the program                                 *
 *----------------------------------------------------------------------*/

void FinishUp(void)
{
    extern	NULoaderDataRec far *loaderData;
    extern	Boolean		bClearDOSScreen;
    extern      Byte            OUT_OF_MEMORY[];

    if (loaderData->bScreenWasCleared)
	bClearDOSScreen = TRUE;

    if (!gbOutOfMemory)
        MWindowTerminate();

    NUFastRestoreScreen();     		/* Display/mouse cleanup	*/

    if (!gbOutOfMemory)
        HelpVMMDone();			/* Help module cleanup		*/

    VMMTerminate();

    RestoreError();			/* Error handling cleanup 	*/

    if (gbOutOfMemory)
	{
	DOSPrint("%s1\n", OUT_OF_MEMORY);
	}
}

/*----------------------------------------------------------------------*
 * Process NULoaderDataRec communication area				*
 *----------------------------------------------------------------------*/

static NULoaderDataRec localLoaderData;

Boolean LOCAL ProcessCommandLine(char *args)
{
    extern	Byte		*HELP_SCREEN[];
    extern	NULoaderDataRec	far *loaderData;
    auto	NLoaderRec	far *lpLoaderRec;

    NLoaderInit("NI", args);		/* Initailize the loader	*/

    lpLoaderRec = NLoaderGetRecPtr();
    if (lpLoaderRec->wState == NLDR_FIRST_TIME)
	{
	/* Use local data area the first time we're called */
	lpLoaderRec->lpData = &localLoaderData;
	lpLoaderRec->wDataSize = sizeof(NULoaderDataRec);
	}

    loaderData = lpLoaderRec->lpData;

    ProcessLauncherCmdLine();

    MainCheckHelp(HELP_SCREEN);

    if (lpLoaderRec->wState == NLDR_FIRST_TIME)
        {
        return(TRUE);
	}
    else
	{
        RestoreVarsFromLoader();
	return(FALSE);
	}
}

/*----------------------------------------------------------------------*
 * Copy NORTON's original command line from the NLoaderRec and parse it	*
 *									*
 * MainProcessCmdLine() modifies the command line you pass to it, so	*
 * we copy the actual command line to a static buffer before processing	*
 * it.  We wouldn't want to modify the one stored in the NLoaderRec.	*
 *----------------------------------------------------------------------*/

void ProcessLauncherCmdLine(void)
{
    static	Byte	 	loaderCmdLine[129];
    auto	NLoaderRec	far *lpLoaderRec;

    lpLoaderRec = NLoaderGetRecPtr();

    MemoryFarMove(loaderCmdLine,
		  lpLoaderRec->szReloadCmdline+1,
		  lpLoaderRec->szReloadCmdline[0]);

    loaderCmdLine[ lpLoaderRec->szReloadCmdline[0] ] = 0;

    MainProcessCmdLine(loaderCmdLine);
}

/*----------------------------------------------------------------------*
 * This procedure sets up handling of BIOS and DOS Ctrl-Break and 	*
 * critical errors.							*
 *----------------------------------------------------------------------*/

void InitError(void)
{
    DOSBreakOn();				/* Trap ^C, ^Break	*/
    DiskErrorOn(ErrorTrap);		  	/* Trap disk errors	*/
}

/*----------------------------------------------------------------------*
 * This procedure restores original handling of BIOS and DOS Ctrl-Break *
 * and critical errors.							*
 *----------------------------------------------------------------------*/

void RestoreError(void)
{
    DOSBreakOff();				/* Restore ^C, ^Break	*/
    DiskErrorOff();				/* Turn off trapping	*/
}

/*----------------------------------------------------------------------*
 * This procedure redraws the pull-down menu bar.			*
 *									*
 * The bar must be redrawn after the action for a pull-down menu item	*
 * is completed, in order to remove the highlight on the pull-down	*
 * menu name.								*
 *----------------------------------------------------------------------*/

void RedrawMenuBar(void)
{
#ifndef EMERGENCY
    extern	MenuBarRec	menuBar;   /* The pull-down menu bar	*/

    PulldownDrawBar(&menuBar);		/* Remove the menu highlight	*/
    HyperHelpTopicSet(H_NI_MAIN);	/* Reset default topic 		*/
    SetCommandHelp();			/* Set better topic if possible	*/
#endif
}


/*----------------------------------------------------------------------*
 * This routine saves important global variables to the loader's shared	*
 * data area so they can be retrieved after a launch.			*
 *----------------------------------------------------------------------*/

void SaveVarsToLoader(void)
{
    extern	NULoaderDataRec	far *loaderData;
    extern	ListRec		mainList;
    extern	Boolean		sortByName;
    extern	Boolean		passwordOK;
    extern	Byte		commandFile[];
    extern	Word		dosCursorSize;	/* library var	*/

    loaderData->sortByName       = sortByName;
    loaderData->mainListTop      = mainList.top_entry;
    loaderData->mainListActive   = mainList.active_entry;
    loaderData->originalAttr     = originalAttr;
    loaderData->dosCursorSize    = dosCursorSize;
    loaderData->passwordOK       = passwordOK;
    StringCopyFar(loaderData->commandFile, commandFile);
}

/*----------------------------------------------------------------------*
 * This routine restores important global variables from the loader's 	*
 * shared data area, where they were saved prior to a launch.		*
 *----------------------------------------------------------------------*/

void RestoreVarsFromLoader(void)
{
    extern	NULoaderDataRec	far *loaderData;
    extern	ListRec		mainList;
    extern	Boolean		sortByName;
    extern	Boolean		passwordOK;
    extern	Byte		commandFile[];
    extern	Word		dosCursorSize;	/* library var	*/

    sortByName		  = loaderData->sortByName;
    mainList.top_entry    = loaderData->mainListTop;
    mainList.active_entry = loaderData->mainListActive;
    originalAttr          = loaderData->originalAttr;
    dosCursorSize         = loaderData->dosCursorSize;
    passwordOK            = loaderData->passwordOK;
    StringCopyFar(commandFile, loaderData->commandFile);
}

#if 0

/*----------------------------------------------------------------------*
 * This routine gets rid of the H_NI_DISKREET help ID from the help	*
 * index and from the table of EXE commands (used to set the default	*
 * help topic).								*
 *----------------------------------------------------------------------*/

void LOCAL HideDiskreet (void)
{
    extern	Word		helpTopics[];
    extern	NIFileInfoRec	fileInfo [NUM_EXE_FILES];
    auto	Word		i;
    auto	Word		j;

    for (i = 0; helpTopics[i] != 0; i++)
        if (helpTopics[i] == H_NI_DISKREET)
	    {
	    for (j = i; helpTopics[j] != 0; j++)
		helpTopics[j] = helpTopics[j+1];
	    break;
	    }

    for (i = 0; i < NUM_EXE_FILES; i++)
        if (fileInfo[i].helpID == H_NI_DISKREET)
	    fileInfo[i].helpID = H_NI_MAIN;
}

#endif

/*----------------------------------------------------------------------*
 * DoGuiltScreen()							*
 *									*
 * Returns TRUE if user name OK, FALSE if user hit ESC.			*
 *----------------------------------------------------------------------*/
#ifndef EMERGENCY
Boolean LOCAL DoGuiltScreen(void)
{
    extern	VersionRec	PRODUCT_NAME;
    extern	VersionRec	PROGRAM_NAME;
    extern	SwitchRec	SW_NO_GUILT;
    extern	DialogRec	mainDlg;
    extern	DialogRec	guiltDlg;
    extern	Byte		*pszProductEnvVar;
    extern	Byte		*pszProductFileName;
    extern	Byte		*pszRTLFileName;
    extern	Byte		line[];
    extern      Byte 		*cantUpdateRTLFile[];
    auto	Byte            nortonPath[128];
    auto	DIALOG_RECORD	*dr;
    auto	Ulong		startTime;
    auto	Byte		userName[USER_NAME_LEN + 1];
    auto	Byte		companyName[COMPANY_NAME_LEN + 1];
    auto	Byte		serialNumber[1];
    auto	Word		result;

    if (MainSwitchSet(&SW_NO_GUILT))
	return(TRUE);

    MemoryFarMove(userName, lpUserPatchArea->szRegName, USER_NAME_LEN);
    MemoryFarMove(companyName, lpUserPatchArea->szRegCompany, COMPANY_NAME_LEN);

    if (userName[0])
	{
 	UserNameCrypt(lpUserPatchArea);

	MemoryFarMove(userName, lpUserPatchArea->szRegName, USER_NAME_LEN);
	MemoryFarMove(companyName, lpUserPatchArea->szRegCompany, COMPANY_NAME_LEN);

	companyName[COMPANY_NAME_LEN] = 0;
	userName[USER_NAME_LEN] = 0;

	dr = DialogOpen(&guiltDlg,
			PRODUCT_NAME.name,
			PROGRAM_NAME.version,
			userName,
			companyName);

	startTime = TimerTicks();
	while (TimerElapsedTicks(startTime) < 25)
		/* nothing */ ;

	DialogClose(dr, EVENT_USED);

	return(TRUE);
        }

    result = StdDlgGetUserName(mainDlg.title, userName, companyName, serialNumber, TRUE);

    if (result == ESC)
	return(FALSE);

    if (result == TRUE)
	{
	if (!DirSearchNortonPath(pszProductEnvVar, pszRTLFileName, nortonPath))
	    StdDlgError(cantUpdateRTLFile, pszRTLFileName);
	else
	    {
	    if (PatchAreaRead(pszProductEnvVar,
			      nortonPath,
			      USER_SIG,
			      lpUserPatchArea,
			      sizeof(UserPatchRec)) == FALSE)
		{
		StdDlgError(cantUpdateRTLFile, pszRTLFileName);
		}
	    else
		{
				// Erase the current contents....
		MemorySet(lpUserPatchArea, 0, sizeof(UserPatchRec));

				// Put in the new strings...
		StringCopyFar(lpUserPatchArea->szRegName, userName);
		StringCopyFar(lpUserPatchArea->szRegCompany, companyName);

		UserNameCrypt(lpUserPatchArea);

				/* Update the patch area */
		if (PatchAreaUpdate(pszProductFileName,
				    nortonPath,
				    lpUserPatchArea,
				    sizeof(UserPatchRec)) == FALSE)
		    {
		    StdDlgError(cantUpdateRTLFile, pszRTLFileName);
		    }
		}
	    }
	}

    return(TRUE);
}

#endif

typedef struct PSPRecord
    {
    WORD        exitCall;			/* 0xCD, 0x20			*/
    WORD        blockLength;
    BYTE        reserve1;
    BYTE        cpmCall[5];			/* 0x9A, ...			*/
    LPVOID      prevExitRoutine;
    LPVOID	prevCtrlCRoutine;
    LPVOID	prevFatalError;
    WORD        parentID;			/* (undocumented)		*/
    BYTE        handleTable[20];
    WORD        environmentSegPtr;
    LPVOID	userStatck;
    WORD        handleCount;
    LPVOID	handleAddress;
    BYTE        reserve2[24];
    BYTE        dosCall[3];			/* 0xCD, 0x21, 0xCB		*/
    BYTE        reserve3[9];
    BYTE        defaultFCB[16];
    BYTE        secondFCB[16];
    BYTE        reserve4[4];
    BYTE        commandLineLength;
    BYTE        commandLine[127];
    } PSPRecord;

#define MAKELP(h,l) (LPVOID)((((DWORD)(h))<<16L)|(WORD)(l))
void LOCAL PASCAL CheckPSP(void)
{
    auto	PSPRecord	*psp = (PSPRecord *)MAKELP(PSPSegment, 0);
    auto	LPBYTE		env;
    
    if (psp->environmentSegPtr != 0)
	return;
    
    env = DOSAllocParagraphs(1024 / 16);
    
    if (env == NULL)
	return;
    
    _fmemset(env, 0, 1024);
    
    _fstrcpy(env, "PATH=A:\\");
    
    psp->environmentSegPtr = HIWORD(env);
}
