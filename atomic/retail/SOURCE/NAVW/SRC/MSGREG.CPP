/* Copyright 1990 Symantec, Peter Norton Computing Group                */
/************************************************************************/
/*                                                                      */
/* $Header:   S:/NAVW/VCS/msgreg.cpv   1.2   13 Mar 1998 15:30:38   TCASHIN  $ */
/*                                                                      */
/* Description:                                                         */
/*      This file contains the routines necessary for managing the      */
/*      the registered windows messages.                                */
/*                                                                      */
/*      It is recommeneded this code be in a nondiscarable segment      */
/*                                                                      */
/*                                                                      */
/* See Also:                                                            */
/*                                                                      */
/************************************************************************/
// $Log:   S:/NAVW/VCS/msgreg.cpv  $
// 
//    Rev 1.2   13 Mar 1998 15:30:38   TCASHIN
// Added the UWM_DISPLAY_OPTIONS message, sent from AP to NAVW to
// put up the options dialog.
// 
//    Rev 1.1   27 Mar 1997 15:49:06   DALLEE
// Removed old unused NDW drag drop messages.
//************************************************************************

#include "platform.h"
#include "msgreg.h"
#include "ctsn.h"                       // Required by navapcom.h
#include "navapcom.h"

static BOOL NEAR PASCAL SetUpRegStruct(void);

static WORD wTrans[TOTAL_REG_MESSAGES+1][2];
static BOOL bInitialized = FALSE;


WORD SYM_EXPORT WINAPI TranslateRegisteredMessage(WORD msg)
{
    /* Take a registered message, return the define */

    register int x;

    if(!bInitialized)
        bInitialized = SetUpRegStruct();

    for(x=0;x<TOTAL_REG_MESSAGES;x++)
        {
        if(wTrans[x][0] == msg)
            {
            return(wTrans[x][1]);
            }
        }

                                        // Not translated to a defined
                                        // message, so let it go through
                                        // as is.  One CANNOT assume that
                                        // all registered messages were
                                        // handled by this mechanism.  Fixes
                                        // defect 35846.
    return (msg);
}


WORD SYM_EXPORT WINAPI MessageToRegistered(WORD msg)
{
    /* Take a define, return the registered message */

    register int x;

    if(!bInitialized)
        bInitialized = SetUpRegStruct();

    for(x=0;x<TOTAL_REG_MESSAGES;x++)
        if(wTrans[x][1] == msg)
            break;

    return(wTrans[x][0]);
}


static BOOL NEAR PASCAL SetUpRegStruct(void)
{
    if(bInitialized)
        return(TRUE);

    wTrans[0][0] = RegisterWindowMessage("UWM_AGENT_CLOSE");
    wTrans[0][1] = UWM_AGENT_CLOSE;

    wTrans[1][0] = RegisterWindowMessage("UWM_AGENT_SAVE");
    wTrans[1][1] = UWM_AGENT_SAVE;

    wTrans[2][0] = RegisterWindowMessage("UWM_INI_CHANGE");
    wTrans[2][1] = UWM_INI_CHANGE;

    wTrans[3][0] = RegisterWindowMessage ( NAVAP_ACTIVATING_MESSAGE );
    wTrans[3][1] = UWM_NAVAP_ACTIVATING;

    wTrans[4][0] = RegisterWindowMessage ("UWM_DISPLAY_OPTIONS");
    wTrans[4][1] = UWM_NAVAP_ACTIVATING;

    /* Must be last message registered */
    /* Add any new messages before here */
    wTrans[TOTAL_REG_MESSAGES][0] = RegisterWindowMessage("UWM_NOP");
    wTrans[TOTAL_REG_MESSAGES][1] = UWM_NOP;

    return(TRUE);
}

