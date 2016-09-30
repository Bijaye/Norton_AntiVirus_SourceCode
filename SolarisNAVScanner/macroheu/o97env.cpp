//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/o97env.cpv   1.3   20 Nov 1998 17:24:30   DCHI  $
//
// Description:
//  Environment creation and initialization functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/o97env.cpv  $
// 
//    Rev 1.3   20 Nov 1998 17:24:30   DCHI
// Fixed O97EnvCreate() to release lpwKnownIdent memory upon failure.
// 
//    Rev 1.2   12 Oct 1998 13:24:20   DCHI
// - Added WithDepth field initialization in O97EnvExecuteInit().
// - Added big endian support in O97InitKnownIdent().
// 
//    Rev 1.1   19 Aug 1997 15:01:18   DCHI
// Added check for NULL parameters before initialization in O97IdentLookup().
// 
//    Rev 1.0   15 Aug 1997 13:29:28   DCHI
// Initial revision.
// 
//************************************************************************

#include "o97api.h"
#include "olestrnm.h"
#include "o97env.h"

//*************************************************************************
//
// Function:
//  LPO97_ENV O97EnvCreate()
//
// Parameters:
//  lpvRootCookie           Root cookie
//  nNumKnownIdents         Number of known identifiers
//
// Description:
//  This function allocates an environment structure with enough
//  space for nNumKnownIdents known identifiers mappings.
//
//  The function also creates the variable storage area, the evaluation
//  stack, the parameter passing structure, and the control stack.
//
// Returns:
//  LPO97_ENV               On success
//  NULL                    On error
//
//*************************************************************************

LPO97_ENV O97EnvCreate
(
    LPVOID          lpvRootCookie,
    int             nNumKnownIdents
)
{
    LPO97_ENV       lpstEnv;

    // Allocate environment structure

    if (SSMemoryAlloc(lpvRootCookie,
                      sizeof(O97_ENV_T),
                      (LPLPVOID)&lpstEnv) != SS_STATUS_OK)
        return(NULL);

    lpstEnv->lpvRootCookie = lpvRootCookie;

    // Allocate memory for known identifiers

    lpstEnv->nNumKnownIdents = nNumKnownIdents;
    if (SSMemoryAlloc(lpvRootCookie,
                      sizeof(WORD) * 2 * nNumKnownIdents,
                      (LPLPVOID)&lpstEnv->lpwKnownIdent) != SS_STATUS_OK)
    {
        SSMemoryFree(lpvRootCookie,lpstEnv);
        return(NULL);
    }

    lpstEnv->lpwIdentEnum = lpstEnv->lpwKnownIdent + nNumKnownIdents;

    // Allocate global variable storage

    if (O97VarCreate(lpstEnv) == FALSE)
    {
        SSMemoryFree(lpvRootCookie,lpstEnv->lpwKnownIdent);
        SSMemoryFree(lpvRootCookie,lpstEnv);
        return(NULL);
    }

    // Allocate expression evaluation stack

    if (O97EvalStackCreate(lpstEnv) == FALSE)
    {
        O97VarDestroy(lpstEnv);
        SSMemoryFree(lpvRootCookie,lpstEnv->lpwKnownIdent);
        SSMemoryFree(lpvRootCookie,lpstEnv);
        return(NULL);
    }

    // Allocate parameter passing stack

    if (O97ParamCreate(lpstEnv) == FALSE)
    {
        O97EvalStackDestroy(lpstEnv);
        O97VarDestroy(lpstEnv);
        SSMemoryFree(lpvRootCookie,lpstEnv->lpwKnownIdent);
        SSMemoryFree(lpvRootCookie,lpstEnv);
        return(NULL);
    }

    // Allocate control stack

    if (O97ControlCreate(lpstEnv) == FALSE)
    {
        O97ParamDestroy(lpstEnv);
        O97EvalStackDestroy(lpstEnv);
        O97VarDestroy(lpstEnv);
        SSMemoryFree(lpvRootCookie,lpstEnv->lpwKnownIdent);
        SSMemoryFree(lpvRootCookie,lpstEnv);
        return(NULL);
    }

    return(lpstEnv);
}


//*************************************************************************
//
// Function:
//  BOOL O97EnvDestroy()
//
// Parameters:
//  lpvRootCookie           Root cookie
//  lpstEnv                 Ptr to environment structure to destroy
//
// Description:
//  Deallocates memory for the given environment structure.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL O97EnvDestroy
(
    LPVOID          lpvRootCookie,
    LPO97_ENV       lpstEnv
)
{
    BOOL            bResult;

    if (O97ControlDestroy(lpstEnv) == FALSE)
        bResult = FALSE;

    if (O97ParamDestroy(lpstEnv) == FALSE)
        bResult = FALSE;

    if (O97EvalStackDestroy(lpstEnv) == FALSE)
        bResult = FALSE;

    if (O97VarDestroy(lpstEnv) == FALSE)
        bResult = FALSE;

    SSMemoryFree(lpvRootCookie,lpstEnv->lpwKnownIdent);
    SSMemoryFree(lpvRootCookie,lpstEnv);

    return(bResult);
}


//*************************************************************************
//
// Function:
//  BOOL O97EnvInit()
//
// Parameters:
//  lpstEnv                 Ptr to environment structure to initialize
//  lpstRoot                Ptr to the OLE root structure
//  dwVBAChildEntry         Entry number of VBA storage child
//  lpstObjectFunc          Ptr to application object handling structure
//  lplpabyKnownIdentSet    Ptr to known identifier set
//
// Description:
//  Initializes environment structure, counts number of modules,
//  and get known identifier IDs.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL O97EnvInit
(
    LPO97_ENV           lpstEnv,
    LPSS_ROOT           lpstRoot,
    DWORD               dwVBAChildEntry,
    LPO97_OBJECT_FUNC   lpstObjectFunc,
    LPLPBYTE            lplpabyKnownIdentSet
)
{
    int                 i;

    for (i=0;i<NUM_O97_STR;i++)
    {
        lpstEnv->awO97Ident[i] = 0;
        lpstEnv->awO97IdentEnum[i] = (WORD)i;
    }

    for (i=0;i<lpstEnv->nNumKnownIdents;i++)
    {
        lpstEnv->lpwKnownIdent[i] = 0;
        lpstEnv->lpwIdentEnum[i] = (WORD)i;
    }

    lpstEnv->lpstRoot = lpstRoot;
    lpstEnv->dwVBAChildEntry = dwVBAChildEntry;
    lpstEnv->lpstObjectFunc = lpstObjectFunc;

    // Count the number of modules

    if (O97GetModuleCount(lpstRoot,
                          dwVBAChildEntry,
                          &lpstEnv->dwNumModules) == FALSE)
        return(FALSE);

    // Get known identifier IDs

    if (O97InitKnownIdent(lpstEnv,
                          lplpabyKnownIdentSet) == FALSE)
        return(FALSE);

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL O97ExecuteInit()
//
// Parameters:
//  lpstEnv                 Ptr to environment structure to initialize
//
// Description:
//  Called to initialize the environment for a fresh entry.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL O97EnvExecuteInit
(
    LPO97_ENV       lpstEnv
)
{
    if (O97ControlInit(lpstEnv) == FALSE)
        return(FALSE);

    if (O97EvalStackInit(lpstEnv) == FALSE)
        return(FALSE);

    if (O97VarInit(lpstEnv) == FALSE)
        return(FALSE);

    lpstEnv->lpstModEnv = NULL;
    lpstEnv->wModuleDepth = 0;
    lpstEnv->eStateGoto = eO97_STATE_GOTO_NONE;
    lpstEnv->eStateExit = eO97_STATE_EXIT_NONE;
    lpstEnv->nWithDepth = 0;

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL O97InitKnownIdent()
//
// Parameters:
//  lpstEnv                 Ptr to initialized environment struct
//  lplpabyKnownIdentSet    Known identifier set
//
// Description:
//  This function searches enumerates through the identifier table
//  and notes the IDs of the known identifiers.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL O97InitKnownIdent
(
    LPO97_ENV       lpstEnv,
    LPLPBYTE        lplpabyKnownIdentSet
)
{
    LPSS_ENUM_SIBS  lpstSibs;
    DWORD           dwIdentTableOffset;
    DWORD           dwNumIdents;
    BYTE            abyName[256];
    DWORD           dw;
    DWORD           dwBytesRead;
    WORD            wID;
    int             i, n, nGap, j;
    LPBYTE          lpbyCand;
    BYTE            abyModuleNames[512];
    LPSS_STREAM     lpstStream;
    LPWORD          lpwKnownIdent;
    LPWORD          lpwIdentEnum;
    BOOL            bLittleEndian;

    // Create a stream structure

    if (SSAllocStreamStruct(lpstEnv->lpstRoot,
                            &lpstStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
        return(FALSE);

    /////////////////////////////////////////////////////////////
    // First get all the module names up to 512 bytes worth
    /////////////////////////////////////////////////////////////

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstStream->lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        SSFreeStreamStruct(lpstStream);
        return(FALSE);
    }

    SSInitEnumSibsStruct(lpstSibs,lpstEnv->dwVBAChildEntry);

    n = 0;
    lpstEnv->nNumModuleIdents = 0;
    lpwKnownIdent = lpstEnv->lpwKnownIdent;
    lpwIdentEnum = lpstEnv->lpwIdentEnum;
    while (lpstEnv->nNumModuleIdents < O97_ENV_MAX_MODULE_IDENTS &&
           SSEnumSiblingEntriesCB(lpstStream->lpstRoot,
                                  O97OpenVBA5ModuleCB,
                                  abyName,
                                  lpstSibs,
                                  lpstStream) == SS_STATUS_OK)
    {
        lpbyCand = abyModuleNames + n + 1;
        i = 0;
        for (i=0;abyName[i] && (n + i) < 511;i++)
        {
            // Lower case it and store it

            if (abyName[i] <= 'Z' && abyName[i] >= 'A')
                lpbyCand[i] = abyName[i] + ('a' - 'A');
            else
                lpbyCand[i] = abyName[i];
        }

        if ((n + i) >= 511 && abyName[i])
        {
            // Not enough room

            break;
        }

        // Store the length

        abyModuleNames[n] = i;

        n += i + 1;

        // Store the info

        lpstEnv->awModuleIdent[lpstEnv->nNumModuleIdents] = 0;
        lpstEnv->adwModuleEntry[lpstEnv->nNumModuleIdents] =
            SSStreamID(lpstStream);
        lpstEnv->awModuleIndex[lpstEnv->nNumModuleIdents] =
            lpstEnv->nNumModuleIdents;

        lpstEnv->nNumModuleIdents++;
    }
    abyModuleNames[n] = 0;


    /////////////////////////////////////////////////////////////
    // Enumerate through identifier table
    /////////////////////////////////////////////////////////////

    // Open the _VBA_PROJECT stream

    SSInitEnumSibsStruct(lpstSibs,lpstEnv->dwVBAChildEntry);

    if (SSEnumSiblingEntriesCB(lpstStream->lpstRoot,
                               SSOpenStreamByNameCB,
                               gabywsz_VBA_PROJECT,
                               lpstSibs,
                               lpstStream) != SS_STATUS_OK)
    {
        // Couldn't find _VBA_PROJECT stream

        SSFreeEnumSibsStruct(lpstStream->lpstRoot,lpstSibs);
        SSFreeStreamStruct(lpstStream);
        return(FALSE);
    }

    // Free sibling enumeration structure

    SSFreeEnumSibsStruct(lpstStream->lpstRoot,lpstSibs);

    // Remember the entry

    lpstEnv->dw_VBA_PROJECTEntry = SSStreamID(lpstStream);

    // Determine the stream's endian state

    if (O97Get_VBA_PROJECTEndian(lpstStream,
                                 NULL,
                                 &bLittleEndian) == FALSE)
    {
        // Couldn't get the endian state

        SSFreeStreamStruct(lpstStream);
        return(FALSE);
    }

    // Get identifier table location

    if (O97GetIdentifierTableOffset(lpstStream,
                                    &dwIdentTableOffset,
                                    &dwNumIdents) == FALSE)
    {
        // Couldn't find identifier table

        SSFreeStreamStruct(lpstStream);
        return(FALSE);
    }

    // Remember the offset and count

    lpstEnv->dwIdentTableOffset = dwIdentTableOffset;
    lpstEnv->dwNumIdents = dwNumIdents;

    // Enumerate through identifiers

    wID = 0x200;
    for (dw=0;dw<dwNumIdents;dw++,wID+=2)
    {
#ifdef SYM_NLM
        if ((dw & 0xFF) == 0)
        {
            // Relinquish control every 256th identifier

            SSProgress(lpstEnv->lpvRootCookie);
        }
#endif // #ifdef SYM_NLM

        /////////////////////////////////////////////////////////
        // Get the identifier

        // Get the string length and the flag byte

        if (SSSeekRead(lpstStream,
                       dwIdentTableOffset,
                       abyName,
                       2,
                       &dwBytesRead) != SS_STATUS_OK)
            break;

        // Endianize

        if (bLittleEndian == FALSE)
        {
            BYTE        byTemp;

            byTemp = abyName[0];
            abyName[0] = abyName[1];
            abyName[1] = byTemp;
        }

        // If the high bit on the flag byte is set, then skip
        //  the extra bytes

        if (abyName[1] & 0x80)
            dwIdentTableOffset += 8;
        else
            dwIdentTableOffset += 2;

        // Read the identifier

        if (SSSeekRead(lpstStream,
                       dwIdentTableOffset,
                       abyName + 1,
                       abyName[0],
                       &dwBytesRead) != SS_STATUS_OK)
            break;

        // Move over the string and the end bytes

        dwIdentTableOffset += abyName[0] + 4;

        // First lower case the name

        for (n=1;n<=abyName[0];n++)
        {
            if (abyName[n] <= 'Z' && abyName[n] >= 'A')
                abyName[n] += ('a' - 'A');
        }

        /////////////////////////////////////////////////////////
        // First check against module names

        lpbyCand = abyModuleNames;
        n = 0;
        while (*lpbyCand)
        {
            if (lpbyCand[0] == abyName[0])
            {
                for (i=1;i<=abyName[0];i++)
                    if (lpbyCand[i] != abyName[i])
                        break;

                if (i > abyName[0])
                {
                    // Found a match

                    lpstEnv->awModuleIdent[n] = wID;
                    break;
                }
            }

            lpbyCand += *lpbyCand + 1;
            ++n;
        }

        /////////////////////////////////////////////////////////
        // See if it is a known O97 identifier

        // Use a linear search

        for (n=0;n<NUM_O97_STR;n++)
        {
            lpbyCand = galpbyO97_STR[n];
            if (lpbyCand[0] == abyName[0])
            {
                for (i=1;i<=abyName[0];i++)
                    if (lpbyCand[i] != abyName[i])
                        break;

                if (i > abyName[0])
                {
                    // Found a match

                    lpstEnv->awO97Ident[n] = wID;
                    break;
                }
            }
        }

        /////////////////////////////////////////////////////////
        // See if it is a known identifier

        // Use a linear search

        for (n=0;n<lpstEnv->nNumKnownIdents;n++)
        {
            lpbyCand = lplpabyKnownIdentSet[n];
            if (lpbyCand[0] == abyName[0])
            {
                for (i=1;i<=abyName[0];i++)
                    if (lpbyCand[i] != abyName[i])
                        break;

                if (i > abyName[0])
                {
                    // Found a match

                    lpwKnownIdent[n] = wID;
                    break;
                }
            }
        }
    }

    // Do a shell sort on the identifiers

    for (nGap=lpstEnv->nNumKnownIdents/2;nGap>0;nGap/=2)
        for (i=nGap;i<lpstEnv->nNumKnownIdents;i++)
        {
            j = i - nGap;
            while (j >= 0 && lpwKnownIdent[j] > lpwKnownIdent[j+nGap])
            {
                // Swap

                wID = lpwKnownIdent[j];
                lpwKnownIdent[j] = lpwKnownIdent[j+nGap];
                lpwKnownIdent[j+nGap] = wID;

                wID = lpwIdentEnum[j];
                lpwIdentEnum[j] = lpwIdentEnum[j+nGap];
                lpwIdentEnum[j+nGap] = wID;

                j -= nGap;
            }
        }

    // Do a shell sort on the module name identifiers

    lpwKnownIdent = lpstEnv->awModuleIdent;
    for (nGap=lpstEnv->nNumModuleIdents/2;nGap>0;nGap/=2)
        for (i=nGap;i<lpstEnv->nNumModuleIdents;i++)
        {
            j = i - nGap;
            while (j >= 0 && lpwKnownIdent[j] > lpwKnownIdent[j+nGap])
            {
                // Swap

                wID = lpwKnownIdent[j];
                lpwKnownIdent[j] = lpwKnownIdent[j+nGap];
                lpwKnownIdent[j+nGap] = wID;

                dw = lpstEnv->adwModuleEntry[j];
                lpstEnv->adwModuleEntry[j] = lpstEnv->adwModuleEntry[j+nGap];
                lpstEnv->adwModuleEntry[j+nGap] = dw;

                wID = lpstEnv->awModuleIndex[j];
                lpstEnv->awModuleIndex[j] = lpstEnv->awModuleIndex[j+nGap];
                lpstEnv->awModuleIndex[j+nGap] = (WORD)dw;

                j -= nGap;
            }
        }

    // Free the stream structure

    SSFreeStreamStruct(lpstStream);
    return(TRUE);
}


//*************************************************************************
//
// Function:
//  WORD O97GetO97IdentEnum()
//
// Parameters:
//  lpstEnv                 Ptr to initialized environment struct
//  wID                     ID to search for
//
// Description:
//  This function searches through the O97 identifier IDs and returns
//  the enumeration ID for it.  If the ID is not known the function
//  returns 0xFFFF.
//
// Returns:
//  WORD                    Enumeration number
//  0xFFFF                  If unknown
//
//*************************************************************************

WORD O97GetO97IdentEnum
(
    LPO97_ENV       lpstEnv,
    WORD            wID
)
{
    int             i;

    // Do a linear search through the O97 identifiers

    for (i=0;i<NUM_O97_STR;i++)
    {
        if (wID == lpstEnv->awO97Ident[i])
            return(lpstEnv->awO97IdentEnum[i]);
    }

    return(0xFFFF);
}


//*************************************************************************
//
// Function:
//  WORD O97GetKnownIdentEnum()
//
// Parameters:
//  lpstEnv                 Ptr to initialized environment struct
//  wID                     ID to search for
//
// Description:
//  This function searches through the known identifier IDs and returns
//  the enumeration ID for it.  If the ID is not known the function
//  returns 0xFFFF.
//
// Returns:
//  WORD                    Enumeration number
//  0xFFFF                  If unknown
//
//*************************************************************************

WORD O97GetKnownIdentEnum
(
    LPO97_ENV       lpstEnv,
    WORD            wID
)
{
    LPWORD          lpwKnownIdent;
    long            lLow, lMid, lHigh;

    // Do a binary search through the known identifiers

    lpwKnownIdent = lpstEnv->lpwKnownIdent;
    lLow = 0;
    lHigh = lpstEnv->nNumKnownIdents - 1;
    while (lLow <= lHigh)
    {
        lMid = (lLow + lHigh) / 2;

        if (wID < lpwKnownIdent[lMid])
        {
            // In lower half

            lHigh = lMid - 1;
        }
        else
        if (wID > lpwKnownIdent[lMid])
        {
            // In upper half

            lLow = lMid + 1;
        }
        else
        {
            // Found a match

            return lpstEnv->lpwIdentEnum[lMid];
        }
    }

    return(0xFFFF);
}


//*************************************************************************
//
// Function:
//  DWORD O97GetModuleEntryNum()
//
// Parameters:
//  lpstEnv                 Ptr to initialized environment struct
//  wID                     ID to search for
//  lpwModuleIndex          Ptr to WORD for module index
//
// Description:
//  This function searches through the module name IDs and returns
//  the stream entry number for the module.  If the ID is not in the
//  list, then the function returns zero.
//
// Returns:
//  DWORD                   Module stream entry number
//  0                       If module is unknown
//
//*************************************************************************

DWORD O97GetModuleEntryNum
(
    LPO97_ENV       lpstEnv,
    WORD            wID,
    LPWORD          lpwModuleIndex
)
{
    LPWORD          lpwKnownIdent;
    long            lLow, lMid, lHigh;

    // Do a binary search through the known identifiers

    lpwKnownIdent = lpstEnv->awModuleIdent;
    lLow = 0;
    lHigh = lpstEnv->nNumModuleIdents - 1;
    while (lLow <= lHigh)
    {
        lMid = (lLow + lHigh) / 2;

        if (wID < lpwKnownIdent[lMid])
        {
            // In lower half

            lHigh = lMid - 1;
        }
        else
        if (wID > lpwKnownIdent[lMid])
        {
            // In upper half

            lLow = lMid + 1;
        }
        else
        {
            // Found a match

            if (lpwModuleIndex != NULL)
                *lpwModuleIndex = lpstEnv->awModuleIndex[lMid];

            return lpstEnv->adwModuleEntry[lMid];
        }
    }

    return((DWORD)0);
}


//*************************************************************************
//
// Function:
//  BOOL O97IdentLookup()
//
// Parameters:
//  lpstEnv                 Ptr to initialized environment struct
//  lpbyIdent0              Ptr to first string identifier
//  lpbyIdent1              Ptr to second string identifier
//  lpwID0                  Ptr to WORD for first ID
//  lpwID1                  Ptr to WORD for second ID
//
// Description:
//  This function searches enumerates through the identifier table
//  and finds the IDs of the given string identifiers.  If an
//  identifier is not found *lpwID is set to O97_ID_INVALID.
//
//  Either lpbyIdent0 or lpbyIdent1 can be NULL and thus the
//  corresponding lpwID0/1 pointer must also be NULL to indicate
//  that the ID for that identifier is not desired.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL O97IdentLookup
(
    LPO97_ENV       lpstEnv,
    LPBYTE          lpbyIdent0,
    LPBYTE          lpbyIdent1,
    LPWORD          lpwID0,
    LPWORD          lpwID1
)
{
    DWORD           dwIdentTableOffset;
    DWORD           dwNumIdents;
    BYTE            abyName[256];
    BYTE            byChar;
    DWORD           dw;
    DWORD           dwBytesRead;
    WORD            wID;
    int             i;
    LPSS_STREAM     lpstStream;

    // Initialize to O97_ID_INVALID

    if (lpwID0 != NULL)
        *lpwID0 = O97_ID_INVALID;

    if (lpwID1 != NULL)
        *lpwID1 = O97_ID_INVALID;

    // Create a stream structure

    if (SSAllocStreamStruct(lpstEnv->lpstRoot,
                            &lpstStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
        return(FALSE);

    // Open the _VBA_PROJECT stream

    if (SSOpenStreamAtIndex(lpstStream,
                            lpstEnv->dw_VBA_PROJECTEntry) != SS_STATUS_OK)
    {
        SSFreeStreamStruct(lpstStream);
        return(FALSE);
    }

    dwIdentTableOffset = lpstEnv->dwIdentTableOffset;
    dwNumIdents = lpstEnv->dwNumIdents;

    // Enumerate through identifiers

    wID = 0x200;
    for (dw=0;dw<dwNumIdents;dw++,wID+=2)
    {
        /////////////////////////////////////////////////////////
        // Get the identifier

        // Get the string length and the flag byte

        if (SSSeekRead(lpstStream,
                       dwIdentTableOffset,
                       abyName,
                       2,
                       &dwBytesRead) != SS_STATUS_OK)
            break;

        // If the high bit on the flag byte is set, then skip
        //  the extra bytes

        if (abyName[1] & 0x80)
            dwIdentTableOffset += 8;
        else
            dwIdentTableOffset += 2;

        // Read the identifier

        if (SSSeekRead(lpstStream,
                       dwIdentTableOffset,
                       abyName + 1,
                       abyName[0],
                       &dwBytesRead) != SS_STATUS_OK)
        {
            SSFreeStreamStruct(lpstStream);
            return(FALSE);
        }

        // Move over the string and the end bytes

        dwIdentTableOffset += abyName[0] + 4;

        /////////////////////////////////////////////////////////
        // Check against first identifier

        if (lpbyIdent0 && lpbyIdent0[0] == abyName[0])
        {
            // First lower case the name

            for (i=1;i<=abyName[0];i++)
            {
                if (abyName[i] <= 'Z' && abyName[i] >= 'A')
                    abyName[i] += ('a' - 'A');

                byChar = lpbyIdent0[i];
                if (byChar <= 'Z' && byChar >= 'A')
                {
                    if ((byChar + ('a' - 'A')) != abyName[i])
                        break;
                }
                else
                if (byChar != abyName[i])
                    break;
            }

            if (i > abyName[0])
            {
                // Found it

                *lpwID0 = wID;
                if (lpbyIdent1 == NULL || *lpwID1 != O97_ID_INVALID)
                {
                    // Found both

                    SSFreeStreamStruct(lpstStream);
                    return(TRUE);
                }
            }
        }

        /////////////////////////////////////////////////////////
        // Check against second identifier

        if (lpbyIdent1 && lpbyIdent1[0] == abyName[0])
        {
            // First lower case the name

            for (i=1;i<=abyName[0];i++)
            {
                if (abyName[i] <= 'Z' && abyName[i] >= 'A')
                    abyName[i] += ('a' - 'A');

                byChar = lpbyIdent1[i];
                if (byChar <= 'Z' && byChar >= 'A')
                {
                    if ((byChar + ('a' - 'A')) != abyName[i])
                        break;
                }
                else
                if (byChar != abyName[i])
                    break;
            }

            if (i > abyName[0])
            {
                // Found it

                *lpwID1 = wID;
                if (lpbyIdent0 == NULL || *lpwID0 != O97_ID_INVALID)
                {
                    // Found both

                    SSFreeStreamStruct(lpstStream);
                    return(TRUE);
                }
            }
        }
    }

    SSFreeStreamStruct(lpstStream);
    return(TRUE);
}



