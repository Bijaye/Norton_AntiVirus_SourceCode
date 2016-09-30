//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/o97var.cpv   1.1   13 Oct 1998 11:30:54   DCHI  $
//
// Description:
//  Handling of variables.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/o97var.cpv  $
// 
//    Rev 1.1   13 Oct 1998 11:30:54   DCHI
// Changed to return NULL in O97VarSet() if type is not VAR.
// 
//    Rev 1.0   15 Aug 1997 13:29:38   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"

//*************************************************************************
//
// BOOL O97VarCreate()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function allocates memory for storing variables.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97VarCreate
(
    LPO97_ENV       lpstEnv
)
{
    // Allocate variable structure storage

    lpstEnv->stGlobalVar.nMaxNumVars = 64;
    lpstEnv->stLocalVar.nMaxNumVars = 64;

    if (SSMemoryAlloc(lpstEnv->lpvRootCookie,
                      sizeof(O97_VAR_T) *
                      (lpstEnv->stGlobalVar.nMaxNumVars +
                       lpstEnv->stGlobalVar.nMaxNumVars),
                      (LPLPVOID)&lpstEnv->stGlobalVar.
                        lpastVars) != SS_STATUS_OK)
        return(FALSE);

    lpstEnv->stLocalVar.lpastVars =
        lpstEnv->stGlobalVar.lpastVars + lpstEnv->stGlobalVar.nMaxNumVars;

    // Allocate frame memory

    lpstEnv->nMaxVarFrames = 8;
    if (SSMemoryAlloc(lpstEnv->lpvRootCookie,
                      lpstEnv->nMaxVarFrames * sizeof(int),
                      (LPLPVOID)&lpstEnv->lpanVarFrames) != SS_STATUS_OK)
    {
        SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->stGlobalVar.lpastVars);
        return(FALSE);
    }

    // Create variable data store

    lpstEnv->dwGSMVarSize = 4096;
    lpstEnv->lpstGSMVarData = GSMCreate(lpstEnv->lpvRootCookie,
                                        lpstEnv->dwGSMVarSize);

    if (lpstEnv->lpstGSMVarData == NULL)
    {
        SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpanVarFrames);
        SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->stGlobalVar.lpastVars);
        return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97VarDestroy()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function frees memory allocated for storing variables.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97VarDestroy
(
    LPO97_ENV       lpstEnv
)
{
    GSMDestroy(lpstEnv->lpvRootCookie,
               lpstEnv->lpstGSMVarData);

    SSMemoryFree(lpstEnv->lpvRootCookie,
                 lpstEnv->lpanVarFrames);
    SSMemoryFree(lpstEnv->lpvRootCookie,
                 lpstEnv->stGlobalVar.lpastVars);

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97VarInit()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function initializes memory allocated for storing variables.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97VarInit
(
    LPO97_ENV       lpstEnv
)
{
    lpstEnv->stGlobalVar.nNumVars = 0;
    lpstEnv->stLocalVar.nNumVars = 0;
    lpstEnv->nNumVarFrames = 1;
    lpstEnv->lpanVarFrames[0] = 0;

    GSMInit(lpstEnv->lpstGSMVarData,
            lpstEnv->dwGSMVarSize);

    return(TRUE);
}


//*************************************************************************
//
// LPO97_VAR O97VarAlloc()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  bGlobal         TRUE for global variable, FALSE for local variable
//  bArray          TRUE for array variable, FALSE for not
//  wID             ID of the variable
//
// Description:
//  This function allocates the resources for a new variable
//
// Returns:
//  LPO97_VAR       On success, ptr to the allocated variable structure
//  NULL            On failure
//
//*************************************************************************

LPO97_VAR O97VarAlloc
(
    LPO97_ENV       lpstEnv,
    BOOL            bGlobal,
    BOOL            bArray,
    WORD            wID
)
{
    LPO97_VAR       lpstVar;
    LPO97_VAR_ENV   lpstVarEnv;

    if (bGlobal == TRUE)
        lpstVarEnv = &lpstEnv->stGlobalVar;
    else
        lpstVarEnv = &lpstEnv->stLocalVar;

    // Find a free variable structure

    if (lpstVarEnv->nNumVars >= lpstVarEnv->nMaxNumVars)
        return(NULL);

    lpstVar = lpstVarEnv->lpastVars + lpstVarEnv->nNumVars++;

    // Set the type to unknown

    if (bArray == FALSE)
        lpstVar->eType = eO97_TYPE_UNKNOWN;
    else
    {
        lpstVar->eType = eO97_TYPE_ARRAY;
        lpstVar->uData.lpvstElement = NULL;
    }

    // Copy the ID

    lpstVar->wID = wID;

    return(lpstVar);
}


//*************************************************************************
//
// LPO97_VAR O97VarGet()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  wID             ID of the identifier
//
// Description:
//  This function searches for a regular variable, first in the local
//  section, then the global section.
//
// Returns:
//  LPO97_VAR       On success, ptr to the found variable structure
//  NULL            On failure or not found
//
//*************************************************************************

LPO97_VAR O97VarGet
(
    LPO97_ENV       lpstEnv,
    WORD            wID
)
{
    int             n, i;
    int             nStart, nNumVars;
    LPO97_VAR       lpstVar;

    // Start

    assert(lpstEnv->nNumVarFrames > 0);

    // Check local frame first

    nStart = lpstEnv->lpanVarFrames[lpstEnv->nNumVarFrames - 1];
    lpstVar = lpstEnv->stLocalVar.lpastVars + nStart;
    nNumVars = lpstEnv->stLocalVar.nNumVars;

    for (n=0;n<2;n++)
    {
        for (i=nStart;i<nNumVars;i++)
        {
            if (lpstVar->eType != eO97_TYPE_ARRAY &&
                lpstVar->wID == wID)
            {
                // Found a match

                return(lpstVar);
            }

            ++lpstVar;
        }

        // Now try global frame

        nStart = lpstEnv->lpstModEnv->nGlobalVarFrameStart;
        lpstVar = lpstEnv->stGlobalVar.lpastVars + nStart;
        nNumVars = lpstEnv->stGlobalVar.nNumVars;
    }

    // No such variable defined

    return(NULL);
}


//*************************************************************************
//
// LPO97_VAR O97VarArrayGet()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  wID             ID of the identifier
//
// Description:
//  This function searches for an array variable, first in the local
//  section, then the global section.
//
// Returns:
//  LPO97_VAR       On success, ptr to the found variable structure
//  NULL            On failure or not found
//
//*************************************************************************

LPO97_VAR O97VarArrayGet
(
    LPO97_ENV       lpstEnv,
    WORD            wID
)
{
    int             n, i;
    int             nStart, nNumVars;
    LPO97_VAR       lpstVar;

    // Start

    assert(lpstEnv->nNumVarFrames > 0);

    // Check local frame first

    nStart = lpstEnv->lpanVarFrames[lpstEnv->nNumVarFrames - 1];
    lpstVar = lpstEnv->stLocalVar.lpastVars + nStart;
    nNumVars = lpstEnv->stLocalVar.nNumVars;

    for (n=0;n<2;n++)
    {
        for (i=nStart;i<nNumVars;i++)
        {
            if (lpstVar->eType == eO97_TYPE_ARRAY &&
                lpstVar->wID == wID)
            {
                // Found a match

                return(lpstVar);
            }

            ++lpstVar;
        }

        // Now try global frame

        nStart = lpstEnv->lpstModEnv->nGlobalVarFrameStart;
        lpstVar = lpstEnv->stGlobalVar.lpastVars + nStart;
        nNumVars = lpstEnv->stGlobalVar.nNumVars;
    }

    // No such variable defined

    return(NULL);
}


//*************************************************************************
//
// LPO97_ELEMENT O97VarArrayGetElement()
//
// Parameters:
//  lpstVar         Ptr to the array variable
//  wIndex0         First dimension index
//  wIndex1         Second dimension index
//
// Description:
//  This function searches for an element of an array variable.
//
// Returns:
//  LPO97_ELEMENT   On success, ptr to the found element structure
//  NULL            On failure or not found
//
//*************************************************************************

LPO97_ELEMENT O97VarArrayGetElement
(
    LPO97_VAR       lpstVar,
    WORD            wIndex0,
    WORD            wIndex1
)
{
    LPO97_ELEMENT   lpstElement;
    DWORD           dwIndex;

    dwIndex = (DWORD)wIndex0 + ((DWORD)wIndex1 << 16);

    lpstElement = (LPO97_ELEMENT)lpstVar->uData.lpvstElement;
    while (lpstElement != NULL)
    {
        if (lpstElement->dwIndex == dwIndex)
            return(lpstElement);

        lpstElement = lpstElement->lpstNext;
    }

    return(NULL);
}


//*************************************************************************
//
// LPO97_ELEMENT O97VarArrayElementAlloc()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpstVar         Ptr to the array variable
//  wIndex0         First dimension index
//  wIndex1         Second dimension index
//
// Description:
//  This function allocates memory for an element of an array variable.
//
// Returns:
//  LPO97_ELEMENT   On success, ptr to the allocated element structure
//  NULL            On failure or not found
//
//*************************************************************************

LPO97_ELEMENT O97VarArrayElementAlloc
(
    LPO97_ENV       lpstEnv,
    LPO97_VAR       lpstVar,
    WORD            wIndex0,
    WORD            wIndex1
)
{
    LPO97_ELEMENT   lpstElement;
    DWORD           dwIndex;

    dwIndex = (DWORD)wIndex0 + ((DWORD)wIndex1 << 16);

    lpstElement = (LPO97_ELEMENT)GSMAlloc(lpstEnv->lpstGSMVarData,
                                          sizeof(O97_ELEMENT_T));

    if (lpstElement == NULL)
        return(NULL);

    lpstElement->eType = eO97_TYPE_UNKNOWN;
    lpstElement->dwIndex = dwIndex;
    lpstElement->lpstNext = (LPO97_ELEMENT)lpstVar->uData.lpvstElement;
    lpstVar->uData.lpvstElement = lpstElement;

    return(lpstElement);
}


//*************************************************************************
//
// BOOL O97VarFree()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpstVar         Ptr to the array variable
//
// Description:
//  This function frees memory allocated for a variable.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97VarFree
(
    LPO97_ENV       lpstEnv,
    LPO97_VAR       lpstVar
)
{
    switch (lpstVar->eType)
    {
        case eO97_TYPE_STRING:
        case eO97_TYPE_DIALOG:
            if (GSMFree(lpstEnv->lpstGSMVarData,
                        lpstVar->uData.lpbyStr) == FALSE)
            {
                // This is fatal

                return(FALSE);
            }
            break;

        case eO97_TYPE_ARRAY:
        {
            LPO97_ELEMENT lpstElement, lpstNext;

            // Iterate through the elements and free them

            lpstElement = (LPO97_ELEMENT)lpstVar->uData.lpvstElement;
            while (lpstElement != NULL)
            {
                lpstNext = lpstElement->lpstNext;

                if (lpstVar->eType == eO97_TYPE_STRING)
                {
                    if (GSMFree(lpstEnv->lpstGSMVarData,
                                lpstElement->uData.lpbyStr) == FALSE)
                    {
                        // This is fatal

                        return(FALSE);
                    }
                }

                if (GSMFree(lpstEnv->lpstGSMVarData,
                            lpstElement) == FALSE)
                {
                    // This is fatal

                    return(FALSE);
                }

                lpstElement = lpstNext;
            }
            break;
        }

        default:
            break;
    }

    return(TRUE);
}


//*************************************************************************
//
// void O97VarGetTypeData()
//
// Parameters:
//  lpstVar         Ptr to the variable
//  eType           Ptr for data type
//  lpvData         Ptr for data ptr
//
// Description:
//  This function gets the value of a variable as a type and
//  a ptr to the data.
//
// Returns:
//  Nothing
//
//*************************************************************************

void O97VarGetTypeData
(
    LPO97_VAR       lpstVar,
    LPEO97_TYPE     lpeType,
    LPVOID FAR *    lplpvData
)
{
    // If it is a variable reference then it should be a direct
    //  reference to the base variable

    if (lpstVar != NULL && lpstVar->eType == eO97_TYPE_VAR)
    {
        lpstVar = (LPO97_VAR)lpstVar->uData.lpvstVar;

        assert(lpstVar->eType != eO97_TYPE_VAR);
    }

    if (lpstVar == NULL)
    {
        *lpeType = eO97_TYPE_UNKNOWN;
        *lplpvData = NULL;
        return;
    }

    // Now see what kind of data it is

    switch (*lpeType = lpstVar->eType)
    {
        case eO97_TYPE_LONG:
            *lplpvData = &lpstVar->uData.lValue;
            break;

        case eO97_TYPE_STRING:
            *lplpvData = lpstVar->uData.lpbyStr;
            break;

        case eO97_TYPE_DIALOG:
            *lplpvData = lpstVar->uData.lpvstDlg;
            break;

        case eO97_TYPE_OBJECT_ID:
            *lplpvData = &lpstVar->uData.dwObjectID;
            break;

        default:
            *lpeType = eO97_TYPE_UNKNOWN;
            *lplpvData = NULL;
            break;
    }
}


//*************************************************************************
//
// LPO97_VAR O97VarSet()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpstVar         Ptr to the array variable
//  eType           Data type
//  lpvData         Ptr to data
//
// Description:
//  This function sets the value of a variable.
//
// Returns:
//  LPO97_VAR       On success, ptr to the variable structure
//  NULL            On failure
//
//*************************************************************************

LPO97_VAR O97VarSet
(
    LPO97_ENV       lpstEnv,
    LPO97_VAR       lpstVar,
    EO97_TYPE_T     eType,
    LPVOID          lpvData
)
{
    int         i;

    assert(lpstVar != NULL);

    // If it is a variable reference then it should be a direct
    //  reference to the base variable

    if (lpstVar->eType == eO97_TYPE_VAR)
    {
        lpstVar = (LPO97_VAR)lpstVar->uData.lpvstVar;
        return(NULL);
//        assert(lpstVar->eType != eO97_TYPE_VAR);
    }

    // Free the current value usage if any

    if (O97VarFree(lpstEnv,
                   lpstVar) == FALSE)
        return(NULL);

    // If it is a variable ID, then get the data

    if (eType == eO97_TYPE_VAR_ID)
    {
        LPO97_VAR   lpstTempVar;

        lpstTempVar = O97VarGet(lpstEnv,*(LPWORD)lpvData);
        if (lpstTempVar != NULL)
            O97VarGetTypeData(lpstTempVar,&eType,&lpvData);
    }

    /////////////////////////////////////////////////////////////
    // Now see what kind of data needs to be stored and store it
    /////////////////////////////////////////////////////////////

    switch (eType)
    {
        case eO97_TYPE_LONG:
            lpstVar->eType = eO97_TYPE_LONG;
            lpstVar->uData.lValue = *(LPLONG)lpvData;
            break;

        case eO97_TYPE_VAR:
            lpstVar->eType = eO97_TYPE_VAR;
            lpstVar->uData.lpvstVar = lpvData;
            break;

        case eO97_TYPE_STRING:
        {
            LPBYTE      lpbySrcStr, lpbyDstStr;
            int         nLen;

            // Allocate memory for the string

            // The first byte of the string always indicates the
            //  length of the string

            lpstVar->uData.lpbyStr = (LPBYTE)
                GSMAlloc(lpstEnv->lpstGSMVarData,
                         *(LPBYTE)lpvData + 1);

            if (lpstVar->uData.lpbyStr == NULL)
            {
                lpstVar->eType = eO97_TYPE_UNKNOWN;
                return(NULL);
            }

            lpstVar->eType = eO97_TYPE_STRING;

            // Copy the string

            lpbySrcStr = (LPBYTE)lpvData;
            lpbyDstStr = lpstVar->uData.lpbyStr;
            nLen = *lpbySrcStr;
            for (i=0;i<=nLen;i++)
                *lpbyDstStr++ = *lpbySrcStr++;
            break;
        }

        case eO97_TYPE_DIALOG:
            // Allocate memory for the dialog structure

            lpstVar->uData.lpvstDlg =
                GSMAlloc(lpstEnv->lpstGSMVarData,
                         ((LPO97_DIALOG)lpvData)->dwSize);

            if (lpstVar->uData.lpvstDlg == NULL)
            {
                lpstVar->eType = eO97_TYPE_UNKNOWN;
                return(NULL);
            }

            lpstVar->eType = eO97_TYPE_DIALOG;
            ((LPO97_DIALOG)lpstVar->uData.lpvstDlg)->dwSize =
                ((LPO97_DIALOG)lpvData)->dwSize;
            ((LPO97_DIALOG)lpstVar->uData.lpvstDlg)->dwDlgID =
                ((LPO97_DIALOG)lpvData)->dwDlgID;
            break;

        case eO97_TYPE_OBJECT_ID:
            lpstVar->eType = eO97_TYPE_OBJECT_ID;
            lpstVar->uData.dwObjectID = *(LPDWORD)lpvData;
            break;

        default:
            // This is fatal

            lpstVar->eType = eO97_TYPE_UNKNOWN;
            break;
    }

    return(lpstVar);
}


//*************************************************************************
//
// LPO97_VAR O97VarGetFirstFrameVar()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  lpeType         Ptr to variable for data type
//  lplpvData       Ptr to variable to store ptr to data
//
// Description:
//  This function gets the first variable of the local frame.  It is
//  used for getting the return value of a function.
//
// Returns:
//  LPVAR           On success, ptr to the variable structure
//  NULL            On failure
//
//*************************************************************************

LPO97_VAR O97VarGetFirstFrameVar
(
    LPO97_ENV       lpstEnv,
    LPEO97_TYPE     lpeType,
    LPLPVOID        lplpvData
)
{
    LPO97_VAR       lpstVar;

    assert(lpstEnv->stLocalVar.nNumVars >
           lpstEnv->lpanVarFrames[lpstEnv->nNumVarFrames-1]);

    lpstVar = lpstEnv->stLocalVar.lpastVars +
        lpstEnv->lpanVarFrames[lpstEnv->nNumVarFrames - 1];

    if (lpeType != NULL)
        *lpeType = lpstVar->eType;

    if (lplpvData != NULL)
    {
        switch (lpstVar->eType)
        {
            case eO97_TYPE_LONG:
                *lplpvData = &lpstVar->uData.lValue;
                break;

            case eO97_TYPE_STRING:
                *lplpvData = lpstVar->uData.lpbyStr;
                break;

            default:
                *lpeType = eO97_TYPE_UNKNOWN;
                break;
        }
    }

    return(lpstVar);
}


//*************************************************************************
//
// BOOL O97VarPushFrame()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function creates a local variable frame.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97VarPushFrame
(
    LPO97_ENV       lpstEnv
)
{
    if (lpstEnv->nNumVarFrames >= lpstEnv->nMaxVarFrames)
        return(FALSE);

    lpstEnv->lpanVarFrames[lpstEnv->nNumVarFrames++] =
        lpstEnv->stLocalVar.nNumVars;

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97VarPopFrame()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function destroys a local variable frame.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97VarPopFrame
(
    LPO97_ENV       lpstEnv
)
{
    int             i;
    LPO97_VAR       lpstVar;

    assert(lpstEnv->nNumVarFrames > 0);

    // Readjust frame count

    lpstEnv->nNumVarFrames--;

    i = lpstEnv->lpanVarFrames[lpstEnv->nNumVarFrames];

    // Deallocate memory of variables in frame

    lpstVar = lpstEnv->stLocalVar.lpastVars + i;
    while (i < lpstEnv->stLocalVar.nNumVars)
    {
        if (O97VarFree(lpstEnv,
                       lpstVar) == FALSE)
            return(FALSE);

        ++lpstVar;
        ++i;
    }

    // Reset variable count

    lpstEnv->stLocalVar.nNumVars =
        lpstEnv->lpanVarFrames[lpstEnv->nNumVarFrames];

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97VarPushGlobalFrame()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function creates a global variable frame.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97VarPushGlobalFrame
(
    LPO97_ENV       lpstEnv
)
{
    lpstEnv->lpstModEnv->nGlobalVarFrameStart =
        lpstEnv->stGlobalVar.nNumVars;

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97VarPopGlobalFrame()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  This function destroys a global variable frame.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL O97VarPopGlobalFrame
(
    LPO97_ENV       lpstEnv
)
{
    int             i;
    LPO97_VAR       lpstVar;

    i = lpstEnv->lpstModEnv->nGlobalVarFrameStart;

    // Deallocate memory of variables in frame

    lpstVar = lpstEnv->stGlobalVar.lpastVars + i;
    while (i < lpstEnv->stGlobalVar.nNumVars)
    {
        if (O97VarFree(lpstEnv,
                       lpstVar) == FALSE)
            return(FALSE);

        ++lpstVar;
        ++i;
    }

    // Reset variable count

    lpstEnv->stGlobalVar.nNumVars =
        lpstEnv->lpstModEnv->nGlobalVarFrameStart;

    return(TRUE);
}


