//************************************************************************
//
// $Header:   S:/NAVEX/VCS/COPY.CPv   1.5   09 Dec 1998 17:45:16   DCHI  $
//
// Description:
//  Contains heuristic macro virus detection source.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/COPY.CPv  $
// 
//    Rev 1.5   09 Dec 1998 17:45:16   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.4   12 Oct 1998 13:40:44   DCHI
// Added CopyIsEmpty() and import/export support.
// 
//    Rev 1.3   08 Oct 1997 11:43:40   DCHI
// Modified CopyLogCopy() to log copies of the form:
// Source="*" Destination="*:" (e.g., dst takes name of source).
// 
//    Rev 1.2   04 Aug 1997 18:53:44   DCHI
// Changed CopyLogCopy() to not log if either name is empty.
// 
//    Rev 1.1   09 Jul 1997 17:12:58   DCHI
// Added #ifdef MACROHEU around entire source.
// 
//    Rev 1.0   09 Jul 1997 16:14:38   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#ifdef MACROHEU

#include "olessapi.h"
#include "wbutil.h"
#include "copy.h"

//********************************************************************
//
// LPCOPY CopyCreate()
//
// Parameters:
//  lpvRootCookie   Root cookie for SSMemoryAlloc() call
//
// Description:
//  Allocates memory for a copy structure.
//
// Returns:
//  LPCOPY          Ptr to allocated copy structure
//  NULL            On error allocating memory
//
//********************************************************************

LPCOPY CopyCreate
(
    LPVOID      lpvRootCookie
)
{
    LPCOPY      lpstCopy;

    if (SSMemoryAlloc(lpvRootCookie,
                      sizeof(COPY_T),
                      (LPLPVOID)&lpstCopy) != SS_STATUS_OK)
        return(NULL);

    return(lpstCopy);
}


//********************************************************************
//
// BOOL CopyDestroy()
//
// Parameters:
//  lpvRootCookie   Root cookie for SSMemoryFree() call
//
// Description:
//  Frees memory allocated memory for a copy structure.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL CopyDestroy
(
    LPVOID      lpvRootCookie,
    LPCOPY      lpstCopy
)
{
    if (SSMemoryFree(lpvRootCookie,
                     lpstCopy) != SS_STATUS_OK)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL CopyInit()
//
// Parameters:
//  lpstCopy        Ptr to allocated copy structure
//
// Description:
//  Initializes the copy structure for usage.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

BOOL CopyInit
(
    LPCOPY      lpstCopy
)
{
    lpstCopy->wNameBufUsed = 0;
    lpstCopy->wNumNames = 0;
    lpstCopy->wNumMacroPairs = 0;

    return(TRUE);
}


//********************************************************************
//
// WORD CopyAddName()
//
// Parameters:
//  lpstCopy        Ptr to allocated copy structure
//  lpbyMacroName   Name to add
//
// Description:
//  Adds a new name to the list of names.
//
// Returns:
//  WORD            Index of the added name
//  0xFFFF          If there is no more room to add the name
//
//********************************************************************

WORD CopyAddName
(
    LPCOPY      lpstCopy,
    LPBYTE      lpbyMacroName
)
{
    LPBYTE      lpbyName;
    WORD        wIdx;
    int         i;
    BYTE        by;

    if (lpstCopy->wNameBufUsed + lpbyMacroName[0] + 1 >
        COPY_NAME_BUF_SIZE)
        return(0xFFFF);

    if (lpstCopy->wNumNames >= COPY_MAX_NAMES)
        return(0xFFFF);

    wIdx = lpstCopy->wNumNames++;
    lpstCopy->wNameOffsets[wIdx] = lpstCopy->wNameBufUsed;
    lpstCopy->wNameBufUsed += lpbyMacroName[0] + 1;
    lpbyName = lpstCopy->abyNameBuf + lpstCopy->wNameOffsets[wIdx];
    for (i=1;i<=lpbyMacroName[0];i++)
    {
        by = lpbyMacroName[i];
        if (by <= 'Z' && by >= 'A')
            by += ('a' - 'A');

        lpbyName[i] = by;
    }

    lpbyName[0] = lpbyMacroName[0];

    return(wIdx);
}


//********************************************************************
//
// WORD CopyGetNameIndex()
//
// Parameters:
//  lpstCopy        Ptr to allocated copy structure
//  lpbyMacroName   Name to add or check for
//  bAdd            BOOL set to TRUE if the name should be added
//                   if not yet present
//
// Description:
//  Adds a new name to the list of names if the name is not already
//  in the list.
//
// Returns:
//  WORD            Index of the added name or existing name
//  0xFFFF          If there is no more room to add the name
//
//********************************************************************

WORD CopyGetNameIndex
(
    LPCOPY      lpstCopy,
    LPBYTE      lpbyMacroName,
    BOOL        bAdd
)
{
    int         i;
    WORD        w;
    WORD        wIdx;
    LPBYTE      lpbyName;
    BYTE        by;

    // Search for indexes of names

    for (w=0;w<lpstCopy->wNumNames;w++)
    {
        lpbyName = lpstCopy->abyNameBuf + lpstCopy->wNameOffsets[w];
        if (lpbyName[0] == lpbyMacroName[0])
        {
            for (i=1;i<=lpbyName[0];i++)
            {
                by = lpbyMacroName[i];
                if (by <= 'Z' && by >= 'A')
                    by += ('a' - 'A');

                if (lpbyName[i] != by)
                    break;
            }

            if (i > lpbyName[0])
                return(w);
        }
    }

    if (bAdd == FALSE)
        return(0xFFFF);

    // Add the name

    wIdx = CopyAddName(lpstCopy,lpbyMacroName);
    return(wIdx);
}


//********************************************************************
//
// BOOL CopyAddCopy()
//
// Parameters:
//  lpstCopy            Ptr to allocated copy structure
//  lpbySrcMacroName    Ptr to source macro name
//  lpbyDstMacroName    Ptr to destination macro name
//  wFlags              Flags indicating direction of copy
//
// Description:
//  Adds the pair to the list of copies.  If the pair already
//  exists, only the flags are updated if needed.
//
// Returns:
//  TRUE                On success
//  FALSE               On error (i.e., no more room)
//
//********************************************************************

BOOL CopyAddCopy
(
    LPCOPY      lpstCopy,
    LPBYTE      lpbySrcMacroName,
    LPBYTE      lpbyDstMacroName,
    WORD        wFlags
)
{
    WORD        w;
    WORD        wIdxLocal, wIdxGlobal;

    // Search for indexes of names

    if (wFlags == COPY_FLAG_LOCAL_TO_GLOBAL)
    {
        wIdxLocal = CopyGetNameIndex(lpstCopy,
                                     lpbySrcMacroName,
                                     TRUE);

        wIdxGlobal = CopyGetNameIndex(lpstCopy,
                                      lpbyDstMacroName,
                                      TRUE);
    }
    else
    if (wFlags == COPY_FLAG_GLOBAL_TO_LOCAL)
    {
        wIdxGlobal = CopyGetNameIndex(lpstCopy,
                                      lpbySrcMacroName,
                                      TRUE);

        wIdxLocal = CopyGetNameIndex(lpstCopy,
                                     lpbyDstMacroName,
                                     TRUE);
    }

    // Add the pair

    for (w=0;w<lpstCopy->wNumMacroPairs;w++)
    {
        if (wIdxLocal == lpstCopy->astMacroPairs[w].wIdxLocal &&
            wIdxGlobal == lpstCopy->astMacroPairs[w].wIdxGlobal)
        {
            lpstCopy->astMacroPairs[w].wFlags |= wFlags;
            return(TRUE);
        }
    }

    if (lpstCopy->wNumMacroPairs >= COPY_MAX_PAIRS)
        return(FALSE);

    w = lpstCopy->wNumMacroPairs++;
    lpstCopy->astMacroPairs[w].wFlags = wFlags;
    lpstCopy->astMacroPairs[w].wIdxLocal = wIdxLocal;
    lpstCopy->astMacroPairs[w].wIdxGlobal = wIdxGlobal;

    return(TRUE);
}


//********************************************************************
//
// BOOL CopyLogCopy()
//
// Parameters:
//  lpstCopy            Ptr to allocated copy structure
//  lpbySrc             Ptr to qualified source name
//  lpbyDst             Ptr to qualified destination name
//
// Description:
//  Adds the pair to the list of copies.  If the pair already
//  exists, only the flags are updated if needed.  The function
//  checks the prefix to determine which is the source and which
//  is the destination.  If the source is local and the destination
//  is local or the source is global and the destination is global,
//  the function does not log the pair.
//
//  If either macroname is NULL, the copy is also not logged.
//
// Returns:
//  TRUE                On success
//  FALSE               On error (i.e., no more room)
//
//********************************************************************

BOOL CopyLogCopy
(
    LPCOPY      lpstCopy,
    LPBYTE      lpbySrc,
    LPBYTE      lpbyDst
)
{
    BOOL        bSrcIsGlobal;
    BOOL        bDstIsGlobal;
    int         i;
    int         nSrcColonIdx;
    int         nDstColonIdx;
    WORD        wFlags;
    BOOL        bResult;

    // Check the src

    // Search for the colon, backwards

    for (i=lpbySrc[0];i>0;i--)
        if (lpbySrc[i] == ':')
            break;

    if (i <= 0)
    {
        // It is global

        bSrcIsGlobal = TRUE;
        nSrcColonIdx = 0;
    }
    else
    {
        nSrcColonIdx = i;

        // See if it is either NORMAL.DOT, NORMAL, or Global

        if (PascalIsGlobalMacroName(lpbySrc) == FALSE)
            bSrcIsGlobal = FALSE;
        else
            bSrcIsGlobal = TRUE;
    }

    // Check the dst

    // Search for the colon

    for (i=lpbyDst[0];i>0;i--)
        if (lpbyDst[i] == ':')
            break;

    if (i <= 0)
    {
        // It is global

        bDstIsGlobal = TRUE;
        nDstColonIdx = 0;
    }
    else
    {
        nDstColonIdx = i;

        // See if it is either NORMAL.DOT, NORMAL, or Global

        if (PascalIsGlobalMacroName(lpbyDst) == FALSE)
            bDstIsGlobal = FALSE;
        else
            bDstIsGlobal = TRUE;
    }

    if (bSrcIsGlobal == FALSE && bDstIsGlobal == TRUE)
        wFlags = COPY_FLAG_LOCAL_TO_GLOBAL;
    else
    if (bSrcIsGlobal == TRUE && bDstIsGlobal == FALSE)
        wFlags = COPY_FLAG_GLOBAL_TO_LOCAL;
    else
        return(TRUE);

    if (nSrcColonIdx != 0)
        lpbySrc[nSrcColonIdx] = lpbySrc[0] - nSrcColonIdx;

    if (nDstColonIdx != 0)
        lpbyDst[nDstColonIdx] = lpbyDst[0] - nDstColonIdx;

    // If the source name is NULL, don't log it

    if (lpbySrc[nSrcColonIdx] == 0)
        bResult = TRUE;
    else
    {
        // We definitely have a source name at this point

        if (lpbyDst[nDstColonIdx] == 0)
        {
            // If the destination name is NULL, don't log it

            if (nDstColonIdx == 0)
                bResult = TRUE;
            else
            {
                // Destination name is the same as the source name

                bResult = CopyAddCopy(lpstCopy,
                                      lpbySrc + nSrcColonIdx,
                                      lpbySrc + nSrcColonIdx,
                                      wFlags);
            }
        }
        else
        {
            bResult = CopyAddCopy(lpstCopy,
                                  lpbySrc + nSrcColonIdx,
                                  lpbyDst + nDstColonIdx,
                                  wFlags);
        }
    }

    if (nSrcColonIdx != 0)
        lpbySrc[nSrcColonIdx] = ':';

    if (nDstColonIdx != 0)
        lpbyDst[nDstColonIdx] = ':';

    return(bResult);
}


//********************************************************************
//
// Function:
//  BOOL CopyLogExport()
//
// Parameters:
//  lpstCopy            Ptr to allocated copy structure
//  wFlags              Flags indicating direction of export
//  lpabypsFileName     Ptr to name of destination file
//  lpabypsModuleName   Ptr to name of module exported
//
// Description:
//  Adds the pair consisting of the export of the given module
//  to the given file.
//
//  The local field of the pair is used to contain the module
//  name and the global field is used to contain the filename.
//
//  The addition of the pair information may affect more than
//  one existing pair.  Here are the possiblities:
//
//  1. There is an existing pair with the same module and file name:
//      Add the passed in flag to the existing pair.
//  2. There is NO existing pair with the same file name:
//      Add a new pair with the passed in flag.
//  3. There IS an existing pair with the same file name, but
//     a different non-empty module name:
//      Add a new pair with the passed in flag.
//      Add the IMPORT flags of the existing pair.
//  4. There IS an existing pair with the same file name, but
//     an empty module name:
//      Add the module name to the existing pair and add
//      the passed in flag.
//
// Returns:
//  TRUE                On success
//  FALSE               On error (i.e., no more room)
//
//********************************************************************

BOOL CopyLogExport
(
    LPCOPY      lpstCopy,
    WORD        wFlags,
    LPBYTE      lpabypsFileName,
    LPBYTE      lpabypsModuleName
)
{
    WORD        wIdxFileName;
    WORD        wIdxModuleName;
    WORD        wIdxLocal;
    WORD        w;
    BOOL        bAdded;
    WORD        wImportFlags;

    // Get indexes for the filename and the module name

    wIdxFileName = CopyGetNameIndex(lpstCopy,
                                    lpabypsFileName,
                                    TRUE);

    wIdxModuleName = CopyGetNameIndex(lpstCopy,
                                      lpabypsModuleName,
                                      TRUE);

    if (wIdxFileName == 0xFFFF || wIdxModuleName == 0xFFFF)
        return(FALSE);

    // Add it

    bAdded = FALSE;
    wImportFlags = 0;
    for (w=0;w<lpstCopy->wNumMacroPairs;w++)
    {
        if (wIdxFileName == lpstCopy->astMacroPairs[w].wIdxGlobal)
        {
            wIdxLocal = lpstCopy->astMacroPairs[w].wIdxLocal;

            if (wIdxModuleName == wIdxLocal)
            {
                // A pair already exists with the same file name
                //  and module name

                // Add the passed in flag to the existing pair

                lpstCopy->astMacroPairs[w].wFlags |= wFlags;

                // No need to add

                bAdded = TRUE;
            }
            else
            if (wIdxLocal != 0xFFFF)
            {
                // A pair exists with the same file name,
                //  but the non-empty module name does not match

                // Add a new pair with the import flags of this pair

                wImportFlags |= lpstCopy->astMacroPairs[w].wFlags &
                    (COPY_FLAG_IMPORT_TO_LOCAL | COPY_FLAG_IMPORT_TO_GLOBAL);
            }
            else
            {
                // A pair exists with the same file name
                //  and the module name is empty

                // Add the module name and flags to this pair

                lpstCopy->astMacroPairs[w].wIdxLocal = wIdxModuleName;
                lpstCopy->astMacroPairs[w].wFlags |= wFlags;

                // No need to add

                bAdded = TRUE;
            }
        }
    }

    if (bAdded == FALSE)
    {
        if (lpstCopy->wNumMacroPairs >= COPY_MAX_PAIRS)
            return(FALSE);

        w = lpstCopy->wNumMacroPairs++;
        lpstCopy->astMacroPairs[w].wFlags = wFlags | wImportFlags;
        lpstCopy->astMacroPairs[w].wIdxLocal = wIdxModuleName;
        lpstCopy->astMacroPairs[w].wIdxGlobal = wIdxFileName;
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL CopyLogImport()
//
// Parameters:
//  lpstCopy            Ptr to allocated copy structure
//  wFlags              Flags indicating direction of export
//  lpabypsFileName     Ptr to name of source file
//
// Description:
//  Adds the import filename.
//
//  The local field of the pair is used to contain the module
//  name and the global field is used to contain the filename.
//
//  The addition of the pair information may affect more than
//  one existing pair.  Here are the possiblities:
//
//  1. There is an existing pair with the same file name:
//      Add the passed in flag to the existing pair.
//  2. There is NO existing pair with the same file name:
//      Add a new pair with the passed in flag.
//
// Returns:
//  TRUE                On success
//  FALSE               On error (i.e., no more room)
//
//********************************************************************

BOOL CopyLogImport
(
    LPCOPY      lpstCopy,
    WORD        wFlags,
    LPBYTE      lpabypsFileName
)
{
    WORD        wIdxFileName;
    WORD        w;
    BOOL        bAdded;

    // Get indexes for the filename and the module name

    wIdxFileName = CopyGetNameIndex(lpstCopy,
                                    lpabypsFileName,
                                    TRUE);

    if (wIdxFileName == 0xFFFF)
        return(FALSE);

    // Add it

    bAdded = FALSE;
    for (w=0;w<lpstCopy->wNumMacroPairs;w++)
    {
        if (wIdxFileName == lpstCopy->astMacroPairs[w].wIdxGlobal)
        {
            // Add the passed in flag to the existing pair

            lpstCopy->astMacroPairs[w].wFlags |= wFlags;

            // No need to add

            bAdded = TRUE;
        }
    }

    if (bAdded == FALSE)
    {
        if (lpstCopy->wNumMacroPairs >= COPY_MAX_PAIRS)
            return(FALSE);

        w = lpstCopy->wNumMacroPairs++;
        lpstCopy->astMacroPairs[w].wFlags = wFlags;
        lpstCopy->astMacroPairs[w].wIdxLocal = 0xFFFF;
        lpstCopy->astMacroPairs[w].wIdxGlobal = wIdxFileName;
    }

    return(TRUE);
}

/*
void CopyPrint
(
    LPCOPY          lpstCopy
)
{
    WORD            w;
    int             i, nLen;
    LPBYTE          lpbyStr;
    LPMACRO_PAIR    lpstPair;

    // Print local to global copies

    printf("Macros copied from local to global:\n");

    lpstPair = lpstCopy->astMacroPairs;
    for (w=0;w<lpstCopy->wNumMacroPairs;w++)
    {
        if ((lpstPair->wFlags & COPY_FLAG_LOCAL_TO_GLOBAL) ||
            ((lpstPair->wFlags & COPY_FLAG_EXPORT_FROM_LOCAL) &&
             (lpstPair->wFlags & COPY_FLAG_IMPORT_TO_GLOBAL)))
        {
            putchar(' ');
            lpbyStr = lpstCopy->abyNameBuf +
                lpstCopy->wNameOffsets[lpstPair->wIdxLocal];
            nLen = *lpbyStr++;
            for (i=0;i<nLen;i++)
                putchar(lpbyStr[i]);

            for (;i<32;i++)
                putchar(' ');

            lpbyStr = lpstCopy->abyNameBuf +
                lpstCopy->wNameOffsets[lpstPair->wIdxGlobal];
            nLen = *lpbyStr++;
            for (i=0;i<nLen;i++)
                putchar(lpbyStr[i]);

            putchar('\n');
        }

        ++lpstPair;
    }

    putchar('\n');

    printf("Macros copied to local from global:\n");

    lpstPair = lpstCopy->astMacroPairs;
    for (w=0;w<lpstCopy->wNumMacroPairs;w++)
    {
        if ((lpstPair->wFlags & COPY_FLAG_GLOBAL_TO_LOCAL) |
            ((lpstPair->wFlags & COPY_FLAG_EXPORT_FROM_GLOBAL) &&
             (lpstPair->wFlags & COPY_FLAG_IMPORT_TO_LOCAL)))
        {
            putchar(' ');
            lpbyStr = lpstCopy->abyNameBuf +
                lpstCopy->wNameOffsets[lpstPair->wIdxLocal];
            nLen = *lpbyStr++;
            for (i=0;i<nLen;i++)
                putchar(lpbyStr[i]);

            for (;i<32;i++)
                putchar(' ');

            lpbyStr = lpstCopy->abyNameBuf +
                lpstCopy->wNameOffsets[lpstPair->wIdxGlobal];
            nLen = *lpbyStr++;
            for (i=0;i<nLen;i++)
                putchar(lpbyStr[i]);

            putchar('\n');
        }

        ++lpstPair;
    }
}
*/

//********************************************************************
//
// BOOL CopyIsViral()
//
// Parameters:
//  lpstCopy            Ptr to allocated copy structure
//
// Description:
//  Iterates through the list of pairs of names.  If any pair
//  has both the COPY_FLAG_GLOBAL_TO_LOCAL and the
//  COPY_FLAG_LOCAL_TO_GLOBAL flag set, the function returns
//  TRUE, signaling a viral set.  Otherwise, the function
//  returns FALSE.
//
//  The other condition is if there is either a global or a local
//  export of a module to a given file that is the source
//  of both a global and a local import.
//
// Returns:
//  TRUE                If the set is considered viral
//  FALSE               If the set is not considered viral
//
//********************************************************************

BOOL CopyIsViral
(
    LPCOPY          lpstCopy
)
{
    WORD            w;
    WORD            wFlags;
    LPMACRO_PAIR    lpstPair;

    lpstPair = lpstCopy->astMacroPairs;
    for (w=0;w<lpstCopy->wNumMacroPairs;w++)
    {
        wFlags = lpstPair->wFlags;

        if ((wFlags & COPY_FLAG_GLOBAL_TO_LOCAL) != 0 &&
            (wFlags & COPY_FLAG_LOCAL_TO_GLOBAL) != 0)
        {
            return(TRUE);
        }

        // Check for import/export criteria

        if ((wFlags & (COPY_FLAG_IMPORT_TO_GLOBAL |
                       COPY_FLAG_IMPORT_TO_LOCAL)) ==
            (COPY_FLAG_IMPORT_TO_GLOBAL |
             COPY_FLAG_IMPORT_TO_LOCAL) &&
            (wFlags & (COPY_FLAG_EXPORT_FROM_GLOBAL |
                      COPY_FLAG_EXPORT_FROM_LOCAL)) != 0)
        {
            return(TRUE);
        }

        ++lpstPair;
    }

    return(FALSE);
}


//********************************************************************
//
// BOOL CopyIsPartOfViralSet()
//
// Parameters:
//  lpstCopy            Ptr to allocated copy structure
//  lpbyMacroName       Name to search for
//
// Description:
//  If the given name is a member of a pair that has copies
//  in both directions or is a direct source or destination of
//  one that does satisfy the condition, the function assumes
//  that the given name is part of the viral set.
//
//  The other condition is if there is either a global or a local
//  export of a module to a given file that is the source
//  of both a global and a local import.
//
// Returns:
//  TRUE                If the name is part of the viral set
//  FALSE               If the name is not part of the viral set
//
//********************************************************************

BOOL CopyIsPartOfViralSet
(
    LPCOPY      lpstCopy,
    LPBYTE      lpbyMacroName
)
{
    WORD            wFlags;
    WORD            w, wIdx, w2, wPartnerIdx;
    LPMACRO_PAIR    lpstPair, lpstPair2;

    wIdx = CopyGetNameIndex(lpstCopy,lpbyMacroName,FALSE);

    if (wIdx == 0xFFFF)
    {
        // Definitely not part of viral set

        return(FALSE);
    }

    lpstPair = lpstCopy->astMacroPairs;
    for (w=0;w<lpstCopy->wNumMacroPairs;w++)
    {
        if (wIdx == lpstPair->wIdxLocal || wIdx == lpstPair->wIdxGlobal)
        {
            wFlags = lpstPair->wFlags;

            if ((wFlags & COPY_FLAG_GLOBAL_TO_LOCAL) != 0 &&
                (wFlags & COPY_FLAG_LOCAL_TO_GLOBAL) != 0)
            {
                // Definitely part of viral set

                return(TRUE);
            }

            // Check for import/export criteria

            if (wIdx == lpstPair->wIdxLocal &&
                (wFlags & (COPY_FLAG_IMPORT_TO_GLOBAL |
                           COPY_FLAG_IMPORT_TO_LOCAL)) ==
                (COPY_FLAG_IMPORT_TO_GLOBAL |
                 COPY_FLAG_IMPORT_TO_LOCAL) &&
                 (wFlags & (COPY_FLAG_EXPORT_FROM_GLOBAL |
                            COPY_FLAG_EXPORT_FROM_LOCAL)) != 0)
            {
                // Definitely part of viral set

                return(TRUE);
            }

            // Check to see whether the partner is viral
            //  to a depth of one

            if (wIdx == lpstPair->wIdxLocal)
                wPartnerIdx = lpstPair->wIdxGlobal;
            else
                wPartnerIdx = lpstPair->wIdxLocal;

            lpstPair2 = lpstCopy->astMacroPairs;
            for (w2=0;w2<lpstCopy->wNumMacroPairs;w2++,lpstPair2++)
            {
                if (w2 == w)
                    continue;

                if ((lpstPair2->wFlags & COPY_FLAG_GLOBAL_TO_LOCAL) != 0 &&
                    (lpstPair2->wFlags & COPY_FLAG_LOCAL_TO_GLOBAL) != 0 &&
                    (wPartnerIdx == lpstPair2->wIdxLocal ||
                     wPartnerIdx == lpstPair2->wIdxGlobal))
                {
                    return(TRUE);
                }
            }
        }

        ++lpstPair;
    }

    return(FALSE);
}


//********************************************************************
//
// BOOL CopyIsEmpty()
//
// Parameters:
//  lpstCopy        Ptr to allocated copy structure
//
// Description:
//  Iterates through the copy list.  If it is empty, the function
//  returns TRUE.  Otherwise it returns FALSE.
//
// Returns:
//  TRUE            If the copy list is empty
//  FALSE           If the copy list is not empty
//
//********************************************************************

BOOL CopyIsEmpty
(
    LPCOPY          lpstCopy
)
{
    if (lpstCopy->wNumMacroPairs == 0)
        return(TRUE);

    return(FALSE);
}

#endif // #ifdef MACROHEU

