//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOPE.CPv   1.1   12 Oct 1998 13:24:02   DCHI  $
//
// Description:
//  Handling of opcodes 0xE0-0xEF.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOPE.CPv  $
// 
//    Rev 1.1   12 Oct 1998 13:24:02   DCHI
// Filled in body of O97_OP_ED_WITH_A() to handle With.
// 
//    Rev 1.0   15 Aug 1997 13:29:22   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"


//********************************************************************
//
// BOOL O97_OP_E0()
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

BOOL O97_OP_E0
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xE0);

//    printf("O97_OP_E0\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_E1()
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

BOOL O97_OP_E1
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xE1);

//    printf("O97_OP_E1\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_E2_SELECT_CASE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Identifies the value upon which to select.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_E2_SELECT_CASE
(
    LPO97_ENV       lpstEnv
)
{
    EO97_TYPE_T     eType;
    LPVOID          lpvData;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xE2);
    
//    printf("O97_OP_E2_SELECT_CASE\n");

    O97StatePush(lpstEnv,esO97_SELECT);
    if (O97SubStateTop(lpstEnv) == essO97_DO)
    {
        if (O97EvalGetTopBaseData(lpstEnv,
                                  &eType,
                                  &lpvData) == FALSE)
        {
            return(FALSE);
        }

        if (O97ControlCreateSelectControl(lpstEnv,
                                          eType,
                                          lpvData) == FALSE)
        {
            return(FALSE);
        }
    }

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_E3()
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

BOOL O97_OP_E3
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xE3);

//    printf("O97_OP_E3\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_E4()
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

BOOL O97_OP_E4
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xE4);

//    printf("O97_OP_E4\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_E5_SET()
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

BOOL O97_OP_E5_SET
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xE5);

//    printf("O97_OP_E5_SET\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_E6()
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

BOOL O97_OP_E6
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xE6);

//    printf("O97_OP_E6\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_E7()
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

BOOL O97_OP_E7
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xE7);

//    printf("O97_OP_E7\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_E8_TYPE()
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

BOOL O97_OP_E8_TYPE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xE8);

//    printf("O97_OP_E8_TYPE\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_E9()
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

BOOL O97_OP_E9
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xE9);

//    printf("O97_OP_E9\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_EA_SYMBOL_DECL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pops any dimension parameters.  Does not currently handling
//  initial assignment.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_EA_SYMBOL_DECL
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xEA);

    //printf("O97_OP_EA_SYMBOL_DECL\n");

    // Pop any waiting dimension indices

    if (O97EvalPopParameters(lpstEnv,
                             lpstEnv->lpstModEnv->wNumIndices) == FALSE)
        return(FALSE);

    lpstEnv->lpstModEnv->wNumIndices = 0;

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_EB_WEND()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles Wend.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_EB_WEND
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xEB);

//    printf("O97_OP_EB_WEND\n");

    if (O97StateTop(lpstEnv) == esO97_WHILE)
    {
        if (O97SubStateTop(lpstEnv) == essO97_SKIP)
        {
            // Destroy the control if necessary

            if (O97ControlTop(lpstEnv) != NULL)
                O97ControlDestroyTopControl(lpstEnv);

            // Pop the WHILE state

            O97StatePop(lpstEnv);
        }
        else
        {
            LPO97_WHILE     lpstWhile;

            // Jump back to evaluate the condition

            lpstWhile = (LPO97_WHILE)O97ControlTop(lpstEnv);
            if (lpstWhile != NULL)
            {
                // Jump back to beginning of While body

                lpstEnv->bNewIP = TRUE;
                lpstEnv->dwIPLine = (DWORD)lpstWhile->stCondIP.wLine;
                lpstEnv->dwIPOffset = (DWORD)lpstWhile->stCondIP.wOffset;
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
// BOOL O97_OP_EC_WHILE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Begins a While statement.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_EC_WHILE
(
    LPO97_ENV       lpstEnv
)
{
    BOOL            bResult;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xEC);

//    printf("O97_OP_EC_WHILE\n");

    // Get the top operand

    if (O97EvalGetAndPopTopAsBoolean(lpstEnv,
                                     &bResult) == FALSE)
    {
        return(FALSE);
    }

    if (O97StateTop(lpstEnv) != esO97_WHILE ||
        (O97ControlTop(lpstEnv) != NULL &&
         ((LPO97_WHILE)O97ControlTop(lpstEnv))->stCondIP.wLine !=
         (WORD)lpstEnv->dwIPLine))
    {
        O97StatePush(lpstEnv,esO97_WHILE);

        if (O97SubStateTop(lpstEnv) != essO97_SKIP && bResult == TRUE)
        {
            // Create a WHILE control structure

            if (O97ControlCreateControl(lpstEnv,ectO97_WHILE) == FALSE)
                return(FALSE);
        }
    }

    if (bResult == FALSE)
    {
        O97SubStateNew(lpstEnv,essO97_SKIP);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_ED_WITH_A()
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

BOOL O97_OP_ED_WITH_A
(
    LPO97_ENV       lpstEnv
)
{
    EO97_TYPE_T     eType;
    LPVOID          lpvData;

    assert(lpstEnv->abyOpcode[0] == 0xED);

//    printf("O97_OP_ED_WITH_A\n");

    if (lpstEnv->nWithDepth < O97_MAX_WITH_DEPTH)
    {
        if (O97EvalQueryTopOperand(lpstEnv,
                                   &eType,
                                   &lpvData) == FALSE)
            eType = eO97_TYPE_UNKNOWN;

        if (eType == eO97_TYPE_O97_OBJECT_ID ||
            eType == eO97_TYPE_OBJECT_ID)
        {
            lpstEnv->adwWithObjIDs[lpstEnv->nWithDepth] =
                *(LPDWORD)lpvData;
        }
        else
            lpstEnv->adwWithObjIDs[lpstEnv->nWithDepth] =
                0xFFFFFFFF;

        lpstEnv->nWithDepth++;
    }

    return O97EvalPopTopOperand(lpstEnv);
}


//********************************************************************
//
// BOOL O97_OP_EE_WRITEN_BEGIN()
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

BOOL O97_OP_EE_WRITEN_BEGIN
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xEE);

//    printf("O97_OP_EE_WRITEN_BEGIN\n");

    // Pop the file number

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_EF()
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

BOOL O97_OP_EF
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xEF);

//    printf("O97_OP_EF\n");

    return(TRUE);
}


