//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/WHILE.CPv   1.0   30 Jun 1997 16:15:52   DCHI  $
//
// Description:
//  Main Word 6.0/7.0/95 emulation environment while structure
//  emulation functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/WHILE.CPv  $
// 
//    Rev 1.0   30 Jun 1997 16:15:52   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "wd7api.h"
#include "wd7env.h"

//*************************************************************************
//
// void StateWHILE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the esSTATEMENT state when an eTOKEN_WHILE
//  is seen.  If the substate is not essSKIP, the state sets up the
//  WHILE control with the location of the conditional, which is at
//  the current IP.  Also, the state sets up for the first evaluation
//  of the conditional with a transition to the esEXPR state, after
//  which the state will be esWHILE_COND to evaluate the result.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateWHILE
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esWHILE);

    // Set up the current location as the location of the WHILE

    if (SubStateTop(lpstEnv) != essSKIP)
    {
        if (ControlCreateControl(lpstEnv,ectWHILE) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }
    }

    StateNew(lpstEnv,esWHILE_COND);

    // Evaluate the expression for the first time

    StatePush(lpstEnv,esEXPR);
    if (ControlCreateControl(lpstEnv,ectEXPR) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }
    if (SubStateTop(lpstEnv) != essSKIP)
    {
        if (EvalPushBegin(lpstEnv) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }
    }

    RedoToken(lpstEnv);
}


//*************************************************************************
//
// void StateWHILE_COND()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the evaluation of the expression after
//  the esWHILE state.
//
//  If the substate is not essSKIP, the state checks the result of the
//  expression evaluation.  If the result is FALSE, the state destroys
//  the WHILE control and sets the substate to essSKIP to skip
//  the body of the WHILE.
//
//  The state transitions to esWHILE_BODY in all cases.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateWHILE_COND
(
    LPENV       lpstEnv
)
{
    BOOL        bResult;

    assert(StateTop(lpstEnv) == esWHILE_COND);

    // Evaluate the condition

    if (SubStateTop(lpstEnv) != essSKIP)
    {
        // Evaluate the top operand

        if (EvalGetAndPopTopAsBoolean(lpstEnv,
                                      &bResult) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }

        if (bResult == FALSE)
        {
            // Skip WHILE_BODY

            SubStateNew(lpstEnv,essSKIP);

            // Destroy the WHILE control

            ControlDestroyTopControl(lpstEnv);
        }
    }

    StateNew(lpstEnv,esWHILE_BODY);
}


//*************************************************************************
//
// void StateWHILE_BODY()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the test on the conditional expression
//  result during the esWHILE_COND state.
//
//  The state executes the statements in the body of a WHILE structure.
//  If the state encounters an eTOKEN_WEND, the state transitions
//  to esWHILE_WEND to prepare for reevaluation of the condition.
//  An eTOKEN_NEXT or an eTOKEN_END will end the execution of the
//  WHILE and return control to the waiting state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateWHILE_BODY
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esWHILE_BODY);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_WEND:
            StateNew(lpstEnv,esWHILE_WEND);
            RedoToken(lpstEnv);
            break;

        case eTOKEN_NEXT:
        case eTOKEN_END:
            // pop (esWHILE_BODY)

            if (SubStateTop(lpstEnv) != essSKIP)
            {
                ControlDestroyTopControl(lpstEnv);
            }

            StatePop(lpstEnv);
            RedoToken(lpstEnv);
            break;

        case eTOKEN_NEWLINE:
            // Blank line
            break;

        default:
            StatePush(lpstEnv,esSTATEMENT);
            RedoToken(lpstEnv);
            break;
    }
}


//*************************************************************************
//
// void StateWHILE_WEND()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an eTOKEN_WEND is seen during the
//  esWHILE_BODY state.
//
//  If the state is essSKIP, then the WHILE state is popped.
//  Otherwise, the next IP is set to the beginning of the conditional
//  expression and the state transitions to the expression evaluation
//  state, after which the state will be esWHILE_COND to evaluate
//  the current state of the condition.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateWHILE_WEND
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esWHILE_WEND);

    // if skip then don't go back to condition

    if (SubStateTop(lpstEnv) == essSKIP)
        StatePop(lpstEnv);
    else
    {
        // Set IP to the while condition

        StateNew(lpstEnv,esWHILE_COND);
        lpstEnv->dwNextIP = ((LPWHILE)ControlTop(lpstEnv))->dwCondIP;

        StatePush(lpstEnv,esEXPR);
        if (ControlCreateControl(lpstEnv,ectEXPR) == FALSE)
            StateNew(lpstEnv,esERROR);
        else
        if (EvalPushBegin(lpstEnv) == FALSE)
            StateNew(lpstEnv,esERROR);
    }
}


