// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/W7H2SCAN.CPv   1.2   09 Dec 1998 17:45:16   DCHI  $
//
// Description:
//  Contains Word 6.0/95 Heuristics Level II scan code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/W7H2SCAN.CPv  $
// 
//    Rev 1.2   09 Dec 1998 17:45:16   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.1   12 Oct 1998 13:42:04   DCHI
// Added NLM relinquish control call to W7H2ScanFunc().
// 
//    Rev 1.0   10 Jun 1998 13:05:58   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#ifdef MACROHEU

#include "platform.h"
#include "ctsn.h"
#include "navex.h"

#include "wd7api.h"
#include "crc32.h"
#include "wdop.h"
#include "wddecsig.h"
#include "w7h2scan.h"
#include "w7h2sght.h"

#include "mcrxclud.h"

//*************************************************************************
//
// LPENV W7H2Create()
//
// Parameters:
//  lpvRootCookie   The root cookie
//
// Description:
//  Allocates memory for a W7H2_T scan environment.
//
// Returns:
//  LPENV           On success, ptr to environment structure
//  NULL            On failure
//
//*************************************************************************

LPW7H2 W7H2Create
(
    LPVOID          lpvRootCookie
)
{
    LPW7H2          lpstEnv;

    // Allocate environment structure

    if (SSMemoryAlloc(lpvRootCookie,
                      sizeof(W7H2_T),
                      (LPLPVOID)&lpstEnv) != SS_STATUS_OK)
        return(NULL);

    lpstEnv->lpvRootCookie = lpvRootCookie;

    return(lpstEnv);
}


//*************************************************************************
//
// BOOL W7H2Destroy()
//
// Parameters:
//  lpstEnv         Ptr to environment structure
//
// Description:
//  Destroys memory allocated for a W7H2_T scan environment.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL W7H2Destroy
(
    LPW7H2          lpstEnv
)
{
    SSMemoryFree(lpstEnv->lpvRootCookie,lpstEnv);
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL W7H2Init()
//
// Parameters:
//  LPW7H2          Ptr to heuristic level II structure
//
// Description:
//  Initializes the W7H2_T structure for a full scan.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL W7H2Init
(
    LPW7H2          lpstEnv,
    LPSS_STREAM     lpstStream,
    LPWD7ENCKEY     lpstKey
)
{
    lpstEnv->lpstStream = lpstStream;
    lpstEnv->lpstKey = lpstKey;

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL W7H2RefillRunBuf()
//
// Parameters:
//  lpstEnv         Ptr to W7H2_T structure
//  dwOffset        Offset in macro to read from
//
// Description:
//  Refills the run buffer starting from the given offset minus 256.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

BOOL W7H2RefillRunBuf
(
    LPW7H2              lpstEnv,
    DWORD               dwOffset
)
{
    int                 nNumBytes;
    LPBYTE              lpabyBuf;

    // Always have at least 256 bytes before the buffer

    if (dwOffset < 256)
        dwOffset = 0;
    else
        dwOffset -= 256;

    // Determine how many bytes to read

    if (dwOffset + W7H2_RUN_BUF_SIZE > lpstEnv->dwSize)
    {
        if (dwOffset > lpstEnv->dwSize)
            return(FALSE);

        nNumBytes = lpstEnv->dwSize - dwOffset;
    }
    else
        nNumBytes = W7H2_RUN_BUF_SIZE;

    // Read into run buf

    lpabyBuf = lpstEnv->abyRunBuf;
    if (WD7EncryptedRead(lpstEnv->lpstStream,
                         lpstEnv->lpstKey,
                         lpstEnv->dwOffset + dwOffset,
                         lpstEnv->abyRunBuf,
                         nNumBytes) == FALSE)
        return(FALSE);

    lpstEnv->dwRunBufOffset = dwOffset;
    lpstEnv->dwRunBufEndOffset = dwOffset + nNumBytes;

    // Decrypt the buffer

    if (lpstEnv->byEncryptByte != 0)
    {
        while (nNumBytes-- != 0)
            *lpabyBuf++ ^= lpstEnv->byEncryptByte;
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BYTE W7H2GetBYTE()
//
// Parameters:
//  lpstEnv         Ptr to W7H2_T structure
//  dwOffset        Offset in macro to read from
//
// Description:
//  Retrieves the BYTE from the given offset.
//
//  Note: The function returns zero if there is an error, but this
//  is indistinguishable from an actual value of zero at the given
//  offset.
//
// Returns:
//  BYTE            BYTE value at the given offset
//
//*************************************************************************

BYTE W7H2GetBYTE
(
    LPW7H2              lpstEnv,
    DWORD               dwOffset
)
{
    if (dwOffset < lpstEnv->dwRunBufOffset ||
        lpstEnv->dwRunBufEndOffset <= dwOffset)
    {
        // Refill buffer

        if (W7H2RefillRunBuf(lpstEnv,dwOffset) == FALSE)
            return(0);
    }

    return(lpstEnv->abyRunBuf[dwOffset - lpstEnv->dwRunBufOffset]);
}


//*************************************************************************
//
// Function:
//  WORD W7H2GetWORD()
//
// Parameters:
//  lpstEnv         Ptr to W7H2_T structure
//  dwOffset        Offset in macro to read from
//
// Description:
//  Retrieves the WORD value from the given offset.
//
//  Note: The function returns zero if there is an error, but this
//  is indistinguishable from an actual value of zero at the given
//  offset.
//
// Returns:
//  WORD            WORD value at the given offset
//
//*************************************************************************

WORD W7H2GetWORD
(
    LPW7H2              lpstEnv,
    DWORD               dwOffset
)
{
    LPBYTE              lpaby;

    if (dwOffset < lpstEnv->dwRunBufOffset ||
        lpstEnv->dwRunBufEndOffset <= dwOffset + 1)
    {
        // Refill buffer

        if (W7H2RefillRunBuf(lpstEnv,dwOffset) == FALSE)
            return(0);
    }

    lpaby = lpstEnv->abyRunBuf +
        (dwOffset - lpstEnv->dwRunBufOffset);

    if (lpstEnv->bBigEndian == FALSE)
        return(((WORD)lpaby[0]) | ((WORD)lpaby[1] << 8));

    return(((WORD)lpaby[0] << 8) | ((WORD)lpaby[1]));
}


//*************************************************************************
//
// Function:
//  DWORD W7H2CRCStr()
//
// Parameters:
//  lpstEnv         Ptr to W7H2_T structure
//  dwOffset        Offset in macro to read from
//  nStrLen         Length of string
//
// Description:
//  Performs a ToLower() CRC of the given number of bytes at the
//  given offset.
//
// Returns:
//  DWORD           The CRC of the string.
//
//*************************************************************************

DWORD W7H2CRCStr
(
    LPW7H2              lpstEnv,
    DWORD               dwOffset,
    int                 nStrLen
)
{
    LPBYTE              lpaby;
    DWORD               dwCRC;
    BYTE                by;

    if (dwOffset < lpstEnv->dwRunBufOffset ||
        lpstEnv->dwRunBufEndOffset <= dwOffset + nStrLen)
    {
        // Refill buffer

        if (W7H2RefillRunBuf(lpstEnv,dwOffset) == FALSE)
            return(0);
    }

    lpaby = lpstEnv->abyRunBuf +
        (dwOffset - lpstEnv->dwRunBufOffset);

    CRC32Init(dwCRC);
    while (nStrLen-- != 0)
    {
        by = SSToLower(*lpaby++);
        CRC32Continue(dwCRC,by);
    }

    return(dwCRC);
}


//*************************************************************************
//
// Function:
//  LPBYTE W7H2GetByteArrayPtr()
//
// Parameters:
//  lpstEnv         Ptr to W7H2_T structure
//  dwOffset        Offset in macro to read from
//  nLen            Length of byte array
//
// Description:
//  Returns a ptr to a buffer containing the given number of bytes
//  from the given offset.
//
//  Note that the requested number of bytes must not exceed the
//  run buffer size minus 256.  The caller is expected to obey this
//  rule as the function does not check.
//
// Returns:
//  LPBYTE          Ptr to byte array
//
//*************************************************************************

LPBYTE W7H2GetByteArrayPtr
(
    LPW7H2              lpstEnv,
    DWORD               dwOffset,
    int                 nLen
)
{
    if (dwOffset < lpstEnv->dwRunBufOffset ||
        lpstEnv->dwRunBufEndOffset <= dwOffset + nLen)
    {
        // Refill buffer

        if (W7H2RefillRunBuf(lpstEnv,dwOffset) == FALSE)
            return(NULL);
    }

    return(lpstEnv->abyRunBuf + (dwOffset - lpstEnv->dwRunBufOffset));
}


//*************************************************************************
//
// Function:
//  BOOL W7H2MacroGetFuncs()
//
// Parameters:
//  lpstEnv         Ptr to W7H2_T structure
//
// Description:
//  Scans through the current macro and initializes the following
//  fields of the environment structure:
//
//      bBigEndian          Macro endianness
//      nNumFuncs           Number of functions in macro - 1
//      adwFuncNameCRC[]    CRC of function name
//      adwFuncOffset[]     Offset of function
//      adwIsFunc[]         Hash bit array for function name
//      abyRunState[]       Initialiazed to zero
//
//  Index 0 of the arrays is reserved for the MAIN function.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

#define W7H2_BOL            0
#define W7H2_CONTINUE       1
#define W7H2_FUNC           2
#define W7H2_FUNC_TO_END    3
#define W7H2_END            4
#define W7H2_OTHER          5

BOOL W7H2MacroGetFuncs
(
    LPW7H2              lpstEnv
)
{
    DWORD               dwOffset;
    DWORD               dwMaxOffset;
    int                 nState;
    BYTE                byToken;
    BYTE                byArg;
    WORD                wArg;
    DWORD               dwArgOffset;
    BOOL                bBackSlash;
    BOOL                bInProc;
    DWORD               dwCurFuncCRC;
    int                 nNumFuncs;
    int                 i;

    if (lpstEnv->dwSize < 2)
        return(TRUE);

    // Get endianness

    if (W7H2GetBYTE(lpstEnv,0) == 0)
        lpstEnv->bBigEndian = TRUE;
    else
        lpstEnv->bBigEndian = FALSE;

    // Initialize to MAIN not yet found state

    lpstEnv->abyRunState[0] = 0;
    lpstEnv->adwFuncOffset[0] = 0;

    // Initialize IsFunc[] array

    for (i=0;i<(W7H2_IS_FUNC_BIT_ARRAY_SIZE / 32);i++)
        lpstEnv->adwIsFunc[i] = 0;

    // Initialize for function scan

    nState = W7H2_BOL;
    bBackSlash = FALSE;
    dwOffset = 2;
    dwMaxOffset = lpstEnv->dwSize;
    bInProc = FALSE;
    nNumFuncs = 0;

    // Scan entire macro

    while (dwOffset < dwMaxOffset)
    {
        // Get the next token

        byToken = W7H2GetBYTE(lpstEnv,dwOffset++);

        // Read BYTE and WORD parameters

        switch (byToken)
        {
            case WDOP_LABEL:
            case WDOP_PASCAL_STRING:
            case WDOP_QUOTED_PASCAL_STRING:
            case WDOP_COMMENT_PASCAL_STRING:
            case WDOP_MULTIPLE_SPACES:
            case WDOP_MULTIPLE_TABS:
            case WDOP_REM:
            case WDOP_EXTERNAL_MACRO:
            case WDOP_IDENTIFIER:
            case WDOP_UPASCAL_STRING_7B:
            case WDOP_UPASCAL_STRING:
                byArg = W7H2GetBYTE(lpstEnv,dwOffset++);
                break;

            case WDOP_WORD_VALUE_LABEL:
            case WDOP_FUNCTION_VALUE:
            case WDOP_UNSIGNED_WORD_VALUE:
            case WDOP_ASCII_CHARACTER:
            case WDOP_DIALOG_FIELD:
                dwOffset += 2;
                break;

            case WDOP_QUOTED_UPASCAL_STRING:
                wArg = W7H2GetWORD(lpstEnv,dwOffset);
                dwOffset += 2;
                break;

            default:
                break;
        }

        // Calculate next token offset

        switch (byToken)
        {
            case WDOP_LABEL:
            case WDOP_PASCAL_STRING:
            case WDOP_QUOTED_PASCAL_STRING:
            case WDOP_COMMENT_PASCAL_STRING:
            case WDOP_REM:
            case WDOP_EXTERNAL_MACRO:
            case WDOP_IDENTIFIER:
                dwArgOffset = dwOffset;
                dwOffset += byArg;
                break;

            case WDOP_UPASCAL_STRING_7B:
            case WDOP_UPASCAL_STRING:
                dwArgOffset = dwOffset;
                dwOffset += byArg * sizeof(WORD);
                break;

            case WDOP_QUOTED_UPASCAL_STRING:
                dwArgOffset = dwOffset;
                dwOffset += wArg * (DWORD)sizeof(WORD);
                break;

            case WDOP_DOUBLE_VALUE:
                dwOffset += 8;
                break;

            // Skip white space

            case WDOP_SPACE:
            case WDOP_TAB:
            case WDOP_MULTIPLE_SPACES:
            case WDOP_MULTIPLE_TABS:
                continue;

            case WDOP_BACKSLASH:
                bBackSlash = TRUE;
                continue;

            case WDOP_COLON:
            case WDOP_NEWLINE:
                if (bBackSlash == TRUE)
                    bBackSlash = FALSE;
                else
                {
                    if (nState == W7H2_FUNC_TO_END)
                    {
                        if (dwCurFuncCRC == W7H2_main_CRC)
                        {
                            // Assume MAIN

                            lpstEnv->adwFuncNameCRC[0] = dwCurFuncCRC;
                            lpstEnv->adwFuncOffset[0] = dwOffset;
                        }
                        else
                        if (nNumFuncs < (W7H2_MAX_FUNCS - 1))
                        {
                            ++nNumFuncs;

                            // Remember the CRC

                            lpstEnv->adwFuncNameCRC[nNumFuncs] =
                                dwCurFuncCRC;

                            // Set the IsFunc() bit

                            i = (int)(dwCurFuncCRC & W7H2_IS_FUNC_MASK);

                            lpstEnv->adwIsFunc[i >> 5] |=
                                (DWORD)1 << (i & 0x1F);

                            // Initialize flags and set offset

                            lpstEnv->abyRunState[nNumFuncs] = 0;
                            lpstEnv->adwFuncOffset[nNumFuncs] =
                                dwOffset;
                        }
                    }
                    nState = W7H2_BOL;
                }
                continue;

            default:
                break;
        }

        switch (nState)
        {
            case W7H2_BOL:
                switch (byToken)
                {
                    case WDOP_SUB:
                    case WDOP_FUNCTION:
                        if (bInProc == FALSE)
                        {
                            nState = W7H2_FUNC;
                            bInProc = TRUE;
                        }
                        break;

                    case WDOP_END:
                        if (bInProc != FALSE)
                            nState = W7H2_END;
                        else
                            nState = W7H2_OTHER;
                        break;

                    default:
                        nState = W7H2_OTHER;
                        break;
                }
                break;

            case W7H2_FUNC:
                if (byToken == WDOP_PASCAL_STRING)
                {
                    dwCurFuncCRC = W7H2CRCStr(lpstEnv,
                                              dwArgOffset,
                                              byArg);

                    nState = W7H2_FUNC_TO_END;
                }
                else
                    nState = W7H2_BOL;
                break;

            case W7H2_END:
                if (byToken == WDOP_SUB || byToken == WDOP_FUNCTION)
                    bInProc = FALSE;
                break;

            default:
                break;
        }
    }

    // Remember the number of cached functions

    lpstEnv->nNumFuncs = nNumFuncs;

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  void W7H2SortFuncs()
//
// Parameters:
//  lpstEnv         Ptr to W7H2_T structure
//
// Description:
//  Sorts the function arrays based on the name CRC.  Index 0, the
//  entry for MAIN, is left as is.
//
// Returns:
//  Nothing
//
//*************************************************************************

void W7H2SortFuncs
(
    LPW7H2              lpstEnv
)
{
    int                 i;
    int                 j;
    int                 nNumFuncs;
    int                 nGap;
    DWORD               dwTemp;
    LPDWORD             lpadwCRC;
    LPDWORD             lpadwOffset;

    nNumFuncs = lpstEnv->nNumFuncs;
    lpadwCRC = lpstEnv->adwFuncNameCRC;
    lpadwOffset = lpstEnv->adwFuncOffset;

    // Shell sort the functions by their name CRCs

    for (nGap=nNumFuncs;nGap>0;nGap/=2)
    {
        for (i=nGap;i<nNumFuncs;i++)
        {
            j = i - nGap;
            while (j >= 1 && lpadwCRC[j] > lpadwCRC[j+nGap])
            {
                // Swap the CRC

                dwTemp = lpadwCRC[j];
                lpadwCRC[j] = lpadwCRC[j+nGap];
                lpadwCRC[j+nGap] = dwTemp;

                // Swap the offset

                dwTemp = lpadwOffset[j];
                lpadwOffset[j] = lpadwOffset[j+nGap];
                lpadwOffset[j+nGap] = dwTemp;

                j -= nGap;
            }
        }
    }
}


//*************************************************************************
//
// Function:
//  int W7H2GetFuncIndex()
//
// Parameters:
//  lpstEnv         Ptr to W7H2_T structure
//  dwCRC           CRC of function name to search
//
// Description:
//  Determines the index of containing the function information.
//  The function first performs a quick elimination using the
//  adwIsFunc[] bit array.  Then the function performs a binary
//  search for the function through the function list.
//
// Returns:
//  int             Index containing CRC
//  -1              If CRC is not found
//
//*************************************************************************

int W7H2GetFuncIndex
(
    LPW7H2              lpstEnv,
    DWORD               dwCRC
)
{
    int                 nLow;
    int                 nMid;
    int                 nHigh;
    LPDWORD             lpadwCRC;

    // Perform a quick elimination first

    nMid = (int)(dwCRC & W7H2_IS_FUNC_MASK);

    if ((lpstEnv->adwIsFunc[nMid >> 5] & ((DWORD)1 << (nMid & 0x1F))) == 0)
        return(-1);

    lpadwCRC = lpstEnv->adwFuncNameCRC;

    // Perform a binary search for the CRC

    nLow = 1;
    nHigh = lpstEnv->nNumFuncs;
    while (nLow <= nHigh)
    {
        nMid = (nLow + nHigh) / 2;

        if (dwCRC < lpadwCRC[nMid])
        {
            // In lower half

            nHigh = nMid - 1;
        }
        else
        if (dwCRC > lpadwCRC[nMid])
        {
            // In upper half

            nLow = nMid + 1;
        }
        else
        {
            // Found a match

            return(nMid);
        }
    }

    return(-1);
}


//*************************************************************************
//
// Function:
//  void W7H2MarkFuncForRun()
//
// Parameters:
//  lpstEnv         Ptr to W7H2_T structure
//  dwIDOffset      Offset of function name
//  nIDLen          Length of function name
//
// Description:
//  Searches for the function in the list of functions.  If found,
//  the function adds the function to the list of functions for running
//  if the function has not yet run.
//
// Returns:
//  Nothing
//
//*************************************************************************

void W7H2MarkFuncForRun
(
    LPW7H2              lpstEnv,
    DWORD               dwIDOffset,
    int                 nIDLen
)
{
    int                 nFunc;
    DWORD               dwFuncCRC;

    dwFuncCRC = W7H2CRCStr(lpstEnv,
                           dwIDOffset,
                           nIDLen);

    nFunc = W7H2GetFuncIndex(lpstEnv,dwFuncCRC);
    if (nFunc < 0)
        return;

    if (lpstEnv->abyRunState[nFunc] != lpstEnv->nRunState)
    {
        // The function hasn't run yet for this top level function,
        //  so queue it up

        lpstEnv->abyRunState[nFunc] = lpstEnv->nRunState;

        lpstEnv->abyCalledFuncs[lpstEnv->nNumCalledFuncs++] = nFunc;
    }
}


//*************************************************************************
//
// Function:
//  BOOL W7H2ScanFunc()
//
// Parameters:
//  lpstScan        Ptr to W7H2_SCAN_T structure
//  lpstEnv         Ptr to W7H2_T structure
//  dwOffset        Offset to scan from
//
// Description:
//  The function assumes that the given offset points to the first
//  operation of the function.  The function scans the given function
//  until either an End Sub or End Function is reached or when the
//  end of the macro is reached.
//
//  During scanning, the function also marks all called functions
//  so that they may be run subsequently to be included with the
//  hits of this function.
//
// Returns:
//  TRUE            On success
//  FALSE           On failure
//
//*************************************************************************

#define W7H2_SCAN_BOL           0
#define W7H2_SCAN_END           1
#define W7H2_SCAN_BOLID         2
#define W7H2_SCAN_BOLIDDLG      3
#define W7H2_SCAN_OTHERID       4
#define W7H2_SCAN_OTHERIDDLG    5
#define W7H2_SCAN_OTHER         6

#define W7H2_DLG_STATE_NONE     0
#define W7H2_DLG_STATE_DLG      1
#define W7H2_DLG_STATE_EQUAL    2
#define W7H2_DLG_STATE_PROCESS  3

BOOL W7H2ScanFunc
(
    LPW7H2_SCAN         lpstScan,
    LPW7H2              lpstEnv,
    DWORD               dwOffset
)
{
    DWORD               dwMaxOffset;
    int                 nState;
    BYTE                byToken;
    BYTE                byArg;
    WORD                wArg;
    DWORD               dwArgOffset;
    BOOL                bBackSlash;

    int                 nIDLen;
    DWORD               dwIDOffset;

    WORD                wDlg;
    WORD                wDlgValue;
    int                 nDlgState;

#ifdef SYM_NLM
    DWORD               dwIter = 0;
#endif // #ifdef SYM_NLM

    // Initialize for scan

    nState = W7H2_SCAN_BOL;
    nDlgState = W7H2_DLG_STATE_NONE;
    bBackSlash = FALSE;
    nIDLen = -1;
    dwMaxOffset = lpstEnv->dwSize;
    while (dwOffset < dwMaxOffset)
    {
#ifdef SYM_NLM
        if ((dwIter++ & 0x1FF) == 0)
        {
            // Relinquish control every 512th token

            SSProgress(lpstEnv->lpvRootCookie);
        }
#endif // #ifdef SYM_NLM

        byToken = W7H2GetBYTE(lpstEnv,dwOffset++);

        W7H2ScanOpHit(lpstScan,byToken);

        // Read BYTE and WORD parameters

        switch (byToken)
        {
            case WDOP_LABEL:
            case WDOP_PASCAL_STRING:
            case WDOP_QUOTED_PASCAL_STRING:
            case WDOP_COMMENT_PASCAL_STRING:
            case WDOP_MULTIPLE_SPACES:
            case WDOP_MULTIPLE_TABS:
            case WDOP_REM:
            case WDOP_EXTERNAL_MACRO:
            case WDOP_IDENTIFIER:
            case WDOP_UPASCAL_STRING_7B:
            case WDOP_UPASCAL_STRING:
                byArg = W7H2GetBYTE(lpstEnv,dwOffset++);
                break;

            case WDOP_WORD_VALUE_LABEL:
            case WDOP_FUNCTION_VALUE:
            case WDOP_UNSIGNED_WORD_VALUE:
            case WDOP_ASCII_CHARACTER:
            case WDOP_DIALOG_FIELD:
            case WDOP_QUOTED_UPASCAL_STRING:
                wArg = W7H2GetWORD(lpstEnv,dwOffset);
                dwOffset += 2;
                break;

            default:
                break;
        }

        // Calculate next token offset

        switch (byToken)
        {
            case WDOP_LABEL:
            case WDOP_PASCAL_STRING:
            case WDOP_QUOTED_PASCAL_STRING:
            case WDOP_COMMENT_PASCAL_STRING:
            case WDOP_REM:
            case WDOP_EXTERNAL_MACRO:
            case WDOP_IDENTIFIER:
                dwArgOffset = dwOffset;
                dwOffset += byArg;
                if (byToken == WDOP_QUOTED_PASCAL_STRING)
                {
                    LPBYTE  lpabyStr;

                    lpabyStr = W7H2GetByteArrayPtr(lpstEnv,
                                                   dwArgOffset - 1,
                                                   byArg + 1);

                    if (lpabyStr != NULL)
                        W7H2ScanStrHit(lpstScan,lpabyStr);
                }
                break;

            case WDOP_UPASCAL_STRING_7B:
            case WDOP_UPASCAL_STRING:
                dwArgOffset = dwOffset;
                dwOffset += byArg * sizeof(WORD);
                break;

            case WDOP_QUOTED_UPASCAL_STRING:
                dwArgOffset = dwOffset;
                dwOffset += wArg * (DWORD)sizeof(WORD);
                break;

            case WDOP_DOUBLE_VALUE:
                dwOffset += 8;
                break;

            // Skip white space

            case WDOP_SPACE:
            case WDOP_TAB:
            case WDOP_MULTIPLE_SPACES:
            case WDOP_MULTIPLE_TABS:
                continue;

            case WDOP_BACKSLASH:
                bBackSlash = TRUE;
                continue;

            case WDOP_COLON:
            case WDOP_NEWLINE:
                if (bBackSlash == TRUE)
                    bBackSlash = FALSE;
                else
                {
                    if (nState == W7H2_SCAN_BOLID ||
                        nState == W7H2_SCAN_OTHERID)
                    {
                        W7H2MarkFuncForRun(lpstEnv,
                                           dwIDOffset,
                                           nIDLen);
                    }
                    nState = W7H2_SCAN_BOL;

                    switch (nDlgState)
                    {
                        case W7H2_DLG_STATE_DLG:
                        case W7H2_DLG_STATE_EQUAL:
                            // Assume value of zero
                            W7H2ScanDlgHit(lpstScan,wDlg,0);
                            break;

                        case W7H2_DLG_STATE_PROCESS:
                            // Simple WORD value
                            W7H2ScanDlgHit(lpstScan,wDlg,wDlgValue);
                            break;

                        default:
                            break;
                    }

                    W7H2ScanDoLineSigs(lpstScan);
                    nDlgState = W7H2_DLG_STATE_NONE;
                }
                continue;

            default:
                break;
        }

        if (nDlgState != W7H2_DLG_STATE_NONE)
        {
            switch (byToken)
            {
                // Ignore white space

                case WDOP_SPACE:
                case WDOP_TAB:
                case WDOP_MULTIPLE_SPACES:
                case WDOP_MULTIPLE_TABS:
                case WDOP_BACKSLASH:
                case WDOP_COLON:
                case WDOP_NEWLINE:
                    break;

                default:
                    switch (nDlgState)
                    {
                        case W7H2_DLG_STATE_DLG:
                            if (byToken == WDOP_EQUAL)
                                nDlgState = W7H2_DLG_STATE_EQUAL;
                            else
                            {
                                // Assume a dlg field with no equal is zero

                                W7H2ScanDlgHit(lpstScan,wDlg,0);
                                nDlgState = W7H2_DLG_STATE_NONE;
                            }
                            break;
                        case W7H2_DLG_STATE_EQUAL:
                            if (byToken == WDOP_UNSIGNED_WORD_VALUE)
                            {
                                wDlgValue = wArg;
                                nDlgState = W7H2_DLG_STATE_PROCESS;
                            }
                            else
                            {
                                // Non-simple value

                                W7H2ScanDlgHit(lpstScan,wDlg,-1);
                                nDlgState = W7H2_DLG_STATE_NONE;
                            }
                            break;
                        case W7H2_DLG_STATE_PROCESS:
                            if (byToken == WDOP_COMMA)
                            {
                                // Was a simple WORD value

                                W7H2ScanDlgHit(lpstScan,wDlg,wDlgValue);
                                nDlgState = W7H2_DLG_STATE_NONE;
                            }
                            else
                            {
                                // Non-simple value

                                W7H2ScanDlgHit(lpstScan,wDlg,-1);
                                nDlgState = W7H2_DLG_STATE_NONE;
                            }
                            break;
                        default:
                            // This should never happen
                            break;
                    }
                    break;
            }
        }
        else
        if (byToken == WDOP_FUNCTION_VALUE)
        {
            W7H2ScanCmdHit(lpstScan,wArg);
        }
        else
        if (byToken == WDOP_DIALOG_FIELD)
        {
            wDlg = wArg;
            nDlgState = W7H2_DLG_STATE_DLG;
        }

        switch (nState)
        {
            case W7H2_SCAN_BOL:
                if (byToken == WDOP_PASCAL_STRING)
                {
                    nState = W7H2_SCAN_BOLID;
                    nIDLen = byArg;
                    dwIDOffset = dwArgOffset;
                }
                else
                if (byToken == WDOP_END)
                    nState = W7H2_SCAN_END;
                else
                    nState = W7H2_SCAN_OTHER;
                break;

            case W7H2_SCAN_END:
                if (byToken == WDOP_SUB || byToken == WDOP_FUNCTION)
                    return(TRUE);

                nState = W7H2_SCAN_OTHER;
                break;

            case W7H2_SCAN_BOLID:
                if (byToken == WDOP_EXTERNAL_MACRO)
                    nState = W7H2_SCAN_BOLIDDLG;
                else
                {
                    if (byToken != WDOP_EQUAL)
                    {
                        W7H2MarkFuncForRun(lpstEnv,
                                           dwIDOffset,
                                           nIDLen);
                    }

                    nState = W7H2_SCAN_OTHER;
                }
                break;

            case W7H2_SCAN_BOLIDDLG:
                nState = W7H2_SCAN_OTHER;
                break;

            case W7H2_SCAN_OTHER:
                if (byToken == WDOP_PASCAL_STRING)
                {
                    nState = W7H2_SCAN_OTHERID;
                    nIDLen = byArg;
                    dwIDOffset = dwArgOffset;
                }
                break;

            case W7H2_SCAN_OTHERID:
                if (byToken == WDOP_EXTERNAL_MACRO)
                    nState = W7H2_SCAN_OTHERIDDLG;
                else
                {
                    W7H2MarkFuncForRun(lpstEnv,
                                       dwIDOffset,
                                       nIDLen);

                    nState = W7H2_SCAN_OTHER;
                }
                break;

            case W7H2_SCAN_OTHERIDDLG:
                nState = W7H2_SCAN_OTHER;
                break;

            default:
                break;
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS W7H2DoScan()
//
// Parameters:
//  lpstStream              Ptr to WordDocument stream
//  lpstKey                 Ptr to key
//
// Description:
//  Performs a heuristic emulation for macro virus detection.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//  EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//********************************************************************

EXTSTATUS W7H2DoScan
(
    LPSS_STREAM             lpstStream,
    LPWD7ENCKEY             lpstKey
)
{
    EXTSTATUS               extStatus = EXTSTATUS_OK;

    int                     nHitIndex;
    LPW7H2                  lpstEnv;
    WORD                    w;
    int                     nFunc;
    int                     nCalledFunc;
    BOOL                    bActive;
    LPW7H2_SCAN             lpstScan;
    extern W7H2_SIG_SET_T   gstW7H2SigSet;

    // Create heuristic scanning environment

    lpstEnv = W7H2Create(lpstStream->lpstRoot->lpvRootCookie);

    if (lpstEnv == NULL)
        return(EXTSTATUS_MEM_ERROR);

    // Initialize TDT structure

    if (WD7InitTDTInfo(lpstStream,
                       lpstKey,
                       &lpstEnv->stTDTInfo) != WD7_STATUS_OK)
    {
        // Error getting TDT info

        W7H2Destroy(lpstEnv);
        return(EXTSTATUS_FILE_ERROR);
    }

    // Create heuristic scan structure

    lpstScan = W7H2ScanCreate(lpstStream->lpstRoot->lpvRootCookie,
                              &gstW7H2SigSet);
    if (lpstScan == NULL)
    {
        // Error creating heuristic scan structure

        W7H2Destroy(lpstEnv);
        return(EXTSTATUS_FILE_ERROR);
    }


    //////////////////////////////////////////////////////////////////
    // Iterate through macros
    //////////////////////////////////////////////////////////////////

    // Initialize for full scan

    if (W7H2Init(lpstEnv,
                 lpstStream,
                 lpstKey) == FALSE)
    {
        W7H2ScanDestroy(lpstScan);
        W7H2Destroy(lpstEnv);
        return(EXTSTATUS_MEM_ERROR);
    }

    W7H2ScanInit(lpstScan);

    for (w=0;w<lpstEnv->stTDTInfo.wNumMCDs;w++)
    {
        // Get the macro info

        if (WD7GetMacroInfoAtIndex(&lpstEnv->stTDTInfo,
                                   w,
                                   lpstEnv->abyName,
                                   &lpstEnv->dwOffset,
                                   &lpstEnv->dwSize,
                                   &lpstEnv->byEncryptByte,
                                   &bActive) == WD7_STATUS_ERROR ||
            bActive == FALSE)
        {
            // Error getting this macro or not an active macro,
            //  go to the next one

            continue;
        }

        // First get a CRC of the macro and check to see whether
        //  to exclude the macro from emulation

        if (WD7MacroBinaryCRC(lpstStream,
                              lpstKey,
                              lpstEnv->dwOffset,
                              lpstEnv->dwSize,
                              lpstEnv->byEncryptByte,
                              &lpstEnv->dwCRC) == WD7_STATUS_OK)
        {
            if (MacroIsExcluded(MACRO_EXCLUDE_TYPE_WD7,
                                lpstEnv->dwCRC,
                                lpstEnv->dwSize) == TRUE)
            {
                // Exclude this macro from emulation

                continue;
            }
        }

        //printf("M%02u: %s\n",w,lpstEnv->abyName);

        // Initialize run buf

        lpstEnv->dwRunBufOffset = 0;
        lpstEnv->dwRunBufEndOffset = 0;

        // Get the functions

        if (W7H2MacroGetFuncs(lpstEnv) == FALSE)
            continue;

        W7H2SortFuncs(lpstEnv);

        // Initialize the run state

        lpstEnv->nRunState = 0;

        // Do MAIN first, then everything else

        for (nFunc=0;nFunc<=lpstEnv->nNumFuncs;nFunc++)
        {
            lpstEnv->nRunState++;

            if (lpstEnv->abyRunState[nFunc] == 0)
            {
                // Mark as ran

                lpstEnv->abyRunState[nFunc] = (BYTE)lpstEnv->nRunState;
                lpstEnv->abyCalledFuncs[0] = nFunc;
                lpstEnv->nNumCalledFuncs = 1;

                // Now do called functions

                nCalledFunc = 0;
                while (nCalledFunc < lpstEnv->nNumCalledFuncs)
                {
                    W7H2ScanFunc(lpstScan,
                                 lpstEnv,
                                 lpstEnv->adwFuncOffset[lpstEnv->abyCalledFuncs[nCalledFunc]]);

                    ++nCalledFunc;
                }

                W7H2ScanDoFuncSigs(lpstScan);
            }
        }
    }

    nHitIndex = W7H2ScanDoSetSigs(lpstScan);

    W7H2ScanDestroy(lpstScan);

    W7H2Destroy(lpstEnv);

    //printf("\n\n");

    if (nHitIndex < gstW7H2SigSet.nNumSetSigs)
    {
        extStatus = EXTSTATUS_VIRUS_FOUND;
        //printf("****VID: 0x%04X\n",gstW7H2SetSig.lpawVID[nHitIndex]);
    }

    return(extStatus);
}


//************************************************************************
//
// Function:
//  EXTSTATUS W7H2Scan()
//
// Parameters:
//  lpstStream          Ptr to WordDocument stream
//
// Description:
//  Scans a stream for the presence of Word 6.0/7.0/95 Macro viruses
//  using Level II analysis.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//  EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//************************************************************************

EXTSTATUS W7H2Scan
(
    LPSS_STREAM         lpstStream
)
{
    EXTSTATUS           extStatus = EXTSTATUS_OK;
    WD7ENCKEY_T         stKey;

    /////////////////////////////////////////////////////////////
    // Initialize for Word 6.0/7.0/95 scanning
    /////////////////////////////////////////////////////////////

	// Verify that it is a Word file and that it has macros

    if (WD7FindFirstKey(lpstStream,
                        &stKey,
                        &gstRevKeyLocker) != WD7_STATUS_OK)
    {
        // Failure to find key or determining whether it
        //  was encrypted

        return(EXTSTATUS_OK);
    }


    /////////////////////////////////////////////////////////////
    // Iterate on scanning as long as a key is found
    /////////////////////////////////////////////////////////////

    while (1)
    {
        // Call off to heuristic scan

        extStatus = W7H2DoScan(lpstStream,
                                  &stKey);

        if (extStatus != EXTSTATUS_OK)
            break;

        if (stKey.bEncrypted == FALSE)
        {
            // If not encrypted, then just scan once

            break;
        }

        // Try to find another key

        if (WD7FindNextKey(lpstStream,
                           &stKey,
                           &gstRevKeyLocker) != WD7_STATUS_OK)
            break;
	}

    return (extStatus);
}

#endif // #ifdef MACROHEU

