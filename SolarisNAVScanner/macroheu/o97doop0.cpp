//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOP0.CPv   1.0   15 Aug 1997 13:28:58   DCHI  $
//
// Description:
//  Handling of opcodes 0x00-0x0F.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOP0.CPv  $
// 
//    Rev 1.0   15 Aug 1997 13:28:58   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"


//********************************************************************
//
// BOOL O97_OP_00_IMP()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Evaluates the operator Imp.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_00_IMP
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x00);

//    printf("O97_OP_00_IMP\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_IMP);
}


//********************************************************************
//
// BOOL O97_OP_01_EQV()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Evaluates the operator Eqv.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_01_EQV
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x01);

//    printf("O97_OP_01_EQV\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_EQV);
}


//********************************************************************
//
// BOOL O97_OP_02_XOR()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the XOR operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_02_XOR
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x02);

    //printf("O97_OP_02_XOR\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_OR);
}


//********************************************************************
//
// BOOL O97_OP_03_OR()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the OR operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_03_OR
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x03);

    //printf("O97_OP_03_OR\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_OR);
}


//********************************************************************
//
// BOOL O97_OP_04_AND()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the AND operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_04_AND
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x04);

    //printf("O97_OP_04_AND\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_AND);
}


//********************************************************************
//
// BOOL O97_OP_05_EQ()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the EQ operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_05_EQ
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x05);

    //printf("O97_OP_05_EQ\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_EQ);
}


//********************************************************************
//
// BOOL O97_OP_06_NE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the NE operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_06_NE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x06);

    //printf("O97_OP_06_NE\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_NE);
}


//********************************************************************
//
// BOOL O97_OP_07_LE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the LE operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_07_LE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x07);

    //printf("O97_OP_07_LE\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_LE);
}


//********************************************************************
//
// BOOL O97_OP_08_GE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the GE operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_08_GE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x08);

    //printf("O97_OP_08_GE\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_GE);
}


//********************************************************************
//
// BOOL O97_OP_09_LT()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the LT operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_09_LT
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x09);

    //printf("O97_OP_09_LT\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_LT);
}


//********************************************************************
//
// BOOL O97_OP_0A_GT()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the GT operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_0A_GT
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x0A);

    //printf("O97_OP_0A_GT\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_GT);
}


//********************************************************************
//
// BOOL O97_OP_0B_PLUS()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the ADD operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_0B_PLUS
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x0B);

    //printf("O97_OP_0B_PLUS\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_ADD);
}


//********************************************************************
//
// BOOL O97_OP_0C_MINUS()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the SUB operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_0C_MINUS
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x0C);

    //printf("O97_OP_0C_MINUS\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_SUB);
}


//********************************************************************
//
// BOOL O97_OP_0D_MOD()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the MOD operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_0D_MOD
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x0D);

    //printf("O97_OP_0D_MOD\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_MOD);
}


//********************************************************************
//
// BOOL O97_OP_0E_INT_DIV()
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

BOOL O97_OP_0E_INT_DIV
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x0E);

    //printf("O97_OP_0E_INT_DIV\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_DIV);
}


//********************************************************************
//
// BOOL O97_OP_0F_ASTERISK()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Applies the MUL operator to the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_0F_ASTERISK
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x0F);

    //printf("O97_OP_0F_ASTERISK\n");

    return O97EvalDoOperator(lpstEnv,eO97_OPERATOR_MUL);
}


