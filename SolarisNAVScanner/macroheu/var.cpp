//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/VAR.CPv   1.1   15 Jul 1997 17:04:30   DCHI  $
//
// Description:
//  Macro emulation environment variable management functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/VAR.CPv  $
// 
//    Rev 1.1   15 Jul 1997 17:04:30   DCHI
// Changed dialog structure field sSize to wSize.
// 
//    Rev 1.0   30 Jun 1997 16:15:46   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "wd7api.h"
#include "wd7env.h"

//*************************************************************************
//
// BOOL VarCreate()
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

BOOL VarCreate
(
    LPENV       lpstEnv
)
{
    // Allocate variable name storage

    lpstEnv->stGlobalVar.nVarNameStoreSize = 2048;
    lpstEnv->stLocalVar.nVarNameStoreSize = 2048;

    if (SSMemoryAlloc(lpstEnv->lpvRootCookie,
                      sizeof(BYTE) *
                       (lpstEnv->stGlobalVar.nVarNameStoreSize +
                        lpstEnv->stLocalVar.nVarNameStoreSize),
                      (LPLPVOID)&lpstEnv->stGlobalVar.
                        lpbyVarNameStore) != SS_STATUS_OK)
        return(FALSE);

    if (lpstEnv->stGlobalVar.lpbyVarNameStore == NULL)
        return(FALSE);

    lpstEnv->stLocalVar.lpbyVarNameStore =
        lpstEnv->stGlobalVar.lpbyVarNameStore +
        lpstEnv->stGlobalVar.nVarNameStoreSize;

    // Allocate variable structure storage

    lpstEnv->stGlobalVar.nMaxNumVars = 64;
    lpstEnv->stLocalVar.nMaxNumVars = 64;

    if (SSMemoryAlloc(lpstEnv->lpvRootCookie,
                      sizeof(VAR_T) *
                      (lpstEnv->stGlobalVar.nMaxNumVars +
                       lpstEnv->stGlobalVar.nMaxNumVars),
                      (LPLPVOID)&lpstEnv->stGlobalVar.
                        lpastVars) != SS_STATUS_OK)
    {
        SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->stGlobalVar.lpbyVarNameStore);
        return(FALSE);
    }

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
        SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->stGlobalVar.lpbyVarNameStore);
        return(FALSE);
    }

    // Create variable data store

    lpstEnv->dwGSMVarSize = 4096;
    lpstEnv->lpstGSMVarData =
        GSMCreate(lpstEnv->lpvRootCookie,
                  lpstEnv->dwGSMVarSize);

    if (lpstEnv->lpstGSMVarData == NULL)
    {
        SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpanVarFrames);
        SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->stGlobalVar.lpastVars);
        SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->stGlobalVar.lpbyVarNameStore);
        return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL VarDestroy()
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

BOOL VarDestroy
(
    LPENV       lpstEnv
)
{
    BOOL        bResult = TRUE;

    if (GSMDestroy(lpstEnv->lpvRootCookie,
                   lpstEnv->lpstGSMVarData) == FALSE)
        bResult = FALSE;

    if (SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->lpanVarFrames) != SS_STATUS_OK)
        bResult = FALSE;

    if (SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->stGlobalVar.lpastVars) != SS_STATUS_OK)
        bResult = FALSE;

    if (SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstEnv->stGlobalVar.lpbyVarNameStore) != SS_STATUS_OK)
        bResult = FALSE;

    return(bResult);
}


//*************************************************************************
//
// BOOL VarInit()
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

BOOL VarInit
(
    LPENV       lpstEnv
)
{
    lpstEnv->stGlobalVar.nVarNameStoreUsed = 0;
    lpstEnv->stLocalVar.nVarNameStoreUsed = 0;
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
// LPVAR VarAlloc()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  bGlobal         TRUE for global variable, FALSE for local variable
//  bArray          TRUE for array variable, FALSE for not
//  byNameLen       Length of the variable's name
//  lpbyName        Ptr to the bytes of the name
//
// Description:
//  This function allocates the resources for a new variable
//
// Returns:
//  LPVAR           On success, ptr to the allocated variable structure
//  NULL            On failure
//
//*************************************************************************

LPVAR VarAlloc
(
    LPENV       lpstEnv,
    BOOL        bGlobal,
    BOOL        bArray,
    BYTE        byNameLen,
    LPBYTE      lpbyName
)
{
    int         i;
    LPVAR       lpstVar;
    LPVAR_ENV   lpstVarEnv;

    if (bGlobal == TRUE)
        lpstVarEnv = &lpstEnv->stGlobalVar;
    else
        lpstVarEnv = &lpstEnv->stLocalVar;

    ////////////////////////////////////////////////////////
    // Find a free variable structure
    ////////////////////////////////////////////////////////

    if (lpstVarEnv->nNumVars >= lpstVarEnv->nMaxNumVars)
        return(NULL);

    lpstVar = lpstVarEnv->lpastVars + lpstVarEnv->nNumVars++;


    ////////////////////////////////////////////////////////
    // Allocate memory for the name
    ////////////////////////////////////////////////////////

    if (lpstVarEnv->nVarNameStoreUsed + byNameLen >
        lpstVarEnv->nVarNameStoreSize)
    {
        // Not enough room for the name

        lpstVarEnv->nNumVars--;
        return(NULL);
    }

    // Copy the name, lowercasing it

    lpstVar->byNameLen = byNameLen;
    lpstVar->lpbyName = lpstVarEnv->lpbyVarNameStore +
        lpstVarEnv->nVarNameStoreUsed;

    lpstVarEnv->nVarNameStoreUsed += byNameLen;

    for (i=0;i<byNameLen;i++)
    {
        if (lpbyName[i] <= 'Z' && lpbyName[i] >= 'A')
            lpstVar->lpbyName[i] = lpbyName[i] + 'a' - 'A';
        else
            lpstVar->lpbyName[i] = lpbyName[i];
    }

    // Set the type to unknown

    if (bArray == FALSE)
        lpstVar->eType = eTYPE_UNKNOWN;
    else
    {
        lpstVar->eType = eTYPE_ARRAY;
        lpstVar->uData.lpvstElement = NULL;
    }

    return(lpstVar);
}


//*************************************************************************
//
// LPVAR VarGet()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  byNameLen       Length of the variable's name
//  lpbyName        Ptr to the bytes of the name
//
// Description:
//  This function searches for a regular variable, first in the local
//  section, then the global section.
//
// Returns:
//  LPVAR           On success, ptr to the found variable structure
//  NULL            On failure or not found
//
//*************************************************************************

LPVAR VarGet
(
    LPENV       lpstEnv,
    BYTE        byNameLen,
    LPBYTE      lpbyName
)
{
    int         n, i, j;
    int         nStart, nNumVars;
    LPVAR       lpstVar;

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
            if (lpstVar->eType != eTYPE_ARRAY &&
                lpstVar->byNameLen == byNameLen)
            {
                for (j=0;j<byNameLen;j++)
                {
                    if (lpbyName[j] <= 'Z' && lpbyName[j] >= 'A')
                    {
                        if ((lpbyName[j] + 'a' - 'A') !=
                            lpstVar->lpbyName[j])
                            break;
                    }
                    else
                    {
                        if (lpbyName[j] != lpstVar->lpbyName[j])
                            break;
                    }
                }

                if (j == byNameLen)
                {
                    // Found a match

                    return(lpstVar);
                }
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
// LPVAR VarArrayGet()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//  byNameLen       Length of the variable's name
//  lpbyName        Ptr to the bytes of the name
//
// Description:
//  This function searches for an array variable, first in the local
//  section, then the global section.
//
// Returns:
//  LPVAR           On success, ptr to the found variable structure
//  NULL            On failure or not found
//
//*************************************************************************

LPVAR VarArrayGet
(
    LPENV       lpstEnv,
    BYTE        byNameLen,
    LPBYTE      lpbyName
)
{
    int         n, i, j;
    int         nStart, nNumVars;
    LPVAR       lpstVar;

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
            if (lpstVar->eType == eTYPE_ARRAY &&
                lpstVar->byNameLen == byNameLen)
            {
                for (j=0;j<byNameLen;j++)
                {
                    if (lpbyName[j] <= 'Z' && lpbyName[j] >= 'A')
                    {
                        if ((lpbyName[j] + 'a' - 'A') !=
                            lpstVar->lpbyName[j])
                            break;
                    }
                    else
                    {
                        if (lpbyName[j] != lpstVar->lpbyName[j])
                            break;
                    }
                }

                if (j == byNameLen)
                {
                    // Found a match

                    return(lpstVar);
                }
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
// LPELEMENT VarArrayGetElement()
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
//  LPELEMENT       On success, ptr to the found element structure
//  NULL            On failure or not found
//
//*************************************************************************

LPELEMENT VarArrayGetElement
(
    LPVAR       lpstVar,
    WORD        wIndex0,
    WORD        wIndex1
)
{
    LPELEMENT   lpstElement;
    DWORD       dwIndex;

    dwIndex = (DWORD)wIndex0 + ((DWORD)wIndex1 << 16);

    lpstElement = (LPELEMENT)lpstVar->uData.lpvstElement;
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
// LPELEMENT VarArrayElementAlloc()
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
//  LPELEMENT       On success, ptr to the allocated element structure
//  NULL            On failure or not found
//
//*************************************************************************

LPELEMENT VarArrayElementAlloc
(
    LPENV       lpstEnv,
    LPVAR       lpstVar,
    WORD        wIndex0,
    WORD        wIndex1
)
{
    LPELEMENT   lpstElement;
    DWORD       dwIndex;

    dwIndex = (DWORD)wIndex0 + ((DWORD)wIndex1 << 16);

    lpstElement = (LPELEMENT)GSMAlloc(lpstEnv->lpstGSMVarData,
                                      sizeof(ELEMENT_T));

    if (lpstElement == NULL)
        return(NULL);

    lpstElement->eType = eTYPE_UNKNOWN;
    lpstElement->dwIndex = dwIndex;
    lpstElement->lpstNext = (LPELEMENT)lpstVar->uData.lpvstElement;
    lpstVar->uData.lpvstElement = lpstElement;

    return(lpstElement);
}


//*************************************************************************
//
// BOOL VarFree()
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

BOOL VarFree
(
    LPENV       lpstEnv,
    LPVAR       lpstVar
)
{
    switch (lpstVar->eType)
    {
        case eTYPE_STRING:
        case eTYPE_DIALOG:
            if (GSMFree(lpstEnv->lpstGSMVarData,
                        lpstVar->uData.lpbyStr) == FALSE)
            {
                // This is fatal

                return(FALSE);
            }
            break;

        case eTYPE_ARRAY:
        {
            LPELEMENT lpstElement, lpstNext;

            // Iterate through the elements and free them

            lpstElement = (LPELEMENT)lpstVar->uData.lpvstElement;
            while (lpstElement != NULL)
            {
                lpstNext = lpstElement->lpstNext;

                if (lpstVar->eType == eTYPE_STRING)
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
// LPVAR VarSet()
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
//  LPVAR           On success, ptr to the variable structure
//  NULL            On failure
//
//*************************************************************************

LPVAR VarSet
(
    LPENV       lpstEnv,
    LPVAR       lpstVar,
    ETYPE_T     eType,
    LPVOID      lpvData
)
{
    int         i;

    assert(lpstVar != NULL);

    // If it is a variable reference then it should be a direct
    //  reference to the base variable

    if (lpstVar->eType == eTYPE_VAR)
    {
        lpstVar = (LPVAR)lpstVar->uData.lpvstVar;

        assert(lpstVar->eType != eTYPE_VAR);
    }

    // Free the current value usage if any

    if (VarFree(lpstEnv,
                lpstVar) == FALSE)
        return(NULL);

    /////////////////////////////////////////////////////////////
    // Now see what kind of data needs to be stored and store it
    /////////////////////////////////////////////////////////////

    switch (eType)
    {
        case eTYPE_LONG:
            lpstVar->eType = eTYPE_LONG;
            lpstVar->uData.lValue = *(LPLONG)lpvData;
            break;

        case eTYPE_VAR:
            lpstVar->eType = eTYPE_VAR;
            lpstVar->uData.lpvstVar = lpvData;
            break;

        case eTYPE_STRING:
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
                lpstVar->eType = eTYPE_UNKNOWN;
                return(NULL);
            }

            lpstVar->eType = eTYPE_STRING;

            // Copy the string

            lpbySrcStr = (LPBYTE)lpvData;
            lpbyDstStr = lpstVar->uData.lpbyStr;
            nLen = *lpbySrcStr;
            for (i=0;i<=nLen;i++)
                *lpbyDstStr++ = *lpbySrcStr++;
            break;
        }

        case eTYPE_DIALOG:
            // Allocate memory for the dialog structure

            lpstVar->uData.lpvstDlg =
                GSMAlloc(lpstEnv->lpstGSMVarData,
                         ((LPDIALOG)lpvData)->wSize);

            if (lpstVar->uData.lpvstDlg == NULL)
            {
                lpstVar->eType = eTYPE_UNKNOWN;
                return(NULL);
            }

            lpstVar->eType = eTYPE_DIALOG;
            ((LPDIALOG)lpstVar->uData.lpvstDlg)->wSize =
                ((LPDIALOG)lpvData)->wSize;
            ((LPDIALOG)lpstVar->uData.lpvstDlg)->eWDCMD =
                ((LPDIALOG)lpvData)->eWDCMD;
            break;

        default:
            // This is fatal

            assert(0);
            lpstVar->eType = eTYPE_UNKNOWN;
            break;
    }

    return(lpstVar);
}


//*************************************************************************
//
// LPVAR VarGetFirstFrameVar()
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

LPVAR VarGetFirstFrameVar
(
    LPENV       lpstEnv,
    LPETYPE     lpeType,
    LPLPVOID    lplpvData
)
{
    LPVAR       lpstVar;

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
            case eTYPE_LONG:
                *lplpvData = &lpstVar->uData.lValue;
                break;

            default:
                *lplpvData = lpstVar->uData.lpbyStr;
                break;
        }
    }

    return(lpstVar);
}


//*************************************************************************
//
// BOOL VarPushFrame()
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

BOOL VarPushFrame
(
    LPENV       lpstEnv
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
// BOOL VarPopFrame()
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

BOOL VarPopFrame
(
    LPENV       lpstEnv
)
{
    int         i;
    LPVAR       lpstVar;

    assert(lpstEnv->nNumVarFrames > 0);

    // Readjust frame count

    lpstEnv->nNumVarFrames--;

    i = lpstEnv->lpanVarFrames[lpstEnv->nNumVarFrames];

    // Deallocate memory of variables in frame

    lpstVar = lpstEnv->stLocalVar.lpastVars + i;
    while (i < lpstEnv->stLocalVar.nNumVars)
    {
        if (VarFree(lpstEnv,
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
// BOOL VarPushGlobalFrame()
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

BOOL VarPushGlobalFrame
(
    LPENV       lpstEnv
)
{
    lpstEnv->lpstModEnv->nGlobalVarFrameStart =
        lpstEnv->stGlobalVar.nNumVars;

    return(TRUE);
}


//*************************************************************************
//
// BOOL VarPopGlobalFrame()
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

BOOL VarPopGlobalFrame
(
    LPENV       lpstEnv
)
{
    int         i;
    LPVAR       lpstVar;

    i = lpstEnv->lpstModEnv->nGlobalVarFrameStart;

    // Deallocate memory of variables in frame

    lpstVar = lpstEnv->stGlobalVar.lpastVars + i;
    while (i < lpstEnv->stGlobalVar.nNumVars)
    {
        if (VarFree(lpstEnv,
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


