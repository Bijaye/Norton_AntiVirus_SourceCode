//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOP1.CPv   1.0   15 Aug 1997 13:29:00   DCHI  $
//
// Description:
//  Handling of opcodes 0x10-0x1F.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOP1.CPv  $
// 
//    Rev 1.0   15 Aug 1997 13:29:00   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"


//********************************************************************
//
// BOOL O97_OP_10_SLASH()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the DIV operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_10_SLASH
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x10);

    //printf("O97_OP_10_SLASH\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_DIV);
}


//********************************************************************
//
// BOOL O97_OP_11_AMPERSAND()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the concatenate operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_11_AMPERSAND
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x11);

    //printf("O97_OP_11_AMPERSAND\n");

    return O97DoOperatorConcatenate(lpstEnv);
}


//********************************************************************
//
// BOOL O97_OP_12_LIKE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the Like operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_12_LIKE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x12);

//    printf("O97_OP_12_LIKE\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_LIKE);
}


//********************************************************************
//
// BOOL O97_OP_13_CARAT()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the power operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_13_CARAT
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x13);

//    printf("O97_OP_13_CARAT\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_POWER);
}


//********************************************************************
//
// BOOL O97_OP_14_IS()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the Is operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_14_IS
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x14);

//    printf("O97_OP_14_IS\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_IS);
}


//********************************************************************
//
// BOOL O97_OP_15_NOT()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the NOT operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_15_NOT
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x15);

    //printf("O97_OP_15_NOT\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_NOT);
}


//********************************************************************
//
// BOOL O97_OP_16_NEGATE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the NEGATE operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_16_NEGATE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x16);

    //printf("O97_OP_16_NEGATE\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_NEG);
}


//********************************************************************
//
// BOOL O97_OP_17_ABS()
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

BOOL O97_OP_17_ABS
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x17);

//    printf("O97_OP_17_ABS\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_18_FIX()
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

BOOL O97_OP_18_FIX
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x18);

//    printf("O97_OP_18_FIX\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_19_INT()
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

BOOL O97_OP_19_INT
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x19);

//    printf("O97_OP_19_INT\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_1A_SGN()
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

BOOL O97_OP_1A_SGN
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x1A);

//    printf("O97_OP_1A_SGN\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_1B_LEN()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Calculates the length of the top operand.  If the operand
//  is not a string, the length is zero.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_1B_LEN
(
    LPO97_ENV       lpstEnv
)
{
    long            lLen;
    LPBYTE          lpbyStr;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x1B);

//    printf("O97_OP_1B_LEN\n");

    // Get the length of the string

    if (O97EvalQueryTopOperandAsString(lpstEnv,
                                       &lpbyStr) == FALSE)
        lLen = 0;
    else
        lLen = lpbyStr[0];

    // Pop the top operand

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

    return O97EvalPushLongOperand(lpstEnv,lLen);
}


//********************************************************************
//
// BOOL O97_OP_1C_LENB()
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

BOOL O97_OP_1C_LENB
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x1C);

//    printf("O97_OP_1C_LENB\n");

    // Just push a one

    return O97EvalPushLongOperand(lpstEnv,1);
}


//********************************************************************
//
// BOOL O97_OP_1D_PARENTHESIZE()
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

BOOL O97_OP_1D_PARENTHESIZE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x1D);

//    printf("O97_OP_1D_PARENTHESIZE\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_1E()
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

BOOL O97_OP_1E
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x1E);

//    printf("O97_OP_1E\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_1F()
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

BOOL O97_OP_1F
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x1F);            

//    printf("O97_OP_1F\n");

    return(TRUE);
}




