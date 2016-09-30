//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOP6.CPv   1.1   12 Oct 1998 13:22:32   DCHI  $
//
// Description:
//  Handling of opcodes 0x60-0x6F.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOP6.CPv  $
// 
//    Rev 1.1   12 Oct 1998 13:22:32   DCHI
// - Modified O97_OP_66_END_FUNCTION() to handle function end when function
//   is called as a subroutine.
// - Filled body of O97_OP_6E_END_WITH() to handle End With.
// 
//    Rev 1.0   15 Aug 1997 13:29:08   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"


//********************************************************************
//
// BOOL O97_OP_60_ELSE_SINGLE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles the Else clause of a single line if...then..else
//  statement.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_60_ELSE_SINGLE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x60);

//    printf("O97_OP_60_ELSE_SINGLE\n");

    if (O97StateTop(lpstEnv) == esO97_THEN_SINGLE)
    {
        // Pop the THEN_SINGLE state

        O97StatePop(lpstEnv);

        if (O97SubStateTop(lpstEnv) == essO97_FALSE)
        {
            O97StatePush(lpstEnv,esO97_ELSE_SINGLE);
            O97SubStateNew(lpstEnv,essO97_DO);
        }
        else
        {
            O97StatePush(lpstEnv,esO97_ELSE_SINGLE);
            O97SubStateNew(lpstEnv,essO97_SKIP);
        }
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_61_ELSE_MULTIPLE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles the ELSE clause of a multi-line if...then...else
//  statement.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_61_ELSE_MULTIPLE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x61);

//    printf("O97_OP_61_ELSE_MULTIPLE\n");

    if (O97StateTop(lpstEnv) == esO97_THEN_MULTIPLE)
    {
        // Pop the THEN_MULTIPLE state

        O97StatePop(lpstEnv);

        if (O97SubStateTop(lpstEnv) == essO97_FALSE)
        {
            O97StatePush(lpstEnv,esO97_ELSE_MULTIPLE);
            O97SubStateNew(lpstEnv,essO97_DO);
        }
        else
        {
            O97StatePush(lpstEnv,esO97_ELSE_MULTIPLE);
            O97SubStateNew(lpstEnv,essO97_SKIP);
        }
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_62_ELSEIF()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles the ELSEIF clause of a multi-line if...then...else
//  statement.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_62_ELSEIF
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x62);

//    printf("O97_OP_62_ELSEIF\n");

    if (O97StateTop(lpstEnv) == esO97_THEN_MULTIPLE)
    {
        // Pop the THEN_MULTIPLE state

        O97StatePop(lpstEnv);

        if (O97SubStateTop(lpstEnv) == essO97_FALSE)
        {
            BOOL    bResult;

            // No previous if/elseif was TRUE, so we should
            //  evaluate this one

            if (O97EvalGetAndPopTopAsBoolean(lpstEnv,
                                             &bResult) == FALSE)
            {
                return(FALSE);
            }

            if (bResult == FALSE)
            {
                O97SubStateNew(lpstEnv,essO97_FALSE);
                O97StatePush(lpstEnv,esO97_THEN_MULTIPLE);
                O97SubStateNew(lpstEnv,essO97_SKIP);
            }
            else
            {
                O97SubStateNew(lpstEnv,essO97_TRUE);
                O97StatePush(lpstEnv,esO97_THEN_MULTIPLE);
                O97SubStateNew(lpstEnv,essO97_DO);
            }
        }
        else
        {
            // Some previous if/elseif was TRUE, so we should
            //  pop the result off of the stack and ignore it

            if (O97EvalPopTopOperand(lpstEnv) == FALSE)
                return(FALSE);

            // We should also ignore the rest of the if

            O97StatePush(lpstEnv,esO97_THEN_MULTIPLE);
            O97SubStateNew(lpstEnv,essO97_SKIP);
        }
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_63()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_63
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x63);

//    printf("O97_OP_63\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_64_END()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_64_END
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x64);

//    printf("O97_OP_64_END\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_65()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_65
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x65);

//    printf("O97_OP_65\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_66_END_FUNCTION()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles End Function.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_66_END_FUNCTION
(
    LPO97_ENV       lpstEnv
)
{
    LPO97_CALL      lpstCall;
    EO97_TYPE_T     eType;
    LPVOID          lpvData;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x66);

//    printf("O97_OP_66_END_FUNCTION\n");

    // If we are searching for the label of a goto, then
    //  restart search from the beginning

    if (lpstEnv->eStateGoto == eO97_STATE_GOTO_0)
    {
        lpstEnv->eStateGoto = eO97_STATE_GOTO_1;
        lpstEnv->bNewIP = TRUE;
        lpstEnv->dwIPLine = lpstEnv->stRoutineStartIP.wLine;
        lpstEnv->dwIPOffset = lpstEnv->stRoutineStartIP.wOffset;
        return(TRUE);
    }

    // Otherwise, we are returning from the function

    if (O97StateTop(lpstEnv) != esO97_CALL_BODY)
    {
        // For now, just return FALSE if we have gotten into
        //  an inconsistent state (possible because of gotos?)

        return(FALSE);
    }

    // Pop esO97_CALL_BODY

    O97StatePop(lpstEnv);

    if (O97StateTop(lpstEnv) == esO97_CALL_FUNCTION)
    {
        // Get the return value and push it onto the evaluation stack

        if (O97VarGetFirstFrameVar(lpstEnv,
                                   &eType,
                                   &lpvData) != NULL)
        {
            if (O97EvalPushOperand(lpstEnv,eType,lpvData) == FALSE)
                return(FALSE);
        }
        else
        {
            if (O97EvalPushOperand(lpstEnv,eO97_TYPE_UNKNOWN,NULL) == FALSE)
                return(FALSE);
        }
    }
    else
    if (O97StateTop(lpstEnv) != esO97_CALL_SUB)
        return(FALSE);

    // Pop the local variable frame

    if (O97VarPopFrame(lpstEnv) == FALSE)
        return(FALSE);

    // Get the return address

    lpstCall = (LPO97_CALL)O97ControlTop(lpstEnv);

    lpstEnv->dwIPLine = lpstCall->stReturnIP.wLine;
    lpstEnv->dwIPOffset = lpstCall->stReturnIP.wOffset;
    lpstEnv->stRoutineStartIP = lpstCall->stRoutineStartIP;
    lpstEnv->bNewIP = TRUE;

    O97ControlDestroyTopControl(lpstEnv);

    // Pop esO97_CALL_FUNCTION

    O97StatePop(lpstEnv);

    // If it was an intermodule call, then we also need to cleanup
    //  the external call resources

    if (O97StateTop(lpstEnv) == esO97_EXTERNAL_CLEANUP &&
        O97ExternalCallCleanup(lpstEnv) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_67_END_SINGLE_IF()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles the end of a single line if statement.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_67_END_SINGLE_IF
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x67);

//    printf("O97_OP_67_END_SINGLE_IF\n");

    if (O97StateTop(lpstEnv) == esO97_THEN_SINGLE ||
        O97StateTop(lpstEnv) == esO97_ELSE_SINGLE)
    {
        // Pop the THEN_SINGLE or ELSE_SINGLE state

        O97StatePop(lpstEnv);
    }

    // Pop the IF_COND state

    O97StatePop(lpstEnv);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_68_END_IF()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles End If.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_68_END_IF
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x68);

//    printf("O97_OP_68_END_IF\n");

    if (O97StateTop(lpstEnv) == esO97_THEN_MULTIPLE ||
        O97StateTop(lpstEnv) == esO97_ELSE_MULTIPLE)
    {
        // Pop the THEN_MULTIPLE or ELSE_MULTIPLE state

        O97StatePop(lpstEnv);
    }

    // Pop the IF_COND state

    O97StatePop(lpstEnv);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_69()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_69
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x69);

//    printf("O97_OP_69\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_6A_END_PROPERTY()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_6A_END_PROPERTY
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x6A);

//    printf("O97_OP_6A_END_PROPERTY\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_6B_END_SELECT()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles the end of a Select statement.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_6B_END_SELECT
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x6B);

//    printf("O97_OP_6B_END_SELECT\n");

    if (O97StateTop(lpstEnv) == esO97_SELECT_BODY)
        O97StatePop(lpstEnv);

    if (O97StateTop(lpstEnv) != esO97_SELECT)
        return(FALSE);

    if (O97ControlTop(lpstEnv) != NULL)
        O97ControlDestroyTopControl(lpstEnv);

    // Pop esO97_SELECT

    O97StatePop(lpstEnv);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_6C_END_SUB()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  If the control state is in the first stage of a goto label
//  search, the function restarts the search at the beginning
//  of the routine.
//
//  Otherwise, we must be in the CALL_BODY state.  If not,
//  the function returns error.  If indeed the state is CALL_BODY,
//  the function sets the new IP to the return address IP.  The
//  parent routine start IP is also restored.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_6C_END_SUB
(
    LPO97_ENV       lpstEnv
)
{
    LPO97_CALL      lpstCall;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x6C);

//    printf("O97_OP_6C_END_SUB\n");

    // If we are searching for the label of a goto, then
    //  restart search from the beginning

    if (lpstEnv->eStateGoto == eO97_STATE_GOTO_0)
    {
        lpstEnv->eStateGoto = eO97_STATE_GOTO_1;
        lpstEnv->bNewIP = TRUE;
        lpstEnv->dwIPLine = lpstEnv->stRoutineStartIP.wLine;
        lpstEnv->dwIPOffset = lpstEnv->stRoutineStartIP.wOffset;
        return(TRUE);
    }

    // Otherwise, we are returning from the sub

    if (O97StateTop(lpstEnv) != esO97_CALL_BODY)
    {
        // For now, just return FALSE if we have gotten into
        //  an inconsistent state (possible because of gotos?)

        return(FALSE);
    }

    // Pop esO97_CALL_BODY

    O97StatePop(lpstEnv);

    if (O97StateTop(lpstEnv) != esO97_CALL_SUB)
    {
        // Was not in a sub?

        return(FALSE);
    }

    // Pop the local variable frame

    if (O97VarPopFrame(lpstEnv) == FALSE)
        return(FALSE);

    // Get the return address

    lpstCall = (LPO97_CALL)O97ControlTop(lpstEnv);

    lpstEnv->dwIPLine = lpstCall->stReturnIP.wLine;
    lpstEnv->dwIPOffset = lpstCall->stReturnIP.wOffset;
    lpstEnv->stRoutineStartIP = lpstCall->stRoutineStartIP;
    lpstEnv->bNewIP = TRUE;

    O97ControlDestroyTopControl(lpstEnv);

    // Pop esO97_CALL_SUB

    O97StatePop(lpstEnv);

    // If it was an intermodule call, then we also need to cleanup
    //  the external call resources

    if (O97StateTop(lpstEnv) == esO97_EXTERNAL_CLEANUP &&
        O97ExternalCallCleanup(lpstEnv) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_6D_END_TYPE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_6D_END_TYPE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x6D);

//    printf("O97_OP_6D_END_TYPE\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_6E_END_WITH()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_6E_END_WITH
(
    LPO97_ENV       lpstEnv
)
{
    assert(lpstEnv->abyOpcode[0] == 0x6E);

//    printf("O97_OP_6E_END_WITH\n");

    if (lpstEnv->nWithDepth > 0)
        lpstEnv->nWithDepth--;

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_6F()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_6F
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x6F);

//    printf("O97_OP_6F\n");

    return(TRUE);
}

