//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOP7.CPv   1.1   12 Oct 1998 13:23:12   DCHI  $
//
// Description:
//  Handling of opcodes 0x70-0x7F.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOP7.CPv  $
// 
//    Rev 1.1   12 Oct 1998 13:23:12   DCHI
// Added handling of state essO97_DO in O97_OP_7D_INSTR_TWO_PARAMS()
// and in O97_OP_7E_INSTR_THREE_PARAMS().
// 
//    Rev 1.0   15 Aug 1997 13:29:10   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"

#include "wbutil.h"

//********************************************************************
//
// BOOL O97_OP_70()
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

BOOL O97_OP_70
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x70);

//    printf("O97_OP_70\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_71_EXIT_DO()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles Exit Do.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_71_EXIT_DO
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x71);

//    printf("O97_OP_71_EXIT_DO\n");

    if (O97SubStateTop(lpstEnv) == essO97_DO &&
        lpstEnv->eStateExit == eO97_STATE_EXIT_NONE)
    {
        lpstEnv->eStateExit = eO97_STATE_EXIT_DO;
        O97SubStateNew(lpstEnv,essO97_SKIP);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_72_EXIT_FOR()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles Exit For.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_72_EXIT_FOR
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x72);

//    printf("O97_OP_72_EXIT_FOR\n");

    if (O97SubStateTop(lpstEnv) == essO97_DO &&
        lpstEnv->eStateExit == eO97_STATE_EXIT_NONE)
    {
        lpstEnv->eStateExit = eO97_STATE_EXIT_FOR;
        O97SubStateNew(lpstEnv,essO97_SKIP);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_73_EXIT_FUNCTION()
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

BOOL O97_OP_73_EXIT_FUNCTION
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x73);

//    printf("O97_OP_73_EXIT_FUNCTION\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_74_EXIT_PROPERTY()
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

BOOL O97_OP_74_EXIT_PROPERTY
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x74);

//    printf("O97_OP_74_EXIT_PROPERTY\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_75_EXIT_SUB()
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

BOOL O97_OP_75_EXIT_SUB
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x75);

//    printf("O97_OP_75_EXIT_SUB\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_76()
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

BOOL O97_OP_76
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x76);

//    printf("O97_OP_76\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_77()
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

BOOL O97_OP_77
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x77);

//    printf("O97_OP_77\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_78()
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

BOOL O97_OP_78
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x78);

//    printf("O97_OP_78\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_79()
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

BOOL O97_OP_79
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x79);

//    printf("O97_OP_79\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_7A()
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

BOOL O97_OP_7A
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x7A);

//    printf("O97_OP_7A\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_7B()
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

BOOL O97_OP_7B
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x7B);

//    printf("O97_OP_7B\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_7C()
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

BOOL O97_OP_7C
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x7C);

//    printf("O97_OP_7C\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_7D_INSTR_TWO_PARAMS()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles Instr with two parameters.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_7D_INSTR_TWO_PARAMS
(
    LPO97_ENV       lpstEnv
)
{
    LPBYTE          lpbySrc;
    LPBYTE          lpbySearch;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x7D);

//    printf("O97_OP_7D_INSTR_TWO_PARAMS\n");

    if (O97SubStateTop(lpstEnv) == essO97_DO)
    {
        // Get search string

        if (O97EvalGetAndPopTopAsString(lpstEnv,&lpbySearch) == FALSE)
            return(FALSE);

        // Get source string

        if (O97EvalGetAndPopTopAsString(lpstEnv,&lpbySrc) == FALSE)
            return(FALSE);

        // Do the InStr(String$,Find$) operation

        if (O97EvalPushLongOperand(lpstEnv,
                                   PascalStrInStr(lpbySrc,
                                                  1,
                                                  lpbySearch)) == FALSE)
            return(FALSE);
    }
    else
    {
        if (O97EvalPopParameters(lpstEnv,2) == FALSE)
            return(FALSE);

        if (O97EvalPushLongOperand(lpstEnv,0) == FALSE)
            return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_7E_INSTR_THREE_PARAMS()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles Instr with three parameters.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_7E_INSTR_THREE_PARAMS
(
    LPO97_ENV       lpstEnv
)
{
    LPBYTE          lpbySrc;
    LPBYTE          lpbySearch;
    long            lStart;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x7E);

//    printf("O97_OP_7E_INSTR_THREE_PARAMS\n");

    if (O97SubStateTop(lpstEnv) == essO97_DO)
    {
        // Get search string

        if (O97EvalGetAndPopTopAsString(lpstEnv,&lpbySearch) == FALSE)
            return(FALSE);

        // Get source string

        if (O97EvalGetAndPopTopAsString(lpstEnv,&lpbySrc) == FALSE)
            return(FALSE);

        // Get the start offset

        if (O97EvalGetAndPopTopAsLong(lpstEnv,&lStart) == FALSE)
            lStart = 1;

        // Do the InStr(String$,Find$) operation

        if (O97EvalPushLongOperand(lpstEnv,
                                   PascalStrInStr(lpbySrc,
                                                  lStart,
                                                  lpbySearch)) == FALSE)
            return(FALSE);
    }
    else
    {
        if (O97EvalPopParameters(lpstEnv,3) == FALSE)
            return(FALSE);

        if (O97EvalPushLongOperand(lpstEnv,0) == FALSE)
            return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_7F_INSTR_FOUR_PARAMS()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pops the four parameters off of the evaluation stack and
//  ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_7F_INSTR_FOUR_PARAMS
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x7F);

//    printf("O97_OP_7F_INSTR_FOUR_PARAMS\n");

    return O97EvalPopParameters(lpstEnv,4);
}


