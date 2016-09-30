/* $Header:   S:/SRC/NORTON/VCS/CMDIO.C_V   1.5   03 Nov 1992 10:40:22   SKURTZ  $ */

/*----------------------------------------------------------------------*
 * CMDIO.C								*
 *									*
 * This file contains the command file I/O routines.			*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/SRC/NORTON/VCS/CMDIO.C_V  $
 * 
 *    Rev 1.5   03 Nov 1992 10:40:22   SKURTZ
 * Added pragma optimize because a routine is too beig for optimization
 *
 *    Rev 1.4   17 Oct 1992 02:11:38   JOHN
 * General cleanup and update for New Loader
 *
 *    Rev 1.3   25 Sep 1992 14:05:16   SKURTZ
 * Conversion to LARGE memory model
 *
 *    Rev 1.2   24 Jul 1992 17:54:20   SKURTZ
 * Preparations for Scorpio. Fixed source of compiler warning messages.
 *
 *    Rev 1.1   26 Jan 1991 20:44:20   JOHN
 * Removed obsolete code.  The NI patch area no longer has a record
 * of which utility programs are installed and/or have passwords.
 *
 *    Rev 1.0   20 Aug 1990 15:19:24   DAVID
 * Initial revision.
 */

#include	<types.h>
#include	<dialog.h>
#include	<keys.h>
#include	<file.h>
#include	<nuconfig.h>
#include	<main.h>
#include	<password.h>
#include	<lint.h>

#include	"defs.h"
#include	"proto.h"

/*----------------------------------------------------------------------*
 * Local function prototypes						*
 *----------------------------------------------------------------------*/

Word LOCAL PASCAL ReadCmds(Word *n);
Word LOCAL PASCAL ReadCmd(CmdInfoRec far *r);
Word LOCAL PASCAL WriteCmds(Word n);
Word LOCAL PASCAL WriteCmd(CmdInfoRec far *r);
Word LOCAL PASCAL ReadLine(Byte near *buffer, Byte far *line, Word max);
Word LOCAL PASCAL WriteLine(register Byte near *buffer, register Byte far *string);
Boolean LOCAL GetCommandFile(Byte *s);

/*----------------------------------------------------------------------*
 * This routine reads the command file and posts alerts for errors.	*
 *									*
 * On the first load, we construct the full name of the command file.	*
 * We save this in the loader so we are sure to use the same name	*
 * on subsequent loads, even if we change directories (it also saves	*
 * us some search time).						*
 *----------------------------------------------------------------------*/

void ReadCommands(Boolean firstLoad)
{
    extern	Byte		near commandFile[];
    extern	Byte		*cantFindFileStrings[];
    extern	Byte		*cantOpenFileStrings[];
    extern	Byte		*corruptedFileStrings[];
    extern	DialogRec	fileErrorDlg;
    extern	DialogRec	noMemDlg;
    extern	Boolean		sortByName;
    extern	Word		totalListItems;
    extern	ListRec		mainList;
    auto	Ulong		i;
    auto	Byte		s[81];

    if (firstLoad && GetCommandFile(s) == FALSE)
	{
	fileErrorDlg.strings = cantFindFileStrings;
	DialogAlertBox(&fileErrorDlg, s);
	totalListItems = 0;
	}
    else if ((i = ReadCmds(&totalListItems)) != 0 && firstLoad)
        {
	if (i == 2)
	    {
	    fileErrorDlg.strings = cantOpenFileStrings;
	    DialogAlertBox(&fileErrorDlg, commandFile);
	    }
	else if (i == 4 || i == 5)
	    {
	    fileErrorDlg.strings = corruptedFileStrings;
	    DialogAlertBox(&fileErrorDlg, commandFile);
	    }
	else if (i == 6)
	    DialogAlertBox(&noMemDlg);
	}

    SortCommands(sortByName, totalListItems);
    MoveOffSkipItem();
    InitTopicRadios();
}

/*----------------------------------------------------------------------*
 * This routine reads the command file.					*
 *									*
 * Returns:	0	If all went well				*
 *		2	If error opening command file			*
 *		3	If critical error opening command file		*
 *	    	4	If premature EOF reading command file  NOT USED	*
 *	    	5	If error in command file format/values NOT USED	*
 *		6	If out of memory 				*
 *									*
 * "Too many commands" and "Too many topics" errors are not reported	*
 *----------------------------------------------------------------------*/

BlankRec blanks[MAX_TOPICS];

#define	NO_TOPIC (0xFF)

#pragma optimize ("leg",off)
Word LOCAL PASCAL ReadCmds(Word *n)
{
    extern	Byte		COMMAND_TOKEN[];
    extern	Byte		TOPIC_TOKEN[];
    extern	Byte		DOS_CMD_TOKEN[];
    extern	Byte		near commandFile[];
    extern	Byte		near fileBuf[];
    extern	Byte   		near line[];
    extern	CmdInfoRec	far *commands[];
    extern	Boolean		readOnlyCmdFile;
    auto	Word		handle;
    auto	CmdInfoRec	far *newCmd;
    auto	Word		retValue = 0;
    auto	Word		itemType;
    auto	Word		itemLines;
    auto	Byte		topicNum;
    auto	Word		cmds;
    auto	Byte		*s;

    readOnlyCmdFile = FALSE;

    if ((handle = FileOpen(commandFile, READ_ONLY_FILE)) == ERROR)
	{
	if (error < 256)
	    return (2);
	return (3);
	}
    FileOpenBuffer(handle, fileBuf, FILE_BUF_SIZE);

    if ((FileGetAttr(commandFile) & FA_READ_ONLY) != 0)
        readOnlyCmdFile = TRUE;

    cmds = 0;
    topicNum = NO_TOPIC;
    itemType = TYPE_NONE;
    itemLines = 0;

    while (TRUE)
        {
        if (ReadLine(fileBuf, line, MAX_FILE_LINE) == EOF)
	    break;

        if (line[0] == '!' && line[1] != '!')
	    {
	    /* 'line' is of the form:  "!command: text1 text2".
	     * Set 's' to point the text after the command word.
	     * Replace the first ':' with a null so 'line' can be
	     * used to test for the command word. */

	    for (s = line; *s != '\0'; s++)
	        {
	        if (*s == ':')
		    {
		    *s++ = '\0';
		    while (*s != '\0' && (*s == ' ' || *s == '\t'))
		        s++;
		    break;
		    }
	        }

	    if (StringCompare(line, COMMAND_TOKEN, FALSE) == 0)
	        {
		if (itemType != TYPE_NONE)	/* Finish off old cmd	*/
		    {
		    if (cmds == MAX_COMMANDS - 1)
		        break;
		    else
		        cmds += 1;
		    }

	        if ((newCmd = AllocCmdBuf()) == NULL)
	            {
	            retValue = 6;
	            break;
	            }

		commands[cmds] = newCmd;
		ClearCommand(newCmd);
		itemType = newCmd->type = TYPE_COMMAND;
		if (topicNum == NO_TOPIC)
		    newCmd->topic = 0;
		else
		    newCmd->topic = topicNum;
		StringNCopyFar(newCmd->name, s, COMMAND_WIDTH);
		itemLines = 0;
		}

	    else if (StringCompare(line, TOPIC_TOKEN, FALSE) == 0)
	        {
		if (itemType != TYPE_NONE)	/* Finish off old cmd	*/
		    {
		    if (cmds == MAX_COMMANDS - 2)
		        break;
		    else
		        cmds += 1;
		    }

	        if ((newCmd = AllocCmdBuf()) == NULL)
	            {
	            retValue = 6;
	            break;
	            }

		if (topicNum == NO_TOPIC)
		    topicNum = 0;
		else if (++topicNum == MAX_TOPICS)
		    break;

		/* First insert a blank into the list */

		commands[cmds] = (CmdInfoRec far*) &blanks[topicNum];
		blanks[topicNum].type = TYPE_BLANK;
		blanks[topicNum].topic = topicNum;
		cmds++;

		/* Now insert this topic */

		commands[cmds] = newCmd;
		ClearCommand(newCmd);
		itemType = newCmd->type = TYPE_TOPIC;
		newCmd->topic = topicNum;
		StringNCopyFar(newCmd->name, s, TOPIC_WIDTH);
		itemLines = 0;
		}

	    else if (StringCompare(line, DOS_CMD_TOKEN, FALSE) == 0)
	        {
		if (itemType != TYPE_NONE && itemType != TYPE_TOPIC)
		    StringNCopyFar(newCmd->dosCmds[0], s, DOS_CMD_WIDTH);
		}
	    }

	else if (itemType != TYPE_NONE)	/* Line of description text	*/
	    {
	    if (line[0] == '!')		/* Convert !! to !		*/
	        s = line + 1;
	    else
	        s = line;
 	    ConvertDescrStrToCharAttr(newCmd->descr[itemLines], s, DESCR_WIDTH);
	    if (++itemLines == DESCR_LINES)
	        {
		if (cmds == MAX_COMMANDS - 2)
		    break;
		else
		    cmds += 1;
		itemType = TYPE_NONE;	/* Too many lines; ignore rest	*/
		itemLines = 0;
		}
	    }
	}

    if (itemType != TYPE_NONE)		/* Finish off old cmd	*/
	cmds += 1;

    FileClose(handle);
    *n = cmds;
    return (retValue == EOF ? 0 : retValue);
}
#pragma optimize ("",on)

/*----------------------------------------------------------------------*
 * This routine writes the command file and posts alerts for errors.	*
 *----------------------------------------------------------------------*/

void WriteCommands(void)
{
    extern	Byte	 	near commandFile[];
    extern	Word		totalListItems;
    extern	Byte		*cantOpenFileStrings[];
    extern	Byte		*cantWriteFileStrings[];
    extern	DialogRec	fileErrorDlg;
    auto	Word		i;

    SortCommands(FALSE, totalListItems);	/* Sort by Topics	*/
    if ((i = WriteCmds(totalListItems)) != 0)
        {
	if (i == 1)
	    {
	    fileErrorDlg.strings = cantOpenFileStrings;
	    DialogAlertBox(&fileErrorDlg, commandFile);
	    }
	else if (i == 3)
	    {
	    fileErrorDlg.strings = cantWriteFileStrings;
	    DialogAlertBox(&fileErrorDlg, commandFile);
	    }
	}
}

/*----------------------------------------------------------------------*
 * This routine writes the command file.				*
 *									*
 * Returns:	0	If all went well				*
 *		1	If error opening command file 			*
 *		2	If critical error opening command file		*
 *		3	If error writing command file			*
 *----------------------------------------------------------------------*/

Word LOCAL PASCAL WriteCmds(Word n)
{
    extern	Byte		near commandFile[];
    extern	Byte		near fileBuf[];
    extern	CmdInfoRec	far *commands[];
    auto	Word		handle;
    auto	Word		i;
    auto	Word		retValue = 0;

    if ((handle = FileCreate(commandFile, 0)) == ERROR)
	{
	if (error < 256)
	    return (1);
	return (2);
	}

    FileOpenBuffer(handle, fileBuf, FILE_BUF_SIZE);

    retValue = 0;

    for (i = 0; i < n; i++)
	if ((retValue = WriteCmd(commands[i])) != 0)
	    break;

    FileFlush(fileBuf);

    FileClose(handle);
    return(retValue);
}

/*----------------------------------------------------------------------*
 * This routine writes the next command to the open command file	*
 *									*
 * Returns:	0	If all went well				*
 *		3	If error writing command file			*
 *----------------------------------------------------------------------*/

Word LOCAL PASCAL WriteCmd(CmdInfoRec far* r)
{
    extern	Byte		near line[];
    extern	Byte		near fileBuf[];
    static	Byte		COLON_STR[] = ": ";
    extern	Byte		COMMAND_TOKEN[];
    extern	Byte		TOPIC_TOKEN[];
    extern	Byte		DOS_CMD_TOKEN[];
    auto	signed int	k;
    auto	signed int 	maxK;

    if (r->type == TYPE_COMMAND)
        {
	StringCopy(line, COMMAND_TOKEN);
	StringAppend(line, COLON_STR);
	StringAppendFar(line, r->name);
	}
    else if (r->type == TYPE_TOPIC)
        {
	StringCopy(line, TOPIC_TOKEN);
	StringAppend(line, COLON_STR);
	StringAppendFar(line, r->name);
	}
    else
        return (0);

    if (WriteLine(fileBuf, line) != 0)
	return (3);

    if (r->type == TYPE_COMMAND)
        {
	StringCopy(line, DOS_CMD_TOKEN);
	StringAppend(line, COLON_STR);
	StringAppendFar(line, r->dosCmds[0]);
        if (WriteLine(fileBuf, line) != 0)
            return (3);
	}

    /* Trim any blank lines from the end of the description */

    for (maxK = DESCR_LINES - 1; maxK >= 0; maxK--)
        {
 	ConvertCharAttrToDescrStr(line, r->descr[maxK], 3 * DESCR_WIDTH);
	StringStripSpaces(line);
	if (StringLength(line) != 0)
	    break;
	}

    for (k = 0; k <= maxK; k++)
	{
 	ConvertCharAttrToDescrStr(line, r->descr[k], 3 * DESCR_WIDTH);
	StringStripTrailingSpaces(line);
        if (WriteLine(fileBuf, line) != 0)
	    return (3);
	}

    if (WriteLine(fileBuf, "") != 0)
       return (3);

    return (0);
}

/*----------------------------------------------------------------------*
 * This routine reads a single CR/LF terminated line from the file	*
 * associated with 'buffer'.						*
 *									*
 * The bytes read (excluding the CR/LF) are stored in 'line' as a null-	*
 * terminated string.  'line' should point to 'max' bytes of storage.	*
 * 									*
 * If the line in the file has more than 'max' - 1 characters, only the	*
 * first 'max' - 1 are stored (followed by a NULL).  The rest are	*
 * discarded (and will NOT be picked up by the next ReadLine() call).   *
 *									*
 * Read errors are treated like an end of file (should probably fix	*
 * this)								*
 *									*
 * Returns:	0	If no error					*
 *		EOF	If at end of file				*
 *----------------------------------------------------------------------*/

Word LOCAL PASCAL ReadLine(Byte near *buffer, Byte far *line, Word max)
{
    auto	Byte		far *endLine = line + max - 1;
    register	Byte		far *d;
    register	Word		c;

    d = MemoryNormalizeAddress(line);

    while (d < endLine  &&
           (c = FileGetC(buffer)) != EOF  &&  c != EOF_Z  &&  c != CR)
	*d++ = (Byte) c; 		/* Copy one char across		*/

    *d = '\0';     			/* Mark end of line		*/

    if (d == endLine)
        while ((c = FileGetC(buffer)) != EOF  &&  c != EOF_Z  &&  c != CR)
            ;

    if (c == CR)			/* Is this end of a line?	*/
	FileGetC(buffer);		/* Yes, skip LF at end of line	*/

    if (c == EOF_Z)			/* Control Z EOF?		*/
	c = EOF;			/* Yes, convert to regular EOF	*/

    if (c == EOF && *line == '\0')	/* Don't return EOF if got data	*/
	return (EOF);
    else
	return (0);
}

/*----------------------------------------------------------------------*
 * This routine writes the specified string to the output file 		*
 * associated with 'buffer', following it with a CR/LF pair.		*
 *									*
 * Returns:	0	if no error					*
 *		1	if error writing file				*
 *----------------------------------------------------------------------*/

Word LOCAL PASCAL WriteLine(register Byte near *buffer, register Byte far *string)
{
    string = MemoryNormalizeAddress(string);

    while (*string != '\0')
	if (FilePutC(*string++, buffer) != 0)
	    return(1);

    if (FilePutC('\r', buffer) != 0 || FilePutC('\n', buffer) != 0)
	return(1);

    return(0);
}

/*----------------------------------------------------------------------*
 * This routine builds the full path name of the command file and	*
 * stores it in commandFile[].						*
 *									*
 * Returns:	TRUE	If command file was found			*
 *		FALSE	If just build default name for new file		*
 *			's' points to missing name to report		*
 *----------------------------------------------------------------------*/

Boolean LOCAL GetCommandFile(Byte *s)
{
    extern	Byte		near commandFile[];
    extern	Byte		CMD_FILE[];
    extern	Byte		*pszProductEnvVar;
    extern	Byte		*pszProductFileName;
    auto	Byte		ext[4];

    /* If a filename was specified on the command line,
     * try to locate that file.  */

    if (MainParamCount() >= 1)
        {
	StringNCopy(s, MainParamGet(0), 80);
	StringToUpper(s);

	/* If no extension, add the extension of the default file */

	if (NameReturnExtension(s, ext) == FALSE &&
	    NameReturnExtension(CMD_FILE, ext) == TRUE)
	    {
	    StringAppendChar(s, '.');
	    StringAppend(s, ext);
	    }

	if (NameIsLocal(s) && s[1] != ':')
	    {
	    if (DirSearchNortonPath(pszProductEnvVar, s, commandFile) == TRUE)
	        return (TRUE);
	    }
        else
	    {
	    NameToFull(commandFile, s);
            if (NameIsFile(commandFile))
	       return (TRUE);
	    }

	/* The named file was not found.
	 * We will report an error and bring up an empty menu.
	 * Any changes will be saved to the new file. */

	NameToFull(commandFile, s);
	return (FALSE);
	}

    /* If there was no filename on the command line,
     * try to find the default command file */

    else
        {
        if (DirSearchNortonPath(pszProductEnvVar, CMD_FILE, commandFile) == TRUE)
            return(TRUE);
        DirNewNortonPath(pszProductFileName, s, commandFile, CMD_FILE);
	StringCopy(s, CMD_FILE);
        return(FALSE);
        }
}