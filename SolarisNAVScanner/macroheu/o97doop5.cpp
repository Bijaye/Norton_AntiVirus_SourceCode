//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOP5.CPv   1.0   15 Aug 1997 13:29:06   DCHI  $
//
// Description:
//  Handling of opcodes 0x50-0x5F.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOP5.CPv  $
// 
//    Rev 1.0   15 Aug 1997 13:29:06   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"


//********************************************************************
//
// BOOL O97_OP_50_CASE_ELSE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles the ELSE case of a SELECT statement.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_50_CASE_ELSE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x50);

//    printf("O97_OP_50_CASE_ELSE\n");

    if (O97StateTop(lpstEnv) == esO97_SELECT_BODY)
    {
        if (O97SubStateTop(lpstEnv) == essO97_DO)
        {
            // Skip the rest of the cases

            O97StatePop(lpstEnv);
            O97SubStateNew(lpstEnv,essO97_SKIP);
        }
        else
        {
            O97StatePop(lpstEnv);
        }
    }

    if (O97StateTop(lpstEnv) != esO97_SELECT)
        return(FALSE);

    if (O97SubStateTop(lpstEnv) == essO97_DO)
    {
        // Do it

        O97StatePush(lpstEnv,esO97_SELECT_BODY);
        O97SubStateNew(lpstEnv,essO97_DO);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_51_CASE_EOL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Determines whether to execute the body of the current case.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_51_CASE_EOL
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x51);

//    printf("O97_OP_51_CASE_EOL\n");

    if (O97StateTop(lpstEnv) != esO97_SELECT)
        return(FALSE);

    if (O97SubStateTop(lpstEnv) == essO97_TRUE)
    {
        // Do it

        O97StatePush(lpstEnv,esO97_SELECT_BODY);
        O97SubStateNew(lpstEnv,essO97_DO);
    }
    else
    {
        // Don't do it

        O97StatePush(lpstEnv,esO97_SELECT_BODY);
        O97SubStateNew(lpstEnv,essO97_SKIP);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_52()
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

BOOL O97_OP_52
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x52);

//    printf("O97_OP_52\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_53_CLOSE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  VBA:    Close FileNumList
//  Binary: FN0 FN1 ... FNn 53 00 (n+1)
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_53_CLOSE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x53);

//    printf("O97_OP_53\n");

    if (O97EvalPopParameters(lpstEnv,lpstEnv->wOpValue) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_54_CLOSE_ALL()
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

BOOL O97_OP_54_CLOSE_ALL
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x54);

//    printf("O97_OP_54\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_55_TYPE_CONVERSION()
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

BOOL O97_OP_55_TYPE_CONVERSION
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x55);

//    printf("O97_OP_55_TYPE_CONVERSION\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_56()
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

BOOL O97_OP_56
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x56);

//    printf("O97_OP_56\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_57()
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

BOOL O97_OP_57
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x57);

//    printf("O97_OP_57\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_58()
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

BOOL O97_OP_58
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x58);

//    printf("O97_OP_58\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_59_DEFTYPE()
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

BOOL O97_OP_59_DEFTYPE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x59);

//    printf("O97_OP_59_DEFTYPE\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_5A_DECLARATION()
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

BOOL O97_OP_5A_DECLARATION
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x5A);

    //printf("O97_OP_5A_DECLARATION\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_5B_TYPE_ELEMENT()
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

BOOL O97_OP_5B_TYPE_ELEMENT
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x5B);

//    printf("O97_OP_5B_TYPE_ELEMENT\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_5C_DO()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Performs set up for executing a Do...Loop statement.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_5C_DO
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x5C);

//    printf("O97_OP_5C_DO\n");

    if (O97StateTop(lpstEnv) != esO97_DO_LOOP ||
        (O97ControlTop(lpstEnv) != NULL &&
         ((LPO97_DO_LOOP)O97ControlTop(lpstEnv))->stCondIP.wLine !=
         (WORD)lpstEnv->dwIPLine))
    {
        O97StatePush(lpstEnv,esO97_DO_LOOP);

        if (O97SubStateTop(lpstEnv) != essO97_SKIP)
        {
            // Create a DO_LOOP control structure

            if (O97ControlCreateControl(lpstEnv,ectO97_DO_LOOP) == FALSE)
                return(FALSE);
        }
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_5D()
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

BOOL O97_OP_5D
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x5D);

//    printf("O97_OP_5D\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_5E_DO_UNTIL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Performs set up for evaluating a Do Until...Loop statement.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_5E_DO_UNTIL
(
    LPO97_ENV       lpstEnv
)
{
    BOOL            bResult;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x5E);

//    printf("O97_OP_5E_DO_UNTIL\n");

    // Get the top operand

    if (O97EvalGetAndPopTopAsBoolean(lpstEnv,
                                     &bResult) == FALSE)
    {
        return(FALSE);
    }

    if (O97StateTop(lpstEnv) != esO97_DO_LOOP ||
        (O97ControlTop(lpstEnv) != NULL &&
         ((LPO97_DO_LOOP)O97ControlTop(lpstEnv))->stCondIP.wLine !=
         (WORD)lpstEnv->dwIPLine))
    {
        O97StatePush(lpstEnv,esO97_DO_LOOP);

        if (O97SubStateTop(lpstEnv) != essO97_SKIP && bResult != TRUE)
        {
            // Create a DO_LOOP control structure

            if (O97ControlCreateControl(lpstEnv,ectO97_DO_LOOP) == FALSE)
                return(FALSE);
        }
    }

    if (bResult != FALSE)
    {
        O97SubStateNew(lpstEnv,essO97_SKIP);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_5F_DO_WHILE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Performs set up for evaluating a Do While...Loop statement.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_5F_DO_WHILE
(
    LPO97_ENV       lpstEnv
)
{
    BOOL            bResult;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x5F);

//    printf("O97_OP_5F_DO_WHILE\n");

    // Get the top operand

    if (O97EvalGetAndPopTopAsBoolean(lpstEnv,
                                     &bResult) == FALSE)
    {
        return(FALSE);
    }

    if (O97StateTop(lpstEnv) != esO97_DO_LOOP ||
        (O97ControlTop(lpstEnv) != NULL &&
         ((LPO97_DO_LOOP)O97ControlTop(lpstEnv))->stCondIP.wLine !=
         (WORD)lpstEnv->dwIPLine))
    {
        O97StatePush(lpstEnv,esO97_DO_LOOP);

        if (O97SubStateTop(lpstEnv) != essO97_SKIP && bResult == TRUE)
        {
            // Create a DO_LOOP control structure

            if (O97ControlCreateControl(lpstEnv,ectO97_DO_LOOP) == FALSE)
                return(FALSE);
        }
    }

    if (bResult == FALSE)
    {
        O97SubStateNew(lpstEnv,essO97_SKIP);
    }

    return(TRUE);
}


