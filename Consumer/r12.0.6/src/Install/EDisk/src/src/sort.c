/* $Header:   S:/SRC/NORTON/VCS/SORT.C_V   1.2   17 Oct 1992 02:11:40   JOHN  $ */

/*----------------------------------------------------------------------*
 * SORT.C     								*
 *									*
 * This file contains routines for sorting the command list.		*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/SRC/NORTON/VCS/SORT.C_V  $
 * 
 *    Rev 1.2   17 Oct 1992 02:11:40   JOHN
 * General cleanup and update for New Loader
 * 
 *    Rev 1.1   24 Jul 1992 17:29:14   SKURTZ
 * Fix source of compiler warning messages.
 *
 *    Rev 1.0   20 Aug 1990 15:19:32   DAVID
 * Initial revision.
 */

#include	<types.h>
#include	<dialog.h>
#include	<pulldown.h>
#include	<editbox.h>
#include	<main.h>
#include	<lint.h>

#include	"defs.h"
#include	"menu.h"
#include	"proto.h"

typedef int (*SortProc)(void far*, void far*);

int CompareAlpha(CmdInfoRec far* far* a, CmdInfoRec far* far* b);
int CompareTopic(CmdInfoRec far* far* a, CmdInfoRec far* far* b);
int PASCAL StringFarNCmpI(Byte far *d, Byte far *s, Word n);

#define TOPIC(p)  (p->type == TYPE_TOPIC)
#define BLANK(p)  (p->type == TYPE_BLANK)

/*----------------------------------------------------------------------*
 * This routine is used to sort commands alphabetically.  Blanks and	*
 * Topics are sorted to the bottom of the list, so they can be hidden.	*
 *----------------------------------------------------------------------*/

int CompareAlpha(CmdInfoRec far* far* a, CmdInfoRec far* far* b)
{
    auto	CmdInfoRec far*	aa;
    auto	CmdInfoRec far*	bb;

    aa = *a;
    bb = *b;

    if (BLANK(aa) || TOPIC(aa))
        return (1);
    if (BLANK(bb) || TOPIC(bb))
        return (-1);

    return ( StringFarNCmpI(bb->name, aa->name, COMMAND_WIDTH + 1) );
}

/*----------------------------------------------------------------------*
 * This routine is used to sort list records by their topic numbers.	*
 * Within each topic, the blank item comes first, then the topic 	*
 * heading record, then the commands (alphabetically). 			*
 *----------------------------------------------------------------------*/

int CompareTopic(CmdInfoRec far* far* a, CmdInfoRec far* far* b)
{
    auto	CmdInfoRec far*	aa;
    auto	CmdInfoRec far*	bb;
    auto	int		i;

    aa = *a;
    bb = *b;

    if ((i = aa->topic - bb->topic) != 0)
        return (i);

    if (BLANK(aa))
        return (-1);
    if (BLANK(bb))
        return (1);

    if (TOPIC(aa))
        return (-1);
    if (TOPIC(bb))
        return (1);

    return( StringFarNCmpI(bb->name, aa->name, COMMAND_WIDTH + 1) );
}

/*----------------------------------------------------------------------*
 * This routine does a case insensitive comparison of up to 'n' 	*
 * characters from two far strings.  Very much like StringFarNCmp()	*
 * in the library.							*
 *									*
 * Returns:	<0	if s < d					*
 *		 0 	if s == d					*
 *		>0	if s > d					*
 *----------------------------------------------------------------------*/

int PASCAL StringFarNCmpI(Byte far *d, Byte far *s, Word n)
{
    Word i = 1;

    for ( ; CharToUpper(*s) == CharToUpper(*d); s++, d++, i++)
	if ( (*s == '\0') || (i == n) )
	    return(0);				/* Strings matched 	*/
    return (CharToUpper(*s) - CharToUpper(*d));	/* Strings didn't match	*/
}

/*----------------------------------------------------------------------*
 * This routine is called when the user picks a sort command from the	*
 * pull-down menus.  It is also called after a command is added or	*
 * edited.								*
 *									*
 * It sorts the commands by name or by topic and redraws the commands	*
 * list.								*
 *----------------------------------------------------------------------*/

void DoSortCommands(Boolean alpha)
{
    extern	Boolean		sortByName;
    extern	Word		totalListItems;
    extern	ListRec		mainList;
    extern	CmdInfoRec	far *commands[];
    auto	CmdInfoRec	far *oldItem;
    auto	Ulong		i;
    auto	Boolean		rememberOld;

    if (sortByName == alpha)	/* Resorting in same order	*/
        {
        rememberOld = TRUE;
        oldItem = commands[mainList.active_entry];
	}
    else
        rememberOld = FALSE;

    sortByName = alpha;
    SortCommands(alpha, totalListItems);

    /* Locate the list item that is to be the active entry.
     * If we are looking for the old active entry, it may not be
     * in the list, e.g. if we were on a topic name
     * and we just resorted by name so topics don't show. */

    if (rememberOld)
        {
        for (i = 0; i < mainList.num; i++)
            if (commands[i] == oldItem)
	        break;
	if (i == mainList.num)
	    i = 0;
	}
    else
        i = 0;

    mainList.active_entry = i;
    MoveOffSkipItem();
    ListDisplayHighlighted(&mainList);
    ScrollBarDraw(mainList.scroll_bar);
}

/*----------------------------------------------------------------------*
 * This routine moves the mainList active entry off a skip item		*
 * (notably item 0 in the topics list), if possible.			*
 *----------------------------------------------------------------------*/

void MoveOffSkipItem (void)
{
    extern	ListRec		mainList;
    auto	Ulong		startItem;

    /* If we are on a skip item (esp. item 0 in topics list),
     * move off it. */

    startItem = mainList.active_entry;
    if (SkipDown(&(mainList.active_entry), mainList.num) == FALSE)
	if (SkipUp(&(mainList.active_entry)) == FALSE)
	    mainList.active_entry = startItem;
    ListAdjustTopEntry(&mainList);
}

/*----------------------------------------------------------------------*
 * This routine sorts the command list either alphabetically or by 	*
 * topic.  It does not redraw the list.					*
 *									*
 * If the list is sorted by name, then the topic headers and blank	*
 * list elements are sorted to the bottom of the commands[] array	*
 * and the number of list elements is reduced to exclude these items.	*
 * 									*
 * Input:	alpha		TRUE to sort by name, FALSE by topic	*
 		n		True number of commands[] elements	*
 *----------------------------------------------------------------------*/

void SortCommands (Boolean alpha, Word n)
{
    extern	ListRec		mainList;
    extern	CmdInfoRec far*	commands[];
    auto	SortProc	proc;
    auto	Word		i;

    if (alpha)
        proc = (SortProc) CompareAlpha;
    else
        proc = (SortProc) CompareTopic;

    MemoryShellSort(commands, sizeof(CmdInfoRec far*), n, proc);

    if (alpha)
        {
	for (i = 0; i < n; i++)
            if (BLANK(commands[i]) || TOPIC(commands[i]))
	    	break;
	mainList.num = i;
	}
    else
        mainList.num = n;

    ListInitScrollRange(&mainList);   
}

/*----------------------------------------------------------------------*
 * This routine dynamically builds the label strings for the command	*
 * topic radio buttons by searching the command list for all topic	*
 * entries.								*
 *----------------------------------------------------------------------*/

Byte editCommandRadioLabels [11] [COMMAND_WIDTH + 1];

void InitTopicRadios (void)
{
    extern	Word		totalListItems;
    extern	Byte*		editCommandRadioStrings[];
    extern	CmdInfoRec far*	commands[];
    auto	Word		i;
    auto	Byte		r;

    /* Just in case a topic number is missing,
     * initialize ALL the radio strings. */

    for (r = 0; r < MAX_TOPICS; r++)
	editCommandRadioStrings[r] = NULL;
        
    for (i = 0; i < totalListItems; i++)
        {
	if (TOPIC(commands[i]))
	    {
	    r = commands[i]->topic;
	    editCommandRadioStrings[r] = editCommandRadioLabels[r];
	    StringLocalize(editCommandRadioLabels[r], commands[i]->name);
	    }
	}
}

/*----------------------------------------------------------------------*
 * This routine counts the number of topic entries in the command list.	* 									*
 *----------------------------------------------------------------------*/

Byte CountTopics (void)
{
    extern	CmdInfoRec	far *commands[];
    extern	Word		totalListItems;
    auto	Byte	  	count;
    auto	Word		i;

    count = 0;
    for (i = 0; i < totalListItems; i++)
	if (TOPIC(commands[i]))
	    count++;
    return (count);
}

/*----------------------------------------------------------------------*
 * This routine prepares the list of command topics.  			*
 *									*
 * 'name' is the name of the topic that the user can edit or move.	*
 * If 'add' is TRUE, then 'name' is a new topic to be places at the	*
 * top of the list.  Otherwise, it is an existing topic that is to be	*
 * edited.								*
 *									*
 * The items in the list contain a topic name and their current topic 	*
 * number.  After the user moves things around, all topics and commands	*
 * are given new numbers based on the new list order.  The old topic	*
 * number is used by RecalcTopics() to perform this translation.	*
 *----------------------------------------------------------------------*/

void InitTopicList (Boolean add, Byte far *name)
{
    extern	TopicRec	editTopics[];
    extern	ListRec		editTopicList;
    extern	Word		totalListItems;
    extern	CmdInfoRec far*	commands[];
    auto	Word		i;
    auto	Byte		r;

    if (add > 1)			/* Just to make sure		*/
        add = 1;

    editTopicList.num = CountTopics() + add;
    editTopicList.top_entry = 0; 

    if (add)
        {
        /* Give the new topic the next available topic id,
         * but put it at the top of the list. */

        editTopics[0].origTopic = (Byte) editTopicList.num;
        editTopicList.num;
        StringCopyFar(editTopics[0].name, name);
        editTopicList.active_entry = 0; 
	}

     /* Put the old topics into the list,
      * with their original topic numbers. */

     for (i = 0; i < totalListItems; i++)
        {
	if (TOPIC(commands[i]))
	    {
	    r = commands[i]->topic;
	    editTopics[r + add].origTopic = r;
	    StringLocalize(editTopics[r + add].name, commands[i]->name);
	    if (!add && StringFarNCmp(name, commands[i]->name, COMMAND_WIDTH+1) == 0)
                editTopicList.active_entry = r; 
	    }
	}

    ListAdjustTopEntry(&editTopicList);
}

/*----------------------------------------------------------------------*
 * This routine gives all items in the command list new topic numbers,	*
 * based on the order of the topics in the topic list.			*
 *----------------------------------------------------------------------*/

void RecalcTopics (void)
{
    extern	TopicRec	editTopics[];
    extern	ListRec		editTopicList;
    extern	Word		totalListItems;
    extern	CmdInfoRec	far *commands[];
    auto	Word		i;
    auto	Ulong		j;
    auto	Byte		oldTopic;

    for (i = 0; i < totalListItems; i++)
        {
	oldTopic = commands[i]->topic;
	for (j = 0; j < editTopicList.num; j++)
	    if (editTopics[j].origTopic == oldTopic)
	        {
                commands[i]->topic = (Byte) j;
		break;
		}
	}
}
