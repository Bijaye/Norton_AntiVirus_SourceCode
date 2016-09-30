//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOP8.CPv   1.1   12 Oct 1998 13:23:32   DCHI  $
//
// Description:
//  Handling of opcodes 0x80-0x8F.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOP8.CPv  $
// 
//    Rev 1.1   12 Oct 1998 13:23:32   DCHI
// Limited number of iterations to max of 64 in O97_OP_8B_B_TO_C().
// Added validation of Step, Start, and End in O97_OP_8E_B_TO_C_STEP_D().
// 
//    Rev 1.0   15 Aug 1997 13:29:12   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"

#include "wbutil.h"

//********************************************************************
//
// BOOL O97_OP_80()
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

BOOL O97_OP_80
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x80);

//    printf("O97_OP_80\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_81()
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

BOOL O97_OP_81
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x81);

//    printf("O97_OP_81\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_82()
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

BOOL O97_OP_82
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x82);

//    printf("O97_OP_82\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_83()
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

BOOL O97_OP_83
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x83);

//    printf("O97_OP_83\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_84()
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

BOOL O97_OP_84
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x84);

//    printf("O97_OP_84\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_85()
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

BOOL O97_OP_85
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x85);

//    printf("O97_OP_85\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_86_STRCOMP_A_B()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Performs a string comparison.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_86_STRCOMP_A_B
(
    LPO97_ENV       lpstEnv
)
{
    LPBYTE          lpbyStr0, lpbyStr1;
    BOOL            bResult = FALSE;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x86);

//    printf("O97_OP_86_STRCOMP_A_B\n");

    bResult = TRUE;

    // Get first string

    if (O97EvalGetAndPopTopAsString(lpstEnv,&lpbyStr0) == FALSE)
    {
        bResult = FALSE;
        if (O97EvalPopTopOperand(lpstEnv) == FALSE)
            return(FALSE);
    }

    // Get second string

    if (O97EvalGetAndPopTopAsString(lpstEnv,&lpbyStr1) == FALSE)
    {
        bResult = FALSE;
        if (O97EvalPopTopOperand(lpstEnv) == FALSE)
            return(FALSE);
    }

    if (bResult == TRUE)
        return O97EvalPushLongOperand(lpstEnv,
                                      PascalStrCmp(lpbyStr0,lpbyStr1));

    // Just push 1

    return O97EvalPushLongOperand(lpstEnv,1);
}


//********************************************************************
//
// BOOL O97_OP_87_STRCOMP_A_B_C()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Performs a string comparison.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_87_STRCOMP_A_B_C
(
    LPO97_ENV       lpstEnv
)
{
    LPBYTE          lpbyStr0, lpbyStr1;
    BOOL            bResult = FALSE;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x87);

//    printf("O97_OP_87_STRCOMP_A_B_C\n");

    bResult = TRUE;

    // Pop the top operand

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

    // Get first string

    if (O97EvalGetAndPopTopAsString(lpstEnv,&lpbyStr0) == FALSE)
    {
        bResult = FALSE;
        if (O97EvalPopTopOperand(lpstEnv) == FALSE)
            return(FALSE);
    }

    // Get second string

    if (O97EvalGetAndPopTopAsString(lpstEnv,&lpbyStr1) == FALSE)
    {
        bResult = FALSE;
        if (O97EvalPopTopOperand(lpstEnv) == FALSE)
            return(FALSE);
    }

    if (bResult == TRUE)
        return O97EvalPushLongOperand(lpstEnv,
                                      PascalStrCmp(lpbyStr0,lpbyStr1));

    // Just push 1

    return O97EvalPushLongOperand(lpstEnv,1);
}


//********************************************************************
//
// BOOL O97_OP_88()
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

BOOL O97_OP_88
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x88);

//    printf("O97_OP_88\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_89()
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

BOOL O97_OP_89
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x89);

//    printf("O97_OP_89\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_8A()
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

BOOL O97_OP_8A
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x8A);

//    printf("O97_OP_8A\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_8B_B_TO_C()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This is:        For A = B To C
//  Represented as: F7 00 A F6 00 B C 8B 00
//
//  The function also limits the number of iterations to the
//  greater of 64 or the number of modules.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_8B_B_TO_C
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x8B);

//    printf("O97_OP_8B_B_TO_C\n");

    O97StatePush(lpstEnv,esO97_FOR_NEXT);
    if (O97SubStateTop(lpstEnv) == essO97_DO)
    {
        LPO97_FOR       lpstFor;
        LPO97_VAR       lpstVar;
        long            lStart, lEnd;
        WORD            wVarID;

        // Get End

        if (O97EvalGetAndPopTopAsLong(lpstEnv,&lEnd) == FALSE)
            return(FALSE);

        // Get Start

        if (O97EvalGetAndPopTopAsLong(lpstEnv,&lStart) == FALSE)
            return(FALSE);

        // Get Var

        if (O97EvalGetAndPopTopAsVarID(lpstEnv,&wVarID) == FALSE)
            return(FALSE);

        // Create a FOR control structure

        if (O97ControlCreateControl(lpstEnv,ectO97_FOR) == FALSE)
            return(FALSE);

        lpstVar = O97VarGet(lpstEnv,wVarID);
        if (lpstVar == NULL)
        {
            lpstVar = O97VarAlloc(lpstEnv,
                                  O97ModEnvVarIsGlobal(lpstEnv,wVarID),
                                  FALSE,
                                  wVarID);

            if (lpstVar == NULL)
            {
                // Could not allocate variable

                return(FALSE);
            }
        }

        // Set the start value of the variable

        if (O97VarSet(lpstEnv,
                      lpstVar,
                      eO97_TYPE_LONG,
                      &lStart) == NULL)
        {
            return(FALSE);
        }

        if (lStart <= lEnd)
        {
            // Set up control

            lpstFor = (LPO97_FOR)O97ControlTop(lpstEnv);

            lpstFor->lpstVar = lpstVar;
            lpstFor->lStart = lStart;

            // Limit end to the greater of 64 and lStart + # modules

            if (lEnd - lStart > 64)
            {
                if (lpstEnv->dwNumModules < 64)
                    lEnd = lStart + 64;
                else
                    lEnd = lStart + lpstEnv->dwNumModules;
            }

            lpstFor->lEnd = lEnd;
            lpstFor->stBodyIP.wLine = (WORD)lpstEnv->dwIPLine;
            lpstFor->stBodyIP.wOffset = (WORD)lpstEnv->dwNextIPOffset;
        }
        else
        {
            // Skip for body

            O97SubStateNew(lpstEnv,essO97_SKIP);
        }
    }
    else
    {
        // Pop top three operands

        if (O97EvalPopTopOperand(lpstEnv) == FALSE ||
            O97EvalPopTopOperand(lpstEnv) == FALSE ||
            O97EvalPopTopOperand(lpstEnv) == FALSE)
            return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_8C_EACH_B()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pops the top operand and ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_8C_EACH_B
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x8C);

//    printf("O97_OP_8C_EACH_B\n");

    return O97EvalPopTopOperand(lpstEnv);
}


//********************************************************************
//
// BOOL O97_OP_8D()
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

BOOL O97_OP_8D
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x8D);

//    printf("O97_OP_8D\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_8E_B_TO_C_STEP_D()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  VBA:            For A = B To C Step D
//  Code:           F7 00 A F6 00 B C D 8E 00
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_8E_B_TO_C_STEP_D
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x8E);

//    printf("O97_OP_8E_B_TO_C_STEP_D\n");

    O97StatePush(lpstEnv,esO97_FOR_NEXT);
    if (O97SubStateTop(lpstEnv) == essO97_DO)
    {
        LPO97_FOR       lpstFor;
        LPO97_VAR       lpstVar;
        long            lStart, lEnd, lStep;
        WORD            wVarID;

        // Get Step

        if (O97EvalGetAndPopTopAsLong(lpstEnv,&lStep) == FALSE)
            return(FALSE);

        // Get End

        if (O97EvalGetAndPopTopAsLong(lpstEnv,&lEnd) == FALSE)
            return(FALSE);

        // Get Start

        if (O97EvalGetAndPopTopAsLong(lpstEnv,&lStart) == FALSE)
            return(FALSE);

        // Get Var

        if (O97EvalGetAndPopTopAsVarID(lpstEnv,&wVarID) == FALSE)
            return(FALSE);

        // Create a FOR control structure

        if (O97ControlCreateControl(lpstEnv,ectO97_FOR) == FALSE)
            return(FALSE);

        lpstVar = O97VarGet(lpstEnv,wVarID);
        if (lpstVar == NULL)
        {
            lpstVar = O97VarAlloc(lpstEnv,
                                  O97ModEnvVarIsGlobal(lpstEnv,wVarID),
                                  FALSE,
                                  wVarID);

            if (lpstVar == NULL)
            {
                // Could not allocate variable

                return(FALSE);
            }
        }

        // Set the start value of the variable

        if (O97VarSet(lpstEnv,
                      lpstVar,
                      eO97_TYPE_LONG,
                      &lStart) == NULL)
        {
            return(FALSE);
        }

        if (lStep >= 1 && lStart <= lEnd ||
            lStep <= -1 && lStart >= lEnd)
        {
            // Set up control

            lpstFor = (LPO97_FOR)O97ControlTop(lpstEnv);

            lpstFor->lpstVar = lpstVar;
            lpstFor->lStart = lStart;
            lpstFor->lEnd = lEnd;
            lpstFor->lStep = lStep;
            lpstFor->stBodyIP.wLine = (WORD)lpstEnv->dwIPLine;
            lpstFor->stBodyIP.wOffset = (WORD)lpstEnv->dwNextIPOffset;
        }
        else
        {
            // Skip for body

            O97SubStateNew(lpstEnv,essO97_SKIP);
        }
    }
    else
    {
        // Pop top three operands

        if (O97EvalPopTopOperand(lpstEnv) == FALSE ||
            O97EvalPopTopOperand(lpstEnv) == FALSE ||
            O97EvalPopTopOperand(lpstEnv) == FALSE)
            return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_8F_SUB_FUNCTION()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Begins a subroutine or function.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_8F_SUB_FUNCTION
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x8F);

//    printf("O97_OP_8F_SUB_FUNCTION\n");

    if (O97StateTop(lpstEnv) == esO97_CALL_BODY)
    {
        // Not in initial call state

        return(TRUE);
    }

    // Create the local variable frame

    if (O97VarPushFrame(lpstEnv) == FALSE)
        return(FALSE);

    switch (lpstEnv->abyOpcode[1])
    {
        case 0x04:
        case 0x14:
            // Sub

            if (O97CallSetUpParameters(lpstEnv,
                                       TRUE) == FALSE)
                return(FALSE);

            break;

        default:
            // Assume function

            if (O97CallSetUpParameters(lpstEnv,
                                       FALSE) == FALSE)
                return(FALSE);

            break;
    }

    O97StatePush(lpstEnv,esO97_CALL_BODY);

    return(TRUE);
}


