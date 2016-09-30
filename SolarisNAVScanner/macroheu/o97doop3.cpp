//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOP3.CPv   1.1   12 Oct 1998 13:21:46   DCHI  $
//
// Description:
//  Handling of opcodes 0x30-0x3F.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOP3.CPv  $
// 
//    Rev 1.1   12 Oct 1998 13:21:46   DCHI
// Filled in O97_OP_35_ROOT_DOT_EVAL() to handle With reference.
// 
//    Rev 1.0   15 Aug 1997 13:29:02   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"


//********************************************************************
//
// BOOL O97_OP_30()
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

BOOL O97_OP_30
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x30);

//    printf("O97_OP_30\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_31()
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

BOOL O97_OP_31
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x31);

//    printf("O97_OP_31\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_32()
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

BOOL O97_OP_32
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x32);

//    printf("O97_OP_32\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_33()
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

BOOL O97_OP_33
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x33);

//    printf("O97_OP_33\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_34()
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

BOOL O97_OP_34
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x34);

//    printf("O97_OP_34\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_35_ROOT_DOT_EVAL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pushes a zero and ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_35_ROOT_DOT_EVAL
(
    LPO97_ENV       lpstEnv
)
{
    assert(lpstEnv->abyOpcode[0] == 0x35);

//    printf("O97_OP_35_ROOT_DOT_EVAL\n");

    if (lpstEnv->nWithDepth > 0)
    {
        BOOL        bKnown;

        // No parameters

        lpstEnv->nNumParams = 0;

        if (lpstEnv->lpstObjectFunc->
            SubObjectEvaluate(lpstEnv,
                              0,
                              eO97_TYPE_OBJECT_ID,
                              lpstEnv->adwWithObjIDs + lpstEnv->nWithDepth - 1,
                              &bKnown) == FALSE)
            return(FALSE);

        // For now, don't do external calls with no parameters

        if (bKnown != FALSE)
            return(TRUE);
    }

    return O97EvalPushLongOperand(lpstEnv,0);
}


//********************************************************************
//
// BOOL O97_OP_36()
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

BOOL O97_OP_36
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x36);

//    printf("O97_OP_36\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_37_ROOT_DOT_FUNCTION_EVAL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pops the parameters, pushes a zero, and ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_37_ROOT_DOT_FUNCTION_EVAL
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x37);

//    printf("O97_OP_37_ROOT_DOT_FUNCTION_EVAL\n");

    // Pop the parameters and then the parameters to assign

    if (O97ParamPopParams(lpstEnv) == NULL)
        return(FALSE);

    return O97EvalPushLongOperand(lpstEnv,0);
}


//********************************************************************
//
// BOOL O97_OP_38()
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

BOOL O97_OP_38
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x38);

//    printf("O97_OP_38\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_39_ASSIGN_ROOT_DOT()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pops the value to assign, and ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_39_ASSIGN_ROOT_DOT
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x39);

//    printf("O97_OP_39_ASSIGN_ROOT_DOT\n");

    // Pop the value to assign

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);
    
    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_3A()
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

BOOL O97_OP_3A
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x3A);

//    printf("O97_OP_3A\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_3B_ASSIGN_ROOT_DOT_FUNCTION()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pops the parameters, the value to assign, and then
//  ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_3B_ASSIGN_ROOT_DOT_FUNCTION
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x3B);

//    printf("O97_OP_3B_ASSIGN_ROOT_DOT_FUNCTION\n");

    // Pop the parameters and then the value to assign

    if (O97ParamPopParams(lpstEnv) == NULL)
        return(FALSE);

    // Pop the value to assign

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);
    
    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_3C()
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

BOOL O97_OP_3C
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x3C);

//    printf("O97_OP_3C\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_3D()
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

BOOL O97_OP_3D
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x3D);

//    printf("O97_OP_3D\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_3E()
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

BOOL O97_OP_3E
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x3E);

//    printf("O97_OP_3E\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_3F()
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

BOOL O97_OP_3F
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x3F);

//    printf("O97_OP_3F\n");

    return(TRUE);
}


