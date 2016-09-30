//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOP9.CPv   1.0   15 Aug 1997 13:29:14   DCHI  $
//
// Description:
//  Handling of opcodes 0x90-0x9F.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOP9.CPv  $
// 
//    Rev 1.0   15 Aug 1997 13:29:14   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"


//********************************************************************
//
// BOOL O97_OP_90()
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

BOOL O97_OP_90
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x90);

//    printf("O97_OP_90\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_91()
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

BOOL O97_OP_91
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x91);

//    printf("O97_OP_91\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_92()
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

BOOL O97_OP_92
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x92);

//    printf("O97_OP_92\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_93_GOTO()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Assumption is that we will get back to this state eventually
//  if we don't find the label.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_93_GOTO
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x93);

//    printf("O97_OP_93_GOTO\n");

    if (O97SubStateTop(lpstEnv) == essO97_DO)
    {
        lpstEnv->eStateGoto = eO97_STATE_GOTO_0;
        lpstEnv->wGotoLabelID = lpstEnv->wOpID;
        lpstEnv->dwGotoIPLine = lpstEnv->dwIPLine;
        lpstEnv->dwGotoIPOffset = lpstEnv->dwIPOffset;

        // Need to skip everything except labels

        O97SubStateNew(lpstEnv,essO97_SKIP);
    }
    else
    if (lpstEnv->eStateGoto == eO97_STATE_GOTO_1 &&
        lpstEnv->dwGotoIPLine == lpstEnv->dwIPLine &&
        lpstEnv->dwGotoIPOffset == lpstEnv->dwIPOffset)
    {
        // Could not find the label

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_94_IF_THEN_SINGLE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Evaluates the condition of a single line If...Then statement.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_94_IF_THEN_SINGLE
(
    LPO97_ENV       lpstEnv
)
{
    BOOL            bResult;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x94);

//    printf("O97_OP_94_IF_THEN_SINGLE\n");

    // Get the top operand

    if (O97EvalGetAndPopTopAsBoolean(lpstEnv,
                                     &bResult) == FALSE)
    {
        return(FALSE);
    }

    O97StatePush(lpstEnv,esO97_IF_COND);
    if (O97SubStateTop(lpstEnv) == essO97_DO)
    {
        if (bResult == FALSE)
        {
            O97SubStateNew(lpstEnv,essO97_FALSE);
            O97StatePush(lpstEnv,esO97_THEN_SINGLE);
            O97SubStateNew(lpstEnv,essO97_SKIP);
        }
        else
        {
            O97SubStateNew(lpstEnv,essO97_TRUE);
            O97StatePush(lpstEnv,esO97_THEN_SINGLE);
            O97SubStateNew(lpstEnv,essO97_DO);
        }
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_95_IF_THEN_MULTIPLE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Evaluates the condition of a multi-line If...Then statement.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_95_IF_THEN_MULTIPLE
(
    LPO97_ENV       lpstEnv
)
{
    BOOL            bResult;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x95);

//    printf("O97_OP_95_IF_THEN_MULTIPLE\n");

    // Get the top operand

    if (O97EvalGetAndPopTopAsBoolean(lpstEnv,
                                     &bResult) == FALSE)
    {
        return(FALSE);
    }

    O97StatePush(lpstEnv,esO97_IF_COND);
    if (O97SubStateTop(lpstEnv) == essO97_DO)
    {
        if (bResult == FALSE)
        {
            O97SubStateNew(lpstEnv,essO97_FALSE);
            O97StatePush(lpstEnv,esO97_THEN_MULTIPLE);
            O97SubStateNew(lpstEnv,essO97_SKIP);
        }
        else
        {
            O97SubStateNew(lpstEnv,essO97_TRUE);
            O97StatePush(lpstEnv,esO97_THEN_MULTIPLE);
            O97SubStateNew(lpstEnv,essO97_DO);
        }
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_96()
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

BOOL O97_OP_96
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x96);

//    printf("O97_OP_96\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_97()
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

BOOL O97_OP_97
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x97);

//    printf("O97_OP_97\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_98()
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

BOOL O97_OP_98
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x98);

//    printf("O97_OP_98\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_99()
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

BOOL O97_OP_99
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x99);

//    printf("O97_OP_99\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_9A()
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

BOOL O97_OP_9A
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x9A);

//    printf("O97_OP_9A\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_9B_LABEL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Determines whether the label is that of an outstanding goto.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_9B_LABEL
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x9B);

//    printf("O97_OP_9B_LABEL\n");

    if (lpstEnv->eStateGoto != eO97_STATE_GOTO_NONE &&
        lpstEnv->wGotoLabelID == lpstEnv->wOpID)
    {
        lpstEnv->eStateGoto = eO97_STATE_GOTO_NONE;
        O97SubStateNew(lpstEnv,essO97_DO);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_9C()
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

BOOL O97_OP_9C
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x9C);

//    printf("O97_OP_9C\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_9D()
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

BOOL O97_OP_9D
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x9D);

//    printf("O97_OP_9D\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_9E_BROKEN_LINE()
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

BOOL O97_OP_9E_BROKEN_LINE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x9E);

//    printf("O97_OP_9E_BROKEN_LINE\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_9F()
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

BOOL O97_OP_9F
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x9F);

//    printf("O97_OP_9F\n");

    return(TRUE);
}


