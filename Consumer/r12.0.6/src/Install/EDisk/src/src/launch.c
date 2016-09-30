/* $Header:   S:/SRC/NORTON/VCS/LAUNCH.C_V   1.16   18 Mar 1993 22:28:58   JOHN  $ */

/*----------------------------------------------------------------------*
 * LAUNCH.C								*
 *									*
 * Copright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/SRC/NORTON/VCS/LAUNCH.C_V  $
 * 
 *    Rev 1.16   18 Mar 1993 22:28:58   JOHN
 * Don't ask for password if program can't be password protected.
 * 
 *    Rev 1.15   04 Mar 1993 17:29:50   SKURTZ
 * Rewrote DOSSearchPath() to change the order of the places files are searched for
 *
 *    Rev 1.14   14 Dec 1992 15:08:56   SKURTZ
 * EMERGENCY configuration only:  Now checks to see if selected program is
 * on emergency disk #1.  If not, uses alternate message reminding user to
 * re-insert emergency disk #1 so that STARTUP.EXE can reload.
 *
 *    Rev 1.13   30 Nov 1992 17:12:06   JOHN
 * Set szReloadFile[] from text in strings file.
 *
 *    Rev 1.12   17 Nov 1992 17:22:36   SKURTZ
 * When built with EMERGENCY on, removes path from szReloadFile[] in
 * NLoaderRec and replaces it with the just the filename alone.
 *
 *    Rev 1.11   16 Nov 1992 18:41:58   SKURTZ
 * Inserted #ifdefs for making EMERGENCY boot disk for DOS 6
 *
 *    Rev 1.10   19 Oct 1992 18:24:22   JOHN
 * Changed the way passwords are checked.
 *
 *    Rev 1.9   17 Oct 1992 02:11:46   JOHN
 * General cleanup and update for New Loader
 *
 *    Rev 1.8   25 Sep 1992 14:05:20   SKURTZ
 * Conversion to LARGE memory model
 *
 *    Rev 1.7   24 Jul 1992 17:42:16   SKURTZ
 * Preparations for Scorpio.  Fixed source of compiler warning messages.
 *
 *    Rev 1.6   05 Sep 1991 14:58:18   JOHN
 * Automagically convert between long and short names, finding the program
 * the user wanted even if they didn't use the right name.  PTR #NU3201.
 *
 *    Rev 1.5   01 Apr 1991 13:21:02   JOHN
 * Changed to close the main dialog before executing other programs
 * if using graphical dialogs.
 *
 *    Rev 1.4   18 Feb 1991 22:10:36   JOHN
 * Added code to skip the password check if the <skipPwdTestForVideo>
 * flag is set.  This is done for Video configuration, which can always
 * be run even if NUCONFIG is password protected.
 *
 * Added code to omit the "Loading..." message if the <skipLoadingMessage>
 * flag is set.  This is done for all calls to NUCONFIG.
 *
 *    Rev 1.3   26 Jan 1991 21:08:34   JOHN
 * Made changes related to the way passwords are patched into programs.
 * We now open each .EXE before we run it and see if it has a passsword
 * patch area.  If it does, and a password is there, will prompt the
 * user to enter it.
 *
 * Prior to these changes, the NORTON.OVL file itself was patched to
 * indicate which programs have passwords.  This patching no longer
 * takes place.
 *
 *    Rev 1.2   16 Jan 1991 13:58:36   JOHN
 * Changed call to exit() to DOSExit()
 *
 *    Rev 1.1   28 Aug 1990 19:35:12   DAVID
 * Fixed bug which caused "Press any key" message to appear on top of
 * full screen output of "UnErase *.*" (TTY_ARG_WILD test was backwards)
 *
 *    Rev 1.0   20 Aug 1990 15:19:26   DAVID
 * Initial revision.
 */

#include	<types.h>
#include	<vmm.h>
#include	<dialog.h>
#include	<main.h>
#include	<file.h>
#include	<nuconfig.h>
#include	<password.h>
#include	<nloader.h>
#include	<lint.h>

#include	"defs.h"
#include	"proto.h"

/*----------------------------------------------------------------------*
 * Local function prototypes						*
 *----------------------------------------------------------------------*/

Word	LOCAL	PrepCommand(Byte *command, Word *tty, Byte *byDisk);
void	LOCAL	ParseCommand(Byte *command, Byte *progName, Byte *commandTail);
void    LOCAL	AddStdSwitchesToCmdline(Byte *commandTail);
Boolean LOCAL	IsInternalDOSCmd(Byte *command);
Boolean LOCAL	IsExternalDOSCmd(Byte *fullProgName, Byte *progName);
Boolean		DOSSearchPath(Byte *fullName, Byte *file);
Boolean		NameIsRunFile(Byte *fullName, Byte *name);
void	LOCAL	BuildInternalCmd(Byte *command, Byte *commandTail);
void	LOCAL	BuildExternalCmd(Byte *fullProgName, Byte *commandTail);
Word	LOCAL	IsOneOfOurUtilityPrograms(Byte *fullProgName, Boolean bCheckExtension);
Word	LOCAL	UtilityIsTTY(Word exeIndex, Byte *commandTail);
Word	LOCAL	MyMainSwitchCount(void);
Boolean LOCAL	MyPasswordCheck(Word wIndex);
void	LOCAL	BadCommand(Byte *s);
void	LOCAL	NUSwitchString(Byte *s, NULoaderDataRec far *p);
Boolean LOCAL   FindFileInPath(Byte * fullPath, Byte * PathToExamine, Byte * file);

/*----------------------------------------------------------------------*
 * These buffers are used only by PrepCommand() for its parsing of	*
 * DOS commands.  (They are too big to put on the stack).		*
 *----------------------------------------------------------------------*/

static	Byte  progName    [129];
static	Byte  commandTail [129];
static	Byte  fullProgName[129];

/*----------------------------------------------------------------------*
 * Recall that the library var 'integrator' is NI's own equivalent	*
 * of SWITCH_INTEGRATOR.  If it is true, NUFastInitScreen and 		*
 * NUFastRestoreScreen will implement the smooth screen transition	*
 * between Norton programs, suppressing screen clears, etc.		*
 *----------------------------------------------------------------------*/

extern Boolean integrator;

/*----------------------------------------------------------------------*
 * This routine instructs the loader to run the specified DOS command,	*
 * and then to re-load this program.					*
 *									*
 * If any of the executable files cannot be loaded, an alert is 	*
 * displayed and the routine returns.  Otherwise, control passes to	*
 * the loader, which eventually restarts this program from the 		*
 * beginning.								*
 *----------------------------------------------------------------------*/

void Launch(Byte *cmd)
{
    extern	DIALOG_RECORD	*mainDlgHdl;
    extern	NULoaderDataRec	far *loaderData;
    extern	Boolean		skipLoadingMessage;
    extern	Boolean		_MOUSE_INSTALLED; /* library var */
    extern	DialogRec	loadingDlg;
    extern	Boolean		bClearDOSScreen;
    extern	Byte		TO_RETURN_STR[];
    extern	Boolean		commandsEdited;
    extern	TERec		mainTE;
#ifdef EMERGENCY
    extern      Byte  		szPauseMsg[];
    extern	Byte		szEmergencyExeName[];
    extern      Byte 		*askEmergencyDisk2[];
    extern      Byte 		*askEmergencyDisk3[];
    extern      Byte 		*askEmergencyDisk4[];
    extern      Byte 		*askEmergencyDiskErr[];
    auto	Boolean		bFoundOnDiskOne = TRUE;
#endif
    auto	NLoaderRec	*lpLoaderRec;
    auto	Word		row, col;
    auto	Word		i;
    auto	DIALOG_RECORD	*dr;
    auto	Word		tty;
    auto    Byte        byDisk = 1;

    StringStripSpaces(cmd);
    if (*cmd == '\0')
        return;



#ifdef EMERGENCY

   /* NOTE: for EMERGENCY configuration, skipLoadingMessage is not used.
    * Always display the "loading" message because we do not run NUCONFIG
    * in the emergency disk.
    */

//    while ((i = PrepCommand(cmd, &tty)) != 0)	// keep looping until the
//	{					// program we need is found
//	bFoundOnDiskOne = FALSE;  		// or until they give up.
//	if (!StdDlgOk(askEmergencyDisk2))
//	    return;
//	}

    while ((i = PrepCommand(cmd, &tty, &byDisk)) != 0)	// keep looping until the
	{					// program we need is found
		bFoundOnDiskOne = FALSE;  		// or until they give up.
		if ( byDisk == 2 )
		{
			if (!StdDlgOk(askEmergencyDisk2))
				return;
		} else if ( byDisk == 3 )
		{
			bClearDOSScreen = TRUE;
     	    integrator = FALSE;			/* Do normal screen clearing	*/

			if (!StdDlgOk(askEmergencyDisk3))
				return;
		}
		else if ( byDisk == 4 )
		{
			if (!StdDlgOk(askEmergencyDisk4))
				return;
		}
		else
		{
            StdDlgError(askEmergencyDiskErr);
			return;
		}

	}

    /* NOTE:  if we did not find this file on disk #2, then we should check
     * to make sure that 'STARTUP.EXE' is also here so that the loader can
     * reload it. If not found, then the person probably swapped disks BEFORE
     * the previous error message came up.
     */


    if (bFoundOnDiskOne)
	{
	bFoundOnDiskOne  = (Boolean)NameIsFile(szEmergencyExeName);
	}

    lpLoaderRec = NLoaderGetRecPtr();

    StringCopy(lpLoaderRec->szReloadFile, szEmergencyExeName);

    lpLoaderRec->wPauseBeforeReload = bFoundOnDiskOne ? NLDR_NO_PAUSE :
    							NLDR_KBD_PAUSE;

    dr = DialogOpen2(&loadingDlg);

#else

    /* Give the user feedback that something is happening. */

    if (skipLoadingMessage)
	{
	dr = NULL;
	skipLoadingMessage = FALSE;
	}
    else
	dr = DialogOpen2(&loadingDlg);

    /* Parse the DOS commands and copy them into the loader's buffer.
     * This will result in some disk activity.  */

    if ((i = PrepCommand(cmd, &tty)) != 0)
	{
	if (dr != NULL)
	    DialogClose(dr, EVENT_USED);
	if (i == 1)			/* Command not found	*/
	    BadCommand(progName);
	return;
	}


    /* Set pause flag in NLoaderRec */

    lpLoaderRec = NLoaderGetRecPtr();
    if (integrator)
	lpLoaderRec->wPauseBeforeReload = NLDR_NO_PAUSE;
    else
	lpLoaderRec->wPauseBeforeReload = _MOUSE_INSTALLED ?
						NLDR_MOUSE_PAUSE :
						NLDR_KBD_PAUSE;

#endif

    /* Save the mouse position into the loader so the launched
     * utility (if any) can restore it in NUFastInitScreen().  */

    MouseGetPosition(&row, &col);
    loaderData->mouseRow = row;
    loaderData->mouseCol = col;

    /* Save variables we need to remember in the loader's storage area */

    SaveVarsToLoader();

    /* Take down the loading dialog and close the main window */

    if (dr != NULL)
	DialogClose(dr, EVENT_USED);
    CloseMainList(mainDlgHdl);

    /* Clean up just as if we had exited
     * the main event loop */

    if (commandsEdited)
	WriteCommands();

    // Clear to DOS colors if last program left up a "Norton"
    // desktop and we're about to run a TTY program
    if (tty && loaderData->bScreenWasCleared)
	{
	loaderData->bScreenWasCleared = FALSE;
	bClearDOSScreen = TRUE;
	}

    loaderData->wLoaderDataVersion = NU7_LOADER_VERSION;
    loaderData->wChildLibraryVersion = tty ? 0x0000: 0x0001;

    FinishUp();

    // Have the loader do it's thing!
#ifdef EMERGENCY
    NLoaderExecuteCore(NULL, NULL,szPauseMsg,NULL);
#else
    NLoaderExecute(NULL, NULL);
#endif

    // In theory, we should never return from NLoaderExecute().
    // If we do come back, all we can do is exit.
    DOSExit();
}

/*----------------------------------------------------------------------*
 * This routine parses the DOS command (as the user typed it) into	*
 * a full executable file name and command tail, and copies the		*
 * parsed command into the loader's data area.				*
 *									*
 * Returns:	0	If was able to locate & prepare executable file	*
 *		1	Was not able to find file			*
 *		2	User failed to provide correct password		*
 *----------------------------------------------------------------------*/

Word LOCAL PrepCommand(Byte *command, Word *tty, Byte *byDisk)
{
    extern	Byte		BAT_EXT[];
    extern	NULoaderDataRec	far *loaderData;
    extern	NIFileInfoRec	fileInfo[NUM_EXE_FILES];
    auto	Byte		ext[4];
    auto	Byte		niSwitchStr[20];
    auto	Word		i;

    /* Assume we will not be running one of our own full screen programs */

    integrator = FALSE;

    /* Find the program name and command tail parts of the command string
     * and expand the program name into a full path. */

    ParseCommand(command, progName, commandTail);

    /*
     *	Automagically convert between long and short utility names.
     *	This is mostly for ADVISE, so it can launch the right program
     *	even if it has been renamed.  It will also help lusers who
     *	can't remember what they're calling their programs this week.
     */

    for (i = 0; i < NUM_EXE_FILES; i++)
        {
        if (StringCompare(progName, fileInfo[i].name, FALSE) == 0)
	    {
		*byDisk = fileInfo[i].byDiskNum;
	    if (IsExternalDOSCmd(fullProgName, progName))
		break;
	    if (IsExternalDOSCmd(fullProgName, fileInfo[i].alias))
		{
		StringCopy(progName, fileInfo[i].alias);
		break;
		}
	    break;
	    }
#ifndef EMERGENCY
        if (StringCompare(progName, fileInfo[i].alias, FALSE) == 0)
	    {
	    if (IsExternalDOSCmd(fullProgName, progName))
		break;
	    if (IsExternalDOSCmd(fullProgName, fileInfo[i].name))
		{
		StringCopy(progName, fileInfo[i].name);
		break;
		}
	    break;
	    }
#endif
	}

    if (IsInternalDOSCmd(progName))
	{
	BuildInternalCmd(progName, commandTail);
	}
    else if (IsExternalDOSCmd(fullProgName, progName))
        {
	/* A batch file must be run via COMMAND.COM
	 * just like internal commands.  */

	if (NameReturnExtension(fullProgName, ext) == TRUE
		&& StringCompare(ext, BAT_EXT, FALSE) == 0)
	    BuildInternalCmd(fullProgName, commandTail);

	/* A Norton utility that is not a TTY program and does
	 * not have a help request gets special treatment. */

	else if ((i = IsOneOfOurUtilityPrograms(fullProgName, TRUE)) != -1 &&
	         (*tty = UtilityIsTTY(i, commandTail)) != 1 && *tty != 2)
	    {
	    /* If the program is password protected,
	     * check the password now (unless we have already
	     * asked the user for the password). */

	    if (MyPasswordCheck(i) == FALSE)
 	        return(2);

	    /* Transfer any swiches passed to NORTON to
	     * the utility we are about to launch. */

	    AddStdSwitchesToCmdline(commandTail);

	    /* Give the program the special integator switch */

	    NUSwitchString(niSwitchStr, NLoaderGetResidentDataAddr());
	    StringAppend(commandTail, niSwitchStr);

	    /* Decide if this program must clear screen on launch */

	    if (*tty == 0)
	        integrator = TRUE;	/* Don't clear screen */

            BuildExternalCmd(fullProgName, commandTail);
	    }

        else			 	/* Non NU prog or TTY NU prog	*/
	    BuildExternalCmd(fullProgName, commandTail);
	}

    else			/* Not an internal or external command	*/
        {
	return (1);
	}

    return (0);
}

/*----------------------------------------------------------------------*
 * This routine copies the program name and command tail portions	*
 * of a command into separate buffers.					*
 *									*
 * Leading and trailing white space is removed from the program name.	*
 *									*
 * A single space is placed at the start of the command tail, replacing	*
 * any pre-existing white space.  This is what command.com does when	*
 * running programs, and external DOS commands expect it.		*
 * Trailing white space is not removed from the program tail.		*
 *									*
 * The last component of the program name is fixed up so it has 	*
 * at most 13 characters (incl. null).  However, both progName		*
 * and commandTail must point to enough storage to hold the entire	*
 * command string.							*
 *									*
 * Examples:	"CHKDSK  /F"	-->	"CHKDSK"  " /F"			*
 *		" CHKDSK/F "	-->	"CHKDSK"  " /F "     		*
 *----------------------------------------------------------------------*/

void LOCAL ParseCommand(Byte *command, Byte *progName, Byte *commandTail)
{
    auto	Byte		far *p;
    auto	Byte		*s;

    /* Move p to start of command. */

    p = command;
    while (isspace(*p))
        p++;

    /* Copy command to buffer. */

    s = progName;
    while (CharIsFile(*p) || *p == ':' || *p == '\\')
        *s++ = *p++;
    *s = '\0';

    NameFixEnd(progName);

    /* Copy tail (DOS command line arguments) to buffer. */

    while (isspace(*p))
        p++;
    commandTail[0] = ' ';
    StringLocalize(commandTail + 1, p);
}

/*----------------------------------------------------------------------*
 * Transfer any swiches passed to NORTON to the utility			*
 * program we are about to launch.					*
 *----------------------------------------------------------------------*/

void LOCAL AddStdSwitchesToCmdline(Byte *commandTail)
{
    extern	SwitchRec	SWITCH_BW;
    extern	SwitchRec	SWITCH_LCD;
    extern	SwitchRec	SWITCH_HERC;
    extern	SwitchRec	SWITCH_NO_ZOOM;
    extern	SwitchRec	SWITCH_GRAPHIC_MODE_0;
    extern	SwitchRec	SWITCH_GRAPHIC_MODE_1;
    extern	SwitchRec	SWITCH_GRAPHIC_MODE_2;
    extern	SwitchRec	SWITCH_MOUSE_MODE_0;
    extern	SwitchRec	SWITCH_MOUSE_MODE_1;

    if (MainSwitchSet(&SWITCH_BW))
        {
        StringAppendChar(commandTail, '/');
        StringAppend(commandTail, SWITCH_BW.string);
	}
    if (MainSwitchSet(&SWITCH_LCD))
        {
        StringAppendChar(commandTail, '/');
        StringAppend(commandTail, SWITCH_LCD.string);
	}
    if (MainSwitchSet(&SWITCH_HERC))
        {
        StringAppendChar(commandTail, '/');
        StringAppend(commandTail, SWITCH_HERC.string);
	}
    if (MainSwitchSet(&SWITCH_NO_ZOOM))
        {
        StringAppendChar(commandTail, '/');
        StringAppend(commandTail, SWITCH_NO_ZOOM.string);
	}
    if (MainSwitchSet(&SWITCH_GRAPHIC_MODE_0))
        {
        StringAppendChar(commandTail, '/');
        StringAppend(commandTail, SWITCH_GRAPHIC_MODE_0.string);
	}
    if (MainSwitchSet(&SWITCH_GRAPHIC_MODE_1))
        {
        StringAppendChar(commandTail, '/');
        StringAppend(commandTail, SWITCH_GRAPHIC_MODE_1.string);
	}
    if (MainSwitchSet(&SWITCH_GRAPHIC_MODE_2))
        {
        StringAppendChar(commandTail, '/');
        StringAppend(commandTail, SWITCH_GRAPHIC_MODE_2.string);
	}
    if (MainSwitchSet(&SWITCH_MOUSE_MODE_0))
        {
        StringAppendChar(commandTail, '/');
        StringAppend(commandTail, SWITCH_MOUSE_MODE_0.string);
	}
    if (MainSwitchSet(&SWITCH_MOUSE_MODE_1))
        {
        StringAppendChar(commandTail, '/');
        StringAppend(commandTail, SWITCH_MOUSE_MODE_1.string);
	}
}

/*----------------------------------------------------------------------*
 * This routine tests if a command is an internal DOS command.		*
 *----------------------------------------------------------------------*/

Boolean LOCAL IsInternalDOSCmd(Byte *command)
{
    extern	Byte*		internalDOSCmds[];
    auto	Word		i;
    auto	Byte*		s;

    if (NameIsLocal(command) && command[1] != ':')
        {
        for (i = 0; (s = internalDOSCmds[i]) != NULL; i++)
	    if (StringCompare(command, s, FALSE) == 0)
	        return (TRUE);
	}

    return (FALSE);
}

/*----------------------------------------------------------------------*
 * This routine tests if a command is an external DOS command.		*
 * It searches for the command just as DOS would, and returns the	*
 * full name of the command, including the exension COM, EXE, or BAT,	*
 * if it is found.							*
 *									*
 * Returns:	TRUE	If command found				*
 *		FALSE	Otherwise					*
 *----------------------------------------------------------------------*/

Boolean LOCAL IsExternalDOSCmd(Byte *fullProgName, Byte *progName)
{
    if (NameIsLocal(progName))		/* Has no '\'s in it		*/
	return(DOSSearchPath(fullProgName, progName));
#ifndef EMERGENCY
    else
        return(NameIsRunFile(fullProgName, progName));
#else
    return(FALSE);
#endif
}

/**************************************************************************
 * FindFileInPath
 *
 * DESCRIPTION  : Searches any path passed to it in "PathToExamine" for file
 *                in "file".
 *
 *
 * RETURN VALUES: If TRUE, returns path in "fullPath"
 *************************************************************************
 * SKURTZ : 03-04-93 at 11:00:50am Function created
 ************************************************************************* */
Boolean LOCAL FindFileInPath(Byte * fullPath, Byte * PathToExamine, Byte * file)
{
    auto	int		numPaths;
    auto	int		i;
    auto	Byte		pName[80]; /* Room for the full file name */
    auto	Byte		*ptr;


    numPaths = DirParsePath(PathToExamine); /* Separate into paths   	*/

    ptr = PathToExamine;		    /* Point to start of path	*/
    for (i = 0; i < numPaths; i++)	    /* Search through path list	*/
	{
	StringNCopy(pName, ptr, 64);	    /* Copy first 64 chars 	*/

	NameAppendFile(pName, file);	    /* Add file name to path	*/

	if (NameIsRunFile(fullPath, pName) == TRUE)
	    return (TRUE);

	ptr += StringLength(ptr) + 1;	    /* Start of next path name	*/
	}
    return (FALSE);

}

/*----------------------------------------------------------------------*
 * This routine searches the current directory and then the path 	*
 * for an executable file (COM, EXE, or BAT, tried in that order).	*
 * It returns the path where the file was found, which includes 	*
 * the file name with the found extension type.		 		*
 * Searches 1)   Search NU env variable                                 *
 *	    2)   Directory where NORTON.EXE was started from.           *
 *	    3)   Current subdirectory                                   *
 *	    4)   Dos PATH                                               *
 *									*
 * Returns:	TRUE	Found the file; fullPath set			*
 *		FALSE	Otherwise; 	fullPath trashed	  	*
 *----------------------------------------------------------------------*/

Boolean DOSSearchPath(Byte *fullPath, Byte *file)
{

#ifdef EMERGENCY
    auto	Byte		pName[14]; /* Room for the full file name */

    /* With emergency disk, full paths are not allowed.  So, if the file
     * is found, we have to use the naked filename.
     */

    if (NameIsRunFile(fullPath, file) == TRUE)
	{
	NameReturnFile(pName,fullPath);
	StringCopy(fullPath,pName);
	return (TRUE);
	}


#else
    extern	Byte		*pszProductEnvVar;
    extern	Byte		PATH_STR[];
    auto	Byte		path[256]; /* Data for PATH environ var	*/


    /* Search through the NU path */
    DOSGetEnvironment(pszProductEnvVar,path,255);

    if ((*path) &&
       FindFileInPath(fullPath,path,file))
	return (TRUE);


    /* check the NORTON.EXE location */
    DOSGetProgName(path);		/* Store program name in path	*/
    if (*path != '\0')			/* Do we have a name?		*/
	{				/* Yes, then look here		*/
	NameToFull(fullPath, path);	/* Convert to full path name	*/
	NameStripFile(fullPath);	   /* Remove name of .EXE file	*/
	if (NameIsRunFile(fullPath, file) == TRUE)
	    return (TRUE);
	}


    *fullPath = '\0';

    /* Check the current directory */
    if (NameIsRunFile(fullPath, file) == TRUE)
	return (TRUE);


    /* Search through the DOS PATH. */
    DOSGetEnvironment(PATH_STR, path, 255);  /* Read PATH from env	*/


    if (FindFileInPath(fullPath,path,file))
	return (TRUE);


#endif
    return (FALSE);
}

/*----------------------------------------------------------------------*
 * This routine expands a file name to its full path name and tests 	*
 * if the file exists as a COM, EXE, or BAT file.  If so, the full	*
 * name is returned which includes the actual extension found.		*
 *									*
 * The input filename's extension, if any, is ignored for this test.  	*
 * This is because DOS does the same thing (e.g. typing hi.xyz at the	*
 * command prompt might cause hi.exe to be run).	 		*
 *									*
 * This routine ignores errors from NameToFull() and NameIsFile()  	*
 *									*
 * Returns:	TRUE	If the name refers to a runnable file 		*
 *		FALSE	Otherwise					*
 *----------------------------------------------------------------------*/

Boolean NameIsRunFile(Byte *fullName, Byte *name)
{
    extern	Byte	COM_STR[];
    extern	Byte	EXE_STR[];
    extern	Byte	BAT_STR[];

    NameToFull(fullName, name);

    if (NameIsRoot(fullName))
	return (FALSE);

    NameStripExtension(fullName);
    StringAppend(fullName, COM_STR);	/* ".COM" */
    if (NameIsFile(fullName) == TRUE)
        return (TRUE);

    NameStripExtension(fullName);
    StringAppend(fullName, EXE_STR);	/* ".EXE" */
    if (NameIsFile(fullName) == TRUE)
        return (TRUE);

    NameStripExtension(fullName);
    StringAppend(fullName, BAT_STR);	/* ".BAT" */
    if (NameIsFile(fullName) == TRUE)
        return(TRUE);

    return (FALSE);
}

/*----------------------------------------------------------------------*
 * This routine sets up the loader for an internal DOS command		*
 *----------------------------------------------------------------------*/

void LOCAL BuildInternalCmd(Byte *command, Byte *commandTail)
{
    extern	Byte	    	commandComPath[];
    auto	Byte		szNewCmdTail[256];

    StringPrint(szNewCmdTail, " /c %s1%s2", command, commandTail);

    BuildExternalCmd(commandComPath, szNewCmdTail);
}

/*----------------------------------------------------------------------*
 * This routine sets up the loader for an external program		*
 *----------------------------------------------------------------------*/

void LOCAL BuildExternalCmd(Byte *fullProgName, Byte *commandTail)
{
    auto	NLoaderRec	*lpLoaderRec;
    auto	Word		cmdTailLength;

    cmdTailLength = StringLength(commandTail);
    if (cmdTailLength > 126)
	cmdTailLength = 126;

    commandTail[cmdTailLength] = '\r';
    commandTail[cmdTailLength+1] = 0;

    lpLoaderRec = NLoaderGetRecPtr();

    StringCopyFar(lpLoaderRec->szRunFile, fullProgName);

    StringCopyFar(lpLoaderRec->szRunCmdline+1, commandTail);
    lpLoaderRec->szRunCmdline[0] = (Byte) cmdTailLength;
}

/*----------------------------------------------------------------------*
 * This routine tests if the named program is one of the Norton 	*
 * Utilties full-screen programs.					*
 *									*
 * We use this test to decide if we need to send this program the	*
 * "you were called from NI" switch on the command line. 		*
 *									*
 * Returns:	NI file table index if program is one of ours		*
 *		-1 otherwise						*
 *----------------------------------------------------------------------*/

Word LOCAL IsOneOfOurUtilityPrograms(Byte *fullProgName, Boolean bCheckExtension)
{
    extern	NIFileInfoRec	fileInfo[NUM_EXE_FILES];
    extern	Byte		EXE_STR[];
    auto	Byte		name[13];
    auto	Byte		ext[4];
    auto	Word		i;

    NameReturnFile(name, fullProgName);
    NameReturnExtension(name, ext);
    NameStripExtension(name);

    if (bCheckExtension && StringCompare(ext, EXE_STR+1, FALSE) != 0)
        return (-1);

    for (i = 0; i < NUM_EXE_FILES; i++)
        {
        if (StringCompare(name, fileInfo[i].name, FALSE) == 0)
	    return (i);
        if (StringCompare(name, fileInfo[i].alias, FALSE) == 0)
	    return (i);
	}

    return (-1);
}

/*----------------------------------------------------------------------*
 * This routine tests if the specified Norton Utility program will run	*
 * in TTY mode if EXEC-ed with the given DOS command line.		*
 *									*
 * Returns:	0	If will run in full screen mode			*
 *		1	TTY output because is a TTY only program	*
 *		2	TTY output due to help request	("?" arg)	*
 *		3	TTY output due to switches/args			*
 *----------------------------------------------------------------------*/

Word LOCAL UtilityIsTTY(Word exeIndex, Byte *commandTail)
{
    extern	NIFileInfoRec	fileInfo [NUM_EXE_FILES];
    extern	SwitchRec	SWITCH_HELP;
    extern	Byte		line[];
    auto	Word		i;
    auto	SwitchRec*	sw;
    auto	Word		tty;
    auto	int		helpRequest;

    tty = 0;				/* Assume will be full screen	*/

    /* Switch over to the command line of the program being launched.
     * We must switch back to our command line (that is, the loader's
     * command line) before returning. */

    StringCopy(line, commandTail);	/* Don't destroy commandTail	*/
    MainProcessCmdLine(line);		/* Parse switches and args	*/

    if (fileInfo[exeIndex].tty)		/* A TTY prog by nature	*/
        tty = 1;

    /* If the program contains the standard Norton help request
     * switch, then it will produce TTY ouput.
     * This is the identical test used by MainCheckHelp() */

    helpRequest = MainSwitchSet(&SWITCH_HELP) ||
		  (MainParamCount() == 1 && StringCmp(MainParamGet(0), "?") == 0);
    if (!tty && helpRequest)
        tty = 2;

    /* Now check our EXE info table to predict
     * if the program will produce TTY output. */

    if (!tty && fileInfo[exeIndex].ttyArgs == TTY_ARG_ANY && MainParamCount() > 0)
        tty = 3;

    if (!tty && fileInfo[exeIndex].ttyArgs == TTY_ARG_WILD && MainParamCount() > 0)
        {
	if (!NameIsWildcard(MainParamGet(0)) && !NameIsDirectory(MainParamGet(0)))
            tty = 3;
	}

    if (!tty && fileInfo[exeIndex].ttySwitches == TTY_SWITCH_ANY && MyMainSwitchCount() > 0)
        tty = 3;

    if (!tty && fileInfo[exeIndex].ttySwitches == TTY_SWITCH_LISTED)
        {
	for (i = 0; i < MAX_SWITCHES; i++)
	    {
	    if ((sw = fileInfo[exeIndex].switches[i]) == NULL)
	        continue;
  	    if (MainSwitchSet(sw) != FALSE)
	        {
		tty = 3;
		break;
		}
	    }
	}

    /* Switch back to our original command line,
     * so we can test and pass on our switches if needed. */

    ProcessLauncherCmdLine();

    return (tty);
}

/*----------------------------------------------------------------------*
 * This routine returns the number of switches on the command line,	*
 * not counting any generic Utilities switches.				*
 *----------------------------------------------------------------------*/

Word LOCAL MyMainSwitchCount(void)
{
    extern	SwitchRec	SWITCH_BW;
    extern	SwitchRec	SWITCH_LCD;
    extern	SwitchRec	SWITCH_HERC;
    extern	SwitchRec	SWITCH_NO_ZOOM;
    extern	SwitchRec	SWITCH_GRAPHIC_MODE_0;
    extern	SwitchRec	SWITCH_GRAPHIC_MODE_1;
    extern	SwitchRec	SWITCH_GRAPHIC_MODE_2;
    extern	SwitchRec	SWITCH_MOUSE_MODE_0;
    extern	SwitchRec	SWITCH_MOUSE_MODE_1;
    auto	Word		count;

    count = MainSwitchCount();

    if (MainSwitchSet(&SWITCH_BW) && count > 0)
        count--;
    if (MainSwitchSet(&SWITCH_LCD) && count > 0)
        count--;
    if (MainSwitchSet(&SWITCH_HERC) && count > 0)
        count--;
    if (MainSwitchSet(&SWITCH_NO_ZOOM) && count > 0)
        count--;
    if (MainSwitchSet(&SWITCH_GRAPHIC_MODE_0) && count > 0)
        count--;
    if (MainSwitchSet(&SWITCH_GRAPHIC_MODE_1) && count > 0)
        count--;
    if (MainSwitchSet(&SWITCH_GRAPHIC_MODE_2) && count > 0)
        count--;
    if (MainSwitchSet(&SWITCH_MOUSE_MODE_0) && count > 0)
        count--;
    if (MainSwitchSet(&SWITCH_MOUSE_MODE_1) && count > 0)
        count--;
    return (count);
}

/*----------------------------------------------------------------------*
 * This routine performs a password security check on behalf of the 	*
 * Norton Utilities program being prepared for launch.			*
 * 									*
 * It tests if this program requires a password.			*
 * If so, it prompts the user and reads in his/her password attempt.	*
 * If it is the wrong password, it prints a failure message.		*
 *									*
 * A flag is set in the loader's shared data area which is checked	*
 * by a utility when it calls PasswordCheck.  If this flag is true,	*
 * it means this program thinks the utility has a password and has	*
 * already received it.  If it is false, the utility will ask for the	*
 * password itself if it needs too, if for some reason this program	*
 * didn't get word of the password requirement.				*
 *									*
 * Returns:	TRUE	Approved for launch				*
 *		FALSE	If user failed the password test; don't launch	*
 *----------------------------------------------------------------------*/

Boolean LOCAL MyPasswordCheck(Word wIndex)
{
    extern      PasswordPatchRec        far *lpPwdPatchArea;
    extern      NIFileInfoRec           fileInfo[NUM_EXE_FILES];
    extern	Boolean			passwordOK;
    extern	Boolean			skipPwdTestForVideo;
    extern	DialogRec		myBadPasswordDlg;
    extern	NULoaderDataRec far*	loaderData;
    auto	Byte			userPassword[PASSWORD_LEN+1];

    /* Don't ask if program can't have a password (e.g. FA) */
    if (fileInfo[wIndex].byPasswordID == 0)
	return(TRUE);

    /* Skip password test if calling NUCONFIG to set video options */
    if (skipPwdTestForVideo)
	{
	skipPwdTestForVideo = FALSE;		// Reset for next time
	loaderData->skipPassword = TRUE;
	return(TRUE);
	}

    /* We already got a correct password, so don't ask again */
    if (passwordOK)
        {
        loaderData->skipPassword = TRUE;
        return (TRUE);
	}

    /* Is any password set? */
    if (PasswordMatch(lpPwdPatchArea->szPassword, PASSWORD_NONE) == TRUE)
	{
	/* No program have passwords */
	return(TRUE);
	}

    /* Is password set on this program? */
    if (StringScan(lpPwdPatchArea->szProtectedPrograms,
                   fileInfo[wIndex].byPasswordID) == NULL)
        {
        /* This program does not have a password */
        return(TRUE);
        }

    /* Assume the program will have to ask for a password if needed. */
    loaderData->skipPassword = FALSE;

    /* Prompt for the password.
     * If it's not correct, don't launch the program. */

    if (PasswordGet(userPassword) == FALSE)
	{
        return(FALSE);
	}
    PasswordEncrypt(userPassword);
    if (PasswordMatch(lpPwdPatchArea->szPassword, userPassword) == FALSE)
        {
	DialogAlertBox(&myBadPasswordDlg);
        return(FALSE);
	}

    /*
     *	We got the right password
     */

    loaderData->skipPassword = TRUE;
    passwordOK = TRUE;
    return (TRUE);
}

/*----------------------------------------------------------------------*
 * This routine finds the full path name of the command processor.	*
 *									*
 * Returns:	TRUE	If path found					*
 *		FALSE	Otherwise (assumes current directory)		*
 *----------------------------------------------------------------------*/

Boolean InitCommandComPath(Byte *commandComPath)
{
    extern	Byte	COMSPEC[];
    extern	Byte	COMMAND_COM[];

    DOSGetEnvironment(COMSPEC, commandComPath, 80);
    if (*commandComPath && NameIsFile(commandComPath))
	return(TRUE);

    if (DirSearchPath(commandComPath, COMMAND_COM))
        {
        NameAppendFile(commandComPath, COMMAND_COM);
	return(TRUE);
	}

    StringCopy(commandComPath, COMMAND_COM);
    return(FALSE);
}

/*----------------------------------------------------------------------*
 * This routine displays the "Unable to locate command" dialog.		*
 *----------------------------------------------------------------------*/

void LOCAL BadCommand(Byte *s)
{
    extern	DialogRec	badCmdDlg;
    extern	ButtonsRec	okButton;

    DialogAlertBox(&badCmdDlg, s, &okButton);
}

/*----------------------------------------------------------------------*
 * This routine builds the integrator switch string.			*
 *----------------------------------------------------------------------*/

void LOCAL NUSwitchString(Byte *s, NULoaderDataRec far *p)
{
    extern	SwitchRec	SWITCH_INTEGRATOR;
    auto	Byte		segStr[8];
    auto	Byte		offStr[8];

    _ConvertWordToString(FP_SEG(p), segStr, 10, 0);
    _ConvertWordToString(FP_OFF(p), offStr, 10, 0);

    FastStringPrint(s, " /%s1:%s2:%s3 ",
    			SWITCH_INTEGRATOR.string, segStr, offStr);
}
