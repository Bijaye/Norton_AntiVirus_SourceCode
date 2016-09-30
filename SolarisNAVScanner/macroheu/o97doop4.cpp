//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOP4.CPv   1.1   12 Oct 1998 13:22:06   DCHI  $
//
// Description:
//  Handling of opcodes 0x40-0x4F.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOP4.CPv  $
// 
//    Rev 1.1   12 Oct 1998 13:22:06   DCHI
// - Added function call support to O97_OP_41_CALL().
// - Added VAR_ID to VAR translation in O97_OP_42_DOT_CALL().
// - Modified O97_OP_42_DOT_CALL() to just return TRUE instead
//   of FALSE with unknown parent type.
// 
//    Rev 1.0   15 Aug 1997 13:29:04   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"

#include "wbutil.h"

//********************************************************************
//
// BOOL O97_OP_40()
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

BOOL O97_OP_40
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x40);

//    printf("O97_OP_40\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_41_CALL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Calls an internal subroutine of the module.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_41_CALL
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x41);

//    printf("O97_OP_41_CALL\n");

    // Push the parameters onto the parameter stack

    if (O97ParamGetParams(lpstEnv) == FALSE)
        return(FALSE);

    // Just ignore the call

    if (O97SubStateTop(lpstEnv) != essO97_DO)
        return(TRUE);

    // Prepare for the call

    if (O97CallSetup(lpstEnv,
                     lpstEnv->wOpID,
                     lpstEnv->wOpID) == FALSE)
    {
        // Just ignore it

        return(TRUE);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_42_DOT_CALL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Makes an intermodule call.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_42_DOT_CALL
(
    LPO97_ENV       lpstEnv
)
{
    BOOL            bKnown;
    EO97_TYPE_T     eParentType;
    LPVOID          lpvParentData;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x42);

//    printf("O97_OP_42_DOT_CALL\n");

    // There must be some object on the evaluation stack

    if (O97EvalQueryTopOperand(lpstEnv,
                               &eParentType,
                               &lpvParentData) == FALSE)
        return(FALSE);

    // Pop the top operand; SubObjectEvaluate() should know
    //  that the parent is transient

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

    switch (eParentType)
    {
        case eO97_TYPE_O97_OBJECT_ID:
        case eO97_TYPE_OBJECT_ID:
            break;

        case eO97_TYPE_VAR_ID:
        {
            LPO97_VAR       lpstVar;

            lpstVar = O97VarGet(lpstEnv,*(LPWORD)lpvParentData);
            if (lpstVar != NULL)
            {
                eParentType= eO97_TYPE_VAR;
                lpvParentData = lpstVar;
            }

            break;
        }

        default:
            // Ignore it

            return(TRUE);
    }

    // Push the parameters onto the parameter stack

    if (O97ParamGetParams(lpstEnv) == FALSE)
        return(FALSE);

    // Evaluate the subobject with parameters

    if (lpstEnv->lpstObjectFunc->
        SubObjectEvaluate(lpstEnv,
                          lpstEnv->wOpCount,
                          eParentType,
                          lpvParentData,
                          &bKnown) == FALSE)
        return(FALSE);

    if (bKnown == TRUE ||
        O97SubStateTop(lpstEnv) != essO97_DO)
        return(TRUE);

    // Could be a string call

    if (lpstEnv->bIsStringCall == TRUE)
        O97SetupCallUsingString(lpstEnv);

    if (eParentType == eO97_TYPE_VAR_ID ||
        lpstEnv->bIsStringCall == TRUE)
    {
        DWORD       dwModuleEntry;
        WORD        wModuleIndex;

        if (lpstEnv->bIsStringCall == FALSE)
        {
            lpstEnv->wCallModuleID = *(LPWORD)lpvParentData;
            lpstEnv->wCallRoutineID = lpstEnv->wOpID;
        }
        else
        {
            // Both the module and routine ID should have already
            //  been set up

            lpstEnv->bIsStringCall = FALSE;
        }

        // Possibly an external module routine call

        if (lpstEnv->wCallModuleID != O97_ID_INVALID)
        {
            dwModuleEntry =
                O97GetModuleEntryNum(lpstEnv,
                                     lpstEnv->wCallModuleID,
                                     &wModuleIndex);

            // Time for an external module routine call

            if (dwModuleEntry != 0)
            {
                if (O97ExternalCallSetup(lpstEnv,
                                         wModuleIndex,
                                         dwModuleEntry) == FALSE)
                    return(FALSE);

                if (O97CallSetup(lpstEnv,
                                 O97_ID_INVALID,
                                 lpstEnv->wCallRoutineID) == FALSE)
                {
                    O97ExternalCallCleanup(lpstEnv);

                    return(FALSE);
                }
            }
        }
        else
        if (lpstEnv->wCallRoutineID != O97_ID_INVALID)
        {
            if (O97CallSetup(lpstEnv,
                             O97_ID_INVALID,
                             lpstEnv->wCallRoutineID) == FALSE)
                return(FALSE);
        }
    }

    // Ignore it

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_43_ROOT_DOT_CALL()
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

BOOL O97_OP_43_ROOT_DOT_CALL
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x43);

//    printf("O97_OP_43_ROOT_DOT_CALL\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_44()
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

BOOL O97_OP_44
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x44);

//    printf("O97_OP_44\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_45_COLON()
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

BOOL O97_OP_45_COLON
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x45);

    //printf("O97_OP_45_COLON\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_46_THEN_ELSE_SINGLE()
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

BOOL O97_OP_46_THEN_ELSE_SINGLE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x46);

//    printf("O97_OP_46_THEN_ELSE_SINGLE\n");

    // This appears to be extraneous since 94 00/60 00 precede

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_47()
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

BOOL O97_OP_47
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x47);

//    printf("O97_OP_47\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_SELECT_SINGLE_ARG()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Handles the select for single argument cases.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_SELECT_SINGLE_ARG
(
    LPO97_ENV       lpstEnv
)
{
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

    // Skip the case?

    if (O97SubStateTop(lpstEnv) == essO97_DO)
    {
        EO97_TYPE_T     eType;
        LPVOID          lpvData;
        LPO97_SELECT    lpstSelect;

        lpstSelect = (LPO97_SELECT)O97ControlTop(lpstEnv);

        if (lpstSelect == NULL)
            return(FALSE);

        // Get the top operand

        if (O97EvalGetTopBaseData(lpstEnv,&eType,&lpvData) == FALSE)
            return(FALSE);

        // Test the top operand

        if (eType == eO97_TYPE_LONG || eType == eO97_TYPE_STRING)
        {
            BOOL    bMatch = FALSE;

            if (eType == eO97_TYPE_STRING &&
                lpstSelect->eType == eO97_TYPE_STRING)
            {
                int             nCompare;

                // Compare the strings

                nCompare = PascalStrCmp(lpstSelect->uData.lpbyStr,
                                        (LPBYTE)lpvData);

                // Do the comparison

                switch (lpstEnv->abyOpcode[0])
                {
                    case eO97_OP_48_CASE:
                    case eO97_OP_4F_CASE_EQ:
                        if (nCompare == 0)
                            bMatch = TRUE;
                        break;

                    case eO97_OP_4A_CASE_GT:
                        if (nCompare > 0)
                            bMatch = TRUE;
                        break;

                    case eO97_OP_4B_CASE_LT:
                        if (nCompare < 0)
                            bMatch = TRUE;
                        break;

                    case eO97_OP_4C_CASE_GE:
                        if (nCompare >= 0)
                            bMatch = TRUE;
                        break;

                    case eO97_OP_4D_CASE_LE:
                        if (nCompare <= 0)
                            bMatch = TRUE;
                        break;

                    case eO97_OP_4E_CASE_NE:
                        if (nCompare != 0)
                            bMatch = TRUE;
                        break;

                    default:
                        return(FALSE);
                }
            }
            else
            {
                long            lValue0;
                long            lValue1;

                // Convert the other operand to a long

                if (lpstSelect->eType == eO97_TYPE_STRING)
                    lValue0 = PascalStrToLong(lpstSelect->uData.lpbyStr);
                else
                    lValue0 = lpstSelect->uData.lValue;

                if (eType == eO97_TYPE_STRING)
                    lValue1 = PascalStrToLong((LPBYTE)lpvData);
                else
                    lValue1 = *(LPLONG)lpvData;

                // Do the comparison

                switch (lpstEnv->abyOpcode[0])
                {
                    case eO97_OP_48_CASE:
                    case eO97_OP_4F_CASE_EQ:
                        if (lValue0 == lValue1)
                            bMatch = TRUE;
                        break;

                    case eO97_OP_4A_CASE_GT:
                        if (lValue0 > lValue1)
                            bMatch = TRUE;
                        break;

                    case eO97_OP_4B_CASE_LT:
                        if (lValue0 < lValue1)
                            bMatch = TRUE;
                        break;

                    case eO97_OP_4C_CASE_GE:
                        if (lValue0 >= lValue1)
                            bMatch = TRUE;
                        break;

                    case eO97_OP_4D_CASE_LE:
                        if (lValue0 <= lValue1)
                            bMatch = TRUE;
                        break;

                    case eO97_OP_4E_CASE_NE:
                        if (lValue0 != lValue1)
                            bMatch = TRUE;
                        break;

                    default:
                        return(FALSE);
                }
            }

            if (bMatch == TRUE)
            {
                // Found a TRUE case

                O97SubStateNew(lpstEnv,essO97_TRUE);
            }
        }
    }

    // Pop the top operand

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_48_CASE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Calls O97_OP_SELECT_SINGLE_ARG().
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_48_CASE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x48);

//    printf("O97_OP_48_CASE\n");

    return O97_OP_SELECT_SINGLE_ARG(lpstEnv);
}


//********************************************************************
//
// BOOL O97_OP_49_CASE_TO()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  VBA:    Case A to B
//  Code:   A B 49 00 51 00
//
//  Determines whether the select value is inclusively within
//  the given bounds.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_49_CASE_TO
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x49);

//    printf("O97_OP_49_CASE_TO\n");

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

    // Skip the case?

    if (O97SubStateTop(lpstEnv) == essO97_DO)
    {
        EO97_TYPE_T     eType;
        LPVOID          lpvData;
        LPO97_SELECT    lpstSelect;
        BOOL            bMatch = FALSE;
        long            lValue0;
        long            lValue1;
        int             nCompare;

        lpstSelect = (LPO97_SELECT)O97ControlTop(lpstEnv);

        if (lpstSelect == NULL)
            return(FALSE);

        // Get the top operand

        if (O97EvalGetTopBaseData(lpstEnv,&eType,&lpvData) == FALSE)
            return(FALSE);

        // Test the top operand

        if (eType == eO97_TYPE_LONG || eType == eO97_TYPE_STRING)
        {
            if (eType == eO97_TYPE_STRING &&
                lpstSelect->eType == eO97_TYPE_STRING)
            {
                // Compare the strings

                nCompare = PascalStrCmp(lpstSelect->uData.lpbyStr,
                                        (LPBYTE)lpvData);

                // The select value must be <= the top operand

                if (nCompare <= 0)
                    bMatch = TRUE;
            }
            else
            {
                // Convert the other operand to a long

                if (lpstSelect->eType == eO97_TYPE_STRING)
                    lValue0 = PascalStrToLong(lpstSelect->uData.lpbyStr);
                else
                    lValue0 = lpstSelect->uData.lValue;

                if (eType == eO97_TYPE_STRING)
                    lValue1 = PascalStrToLong((LPBYTE)lpvData);
                else
                    lValue1 = *(LPLONG)lpvData;

                // The select value must be <= the top operand

                if (lValue0 <= lValue1)
                    bMatch = TRUE;
            }
        }

        // Pop the top operand

        if (O97EvalPopTopOperand(lpstEnv) == FALSE)
            return(FALSE);

        if (bMatch == TRUE)
        {
            // Get the top operand

            if (O97EvalGetTopBaseData(lpstEnv,&eType,&lpvData) == FALSE)
                return(FALSE);

            // Test the top operand

            if (eType == eO97_TYPE_LONG || eType == eO97_TYPE_STRING)
            {
                if (eType == eO97_TYPE_STRING &&
                    lpstSelect->eType == eO97_TYPE_STRING)
                {
                    // Compare the strings

                    nCompare = PascalStrCmp(lpstSelect->uData.lpbyStr,
                                            (LPBYTE)lpvData);

                    // The select value must be >= the top operand

                    if (nCompare < 0)
                        bMatch = FALSE;
                }
                else
                {
                    // Convert the other operand to a long

                    if (lpstSelect->eType == eO97_TYPE_STRING)
                        lValue0 = PascalStrToLong(lpstSelect->uData.lpbyStr);
                    else
                        lValue0 = lpstSelect->uData.lValue;

                    if (eType == eO97_TYPE_STRING)
                        lValue1 = PascalStrToLong((LPBYTE)lpvData);
                    else
                        lValue1 = *(LPLONG)lpvData;

                    // The select value must be >= the top operand

                    if (lValue0 < lValue1)
                        bMatch = FALSE;
                }
            }
            else
                bMatch = FALSE;
        }

        // Pop the top operand which is the lower bound

        if (O97EvalPopTopOperand(lpstEnv) == FALSE)
            return(FALSE);

        if (bMatch == TRUE)
        {
            // Found a TRUE case

            O97SubStateNew(lpstEnv,essO97_TRUE);
        }
    }
    else
    {
        // Pop the top two operands

        if (O97EvalPopTopOperand(lpstEnv) == FALSE)
            return(FALSE);

        if (O97EvalPopTopOperand(lpstEnv) == FALSE)
            return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_4A_CASE_GT()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Calls O97_OP_SELECT_SINGLE_ARG().
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_4A_CASE_GT
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x4A);

//    printf("O97_OP_4A_CASE_GT\n");

    return O97_OP_SELECT_SINGLE_ARG(lpstEnv);
}


//********************************************************************
//
// BOOL O97_OP_4B_CASE_LT()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Calls O97_OP_SELECT_SINGLE_ARG().
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_4B_CASE_LT
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x4B);

//    printf("O97_OP_4B_CASE_LT\n");

    return O97_OP_SELECT_SINGLE_ARG(lpstEnv);
}


//********************************************************************
//
// BOOL O97_OP_4C_CASE_GE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Calls O97_OP_SELECT_SINGLE_ARG().
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_4C_CASE_GE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x4C);

//    printf("O97_OP_4C_CASE_GE\n");

    return O97_OP_SELECT_SINGLE_ARG(lpstEnv);
}


//********************************************************************
//
// BOOL O97_OP_4D_CASE_LE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Calls O97_OP_SELECT_SINGLE_ARG().
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_4D_CASE_LE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x4D);

//    printf("O97_OP_4D_CASE_LE\n");

    return O97_OP_SELECT_SINGLE_ARG(lpstEnv);
}


//********************************************************************
//
// BOOL O97_OP_4E_CASE_NE()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Calls O97_OP_SELECT_SINGLE_ARG().
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_4E_CASE_NE
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x4E);

//    printf("O97_OP_4E_CASE_NE\n");

    return O97_OP_SELECT_SINGLE_ARG(lpstEnv);
}


//********************************************************************
//
// BOOL O97_OP_4F_CASE_EQ()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Calls O97_OP_SELECT_SINGLE_ARG().
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_4F_CASE_EQ
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x4F);

//    printf("O97_OP_4F_CASE_EQ\n");

    return O97_OP_SELECT_SINGLE_ARG(lpstEnv);
}


