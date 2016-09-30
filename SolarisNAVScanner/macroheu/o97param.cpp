//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97PARAM.CPv   1.1   12 Oct 1998 13:25:50   DCHI  $
//
// Description:
//  Parameter management functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97PARAM.CPv  $
// 
//    Rev 1.1   12 Oct 1998 13:25:50   DCHI
// Added handling of O97_TYPE_OBJECT_ID in O97ParamPut()
// and in O97ParamGetValue().
// 
//    Rev 1.0   15 Aug 1997 13:29:34   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"

//*************************************************************************
//
// BOOL O97ParamCreate()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function allocates memory for storing parameters.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ParamCreate
(
    LPO97_ENV       lpstEnv
)
{
    // Allocate variable structure storage

    lpstEnv->nNumParams = 0;

    // Create parameter data store

    lpstEnv->dwGSMParamSize = 1024;
    lpstEnv->lpstGSMParamData = GSMCreate(lpstEnv->lpvRootCookie,
                                          lpstEnv->dwGSMParamSize);
    if (lpstEnv->lpstGSMParamData == NULL)
        return(FALSE);

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ParamDestroy()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function frees memory allocated for storing parameters.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ParamDestroy
(
    LPO97_ENV       lpstEnv
)
{
    GSMDestroy(lpstEnv->lpvRootCookie,
               lpstEnv->lpstGSMParamData);

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ParamInitList()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function initializes the parameter structure for storing
//  parameters.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ParamInitList
(
    LPO97_ENV       lpstEnv
)
{
    lpstEnv->nNumParams = 0;
    GSMInit(lpstEnv->lpstGSMParamData,
            lpstEnv->dwGSMParamSize);

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ParamPut()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  wNamedParamID   ID if parameter is named
//  eType           Type of parameter
//  lpvData         Ptr to parameter data
//
// Description:
//  This function adds a parameter to the parameter list.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ParamPut
(
    LPO97_ENV       lpstEnv,
    WORD            wNamedParamID,
    EO97_TYPE_T     eType,
    LPVOID          lpvData
)
{
    LPO97_PARAM     lpstParam;
    int             i;

    if (lpstEnv->nNumParams >= O97_MAX_PARAMS)
        return(FALSE);

    lpstParam = lpstEnv->astParams + lpstEnv->nNumParams++;

    lpstParam->wNamedParamID = wNamedParamID;
    lpstParam->eType = eType;
    switch (eType)
    {
        case eO97_TYPE_LONG:
            // Store the long value

            lpstParam->uData.lValue = *(LPLONG)lpvData;
            break;

        case eO97_TYPE_STRING:
            // Store the string data

            lpstParam->uData.lpbyStr =
                (LPBYTE)GSMAlloc(lpstEnv->lpstGSMParamData,
                                 *(LPBYTE)lpvData + 1);

            if (lpstParam->uData.lpbyStr == NULL)
                return(FALSE);

            for (i=0;i<=*(LPBYTE)lpvData;i++)
                lpstParam->uData.lpbyStr[i] = ((LPBYTE)lpvData)[i];

            break;

        case eO97_TYPE_VAR_ID:
        {
            LPO97_VAR   lpstVar;

            lpstVar = O97VarGet(lpstEnv,*(LPWORD)lpvData);
            if (lpstVar == NULL)
            {
                // Need to create it

                // See if the variable already exists
                // Determine whether it is a global or local variable

                lpstVar = O97VarAlloc(lpstEnv,
                                      O97ModEnvVarIsGlobal(lpstEnv,
                                                           *(LPWORD)lpvData),
                                      FALSE,
                                      *(LPWORD)lpvData);

                if (lpstVar == NULL)
                    return(FALSE);
            }

            lpstParam->eType = eO97_TYPE_VAR;
            lpstParam->uData.lpvstVar = lpstVar;
            break;
        }

        case eO97_TYPE_OBJECT_ID:
            lpstParam->eType = eO97_TYPE_OBJECT_ID;
            lpstParam->uData.dwObjectID = *(LPDWORD)lpvData;
            break;

        default:
            lpstParam->eType = eO97_TYPE_UNKNOWN;
            break;
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ParamGetNamedParamID()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  nIndex          Index of parameter
//  lpwNamedParamID Ptr to WORD for ID
//
// Description:
//  This function returns in *lpwNamedParamID the ID of the parameter
//  if it has been named and O97_UNNAMED_PARAM_ID otherwise.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ParamGetNamedParamID
(
    LPO97_ENV       lpstEnv,
    int             nIndex,
    LPWORD          lpwNamedParamID
)
{
    assert(nIndex >= 0);

    if (nIndex >= lpstEnv->nNumParams)
        return(FALSE);

    *lpwNamedParamID =
        (lpstEnv->astParams + lpstEnv->nNumParams - nIndex - 1)->
            wNamedParamID;

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ParamGetValue()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  nIndex          Index of parameter
//  lpeType         Ptr for parameter type
//  lplpvData       Ptr to ptr for data
//
// Description:
//  This function attempts to get the type and data pointer for a
//  parameter.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ParamGetValue
(
    LPO97_ENV       lpstEnv,
    int             nIndex,
    LPEO97_TYPE     lpeType,
    LPLPVOID        lplpvData
)
{
    LPO97_PARAM     lpstParam;

    assert(nIndex >= 0);

    if (nIndex >= lpstEnv->nNumParams)
        return(FALSE);

    lpstParam = lpstEnv->astParams + lpstEnv->nNumParams - nIndex - 1;
    *lpeType = lpstParam->eType;
    switch (lpstParam->eType)
    {
        case eO97_TYPE_VAR:
            *lplpvData = lpstParam->uData.lpvstVar;
            break;

        case eO97_TYPE_STRING:
            *lplpvData = lpstParam->uData.lpbyStr;
            break;

        case eO97_TYPE_LONG:
            *lplpvData = &lpstParam->uData.lValue;
            break;

        case eO97_TYPE_OBJECT_ID:
            *lplpvData = &lpstParam->uData.dwObjectID;
            break;

        default:
            *lpeType = eO97_TYPE_UNKNOWN;
            break;
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ParamGetLong()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  nIndex          Index of parameter
//  lplValue        Ptr to variable to store long
//
// Description:
//  This function attempts to get the value of a parameter as a long
//  value.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ParamGetLong
(
    LPO97_ENV       lpstEnv,
    int             nIndex,
    LPLONG          lplValue
)
{
    LPO97_PARAM     lpstParam;

    assert(nIndex >= 0);

    if (nIndex >= lpstEnv->nNumParams)
        return(FALSE);

    lpstParam = lpstEnv->astParams + lpstEnv->nNumParams - nIndex - 1;

    if (lpstParam->eType == eO97_TYPE_VAR)
    {
        LPO97_VAR   lpstVar;

        lpstVar = (LPO97_VAR)lpstParam->uData.lpvstVar;
        if (lpstVar->eType == eO97_TYPE_VAR)
            lpstVar = (LPO97_VAR)lpstVar->uData.lpvstVar;

        if (lpstVar->eType != eO97_TYPE_LONG)
            return(FALSE);

        *lplValue = lpstVar->uData.lValue;
    }
    else
    if (lpstParam->eType != eO97_TYPE_LONG)
        return(FALSE);
    else
        *lplValue = lpstParam->uData.lValue;

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ParamGetString()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  nIndex          Index of parameter
//  lplpbyStr       Ptr to variable to store ptr to string
//
// Description:
//  This function attempts to get the value of a parameter as a string
//  value.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ParamGetString
(
    LPO97_ENV       lpstEnv,
    int             nIndex,
    LPLPBYTE        lplpbyStr
)
{
    LPO97_PARAM     lpstParam;

    assert(nIndex >= 0);

    if (nIndex >= lpstEnv->nNumParams)
        return(FALSE);

    lpstParam = lpstEnv->astParams + lpstEnv->nNumParams - nIndex - 1;

    if (lpstParam->eType == eO97_TYPE_VAR)
    {
        LPO97_VAR   lpstVar;

        lpstVar = (LPO97_VAR)lpstParam->uData.lpvstVar;
        if (lpstVar->eType == eO97_TYPE_VAR)
            lpstVar = (LPO97_VAR)lpstVar->uData.lpvstVar;

        if (lpstVar->eType != eO97_TYPE_STRING)
            return(FALSE);

        *lplpbyStr = lpstVar->uData.lpbyStr;
    }
    else
    if (lpstParam->eType != eO97_TYPE_STRING)
        return(FALSE);
    else
        *lplpbyStr = lpstParam->uData.lpbyStr;

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ParamGetParams()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function transfers parameters from the evaluation stack
//  to the parameter stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ParamGetParams
(
    LPO97_ENV       lpstEnv
)
{
    WORD            w;
    WORD            wNamedParamID;
    EO97_TYPE_T     eType;
    LPVOID          lpvData;

    // Initialize the list

    if (O97ParamInitList(lpstEnv) == FALSE)
        return(FALSE);

    // Push the parameters onto the parameter stack

    for (w=0;w<lpstEnv->wOpCount;w++)
    {
        if (O97EvalQueryTopOperand(lpstEnv,
                                   &eType,
                                   &lpvData) == FALSE)
            return(FALSE);

        if (eType == eO97_TYPE_PARAM_ASSIGN)
        {
            wNamedParamID = *(LPWORD)lpvData;

            if (O97EvalPopTopOperand(lpstEnv) == FALSE)
                return(FALSE);

            if (O97EvalQueryTopOperand(lpstEnv,
                                       &eType,
                                       &lpvData) == FALSE)
                return(FALSE);
        }
        else
            wNamedParamID = O97_UNNAMED_PARAM_ID;

        if (O97ParamPut(lpstEnv,
                        wNamedParamID,
                        eType,
                        lpvData) == FALSE)
            return(FALSE);

        if (O97EvalPopTopOperand(lpstEnv) == FALSE)
            return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ParamInvalidateFirst()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function eliminates the first parameter so that the second
//  parameter becomes the first, the third becomes the second, and
//  so on.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ParamInvalidateFirst
(
    LPO97_ENV       lpstEnv
)
{
    if (lpstEnv->nNumParams < 1)
        return(FALSE);

    lpstEnv->nNumParams--;

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ParamPopParams()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function pops the parameters off of the evaluation stack
//  without first saving them on the parameter stack.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97ParamPopParams
(
    LPO97_ENV       lpstEnv
)
{
    WORD            w;
    EO97_TYPE_T     eType;
    LPVOID          lpvData;

    // Pop the parameters off of the evaluation stack

    for (w=0;w<lpstEnv->wOpCount;w++)
    {
        if (O97EvalQueryTopOperand(lpstEnv,
                                   &eType,
                                   &lpvData) == FALSE)
            return(FALSE);

        if (eType == eO97_TYPE_PARAM_ASSIGN)
        {
            if (O97EvalPopTopOperand(lpstEnv) == FALSE)
                return(FALSE);
        }

        if (O97EvalPopTopOperand(lpstEnv) == FALSE)
            return(FALSE);
    }

    return(TRUE);
}



