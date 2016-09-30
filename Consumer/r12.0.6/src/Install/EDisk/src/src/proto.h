/* $Header:   S:/SRC/NORTON/VCS/PROTO.H_V   1.5   06 Jan 1993 11:29:08   SCOTTP  $ */

/*----------------------------------------------------------------------*
 * Norton Integrator		     			PROTO.H		*
 *									*
 * This file contains prototypes for all non-local functions.		*
 *									*
 * Copyright 1991 by Symantec Inc.					*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/SRC/NORTON/VCS/PROTO.H_V  $
 * 
 *    Rev 1.5   06 Jan 1993 11:29:08   SCOTTP
 * added do advise
 *
 *    Rev 1.4   28 Aug 1992 11:05:04   SKURTZ
 * Preparations for Scorpio
 *
 *    Rev 1.3   25 Jun 1991 11:11:14   PETERD
 * Moved Advise menu items and related functions to library.
 *
 *    Rev 1.2   07 Feb 1991 15:00:06   ED
 *
 *
 *    Rev 1.1   18 Dec 1990 16:10:20   ED
 * Added function declarations for the search module
 *
 *    Rev 1.0   20 Aug 1990 15:19:46   DAVID
 * Initial revision.
 */

#include "types.h"
#include "password.h"


	    /***** NI.C *****/

void	    main (char *args);
void	    ProcessLauncherCmdLine (void);
void        InitError (void);
void        RestoreError (void);
void	    InitDisplay (void);
void	    RedrawMenuBar (void);
void	    SaveVarsToLoader (void);
void	    RestoreVarsFromLoader (void);
void	    FinishUp(void);

	    /***** EVENTS.C *****/

void	    EventLoop (void);


	    /***** MAINLIST.C *****/

DIALOG_RECORD*	OpenMainList (void);
void	    ChangeMainList (Word oldButton, Word newButton, Ulong topEntry, Ulong activeEntry);
void	    CloseMainList (DIALOG_RECORD *dr);
Word	    MainFilter (DIALOG_RECORD *dr, Word event);
Boolean     SkipUp (Ulong *entry);
Boolean     SkipDown (Ulong *entry, Ulong num);
int	    MainListDoMouse (ListRec *list, Ulong entryNum, RECT *entryRect, Word buttons, int mouseClicks);
Word	    MainListEventFilter (ListRec *list, Word *c);
void	    MainListWriteEntry (ListRec *list, CmdInfoRec far* far* entry, int attr, Ulong itemNum);
void	    DoMainListCommand (void);
CmdInfoRec far *GetCurrentCommand (void);
void        SetCommandHelp (void);


	    /***** OPS.C *****/

void	    MenuDisableItems (void);
void	    DoAddCmd (void);
void	    DoEditCmd (void);
void	    DoMoveCmd (void);
void	    EndMoveCmd (void);
Boolean	    UnDoMoveCmd (void);
void	    DoDeleteCmd (void);
void        ClearCommand (CmdInfoRec far* info);


	    /***** UTIL.C *****/

void PASCAL DrawListBox (Byte *title, Word top, Word left, Word height, Word width, Boolean gap);
void PASCAL DrawTitledSingleBox (Byte *title, Word top, Word left, Word height, Word width);
int  PASCAL ButtonFind (ButtonsRec *b);
Boolean	    CharIsFile (Byte c);
void	    ConvertDescrStrToCharAttr (Word far *d, Byte *s, Word n);
void	    ConvertCharAttrToDescrStr (Byte *d, Word far *s, Word n);
void	    NameFixEnd (Byte *name);

void PASCAL StringStripTrailingSpaces (Byte *string);


	    /***** LAUNCH.C *****/

void	    Launch (Byte *cmd);
Boolean     InitCommandComPath (Byte *commandComPath);
void	    DoDOSCmd (void);
void	    DoDOSShell (void);

	    /***** CMDIO.C *****/

void	    ReadCommands (Boolean firstLoad);
void	    WriteCommands (void);


	    /***** SORT.C *****/

void	    DoSortCommands (Boolean alpha);
void	    MoveOffSkipItem (void);
void	    SortCommands (Boolean alpha, Word n);
void	    InitTopicRadios (void);
Byte	    CountTopics (void);
void	    InitTopicList (Boolean add, Byte far* name);
void	    RecalcTopics (void);


	    /***** CMDALLOC.C *****/

void	    InitCmdBuf (void);
CmdInfoRec far *AllocCmdBuf (void);
void	    FreeCmdBuf (CmdInfoRec huge *r);


	    /***** OPTIONS.C *****/

void	    DoOptions (void);


	    /***** CONFIG.C *****/
void DoExecConfig (int item);
void InitConfigMenu (void);


	    /****** ADVISE.C *******/
void  DoAdvise(void);

