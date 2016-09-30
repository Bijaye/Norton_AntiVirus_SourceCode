//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/MODENV.CPv   1.0   30 Jun 1997 16:15:40   DCHI  $
//
// Description:
//  Macro emulation environment module environment functions.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/MODENV.CPv  $
// 
//    Rev 1.0   30 Jun 1997 16:15:40   DCHI
// Initial revision.
// 
//************************************************************************

#include <stdio.h>

#include "platform.h"
#include "wd7api.h"

#include "wdop.h"
#include "wd7env.h"

#include "crc32.h"

//*************************************************************************
//
// LPMODULE_ENV ModuleEnvCreate()
//
// Parameters:
//  lpstEnv             Ptr to environment structure
//  lpstKey             Ptr to key structure
//  wMacroIndex         Index of macro
//  dwMacroSize         Size of macro
//  dwMacroOffset       Offset of macro
//  byMacroEncryptByte  Macro encryption byte
//
// Description:
//  This function creates a module environment
//
// Returns:
//  LPMODULE_ENV    On success, ptr to module environment structure
//  NULL            On failure
//
//*************************************************************************

LPMODULE_ENV ModuleEnvCreate
(
    LPENV               lpstEnv,
    LPWD7ENCKEY         lpstKey,
    WORD                wMacroIndex,
    DWORD               dwMacroSize,
    DWORD               dwMacroOffset,
    BYTE                byMacroEncryptByte
)
{
    LPMODULE_ENV    lpstModEnv;
    size_t          sAllocSize;
    int             i;
    LPDWORD         lpdw;

    // Limit module depth to 4

    if (lpstEnv->wModuleDepth >= 4)
        return(NULL);

    sAllocSize = sizeof(MODULE_ENV_T) +
        MODULE_ENV_MAX_CACHED_FUNCTIONS * 2 * sizeof(DWORD) +
        MODULE_ENV_MAX_CACHED_SUBS * 2 * sizeof(DWORD);

    if (SSMemoryAlloc(lpstEnv->lpvRootCookie,
                      sAllocSize,
                      (LPLPVOID)&lpstModEnv) != SS_STATUS_OK)
        return(NULL);

    for (i=0;i<MODULE_ENV_HASH_ARRAY_SIZE;i++)
    {
        lpstModEnv->abyGlobalVarHash[i] = 0;
        lpstModEnv->abyFunctionHash[i] = 0;
        lpstModEnv->abySubHash[i] = 0;
    }

    lpstModEnv->nMaxCachedFunctions = MODULE_ENV_MAX_CACHED_FUNCTIONS;
    lpstModEnv->nNumCachedFunctions = 0;

    lpstModEnv->nMaxCachedSubs = MODULE_ENV_MAX_CACHED_SUBS;
    lpstModEnv->nNumCachedSubs = 0;

    lpstModEnv->dwMainIP = 0;

    lpstModEnv->lpstPrev = NULL;

    // Assign the function and sub hash and IP arrays

    lpdw = (LPDWORD)(lpstModEnv + 1);
    lpstModEnv->lpdwCachedFunctionsHash = lpdw;
    lpdw += lpstModEnv->nMaxCachedFunctions;
    lpstModEnv->lpdwCachedFunctionsIP = lpdw;
    lpdw += lpstModEnv->nMaxCachedFunctions;

    lpstModEnv->lpdwCachedSubsHash = lpdw;
    lpdw += lpstModEnv->nMaxCachedSubs;
    lpstModEnv->lpdwCachedSubsIP = lpdw;

    // Initialize macro access fields

    lpstModEnv->lpstStream = lpstEnv->lpstStream;
    lpstModEnv->lpstKey = lpstKey;
    lpstModEnv->wMacroIndex = wMacroIndex;
    lpstModEnv->dwMacroOffset = dwMacroOffset;
    lpstModEnv->dwSize = dwMacroSize;
    lpstModEnv->byMacroEncryptByte = byMacroEncryptByte;

    lpstModEnv->dwNumRunBufBytes = 0;
    lpstModEnv->dwRunBufRelOffset = 0;

    // Initialized hashed gotos

    for (i=0;i<MOD_ENV_MAX_HASHED_GOTOS;i++)
    {
        lpstModEnv->awGotoLabelHash[i] = 0;
        lpstModEnv->awGotoLabelCount[i] = 0;
    }

    lpstEnv->wModuleDepth++;

    return(lpstModEnv);
}


//*************************************************************************
//
// BOOL ModuleEnvDestroy()
//
// Parameters:
//  lpstEnv             Ptr to environment structure
//  lpstModEnv          Ptr to module environment
//
// Description:
//  This function destroys a module environment
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL ModuleEnvDestroy
(
    LPENV           lpstEnv,
    LPMODULE_ENV    lpstModEnv
)
{
    lpstEnv->wModuleDepth--;

    if (SSMemoryFree(lpstEnv->lpvRootCookie,
                     lpstModEnv) != SS_STATUS_OK)
        return(FALSE);

    return(TRUE);
}


//*************************************************************************
//
// void ModuleEnvGotoLabelOkay()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  byLen               Length of label
//  lpbyLabel           Ptr to label
//
// Description:
//  Determines whether the number of gotos to the given label
//  have exceeded the artificial limit.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL ModuleEnvGotoLabelOkay
(
    LPENV           lpstEnv,
    BYTE            byLen,
    LPBYTE          lpbyLabel
)
{
    WORD            wHash;
    int             i;
    LPMODULE_ENV    lpstModEnv = lpstEnv->lpstModEnv;

    wHash = (WORD)CRC32I(byLen,lpbyLabel);

    for (i=0;i<MOD_ENV_MAX_HASHED_GOTOS;i++)
    {
        if (lpstModEnv->awGotoLabelHash[i] == 0)
            break;

        if (lpstModEnv->awGotoLabelHash[i] == wHash)
        {
            if (lpstModEnv->awGotoLabelCount[i] >= 64)
            {
                // Don't allow it

                return(FALSE);
            }

            lpstModEnv->awGotoLabelCount[i]++;
            return(TRUE);
        }
    }

    if (i >= MOD_ENV_MAX_HASHED_GOTOS)
        return(TRUE);

    lpstModEnv->awGotoLabelHash[i] = wHash;
    lpstModEnv->awGotoLabelCount[i] = 1;
    return(TRUE);
}


//*************************************************************************
//
// void ModuleEnvCacheGlobalVar()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  byNameLen           Length of name
//  lpbyName            Ptr to name
//
// Description:
//  This function hashes a global variable name to indicate that it
//  is a global variable.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

void ModuleEnvCacheGlobalVar
(
    LPMODULE_ENV    lpstModEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName
)
{
    DWORD           dwCRC;
    BYTE            byBit;

    dwCRC = CRC32I(byNameLen,lpbyName);

    // Turn on the bit

    byBit = (BYTE)(dwCRC & 0xFF);

    lpstModEnv->abyGlobalVarHash[byBit >> 3] |= 1 << (byBit & 0x07);
}


//*************************************************************************
//
// void ModuleEnvCacheFunction()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  byNameLen           Length of name
//  lpbyName            Ptr to name
//  dwIP                IP of function body
//
// Description:
//  This function hashes a function name to indicate that it
//  is a function name.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

void ModuleEnvCacheFunction
(
    LPMODULE_ENV    lpstModEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName,
    DWORD           dwIP
)
{
    DWORD           dwCRC;
    BYTE            byBit;
    int             nIndex;

    dwCRC = CRC32I(byNameLen,lpbyName);

    // Turn on the bit

    byBit = (BYTE)(dwCRC & 0xFF);

    lpstModEnv->abyFunctionHash[byBit >> 3] |= 1 << (byBit & 0x07);

    // Cache the IP if there is room

    nIndex = lpstModEnv->nNumCachedFunctions;
    if (nIndex < lpstModEnv->nMaxCachedFunctions)
    {
        lpstModEnv->lpdwCachedFunctionsHash[nIndex] = dwCRC;
        lpstModEnv->lpdwCachedFunctionsIP[nIndex] = dwIP;
        lpstModEnv->nNumCachedFunctions++;
    }
}


//*************************************************************************
//
// void ModuleEnvCacheSubroutine()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  byNameLen           Length of name
//  lpbyName            Ptr to name
//  dwIP                IP of subroutine body
//
// Description:
//  This function hashes a subroutine name to indicate that it
//  is a subroutine name.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

void ModuleEnvCacheSub
(
    LPMODULE_ENV    lpstModEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName,
    DWORD           dwIP
)
{
    DWORD           dwCRC;
    BYTE            byBit;
    int             nIndex;

    dwCRC = CRC32I(byNameLen,lpbyName);

    // Turn on the bit

    byBit = (BYTE)(dwCRC & 0xFF);

    lpstModEnv->abySubHash[byBit >> 3] |= 1 << (byBit & 0x07);

    // Cache the IP if there is room

    nIndex = lpstModEnv->nNumCachedSubs;
    if (nIndex < lpstModEnv->nMaxCachedSubs)
    {
        lpstModEnv->lpdwCachedSubsHash[nIndex] = dwCRC;
        lpstModEnv->lpdwCachedSubsIP[nIndex] = dwIP;
        lpstModEnv->nNumCachedSubs++;
    }
}


//*************************************************************************
//
// EIDENTIFIER_T ModuleEnvClassifyIdentifier()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  byNameLen           Length of name
//  lpbyName            Ptr to name
//
// Description:
//  Classifies an identifier as either a global variable, subroutine,
//  or function.
//
// Returns:
//  EIDENTIFIER_T       Type of identifier
//
//*************************************************************************

EIDENTIFIER_T ModuleEnvClassifyIdentifier
(
    LPENV           lpstEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName
)
{
    DWORD           dwCRC;
    BOOL            bGlobalVarHit;
    BOOL            bFunctionHit;
    BOOL            bSubHit;
    BYTE            byBit;
    LPMODULE_ENV    lpstModEnv;

    lpstModEnv = lpstEnv->lpstModEnv;

    dwCRC = CRC32I(byNameLen,lpbyName);

    byBit = (BYTE)(dwCRC & 0xFF);

    // Check the global var hash

    if (lpstModEnv->abyGlobalVarHash[byBit >> 3] &
        (1 << (byBit & 0x07)))
        bGlobalVarHit = TRUE;
    else
        bGlobalVarHit = FALSE;

    // Check the function hash

    if (lpstModEnv->abyFunctionHash[byBit >> 3] &
        (1 << (byBit & 0x07)))
        bFunctionHit = TRUE;
    else
        bFunctionHit = FALSE;

    // Check the sub hash

    if (lpstModEnv->abySubHash[byBit >> 3] &
        (1 << (byBit & 0x07)))
        bSubHit = TRUE;
    else
        bSubHit = FALSE;

    if (bGlobalVarHit == FALSE &&
        bFunctionHit == FALSE &&
        bSubHit == FALSE)
    {
        // Assumption is local var

        return(eIDENTIFIER_LOCAL_VAR);
    }

    if (bGlobalVarHit == TRUE &&
        bFunctionHit == FALSE &&
        bSubHit == FALSE)
    {
        // Must be global var

        return(eIDENTIFIER_GLOBAL_VAR);
    }

    if (bGlobalVarHit == FALSE &&
        bFunctionHit == TRUE &&
        bSubHit == FALSE)
    {
        // Must be function

        return(eIDENTIFIER_FUNCTION);
    }

    if (bGlobalVarHit == FALSE &&
        bFunctionHit == FALSE &&
        bSubHit == TRUE)
    {
        // Must be sub

        return(eIDENTIFIER_SUB);
    }

    // The following should be fixed

    if (bGlobalVarHit == TRUE)
    {
        // Assume global var

        return(eIDENTIFIER_GLOBAL_VAR);
    }

    if (bFunctionHit == TRUE)
    {
        // Assume function

        return(eIDENTIFIER_FUNCTION);
    }

    // Assume sub

    return(eIDENTIFIER_SUB);
}


//*************************************************************************
//
// EIDENTIFIER_T ModuleEnvClassifyExprIdentifier()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  byNameLen           Length of name
//  lpbyName            Ptr to name
//
// Description:
//  Classifies an identifier with a function as either a global variable
//  a function, or a local variable.
//
// Returns:
//  EIDENTIFIER_T       Type of identifier
//
//*************************************************************************

EIDENTIFIER_T ModuleEnvClassifyExprIdentifier
(
    LPENV           lpstEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName
)
{
    DWORD           dwCRC;
    BOOL            bGlobalVarHit;
    BOOL            bFunctionHit;
    BYTE            byBit;
    LPMODULE_ENV    lpstModEnv;

    lpstModEnv = lpstEnv->lpstModEnv;

    dwCRC = CRC32I(byNameLen,lpbyName);

    byBit = (BYTE)(dwCRC & 0xFF);

    // Check the global var hash

    if (lpstModEnv->abyGlobalVarHash[byBit >> 3] &
        (1 << (byBit & 0x07)))
        bGlobalVarHit = TRUE;
    else
        bGlobalVarHit = FALSE;

    // Check the function hash

    if (lpstModEnv->abyFunctionHash[byBit >> 3] &
        (1 << (byBit & 0x07)))
        bFunctionHit = TRUE;
    else
        bFunctionHit = FALSE;

    if (bGlobalVarHit == FALSE &&
        bFunctionHit == FALSE)
    {
        // Assumption is local var

        return(eIDENTIFIER_LOCAL_VAR);
    }

    if (bGlobalVarHit == TRUE &&
        bFunctionHit == FALSE)
    {
        // Must be global var

        return(eIDENTIFIER_GLOBAL_VAR);
    }

    if (bGlobalVarHit == FALSE &&
        bFunctionHit == TRUE)
    {
        // Must be function

        return(eIDENTIFIER_FUNCTION);
    }

    // It could be anything, assume GLOBAL_VAR if bit is set

    if (bGlobalVarHit == TRUE)
        return(eIDENTIFIER_GLOBAL_VAR);

    // Otherwise, assume LOCAL_VAR

    return(eIDENTIFIER_LOCAL_VAR);
}


//*************************************************************************
//
// BOOL ModuleEnvVariableIsGlobal()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  byNameLen           Length of name
//  lpbyName            Ptr to name
//
// Description:
//  Determines whether an identifier is a global variable or not.
//
// Returns:
//  TRUE                Identifier is global
//  FALSE               Identifier is not global
//
//*************************************************************************

BOOL ModuleEnvVariableIsGlobal
(
    LPENV           lpstEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName
)
{
    DWORD           dwCRC;
    BYTE            byBit;
    LPMODULE_ENV    lpstModEnv;

    lpstModEnv = lpstEnv->lpstModEnv;

    dwCRC = CRC32I(byNameLen,lpbyName);

    byBit = (BYTE)(dwCRC & 0xFF);

    // Check the global var hash

    if (lpstModEnv->abyGlobalVarHash[byBit >> 3] &
        (1 << (byBit & 0x07)))
        return(TRUE);

    return(FALSE);
}


//*************************************************************************
//
// BOOL ModuleEnvGetFunctionIP()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  byNameLen           Length of name
//  lpbyName            Ptr to name
//  lpdwIP              Ptr to DWORD variable for IP
//
// Description:
//  Gets the IP for a function.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//*************************************************************************

BOOL ModuleEnvGetFunctionIP
(
    LPENV           lpstEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName,
    LPDWORD         lpdwIP
)
{
    DWORD           dwCRC;
    int             nIndex;

    LPMODULE_ENV    lpstModEnv;

    lpstModEnv = lpstEnv->lpstModEnv;

    dwCRC = CRC32I(byNameLen,lpbyName);

    for (nIndex=0;nIndex<lpstModEnv->nNumCachedFunctions;nIndex++)
    {
        if (dwCRC == lpstModEnv->lpdwCachedFunctionsHash[nIndex])
        {
            *lpdwIP = lpstModEnv->lpdwCachedFunctionsIP[nIndex] +
                byNameLen + 1;
            return(TRUE);
        }
    }

    return(FALSE);
}


//*************************************************************************
//
// BOOL ModuleEnvGetSubIP()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  byNameLen           Length of name
//  lpbyName            Ptr to name
//  lpdwIP              Ptr to DWORD variable for IP
//
// Description:
//  Gets the IP for a subroutine.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//*************************************************************************

BOOL ModuleEnvGetSubIP
(
    LPENV           lpstEnv,
    BYTE            byNameLen,
    LPBYTE          lpbyName,
    LPDWORD         lpdwIP
)
{
    DWORD           dwCRC;
    int             nIndex;

    LPMODULE_ENV    lpstModEnv;

    lpstModEnv = lpstEnv->lpstModEnv;

    dwCRC = CRC32I(byNameLen,lpbyName);

    for (nIndex=0;nIndex<lpstModEnv->nNumCachedSubs;nIndex++)
    {
        if (dwCRC == lpstModEnv->lpdwCachedSubsHash[nIndex])
        {
            *lpdwIP = lpstModEnv->lpdwCachedSubsIP[nIndex] +
                byNameLen + 1;
            return(TRUE);
        }
    }

    return(FALSE);
}


//*************************************************************************
//
// BOOL ModuleRead()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  dwOffset            Offset from which to read
//  lpbyBuf             Ptr to buffer to store data
//  dwNumBytes          Number of bytes to read
//
// Description:
//  Reads from the module at the given offset.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//*************************************************************************

BOOL ModuleRead
(
    LPMODULE_ENV        lpstModEnv,
    DWORD               dwOffset,
    LPBYTE              lpbyBuf,
    DWORD               dwNumBytes
)
{
    LPBYTE              lpby;
    WORD                wBytesToRead;

    if (dwOffset + dwNumBytes >=
        lpstModEnv->dwRunBufRelOffset + lpstModEnv->dwNumRunBufBytes ||
        dwOffset < lpstModEnv->dwRunBufRelOffset)
    {
        if (dwOffset + dwNumBytes > lpstModEnv->dwSize)
        {
            // Attempt to read past the end

            return(FALSE);
        }

        if (dwOffset + HEU_RUN_BUF_SIZE > lpstModEnv->dwSize)
            wBytesToRead = (WORD)(lpstModEnv->dwSize - dwOffset);
        else
            wBytesToRead = HEU_RUN_BUF_SIZE;

        if (WD7EncryptedRead(lpstModEnv->lpstStream,
                             lpstModEnv->lpstKey,
                             lpstModEnv->dwMacroOffset + dwOffset,
                             lpstModEnv->abyRunBuf,
                             wBytesToRead) != wBytesToRead)
        {
            // Failed to read

            return(FALSE);
        }

        lpstModEnv->dwRunBufRelOffset = dwOffset;
        lpstModEnv->dwNumRunBufBytes = wBytesToRead;

        // Decrypt if necessary

        if (lpstModEnv->byMacroEncryptByte != 0)
        {
            BYTE byEncrypt = lpstModEnv->byMacroEncryptByte;

            lpby = lpstModEnv->abyRunBuf;

            while (wBytesToRead-- > 0)
                *lpby++ ^= byEncrypt;
        }
    }

    // Copy the bytes

    lpby = lpstModEnv->abyRunBuf +
        (dwOffset - lpstModEnv->dwRunBufRelOffset);

    while (dwNumBytes-- != 0)
        *lpbyBuf++ = *lpby++;

    return(TRUE);
}


//*************************************************************************
//
// BOOL ModuleReadIdentifier()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  dwIP                IP to read from
//  lpbyIdentifierLen   Ptr to BYTE variable for identifier length
//  lpbyIdentifier      Ptr to storage area for identifier
//
// Description:
//  Reads an identifier from the given IP.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//*************************************************************************

BOOL ModuleReadIdentifier
(
    LPMODULE_ENV        lpstModEnv,
    DWORD               dwIP,
    LPBYTE              lpbyIdentifierLen,
    LPBYTE              lpbyIdentifier
)
{
    if (ModuleRead(lpstModEnv,
                   dwIP,
                   lpbyIdentifierLen,
                   sizeof(BYTE)) == FALSE)
        return(FALSE);

    if (ModuleRead(lpstModEnv,
                   dwIP+1,
                   lpbyIdentifier,
                   *lpbyIdentifierLen) == FALSE)
        return(FALSE);

    return(TRUE);
}


//*************************************************************************
//
// BOOL ModuleReadUIdentifier()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  dwIP                IP to read from
//  lpbyIdentifierLen   Ptr to BYTE variable for identifier length
//  lpbyIdentifier      Ptr to storage area for identifier
//
// Description:
//  Reads a Unicode identifier from the given IP.  Only the first
//  byte of each Unicode character is read.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//*************************************************************************

BOOL ModuleReadUIdentifier
(
    LPMODULE_ENV        lpstModEnv,
    DWORD               dwIP,
    LPBYTE              lpbyIdentifierLen,
    LPBYTE              lpbyIdentifier
)
{
    int                 i;

    if (ModuleRead(lpstModEnv,
                   dwIP,
                   lpbyIdentifierLen,
                   sizeof(BYTE)) == FALSE)
        return(FALSE);

    ++dwIP;
    for (i=0;i<lpbyIdentifierLen[0];i++)
    {
        if (ModuleRead(lpstModEnv,
                       dwIP,
                       lpbyIdentifier + i,
                       sizeof(BYTE)) == FALSE)
            return(FALSE);

        dwIP += 2;
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL ModuleScan()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//
// Description:
//  Scan for the following:
//      global variables        Indicated by Dim Shared
//      subroutines             Indicated by Sub
//      functions               Indicated by Function
//
//  The algorithm works using a state machine that follows the
//  following guidelines:
//  - All SPACEs, TABs, and BACKSLASHes are ignored
//  - A "Dim Shared", "Sub", or "Function" must be the first simple
//    on a line to be considered
//  - The first token after the "Dim Shared" must be a PASCAL_IDENTIFIER
//    to be considered a valid global variable
//  - Multiple global variables in a single "Dim Shared" must each
//    be specified as a PASCAL_IDENTIFIER immediately following the
//    comma separating them
//  - The first token after the "Sub" must be a PASCAL_IDENTIFIER
//    to be considered a valid subroutine name
//  - The first token after the "Function" must be a PASCAL_IDENTIFIER
//    to be considered a valid function name
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//*************************************************************************

typedef enum tagESSCAN_GLOBAL
{
    esSCAN_GLOBAL_BOL,
    esSCAN_GLOBAL_WAIT_EOL,
    esSCAN_GLOBAL_SUB,
    esSCAN_GLOBAL_FUNCTION,
    esSCAN_GLOBAL_DIM,
    esSCAN_GLOBAL_SHARED,
    esSCAN_GLOBAL_WAIT_NEXT_SHARED
} ESSCAN_GLOBAL_T, FAR *LPESSCAN_GLOBAL;

/*
void PrintPascalString(LPBYTE lpabyMacroBinary)
{
    int     i, len;

    len = lpabyMacroBinary[1];

    for (i=0;i<len;i++)
    {
        putchar(lpabyMacroBinary[i + 2]);
    }
}
*/

BOOL ModuleScan
(
    LPMODULE_ENV        lpstModEnv
)
{
    DWORD               dwIP;
    BYTE                byCurToken;
    ESSCAN_GLOBAL_T     esState;
    WORD                wTemp;
    BYTE                byLen;
    BYTE                abyIdentifier[256];
    BYTE                abyTwoBytes[2];

    // Determine endianness

    if (ModuleRead(lpstModEnv,
                   0,
                   abyTwoBytes,
                   2) == FALSE)
        return(FALSE);

    if (abyTwoBytes[0] == 0x01 && abyTwoBytes[1] == 0x00)
        lpstModEnv->bLittleEndian = TRUE;
    else
    if (abyTwoBytes[0] == 0x00 && abyTwoBytes[1] == 0x01)
        lpstModEnv->bLittleEndian = FALSE;
    else
    {
        // Assume little endian

        lpstModEnv->bLittleEndian = TRUE;
    }

    dwIP = 2;
    lpstModEnv->dwMainIP = lpstModEnv->dwSize;
    esState = esSCAN_GLOBAL_BOL;
    while (dwIP < lpstModEnv->dwSize)
    {
        if (ModuleRead(lpstModEnv,
                       dwIP,
                       &byCurToken,
                       sizeof(BYTE)) == FALSE)
            return(FALSE);

        // Perform any necessary state transitions

        switch (byCurToken)
        {
            // Skip white space

            case WDOP_SPACE:
            case WDOP_TAB:
            case WDOP_MULTIPLE_SPACES:
            case WDOP_MULTIPLE_TABS:
            case WDOP_BACKSLASH:
                break;

            default:
                switch (esState)
                {
                    case esSCAN_GLOBAL_BOL:
                        switch (byCurToken)
                        {
                            case WDOP_SUB:
                                esState = esSCAN_GLOBAL_SUB;
                                break;

                            case WDOP_FUNCTION:
                                esState = esSCAN_GLOBAL_FUNCTION;
                                break;

                            case WDOP_DIM:
                                esState = esSCAN_GLOBAL_DIM;
                                break;

                            case WDOP_NEWLINE:
                                // Stay in BOL state

                                break;

                            default:
                                esState = esSCAN_GLOBAL_WAIT_EOL;
                                break;
                        }
                        break;

                    case esSCAN_GLOBAL_WAIT_EOL:
                        if (byCurToken == WDOP_NEWLINE)
                            esState = esSCAN_GLOBAL_BOL;
                        break;

                    case esSCAN_GLOBAL_SUB:
                        if (byCurToken == WDOP_UPASCAL_STRING_7B ||
                            byCurToken == WDOP_UPASCAL_STRING ||
                            byCurToken == WDOP_PASCAL_STRING)
                        {
                            // Add the subroutine name to the list

                            if (byCurToken == WDOP_PASCAL_STRING)
                            {
                                if (ModuleReadIdentifier(lpstModEnv,
                                                         dwIP+1,
                                                         &byLen,
                                                         abyIdentifier) == FALSE)
                                    return(FALSE);
                            }
                            else
                            {
                                if (ModuleReadUIdentifier(lpstModEnv,
                                                          dwIP+1,
                                                          &byLen,
                                                          abyIdentifier) == FALSE)
                                    return(FALSE);
                            }

                            // Check for MAIN first

                            if (byLen == 4 &&
                                (abyIdentifier[0] == 'M' ||
                                 abyIdentifier[0] == 'm') &&
                                (abyIdentifier[1] == 'A' ||
                                 abyIdentifier[1] == 'a') &&
                                (abyIdentifier[2] == 'I' ||
                                 abyIdentifier[2] == 'i') &&
                                (abyIdentifier[3] == 'N' ||
                                 abyIdentifier[3] == 'n'))
                            {
                                lpstModEnv->dwMainIP = dwIP + 6;
                            }
                            else
                            {
                                ModuleEnvCacheSub(lpstModEnv,
                                                  byLen,
                                                  abyIdentifier,
                                                  dwIP + 1);
                            }
//                            printf("Sub: ");
//                            PrintPascalString(lpabyMacroBinary+dwIP);
//                            printf("\n");
                        }
                        esState = esSCAN_GLOBAL_WAIT_EOL;
                        break;

                    case esSCAN_GLOBAL_FUNCTION:
                        if (byCurToken == WDOP_PASCAL_STRING)
                        {
                            // Add the function name to the list

                            if (ModuleReadIdentifier(lpstModEnv,
                                                     dwIP+1,
                                                     &byLen,
                                                     abyIdentifier) == FALSE)
                                return(FALSE);

                            ModuleEnvCacheFunction(lpstModEnv,
                                                   byLen,
                                                   abyIdentifier,
                                                   dwIP + 1);

//                            printf("Function: ");
//                            PrintPascalString(lpabyMacroBinary+dwIP);
//                            printf("\n");
                        }
                        esState = esSCAN_GLOBAL_WAIT_EOL;
                        break;

                    case esSCAN_GLOBAL_DIM:
                        if (byCurToken == WDOP_SHARED)
                            esState = esSCAN_GLOBAL_SHARED;
                        else
                            esState = esSCAN_GLOBAL_WAIT_EOL;
                        break;

                    case esSCAN_GLOBAL_SHARED:
                        if (byCurToken == WDOP_PASCAL_STRING)
                        {
                            // Add the global variable name to the list

                            if (ModuleReadIdentifier(lpstModEnv,
                                                     dwIP+1,
                                                     &byLen,
                                                     abyIdentifier) == FALSE)
                                return(FALSE);

                            ModuleEnvCacheGlobalVar(lpstModEnv,
                                                    byLen,
                                                    abyIdentifier);

//                            printf("Global: ");
//                            PrintPascalString(lpabyMacroBinary+dwIP);
//                            printf("\n");

                            esState = esSCAN_GLOBAL_WAIT_NEXT_SHARED;
                        }
                        else
                            esState = esSCAN_GLOBAL_WAIT_EOL;
                        break;

                    case esSCAN_GLOBAL_WAIT_NEXT_SHARED:
                        switch (byCurToken)
                        {
                            case WDOP_COMMA:
                                esState = esSCAN_GLOBAL_SHARED;
                                break;

                            case WDOP_NEWLINE:
                                esState = esSCAN_GLOBAL_BOL;
                                break;

                            default:
                                break;
                        }
                        break;

                    default:
                        // This should never happen

                        break;
                }
                break;
        }

        // Advance past the token

        switch (byCurToken)
        {
            case WDOP_DOUBLE_VALUE:
                dwIP += 9;
                break;

            case WDOP_LABEL:
            case WDOP_PASCAL_STRING:
            case WDOP_QUOTED_PASCAL_STRING:
            case WDOP_COMMENT_PASCAL_STRING:
            case WDOP_REM:
            case WDOP_EXTERNAL_MACRO:
            case WDOP_IDENTIFIER:
                if (ModuleRead(lpstModEnv,
                               dwIP + 1,
                               &byLen,
                               sizeof(BYTE)) == FALSE)
                    return(FALSE);
                dwIP += byLen + 2;
                break;

            case WDOP_UNSIGNED_WORD_VALUE:
            case WDOP_ASCII_CHARACTER:
            case WDOP_DIALOG_FIELD:
            case WDOP_WORD_VALUE_LABEL:
            case WDOP_FUNCTION_VALUE:
                dwIP += 3;
                break;

            case WDOP_MULTIPLE_SPACES:
            case WDOP_MULTIPLE_TABS:
                dwIP += 2;
                break;

            case WDOP_QUOTED_UPASCAL_STRING:
                if (ModuleRead(lpstModEnv,
                               dwIP + 1,
                               (LPBYTE)&wTemp,
                               sizeof(WORD)) == FALSE)
                    return(FALSE);
                dwIP += 2 * wTemp + 3;
                break;

            case WDOP_UPASCAL_STRING_7B:
            case WDOP_UPASCAL_STRING:
                if (ModuleRead(lpstModEnv,
                               dwIP + 1,
                               &byLen,
                               sizeof(BYTE)) == FALSE)
                    return(FALSE);
                dwIP += 2 * byLen + 2;
                break;

            default:
                ++dwIP;
                break;
        }
    }

	return(TRUE);
}
