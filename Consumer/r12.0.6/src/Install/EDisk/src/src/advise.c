/* $Header:   S:/SRC/NORTON/VCS/ADVISE.C_V   1.4   07 Apr 1993 18:05:00   SKURTZ  $ */

/*----------------------------------------------------------------------*
 * ADVISE.C								*
 *									*
 * controls the advise portion of the pulldown menu.  This includes	*
 * the search routine.   						*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/SRC/NORTON/VCS/ADVISE.C_V  $
 * 
 *    Rev 1.4   07 Apr 1993 18:05:00   SKURTZ
 * Added some programs that are launchable from advise hotlink. PTR 8315
 *
 *    Rev 1.3   04 Mar 1993 17:16:18   SKURTZ
 * Added advise topics for Excel and Qpro
 *
 *    Rev 1.2   02 Mar 1993 16:09:32   SKURTZ
 * Added F1 help for advise search
 *
 *    Rev 1.1   26 Jan 1993 12:32:34   SKURTZ
 * Selecting Search after a search caused an infinite loop
 *
 *    Rev 1.0   05 Jan 1993 15:43:44   SKURTZ
 * Initial revision.
 *
 */

#ifndef EMERGENCY


#include	<types.h>
#include	<dialog.h>
#include	<pulldown.h>
#include	<keys.h>
#include	<main.h>
#include        <hyper.h>
#include	<lint.h>

#include	"menu.h"
#include        "adv.h"
#include        "nort-hlp.h"
#include	"defs.h"
#include	"proto.h"

#define		_DOS_		0
#define		_CHKDSK_	1
#define		_WP_		2
#define		_123_		3
#define		_DBASE_		4
#define		_EXCEL_		5
#define		_QUATTRO_	6


/*  static local variables */
static	Byte		searchKey [KEY_LENGTH + 1];
static	Byte		originalKey [KEY_LENGTH + 1];
static	Byte 		*keyWords [MAX_KEYS + 1];

static  Word		wRunAProgram;

/* -------------------------------------------------------------------- *
 * Search variables							*
 * -------------------------------------------------------------------- */

#define	CLEAN_STRING		80

static Byte	cleanString [CLEAN_STRING];
Byte		errorType [MAX_ERRORS];
ErrorInfoRec	foundErrorsData [MAX_ERRORS];
int		totalMessages;


extern      NIFileInfoRec 	fileInfo[NUM_EXE_FILES];



/* local prototypes */

void  AdviseLaunch(Word wTopic);
void LOCAL SearchAdvise (void);
void LOCAL CombineErrors (ErrorInfoRec *errorsData, Byte type);
Boolean LOCAL GetSearchText (Byte *searchKey);
Boolean LOCAL ExactSearch (Byte *searchText, ErrorInfoRec *errorsData, Byte type);
int LOCAL ParseSearchText (Byte *searchText, Byte *keyWords[]);
Boolean LOCAL AndSearch (int keys, Byte *keyWords[], ErrorInfoRec *errorsData, Byte type);
Boolean LOCAL OrSearch (int keys, Byte *keyWords[], ErrorInfoRec *errorsData, Byte type);
Boolean LOCAL ShowNoneFound (Byte *searchKey);
Boolean LOCAL ShowFoundErrors (Boolean standAlone, Boolean showLegend);
void LOCAL InsertErrorMessage (ErrorInfoRec *inputMessage, Byte type);
static Byte * near FilterString (Byte *s1, Byte *s2);
Boolean LOCAL MatchStrings (Byte *s1, Byte *s2);
STATIC void ErrorListWriteEntry2 (ListRec *list, ErrorInfoRec far* entry, int  attr, Ulong itemNum);
void  CopyUsingHelpIndex(Word wHelpIndex,Byte * bpString);

ScrollBarRec errorListScrollBar;

static ListRec errorList2 =		/* List	description		*/
    {
    0, 0,				/* row, col (set by library)	*/
    1,					/* number of columns		*/
    ERROR_LINE_WIDTH2,	     		/* width of each column		*/
    ERROR_LINES,			/* number of rows		*/
    ErrorListWriteEntry2,  		/* procedure to display entry	*/
    NULL,	    			/* procedure for mouse events	*/
    NULL,				/* procedure to check matches	*/
    NULL,				/* procedure to filter events	*/
    (void far *) NULL,		   	/* start address of entries	*/
    sizeof(ErrorInfoRec),    		/* size of each entry		*/
    ACCEPT_DCLICKS | THIN_LIST_BOX, 	/* flags used for operation	*/
    &errorListScrollBar,  		/* pointer to scroll bar defn	*/
    NULL_STR,	       			/* str to display between cols	*/
    5, 2,				/* scroll delay values		*/
    0,					/* number of entries in list	*/
    0,					/* top entry			*/
    0,					/* active entry			*/
    };

/**************************************************************************
 * CopyUsingHelpIndex
 *
 * DESCRIPTION  : Using the helpdefines as a search item, looks through
 *		  fileInfo and copies the selected filename
 *		  into bpString.
 *
 * IN      :      The help topic and string
 *
 * RETURN VALUES: the string with the filename in it.
 *************************************************************************
 * SKURTZ : 01-05-93 at 01:04:09pm Function created
 ************************************************************************* */
void  CopyUsingHelpIndex(Word wHelpIndex,Byte * bpString)
{

    auto	Word		wIndex;

    for (wIndex = 0; wIndex < NUM_EXE_FILES;wIndex++)

	{
	if (fileInfo[wIndex].helpID == wHelpIndex)
	    break;
	};


    StringCopy(bpString,fileInfo[wIndex].name);
}
/**************************************************************************
 * AdviseLaunch
 *
 * DESCRIPTION  : Launches an application chosen from the Advise/help screens
 *
 * IN      : The Help topic
 *
 * RETURN VALUES: May not return!
 *************************************************************************
 * SKURTZ : 01-05-93 at 02:21:40pm Function created
 ************************************************************************* */
void  AdviseLaunch(Word wTopic)
{
    extern      Byte		mainString[];
    extern      Byte  		DIR_STR[];
    extern      Byte  		CHKDSK_STR[];
    extern      Byte  		NAV_STR[];



    switch (wTopic)
        {
        case ADV_ADVISE_ITEM_1:		//  ndd.exe
            CopyUsingHelpIndex(H_NI_NDD,mainString);
            break;
    	case ADV_ADVISE_ITEM_2:		//  disktool.exe
            CopyUsingHelpIndex(H_NI_DISKTOOL,mainString);
            break;
        case ADV_ADVISE_ITEM_3:	        //  unerase.exe
            CopyUsingHelpIndex(H_NI_UNERASE,mainString);
            break;
    	case ADV_ADVISE_ITEM_4:		//  unformat.exe
            CopyUsingHelpIndex(H_NI_UNFORMAT,mainString);
            break;
    	case ADV_ADVISE_ITEM_5:		//  speedisk.exe
            CopyUsingHelpIndex(H_NI_SD,mainString);
            break;
    	case ADV_ADVISE_ITEM_6:		//  calibrate.exe
            CopyUsingHelpIndex(H_NI_CALIBRATE,mainString);
            break;
    	case ADV_ADVISE_ITEM_7:		//  dir
            StringCopy(mainString,DIR_STR);
            break;
    	case ADV_ADVISE_ITEM_8:		//  chkdsk.exe
            StringCopy(mainString,CHKDSK_STR);
            break;
    	case ADV_ADVISE_ITEM_9:		//  sysinfo.exe
            CopyUsingHelpIndex(H_NI_SI,mainString);
            break;
    	case ADV_ADVISE_ITEM_10:	//  NAV  (?!)
            StringCopy(mainString,NAV_STR);
            break;
    	case ADV_ADVISE_ITEM_11:	//  filefind
            CopyUsingHelpIndex(H_NI_FILEFIND,mainString);
            break;


	/*---------------04-07-93 05:57:05pm---------------
 	* Advise help actually only uses ITEM_12 and ITEM_13
	* The others are included for completeness.
 	*-------------------------------------------------*/

    	case ADV_ADVISE_ITEM_12:	//  filefix /L
    	case ADV_ADVISE_ITEM_13:	//  filefix /D
    	case ADV_ADVISE_ITEM_14:	//  filefix /W
    	case ADV_ADVISE_ITEM_15:	//  filefix /E
    	case ADV_ADVISE_ITEM_16:	//  filefix /S
    	case ADV_ADVISE_ITEM_17:	//  filefix /Q
            CopyUsingHelpIndex(H_NI_FILEFIX,mainString);
	    break;

    	case ADV_ADVISE_ITEM_18:	//  diskedit.exe
            CopyUsingHelpIndex(H_NI_DISKEDITOR,mainString);
            break;

    	default:
	    return;
        }

        Launch(mainString);			/* May not return	        	*/
}
/**************************************************************************
 * DoAdvise
 *
 * DESCRIPTION  : Performs the ADVISE system
 *
 * IN      :
 *
 * RETURN VALUES:
 *************************************************************************
 * SKURTZ : 01-04-93 at 05:04:27pm Function created
 ************************************************************************* */
void  DoAdvise(void)
{
    extern	Byte		*pszProductEnvVar;
    auto	Word		wResult;


    HelpVMMDone();		// close down original file
    HelpVMMInit(pszProductEnvVar, ADV_HELPFILE_NAME, ADV_HELPFILE_VERSION, TRUE);
    HyperHelpTopicSet(ADV_ADVISE);

    wResult = HyperHelpDisplay();

DoAnotherSearch:

    if (wResult == ADV_ADVISE_SEARCH)
	{
	SearchAdvise();                 // SearchAdvise() modifies wRunAProgram
	wResult = wRunAProgram;
        goto DoAnotherSearch;
	}
    else if (wResult != 0)
	{
	AdviseLaunch(wResult);
	}

    HelpVMMDone();		// close down Advise file.


    HelpVMMInit(pszProductEnvVar, HELPFILE_NAME, HELPFILE_VERSION, TRUE);
    HyperHelpTopicSet(H_NI_MAIN);

}
/* ------------------------------------------------------------------------
 * SearchAdvise
 *
 * This function is called when the user selects "Search..." from the
 * main menu.  It prompts the user to enter search text, then performs
 * Exact, AND, and OR searches on the search text found.  Finally, it
 * displays the error messages found, if any, and loops until the user
 * quits.
 * ------------------------------------------------------------------------ */

void LOCAL SearchAdvise (void)
{
    extern	ErrorInfoRec	foundErrorsData[];	/* found messages */
    extern	ErrorInfoRec	dosErrorsData[];
    extern	ErrorInfoRec	chkdskErrorsData[];
    extern	ErrorInfoRec	wpErrorsData[];
    extern	ErrorInfoRec	lotusErrorsData[];
    extern	ErrorInfoRec	dbaseErrorsData[];
    extern      ErrorInfoRec	excelErrorsData[];
    extern	ErrorInfoRec	qProErrorsData[];
    extern	CheckBoxRec	searchForErrorsCB;
    extern	int		totalMessages;		/* total messages  */
    auto	Byte		searchKey [KEY_LENGTH + 1];
    auto	Byte		originalKey [KEY_LENGTH + 1];
    auto	Byte 		*keyWords [MAX_KEYS + 1];
    auto	int		keys = 0;
    auto	Boolean		done = FALSE;


    *searchKey = *originalKey = '\0';		/* initialize these */
    searchForErrorsCB.value = FALSE;

    while (!done)
	{
	StringCopy (searchKey, originalKey);	/* restore previous text */

	if (!GetSearchText (searchKey))		/* get new search text */
	    {
	    wRunAProgram = 0;			/* signal no more. leave */
	    return;
	    }

	StringCopy (originalKey, searchKey);	/* save new search text */


	totalMessages = 0;		/* start at zero messages found */
	errorList2.entries = (void far *) NULL;
	errorList2.num = 0;
	done = FALSE;

					/* start searching... */

					/* look for exact matches... */
	done |= ExactSearch (searchKey, dosErrorsData, _DOS_);
	done |= ExactSearch (searchKey, chkdskErrorsData, _CHKDSK_);

	if (searchForErrorsCB.value)
	    {
	    done |= ExactSearch (searchKey, wpErrorsData, _WP_);
	    done |= ExactSearch (searchKey, lotusErrorsData, _123_);
	    done |= ExactSearch (searchKey, dbaseErrorsData, _DBASE_);
	    done |= ExactSearch (searchKey, excelErrorsData, _EXCEL_);
	    done |= ExactSearch (searchKey, qProErrorsData, _QUATTRO_);
	    }

					/* parse up the search text */
	keys = ParseSearchText (searchKey, keyWords);

					/* now do an AND search... */
	done |= AndSearch (keys, keyWords, dosErrorsData, _DOS_);
	done |= AndSearch (keys, keyWords, chkdskErrorsData, _CHKDSK_);

	if (searchForErrorsCB.value)
	    {
	    done |= AndSearch (keys, keyWords, wpErrorsData, _WP_);
	    done |= AndSearch (keys, keyWords, lotusErrorsData, _123_);
	    done |= AndSearch (keys, keyWords, dbaseErrorsData, _DBASE_);
	    done |= AndSearch (keys, keyWords, excelErrorsData, _EXCEL_);
	    done |= AndSearch (keys, keyWords, qProErrorsData, _QUATTRO_);
	    }

					/* finally, do an OR search */
 	done |= OrSearch (keys, keyWords, dosErrorsData, _DOS_);
	done |= OrSearch (keys, keyWords, chkdskErrorsData, _CHKDSK_);

	if (searchForErrorsCB.value)
	    {
	    done |= OrSearch (keys, keyWords, wpErrorsData, _WP_);
	    done |= OrSearch (keys, keyWords, lotusErrorsData, _123_);
	    done |= OrSearch (keys, keyWords, dbaseErrorsData, _DBASE_);
	    done |= OrSearch (keys, keyWords, excelErrorsData, _EXCEL_);
	    done |= OrSearch (keys, keyWords, qProErrorsData, _QUATTRO_);
	    }

	if (!done)
	    done = ShowNoneFound (originalKey);	/* no matches found */
	else
	    done = ShowFoundErrors (FALSE, searchForErrorsCB.value);
	}

}

/* ------------------------------------------------------------------------
 * DoApplErrors
 *
 * This function is called when the user selects "Application Errors..."
 * from the menu.  It combines all the application error messages into a
 * master list, then displays them.
 * ------------------------------------------------------------------------ */

void AdviseApplErrors (Byte *fileBuffer, Word bufferSize, void (*launchProc)(Byte *command))
{
    extern	ErrorInfoRec	foundErrorsData[];	/* found messages */
    extern	ErrorInfoRec	wpErrorsData[];
    extern	ErrorInfoRec	lotusErrorsData[];
    extern	ErrorInfoRec	dbaseErrorsData[];
    extern      ErrorInfoRec	excelErrorsData[];
    extern	ErrorInfoRec	qProErrorsData[];
    extern	int		totalMessages;		/* total messages  */



    totalMessages = 0;		/* start at zero messages found */
    errorList2.entries = (void far *) NULL;
    errorList2.num = 0;

    CombineErrors (wpErrorsData, _WP_);
    CombineErrors (lotusErrorsData, _123_);
    CombineErrors (dbaseErrorsData, _DBASE_);
    CombineErrors (excelErrorsData, _EXCEL_);
    CombineErrors (qProErrorsData, _QUATTRO_);

    ShowFoundErrors (TRUE, TRUE);
}

/* ------------------------------------------------------------------------
 * CombineErrors
 *
 * Shoves an entire list of error messages into the combined messages list.
 * This is used to combine all of the various application error message
 * sections into one list.
 *
 * Parameters:
 *	errorsData	The ErrorInfoRec array for a particular application
 *	type		The type of application being added
 * ------------------------------------------------------------------------ */

void LOCAL CombineErrors (ErrorInfoRec *errorsData, Byte type)
{
    while (errorsData->title != NULL)
	{
	InsertErrorMessage (errorsData, type);
	errorsData++;
	}
}

/* ------------------------------------------------------------------------
 * ShowNoneFound
 *
 * If no matches are found, this box explains that fact and asks the
 * user if he/she wants to do a new search or to quit;
 *
 * Returns:
 * 	FALSE	We aren't done yet.  Do another search
 *	TRUE	We are done
 * ------------------------------------------------------------------------ */

Boolean LOCAL ShowNoneFound (Byte *searchKey)
{
    extern	DialogRec	searchResultNoneDlg;
    extern	ButtonsRec	newSearchButtons;
    auto	int		result;

    result = DialogBox2 (&searchResultNoneDlg, searchKey, &newSearchButtons);

    if (result == ABORT_DIALOG || result == ESC || newSearchButtons.value == 1)
        return (TRUE);
    else
    	return (FALSE);
}

/* ------------------------------------------------------------------------
 * ShowFoundErrors
 *
 * If matches are found, this box shows them in the list box.  The user
 * can then expand the messages, start a new search, or quit.  The
 * standAlone parameter determines which set of buttons to use.  The
 * DoSearch() function uses the button set that includes "New Search".
 *
 * Parameters:
 *	standAlone	TRUE if this is called from DoApplErrors()
 *			FALSE if called from DoSearch()
 *
 * Returns:
 * 	FALSE	We aren't done yet.  Do another search
 *	TRUE	We are done
 * ------------------------------------------------------------------------ */

Boolean LOCAL ShowFoundErrors (Boolean standAlone, Boolean showLegend)
{
    extern	ErrorInfoRec	foundErrorsData[];	/* found messages */
    extern	DialogRec	searchResultDlg;
    extern	ButtonsRec	newLookupButtons;
    extern	ButtonsRec	lookupButtons;
    extern	int		totalMessages;		/* total messages  */
    extern	Byte		*applLegend;
    extern	Byte		*applLegend2;
    extern	Byte		*applTitle;
    extern	Byte		*searchLegend;
    extern	Byte		*searchTitle;
    auto	Word		wItemToOpen;
    auto	DIALOG_RECORD	*dr;
    auto	Boolean		done = FALSE;


					/* patch our error list in */
    errorList2.entries = (void far *) foundErrorsData;
    errorList2.num = totalMessages;

    searchResultDlg.title = standAlone ? applTitle : searchTitle;
    searchResultDlg.strings[2] = showLegend ? applLegend  : searchLegend;
    searchResultDlg.strings[3] = showLegend ? applLegend2 : searchLegend;

    						/* light up the stage */
    dr = DialogOpen2 (&searchResultDlg,
    			&errorList2,
			(standAlone) ? &lookupButtons : &newLookupButtons);

    while (TRUE)
        {
	if (DialogLoop(dr, NULL) == ABORT_DIALOG ||
		((standAlone) ?
		  lookupButtons.value == 1 :
		  newLookupButtons.value == 2))
            {
            done = TRUE;
            break;
            }

	if (!standAlone && newLookupButtons.value == 1)	/* new search */
            {
            done = FALSE;
            break;
            }

	if (!standAlone && newLookupButtons.value == 0)
	    {
    	    DialogClose(dr, EVENT_USED);
	    wItemToOpen = foundErrorsData[errorList2.active_entry].helpTopic;
            HyperHelpTopicSet(wItemToOpen);
    	    wRunAProgram = HyperHelpDisplay();
	    return(TRUE);
	    }
	}

    DialogClose(dr, EVENT_USED);

    return (done);
}

/* -----------------------------------------------------------------------
 * GetSearchText
 *
 * Throws up a text edit field that allows the user to enter some text
 * to search for.
 *
 * Parameters:
 *	searchKey	a string at least KEY_LENGTH characters long
 *
 * Returns:
 *	TRUE		text was entered
 *	FALSE		cancel the search
 * ----------------------------------------------------------------------- */

Boolean LOCAL GetSearchText (Byte *searchKey)
{
    extern	DialogRec 	searchForErrorsDlg;
    extern	ButtonsRec	startSearchButtons;
    extern	CheckBoxRec	searchForErrorsCB;
    extern	TERec 		searchForErrorsTE;
    auto	int		result;


    searchForErrorsTE.string = searchKey;

    HyperHelpTopicSet(ADV_DLG_ADVISE_SEARCH);
    result = DialogBox2 (&searchForErrorsDlg, &searchForErrorsTE,
    				&searchForErrorsCB, &startSearchButtons);

    if (result == ESC || result == 1 || *searchKey == '\0')
	return (FALSE);

    StringToUpper (searchKey);

    return (TRUE);
}

/* -----------------------------------------------------------------------
 * ExactSearch
 *
 * Does a simple comparison between the search text and the messages on
 * file.  The search text must match part of the message EXACTLY for there
 * to be a match, including spaces, punctuation, etc.  The search is
 * case-insensitive.
 *
 * Returns:
 *	TRUE	if any messages were found
 *	FALSE	if no messages were found
 * ----------------------------------------------------------------------- */

Boolean LOCAL ExactSearch (Byte *searchText, ErrorInfoRec *errorsData, Byte type)
{
    auto	Boolean		anythingFound = FALSE;


    while (errorsData->title != NULL)
	{
	FilterString (errorsData->title, cleanString);

	if (MatchStrings (cleanString, searchText))
	    {
	    				/* found!  stick it in the box */
	    InsertErrorMessage (errorsData, type);
	    anythingFound = TRUE;
	    }

	errorsData++;			/* next message */
	}

    return (anythingFound);
}

/* ----------------------------------------------------------------------
 * AndSearch
 *
 * Does a comparison on a word-by-word basis between the search text and
 * the messages on file.  If ALL of the search text is found within the
 * message, in any order, there is a match.
 *
 * Returns:
 *	TRUE	if any messages were found
 *	FALSE	if no messages were found
 * ---------------------------------------------------------------------- */

Boolean LOCAL AndSearch (int keys, Byte *keyWords[], ErrorInfoRec *errorsData, Byte type)
{
    auto	Boolean		anythingFound = FALSE;
    auto	int		x;
    auto	int		hits;


    if (keys == 1)			/* don't bother if only one word */
    	return (FALSE);

    while (errorsData->title != NULL)
	{
	hits = 0;

	FilterString (errorsData->title, cleanString);

	for (x = 0; x < keys + 1; x++)
	    hits += MatchStrings (cleanString, keyWords[x]);

	if (hits == keys)
	    {  				/* found!  stick it in the box */
	    InsertErrorMessage (errorsData, type);
	    anythingFound = TRUE;
	    }

	errorsData++;			/* next message */
	}

    return (anythingFound);
}


/* ----------------------------------------------------------------------
 * OrSearch
 *
 * Does a comparison on a word-by-word basis between the search text and
 * the messages on file.  If ANY word in the search text is found within
 * a message, there is a match.
 *
 * Returns:
 *	TRUE	if any messages were found
 *	FALSE	if no messages were found
 * ---------------------------------------------------------------------- */

Boolean LOCAL OrSearch (int keys, Byte *keyWords[], ErrorInfoRec *errorsData, Byte type)
{
    auto	Boolean		anythingFound = FALSE;
    auto	int		x;
    auto	int		hits;

    if (keys == 1)			/* don't bother if only one word */
    	return (FALSE);

    while (errorsData->title != NULL)
	{
	hits = 0;

	FilterString (errorsData->title, cleanString);

	for (x = 0; x < keys + 1; x++)
	    {
	    if (MatchStrings (cleanString, keyWords[x]))
		{
		InsertErrorMessage (errorsData, type);
		anythingFound = TRUE;
		break;
		}
	    }

	errorsData++;			/* next message */
	}

    return (anythingFound);
}

/* ---------------------------------------------------------------------
 * ParseSearchText
 *
 * Parses the text to search for into a maximum of MAX_KEYS arguments,
 * which are placed in an array of pointers passed to the function.
 * Arguments are delimited by spaces, and leading white spaces are
 * eliminated.
 *
 * Parameters:
 *	searchText	a line of search text
 *	keyWords[]		an array of pointers to receive the arguments
 *
 * Returns:
 *	the total number of arguments found
 * --------------------------------------------------------------------- */

int LOCAL ParseSearchText (Byte *searchText, Byte *keyWords[])
{
    auto	int	keys = 0;    		/* total arguments found */


    keyWords [keys++] = searchText;			/* first argument is easy */

    while (*searchText && keys < MAX_KEYS)	/* go until the end */
	{
	if (*searchText == ' ')			/* found a space */
	    {
	    *searchText++ = '\0';		/* replace with a NULL */

	    while (*searchText == ' ')		/* suck up white spaces */
		searchText++;

	    keyWords [keys++] = searchText;		/* next argument starts here */
	    }
	else
	    searchText++;			/* next character */
	}

    keyWords [keys] = NULL;
    return (keys);				/* how many did we do? */
}

/* ------------------------------------------------------------------------
 * InsertErrorMessage
 *
 * Error messages found during one of the searches are sent here, where
 * they are inserted into the list box.  The same message may already be
 * in the list box, so the new message is compared against all of the
 * messages in the list box to see if it should be inserted or not.
 *
 * Parameters:
 *	inputMessage	ErrorInfoRec of the message to insert
 *
 * Returns:
 *	Nothing
 * ------------------------------------------------------------------------ */

void LOCAL InsertErrorMessage (ErrorInfoRec *inputMessage, Byte type)
{
    extern	ErrorInfoRec	foundErrorsData[];	/* found messages */
    extern	int		totalMessages;	/* total messages found */
    auto	int		x;


    if (totalMessages > MAX_ERRORS)
	return;

    for (x = 0; x < totalMessages; x++)		/* scan existing messages */
	{
	if (foundErrorsData[x].title == inputMessage->title)
	    return;
	}
						/* add the message to the list */
    foundErrorsData [totalMessages].title = inputMessage->title;
    foundErrorsData [totalMessages].titleHelpTopic =
    					inputMessage->titleHelpTopic;
    foundErrorsData [totalMessages].helpTopic = inputMessage->helpTopic;

    errorType [totalMessages] = type;

    totalMessages++;
}


/* ----------------------------------------------------------------------
 * FilterString
 *
 * Copies the input string to the output string, making it upper-case,
 * and stripping out attributes specified by a caret (^) symbol followed
 * by a one-byte attribute code.  A double-caret is converted into a
 * single caret and copied to the output string.
 *
 * Parameters:
 *	s1	input string
 *	s2	output string, large enough to hold the input string
 *
 * Returns
 *	s2
 * ----------------------------------------------------------------------- */

static Byte * near FilterString (Byte *s1, Byte *s2)
{
    register	Word	x = 0;

    while (*s1 && (x < ERROR_LINE_WIDTH))
	{
	if (*s1 == '^')			/* filter out attributes */
	    {
	    s1++;
	    if (*s1 != '^')		/* but keep double-carets */
	    	s1++;
	    }

	*s2++ = *s1++;			/* copy the string */
	x++;
	}

    *s2 = '\0';				/* terminate the copy */
    StringToUpper (s2);			/* upper case the copy */
    return (s2);
}

/* ---------------------------------------------------------------------
 * MatchStrings
 *
 * All this does is front the StringSearch() function.  I had something
 * much more nobler in mind, but I didn't need to do it.
 *
 * Parameters:
 *	s1	string to search (the filtered version)
 *	s2	string searched for
 *
 * Returns:
 *	TRUE	if s2 was found in s1
 *	FALSE	if s2 was not found in s1
 * ---------------------------------------------------------------------- */

Boolean LOCAL MatchStrings (Byte *s1, Byte *s2)
{
    auto	int	len;


    len = StringLength (s1);

    if (StringSearch (s1, s2, len) == (Byte *)-1)
    	return (FALSE);
    else
	return (TRUE);
}

/*----------------------------------------------------------------------*
 * This is the write entry routine for the combined lists of error
 * messages.  It adds a seven-character error "type" string (WP, DOS,
 * etc.)
 *----------------------------------------------------------------------*/

STATIC void ErrorListWriteEntry2 (ListRec *list, ErrorInfoRec far* entry, int  attr, Ulong itemNum)
{
    extern	Byte	errorType[];
    extern	Byte	*adviseErrorSource[];

    if (entry == NULL)			/* Nothing to draw		*/
        {
        FastWriteSpaces(list->width);
        return;
	}

    FastWriteChar(' ');
    ColorSaveAll();
    ColorSetAttr(attr);
    FastWriteChar(' ');
    FastWriteString(adviseErrorSource[ errorType[ (int)itemNum ] ]);
    HelpWriteLine(entry->title, list->column_width - 11);
    ColorRestoreAll();
    FastWriteChar(' ');
}
















#endif

/**/
