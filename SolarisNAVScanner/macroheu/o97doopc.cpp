//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOPC.CPv   1.0   15 Aug 1997 13:29:18   DCHI  $
//
// Description:
//  Handling of opcodes 0xC0-0xCF.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOPC.CPv  $
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
// BOOL O97_OP_C0_NEXT_A()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  VBA:            Next A
//  Code:           F7 00 A F6 00 C0 00
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_C0_NEXT_A
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xC0);

//    printf("O97_OP_C0_NEXT_A\n");

    // Get rid of the top operand which is the variable

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

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

            lpstEnv->bNewIP = TRUE;
            lpstEnv->dwIPLine = (DWORD)lpstFor->stBodyIP.wLine;
            lpstEnv->dwIPOffset = (DWORD)lpstFor->stBodyIP.wOffset;
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


//********************************************************************
//
// BOOL O97_OP_C1_ON_ERROR_GOTO()
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

BOOL O97_OP_C1_ON_ERROR_GOTO
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xC1);

    //printf("O97_OP_C1_ON_ERROR_GOTO\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_C2()
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

BOOL O97_OP_C2
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xC2);

//    printf("O97_OP_C2\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_C3()
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

BOOL O97_OP_C3
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xC3);

//    printf("O97_OP_C3\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_C4_OPEN()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  VBA:    Open For [MD] [Access [A]] [L] As [#]filenum [Len=reclen]
//  Binary: C4 00 MD LA
//              [MD]    Append          08
//                      Binary          20
//                      Input           01
//                      Output          02
//                      Random          04
//              [A]     Read             1
//                      Write            2
//                      Read Write       3
//              [L]     Shared          4
//                      Lock Read       3
//                      Lock Write      2
//                      Lock Read Write 1
//
//  Ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_C4_OPEN
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xC4);

//    printf("O97_OP_C4_OPEN\n");

    // Pop the top three operands:
    //  Filename
    //  Filenumber
    //  Recordlength

    if (O97EvalPopParameters(lpstEnv,3) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_C5_OPTION()
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

BOOL O97_OP_C5_OPTION
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xC5);

//    printf("O97_OP_C5_OPTION\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_C6_ARRAY_SIZE()
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

BOOL O97_OP_C6_ARRAY_SIZE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xC6);

//    printf("O97_OP_C6_ARRAY_SIZE\n");

    lpstEnv->lpstModEnv->wNumIndices++;

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_C7()
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

BOOL O97_OP_C7
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xC7);

//    printf("O97_OP_C7\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_C8()
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

BOOL O97_OP_C8
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xC8);

//    printf("O97_OP_C8\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_C9_PARAM_ASSIGN()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Assigns a value to a named parameter.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_C9_PARAM_ASSIGN
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xC9);

    //printf("O97_OP_C9_PARAM_ASSIGN\n");

    return O97EvalPushOperand(lpstEnv,
                              eO97_TYPE_PARAM_ASSIGN,
                              &lpstEnv->wOpID);
}


//********************************************************************
//
// BOOL O97_OP_CA_PRINTN_BEGIN()
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

BOOL O97_OP_CA_PRINTN_BEGIN
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xCA);

//    printf("O97_OP_CA_PRINTN_BEGIN\n");

    // Pop the file number

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_CB_PRINT_COMMA()
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

BOOL O97_OP_CB_PRINT_COMMA
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xCB);

//    printf("O97_OP_CB_PRINT_COMMA\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_CC_PRINT_END()
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

BOOL O97_OP_CC_PRINT_END
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xCC);

//    printf("O97_OP_CC_PRINT_END\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_CD_PRINT_ARG_COMMA()
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

BOOL O97_OP_CD_PRINT_ARG_COMMA
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xCD);

//    printf("O97_OP_CD\n");

    // Pop the print argument

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_CE_PRINT_ARG_END()
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

BOOL O97_OP_CE_PRINT_ARG_END
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xCE);

//    printf("O97_OP_CE_PRINT_ARG_END\n");

    // Pop the print argument

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_CF_PRINT_ARG_SEMICOLON()
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

BOOL O97_OP_CF_PRINT_ARG_SEMICOLON
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xCF);

//    printf("O97_OP_CF_PRINT_ARG_SEMICOLON\n");

    // Pop the print argument

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

    return(TRUE);
}


