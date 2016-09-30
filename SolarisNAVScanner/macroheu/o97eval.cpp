//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97EVAL.CPv   1.2   12 Oct 1998 13:24:48   DCHI  $
//
// Description:
//  Evaluation stack functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97EVAL.CPv  $
// 
//    Rev 1.2   12 Oct 1998 13:24:48   DCHI
// - Changed O97EvalPushOperand() to not return FALSE on an unknown type.
// - Added equality checking of objects to O97EvalDoOperator().
// 
//    Rev 1.1   01 May 1998 11:16:00   DCHI
// Increased abyStr1 buffer size in O97DoOperatorConcatenate().
// 
//    Rev 1.0   15 Aug 1997 13:29:26   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"

#include "wbutil.h"

//*************************************************************************
//
// BOOL O97EvalStackCreate()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function creates an evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalStackCreate
(
    LPO97_ENV       lpstEnv
)
{
    // Allocate operand stack

    lpstEnv->nMaxOperands = 64;

    // Allocate variable operand storage

    if (SSMemoryAlloc(lpstEnv->lpvRootCookie,
                      lpstEnv->nMaxOperands * sizeof(O97_OPERAND_T),
                      (LPLPVOID)&lpstEnv->lpastOperands) != SS_STATUS_OK)
        return(FALSE);

    // Create parallel variable storage

    if (SSMemoryAlloc(lpstEnv->lpvRootCookie,
                      lpstEnv->nMaxOperands * sizeof(O97_VAR_OPERAND_T),
                      (LPLPVOID)&lpstEnv->lpastVarOperands) != SS_STATUS_OK)
    {
        SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpastOperands);

        return(FALSE);
    }

    // Allocate evaluation data store

    lpstEnv->nEvalDataStoreSize = 2048;
    if (SSMemoryAlloc(lpstEnv->lpvRootCookie,
                      lpstEnv->nEvalDataStoreSize * sizeof(BYTE),
                      (LPLPVOID)&lpstEnv->lpbyEvalDataStore) != SS_STATUS_OK)
    {
        SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpastVarOperands);

        SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpastOperands);

        return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97EvalStackDestroy()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function destroys an evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalStackDestroy
(
    LPO97_ENV       lpstEnv
)
{
    BOOL            bResult = TRUE;

    if (SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpastOperands) != SS_STATUS_OK)
        bResult = FALSE;

    if (SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpastVarOperands) != SS_STATUS_OK)
        bResult = FALSE;

    if (SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpbyEvalDataStore) != SS_STATUS_OK)
        bResult = FALSE;

    return(bResult);
}


//*************************************************************************
//
// BOOL O97EvalStackInit()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function initializes an evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalStackInit
(
    LPO97_ENV       lpstEnv
)
{
    lpstEnv->nOperandsUsed = 0;
    lpstEnv->nEvalDataStoreUsed = 0;

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97EvalPushOperand()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  eType           Data type
//  lpvData         Ptr to data
//
// Description:
//  This function pushes an operand onto the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalPushOperand
(
    LPO97_ENV           lpstEnv,
    EO97_TYPE_T         eType,
    LPVOID              lpvData
)
{
    LPO97_OPERAND       lpstOperand;
//    LPO97_VAR_OPERAND   lpstVarOperand;
//    BYTE                byZero = 0;
    int                 i;

    // First allocate an operand structure

    if (lpstEnv->nOperandsUsed >= lpstEnv->nMaxOperands)
    {
        // No more room

        return(FALSE);
    }
/*
    /////////////////////////////////////////////////////////////
    // Set the parallel variable entry
    /////////////////////////////////////////////////////////////

    lpstVarOperand = lpstEnv->lpastVarOperands + lpstEnv->nOperandsUsed;

    if (eType == eO97_TYPE_VAR_ID)
    {
        lpstVarOperand->wID = *(LPWORD)lpvData;
        lpstVarOperand->lpstVar = O97VarGet(lpstEnv,*(LPWORD)lpvData);
        if (lpstVarOperand->lpstVar != NULL)
        {
            switch (lpstVarOperand->lpstVar->eType)
            {
                case eO97_TYPE_LONG:
                    eType = eO97_TYPE_LONG;
                    lpvData = &lpstVarOperand->lpstVar->uData.lValue;
                    break;

                case eO97_TYPE_STRING:
                    eType = eO97_TYPE_STRING;
                    lpvData = &lpstVarOperand->lpstVar->uData.lpbyStr;
                    break;

                case eO97_TYPE_DIALOG_ID:
                    eType = eO97_TYPE_DIALOG_ID;
                    lpvData = &lpstVarOperand->lpstVar->uData.dwDlgID;
                    break;

                case eO97_TYPE_OBJECT_ID:
                    eType = eO97_TYPE_OBJECT_ID;
                    lpvData = &lpstVarOperand->lpstVar->uData.dwObjectID;
                    break;

                case eO97_TYPE_PARAM_ASSIGN:
                    eType = eO97_TYPE_PARAM_ASSIGN;
                    lpvData = &lpstVarOperand->lpstVar->uData.wVarID;
                    break;

                default:
                    // Assume empty string

                    eType = eO97_TYPE_STRING;
                    lpvData = &byZero;
                    break;
            }
        }
        else
        {
            // Assume empty string

            eType = eO97_TYPE_STRING;
            lpvData = &byZero;
        }
    }
    else
    {
        lpstVarOperand->wID = O97_ID_INVALID;
        lpstVarOperand->lpstVar = NULL;
    }
*/

    /////////////////////////////////////////////////////////////
    // Now store the data
    /////////////////////////////////////////////////////////////

    lpstOperand = lpstEnv->lpastOperands + lpstEnv->nOperandsUsed;
    lpstOperand->eType = eType;
    switch (eType)
    {
        case eO97_TYPE_LONG:
            lpstOperand->uData.lValue = *(LPLONG)lpvData;
            break;

        case eO97_TYPE_STRING:
            // Allocate storage from the evaluation data store

            // First byte of the data specifies the string length

            if ((lpstEnv->nEvalDataStoreUsed + *(LPBYTE)lpvData + 1) >
                lpstEnv->nEvalDataStoreSize)
            {
                // No more room

                return(FALSE);
            }

            // Set the pointer to the buffer

            lpstOperand->uData.lpbyStr =
                lpstEnv->lpbyEvalDataStore +
                lpstEnv->nEvalDataStoreUsed;

            // Copy the string

            for (i=0;i<=*(LPBYTE)lpvData;i++)
                lpstOperand->uData.lpbyStr[i] = ((LPBYTE)lpvData)[i];

            // Update the data store usage

            lpstEnv->nEvalDataStoreUsed += *(LPBYTE)lpvData + 1;

            break;

        case eO97_TYPE_DIALOG_ID:
            lpstOperand->uData.dwDlgID = *(LPDWORD)lpvData;
            break;

        case eO97_TYPE_O97_OBJECT_ID:
        case eO97_TYPE_OBJECT_ID:
            lpstOperand->uData.dwObjectID = *(LPDWORD)lpvData;
            break;

        case eO97_TYPE_PARAM_ASSIGN:
            lpstOperand->uData.wVarID = *(LPWORD)lpvData;
            break;

        case eO97_TYPE_VAR_ID:
            lpstOperand->uData.wVarID = *(LPWORD)lpvData;
            break;

        default:
            // Unknown type

            break;
    }

    // Increment the operand usage count

    lpstEnv->nOperandsUsed++;

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97EvalPushLongOperand()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  l               Long value
//
// Description:
//  This function pushes a long operand onto the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalPushLongOperand
(
    LPO97_ENV       lpstEnv,
    long            l
)
{
    return (O97EvalPushOperand(lpstEnv,
                               eO97_TYPE_LONG,
                               &l));
}


//*************************************************************************
//
// BOOL O97EvalPushSZOperand()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpsz            Ptr to zero-terminated string or NULL for empty sz
//
// Description:
//  This function pushes a zero-terminated string onto the evaluation
//  stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalPushSZOperand
(
    LPO97_ENV       lpstEnv,
    LPSTR           lpsz
)
{
    BYTE            abyStr[256];
    int             nLen;

    nLen = 0;
    if (lpsz != NULL)
    {
        while (nLen < 255 && *lpsz)
            abyStr[++nLen] = (BYTE)*lpsz++;
    }

    abyStr[0] = (BYTE)nLen;

    return (O97EvalPushOperand(lpstEnv,
                               eO97_TYPE_STRING,
                               abyStr));
}


//*************************************************************************
//
// BOOL O97EvalDoOperatorConcatenate()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function casts both operands to strings and concatenates
//  the results.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97DoOperatorConcatenate
(
    LPO97_ENV       lpstEnv
)
{
    LPO97_OPERAND   lpstOperand0, lpstOperand1;
    LPO97_VAR       lpstVar0, lpstVar1;
    LPBYTE          lpbyStr0, lpbyStr1, lpbyDst;
    BYTE            abyStr0[256], abyStr1[256];
    int             nLen0, nLen1, n;

    if (lpstEnv->nOperandsUsed < 2)
        return(FALSE);

    // Get the operands

    lpstOperand0 = lpstEnv->lpastOperands +
        lpstEnv->nOperandsUsed - 2;

    lpstOperand1 = lpstOperand0 + 1;

    // Pop second operand because result will be stored
    //  in first operand

    lpstEnv->nOperandsUsed--;

    // Check for variable operands

    if (lpstOperand0->eType == eO97_TYPE_VAR_ID)
    {
        lpstVar0 = O97VarGet(lpstEnv,lpstOperand0->uData.wVarID);
        if (lpstVar0 != NULL && lpstVar0->eType == eO97_TYPE_VAR)
            lpstVar0 = (LPO97_VAR)lpstVar0->uData.lpvstVar;
    }
    else
        lpstVar0 = NULL;

    if (lpstOperand1->eType == eO97_TYPE_VAR_ID)
    {
        lpstVar1 = O97VarGet(lpstEnv,lpstOperand1->uData.wVarID);
        if (lpstVar1 != NULL && lpstVar1->eType == eO97_TYPE_VAR)
            lpstVar1 = (LPO97_VAR)lpstVar1->uData.lpvstVar;
    }
    else
        lpstVar1 = NULL;

    ////////////////////////////////////////////////////////
    // If either operand is not a string, convert it
    ////////////////////////////////////////////////////////

    if (lpstVar0 != NULL)
    {
        if (lpstVar0->eType == eO97_TYPE_STRING)
            lpbyStr0 = lpstVar0->uData.lpbyStr;
        else
        {
            LongToPascalStr(lpstVar0->uData.lValue,abyStr0);
            lpbyStr0 = abyStr0;
        }
    }
    else
    if (lpstOperand0->eType == eO97_TYPE_STRING)
    {
        lpbyStr0 = lpstOperand0->uData.lpbyStr;

        // Copy the string

        for (n=0;n<=lpbyStr0[0];n++)
            abyStr0[n] = lpbyStr0[n];

        // Free the string

        lpstEnv->nEvalDataStoreUsed -=
            lpstOperand0->uData.lpbyStr[0] + 1;

        lpbyStr0 = abyStr0;
    }
    else
    {
        LongToPascalStr(lpstOperand0->uData.lValue,abyStr0);
        lpbyStr0 = abyStr0;
    }

    if (lpstVar1 != NULL)
    {
        if (lpstVar1->eType == eO97_TYPE_STRING)
            lpbyStr1 = lpstVar1->uData.lpbyStr;
        else
        {
            LongToPascalStr(lpstVar1->uData.lValue,abyStr1);
            lpbyStr1 = abyStr1;
        }
    }
    else
    if (lpstOperand1->eType == eO97_TYPE_STRING)
    {
        lpbyStr1 = lpstOperand1->uData.lpbyStr;

        // Copy the string

        for (n=0;n<=lpbyStr1[0];n++)
            abyStr1[n] = lpbyStr1[n];

        // Free the string

        lpstEnv->nEvalDataStoreUsed -=
            lpstOperand1->uData.lpbyStr[0] + 1;

        lpbyStr1 = abyStr1;
    }
    else
    {
        LongToPascalStr(lpstOperand1->uData.lValue,abyStr1);
        lpbyStr1 = abyStr1;
    }


    ////////////////////////////////////////////////////////
    // Concatenate the two strings
    ////////////////////////////////////////////////////////

    nLen0 = lpbyStr0[0];
    nLen1 = lpbyStr1[0];

    // Make sure the string doesn't exceed 255 characters

    if ((nLen0 + nLen1) > 255)
        nLen1 = 255 - nLen0;

    // See if there is enough room for the string

    if ((lpstEnv->nEvalDataStoreUsed + nLen0 + nLen1 + 1) >
        lpstEnv->nEvalDataStoreSize)
    {
        // No more room

        return(FALSE);
    }

    lpstOperand0->uData.lpbyStr = lpbyDst =
        lpstEnv->lpbyEvalDataStore + lpstEnv->nEvalDataStoreUsed;

    // Concatenate the strings onto the evaluation store

    *lpbyDst++ = nLen0 + nLen1;
    for (n=1;n<=nLen0;n++)
        *lpbyDst++ = lpbyStr0[n];

    for (n=1;n<=nLen1;n++)
        *lpbyDst++ = lpbyStr1[n];

    lpstOperand0->eType = eO97_TYPE_STRING;

    // Update data store usage

    lpstEnv->nEvalDataStoreUsed += nLen0 + nLen1 + 1;

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97EvalDoOperator()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  eOperator       Operator
//
// Description:
//  This function applies the given operator.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalDoOperator
(
    LPO97_ENV       lpstEnv,
    EO97_OPERATOR_T eOperator
)
{
    LPO97_OPERAND   lpstOperand0, lpstOperand1;
    LPO97_VAR       lpstVar0, lpstVar1;
    EO97_TYPE_T     eOrigOp0Type, eOrigOp1Type;

    // Take care of unary operators

    if (eOperator == eO97_OPERATOR_NEG ||
        eOperator == eO97_OPERATOR_NOT)
    {
        if (lpstEnv->nOperandsUsed < 1)
            return(FALSE);

        lpstOperand0 = lpstEnv->lpastOperands +
            lpstEnv->nOperandsUsed - 1;

        // If the operand is a variable then get the long value for it

        if (lpstOperand0->eType == eO97_TYPE_VAR_ID)
        {
            lpstOperand0->eType = eO97_TYPE_LONG;
            lpstVar0 = O97VarGet(lpstEnv,lpstOperand0->uData.wVarID);
            if (lpstVar0 != NULL && lpstVar0->eType == eO97_TYPE_VAR)
                lpstVar0 = (LPO97_VAR)lpstVar0->uData.lpvstVar;

            if (lpstVar0 == NULL || lpstVar0->eType != eO97_TYPE_LONG)
            {
                // Set the value to zero

                lpstOperand0->uData.lValue = 0;
            }
            else
                lpstOperand0->uData.lValue = lpstVar0->uData.lValue;
        }

        if (lpstOperand0->eType == eO97_TYPE_LONG)
        {
            if (eOperator == eO97_OPERATOR_NEG)
                lpstOperand0->uData.lValue = -lpstOperand0->uData.lValue;
            else
                lpstOperand0->uData.lValue = ~lpstOperand0->uData.lValue;
        }

        // If the type is not numeric, just leave it

        return(TRUE);
    }

    // First check should be to make sure that there are enough
    //  operands.  Unary operators require one operand.  Binary
    //  operators require two operands.

    if (lpstEnv->nOperandsUsed < 2)
        return(FALSE);

    // Get the operands

    lpstOperand0 = lpstEnv->lpastOperands +
        lpstEnv->nOperandsUsed - 2;

    lpstOperand1 = lpstOperand0 + 1;

    // Pop second operand because result will be stored
    //  in first operand

    lpstEnv->nOperandsUsed--;

    // Check for variable operands

    if (lpstOperand0->eType == eO97_TYPE_VAR_ID)
    {
        lpstVar0 = O97VarGet(lpstEnv,lpstOperand0->uData.wVarID);
        if (lpstVar0 != NULL && lpstVar0->eType == eO97_TYPE_VAR)
            lpstVar0 = (LPO97_VAR)lpstVar0->uData.lpvstVar;
    }
    else
        lpstVar0 = NULL;

    if (lpstOperand1->eType == eO97_TYPE_VAR_ID)
    {
        lpstVar1 = O97VarGet(lpstEnv,lpstOperand1->uData.wVarID);
        if (lpstVar1 != NULL && lpstVar1->eType == eO97_TYPE_VAR)
            lpstVar1 = (LPO97_VAR)lpstVar1->uData.lpvstVar;
    }
    else
        lpstVar1 = NULL;

    // At this point we know we have the requisite number
    //  of operands on the stack.

    if ((lpstOperand0->eType == eO97_TYPE_STRING ||
         (lpstVar0 != NULL && lpstVar0->eType == eO97_TYPE_STRING)) &&
        (lpstOperand1->eType == eO97_TYPE_STRING ||
         (lpstVar1 != NULL && lpstVar1->eType == eO97_TYPE_STRING)))
    {
        LPBYTE  lpbyStr0, lpbyStr1;

        // Get ptrs to the strings

        if (lpstVar0 == NULL)
        {
            lpbyStr0 = lpstOperand0->uData.lpbyStr;

            // Free the string

            lpstEnv->nEvalDataStoreUsed -=
                lpstOperand0->uData.lpbyStr[0] + 1;
        }
        else
            lpbyStr0 = lpstVar0->uData.lpbyStr;

        if (lpstVar1 == NULL)
        {
            lpbyStr1 = lpstOperand1->uData.lpbyStr;

            // Free the string

            lpstEnv->nEvalDataStoreUsed -=
                lpstOperand1->uData.lpbyStr[0] + 1;
        }
        else
            lpbyStr1 = lpstVar1->uData.lpbyStr;

        switch (eOperator)
        {
            case eO97_OPERATOR_ADD:
            {
                int     nLen0, nLen1, n;
                BYTE    abyStr[256];
                LPBYTE  lpbyDst;

                // Concatenate the two strings

                nLen0 = *lpbyStr0;
                nLen1 = *lpbyStr1;

                // Make sure the string doesn't exceed 255 characters

                if ((nLen0 + nLen1) > 255)
                    nLen1 = 255 - nLen0;

                // Allocate storage from the evaluation data store

                // First byte of the data specifies the string length

                if ((lpstEnv->nEvalDataStoreUsed + nLen0 + nLen1 + 1) >
                    lpstEnv->nEvalDataStoreSize)
                {
                    // No more room

                    return(FALSE);
                }

                // Copy to a temporary buffer

                abyStr[0] = (BYTE)(nLen0 + nLen1);
                lpbyDst = abyStr + 1;

                // Copy the first string

                for (n=0;n<nLen0;n++)
                    *lpbyDst++ = *++lpbyStr0;

                // Copy the second string

                for (n=0;n<nLen1;n++)
                    *lpbyDst++ = *++lpbyStr1;

                // Now copy it to the real destination

                nLen0 += nLen1;
                lpbyDst = lpstOperand0->uData.lpbyStr =
                    lpstEnv->lpbyEvalDataStore + lpstEnv->nEvalDataStoreUsed;

                for (n=0;n<=nLen0;n++)
                    *lpbyDst++ = abyStr[n];

                // Update data store usage

                lpstEnv->nEvalDataStoreUsed += nLen0 + 1;

                // Make sure that the data type is string

                lpstOperand0->eType = eO97_TYPE_STRING;

                return(TRUE);
            }

            case eO97_OPERATOR_EQ:
            {
                lpstOperand0->eType = eO97_TYPE_LONG;
                if (*lpbyStr0 != *lpbyStr1)
                {
                    lpstOperand0->uData.lValue = 0;
                }
                else
                if (PascalStrCmp(lpbyStr0,lpbyStr1) == 0)
                {
                    lpstOperand0->uData.lValue = -1;
                }
                else
                {
                    lpstOperand0->uData.lValue = 0;
                }
                return(TRUE);
            }

            case eO97_OPERATOR_NE:
            {
                lpstOperand0->eType = eO97_TYPE_LONG;
                if (*lpbyStr0 != *lpbyStr1)
                    lpstOperand0->uData.lValue = -1;
                else
                if (PascalStrCmp(lpbyStr0,lpbyStr1) == 0)
                    lpstOperand0->uData.lValue = 0;
                else
                    lpstOperand0->uData.lValue = -1;
                return(TRUE);
            }

            case eO97_OPERATOR_LT:
            {
                lpstOperand0->eType = eO97_TYPE_LONG;
                if (PascalStrCmp(lpbyStr0,lpbyStr1) < 0)
                    lpstOperand0->uData.lValue = -1;
                else
                    lpstOperand0->uData.lValue = 0;
                return(TRUE);
            }

            case eO97_OPERATOR_GT:
            {
                lpstOperand0->eType = eO97_TYPE_LONG;
                if (PascalStrCmp(lpbyStr0,lpbyStr1) > 0)
                    lpstOperand0->uData.lValue = -1;
                else
                    lpstOperand0->uData.lValue = 0;
                return(TRUE);
            }

            case eO97_OPERATOR_LE:
            {
                lpstOperand0->eType = eO97_TYPE_LONG;
                if (PascalStrCmp(lpbyStr0,lpbyStr1) <= 0)
                    lpstOperand0->uData.lValue = -1;
                else
                    lpstOperand0->uData.lValue = 0;
                return(TRUE);
            }

            case eO97_OPERATOR_GE:
            {
                lpstOperand0->eType = eO97_TYPE_LONG;
                if (PascalStrCmp(lpbyStr0,lpbyStr1) >= 0)
                    lpstOperand0->uData.lValue = -1;
                else
                    lpstOperand0->uData.lValue = 0;
                return(TRUE);
            }

            default:
                if (lpstVar0 == NULL)
                {
                    // Restore the first string and ignore

                    lpstEnv->nEvalDataStoreUsed +=
                        lpstOperand0->uData.lpbyStr[0] + 1;
                }
                return(TRUE);
        }
    }
    else
    if ((lpstOperand0->eType == eO97_TYPE_OBJECT_ID ||
         (lpstVar0 != NULL && lpstVar0->eType == eO97_TYPE_OBJECT_ID)) &&
        (lpstOperand1->eType == eO97_TYPE_OBJECT_ID ||
         (lpstVar1 != NULL && lpstVar1->eType == eO97_TYPE_OBJECT_ID)))
    {
        DWORD   dwObjID0, dwObjID1;

        // Get the object IDs

        if (lpstVar0 == NULL)
            dwObjID0 = lpstOperand0->uData.dwObjectID;
        else
            dwObjID0 = lpstVar0->uData.dwObjectID;

        if (lpstVar1 == NULL)
            dwObjID1 = lpstOperand1->uData.dwObjectID;
        else
            dwObjID1 = lpstVar1->uData.dwObjectID;

        switch (eOperator)
        {
            case eO97_OPERATOR_EQ:
            case eO97_OPERATOR_LE:
            case eO97_OPERATOR_GE:
            {
                lpstOperand0->eType = eO97_TYPE_LONG;
                if (dwObjID0 != dwObjID1)
                    lpstOperand0->uData.lValue = 0;
                else
                    lpstOperand0->uData.lValue = -1;
                return(TRUE);
            }

            case eO97_OPERATOR_NE:
            {
                lpstOperand0->eType = eO97_TYPE_LONG;
                if (dwObjID0 == dwObjID1)
                    lpstOperand0->uData.lValue = 0;
                else
                    lpstOperand0->uData.lValue = -1;

                return(TRUE);
            }

            default:
                // Ignore

                return(TRUE);
        }
    }

    // Convert to numeric if necessary

    eOrigOp0Type = eO97_TYPE_LONG;
    if (lpstOperand0->eType == eO97_TYPE_STRING)
    {
        eOrigOp0Type = eO97_TYPE_STRING;

        // Deallocate the memory for it first

        lpstEnv->nEvalDataStoreUsed -= lpstOperand0->uData.lpbyStr[0] + 1;

        // Get the value

        lpstOperand0->uData.lValue =
            PascalStrToLong(lpstOperand0->uData.lpbyStr);
    }
    else
    if (lpstVar0 != NULL)
    {
        if (lpstVar0->eType == eO97_TYPE_STRING)
        {
            eOrigOp0Type = eO97_TYPE_STRING;

            lpstOperand0->uData.lValue =
                PascalStrToLong(lpstVar0->uData.lpbyStr);
        }
        else
        if (lpstVar0->eType == eO97_TYPE_LONG)
            lpstOperand0->uData.lValue = lpstVar0->uData.lValue;
        else
            lpstOperand0->uData.lValue = 0;
    }
    else
    if (lpstOperand0->eType != eO97_TYPE_LONG)
        lpstOperand0->uData.lValue = 0;

    eOrigOp1Type = eO97_TYPE_LONG;
    if (lpstOperand1->eType == eO97_TYPE_STRING)
    {
        eOrigOp1Type = eO97_TYPE_STRING;

        // Deallocate the memory for it

        lpstEnv->nEvalDataStoreUsed -= lpstOperand1->uData.lpbyStr[0] + 1;

        // Get the value

        lpstOperand1->uData.lValue =
            PascalStrToLong(lpstOperand1->uData.lpbyStr);
    }
    else
    if (lpstVar1 != NULL)
    {
        if (lpstVar1->eType == eO97_TYPE_STRING)
        {
            eOrigOp1Type = eO97_TYPE_STRING;

            lpstOperand1->uData.lValue =
                PascalStrToLong(lpstVar1->uData.lpbyStr);
        }
        else
        if (lpstVar1->eType == eO97_TYPE_LONG)
            lpstOperand1->uData.lValue = lpstVar1->uData.lValue;
        else
            lpstOperand1->uData.lValue = 0;
    }
    else
    if (lpstOperand1->eType != eO97_TYPE_LONG)
        lpstOperand1->uData.lValue = 0;

    // Now both operands are of type long

    // Make sure the result is a LONG

    lpstOperand0->eType = eO97_TYPE_LONG;

    switch (eOperator)
    {
        case eO97_OPERATOR_ADD:
            lpstOperand0->uData.lValue += lpstOperand1->uData.lValue;
            return(TRUE);

        case eO97_OPERATOR_SUB:
            lpstOperand0->uData.lValue -= lpstOperand1->uData.lValue;
            return(TRUE);

        case eO97_OPERATOR_MUL:
            lpstOperand0->uData.lValue *= lpstOperand1->uData.lValue;
            return(TRUE);

        case eO97_OPERATOR_DIV:
            // Prevent divides by zero

            if (lpstOperand1->uData.lValue != 0)
                lpstOperand0->uData.lValue /= lpstOperand1->uData.lValue;

            return(TRUE);

        case eO97_OPERATOR_MOD:
            // Prevent divides by zero

            if (lpstOperand1->uData.lValue != 0)
                lpstOperand0->uData.lValue %= lpstOperand1->uData.lValue;

            return(TRUE);

        case eO97_OPERATOR_EQ:
            // Not both operands could have been strings if we are here

            if (eOrigOp0Type != eO97_TYPE_STRING &&
                eOrigOp1Type != eO97_TYPE_STRING &&
                lpstOperand0->uData.lValue == lpstOperand1->uData.lValue)
                lpstOperand0->uData.lValue = -1;
            else
                lpstOperand0->uData.lValue = 0;
            return(TRUE);

        case eO97_OPERATOR_NE:
            // Not both operands could have been strings if we are here

            if (eOrigOp0Type == eO97_TYPE_STRING ||
                eOrigOp1Type == eO97_TYPE_STRING ||
                lpstOperand0->uData.lValue != lpstOperand1->uData.lValue)
                lpstOperand0->uData.lValue = -1;
            else
                lpstOperand0->uData.lValue = 0;
            return(TRUE);

        case eO97_OPERATOR_LT:
            if (lpstOperand0->uData.lValue < lpstOperand1->uData.lValue)
                lpstOperand0->uData.lValue = -1;
            else
                lpstOperand0->uData.lValue = 0;
            return(TRUE);

        case eO97_OPERATOR_GT:
            if (lpstOperand0->uData.lValue > lpstOperand1->uData.lValue)
                lpstOperand0->uData.lValue = -1;
            else
                lpstOperand0->uData.lValue = 0;
            return(TRUE);

        case eO97_OPERATOR_LE:
            if (lpstOperand0->uData.lValue <= lpstOperand1->uData.lValue)
                lpstOperand0->uData.lValue = -1;
            else
                lpstOperand0->uData.lValue = 0;
            return(TRUE);

        case eO97_OPERATOR_GE:
            if (lpstOperand0->uData.lValue >= lpstOperand1->uData.lValue)
                lpstOperand0->uData.lValue = -1;
            else
                lpstOperand0->uData.lValue = 0;
            return(TRUE);

        case eO97_OPERATOR_AND:
            lpstOperand0->uData.lValue &= lpstOperand1->uData.lValue;
            return(TRUE);

        case eO97_OPERATOR_OR:
            lpstOperand0->uData.lValue |= lpstOperand1->uData.lValue;
            return(TRUE);

        case eO97_OPERATOR_XOR:
            lpstOperand0->uData.lValue ^= lpstOperand1->uData.lValue;
            return(TRUE);

        case eO97_OPERATOR_EQV:
            lpstOperand0->uData.lValue =
                ~(lpstOperand0->uData.lValue ^ lpstOperand1->uData.lValue);
            return(TRUE);

        default:
            // Ignore
            return(TRUE);
    }
}

#if 0

//*************************************************************************
//
// BOOL O97EvalTopVarToValue()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  If the top operand is a variable, then the operand is replaced by
//  the value of the variable.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalTopVarToValue
(
    LPO97_ENV       lpstEnv
)
{
    LPO97_OPERAND   lpstOperand;
    LPO97_VAR       lpstVar;
    int             i;

    if (lpstEnv->nOperandsUsed < 1)
        return(FALSE);

    lpstOperand = lpstEnv->lpastOperands +
        lpstEnv->nOperandsUsed - 1;

    // If the operand is not a variable just return

    if (lpstOperand->eType != eO97_TYPE_VAR_ID)
        return(TRUE);

    lpstVar = O97VarGet(lpstEnv,lpstOperand->uData.wVarID);
    if (lpstVar != NULL && lpstVar->eType == eO97_TYPE_VAR)
        lpstVar = (LPO97_VAR)lpstVar->uData.lpvstVar;

    if (lpstVar == NULL || (lpstVar->eType != eO97_TYPE_LONG &&
                            lpstVar->eType != eO97_TYPE_STRING))
    {
        lpstOperand->eType = eO97_TYPE_UNKNOWN;
        return(TRUE);
    }

    lpstOperand->eType = lpstVar->eType;
    if (lpstVar->eType == eO97_TYPE_LONG)
    {
        // Set the value to zero

        lpstOperand->uData.lValue = 0;
    }
    else
    {
        // Must be a string

        // Allocate storage from the evaluation data store

        // First byte of the data specifies the string length

        if ((lpstEnv->nEvalDataStoreUsed + lpstVar->uData.lpbyStr[0] + 1) >
            lpstEnv->nEvalDataStoreSize)
        {
            // No more room

            return(FALSE);
        }

        // Set the pointer to the buffer

        lpstOperand->uData.lpbyStr =
            lpstEnv->lpbyEvalDataStore + lpstEnv->nEvalDataStoreUsed;

        // Copy the string

        for (i=0;i<=lpstVar->uData.lpbyStr[0];i++)
            lpstOperand->uData.lpbyStr[i] = lpstVar->uData.lpbyStr[i];

        // Update the data store usage

        lpstEnv->nEvalDataStoreUsed += lpstVar->uData.lpbyStr[0] + 1;
    }

    return(TRUE);
}

#endif


//*************************************************************************
//
// BOOL O97EvalQueryTopOperand()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpeType         Ptr to variable for data type
//  lplpvData       Ptr to variable to store ptr to data
//
// Description:
//  This function gets information on the top operand.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalQueryTopOperand
(
    LPO97_ENV       lpstEnv,
    LPEO97_TYPE     lpeType,
    LPLPVOID        lplpvData
)
{
    LPO97_OPERAND   lpstOperand;

    if (lpstEnv->nOperandsUsed <= 0)
        return(FALSE);

    lpstOperand = lpstEnv->lpastOperands + lpstEnv->nOperandsUsed - 1;
    *lpeType = lpstOperand->eType;
    switch (lpstOperand->eType)
    {
        case eO97_TYPE_LONG:
            *lplpvData = &lpstOperand->uData.lValue;
            break;

        case eO97_TYPE_O97_OBJECT_ID:
        case eO97_TYPE_OBJECT_ID:
            *lplpvData = &lpstOperand->uData.dwObjectID;
            break;

        case eO97_TYPE_STRING:
            *lplpvData = lpstOperand->uData.lpbyStr;
            break;

        case eO97_TYPE_PARAM_ASSIGN:
            *lplpvData = &lpstOperand->uData.wVarID;
            break;

        case eO97_TYPE_VAR_ID:
            *lplpvData = &lpstOperand->uData.wVarID;
            break;

        case eO97_TYPE_DIALOG_ID:
            *lplpvData = &lpstOperand->uData.dwDlgID;
            break;

        default:
            break;
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97EvalGetTopBaseData()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpeType         Ptr to variable for data type
//  lplpvData       Ptr to variable to store ptr to data
//
// Description:
//  This function gets information on the top operand.  If the operand
//  is a variable the function dereferences the data.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalGetTopBaseData
(
    LPO97_ENV       lpstEnv,
    LPEO97_TYPE     lpeType,
    LPLPVOID        lplpvData
)
{
    LPO97_OPERAND   lpstOperand;

    if (lpstEnv->nOperandsUsed <= 0)
        return(FALSE);

    lpstOperand = lpstEnv->lpastOperands + lpstEnv->nOperandsUsed - 1;
    if (lpstOperand->eType == eO97_TYPE_VAR_ID)
    {
        LPO97_VAR   lpstVar;

        lpstVar = O97VarGet(lpstEnv,lpstOperand->uData.wVarID);
        if (lpstVar != NULL && lpstVar->eType == eO97_TYPE_VAR)
            lpstVar = (LPO97_VAR)lpstVar->uData.lpvstVar;

        O97VarGetTypeData(lpstVar,lpeType,lplpvData);
        return(TRUE);
    }

    return O97EvalQueryTopOperand(lpstEnv,lpeType,lplpvData);
}


//*************************************************************************
//
// BOOL O97EvalQueryTopOperandAsString()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lplpbyStr       Ptr to ptr for str ptr
//
// Description:
//  This function gets a pointer to the value of the top operand.
//  If the top operand does not evaluate to a string, the function
//  returns error.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalQueryTopOperandAsString
(
    LPO97_ENV       lpstEnv,
    LPLPBYTE        lplpbyStr
)
{
    LPO97_OPERAND   lpstOperand;

    if (lpstEnv->nOperandsUsed <= 0)
        return(FALSE);

    lpstOperand = lpstEnv->lpastOperands + lpstEnv->nOperandsUsed - 1;
    if (lpstOperand->eType == eO97_TYPE_VAR_ID)
    {
        LPO97_VAR   lpstVar;

        lpstVar = O97VarGet(lpstEnv,lpstOperand->uData.wVarID);
        if (lpstVar != NULL && lpstVar->eType == eO97_TYPE_VAR)
            lpstVar = (LPO97_VAR)lpstVar->uData.lpvstVar;

        if (lpstVar != NULL && lpstVar->eType == eO97_TYPE_STRING)
        {
            *lplpbyStr = lpstVar->uData.lpbyStr;
            return(TRUE);
        }
    }
    else
    if (lpstOperand->eType == eO97_TYPE_STRING)
    {
        *lplpbyStr = lpstOperand->uData.lpbyStr;
        return(TRUE);
    }

    return(FALSE);
}


//*************************************************************************
//
// BOOL O97EvalPopTopOperand()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function pops the top operand off of the operand stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalPopTopOperand
(
    LPO97_ENV       lpstEnv
)
{
    LPO97_OPERAND   lpstOperand;

    if (lpstEnv->nOperandsUsed <= 0)
        return(FALSE);

    lpstOperand = lpstEnv->lpastOperands + lpstEnv->nOperandsUsed - 1;
    switch (lpstOperand->eType)
    {
        case eO97_TYPE_STRING:
            // Free evaluation data store usage

            lpstEnv->nEvalDataStoreUsed -=
                lpstOperand->uData.lpbyStr[0] + 1;
            break;

        default:
            break;
    }

    // Pop it

    lpstEnv->nOperandsUsed--;

    return(TRUE);
}


//*************************************************************************
//
// BOOL EvalGetAndPopTopAsBoolean()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpbResult       Ptr to BOOL variable for result
//
// Description:
//  This function queries the top operand, converts it to a boolean
//  value at *lpbResult, and pops the top operand.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalGetAndPopTopAsBoolean
(
    LPO97_ENV       lpstEnv,
    LPBOOL          lpbResult
)
{
    EO97_TYPE_T     eType;
    LPVOID          lpvData;

    // Check the result of the top operand

    if (O97EvalQueryTopOperand(lpstEnv,
                               &eType,
                               &lpvData) == FALSE)
    {
        return(FALSE);
    }

    if (eType == eO97_TYPE_VAR_ID)
    {
        LPO97_VAR   lpstVar;

        lpstVar = O97VarGet(lpstEnv,*(LPWORD)lpvData);
        if (lpstVar != NULL && lpstVar->eType == eO97_TYPE_VAR)
            lpstVar = (LPO97_VAR)lpstVar->uData.lpvstVar;

        O97VarGetTypeData(lpstVar,&eType,&lpvData);
    }

    switch (eType)
    {
        case eO97_TYPE_LONG:
            if (*(LPLONG)lpvData == 0)
                *lpbResult = FALSE;
            else
                *lpbResult = TRUE;
            break;

        case eO97_TYPE_STRING:
            // Assume a zero-length string means FALSE

            if (*(LPBYTE)lpvData == 0)
                *lpbResult = FALSE;
            else
                *lpbResult = TRUE;
            break;

        default:
            // Assume everything else means FALSE for now

            *lpbResult = FALSE;
            break;
    }

    // Pop the result

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
    {
        // Failed to pop

        return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL EvalGetAndPopTopAsLong()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lplResult       Ptr to LONG variable for result
//
// Description:
//  This function queries the top operand, converts it to a long
//  value at *lplResult, and pops the top operand.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalGetAndPopTopAsLong
(
    LPO97_ENV       lpstEnv,
    LPLONG          lplResult
)
{
    EO97_TYPE_T     eType;
    LPVOID          lpvData;

    // Check the result of the top operand

    if (O97EvalQueryTopOperand(lpstEnv,
                               &eType,
                               &lpvData) == FALSE)
    {
        return(FALSE);
    }

    if (eType == eO97_TYPE_VAR_ID)
    {
        LPO97_VAR   lpstVar;

        lpstVar = O97VarGet(lpstEnv,*(LPWORD)lpvData);
        O97VarGetTypeData(lpstVar,&eType,&lpvData);
        if (lpstVar != NULL && lpstVar->eType == eO97_TYPE_VAR)
            lpstVar = (LPO97_VAR)lpstVar->uData.lpvstVar;

    }

    switch (eType)
    {
        case eO97_TYPE_LONG:
            *lplResult = *(LPLONG)lpvData;
            break;

        case eO97_TYPE_STRING:
            // Convert it to a numeric value

            *lplResult = PascalStrToLong((LPBYTE)lpvData);
            break;

        default:
            // Assume everything else means 0

            *lplResult = 0;
            break;
    }

    // Pop the result

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
    {
        // Failed to pop

        return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL EvalGetAndPopTopAsString()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lplpbyStr       Ptr to ptr for str ptr
//
// Description:
//  This function gets a pointer to the value of the top operand.
//  If the top operand does not evaluate to a string, the function
//  returns error.  The operand is also popped.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalGetAndPopTopAsString
(
    LPO97_ENV       lpstEnv,
    LPLPBYTE        lplpbyStr
)
{
    // Check the result of the top operand

    if (O97EvalQueryTopOperandAsString(lpstEnv,
                                       lplpbyStr) == FALSE)
        return(FALSE);

    // Pop the result

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
    {
        // Failed to pop

        return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL EvalGetAndPopTopAsVarID()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpwVarID        Ptr to WORD variable for result
//
// Description:
//  This function queries the top operand, converts it to a variable ID
//  value at *lpwVarID, and pops the top operand.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalGetAndPopTopAsVarID
(
    LPO97_ENV       lpstEnv,
    LPWORD          lpwVarID
)
{
    EO97_TYPE_T     eType;
    LPVOID          lpvData;

    // Check the result of the top operand

    if (O97EvalQueryTopOperand(lpstEnv,
                               &eType,
                               &lpvData) == FALSE)
    {
        return(FALSE);
    }

    if (eType == eO97_TYPE_VAR_ID)
    {
        *lpwVarID = *(LPWORD)lpvData;
        return(TRUE);
    }

    // Not a variable ID

    return(FALSE);
}


//*************************************************************************
//
// BOOL O97EvalPopParameters()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  wNumParams      Number of parameters
//
// Description:
//  This function pops the given number of parameters from the
//  evaluation stack.  Encountering an eO97_TYPE_PARAM_ASSIGN pops
//  that entry and the next entry.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97EvalPopParameters
(
    LPO97_ENV       lpstEnv,
    WORD            wNumParams
)
{
    LPO97_OPERAND   lpstOperand;

    while (lpstEnv->nOperandsUsed > 0 && wNumParams != 0)
    {
        lpstOperand = lpstEnv->lpastOperands + lpstEnv->nOperandsUsed - 1;
        if (lpstOperand->eType != eO97_TYPE_PARAM_ASSIGN)
        {
            switch (lpstOperand->eType)
            {
                case eO97_TYPE_STRING:
                    // Free evaluation data store usage

                    lpstEnv->nEvalDataStoreUsed -=
                        lpstOperand->uData.lpbyStr[0] + 1;
                    break;

                default:
                    break;
            }

            --wNumParams;
        }

        // Pop it

        lpstEnv->nOperandsUsed--;
    }

    return(TRUE);
}


