// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/PATHNAME.CPv   1.3   12 Aug 1996 18:46:54   DCHI  $
//
// Description:
//  Functions for working with pathnames.
//
// Contains:
//  PathNameGetName()
//  PathNameHasWildCard()
//  PathNameIsDirectory()
//  PathNameIsVolume()
//  PathNameChangeExtension()
//  PathNameSplit()
//  PathNameMerge()
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/PATHNAME.CPv  $
// 
//    Rev 1.3   12 Aug 1996 18:46:54   DCHI
// Modifications for UNIX.
// 
//    Rev 1.2   04 Jun 1996 18:15:00   DCHI
// Modifications to conform to new header file structure.
// 
//    Rev 1.1   03 Jun 1996 17:21:06   DCHI
// Changes to support building on BORLAND, VC20, SYM_W32, SYM_DOS, and SYM_DOSX.
// 
//    Rev 1.0   21 May 1996 12:25:32   DCHI
// Initial revision.
// 
//************************************************************************

#include <stdio.h>
#include <string.h>

#include "avtypes.h"
#include "filefind.h"
#include "pathname.h"

//*************************************************************************
//
// Function:
//  PathNameGetName()
//
// Parameters:
//  lpstrPathName   Path name from which to separate name
//  lpstrName       Buffer to store name
//
// Description:
//  Looks for the name part of a pathname and copies it to lpstrName.
//  The first character of the name is replaced with a '\0'.
//
//  The function searches backwards for the first forward slash,
//  back slash, or volume separator.  The character after the found
//  directory or volume separator is assumed to be the first character
//  of the name.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void PathNameGetName
(
    LPSTR           lpstrPathName,
    LPSTR           lpstrName
)
{
    LPSTR           pszColon;
    LPSTR           pszBackSlash;
    LPSTR           pszForwardSlash;
    LPSTR           pszName;

    pszColon = strrchr(lpstrPathName,':');
    pszBackSlash = strrchr(lpstrPathName,'\\');
    pszForwardSlash = strrchr(lpstrPathName,'/');

    pszName = pszColon;
    if (pszColon == NULL ||
        pszBackSlash != NULL && pszBackSlash > pszColon)
        pszName = pszBackSlash;

    if (pszName == NULL ||
        pszForwardSlash != NULL && pszForwardSlash > pszName)
        pszName = pszForwardSlash;

    if (pszName == NULL)
    {
        if (lpstrName != NULL)
            strcpy(lpstrName,lpstrPathName);
        lpstrPathName[0] = '\0';
    }
	else
    {
        if (lpstrName != NULL)
            strcpy(lpstrName,pszName+1);
       	*(pszName+1) = '\0';
	}
}


//*************************************************************************
//
// Function:
//  PathNameHasWildCard()
//
// Parameters:
//  lpstrPathName   Path name from which to look for wildcard characters
//
// Description:
//  Searches from beginning to end for the presence of either '*'
//  or '?'.
//
// Returns:
//  FALSE       If the string has no wildcard characters
//  TRUE        If the string does have wildcard characters
//
//*************************************************************************

BOOL PathNameHasWildCard
(
    LPSTR           lpstrPathName
)
{
    while (*lpstrPathName != '\0')
    {
        if (*lpstrPathName == '*' ||
            *lpstrPathName == '?')
            return(TRUE);

        ++lpstrPathName;
    }

    return(FALSE);
}


//*************************************************************************
//
// Function:
//  PathNameIsDirectory()
//
// Parameters:
//  lpstrPathName   Path name for directory determination
//
// Description:
//  Performs a FindFirstFile() on lpstrPathName and examines the
//  attributes if found to determine directory status of path name.
//
// Returns:
//  FALSE       If the path name is not a directory
//  TRUE        If the path name is a directory
//
//*************************************************************************

BOOL PathNameIsDirectory
(
    LPSTR           lpstrPathName
)
{
    FILEFINDDATA_T  stFindData;

    if (FindFirstFile(lpstrPathName,&stFindData) == FALSE)
        return(FALSE);

    CloseFileFind(&stFindData);

    if (stFindData.dwFileAttributes & FF_ATTRIB_DIREC)
        return(TRUE);

    return(FALSE);
}


//*************************************************************************
//
// Function:
//  PathNameIsVolume()
//
// Parameters:
//  lpstrPathName   Path name for volume determination
//
// Description:
//  Determines whether the string represents a only volume.
//
// Returns:
//  FALSE       If the path name is not a volume
//  TRUE        If the path name is a volume
//
//*************************************************************************

BOOL PathNameIsVolume
(
    LPSTR           lpstrPathName
)
{
    // Does it look like a drive letter?

	if (lpstrPathName[0] != '\0' && lpstrPathName[1] != '\0' &&
    	lpstrPathName[2] == '\0' && lpstrPathName[1] == ':' &&
        (lpstrPathName[0] >= 'A' && lpstrPathName[0] <= 'Z' ||
         lpstrPathName[0] >= 'a' && lpstrPathName[0] <= 'z'))
        return(TRUE);

    return(FALSE);
}


//*************************************************************************
//
// Function:
//  PathNameChangeExtension()
//
// Parameters:
//  lpstrPathName       Path name for directory determination
//  lpstrNewExtension   New extension
//
// Description:
//  Changes the extension of lpstrPathName to lpstrNewExtension.
//
//  The function does this by searching backwards for the first '.'.
//  If a directory separator or a volume separator is found before
//  the first '.', then the extension is simply appended to the end.
//
//  Otherwise, if a '.' is found first, the extension is appended
//  at that location.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void PathNameChangeExtension
(
    LPSTR           lpstrPathName,
    LPSTR           lpstrNewExtension
)
{
    int             i, nLen;

    nLen = strlen(lpstrPathName);

    if (nLen > 0)
    {
        // Go backwards to find a period, but
        //  if we see either a forward slash, backward slash, or colon
        //  first then just append to the end

        for (i=nLen-1;i>=0;i--)
        {
            if (lpstrPathName[i] == '.')
                break;

            if (lpstrPathName[i] == '/' ||
                lpstrPathName[i] == '\\' ||
                lpstrPathName[i] == ':')
            {
                i = nLen;
                break;
            }
        }
    }
    else
        i = 0;

    // Change the extension

    sprintf(lpstrPathName+i,".%s",lpstrNewExtension);
}


//*************************************************************************
//
// Function:
//  PathNameSplit()
//
// Parameters:
//  lpstrPathName       Path name for directory determination
//  lpstrVolume         Buffer for the volume or NULL
//  lpstrDir            Buffer for the directory or NULL
//  lpstrName           Buffer for the name or NULL
//  lpstrExt            Buffer for the extension or NULL
//
// Description:
//  Takes a path name and stores its constituent parts into the
//  corresponding buffers that are non-NULL.
//
// Returns:
//  Nothing.
//
//*************************************************************************

int PathNameSplit
(
    LPSTR           lpstrPathName,
    LPSTR           lpstrVolume,
    LPSTR           lpstrDir,
    LPSTR           lpstrName,
    LPSTR           lpstrExt
)
{
    int             nContents;

#ifdef BORLAND
    nContents = fnsplit(lpstrPathName,
                        lpstrVolume,
                        lpstrDir,
                        lpstrName,
                        lpstrExt);
#endif // BORLAND

#if defined(VC20) || defined(SYM_WIN32) || \
    defined(SYM_DOS) || defined(SYM_DOSX)

    nContents = 0;

    _splitpath(lpstrPathName,lpstrVolume,lpstrDir,lpstrName,lpstrExt);

    if (lpstrVolume && *lpstrVolume != '\0')
        nContents |= PATHNAME_HAS_VOLUME;

    if (lpstrDir  && *lpstrDir != '\0')
        nContents |= PATHNAME_HAS_DIR;

    if (lpstrName && *lpstrName != '\0')
        nContents |= PATHNAME_HAS_NAME;

    if (lpstrExt && *lpstrExt != '\0')
        nContents |= PATHNAME_HAS_EXT;

#endif // VC20 || SYM_WIN32 || SYM_DOS || SYM_DOSX

#if defined(SYM_UNIX)
    int nStrLen = strlen(lpstrPathName);
    int nFirstSlashIndex;
    int nFirstPeriodIndex;
    int i;

    nContents = 0;
    nFirstSlashIndex = nFirstPeriodIndex = -1;
    for (i=nStrLen-1;i>=0;i--)
    {
        if (lpstrPathName[i] == '/' && nFirstSlashIndex == -1)
            nFirstSlashIndex = i;

        if (lpstrPathName[i] == '.' && nFirstPeriodIndex == -1)
            nFirstPeriodIndex = i;
    }

    if (lpstrExt && nFirstPeriodIndex != -1)
    {
        if (nFirstPeriodIndex > nFirstSlashIndex)
        {
            strcpy(lpstrExt,lpstrPathName+nFirstPeriodIndex);
            nContents |= PATHNAME_HAS_EXT;
        }
        else
            lpstrExt[0] = '\0';
    }

    if (lpstrName)
    {
        if (nFirstSlashIndex == -1)
        {
            // Copy from the beginning

            if (nFirstPeriodIndex == -1)
            {
                // No extension

                strcpy(lpstrName,lpstrPathName);
            }
            else
            {
                // Has extension

                strncpy(lpstrName,lpstrPathName,nFirstPeriodIndex);
                lpstrName[nFirstPeriodIndex] = '\0';
            }
        }
        else
        {
            // Has directory

            if (nFirstPeriodIndex == -1 ||
                nFirstPeriodIndex < nFirstSlashIndex)
            {
                // No extension

                strcpy(lpstrName,lpstrPathName+nFirstSlashIndex+1);
            }
            else
            {
                // Has extension

                strncpy(lpstrName,lpstrPathName+nFirstSlashIndex+1,
                    nFirstPeriodIndex-nFirstSlashIndex-1);
                lpstrName[nFirstPeriodIndex-nFirstSlashIndex-1] = '\0';
            }
        }

        if (lpstrName[0] != '\0')
            nContents |= PATHNAME_HAS_NAME;
    }

    if (lpstrDir)
    {
        if (nFirstSlashIndex != -1)
        {
            strncpy(lpstrDir,lpstrPathName,nFirstSlashIndex+1);
            lpstrDir[nFirstSlashIndex+1] = '\0';

            nContents |= PATHNAME_HAS_DIR;
        }
        else
            lpstrDir[0] = '\0';
    }

    if (lpstrVolume)
        lpstrVolume[0] = '\0';

#endif // SYM_UNIX

    return nContents;
}


//*************************************************************************
//
// Function:
//  PathNameMerge()
//
// Parameters:
//  lpstrPathName       Path name for directory determination
//  lpstrVolume         Buffer for the volume or NULL
//  lpstrDir            Buffer for the directory or NULL
//  lpstrName           Buffer for the name or NULL
//  lpstrExt            Buffer for the extension or NULL
//
// Description:
//  Combines the non-NULL parts of a path name into a full path name
//  and stores the result at lpstrPathName.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void PathNameMerge
(
    LPSTR           lpstrPathName,
    LPSTR           lpstrVolume,
    LPSTR           lpstrDir,
    LPSTR           lpstrName,
    LPSTR           lpstrExt
)
{
#ifdef BORLAND
    fnmerge(lpstrPathName,lpstrVolume,lpstrDir,lpstrName,lpstrExt);
#endif // BORLAND

#if defined(VC20) || defined(SYM_WIN32) || \
    defined(SYM_DOS) || defined(SYM_DOSX)

    _makepath(lpstrPathName,lpstrVolume,lpstrDir,lpstrName,lpstrExt);

#endif // VC20 || SYM_WIN32 || SYM_DOS || SYM_DOSX

#if defined(SYM_UNIX)

    strcpy(lpstrPathName,lpstrVolume);
    strcat(lpstrPathName,lpstrDir);
    strcat(lpstrPathName,lpstrName);
    strcat(lpstrPathName,lpstrExt);

#endif // SYM_UNIX
}

