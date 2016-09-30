//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97DOOP2.CPv   1.2   12 Oct 1998 13:21:18   DCHI  $
//
// Description:
//  Handling of opcodes 0x20-0x2F.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97DOOP2.CPv  $
// 
//    Rev 1.2   12 Oct 1998 13:21:18   DCHI
// - Added VAR_ID to VAR translation in O97_OP_25_DOT_FUNCTION_EVAL().
// - Added support of essO97_DO state in O97_OP_2E_ASSIGN_USING_SET()
//   so it is now in sync with behavior of assignment operation.
// 
//    Rev 1.1   17 Nov 1997 14:15:48   DCHI
// Fixed O97_OP_24_FUNCTION_EVAL() to correctly check for array element types.
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
// BOOL O97_OP_20_EVAL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function uses the following order for handling the
//  identifier:
//      1. Top-level known identifier
//      2. Built-in O97 identifier
//      3. Function/Module identifier
//      4. Variable identifier
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_20_EVAL
(
    LPO97_ENV       lpstEnv
)
{
    BOOL            bKnown;
    DWORD           dwObjectID;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x20);

    //printf("O97_OP_20_EVAL\n");

    // No parameters

    lpstEnv->nNumParams = 0;

    ////////////////////////////////////////////////////////
    // Is it a top-level known identifier?

    // See if it is a top level known identifier, e.g. WordBasic

    if (lpstEnv->lpstObjectFunc->
        ObjectEvaluate(lpstEnv,
                       0,
                       &bKnown) == FALSE)
        return(FALSE);

    if (bKnown == TRUE)
        return(TRUE);

    ////////////////////////////////////////////////////////
    // Is it an O97 identifier

    dwObjectID = O97GetO97IdentEnum(lpstEnv,lpstEnv->wOpID);
    if (dwObjectID != 0xFFFF)
    {
        switch (dwObjectID)
        {
            case eO97_STR_application:
            case eO97_STR_err:
                return O97EvalPushOperand(lpstEnv,
                                          eO97_TYPE_O97_OBJECT_ID,
                                          &dwObjectID);

            default:
                break;
        }
    }

    ////////////////////////////////////////////////////////
    // Is it a function?

    // Try to call as a function

    if (O97SubStateTop(lpstEnv) == essO97_DO &&
        O97CallSetup(lpstEnv,
                     lpstEnv->wOpID,
                     O97_ID_INVALID) != FALSE)
        return(TRUE);

    // We are either skipping or the identifier was not a
    //  function, so presumably it is not important
    //  that we actually evaluate the identifier

    // Just push it as a variable identifier

    ////////////////////////////////////////////////////////
    // Is it a module identifier

    // Just push it as a var id, the user will have the
    //  responsibility of determining whether it is a module
    //  identifier or not.

    ////////////////////////////////////////////////////////
    // Assume it is a variable identifier

    // Otherwise, push it as a variable

    return O97EvalPushOperand(lpstEnv,
                              eO97_TYPE_VAR_ID,
                              &lpstEnv->wOpID);
}


//********************************************************************
//
// BOOL O97_OP_21_DOT_EVAL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  The application's SubObjectEvaluate() is called.  If the
//  parent object is unknown by the application, and is Err,
//  then the error value is returned.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_21_DOT_EVAL
(
    LPO97_ENV       lpstEnv
)
{
    EO97_TYPE_T     eParentType;
    LPVOID          lpvParentData;
    BOOL            bKnown;
    DWORD           dwObjectID;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x21);

    //printf("O97_OP_21_DOT_EVAL\n");

    // No parameters

    lpstEnv->nNumParams = 0;

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
            eParentType = eO97_TYPE_OBJECT_ID;
            dwObjectID = O97_OBJECT_UNKNOWN_ID;
            lpvParentData = &dwObjectID;
            break;
    }

    if (lpstEnv->lpstObjectFunc->
        SubObjectEvaluate(lpstEnv,
                          0,
                          eParentType,
                          lpvParentData,
                          &bKnown) == FALSE)
        return(FALSE);

    if (bKnown == FALSE && eParentType == eO97_TYPE_O97_OBJECT_ID)
    {
        if (*(LPDWORD)lpvParentData == eO97_STR_err)
        {
            // Push the err value

            return O97EvalPushLongOperand(lpstEnv,lpstEnv->lErr);
        }
    }

    // For now, don't do external calls with no parameters

    if (bKnown == FALSE)
    {
        // Push an unknown object

        dwObjectID = O97_OBJECT_UNKNOWN_ID;

        return O97EvalPushOperand(lpstEnv,
                                  eO97_TYPE_OBJECT_ID,
                                  &dwObjectID);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_22()
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

BOOL O97_OP_22
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x22);

//    printf("O97_OP_22\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_23()
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

BOOL O97_OP_23
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x23);

//    printf("O97_OP_23\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_24_FUNCTION_EVAL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  The order of evaluation is:
//      1. Top-level known identifier
//      2. Array variable identifier
//      3. Function identifier
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_24_FUNCTION_EVAL
(
    LPO97_ENV       lpstEnv
)
{
    BOOL            bKnown;
    LPO97_VAR       lpstVar;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x24);

    //printf("O97_OP_24_FUNCTION_EVAL\n");

    // Push the parameters onto the parameter stack

    if (O97ParamGetParams(lpstEnv) == FALSE)
        return(FALSE);

    ////////////////////////////////////////////////////////
    // Is it a top-level known identifier?

    // See if it is a top level known identifier

    if (lpstEnv->lpstObjectFunc->
        ObjectEvaluate(lpstEnv,
                       lpstEnv->wOpCount,
                       &bKnown) == FALSE)
        return(FALSE);

    if (bKnown == TRUE)
        return(TRUE);

    ////////////////////////////////////////////////////////
    // See if we are skipping

    if (O97SubStateTop(lpstEnv) != essO97_DO)
    {
        // We are skipping, so presumably it is not important
        //  that we actually evaluate the identifier

        if (lpstEnv->abyOpcode[1] == (BYTE)eO97_DEFTYPE_STR)
            return O97EvalPushSZOperand(lpstEnv,NULL);

        return O97EvalPushLongOperand(lpstEnv,0);
    }

    ////////////////////////////////////////////////////////
    // Is it a variable?

    lpstVar = O97VarArrayGet(lpstEnv,lpstEnv->wOpID);
    if (lpstEnv->nNumParams > 0 && lpstVar != NULL)
    {
        long            lIndex0, lIndex1;
        LPO97_ELEMENT   lpstElement;

        // It's an array variable

        if (O97ParamGetLong(lpstEnv,0,&lIndex0) == FALSE)
            lIndex0 = 0;

        if (lpstEnv->nNumParams == 1 ||
            O97ParamGetLong(lpstEnv,1,&lIndex1) == FALSE)
            lIndex1 = 0;

        lpstElement = O97VarArrayGetElement(lpstVar,
                                            (WORD)lIndex0,
                                            (WORD)lIndex1);

        if (lpstElement == NULL)
        {
            if (lpstEnv->abyOpcode[1] == (BYTE)eO97_DEFTYPE_STR)
                return O97EvalPushSZOperand(lpstEnv,NULL);

            return O97EvalPushLongOperand(lpstEnv,0);
        }

        // Return the value of the element

        if (lpstElement->eType == eO97_TYPE_LONG)
            return O97EvalPushLongOperand(lpstEnv,
                                          lpstElement->uData.lValue);
        else
        if (lpstElement->eType == eO97_TYPE_STRING)
            return O97EvalPushOperand(lpstEnv,
                                      lpstElement->eType,
                                      lpstElement->uData.lpbyStr);
        else
            return O97EvalPushOperand(lpstEnv,
                                      eO97_TYPE_STRING,
                                      "");
    }

    ////////////////////////////////////////////////////////
    // Is it a function?

    // Get the location of the function

    // Try to call as a function

    if (O97CallSetup(lpstEnv,
                     lpstEnv->wOpID,
                     O97_ID_INVALID) == FALSE)
    {
        // Just ignore it

        if (lpstEnv->abyOpcode[1] == (BYTE)eO97_DEFTYPE_STR)
            return O97EvalPushSZOperand(lpstEnv,NULL);

        return O97EvalPushLongOperand(lpstEnv,0);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_25_DOT_FUNCTION_EVAL()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Evaluates a subobject that returns a value.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_25_DOT_FUNCTION_EVAL
(
    LPO97_ENV       lpstEnv
)
{
    BOOL            bKnown;
    EO97_TYPE_T     eParentType;
    LPVOID          lpvParentData;
    DWORD           dwObjectID;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x25);

//    printf("O97_OP_25_DOT_FUNCTION_EVAL\n");

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
            eParentType = eO97_TYPE_OBJECT_ID;
            dwObjectID = O97_OBJECT_UNKNOWN_ID;
            lpvParentData = &dwObjectID;
            break;
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

    // Could be a string call

    if (lpstEnv->bIsStringCall == TRUE)
        O97SetupCallUsingString(lpstEnv);

    if (bKnown == FALSE && eParentType == eO97_TYPE_VAR_ID ||
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
                                 lpstEnv->wCallRoutineID,
                                 O97_ID_INVALID) == FALSE)
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
                             lpstEnv->wCallRoutineID,
                             O97_ID_INVALID) == FALSE)
                return(FALSE);
        }

        bKnown = FALSE;
    }

    if (bKnown == FALSE)
    {
        // Push an unknown object

        dwObjectID = O97_OBJECT_UNKNOWN_ID;

        return O97EvalPushOperand(lpstEnv,
                                  eO97_TYPE_OBJECT_ID,
                                  &dwObjectID);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_26()
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

BOOL O97_OP_26
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x26);

//    printf("O97_OP_26\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_27_ASSIGN()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Assigns a value to a top level object.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_27_ASSIGN
(
    LPO97_ENV       lpstEnv
)
{
    EO97_TYPE_T     eType;
    LPVOID          lpvData;
    LPO97_VAR       lpstVar;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x27);

    //printf("O97_OP_27_ASSIGN\n");

    // Get the data to assign to the variable

    if (O97EvalQueryTopOperand(lpstEnv,
                               &eType,
                               &lpvData) == FALSE)
    {
        return(FALSE);
    }

    // Get the variable

    lpstVar = O97VarGet(lpstEnv,lpstEnv->wOpID);

    // If we are skipping and the variable hasn't yet been assigned
    //  a value, just assign one anyway only if it is a string
    //  value that is being assigned, since it shouldn't hurt
    //  in most cases since uninitialized variables should not
    //  affect true program execution in correct programs

    if (O97SubStateTop(lpstEnv) == essO97_DO ||
        (lpstVar == NULL && eType == eO97_TYPE_STRING))
    {
        // Allocate the variable if necessary

        if (lpstVar == NULL)
        {
            // Create it

            lpstVar = O97VarAlloc(lpstEnv,
                                  O97ModEnvVarIsGlobal(lpstEnv,lpstEnv->wOpID),
                                  FALSE,
                                  lpstEnv->wOpID);

            if (lpstVar == NULL)
            {
                // Could not allocate variable

                return(FALSE);
            }
        }

        if (O97VarSet(lpstEnv,
                      lpstVar,
                      eType,
                      lpvData) == NULL)
        {
            return(FALSE);
        }
    }

    // Pop top operand value that was assigned

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
    {
        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_28_DOT_ASSIGN()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Assigns a value to a subobject.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_28_DOT_ASSIGN
(
    LPO97_ENV       lpstEnv
)
{
    EO97_TYPE_T     eParentType;
    LPVOID          lpvParentData;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x28);

//    printf("O97_OP_28_DOT_ASSIGN\n");

    // There must be some object on the evaluation stack

    if (O97EvalQueryTopOperand(lpstEnv,
                               &eParentType,
                               &lpvParentData) == FALSE)
        return(FALSE);

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

    if (eParentType == eO97_TYPE_VAR_ID)
    {
        LPO97_VAR       lpstVar;

        lpstVar = O97VarGet(lpstEnv,*(LPWORD)lpvParentData);
        if (lpstVar != NULL && lpstVar->eType == eO97_TYPE_DIALOG)
        {
            if (lpstEnv->lpstObjectFunc->
                AssignDialogField(lpstEnv,
                                  (LPO97_DIALOG)lpstVar->uData.lpvstDlg) != FALSE)
                return(TRUE);
        }
    }

    if (eParentType == eO97_TYPE_O97_OBJECT_ID)
    {
        if (*(LPDWORD)lpvParentData == eO97_STR_err)
        {
            long    lResult;

            if (O97EvalGetAndPopTopAsLong(lpstEnv,&lResult) != FALSE)
            {
                // Assign the error value

                lpstEnv->lErr = lResult;
                return(TRUE);
            }
        }
    }

    // Just pop the operand and return

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
    {
        // Failed to pop

        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_29()
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

BOOL O97_OP_29
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x29);

//    printf("O97_OP_29\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_2A()
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

BOOL O97_OP_2A
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x2A);

//    printf("O97_OP_2A\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_2B_ASSIGN_FUNCTION()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Assigns a value to an array element.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_2B_ASSIGN_FUNCTION
(
    LPO97_ENV       lpstEnv
)
{
    LPO97_VAR       lpstVar;
    LPO97_ELEMENT   lpstElement;
    long            lIndex0, lIndex1;
    EO97_TYPE_T     eType;
    LPVOID          lpvData;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x2B);

//    printf("O97_OP_2B_ASSIGN_FUNCTION\n");

    if (O97SubStateTop(lpstEnv) != essO97_DO)
    {
        // Pop the parameters and then the parameters to assign

        if (O97ParamPopParams(lpstEnv) == NULL)
            return(FALSE);

        // Pop the value to assign

        if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        {
            return(FALSE);
        }
    
        return(TRUE);
    }

    // Push the parameters onto the parameter stack

    if (O97ParamGetParams(lpstEnv) == FALSE)
        return(FALSE);

    // If there are no indices, what can we do?

    if (lpstEnv->nNumParams <= 0)
        return(FALSE);

    // See if an array variable by the given name exists

    lpstVar = O97VarArrayGet(lpstEnv,lpstEnv->wOpID);
    if (lpstVar == NULL)
    {
        // Have to create it

        lpstVar = O97VarAlloc(lpstEnv,
                              O97ModEnvVarIsGlobal(lpstEnv,lpstEnv->wOpID),
                              TRUE,
                              lpstEnv->wOpID);

        if (lpstVar == NULL)
        {
            // Could not allocate variable

            return(FALSE);
        }
    }

    // Get the indices

    if (O97ParamGetLong(lpstEnv,0,&lIndex0) == FALSE)
        lIndex0 = 0;

    if (lpstEnv->nNumParams == 1 ||
        O97ParamGetLong(lpstEnv,1,&lIndex1) == FALSE)
        lIndex1 = 0;

    // Get a pointer to the element

    lpstElement = O97VarArrayGetElement(lpstVar,
                                        (WORD)lIndex0,
                                        (WORD)lIndex1);

    if (lpstElement == NULL)
    {
        // Create the element

        lpstElement = O97VarArrayElementAlloc(lpstEnv,
                                              lpstVar,
                                              (WORD)lIndex0,
                                              (WORD)lIndex1);

    }

    if (lpstElement != NULL)
    {
        // Get the value to assign

        if (O97EvalQueryTopOperand(lpstEnv,
                                   &eType,
                                   &lpvData) == FALSE)
        {
            return(FALSE);
        }

        // Now set the new value of the element

        if (O97VarSet(lpstEnv,
                      (LPO97_VAR)lpstElement,
                      eType,
                      lpvData) == FALSE)
            return(FALSE);
    }

    // Pop top operand value that was assigned

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
    {
        return(FALSE);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_2C_ASSIGN_DOT_FUNCTION()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pops the top operand and ignores the operation.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_2C_ASSIGN_DOT_FUNCTION
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x2C);

//    printf("O97_OP_2C_ASSIGN_DOT_FUNCTION\n");

    // Just pop the parent object

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

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
// BOOL O97_OP_2D()
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

BOOL O97_OP_2D
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x2D);

//    printf("O97_OP_2D\n");

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_2E_ASSIGN_USING_SET()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Sets the value of a variable to be a reference to an object.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_2E_ASSIGN_USING_SET
(
    LPO97_ENV       lpstEnv
)
{
    EO97_TYPE_T     eType;
    LPVOID          lpvData;
    LPO97_VAR       lpstVar;

    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x2E);

//    printf("O97_OP_2E_ASSIGN_USING_SET\n");

    // If the top operand is a variable ID, set the variable as a
    //  reference.  Otherwise, ignore the set.

    // Get the data to assign to the variable

    if (O97EvalQueryTopOperand(lpstEnv,
                               &eType,
                               &lpvData) == FALSE)
        return(FALSE);

    if (O97SubStateTop(lpstEnv) == essO97_DO)
    {
        if (eType == eO97_TYPE_OBJECT_ID)
        {
            // Does the variable exist?

            lpstVar = O97VarGet(lpstEnv,lpstEnv->wOpID);

            if (lpstVar == NULL)
            {
                lpstVar = O97VarAlloc(lpstEnv,
                                      O97ModEnvVarIsGlobal(lpstEnv,
                                                           lpstEnv->wOpID),
                                      FALSE,
                                      lpstEnv->wOpID);

                if (lpstVar == NULL)
                    return(FALSE);
            }

            // Make the variable an object

            if (O97VarSet(lpstEnv,
                          lpstVar,
                          eO97_TYPE_OBJECT_ID,
                          lpvData) == NULL)
                return(FALSE);
        }
        else
        if (eType == eO97_TYPE_DIALOG_ID)
        {
            O97_DIALOG_T    stDlg;

            // Does the variable exist?

            lpstVar = O97VarGet(lpstEnv,lpstEnv->wOpID);

            if (lpstVar == NULL)
            {
                lpstVar = O97VarAlloc(lpstEnv,
                                      O97ModEnvVarIsGlobal(lpstEnv,
                                                           lpstEnv->wOpID),
                                      FALSE,
                                      lpstEnv->wOpID);

                if (lpstVar == NULL)
                    return(FALSE);
            }

            // Make the variable a dialog

            lpstEnv->lpstObjectFunc->
                GetDialogInfo(&stDlg,
                              *(LPDWORD)lpvData);

            if (O97VarSet(lpstEnv,
                          lpstVar,
                          eO97_TYPE_DIALOG,
                          &stDlg) == NULL)
                return(FALSE);

            // Initialize the dialog fields

            lpstEnv->lpstObjectFunc->
                InitDialogInfo(lpstEnv,
                               (LPO97_DIALOG)lpstVar->uData.lpvstDlg);
        }
    }

    // Pop top operand value that was assigned

    if (O97EvalPopTopOperand(lpstEnv) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97_OP_2F_ASSIGN_DOTTED_SET()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Pops the top two operands which are the parent object and the
//  value to assign.  The assignment is ignored.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//********************************************************************

BOOL O97_OP_2F_ASSIGN_DOTTED_SET
(
    LPO97_ENV       lpstEnv
)
{
    (void)lpstEnv;

    assert(lpstEnv->abyOpcode[0] == 0x2F);

//    printf("O97_OP_2F_ASSIGN_DOTTED_SET\n");

    // Just pop the top operand

    if (O97EvalPopParameters(lpstEnv,2) == FALSE)
        return(FALSE);

    return(TRUE);
}


