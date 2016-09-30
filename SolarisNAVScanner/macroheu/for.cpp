//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/FOR.CPv   1.0   30 Jun 1997 16:15:30   DCHI  $
//
// Description:
//  Macro emulation environment for structure emulation functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/FOR.CPv  $
// 
//    Rev 1.0   30 Jun 1997 16:15:30   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "wd7api.h"
#include "wd7env.h"

//*************************************************************************
//
// void StateFOR_IDENTIFIER()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the esSTATEMENT state when an eTOKEN_FOR
//  is seen.
//
//  If the current token is not eTOKEN_IDENTIFIER, the state
//  transitions to esERROR.
//
//  If the current substate is essSKIP, the state immediately
//  transitions to esFOR_EQUAL.
//
//  Otherwise, if the identifier doesn't yet exist, the state
//  determines whether it is a global or local variable and creates
//  the variable.
//
//  A FOR control is created and initialized with the counter
//  variable and the state transitions to esFOR_EQUAL.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateFOR_IDENTIFIER
(
    LPENV       lpstEnv
)
{
    LPVAR       lpstVar;

    assert(StateTop(lpstEnv) == esFOR_IDENTIFIER);

    if (lpstEnv->eToken != eTOKEN_IDENTIFIER)
    {
        // Expected an identifier but did not see one

        StateNew(lpstEnv,esERROR);
        return;
    }

    if (SubStateTop(lpstEnv) == essSKIP)
    {
        StateNew(lpstEnv,esFOR_EQUAL);
        return;
    }

    lpstVar = VarGet(lpstEnv,
                     lpstEnv->uTokenData.Identifier.byLen,
                     lpstEnv->uTokenData.Identifier.lpby);

    if (lpstVar == NULL)
    {
        // Determine whether it is a local or a global variable

        if (ModuleEnvVariableIsGlobal(lpstEnv,
                                      lpstEnv->uTokenData.Identifier.byLen,
                                      lpstEnv->uTokenData.Identifier.lpby) ==
            FALSE)
        {
            // Create a local variable

            lpstVar = VarAlloc(lpstEnv,
                               FALSE,
                               FALSE,
                               lpstEnv->uTokenData.Identifier.byLen,
                               lpstEnv->uTokenData.Identifier.lpby);
        }
        else
        {
            // Create a global variable

            lpstVar = VarAlloc(lpstEnv,
                               TRUE,
                               FALSE,
                               lpstEnv->uTokenData.Identifier.byLen,
                               lpstEnv->uTokenData.Identifier.lpby);
        }

        if (lpstVar == NULL)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }
    }

    StateNew(lpstEnv,esFOR_EQUAL);
    if (ControlCreateControl(lpstEnv,ectFOR) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    ((LPFOR)ControlTop(lpstEnv))->lpstVar = lpstVar;
}


//*************************************************************************
//
// void StateFOR_EQUAL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the esFOR_IDENTIFIER state.
//
//  If the current token is not eTOKEN_EQ, the state
//  transitions to esERROR.
//
//  Otherwise, the state sets up for a transition to the expression
//  evaluation state after which the state will be esFOR_START
//  for noting the starting counter value.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateFOR_EQUAL
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esFOR_EQUAL);

    if (lpstEnv->eToken == eTOKEN_EQ)
    {
        StateNew(lpstEnv,esFOR_START);
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
    }
    else
    {
        StateNew(lpstEnv,esERROR);
    }
}


//*************************************************************************
//
// void StateFOR_START()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the evaluation of the expression that
//  evaluates to the start count.
//
//  If the current token is not eTOKEN_TO, the state
//  transitions to esERROR.
//
//  Otherwise, the state sets up for a transition to the expression
//  evaluation state after which the state will be esFOR_END
//  for noting the ending counter value.
//
//  If the substate is not essSKIP, the state initializes the
//  counter variable with the starting value.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateFOR_START
(
    LPENV       lpstEnv
)
{
    ETYPE_T     eType;
    LPVOID      lpvData;

    assert(StateTop(lpstEnv) == esFOR_START);

    // The top operand should contain the starting value

    if (lpstEnv->eToken != eTOKEN_TO)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    if (SubStateTop(lpstEnv) == essSKIP)
    {
        StateNew(lpstEnv,esFOR_END);
        StatePush(lpstEnv,esEXPR);
        if (ControlCreateControl(lpstEnv,ectEXPR) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }
        return;
    }

    if (EvalQueryTopOperand(lpstEnv,
                            &eType,
                            &lpvData) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    if (eType != eTYPE_LONG)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Remember the starting value

    ((LPFOR)ControlTop(lpstEnv))->lStart = *(LPLONG)lpvData;

    // Set the value

    if (VarSet(lpstEnv,
               ((LPFOR)ControlTop(lpstEnv))->lpstVar,
               eType,
               lpvData) == NULL)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // Free the operand

    if (EvalPopTopOperand(lpstEnv) == FALSE)
    {
        StatePush(lpstEnv,esERROR);
        return;
    }

    StateNew(lpstEnv,esFOR_END);
    StatePush(lpstEnv,esEXPR);
    if (ControlCreateControl(lpstEnv,ectEXPR) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }
    if (EvalPushBegin(lpstEnv) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }
}


//*************************************************************************
//
// void StateFOR_END()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the evaluation of the expression that
//  evaluates to the end count.
//
//  If the current token is eTOKEN_STEP, the state transitions
//  to the expression evaluation state, followed by the esFOR_STEP
//  state to store the step.
//
//  Otherwise, the step is assumed to be one and a transition is
//  made to the esFOR_BODY state.  If it is the case that the
//  starting value is greater than the ending value, then the
//  substate is set to essSKIP to skip the evaluation of the
//  body.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateFOR_END
(
    LPENV       lpstEnv
)
{
    ETYPE_T     eType;
    LPVOID      lpvData;

    assert(StateTop(lpstEnv) == esFOR_END);

    if (SubStateTop(lpstEnv) != essSKIP)
    {
        // The top operand should contain the ending value

        if (EvalQueryTopOperand(lpstEnv,
                                &eType,
                                &lpvData) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }

        if (eType != eTYPE_LONG)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }

        // Set the value

        ((LPFOR)ControlTop(lpstEnv))->lEnd = *(LPLONG)lpvData;

        // Free the operand

        if (EvalPopTopOperand(lpstEnv) == FALSE)
        {
            StatePush(lpstEnv,esERROR);
            return;
        }
    }

    switch (lpstEnv->eToken)
    {
        case eTOKEN_STEP:
            StateNew(lpstEnv,esFOR_STEP);
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
            break;

        case eTOKEN_NEWLINE:
        case eTOKEN_LABEL:
        case eTOKEN_WORD_LABEL:
            // Store end

            StateNew(lpstEnv,esFOR_BODY);

            if (SubStateTop(lpstEnv) != essSKIP)
            {
                LPFOR   lpstFor;

                // If start is greater than end, then don't do it

                lpstFor = ((LPFOR)ControlTop(lpstEnv));
                if (lpstFor->lStart > lpstFor->lEnd)
                {
                    ControlDestroyTopControl(lpstEnv);
                    SubStateNew(lpstEnv,essSKIP);
                }
                else
                    lpstFor->dwBodyIP = lpstEnv->dwNextIP;
            }

            if (lpstEnv->eToken != eTOKEN_NEWLINE)
                RedoToken(lpstEnv);

            break;

        default:
            StateNew(lpstEnv,esERROR);
            break;
    }
}


//*************************************************************************
//
// void StateFOR_STEP()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the evaluation of the expression that
//  evaluates to the step value.
//
//  If the substate is essSKIP, the state immediately transitions
//  to the esFOR_BODY state.
//
//  If the substate is not essSKIP, the state initializes the
//  step field of the FOR control with the step value.
//
//  A transition is made to the esFOR_BODY state.  If it is the case
//  that the starting value is greater than the ending value and the
//  step is not negative or it is the case that the reverse is true,
//  then the substate is set to essSKIP to skip the evaluation of the
//  body.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateFOR_STEP
(
    LPENV       lpstEnv
)
{
    ETYPE_T     eType;
    LPVOID      lpvData;
    LPFOR       lpstFor;
    long        lStep;

    assert(StateTop(lpstEnv) == esFOR_STEP);

    if (lpstEnv->eToken != eTOKEN_NEWLINE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    // The top operand should contain the starting value

    if (SubStateTop(lpstEnv) == essSKIP)
    {
        StateNew(lpstEnv,esFOR_BODY);
        return;
    }

    if (EvalQueryTopOperand(lpstEnv,
                            &eType,
                            &lpvData) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    if (eType != eTYPE_LONG)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }   

    lpstFor = ((LPFOR)ControlTop(lpstEnv));
    lStep = *(LPLONG)lpvData;
    if (lStep == 0)
        lStep = 1;

    // If start is greater than end and step is not negative or vice versa,
    //  don't do it

    if (lpstFor->lStart > lpstFor->lEnd && lStep >= 0 ||
        lpstFor->lStart < lpstFor->lEnd && lStep < 0)
    {
        ControlDestroyTopControl(lpstEnv);
        SubStateNew(lpstEnv,essSKIP);
    }
    else
    {
        // Set the value

        lpstFor->lStep = *(LPLONG)lpvData;
        lpstFor->dwBodyIP = lpstEnv->dwNextIP;
    }

    // Free the operand

    if (EvalPopTopOperand(lpstEnv) == FALSE)
    {
        StatePush(lpstEnv,esERROR);
        return;
    }

    // Do the body

    StateNew(lpstEnv,esFOR_BODY);
    RedoToken(lpstEnv);
}


//*************************************************************************
//
// void StateFOR_BODY()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after either the esFOR_END, esFOR_STEP, or
//  esFOR_NEXT state.
//
//  If the substate is essSKIP, each statement in the body is skipped,
//  and executed otherwise.
//
//  If the state encounters an eTOKEN_NEXT, the state transitions
//  to the esFOR_NEXT state.  An eTOKEN_WEND or eTOKEN_END causes a
//  pop of the state, a redo of the token, and thus a transition
//  to the waiting state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateFOR_BODY
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esFOR_BODY);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_NEXT:
            StateNew(lpstEnv,esFOR_NEXT);
            break;

        case eTOKEN_WEND:
        case eTOKEN_END:
            // pop (esFOR_BODY)

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
// void StateFOR_NEXT()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after an eTOKEN_NEXT is seen in the esFOR_BODY
//  state.
//
//  If the substate is essSKIP, the state immediately transitions to the
//  esSKIP_LINE state.
//
//  Otherwise, the state compares the current value of the counter
//  variable against the ending value of the FOR control and transitions
//  back to the beginning of the FOR body if the ending limit has not
//  been reached.  Otherwise the state transitions to the esSKIP_LINE
//  state, after which the waiting state will gain control.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateFOR_NEXT
(
    LPENV       lpstEnv
)
{
    LPFOR       lpstFor;

    assert(StateTop(lpstEnv) == esFOR_NEXT);

    if (SubStateTop(lpstEnv) == essSKIP)
    {
        // pop (esFOR_NEXT)

        StateNew(lpstEnv,esSKIP_LINE);
        RedoToken(lpstEnv);
        return;
    }

    // Find the most recent FOR

    lpstFor = (LPFOR)ControlTop(lpstEnv);

    // Check current value

    lpstFor->lpstVar->uData.lValue += lpstFor->lStep;
    if (lpstFor->lStep < 0 &&
        lpstFor->lpstVar->uData.lValue < lpstFor->lEnd ||
        lpstFor->lStep > 0 &&
        lpstFor->lpstVar->uData.lValue > lpstFor->lEnd)
    {
        ControlDestroyTopControl(lpstEnv);

        // pop (esFOR_NEXT)

        StateNew(lpstEnv,esSKIP_LINE);
        RedoToken(lpstEnv);
    }
    else
    {
        // Jump back to beginning of FOR_BODY

        lpstEnv->dwNextIP = lpstFor->dwBodyIP;
        StateNew(lpstEnv,esFOR_BODY);
    }
}


