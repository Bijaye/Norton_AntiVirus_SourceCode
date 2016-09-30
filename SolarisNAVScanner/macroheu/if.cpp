//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/IF.CPv   1.0   30 Jun 1997 16:15:38   DCHI  $
//
// Description:
//  Macro emulation environment if structure emulation functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/IF.CPv  $
// 
//    Rev 1.0   30 Jun 1997 16:15:38   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "wd7api.h"
#include "wd7env.h"

//*************************************************************************
//
// void StateIF_COND()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the esSTATEMENT state when an eTOKEN_IF
//  is seen and after the evaluation of the expression following it.
//
//  If the next token is not eTOKEN_THEN, then the state transitions
//  to esERROR.
//
//  Otherwise, if the substate is not essSKIP, the state checks the
//  value of the just evaluated expression.  If the value is FALSE,
//  the state sets the substate to essFALSE, and essTRUE otherwise.
//  The next state will be esIF_THEN.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateIF_COND
(
    LPENV       lpstEnv
)
{
    BOOL        bResult;

    assert(StateTop(lpstEnv) == esIF_COND);

    if (lpstEnv->eToken == eTOKEN_THEN)
    {
        if (SubStateTop(lpstEnv) != essSKIP)
        {
            // Evaluate the top operand

            if (EvalGetAndPopTopAsBoolean(lpstEnv,
                                          &bResult) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }

            StateNew(lpstEnv,esIF_THEN);

            if (bResult == FALSE)
            {
                // Skip THEN

                SubStateNew(lpstEnv,essFALSE);
            }
            else
            {
                // Do THEN

                SubStateNew(lpstEnv,essTRUE);
            }
        }
        else
            StateNew(lpstEnv,esIF_THEN);
    }
    else
    {
        // Syntax error

        StateNew(lpstEnv,esERROR);
    }
}


//*************************************************************************
//
// void StateIF_THEN()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the esIF_THEN state when an eTOKEN_THEN
//  is seen.
//
//  This state determines whether the statement is a single line
//  if...then structure or a multi-line if...then structure.  A
//  newline or a label signals a multi-line if...then.  In this
//  case, the state transitions to esTHEN_MULTIPLE for evaluating
//  then body.
//
//  Otherwise, for a single-line if...then, the state will transition
//  to the esTHEN_SINGLE state after the evaluation of the
//  single line statement.  The single line statement of the THEN
//  part will be evaluated only if the result of the conditional
//  was true and the state is not essSKIP.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateIF_THEN
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esIF_THEN);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_NEWLINE:
        case eTOKEN_LABEL:
        case eTOKEN_WORD_LABEL:
            // Multiple statement THEN

            StateNew(lpstEnv,esTHEN_MULTIPLE);
            break;

        default:
        {
            ESUBSTATE_T eSubState;

            // Only expect a single statement

            StateNew(lpstEnv,esTHEN_SINGLE);

            eSubState = SubStateTop(lpstEnv);

            StatePush(lpstEnv,esSTATEMENT);

            switch (eSubState)
            {
                case essFALSE:
                case essSKIP:
                    SubStateNew(lpstEnv,essSKIP);
                    break;

                default:
                    SubStateNew(lpstEnv,essDO);
                    break;
            }

            // redo token

            RedoToken(lpstEnv);
            break;
        }
    }
}


//*************************************************************************
//
// void StateTHEN_SINGLE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the evaluation of the single-line statement
//  after the esIF_THEN state.
//
//  If the state sees an eTOKEN_ELSE, then the state will transition
//  to the esELSE_SINGLE state after the evaluation of the
//  single line statement.  The single line statement of the ELSE
//  part will be evaluated only if the result of the conditional
//  was false and the state is not essSKIP.
//
//  Otherwise, the state transitions out of esTHEN_SINGLE with a pop.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateTHEN_SINGLE
(
    LPENV       lpstEnv
)
{
    ESUBSTATE_T eSubState;

    assert(StateTop(lpstEnv) == esTHEN_SINGLE);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_NEWLINE:
        case eTOKEN_LABEL:
        case eTOKEN_WORD_LABEL:
        case eTOKEN_COLON:
            // pop (esTHEN_SINGLE)

            StatePop(lpstEnv);

            // End of single THEN statement

            break;

        case eTOKEN_ELSE:
            StateNew(lpstEnv,esELSE_SINGLE);

            eSubState = SubStateTop(lpstEnv);

            StatePush(lpstEnv,esSTATEMENT);

            switch (eSubState)
            {
                case essTRUE:
                case essSKIP:
                    SubStateNew(lpstEnv,essSKIP);
                    break;

                default:
                    SubStateNew(lpstEnv,essDO);
                    break;
            }

            break;

        default:
            StatePop(lpstEnv);
            RedoToken(lpstEnv);
            break;
    }
}


//*************************************************************************
//
// void StateELSE_SINGLE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the evaluation of the single-line statement
//  after the esELSE_SINGLE state.
//
//  The state transitions out of esELSE_SINGLE with a pop.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateELSE_SINGLE
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esELSE_SINGLE);

    // End of if statement

    // pop (esELSE_SINGLE)

    StatePop(lpstEnv);
}


//*************************************************************************
//
// void StateTHEN_MULTIPLE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs when a multiple-line statement is signaled
//  in the esIF_THEN state.  Each statement in the body of the THEN
//  will be evaluated if the substate is essTRUE.
//
//  If the state encounters an esTOKEN_ELSE, the state transitions
//  to esELSE_MULTIPLE.
//
//  If the state encounters an esTOKEN_ELSE_IF, the state transitions
//  to esEXPR which will transition to esIF_COND after the evaluation
//  of the conditional expression.
//
//  If the state encounters an esTOKEN_END_IF, the state exits with
//  a pop.
//
//  If the state encounters an esTOKEN_END, the state transitions
//  to esIF_THEN_END to check the type of end.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateTHEN_MULTIPLE
(
    LPENV       lpstEnv
)
{
    ESUBSTATE_T eSubState;

    assert(StateTop(lpstEnv) == esTHEN_MULTIPLE);

    eSubState = SubStateTop(lpstEnv);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_ELSE:
            StateNew(lpstEnv,esELSE_MULTIPLE);
            break;

        case eTOKEN_ELSE_IF:
            StateNew(lpstEnv,esIF_COND);
            if (eSubState != essSKIP)
            {
                SubStateNew(lpstEnv,essDO);
            }
            StatePush(lpstEnv,esEXPR);
            if (ControlCreateControl(lpstEnv,ectEXPR) == FALSE)
            {
                StateNew(lpstEnv,esERROR);
                return;
            }
            if (eSubState != essSKIP)
            {
                if (EvalPushBegin(lpstEnv) == FALSE)
                {
                    StateNew(lpstEnv,esERROR);
                    return;
                }
            }
            break;

        case eTOKEN_END_IF:
            // pop (esTHEN_MULTIPLE)

            StatePop(lpstEnv);
            break;

        case eTOKEN_END:
            StateNew(lpstEnv,esIF_THEN_END);
            break;

        case eTOKEN_NEWLINE:
            // Blank line
            break;

        default:
            StatePush(lpstEnv,esSTATEMENT);

            if (eSubState == essTRUE)
                SubStateNew(lpstEnv,essDO);
            else
                SubStateNew(lpstEnv,essSKIP);

            RedoToken(lpstEnv);

            break;
    }
}


//*************************************************************************
//
// void StateELSE_MULTIPLE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs when a multiple-line statement is signaled
//  in the esIF_THEN state and the THEN part has been completed.
//  Each statement in the body of the ELSE will be evaluated
//  if the substate is essFALSE.
//
//  If the state encounters an esTOKEN_END_IF, the state exits with
//  a pop.
//
//  If the state encounters an esTOKEN_END, the state transitions
//  to esIF_THEN_END to check the type of end.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateELSE_MULTIPLE
(
    LPENV       lpstEnv
)
{
    ESUBSTATE_T eSubState;

    assert(StateTop(lpstEnv) == esELSE_MULTIPLE);

    eSubState = SubStateTop(lpstEnv);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_END_IF:
            // pop (esELSE_MULTIPLE)

            StatePop(lpstEnv);
            break;

        case eTOKEN_END:
            StateNew(lpstEnv,esIF_THEN_END);
            break;

        case eTOKEN_NEWLINE:
            // Blank line
            break;

        default:
            StatePush(lpstEnv,esSTATEMENT);

            if (eSubState == essFALSE)
                SubStateNew(lpstEnv,essDO);
            else
                SubStateNew(lpstEnv,essSKIP);

            RedoToken(lpstEnv);

            break;
    }
}


//*************************************************************************
//
// void StateIF_THEN_END()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs when an eTOKEN_END is seen during a multiple-line
//  IF...THEN body execution.  The esIF_THEN_END state is popped and
//  if the current token is not eTOKEN_IF, the IP is reset back to the
//  IP of the eTOKEN_END, so that the END can be redone at the level
//  above.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateIF_THEN_END
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esIF_THEN_END);

    // pop (esIF_THEN_END)

    StatePop(lpstEnv);

    if (lpstEnv->eToken != eTOKEN_IF)
    {
        // Go back to the End

        lpstEnv->dwNextIP = lpstEnv->dwIP - 2;
    }
}


