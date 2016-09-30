//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/EVAL.CPv   1.0   30 Jun 1997 16:15:24   DCHI  $
//
// Description:
//  Macro emulation environment expression evaluation functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/EVAL.CPv  $
// 
//    Rev 1.0   30 Jun 1997 16:15:24   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "wd7api.h"
#include "wd7env.h"

#include "wbutil.h"

//*************************************************************************
//
// BOOL EvalStackCreate()
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

BOOL EvalStackCreate
(
    LPENV       lpstEnv
)
{
    // Allocate room for same number of operands and operators

    lpstEnv->nMaxOperands = 64;

    // Allocate variable operand storage

    if (SSMemoryAlloc(lpstEnv->lpvRootCookie,
                      lpstEnv->nMaxOperands * sizeof(OPERAND_T),
                      (LPLPVOID)&lpstEnv->lpastOperands) != SS_STATUS_OK)
        return(FALSE);

    // Allocate operator storage

    if (SSMemoryAlloc(lpstEnv->lpvRootCookie,
                      lpstEnv->nMaxOperands * sizeof(EOPERATOR_T),
                      (LPLPVOID)&lpstEnv->lpaeOperators) != SS_STATUS_OK)
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
                     lpstEnv->lpaeOperators);
        SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpastOperands);

        return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL EvalStackDestroy()
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

BOOL EvalStackDestroy
(
    LPENV       lpstEnv
)
{
    BOOL        bResult = TRUE;

    if (SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpaeOperators) != SS_STATUS_OK)
        bResult = FALSE;

    if (SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpastOperands) != SS_STATUS_OK)
        bResult = FALSE;

    if (SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpbyEvalDataStore) != SS_STATUS_OK)
        bResult = FALSE;

    return(bResult);
}


//*************************************************************************
//
// BOOL EvalStackDestroy()
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

BOOL EvalStackInit
(
    LPENV       lpstEnv
)
{
    lpstEnv->nOperandsUsed = 0;
    lpstEnv->nOperatorsUsed = 0;
    lpstEnv->nEvalDataStoreUsed = 0;

    return(TRUE);
}


//*************************************************************************
//
// BOOL EvalPushOperand()
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

BOOL EvalPushOperand
(
    LPENV       lpstEnv,
    ETYPE_T     eType,
    LPVOID      lpvData
)
{
    LPOPERAND   lpstOperand;
    int         i;

    // Make sure we are expecting an operand

    if (lpstEnv->nOperatorsUsed == 0 ||
        lpstEnv->lpaeOperators[lpstEnv->nOperatorsUsed-1] !=
            eOPERATOR_WAIT_OPERAND)
    {
        // Double operand?

        return(FALSE);
    }

    // Take away the eOPERATOR_WAIT_OPERAND

    lpstEnv->nOperatorsUsed--;

    // First allocate an operand structure

    if (lpstEnv->nOperandsUsed >= lpstEnv->nMaxOperands)
    {
        // No more room

        return(FALSE);
    }

    lpstOperand = lpstEnv->lpastOperands + lpstEnv->nOperandsUsed;

    // Now store the data

    lpstOperand->eType = eType;
    switch (eType)
    {
        case eTYPE_LONG:
            lpstOperand->uData.lValue = *(LPLONG)lpvData;
            break;

        case eTYPE_STRING:
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

        default:
            // Unknown type

            return(FALSE);
    }

    // Increment the operand usage count

    lpstEnv->nOperandsUsed++;

    return(TRUE);
}


//*************************************************************************
//
// BOOL EvalPushOperand()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  eType           Data type
//  lpvData         Ptr to data
//
// Description:
//  This function pushes a long operand onto the evaluation stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL EvalPushLongOperand
(
    LPENV       lpstEnv,
    long        l
)
{
    return (EvalPushOperand(lpstEnv,
                            eTYPE_LONG,
                            &l));
}


//*************************************************************************
//
// BOOL EvalPushOperand()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  eType           Data type
//  lpvData         Ptr to data
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

BOOL EvalPushSZOperand
(
    LPENV       lpstEnv,
    LPSTR       lpsz
)
{
    BYTE        abyStr[256];
    int         nLen;

    nLen = 0;
    while (nLen < 255 && *lpsz)
        abyStr[++nLen] = (BYTE)*lpsz++;

    abyStr[0] = (BYTE)nLen;

    return (EvalPushOperand(lpstEnv,
                            eTYPE_STRING,
                            abyStr));
}


//*************************************************************************
//
// BOOL EvalDoOperator()
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

BOOL EvalDoOperator
(
    LPENV       lpstEnv,
    EOPERATOR_T eOperator
)
{
    LPOPERAND   lpstOperand0, lpstOperand1;

    // Go back to a consistent state

    if (eOperator == eOPERATOR_WAIT_OPERAND)
    {
        return EvalPushLongOperand(lpstEnv,0);
    }

    // Take care of unary operators

    if (eOperator == eOPERATOR_NEG ||
        eOperator == eOPERATOR_NOT)
    {
        if (lpstEnv->nOperandsUsed < 1)
            return(FALSE);

        lpstOperand0 = lpstEnv->lpastOperands +
            lpstEnv->nOperandsUsed - 1;

        if (lpstOperand0->eType == eTYPE_LONG)
        {
            if (eOperator == eOPERATOR_NEG)
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

    // At this point we know we have the requisite number
    //  of operands on the stack.

    if (lpstOperand0->eType == eTYPE_STRING &&
        lpstOperand1->eType == eTYPE_STRING)
    {
        LPBYTE  lpbyStr0, lpbyStr1;

        // Get ptrs to the strings

        lpbyStr0 = lpstOperand0->uData.lpbyStr;
        lpbyStr1 = lpstOperand1->uData.lpbyStr;

        // Free the strings

        lpstEnv->nEvalDataStoreUsed -=
            lpstOperand0->uData.lpbyStr[0] +
            lpstOperand1->uData.lpbyStr[0] + 2;

        switch (eOperator)
        {
            case eOPERATOR_ADD:
            {
                int     nLen0, nLen1, n;

                // Concatenate the two strings

                nLen0 = *lpbyStr0;
                nLen1 = *lpbyStr1;

                // Move to the end of the buffer

                lpbyStr0 += nLen0 + 1;

                // Move to the first character of the second string

                ++lpbyStr1;

                // Make sure the string doesn't exceed 255 characters

                if ((nLen0 += nLen1) > 255)
                {
                    nLen1 -= nLen0 - 255;
                    nLen0 = 255;
                }

                n = 0;
                while (n++ < nLen1)
                    *lpbyStr0++ = *lpbyStr1++;

                lpstOperand0->uData.lpbyStr[0] = (BYTE)nLen0;

                // Update data store usage

                lpstEnv->nEvalDataStoreUsed += nLen0 + 1;

                return(TRUE);
            }

            case eOPERATOR_EQ:
            {
                lpstOperand0->eType = eTYPE_LONG;
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

            case eOPERATOR_NE:
            {
                lpstOperand0->eType = eTYPE_LONG;
                if (*lpbyStr0 != *lpbyStr1)
                    lpstOperand0->uData.lValue = -1;
                else
                if (PascalStrCmp(lpbyStr0,lpbyStr1) == 0)
                    lpstOperand0->uData.lValue = 0;
                else
                    lpstOperand0->uData.lValue = -1;
                return(TRUE);
            }

            case eOPERATOR_LT:
            {
                lpstOperand0->eType = eTYPE_LONG;
                if (PascalStrCmp(lpbyStr0,lpbyStr1) < 0)
                    lpstOperand0->uData.lValue = -1;
                else
                    lpstOperand0->uData.lValue = 0;
                return(TRUE);
            }

            case eOPERATOR_GT:
            {
                lpstOperand0->eType = eTYPE_LONG;
                if (PascalStrCmp(lpbyStr0,lpbyStr1) > 0)
                    lpstOperand0->uData.lValue = -1;
                else
                    lpstOperand0->uData.lValue = 0;
                return(TRUE);
            }

            case eOPERATOR_LE:
            {
                lpstOperand0->eType = eTYPE_LONG;
                if (PascalStrCmp(lpbyStr0,lpbyStr1) <= 0)
                    lpstOperand0->uData.lValue = -1;
                else
                    lpstOperand0->uData.lValue = 0;
                return(TRUE);
            }

            case eOPERATOR_GE:
            {
                lpstOperand0->eType = eTYPE_LONG;
                if (PascalStrCmp(lpbyStr0,lpbyStr1) >= 0)
                    lpstOperand0->uData.lValue = -1;
                else
                    lpstOperand0->uData.lValue = 0;
                return(TRUE);
            }

            default:
                // Restore the first string and ignore

                lpstEnv->nEvalDataStoreUsed +=
                    lpstOperand0->uData.lpbyStr[0] + 1;
                return(TRUE);
        }
    }
    else
    if (lpstOperand0->eType == eTYPE_LONG &&
        lpstOperand1->eType == eTYPE_LONG)
    {
        switch (eOperator)
        {
            case eOPERATOR_ADD:
                lpstOperand0->uData.lValue += lpstOperand1->uData.lValue;
                return(TRUE);

            case eOPERATOR_SUB:
                lpstOperand0->uData.lValue -= lpstOperand1->uData.lValue;
                return(TRUE);

            case eOPERATOR_MUL:
                lpstOperand0->uData.lValue *= lpstOperand1->uData.lValue;
                return(TRUE);

            case eOPERATOR_DIV:
                // Prevent divides by zero

                if (lpstOperand1->uData.lValue != 0)
                    lpstOperand0->uData.lValue /= lpstOperand1->uData.lValue;

                return(TRUE);

            case eOPERATOR_MOD:
                // Prevent divides by zero

                if (lpstOperand1->uData.lValue != 0)
                    lpstOperand0->uData.lValue %= lpstOperand1->uData.lValue;

                return(TRUE);

            case eOPERATOR_EQ:
                if (lpstOperand0->uData.lValue == lpstOperand1->uData.lValue)
                    lpstOperand0->uData.lValue = -1;
                else
                    lpstOperand0->uData.lValue = 0;
                return(TRUE);

            case eOPERATOR_NE:
                if (lpstOperand0->uData.lValue != lpstOperand1->uData.lValue)
                    lpstOperand0->uData.lValue = -1;
                else
                    lpstOperand0->uData.lValue = 0;
                return(TRUE);

            case eOPERATOR_LT:
                if (lpstOperand0->uData.lValue < lpstOperand1->uData.lValue)
                    lpstOperand0->uData.lValue = -1;
                else
                    lpstOperand0->uData.lValue = 0;
                return(TRUE);

            case eOPERATOR_GT:
                if (lpstOperand0->uData.lValue > lpstOperand1->uData.lValue)
                    lpstOperand0->uData.lValue = -1;
                else
                    lpstOperand0->uData.lValue = 0;
                return(TRUE);

            case eOPERATOR_LE:
                if (lpstOperand0->uData.lValue <= lpstOperand1->uData.lValue)
                    lpstOperand0->uData.lValue = -1;
                else
                    lpstOperand0->uData.lValue = 0;
                return(TRUE);

            case eOPERATOR_GE:
                if (lpstOperand0->uData.lValue >= lpstOperand1->uData.lValue)
                    lpstOperand0->uData.lValue = -1;
                else
                    lpstOperand0->uData.lValue = 0;
                return(TRUE);

            case eOPERATOR_AND:
                lpstOperand0->uData.lValue &= lpstOperand1->uData.lValue;
                return(TRUE);

            case eOPERATOR_OR:
                lpstOperand0->uData.lValue |= lpstOperand1->uData.lValue;
                return(TRUE);

            default:
                // Ignore, leave the first operand
                return(TRUE);
        }
    }

    // Free the second operand

    if (lpstOperand1->eType == eTYPE_STRING)
    {
        lpstEnv->nEvalDataStoreUsed -=
            lpstOperand1->uData.lpbyStr[0] + 1;
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL EvalQueryTopOperand()
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

BOOL EvalQueryTopOperand
(
    LPENV       lpstEnv,
    LPETYPE     lpeType,
    LPLPVOID    lplpvData
)
{
    LPOPERAND   lpstOperand;

    if (lpstEnv->nOperandsUsed == 0)
    {
        // This should never happen

        return(FALSE);
    }

    lpstOperand = lpstEnv->lpastOperands + lpstEnv->nOperandsUsed - 1;
    *lpeType = lpstOperand->eType;
    switch (lpstOperand->eType)
    {
        case eTYPE_LONG:
            *lplpvData = &lpstOperand->uData.lValue;
            break;

        case eTYPE_STRING:
            *lplpvData = lpstOperand->uData.lpbyStr;
            break;

        default:
            break;
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL EvalPopTopOperand()
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

BOOL EvalPopTopOperand
(
    LPENV       lpstEnv
)
{
    LPOPERAND   lpstOperand;

    assert(lpstEnv->nOperandsUsed > 0);

    lpstOperand = lpstEnv->lpastOperands + lpstEnv->nOperandsUsed - 1;
    switch (lpstOperand->eType)
    {
        case eTYPE_STRING:
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
// int EvalGetOperatorPrecedence()
//
// Parameters:
//  eOperator       Operator
//
// Description:
//  This function returns the precedence of the given operator.
//
// Returns:
//  int             Precedence value
//
//*************************************************************************

int EvalGetOperatorPrecedence
(
    EOPERATOR_T eOperator
)
{
    // Determine precedence value of new operator
    //  1. NEG NOT
    //  2. MUL DIV MOD
    //  3. ADD SUB
    //  4. EQ NE LT GT LE GE
    //  5. AND OR

    switch (eOperator)
    {
        case eOPERATOR_NEG:
        case eOPERATOR_NOT:
            return(100);

        case eOPERATOR_MUL:
        case eOPERATOR_DIV:
        case eOPERATOR_MOD:
            return(200);

        case eOPERATOR_ADD:
        case eOPERATOR_SUB:
            return(300);

        case eOPERATOR_EQ:
        case eOPERATOR_NE:
        case eOPERATOR_LT:
        case eOPERATOR_GT:
        case eOPERATOR_LE:
        case eOPERATOR_GE:
            return(400);

        case eOPERATOR_AND:
        case eOPERATOR_OR:
            return(500);

        // OPEN_PAREN and CLOSE_PAREN should always be adjacent

        case eOPERATOR_CLOSE_PAREN:
            return(600);

        case eOPERATOR_OPEN_PAREN:
            return(601);

        // END and BEGIN should always be adjacent

        case eOPERATOR_END:
            return(700);

        case eOPERATOR_BEGIN:
            return(701);

        default:
            // This should never happen
            return(0);
    }
}


//*************************************************************************
//
// BOOL EvalPopTopOperand()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function pushes the begin operator onto the operator stack
//  to prepare for the evaluation of an expression.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL EvalPushBegin
(
    LPENV       lpstEnv
)
{
    if (lpstEnv->nOperatorsUsed >= lpstEnv->nMaxOperands)
        return(FALSE);

    lpstEnv->lpaeOperators[lpstEnv->nOperatorsUsed++] = eOPERATOR_BEGIN;

    // Push eOPERATOR_WAIT_OPERAND

    if (lpstEnv->nOperatorsUsed >= lpstEnv->nMaxOperands)
        return(FALSE);

    lpstEnv->lpaeOperators[lpstEnv->nOperatorsUsed++] =
        eOPERATOR_WAIT_OPERAND;

    return(TRUE);
}


//*************************************************************************
//
// BOOL EvalPushOperator()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  eOperator       Operator to push
//
// Description:
//  This function pushes the given operator onto the operator stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL EvalPushOperator
(
    LPENV       lpstEnv,
    EOPERATOR_T eOperator
)
{
    int         nNewOpPrecedence;
    int         nHeldOpPrecedence;
    EOPERATOR_T eHeldOperator;

    // If the top waiting operator is eOPERATOR_WAIT_OPERAND,
    //  then there was no push of an operand immediately after
    //  the last operator push

    if (lpstEnv->nOperatorsUsed != 0)
    {
        if (lpstEnv->lpaeOperators[lpstEnv->nOperatorsUsed-1] ==
            eOPERATOR_WAIT_OPERAND)
        {
            // The only allowed operators are then negation
            //  NOT, and open parenthesis

            switch (eOperator)
            {
                case eOPERATOR_SUB:
                    // Change to negation

                    eOperator = eOPERATOR_NEG;
                    break;

                case eOPERATOR_NEG:
                case eOPERATOR_NOT:
                case eOPERATOR_OPEN_PAREN:
                case eOPERATOR_END:
                    break;

                default:
                    // Syntax error in the expression

                    return(FALSE);
            }

            // Pop eOPERATOR_WAIT_OPERAND

            lpstEnv->nOperatorsUsed--;
        }
    }

    nNewOpPrecedence = EvalGetOperatorPrecedence(eOperator);

    // Always push open parenthesis, begin, begin and not

    if (eOperator != eOPERATOR_OPEN_PAREN &&
        eOperator != eOPERATOR_BEGIN &&
        eOperator != eOPERATOR_NEG &&
        eOperator != eOPERATOR_NOT)
    {
        /////////////////////////////////////////////////////////
        // Do operator on stack as long as the new operator has
        //  lower or equal precedence
        /////////////////////////////////////////////////////////

        while (1)
        {
            // There should always be an operator on the stack
            //  because eOPERATOR_BEGIN should have been pushed
            //  at the beginning of the expression

            if (lpstEnv->nOperatorsUsed == 0)
                return(FALSE);

            // Get the precedence of the held operator

            eHeldOperator =
                lpstEnv->lpaeOperators[lpstEnv->nOperatorsUsed - 1];

            nHeldOpPrecedence = EvalGetOperatorPrecedence(eHeldOperator);

            // Does the new operator have higher precedence?
            //  If so, then break out

            if (nNewOpPrecedence < nHeldOpPrecedence)
                break;

            // Apply the operator

            if (EvalDoOperator(lpstEnv,
                               eHeldOperator) == FALSE)
            {
                // Failure applying operator

                return(FALSE);
            }

            // Move to the next operator

            lpstEnv->nOperatorsUsed--;
        }

        // At this point, the new operator should have higher
        //  precedence than the held operator
    }


    /////////////////////////////////////////////////////////////
    // Now decide what to do with the new operator
    /////////////////////////////////////////////////////////////

    if (eOperator == eOPERATOR_CLOSE_PAREN)
    {
        // If the held operator is not an open parenthesis
        //  then there is an error in the expression

        if (lpstEnv->lpaeOperators[lpstEnv->nOperatorsUsed - 1] !=
            eOPERATOR_OPEN_PAREN)
            return(FALSE);

        // Pop the open parenthesis

        lpstEnv->nOperatorsUsed--;
        return(TRUE);
    }
    else
    if (eOperator == eOPERATOR_END)
    {
        // If the held operator is not eOPERATOR_BEGIN
        //  then there is an error parsing

        if (lpstEnv->lpaeOperators[lpstEnv->nOperatorsUsed - 1] !=
            eOPERATOR_BEGIN)
            return(FALSE);

        // Pop the begin

        lpstEnv->nOperatorsUsed--;
        return(TRUE);
    }

    // Otherwise, push the operator

    if (lpstEnv->nOperatorsUsed >= lpstEnv->nMaxOperands)
        return(FALSE);

    lpstEnv->lpaeOperators[lpstEnv->nOperatorsUsed++] = eOperator;

    // Push eOPERATOR_WAIT_OPERAND

    if (lpstEnv->nOperatorsUsed >= lpstEnv->nMaxOperands)
        return(FALSE);

    lpstEnv->lpaeOperators[lpstEnv->nOperatorsUsed++] =
        eOPERATOR_WAIT_OPERAND;

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

BOOL EvalGetAndPopTopAsBoolean
(
    LPENV       lpstEnv,
    LPBOOL      lpbResult
)
{
    ETYPE_T     eType;
    LPVOID      lpvData;

    // Check the result of the top operand

    if (EvalQueryTopOperand(lpstEnv,
                            &eType,
                            &lpvData) == FALSE)
    {
        // Error getting result of if conditional

        return(FALSE);
    }

    switch (eType)
    {
        case eTYPE_LONG:
            if (*(LPLONG)lpvData == 0)
                *lpbResult = FALSE;
            else
                *lpbResult = TRUE;
            break;

        case eTYPE_STRING:
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

    if (EvalPopTopOperand(lpstEnv) == FALSE)
    {
        // Failed to pop

        return(FALSE);
    }

    return(TRUE);
}
