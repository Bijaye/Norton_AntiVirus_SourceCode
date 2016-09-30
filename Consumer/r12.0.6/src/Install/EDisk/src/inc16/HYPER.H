/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   I:/VCS/HYPER.H_V   1.6   02 May 1992 09:49:20   ED  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************ 
 * $Log:   I:/VCS/HYPER.H_V  $ *
 * 
 *    Rev 1.6   02 May 1992 09:49:20   ED
 * Added HyperHelpTopicGet
 * 
 *    Rev 1.5   27 Mar 1992 13:38:20   TORSTEN
 * Updated the HELPSYS_STATUS structure.
 * 
 *    Rev 1.4   02 Mar 1992 01:19:50   JOHN
 * Added bFreeMemory parameter to HelpFarDone()
 * 
 *    Rev 1.3   06 Feb 1992 23:19:12   TORSTEN
 * Changed the prototypes of HelpVMMInit(), HelpFarInit(), and
 * HyperHelpOpen().
 * 
 *    Rev 1.2   09 Jan 1992 19:50:08   TORSTEN
 * Added the bHelpActive field to the HELPSYS_STATUS structure.
 * 
 *    Rev 1.1   12 Dec 1991 10:41:42   TORSTEN
 * I added the prototypes for the Far Heap version of the help display
 * module.
 * 
 *    Rev 1.0   11 Dec 1991 18:49:50   BRUNO
 * Initial revision.
 ************************************************************************/

#ifndef _HELPIO_H_

#define _HELPIO_H_

#include    <helpcomp.h>

/************************************************************************
 * the size of the backtrack stack                                      *
 ************************************************************************/

#define HELPSYS_STACKSIZE   10


/************************************************************************
 * find codes for HyperHelpLinkFind()                                   *
 ************************************************************************/

#define	HYPERLINK_FIRST         0
#define	HYPERLINK_LAST          1
#define	HYPERLINK_NEXT          2
#define	HYPERLINK_PREVIOUS      3
#define HYPERLINK_ADJUSTDOWN    4
#define HYPERLINK_ADJUSTUP      5


/************************************************************************
 * data type declarations                                               *
 ************************************************************************/

typedef struct {
    Word    wTopicID;                       // the ID of the topic
    Byte    szTopicName[MAX_TOPIC + 1];     // the name of the topic
    } HELPSYS_INDEX;

typedef struct {
    Word    wTopicID;                       // the topic ID to return to
    Word    wTopEntry;                      // the top entry in the listbox
    Word    wActiveEntry;                   // the active entry in the listbox
    Word    wHyperLinkLine;                 // the line of the active hyperlink
    Word    wHyperLinkColumn;               // the column of the active hyperlink
    } HELPSYS_STACK;

typedef struct {
    int                 nFileHandle;                 // the handle of the help file
    HELP_SEGMENTHEADER  rHeader;                     // the help segment header record
    Ulong               ulSegmentOffset;             // the offset to the desired help
                                                     //  segment within the file
    Boolean             bAdvanced;                   // TRUE if advanced topics should
                                                     //  be shown to the user
    Word                wCurrentTopicID;             // the ID of the displayed topic
    Byte far *          szHelpTextPointers;          // pointers to the start of each
                                                     //  line of help text
    Byte far *          szHelpText;                  // the help text for the current
                                                     //  topic
    Word                wHelpLines;                  // the number of lines
    HELP_LINKNODE far * lpLinkNodes;                 // the link nodes for the current
                                                     //  topic
    Word                wLinkNodeCount;              // the number of entries in the
                                                     //  link node table
    HELPSYS_INDEX far * lpIndexTable;                // the index table
    Word                wIndexEntries;               // the number of entries in the
                                                     //  topic index
    Boolean (PASCAL * HelpPageLoad) (Word wTopicID); // function to load help pages
    void (PASCAL * HelpPageDiscard) (void);          // function to discard help pages
    Boolean (PASCAL * HelpIndexLoad) (void);         // function to load the index
    void (PASCAL * HelpIndexDiscard) (void);         // function to discard the index
    Word                wHyperLinkLine;              // line of active hyperlink (or 0xFFFF)
    Word                wHyperLinkColumn;            // column of active hyperlink (or 0xFFFF)
    Byte                byBackTrackStackDepth;       // backtrack stack depth
    HELPSYS_STACK       rStack[HELPSYS_STACKSIZE];   // the backtrack stack
    Boolean             bHelpActive;                 // TRUE if help dialog is active
    Boolean             bHelpIsOpen;                 // TRUE if the help file
                                                     // has been opened
    Byte far *          szOpenProductVar;            // the product code
    Byte far *          szOpenSearchVar;             // the search variable
    Word                wOpenVersion;                // the file version
    Boolean             bOpenAdvanced;               // are advanced topics shown?
    Boolean (PASCAL * HelpOpen) (Byte far * szSearchVar, Byte far * szProductVar, Word wVersion, Boolean bAdvanced);
    } HELPSYS_STATUS;


/************************************************************************
 * function prototypes                                                  *
 ************************************************************************/

/* s:\libsrc\hyprhelp\helpdisp.c */
extern Word PASCAL HyperHelpDisplay(void);

/* s:\libsrc\hyprhelp\helpsys.c */
extern Boolean PASCAL HyperHelpOpen(Byte far * szSearchVar, Byte far * szProductVar, Word wVersion, Boolean bAdvanced);
extern void PASCAL HyperHelpClose(void);
extern Word PASCAL HyperHelpTopicSet(Word wTopicID);
extern Word PASCAL HyperHelpTopicGet(void);
extern Boolean PASCAL HyperHelpFindTopicHeader(Word wTopicID);

/* s:\libsrc\hyprhelp\helpvmm.c */
extern Boolean PASCAL HelpVMMInit(Byte far * szSearchVar, Byte far * szProductVar, Word wVersion, Boolean bAdvanced);
extern void PASCAL HelpVMMDone(void);
extern Boolean PASCAL HyperHelpVMMPageLoad(Word wTopicID);
extern void PASCAL HyperHelpVMMPageDiscard(void);
extern Boolean PASCAL HyperHelpVMMIndexLoad(void);
extern void PASCAL HyperHelpVMMIndexDiscard(void);
extern void PASCAL HelpVMMFastInit(Byte far * szSearchVar, Byte far * szProductVar, Word wVersion, Boolean bAdvanced);

/* s:\libsrc\hyprhelp\helpfar.c */
extern Boolean PASCAL HelpFarInit(Byte far * szSearchVar, Byte far * szProductVar, Word wVersion, Boolean bAdvanced);
extern void PASCAL HelpFarDone(Boolean bFreeMemory);
extern Boolean PASCAL HyperHelpFarPageLoad(Word wTopicID);
extern void PASCAL HyperHelpFarPageDiscard(void);
extern Boolean PASCAL HyperHelpFarIndexLoad(void);
extern void PASCAL HyperHelpFarIndexDiscard(void);

#endif
