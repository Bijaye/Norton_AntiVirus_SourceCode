//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOPA.CPv   1.0   15 Aug 1997 13:29:16   DCHI  $
//
// Description:
//  Handling of opcodes 0xA0-0xAF.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOPA.CPv  $
// 
//    Rev 1.0   15 Aug 1997 13:29:16   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"


//********************************************************************
//
// BOOL O97_OP_A0()
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

BOOL O97_OP_A0
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xA0);

//    printf("O97_OP_A0\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_A1()
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

BOOL O97_OP_A1
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xA1);

//    printf("O97_OP_A1\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_A2()
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

BOOL O97_OP_A2
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xA2);

//    printf("O97_OP_A2\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_A3_NO_LEN()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  So far this is assumed to only be used in an Open to specify
//  no record length specification.  Pushes a zero.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_A3_NO_LEN
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xA3);

//    printf("O97_OP_A3\n");

    if (O97EvalPushLongOperand(lpstEnv,0) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_A4_WORD()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pushes the WORD value onto the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_A4_WORD
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xA4);

    //printf("O97_OP_A4_WORD\n");

    return O97EvalPushLongOperand(lpstEnv,(long)lpstEnv->wOpValue);
}


//********************************************************************
//
// BOOL O97_OP_A5_DWORD()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pushes the DWORD value onto the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_A5_DWORD
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xA5);

    //printf("O97_OP_A5_DWORD\n");

    return O97EvalPushLongOperand(lpstEnv,(long)lpstEnv->dwOpValue);
}


//********************************************************************
//
// BOOL O97_OP_A6_HEX_WORD()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pushes the WORD value onto the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_A6_HEX_WORD
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xA6);

//    printf("O97_OP_A6_HEX_WORD\n");

    return O97EvalPushLongOperand(lpstEnv,(long)lpstEnv->wOpValue);
}


//********************************************************************
//
// BOOL O97_OP_A7_HEX_DWORD()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pushes the DWORD value onto the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_A7_HEX_DWORD
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xA7);

//    printf("O97_OP_A7_HEX_DWORD\n");

    return O97EvalPushLongOperand(lpstEnv,(long)lpstEnv->dwOpValue);
}


//********************************************************************
//
// BOOL O97_OP_A8_NOTHING()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pushes a zero onto the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_A8_NOTHING
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xA8);

    //printf("O97_OP_A8_NOTHING\n");

    return O97EvalPushLongOperand(lpstEnv,0);
}


//********************************************************************
//
// BOOL O97_OP_A9()
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

BOOL O97_OP_A9
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xA9);

//    printf("O97_OP_A9\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_AA()
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

BOOL O97_OP_AA
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xAA);

//    printf("O97_OP_AA\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_AB()
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

BOOL O97_OP_AB
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xAB);

//    printf("O97_OP_AB\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_AC_DOUBLE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pushes a zero onto the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_AC_DOUBLE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xAC);

    //printf("O97_OP_AC_DOUBLE\n");

    return O97EvalPushLongOperand(lpstEnv,0);
}


//********************************************************************
//
// BOOL O97_OP_AD()
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

BOOL O97_OP_AD
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xAD);

//    printf("O97_OP_AD\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_AE_STRING()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pushes the string onto the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_AE_STRING
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xAE);

//    printf("O97_OP_AE_STRING\n");

    return O97EvalPushOperand(lpstEnv,
                              eO97_TYPE_STRING,
                              lpstEnv->abyOpString);
}


//********************************************************************
//
// BOOL O97_OP_AF_TRUE_OR_FALSE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pushes 0 for FALSE if abyOpcode[1] is zero.  Otherwise, pushes
//  -1 for TRUE.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_AF_TRUE_OR_FALSE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0xAF);            

    //printf("O97_OP_AF_TRUE_OR_FALSE\n");

    // Assume FALSE if abyOpcode[1] is zero

    if (lpstEnv->abyOpcode[1] == 0)
        return O97EvalPushLongOperand(lpstEnv,0);

    // Assume TRUE

    return O97EvalPushLongOperand(lpstEnv,-1);
}




