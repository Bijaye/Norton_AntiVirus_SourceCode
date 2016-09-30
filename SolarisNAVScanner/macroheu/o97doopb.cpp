//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOPB.CPv   1.0   15 Aug 1997 13:29:18   DCHI  $
//
// Description:
//  Handling of opcodes 0xB0-0xBF.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOPB.CPv  $
// 
//    Rev 1.0   15 Aug 1997 13:29:18   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"


//********************************************************************
//
// BOOL O97_OP_B0()
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

BOOL O97_OP_B0
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xB0);

//    printf("O97_OP_B0\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_B1_LOOP()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles looping back to the beginning of a Do...Loop statement.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_B1_LOOP
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xB1);

//    printf("O97_OP_B1_LOOP\n");

    if (O97StateTop(lpstEnv) == esO97_DO_LOOP)
    {
        if (O97SubStateTop(lpstEnv) == essO97_SKIP)
        {
            // Destroy the control if necessary

            if (O97ControlTop(lpstEnv) != NULL)
                O97ControlDestroyTopControl(lpstEnv);

            // If the exit state was an Exit Do, then reset
            //  the exit state

            if (lpstEnv->eStateExit == eO97_STATE_EXIT_DO)
                lpstEnv->eStateExit = eO97_STATE_EXIT_NONE;

            // Pop the DO_LOOP state

            O97StatePop(lpstEnv);

        }
        else
        {
            LPO97_DO_LOOP   lpstDoLoop;

            // Jump back to evaluate the condition

            lpstDoLoop = (LPO97_DO_LOOP)O97ControlTop(lpstEnv);
            if (lpstDoLoop != NULL)
            {
                // Jump back to beginning of Do Loop body

                lpstEnv->bNewIP = TRUE;
                lpstEnv->dwIPLine = (DWORD)lpstDoLoop->stCondIP.wLine;
                lpstEnv->dwIPOffset = (DWORD)lpstDoLoop->stCondIP.wOffset;
                return(TRUE);
            }
        }
    }
    else
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_B2_LOOP_UNTIL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles looping back to the beginning of a Do...Loop Until
//  statement.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_B2_LOOP_UNTIL
(
    LPO97_ENV       lpstEnv
)
{
    BOOL            bResult;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xB2);

//    printf("O97_OP_B2_LOOP_UNTIL\n");

    // Get the top operand

    if (O97EvalGetAndPopTopAsBoolean(lpstEnv,
                                     &bResult) == FALSE)
    {
        return(FALSE);
    }

    if (O97StateTop(lpstEnv) == esO97_DO_LOOP)
    {
        if (O97SubStateTop(lpstEnv) == essO97_SKIP ||
            bResult == TRUE)
        {
            // Destroy the control if necessary

            if (O97ControlTop(lpstEnv) != NULL)
                O97ControlDestroyTopControl(lpstEnv);

            // If the exit state was an Exit Do, then reset
            //  the exit state

            if (lpstEnv->eStateExit == eO97_STATE_EXIT_DO)
                lpstEnv->eStateExit = eO97_STATE_EXIT_NONE;

            // Pop the DO_LOOP state

            O97StatePop(lpstEnv);
        }
        else
        {
            LPO97_DO_LOOP   lpstDoLoop;

            // Jump back to evaluate the condition

            lpstDoLoop = (LPO97_DO_LOOP)O97ControlTop(lpstEnv);
            if (lpstDoLoop != NULL)
            {
                // Jump back to beginning of Do Loop body

                lpstEnv->bNewIP = TRUE;
                lpstEnv->dwIPLine = (DWORD)lpstDoLoop->stCondIP.wLine;
                lpstEnv->dwIPOffset = (DWORD)lpstDoLoop->stCondIP.wOffset;
                return(TRUE);
            }
        }
    }
    else
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_B3_LOOP_WHILE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles looping back to the beginning of a Do...Loop While
//  statement.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_B3_LOOP_WHILE
(
    LPO97_ENV       lpstEnv
)
{
    BOOL            bResult;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xB3);

//    printf("O97_OP_B3_LOOP_WHILE\n");

    // Get the top operand

    if (O97EvalGetAndPopTopAsBoolean(lpstEnv,
                                     &bResult) == FALSE)
    {
        return(FALSE);
    }

    if (O97StateTop(lpstEnv) == esO97_DO_LOOP)
    {
        if (O97SubStateTop(lpstEnv) == essO97_SKIP ||
            bResult == FALSE)
        {
            // Destroy the control if necessary

            if (O97ControlTop(lpstEnv) != NULL)
                O97ControlDestroyTopControl(lpstEnv);

            // If the exit state was an Exit Do, then reset
            //  the exit state

            if (lpstEnv->eStateExit == eO97_STATE_EXIT_DO)
                lpstEnv->eStateExit = eO97_STATE_EXIT_NONE;

            // Pop the DO_LOOP state

            O97StatePop(lpstEnv);
        }
        else
        {
            LPO97_DO_LOOP   lpstDoLoop;

            // Jump back to evaluate the condition

            lpstDoLoop = (LPO97_DO_LOOP)O97ControlTop(lpstEnv);
            if (lpstDoLoop != NULL)
            {
                // Jump back to beginning of Do Loop body

                lpstEnv->bNewIP = TRUE;
                lpstEnv->dwIPLine = (DWORD)lpstDoLoop->stCondIP.wLine;
                lpstEnv->dwIPOffset = (DWORD)lpstDoLoop->stCondIP.wOffset;
                return(TRUE);
            }
        }
    }
    else
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_B4()
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

BOOL O97_OP_B4
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xB4);

//    printf("O97_OP_B4\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_B5()
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

BOOL O97_OP_B5
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xB5);

//    printf("O97_OP_B5\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_B6_PRINT_BEGIN()
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

BOOL O97_OP_B6_PRINT_BEGIN
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xB6);

//    printf("O97_OP_B6_PRINT_BEGIN\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_B7()
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

BOOL O97_OP_B7
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xB7);

//    printf("O97_OP_B7\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_B8()
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

BOOL O97_OP_B8
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xB8);

//    printf("O97_OP_B8\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_B9()
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

BOOL O97_OP_B9
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xB9);

//    printf("O97_OP_B9\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_BA()
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

BOOL O97_OP_BA
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xBA);

//    printf("O97_OP_BA\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_BB()
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

BOOL O97_OP_BB
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xBB);

//    printf("O97_OP_BB\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_BC()
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

BOOL O97_OP_BC
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xBC);

//    printf("O97_OP_BC\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_BD()
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

BOOL O97_OP_BD
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xBD);

//    printf("O97_OP_BD\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_BE_NEW()
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

BOOL O97_OP_BE_NEW
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xBE);

//    printf("O97_OP_BE_NEW\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_BF_NEXT()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  VBA:            Next
//  Code:           F7 00 BF 00
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_BF_NEXT
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xBF);

//    printf("O97_OP_BF_NEXT\n");

    // In this case there is no variable

    if (O97SubStateTop(lpstEnv) == essO97_DO)
    {
        LPO97_FOR       lpstFor;

        lpstFor = (LPO97_FOR)O97ControlTop(lpstEnv);

        if (lpstFor == NULL || lpstFor->ecType != ectO97_FOR)
            return(FALSE);

        // Check the current value

        lpstFor->lpstVar->uData.lValue += lpstFor->lStep;
        if (lpstFor->lStep < 0 &&
            lpstFor->lpstVar->uData.lValue >= lpstFor->lEnd ||
            lpstFor->lStep > 0 &&
            lpstFor->lpstVar->uData.lValue <= lpstFor->lEnd)
        {
            // Jump back to beginning of FOR_BODY

            lpstEnv->dwIPLine = (WORD)lpstFor->stBodyIP.wLine;
            lpstEnv->dwIPOffset = (WORD)lpstFor->stBodyIP.wOffset;
            return(TRUE);
        }
    }

    // Destroy the control if necessary

    if (O97ControlTop(lpstEnv) != NULL)
        O97ControlDestroyTopControl(lpstEnv);

    // If the exit state was an Exit For, then reset
    //  the exit state

    if (lpstEnv->eStateExit == eO97_STATE_EXIT_FOR)
        lpstEnv->eStateExit = eO97_STATE_EXIT_NONE;

    // Pop the FOR_NEXT state

    O97StatePop(lpstEnv);

    return(TRUE);
}


