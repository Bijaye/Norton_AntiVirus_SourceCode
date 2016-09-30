//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/GOTO.CPv   1.0   30 Jun 1997 16:15:32   DCHI  $
//
// Description:
//  Macro emulation environment goto structure emulation functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/GOTO.CPv  $
// 
//    Rev 1.0   30 Jun 1997 16:15:32   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "wd7api.h"
#include "wd7env.h"
#include "wbutil.h"

//*************************************************************************
//
// void StateGOTO()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the esSTATEMENT state when an eTOKEN_GOTO
//  is seen.  This state checks the next token.  If the token is an
//  eTOKEN_IDENTIFIER, the state transitions to the esSTATE_IDENTIFIER0
//  state to begin searching forward for the identifier label.  If the
//  token is an eTOKEN_WORD_VALUE, the state transitions to the
//  esSTATE_WORD0 state to begin searching forward for the word label.
//  Otherwise, the state transitions to the esERROR state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateGOTO
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esGOTO);

    if (SubStateTop(lpstEnv) == essSKIP)
    {
        // pop (esGOTO)

        StatePop(lpstEnv);
        return;
    }

    switch (lpstEnv->eToken)
    {
        case eTOKEN_IDENTIFIER:
            // See if it is okay to go to this label first

            if (ModuleEnvGotoLabelOkay(lpstEnv,
                                       lpstEnv->uTokenData.Identifier.byLen,
                                       lpstEnv->uTokenData.Identifier.
                                           lpby) == FALSE)
            {
                // Don't do the goto

                StatePop(lpstEnv);
                return;
            }

            if (ControlCreateControl(lpstEnv,
                                     ectLABEL_IDENTIFIER) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }
            StateNew(lpstEnv,esGOTO_IDENTIFIER0);
            break;

        case eTOKEN_WORD_VALUE:
            if (ControlCreateControl(lpstEnv,
                                     ectLABEL_WORD) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }
            StateNew(lpstEnv,esGOTO_WORD0);
            break;

        default:
            // Just ignore it and continue
            StatePop(lpstEnv);
            //StatePush(lpstEnv,esERROR);
            break;
    }
}


//*************************************************************************
//
// void StateGOTO_IDENTIFIER0()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the esGOTO state for an identifier label.
//  The state continues searching for the given label until it is seen
//  or the end of the routine is seen, signaled by an eTOKEN_SUB or
//  eTOKEN_FUNCTION, in which case, the state transitions to the
//  esGOTO_IDENTIFIER1 state which searches for the label starting from
//  the beginning of the routine and ending from the start of the search
//  that began with this state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateGOTO_IDENTIFIER0
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esGOTO_IDENTIFIER0);
    assert(SubStateTop(lpstEnv) != essSKIP);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_NEWLINE:
            StatePush(lpstEnv,esGOTO_CHECK_FOR_SPECIAL);
            break;

        case eTOKEN_LABEL:
        {
            // Is this the label?

            if (PascalStrIEq(((LPLABEL_IDENTIFIER)ControlTop(lpstEnv))->lpbyLabel,
                             lpstEnv->uTokenData.String.lpby) == FALSE)
                return;

            // Found it

            ControlDestroyTopControl(lpstEnv);
            StatePop(lpstEnv);
            break;
        }

        case eTOKEN_SUB:
        case eTOKEN_FUNCTION:
            // End routine?
            lpstEnv->dwNextIP = lpstEnv->dwRoutineStartIP;
            StateNew(lpstEnv,esGOTO_IDENTIFIER1);
            break;

        default:
            break;
    }
}


//*************************************************************************
//
// void StateGOTO_IDENTIFIER1()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an eTOKEN_SUB or eTOKEN_FUNCTION is seen
//  in the esGOTO_IDENTIFIER0 state, signaling the end of the routine.
//  In the esGOTO_IDENTIFIER0 state, the label is searched forward
//  until the end of the routine.  This state is used to search
//  from the beginning of the routine up until where the search for
//  the label started in the esGOTO_IDENTIFIER0 state.  If the label is
//  seen before the stopping location, the state is popped.  Otherwise,
//  if the state is not seen, the state transitions to the esERROR state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateGOTO_IDENTIFIER1
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esGOTO_IDENTIFIER1);
    assert(SubStateTop(lpstEnv) != essSKIP);

    if (lpstEnv->eToken == eTOKEN_NEWLINE)
    {
        StatePush(lpstEnv,esGOTO_CHECK_FOR_SPECIAL);
    }
    else
    if (lpstEnv->eToken == eTOKEN_LABEL)
    {
        // Is this the label?

        if (PascalStrIEq(((LPLABEL_IDENTIFIER)ControlTop(lpstEnv))->lpbyLabel,
                         lpstEnv->uTokenData.String.lpby) == FALSE)
            return;

        // Found it

        ControlDestroyTopControl(lpstEnv);
        StatePop(lpstEnv);
    }
    else
    if (lpstEnv->dwIP >=
        ((LPLABEL_IDENTIFIER)ControlTop(lpstEnv))->dwStartIP)
    {
        // Could not find label, just continue

        StatePop(lpstEnv);
        //ControlDestroyTopControl(lpstEnv);
        //StateNew(lpstEnv,esERROR);
    }
}


//*************************************************************************
//
// void StateGOTO_WORD0()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the esGOTO state for a word label.  The
//  state continues searching for the given label until it is seen
//  or the end of the routine is seen, signaled by an eTOKEN_SUB or
//  eTOKEN_FUNCTION, in which case, the state transitions to the
//  esGOTO_WORD1 state which searches for the label starting from the
//  beginning of the routine and ending from the start of the search
//  that began with this state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateGOTO_WORD0
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esGOTO_WORD0);
    assert(SubStateTop(lpstEnv) != essSKIP);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_NEWLINE:
            StatePush(lpstEnv,esGOTO_CHECK_FOR_SPECIAL);
            break;

        case eTOKEN_WORD_LABEL:
        {
            // Is this the label?

            if (((LPLABEL_WORD)ControlTop(lpstEnv))->wLabel !=
                lpstEnv->uTokenData.wValue)
                return;

            // Found it

            ControlDestroyTopControl(lpstEnv);
            StatePop(lpstEnv);
            break;
        }

        case eTOKEN_SUB:
        case eTOKEN_FUNCTION:
            // End routine?
            lpstEnv->dwNextIP = lpstEnv->dwRoutineStartIP;
            StateNew(lpstEnv,esGOTO_WORD1);
            break;

        default:
            break;
    }
}


//*************************************************************************
//
// void StateGOTO_WORD1()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an eTOKEN_SUB or eTOKEN_FUNCTION is seen
//  in the esGOTO_WORD0 state, signaling the end of the routine.
//  In the esGOTO_WORD0 state, the word label is searched forward
//  until the end of the routine.  This state is used to search
//  from the beginning of the routine up until where the search for
//  the label started in the esGOTO_WORD0 state.  If the label is seen
//  before the stopping location, the state is popped.  Otherwise,
//  if the state is not seen, the state transitions to the esERROR state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateGOTO_WORD1
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esGOTO_WORD1);
    assert(SubStateTop(lpstEnv) != essSKIP);

    if (lpstEnv->eToken == eTOKEN_NEWLINE)
    {
        StatePush(lpstEnv,esGOTO_CHECK_FOR_SPECIAL);
    }
    else
    if (lpstEnv->eToken == eTOKEN_WORD_LABEL)
    {
        // Is this the label?

        if (((LPLABEL_WORD)ControlTop(lpstEnv))->wLabel !=
            lpstEnv->uTokenData.wValue)
            return;

        // Found it

        ControlDestroyTopControl(lpstEnv);
        StatePop(lpstEnv);
    }
    else
    if (lpstEnv->dwIP >=
        ((LPLABEL_WORD)ControlTop(lpstEnv))->dwStartIP)
    {
        // Could not find label, just continue

        StatePop(lpstEnv);
        //ControlDestroyTopControl(lpstEnv);
        //StateNew(lpstEnv,esERROR);
    }
}


//*************************************************************************
//
// void StateGOTO_CHECK_FOR_SPECIAL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs during a search for a goto label.  It
//  transitions to the statement state if a macrocopy or organizer
//  routine is invoked.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateGOTO_CHECK_FOR_SPECIAL
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esGOTO_CHECK_FOR_SPECIAL);
    assert(SubStateTop(lpstEnv) != essSKIP);

    RedoToken(lpstEnv);

    if (lpstEnv->eToken == eTOKEN_FUNCTION_VALUE)
    {
        // Always do macrocopies and organizer operations

        if (lpstEnv->uTokenData.eWDCMD == eWDCMD_MacroCopy ||
            lpstEnv->uTokenData.eWDCMD == eWDCMD_Organizer)
        {
            StateNew(lpstEnv,esSTATEMENT);
            return;
        }
    }

    StatePop(lpstEnv);
}

