/* $Header:   S:/SRC/NORTON/VCS/OPS.C_V   1.9   22 Apr 1993 12:39:06   KEVIN  $ */

/*----------------------------------------------------------------------*
 * OPS.C								*
 *									*
 * This file contains routines for implementing the add, edit, move	*
 * and delete operations.						*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/SRC/NORTON/VCS/OPS.C_V  $
 * 
 *    Rev 1.9   22 Apr 1993 12:39:06   KEVIN
 * handle alt-hotkeys in add-topic
 * 
 *    Rev 1.8   04 Jan 1993 18:00:02   SKURTZ
 * Updated names of some DEFINEs that are created by the help system
 *
 *    Rev 1.7   16 Nov 1992 18:41:58   SKURTZ
 * Inserted #ifdefs for making EMERGENCY boot disk for DOS 6
 *
 *    Rev 1.6   17 Oct 1992 02:11:44   JOHN
 * General cleanup and update for New Loader
 *
 *    Rev 1.5   24 Jul 1992 17:38:40   SKURTZ
 * Preparation for Scorpio.  Adjustments to use Passport Libraries. Fixed
 * source of compiler warning messages.
 *
 *    Rev 1.4   24 Mar 1991 09:58:54   ED
 * No change.
 *
 *    Rev 1.3   23 Mar 1991 15:51:46   JOHN
 * Removed obsolete code that was already inside an "#if 0"
 *
 *    Rev 1.2   07 Feb 1991 14:55:12   ED
 * Modified for new configuration menu stuff
 *
 *    Rev 1.1   05 Sep 1990 15:24:18   DAVID
 * Cleaned up screen re-initialization sequence in DoOptions().
 *
 *    Rev 1.0   20 Aug 1990 15:19:38   DAVID
 * Initial revision.
 */

#include	<types.h>
#include	<dialog.h>
#include	<pulldown.h>
#include	<editbox.h>
#include	<main.h>
#include	<nuconfig.h>
#include	<hyper.h>
#include	<lint.h>

#include	"nort-hlp.h"
#include	"defs.h"
#include	"menu.h"
#include	"proto.h"

/*----------------------------------------------------------------------*
 * Local function prototypes						*
 *----------------------------------------------------------------------*/

Boolean LOCAL EditCommand(CmdInfoRec *info, Boolean add);
Boolean LOCAL EditTopic(CmdInfoRec *info, Boolean add);
void    LOCAL EditCommandDescr(CmdInfoRec *info, Boolean add);
Word	      EditCommandDescrFilter(DIALOG_RECORD *dr, Word *c);
void	LOCAL ShowEditColor(void);
void	LOCAL CopyDescrToBuffer(CmdInfoRec *info);
void	LOCAL CopyBufferToDescr(CmdInfoRec *info);
void	LOCAL MyListInsertEntry(ListRec *list, void far *newEntry);
void	LOCAL MyListDeleteEntry(ListRec *list, Ulong i);

/*----------------------------------------------------------------------*
 * This procedure is called just before the user pulls down a menu.	*
 * It adjusts which menu items are enabled and disabled based on the	*
 * current list entry.							*
 *----------------------------------------------------------------------*/

void MenuDisableItems(void)
{
#ifndef EMERGENCY
    extern	MenuRec		menuList[];
    extern	ListRec		mainList;
    extern	MenuBarRec	menuBar;
    extern	CmdInfoRec	far *commands[MAX_COMMANDS];
    extern	Boolean		sortByName;
    extern	Boolean		readOnlyCmdFile;
    extern	Word		totalListItems;
    auto	Boolean		nonEmptyList;
    auto	Boolean		nonFullList;
    auto	Boolean		canDelete;
    auto	Byte		type;
    auto	Byte		topic;
    auto	Byte		nextTopic;

    nonEmptyList = (Boolean) (mainList.num > 0);
    nonFullList  = (Boolean) (totalListItems < MAX_COMMANDS);

    type  = commands[mainList.active_entry]->type;
    topic = commands[mainList.active_entry]->topic;

    canDelete = (Boolean) (nonEmptyList && type != TYPE_BLANK);

    /* Do not allow a topic to be deleted
     * unless their are no commands within that topic. */

    if (type == TYPE_TOPIC && mainList.active_entry + 1 < mainList.num)
        {
        nextTopic = commands[mainList.active_entry + 1]->topic;
	if (nextTopic == topic)
	    canDelete = FALSE;
	}

    /* enable */
    PulldownSetItemState(&menuBar, ADD_CMD,
    				   (Boolean) !(!readOnlyCmdFile && nonFullList),
			           UNCHANGED);
    PulldownSetItemState(&menuBar, EDIT_CMD,
    				   (Boolean) !(!readOnlyCmdFile && nonEmptyList),
				   UNCHANGED);
    PulldownSetItemState(&menuBar, DELETE_CMD,
    				   (Boolean) !(!readOnlyCmdFile && canDelete),
				   UNCHANGED);

    /* turn on check mark */
    PulldownSetItemState(&menuBar, SORT_BY_NAME,  UNCHANGED,(Boolean) sortByName);
    PulldownSetItemState(&menuBar, SORT_BY_TOPIC, UNCHANGED,(Boolean) !sortByName);
#endif
}

/*----------------------------------------------------------------------*
 * This routine implements the "add menu item" command			*
 *----------------------------------------------------------------------*/

void DoAddCmd(void)
{
#ifndef EMERGENCY
    extern	CmdInfoRec		tempCommandInfo;
    extern	Boolean			commandsEdited;
    extern	Word			totalListItems;
    extern	ListRec			mainList;
    extern	DialogRec		noMemDlg;
    extern	ButtonsRec		okButton;
    extern	Boolean			sortByName;
    extern	DialogRec		addWhatDlg;
    extern	ButtonsRec		addWhatButtons;
    extern	DialogRec		tooManyTopicsDlg;
    extern	ButtonsRec		okButton;
    extern	CmdInfoRec far*		commands[MAX_COMMANDS];
    extern	BlankRec		blanks [MAX_TOPICS];
    auto	CmdInfoRec far*		cmdInfo;
    auto	Byte			type;
    auto	Word			i;
    auto	Boolean			result;
    auto	Word			topicNum;

    cmdInfo = AllocCmdBuf();
    if (cmdInfo == NULL)
        {
	DialogAlertBox(&noMemDlg, &okButton);
	return;
	}

    ClearCommand(&tempCommandInfo);

    if (sortByName == TRUE)
        type = TYPE_COMMAND;
    else
        {
	i = DialogBox2(&addWhatDlg, &addWhatButtons);
	if (i == 0)
	    type = TYPE_COMMAND;
	else if (i == 1)
	    type = TYPE_TOPIC;
	else
	    {
            FreeCmdBuf(cmdInfo);
	    return;
	    }
	}

    if (type == TYPE_TOPIC && CountTopics() == MAX_TOPICS)
	{
	DialogAlertBox(&tooManyTopicsDlg, MAX_TOPICS, &okButton);
	FreeCmdBuf(cmdInfo);
	return;
	}

    tempCommandInfo.type = type;

    if (type == TYPE_COMMAND)
        result = EditCommand(&tempCommandInfo, TRUE);
    else
        result = EditTopic(&tempCommandInfo, TRUE);

    if (result == TRUE)
        {
	*cmdInfo = tempCommandInfo;
	MyListInsertEntry(&mainList, &cmdInfo);

	if (type == TYPE_TOPIC)
	    {
	    topicNum = CountTopics();
	    commands[totalListItems] = (CmdInfoRec far*) &blanks[topicNum];
	    blanks[topicNum].type = TYPE_BLANK;
	    blanks[topicNum].topic = cmdInfo->topic;
	    totalListItems += 1;
	    mainList.num += 1;
	    }

	DoSortCommands(sortByName);
	commandsEdited = TRUE;
	}
    else
        FreeCmdBuf(cmdInfo);
#endif
}

/*----------------------------------------------------------------------*
 * This routine implements the "edit menu item" command			*
 *----------------------------------------------------------------------*/

void DoEditCmd(void)
{
#ifndef EMERGENCY
    extern	Boolean			sortByName;
    extern	ListRec			mainList;
    extern	CmdInfoRec		tempCommandInfo;
    extern	Boolean			commandsEdited;
    extern	Boolean			sortByName;
    auto	CmdInfoRec far*		cmdInfo;
    auto	Boolean			result;

    cmdInfo = GetCurrentCommand();
    tempCommandInfo = *cmdInfo;
    if (tempCommandInfo.type == TYPE_COMMAND)
        result = EditCommand(&tempCommandInfo, FALSE);
    else
        result = EditTopic(&tempCommandInfo, FALSE);

    if (result == TRUE)
        {
	*cmdInfo = tempCommandInfo;
	DoSortCommands(sortByName);
	commandsEdited = TRUE;
	}
#endif
}

/*----------------------------------------------------------------------*
 * This routine implements the "delete menu item" command		*
 *----------------------------------------------------------------------*/

void DoDeleteCmd(void)
{
#ifndef EMERGENCY
    extern	ListRec			mainList;
    extern	DialogRec		deleteConfirmDlg;
    extern	ButtonsRec		deleteConfirmButtons;
    extern	Boolean			commandsEdited;
    extern	CmdInfoRec far*		commands[MAX_COMMANDS];
    auto	CmdInfoRec far*		cmdInfo;
    auto	Byte			name[COMMAND_WIDTH + 1];
    auto	Ulong			i;

    cmdInfo = GetCurrentCommand();

    if (cmdInfo == NULL)		/* Should never happen		*/
        return;

    deleteConfirmButtons.value = 1;	/* Make No the default		*/
    StringLocalize(name, cmdInfo->name);

    if (DialogBox2(&deleteConfirmDlg, name, &deleteConfirmButtons) == 0)
        {
	MyListDeleteEntry(&mainList, mainList.active_entry);

	if (cmdInfo->type == TYPE_TOPIC)
	    {
	    /* Delete the blank line that was above the topic */
	    MyListDeleteEntry(&mainList, mainList.active_entry - 1);

	    /* Adjust all topic id numbers following the removed topic */
	    for (i = 0; i < mainList.num; i++)
	        if (commands[i]->topic > cmdInfo->topic)
		    commands[i]->topic -= 1;
	    }

	FreeCmdBuf(cmdInfo);

        ListInitScrollRange(&mainList);
        ListDisplayHighlighted(&mainList);
        ScrollBarDraw(mainList.scroll_bar);

	commandsEdited = TRUE;
        }
#endif
}

/*----------------------------------------------------------------------*
 * This routine clears a command info record to zero.			*
 *----------------------------------------------------------------------*/

void ClearCommand(CmdInfoRec far* info)
{
    MemorySet(info, 0, sizeof(CmdInfoRec));
}

/*----------------------------------------------------------------------*
 * This routine is used by both the Add and Edit commands to display	*
 * the main topic/command editing dialog box.				*
 *----------------------------------------------------------------------*/

Boolean LOCAL EditCommand(CmdInfoRec *info, Boolean add)
{
#ifndef EMERGENCY
    extern	Byte		addCommandTitle[];
    extern	Byte		editCommandTitle[];
    extern	TERec		editCommandNameTE;
    extern	TERec		editCommandDosTE;
    extern	RadioButtonsRec	editCommandRadios;
    extern	ButtonsRec	editCommandButtons;
    extern	DialogRec	editCommandDlg;
    auto	DIALOG_RECORD	*dr;
    auto	Boolean		done = FALSE;
    auto	Boolean		retVal = FALSE;

    editCommandDlg.title = (add) ? addCommandTitle : editCommandTitle;

    editCommandNameTE.string = info->name;
    editCommandDosTE.string  = info->dosCmds[0];
    editCommandRadios.value  = info->topic;

    InitTopicRadios();

    dr = DialogOpen2(&editCommandDlg,
    		     &editCommandNameTE,
     		     &editCommandDosTE,
		     &editCommandRadios,
		     &editCommandButtons);

    while (!done)
        {
	if (DialogLoop(dr, NULL) == ABORT_DIALOG)
	    break;

        switch (editCommandButtons.value)
	    {
	    case 0:   				/* OK button		*/
	        info->topic = editCommandRadios.value;
	        retVal = TRUE;
	        done = TRUE;
		break;

	    case 1:   				/* DESCRIPTION button	*/
	        EditCommandDescr(info, add);
					/* Reset the button to OK */
		editCommandButtons.value = 0;
		ButtonsWrite(&editCommandButtons, FALSE, TRUE, -1);
	        break;

	    case 2:				/* CANCEL button	*/
	    default:
	        done = TRUE;
	        break;
            }
	}

    DialogClose(dr, EVENT_USED);
    return (retVal);
#else
    return(TRUE);
#endif
}

/*----------------------------------------------------------------------*
 * This routine displays the topic editing dialog box, where the user	*
 * can rename and/or reorder a command topic.				*
 *----------------------------------------------------------------------*/

Boolean LOCAL EditTopic(CmdInfoRec *info, Boolean add)
{
#ifndef EMERGENCY
    extern	Byte		addCommandTitle[];
    extern	Byte		editCommandTitle[];
    extern	TopicRec	editTopics[MAX_TOPICS];
    extern	ListRec		editTopicList;
    extern	ButtonsRec	editTopicButtons;
    extern	DialogRec	editTopicDlg;
    extern	Boolean		editTopicFirstChar;
    auto	DIALOG_RECORD	*dr;
    auto	Boolean		done = FALSE;
    auto	Boolean		retVal = FALSE;
    extern	Boolean     	teFieldActive;

    editTopicDlg.title = (add) ? addCommandTitle : editCommandTitle;

    InitTopicList(add, info->name);

    dr = DialogOpen2(&editTopicDlg,
    		     &editTopicList,
		     &editTopicButtons);

    editTopicList.flags |= MOVE_MODE;	/* Turned off by ListInit()	*/
    editTopicFirstChar = TRUE;
    teFieldActive = FALSE;

    while (!done)
        {
	if (DialogLoop(dr, NULL) == ABORT_DIALOG)
	    break;

	{
	auto	TopicRec	far *topic;
	extern	Byte		teData[];
	topic = ListEntryAddress(&editTopicList, editTopicList.active_entry);
	StringCopyFar(topic->name, teData);
	}

        switch (editTopicButtons.value)
	    {
	    case 0:   				/* OK button		*/
	        RecalcTopics();
                StringCopyFar(info->name, editTopics[editTopicList.active_entry].name);
	        info->topic = (Byte) (editTopicList.active_entry);
	        retVal = TRUE;
	        done = TRUE;
		break;

	    case 1:   				/* DESCRIPTION button	*/
	        EditCommandDescr(info, add);
					/* Reset the button to OK */
		editTopicButtons.value = 0;
		ButtonsWrite(&editTopicButtons, FALSE, TRUE, -1);
	        break;

	    case 2:				/* CANCEL button	*/
	    default:
	        done = TRUE;
	        break;
            }
	}

    DialogClose(dr, EVENT_USED);
    return (retVal);
#else
    return(TRUE);
#endif
}

/*----------------------------------------------------------------------*
 * This routine implements the command description editor dialog box.	*
 *----------------------------------------------------------------------*/

void LOCAL EditCommandDescr(CmdInfoRec *info, Boolean add)
{
#ifndef EMERGENCY
    extern	COLORS		panel_colors;
    extern	Byte		addCommandTitle[];
    extern	Byte		editCommandTitle[];
    extern	EditBoxRec	descrEditBox;
    extern	UserControlRec	descrEditUCR;
    extern	OutputFieldRec	descrEditField;
    extern	ButtonsRec	okButton;
    extern	ButtonsRec	editDescrButtons;
    extern	DialogRec	editDescrDlg;
    extern	Byte		DESCRIPTION_STR[];
    extern	Byte		descrHeight, descrWidth;
    extern	Word		editBoxColor;
    extern	Word		descrBoxOffset;
    auto	DIALOG_RECORD	*dr;
    auto	Word		oldHelp;

    editDescrDlg.title = (add) ? addCommandTitle : editCommandTitle;

    CopyDescrToBuffer(info);

    oldHelp = HyperHelpTopicGet();
    HyperHelpTopicSet(H_NI_DLG_ADD_EDIT_ITEM_DESC);

    ColorSetSet(&panel_colors);
    dr = DialogOpen(&editDescrDlg,
    		    &descrEditUCR,
		    &descrEditField,
		    &okButton);

    /* Fix up the dialog with a frame around the edit box.  */

    CursorPush();
    DrawListBox(DESCRIPTION_STR,
    		dr->d.row + 4, dr->d.col - descrBoxOffset,
		DESCR_LINES, descrWidth, FALSE);
    CursorPop();

    editBoxColor = 0;
    ShowEditColor();

    DialogLoop(dr, EditCommandDescrFilter);
    DialogClose(dr, EVENT_USED);
    ColorRestoreSet();
    HyperHelpTopicSet(oldHelp);

    CopyBufferToDescr(info);
#endif
}

/*----------------------------------------------------------------------*
 * This routine checks for F2, which cycles the drawing color.		*
 *									*
 * Returns:	EVENT_USED	If F2 trapped				*
 *		EVENT_AVAIL	Otherwise				*
 *----------------------------------------------------------------------*/

Word EditCommandDescrFilter(DIALOG_RECORD *dr, Word *c)
{
#ifndef EMERGENCY
    extern	Word		editBoxColor;

    if (*c != F2)
        return (EVENT_AVAIL);

    if (++editBoxColor == 4)
        editBoxColor = 0;

    ShowEditColor();

    return(EVENT_USED);
#else
    return(0);
#endif
}

/*----------------------------------------------------------------------*
 * This routine updates the string that tells the user what the current	*
 * drawing color its.							*
 *----------------------------------------------------------------------*/

void LOCAL ShowEditColor(void)
{
#ifndef EMERGENCY
    extern	OutputFieldRec	descrEditField;
    extern	Byte*		editColorNames[];
    extern	Word		editBoxColor;
    auto	Word		row, col;

    FastGetRowCol(&row, &col);
    FastSetRowCol(descrEditField.row, descrEditField.col);

    switch (editBoxColor)
        {
	case 0:
	default:
            ColorSetAttr(NORMAL);
	    break;
	case 1:
            ColorSetAttr(INVERSE);
	    break;
	case 2:
            ColorSetAttr(BRIGHT);
	    break;
	case 3:
            ColorSetAttr(BRIGHT_INVERSE);
	    break;
	}

    FastWriteString(editColorNames[editBoxColor]);
    ColorSetAttr(NORMAL);
    FastWriteChar('.');
    FastWriteSpaces(descrEditField.maxWidth - StringLength(editColorNames[editBoxColor]) - 1);
    FastSetRowCol(row, col);
#endif
}

/*----------------------------------------------------------------------*
 * These routines copy a command description between a CmdInfoRec	*
 * and the global buffer used by the mini editor dialog box control.	*
 * Note that the format of the two now differs slightly.		*
 *----------------------------------------------------------------------*/

void LOCAL CopyDescrToBuffer(CmdInfoRec *info)
{
#ifndef EMERGENCY
    extern	Word		descrBuffer[DESCR_LINES] [DESCR_WIDTH + 2];
    auto	Word		i;

    for (i = 0; i < DESCR_LINES; i++)
        MemoryFarMove(descrBuffer[i], info->descr[i], 2 * (DESCR_WIDTH + 2));
#endif
}

void LOCAL CopyBufferToDescr(CmdInfoRec *info)
{
#ifndef EMERGENCY
    extern	Word		descrBuffer[DESCR_LINES] [DESCR_WIDTH + 2];
    auto	Word		i;

    for (i = 0; i < DESCR_LINES; i++)
        {
        MemoryFarMove(info->descr[i], descrBuffer[i], 2 * (DESCR_WIDTH + 2));
        info->descr[i][DESCR_WIDTH] = 0;	/* Make sure line terminated */
	}
#endif
}

/*----------------------------------------------------------------------*
 * This is a modified version of the library routine ListInsertEntry.	*
 * It inserts items into the whole list of totalListItems elements,	*
 * even when we have set the list->num field to a smaller number.	*
 *									*
 * This procedure inserts a new item into a list at the current list	*
 * position.  The caller must ensure that there is room in the list 	*
 * data array.  The list is NOT redrawn.				*
 *----------------------------------------------------------------------*/

void LOCAL MyListInsertEntry(ListRec *list, void far *newEntry)
{
#ifndef EMERGENCY
    extern	Word	totalListItems;
    auto	Ulong	active = list->active_entry;

    ListCopyEntries(list, active, totalListItems - active, active + 1);
    MemoryFarMove(ListEntryAddress(list, active), newEntry, list->entry_size);
    list->num += 1;
    totalListItems += 1;
    ListInitScrollRange(list);
#endif
}

/*----------------------------------------------------------------------*
 * This is a modified version of the library routine ListDeleteEntry.	*
 * It deletes items from the whole list of totalListItems elements,	*
 * even when we have set the list->num field to a smaller number.	*
 *									*
 * This procedure removes the current item from a list.			*
 * The list is NOT redawn.						*
 *----------------------------------------------------------------------*/

void LOCAL MyListDeleteEntry(ListRec *list, Ulong index)
{
#ifndef EMERGENCY
    extern	Word	totalListItems;

    if (list->num == 0L)
        return;

    list->num--;
    totalListItems--;

    if (list->num == 1L)
        {
        list->active_entry = 0L;
	}
    else
        {
        ListCopyEntries(list, index + 1, totalListItems - index, index);
        if (index == list->active_entry && index == list->num)
            list->active_entry -= 1;
	}
    ListInitScrollRange(list);
#endif
}

