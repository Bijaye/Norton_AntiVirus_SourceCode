//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/select.cpv   1.1   05 Aug 1997 18:16:44   DCHI  $
//
// Description:
//  Macro emulation environment select structure emulation functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/select.cpv  $
// 
//    Rev 1.1   05 Aug 1997 18:16:44   DCHI
// Fixed select problem and backward comparison for numeric args.
// 
//    Rev 1.0   30 Jun 1997 16:15:42   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "wd7api.h"
#include "wd7env.h"
#include "wbutil.h"

//*************************************************************************
//
// void StateSELECT()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the esSTATEMENT state when an eTOKEN_SELECT
//  is seen.  This state assumes that the next token is an eTOKEN_CASE.
//  The state sets up the next state for the evaluation of the expression
//  to match in the select case structure.  The state to transition to
//  after the evaluation of the expression is the esSELECT_CASE state
//  which will set up the internal control structure necessary to
//  correctly execute the Select Case structure.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateSELECT
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esSELECT);

    // Assume that we correctly get a CASE token

    StateNew(lpstEnv,esSELECT_CASE);
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


//*************************************************************************
//
// void StateSELECT_CASE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the evaluation of an expression as a result
//  of a transition from the esSELECT state.  If the substate is not
//  essSKIP, the state creates a select control structure containing
//  the value of the expression to match against in the cases that are
//  to follow.  The state transitions to the esSELECT_BODY state for
//  evaluation of the cases.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateSELECT_CASE
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esSELECT_CASE);

    if (SubStateTop(lpstEnv) != essSKIP)
    {
        ETYPE_T     eType;
        LPVOID      lpvData;

        // Get the value of the expression

        if (EvalQueryTopOperand(lpstEnv,
                                &eType,
                                &lpvData) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }

        // Create a control with the value

        if (ControlCreateSelectControl(lpstEnv,
                                       eType,
                                       lpvData) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }

        // Pop the top operand

        if (EvalPopTopOperand(lpstEnv) == FALSE)
        {
            StateNew(lpstEnv,esERROR);
            return;
        }
    }

    StateNew(lpstEnv,esSELECT_BODY);
}


//*************************************************************************
//
// void StateSELECT_BODY()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs after the esSELECT_CASE statement which sets up
//  the internal select control structure.  In this state, an eTOKEN_CASE
//  causes a transition to the esCASE state if the substate is not
//  essSKIP, so that the candidate case expression can be evaluated.
//  If the substate is essSKIP, then the rest of the line is skipped and
//  the state transitions to the esCASE_BODY state to skip the case body.
//
//  If the token is not eTOKEN_CASE, the state assumes that it is the end
//  of the Select Case structure, transitioning to the esSKIP_CASE_LINE
//  state to skip the rest of the case line.  The state following the
//  esSKIP_CASE_LINE state will be the esSELECT_END state for any
//  necessary clean up.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateSELECT_BODY
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esSELECT_BODY);

    if (lpstEnv->eToken == eTOKEN_CASE)
    {
        if (SubStateTop(lpstEnv) == essSKIP)
        {
            // Skip the rest of the line and then skip over the body

            StatePush(lpstEnv,esCASE_BODY);
            StatePush(lpstEnv,esSKIP_CASE_LINE);
        }
        else
        {
            StateNew(lpstEnv,esCASE);
        }
    }
    else
    {
        StateNew(lpstEnv,esSELECT_END);
    }
}


//*************************************************************************
//
// void StateSELECT_END()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs following the esSKIP_CASE_LINE state that
//  immediately follows the esSELECT_BODY state and occurs at the end of
//  a Select Case structure.  The state destroys the internal select
//  control structure and then pops the state to transition to the next
//  waiting state.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateSELECT_END
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esSELECT_END);

    if (ControlTop(lpstEnv) != NULL)
        ControlDestroyTopControl(lpstEnv);

    if (lpstEnv->eToken != eTOKEN_SELECT)
    {
        lpstEnv->dwNextIP = lpstEnv->dwIP;
    }
    else
    {
        // Skip the rest of the line and terminate the select

        RedoToken(lpstEnv);
        StateNew(lpstEnv,esSKIP_LINE);
    }
}


//*************************************************************************
//
// void StateCASE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs following the esSELECT_BODY state when an
//  eTOKEN_CASE is seen.  The token encountered in this state determines
//  what type of candidate case expression needs to be evaluated.
//
//  If the token is eTOKEN_IS, the state transitions to the esCASE_IS
//  state to evaluate a relational case.
//
//  If the token is eTOKEN_ELSE, then the case is taken to be true,
//  so the body of the case should be evaluated.  The rest of the case
//  line is skipped through a transition to the esSKIP_CASE_LINE state.
//  Following the esSKIP_CASE_LINE state, the transition is to the
//  esCASE_BODY state to evaluate the case body.  Following the execution
//  of the case body, the state will transition to the esCASE_FOUND_MATCH
//  state so that any other cases will be skipped.
//
//  Otherwise, the case to match is either a single value or a range.
//  Thus an expression needs to be evaluated.  The state transitions
//  to the expression evaluation state.  Following the expression
//  evaluation state, the state will transition to the esCASE_EXPR
//  state which will determine whether the case is a single value
//  match or a range to match.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateCASE
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esCASE);

    assert(SubStateTop(lpstEnv) != essSKIP);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_IS:
            StateNew(lpstEnv,esCASE_IS);
            break;

        case eTOKEN_ELSE:
            // Do the else case, skip the rest of the cases

            StateNew(lpstEnv,esCASE_FOUND_MATCH);

            // Skip the rest of the line and do the case

            StatePush(lpstEnv,esCASE_BODY);
            StatePush(lpstEnv,esSKIP_CASE_LINE);
            break;

        default:
            StateNew(lpstEnv,esCASE_EXPR);

            // Evaluate the expression

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
            RedoToken(lpstEnv);
            break;
    }
}


//*************************************************************************
//
// void StateCASE_EXPR()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs following the expression evaluation state after
//  the esCASE state.
//
//  If the next token is an eTOKEN_TO, then the expression is a range
//  match.  The result of the just evaluated expression is compared
//  against the value to match.  If the value is less than the just
//  evaluated lower bound, then comparison with the upper bound is
//  ignored.  If the value is greater than or equal to the just
//  evaluated lower bound, then the upper bound must be checked.  In
//  both cases the upper bound is evaluated and then the state will
//  transition to the esCASE_TO state to check the upper bound.
//
//  If the expression was a match, the next state is esSKIP_CASE_LINE
//  if the next token is not eTOKEN_NEWLINE or eTOKEN_COLON.
//  After skipping the rest of the case line, the state will transition
//  to the esCASE_BODY state to execute the body of this case.  Since
//  a matching case is found, the state after esCASE_BODY will be
//  esCASE_FOUND_MATCH to skip the rest of the cases.
//
//  Otherwise, if the expression was not a match, but the next token
//  is an eTOKEN_COMMA, the next part of the case is tested with a
//  transition to the esCASE state.  If the next token is not an
//  eTOKEN_COMMA, the body is skipped.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateCASE_EXPR
(
    LPENV       lpstEnv
)
{
    BOOL        bMatch;
    int         nCompare;
    ETYPE_T     eType;
    LPVOID      lpvData;
    LPSELECT    lpstSelect;

    assert(StateTop(lpstEnv) == esCASE_EXPR);
    assert(SubStateTop(lpstEnv) != essSKIP);

    lpstSelect = (LPSELECT)ControlTop(lpstEnv);

    // Get the value of the expression

    if (EvalQueryTopOperand(lpstEnv,
                            &eType,
                            &lpvData) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    if (eType == lpstSelect->eType)
    {
        // Check for a match

        bMatch = TRUE;
        switch (eType)
        {
            case eTYPE_LONG:
            {
                long lValue = *(LPLONG)lpvData;

                if (lValue == lpstSelect->uData.lValue)
                    nCompare = 0;
                else
                if (lValue < lpstSelect->uData.lValue)
                    nCompare = 1;
                else
                    nCompare = -1;
                break;
            }

            case eTYPE_STRING:
                nCompare = PascalStrCmp((LPBYTE)lpvData,
                                        lpstSelect->uData.lpbyStr);
                break;

            default:
                // No match
                bMatch = FALSE;
                break;
        }
    }
    else
        bMatch = FALSE;

    // Pop the top operand

    if (EvalPopTopOperand(lpstEnv) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    switch (lpstEnv->eToken)
    {
        case eTOKEN_TO:
            StateNew(lpstEnv,esCASE_TO);
            if (bMatch == TRUE && nCompare >= 0)
            {
                // Have to make sure that the value falls
                //  within the range

                lpstSelect->eCaseType = eCASE_TYPE_TO_CHECK;
            }
            else
            {
                // The value falls below the lower bound or the
                //  type is different

                lpstSelect->eCaseType = eCASE_TYPE_TO_NO_CHECK;
            }

            StateNew(lpstEnv,esCASE_EXPR);

            // Evaluate the expression

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
            break;

        case eTOKEN_COMMA:
            if (bMatch == TRUE && nCompare == 0)
            {
                // This case matches, skip the rest of the cases

                StateNew(lpstEnv,esCASE_FOUND_MATCH);

                // Skip the rest of the line and do the case

                StatePush(lpstEnv,esCASE_BODY);
                StatePush(lpstEnv,esSKIP_CASE_LINE);
            }
            else
            {
                // Multiple expressions

                StateNew(lpstEnv,esCASE);
            }
            break;

        default:
            if (bMatch == TRUE && nCompare == 0)
            {
                // This case matches, skip the rest of the cases

                StateNew(lpstEnv,esCASE_FOUND_MATCH);

                // Do the body

                StatePush(lpstEnv,esCASE_BODY);
            }
            else
            {
                // Skip the body

                StatePush(lpstEnv,esCASE_BODY);
                SubStateNew(lpstEnv,essSKIP);
            }
            if (lpstEnv->eToken != eTOKEN_NEWLINE &&
                lpstEnv->eToken != eTOKEN_COLON)
            {
                StatePush(lpstEnv,esSKIP_CASE_LINE);
                RedoToken(lpstEnv);
            }
            break;
    }
}


//*************************************************************************
//
// void StateCASE_TO()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs following the expression evaluation state after
//  an eTOKEN_TO is seen in the esCASE_EXPR state.
//
//  If the value to match was greater than or equal to the
//  beginning boundary checked in the esCASE_EXPR state, then the value
//  to match is checked in this state to see if it is less than or
//  equal to the just evaluated upper boundary.  If so, then this
//  case is assumed to be a match.
//
//  If the expression was a match, the next state is esSKIP_CASE_LINE
//  if the next token is not eTOKEN_NEWLINE or eTOKEN_COLON.
//  After skipping the rest of the case line, the state will transition
//  to the esCASE_BODY state to execute the body of this case.  Since
//  a matching case is found, the state after esCASE_BODY will be
//  esCASE_FOUND_MATCH to skip the rest of the cases.
//
//  Otherwise, if the expression was not a match, but the next token
//  is an eTOKEN_COMMA, the next part of the case is tested with a
//  transition to the esCASE state.  If the next token is not an
//  eTOKEN_COMMA, the body is skipped.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateCASE_TO
(
    LPENV       lpstEnv
)
{
    BOOL        bMatch;
    ETYPE_T     eType;
    LPVOID      lpvData;
    LPSELECT    lpstSelect;

    assert(StateTop(lpstEnv) == esCASE_TO);
    assert(SubStateTop(lpstEnv) != essSKIP);

    lpstSelect = (LPSELECT)ControlTop(lpstEnv);

    // Get the value of the expression

    if (EvalQueryTopOperand(lpstEnv,
                            &eType,
                            &lpvData) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    bMatch = FALSE;
    if (lpstSelect->eCaseType == eCASE_TYPE_TO_CHECK &&
        eType == lpstSelect->eType)
    {
        // Check for a match

        bMatch = TRUE;
        switch (eType)
        {
            case eTYPE_LONG:
            {
                long lValue = *(LPLONG)lpvData;

                if (lValue <= lpstSelect->uData.lValue)
                    bMatch = TRUE;
                break;
            }

            case eTYPE_STRING:
                if (PascalStrCmp(lpstSelect->uData.lpbyStr,
                                 (LPBYTE)lpvData) <= 0)
                    bMatch = TRUE;
                break;

            default:
                // No match
                break;
        }
    }

    // Pop the top operand

    if (EvalPopTopOperand(lpstEnv) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    switch (lpstEnv->eToken)
    {
        case eTOKEN_COMMA:
            if (bMatch == TRUE)
            {
                // This case matches, skip the rest of the cases

                StateNew(lpstEnv,esCASE_FOUND_MATCH);

                // Skip the rest of the line and do the case

                StatePush(lpstEnv,esCASE_BODY);
                StatePush(lpstEnv,esSKIP_CASE_LINE);
            }
            else
            {
                // Multiple expressions

                StateNew(lpstEnv,esCASE);
            }
            break;

        default:
            if (bMatch == TRUE)
            {
                // This case matches, skip the rest of the cases

                StateNew(lpstEnv,esCASE_FOUND_MATCH);

                // Do the body

                StatePush(lpstEnv,esCASE_BODY);
            }
            else
            {
                // Skip the body

                StatePush(lpstEnv,esCASE_BODY);
                SubStateNew(lpstEnv,essSKIP);
            }
            if (lpstEnv->eToken != eTOKEN_NEWLINE &&
                lpstEnv->eToken != eTOKEN_COLON)
            {
                StatePush(lpstEnv,esSKIP_CASE_LINE);
                RedoToken(lpstEnv);
            }
            break;
    }
}


//*************************************************************************
//
// void StateCASE_IS()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs following the esCASE state after an eTOKEN_IS
//  is seen.
//
//  The next token is expected to be one of:
//      eTOKEN_EQ
//      eTOKEN_NE
//      eTOKEN_LT
//      eTOKEN_GT
//      eTOKEN_LE
//      eTOKEN_GE
//
//  If the next token matches one of the above, then the
//  state transitions to the expression evaluation state
//  to evaluate the expression to compare against.
//
//  The value of the expression is compared in the esCASE_IS_EXPR
//  state.
//
//  Otherwise,
// Returns:
//  Nothing.
//
//*************************************************************************

void StateCASE_IS
(
    LPENV       lpstEnv
)
{
    LPSELECT    lpstSelect;

    assert(StateTop(lpstEnv) == esCASE_IS);
    assert(SubStateTop(lpstEnv) != essSKIP);

    lpstSelect = (LPSELECT)ControlTop(lpstEnv);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_EQ:
            lpstSelect->eCaseType = eCASE_TYPE_IS_EQ;
            break;

        case eTOKEN_NE:
            lpstSelect->eCaseType = eCASE_TYPE_IS_NE;
            break;

        case eTOKEN_LT:
            lpstSelect->eCaseType = eCASE_TYPE_IS_LT;
            break;

        case eTOKEN_GT:
            lpstSelect->eCaseType = eCASE_TYPE_IS_GT;
            break;

        case eTOKEN_LE:
            lpstSelect->eCaseType = eCASE_TYPE_IS_LE;
            break;

        case eTOKEN_GE:
            lpstSelect->eCaseType = eCASE_TYPE_IS_GE;
            break;

        default:
            // Just skip the case
            StatePush(lpstEnv,esCASE_BODY);
            SubStateNew(lpstEnv,essSKIP);
            StatePush(lpstEnv,esSKIP_CASE_LINE);
            RedoToken(lpstEnv);
            return;
    }

    // Evaluate the expression

    StateNew(lpstEnv,esCASE_IS_EXPR);
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
// void StateCASE_IS_EXPR()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs following the evaluation of the expression
//  after the esCASE_IS state.
//
//  The function switches on the relational operation that was
//  specified in the esCASE_IS state and performs the appropriate
//  relational comparison.
//
//  If the expression was a match, the next state is esSKIP_CASE_LINE
//  if the next token is not eTOKEN_NEWLINE or eTOKEN_COLON.
//  After skipping the rest of the case line, the state will transition
//  to the esCASE_BODY state to execute the body of this case.  Since
//  a matching case is found, the state after esCASE_BODY will be
//  esCASE_FOUND_MATCH to skip the rest of the cases.
//
//  Otherwise, if the expression was not a match, but the next token
//  is an eTOKEN_COMMA, the next part of the case is tested with a
//  transition to the esCASE state.  If the next token is not an
//  eTOKEN_COMMA, the body is skipped.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateCASE_IS_EXPR
(
    LPENV       lpstEnv
)
{
    ETYPE_T     eType;
    LPVOID      lpvData;
    LPSELECT    lpstSelect;
    BOOL        bMatch;

    assert(StateTop(lpstEnv) == esCASE_IS_EXPR);
    assert(SubStateTop(lpstEnv) != essSKIP);

    lpstSelect = (LPSELECT)ControlTop(lpstEnv);

    // Get the value of the expression

    if (EvalQueryTopOperand(lpstEnv,
                            &eType,
                            &lpvData) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    bMatch = FALSE;
    if (eType == lpstSelect->eType)
    {
        // Check for a match

        switch (eType)
        {
            case eTYPE_LONG:
            {
                long lValue = *(LPLONG)lpvData;

                switch (lpstSelect->eCaseType)
                {
                    case eCASE_TYPE_IS_EQ:
                        if (lValue == lpstSelect->uData.lValue)
                            bMatch = TRUE;
                        break;

                    case eCASE_TYPE_IS_NE:
                        if (lValue != lpstSelect->uData.lValue)
                            bMatch = TRUE;
                        break;

                    case eCASE_TYPE_IS_LT:
                        if (lpstSelect->uData.lValue < lValue)
                            bMatch = TRUE;
                        break;

                    case eCASE_TYPE_IS_GT:
                        if (lpstSelect->uData.lValue > lValue)
                            bMatch = TRUE;
                        break;

                    case eCASE_TYPE_IS_LE:
                        if (lpstSelect->uData.lValue <= lValue)
                            bMatch = TRUE;
                        break;

                    case eCASE_TYPE_IS_GE:
                        if (lpstSelect->uData.lValue >= lValue)
                            bMatch = TRUE;
                        break;

                    default:
                        // This should never happen
                        assert(0);
                        break;
                }
                break;
            }

            case eTYPE_STRING:
            {
                int nCompare;

                nCompare = PascalStrCmp(lpstSelect->uData.lpbyStr,
                                        (LPBYTE)lpvData);

                switch (lpstSelect->eCaseType)
                {
                    case eCASE_TYPE_IS_EQ:
                        if (nCompare == 0)
                            bMatch = TRUE;
                        break;

                    case eCASE_TYPE_IS_NE:
                        if (nCompare != 0)
                            bMatch = TRUE;
                        break;

                    case eCASE_TYPE_IS_LT:
                        if (nCompare < 0)
                            bMatch = TRUE;
                        break;

                    case eCASE_TYPE_IS_GT:
                        if (nCompare > 0)
                            bMatch = TRUE;
                        break;

                    case eCASE_TYPE_IS_LE:
                        if (nCompare <= 0)
                            bMatch = TRUE;
                        break;

                    case eCASE_TYPE_IS_GE:
                        if (nCompare >= 0)
                            bMatch = TRUE;
                        break;

                    default:
                        // This should never happen
                        assert(0);
                        break;
                }
                break;
            }

            default:
                // No match
                break;
        }
    }

    // Pop the top operand

    if (EvalPopTopOperand(lpstEnv) == FALSE)
    {
        StateNew(lpstEnv,esERROR);
        return;
    }

    switch (lpstEnv->eToken)
    {
        case eTOKEN_COMMA:
            if (bMatch == TRUE)
            {
                // This case matches, skip the rest of the cases

                StateNew(lpstEnv,esCASE_FOUND_MATCH);

                // Skip the rest of the line and do the case

                StatePush(lpstEnv,esCASE_BODY);
                StatePush(lpstEnv,esSKIP_CASE_LINE);
            }
            else
            {
                // Multiple expressions

                StateNew(lpstEnv,esCASE);
            }
            break;

        default:
            if (bMatch == TRUE)
            {
                // This case matches, skip the rest of the cases

                StateNew(lpstEnv,esCASE_FOUND_MATCH);

                // Do the body

                StatePush(lpstEnv,esCASE_BODY);
            }
            else
            {
                // Skip the body

                StatePush(lpstEnv,esCASE_BODY);
                SubStateNew(lpstEnv,essSKIP);
            }
            if (lpstEnv->eToken != eTOKEN_NEWLINE &&
                lpstEnv->eToken != eTOKEN_COLON)
            {
                StatePush(lpstEnv,esSKIP_CASE_LINE);
                RedoToken(lpstEnv);
            }
            break;
    }
}


//*************************************************************************
//
// void StateCASE_BODY()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs following the evaluation of a case line.
//
//  If the next token is either eTOKEN_CASE or eTOKEN_END, then
//  this state is popped to exit out of the case body.
//  Otherwise, it is assumed that the token begins a statement
//  of the case body and a transition to the esSTATEMENT state is
//  made to execute the statement.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateCASE_BODY
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esCASE_BODY);

    switch (lpstEnv->eToken)
    {
        case eTOKEN_CASE:
        case eTOKEN_END:
            // pop (esCASE_BODY)

            StatePop(lpstEnv);
            if (StateTop(lpstEnv) != esCASE_FOUND_MATCH)
                StateNew(lpstEnv,esSELECT_BODY);
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
// void StateCASE_FOUND_MATCH()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs following the execution of the body of a
//  case that evaluated to true.  The state sets the substate of the
//  esSELECT_BODY state to essSKIP so that the rest of the case
//  statements and bodies are not evaluated.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateCASE_FOUND_MATCH
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esCASE_FOUND_MATCH);

    // Skip the rest of the select case

    StateNew(lpstEnv,esSELECT_BODY);
    SubStateNew(lpstEnv,essSKIP);
    RedoToken(lpstEnv);
}


//*************************************************************************
//
// void StateCASE_SKIP_LINE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This state occurs when the rest of a case line should be skipped.
//  The end of the case line is signalled by either a colon, newline,
//  or label.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void StateSKIP_CASE_LINE
(
    LPENV       lpstEnv
)
{
    assert(StateTop(lpstEnv) == esSKIP_CASE_LINE);

    // Skip until either a colon or a newline is seen

    switch (lpstEnv->eToken)
    {
        case eTOKEN_COLON:
        case eTOKEN_NEWLINE:
            // pop (esSKIP_CASE_LINE)

            StatePop(lpstEnv);
            break;

        case eTOKEN_LABEL:
        case eTOKEN_WORD_LABEL:
            // pop (esSKIP_CASE_LINE)

            StatePop(lpstEnv);
            RedoToken(lpstEnv);
            break;
    }
}

