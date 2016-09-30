// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/FILEFIND.CPv   1.3   12 Aug 1996 18:46:52   DCHI  $
//
// Description:
//  Functions for working with the file storage hierarchy.
//
// Contains:
//  FindFirstFile()
//  FindNextFile()
//  CloseFileFind()
//  ChangeVolume()
//  ChangeDirectory()
//  ChangeFullVolDir()
//  GetCurrentDirectory()
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/FILEFIND.CPv  $
// 
//    Rev 1.3   12 Aug 1996 18:46:52   DCHI
// Modifications for UNIX.
// 
//    Rev 1.2   04 Jun 1996 18:14:58   DCHI
// Modifications to conform to new header file structure.
// 
//    Rev 1.1   03 Jun 1996 17:21:04   DCHI
// Changes to support building on BORLAND, VC20, SYM_W32, SYM_DOS, and SYM_DOSX.
// 
//    Rev 1.0   21 May 1996 12:25:28   DCHI
// Initial revision.
// 
//************************************************************************

#ifdef BORLAND
#include <dir.h>
#endif // BORLAND

#if defined(VC20) || defined(SYM_WIN32) || \
    defined(SYM_DOS) || defined(SYM_DOSX)

#include <direct.h>

#endif // VC20 || SYM_WIN32 || SYM_DOS || SYM_DOSX

#ifdef SYM_DOS

extern "C" {
int __cdecl __chdir(const char *);
char * __cdecl __getcwd(char *, int);
}

#define chdir       _chdir
#define getcwd      _getcwd

#endif // SYM_DOS

#include <string.h>

#include "avtypes.h"
#include "pathname.h"
#include "filefind.h"

#ifdef SYM_UNIX

#include <sys/stat.h>
#include <unistd.h>

//*************************************************************************
//
// Function:
//  FileSpecMatches()
//
// Parameters:
//  lpFileFindData  Pointer to FILEFINDDATA_T structure
//  lpstrName       Pointer to name to match
//
// Description:
//  Determines whether the given name matches the file spec
//  in the FILEFINDDATA_T structure
//
// Returns:
//  FALSE           If the name doesn't match the file spec
//  TRUE            If the name matches the file spec
//
//*************************************************************************

BOOL FileSpecMatches
(
    LPFILEFINDDATA  lpFileFindData,
    LPSTR           lpstrName
)
{
    // Fix later

    if (strchr(lpFileFindData->szFileSpec,'*') == NULL && 
        strchr(lpFileFindData->szFileSpec,'?') == NULL)
    {
        // Exact name does not match

        if (strcmp(lpFileFindData->szFileSpec,lpstrName) == 0)
            return(TRUE);
        
        return(FALSE);
    }

    return(TRUE);
}

#endif // SYM_UNIX

//*************************************************************************
//
// Function:
//  FindFirstFile()
//
// Parameters:
//  lpstrFileSpec   File specification of files to find
//  lpFileFindData  Pointer to FILEFINDDATA_T structure
//
// Description:
//  Finds the first file matching the file specification.
//
//  CloseFileFind() should be called after the end of the
//  FindFirstFile()/FindNextFile() sequence.
//
// Returns:
//  FALSE           If a matching file was not found
//  TRUE            If a matching file was found
//
//*************************************************************************

BOOL FindFirstFile
(
    LPSTR           lpstrFileSpec,
    LPFILEFINDDATA  lpFileFindData
)
{
#if defined(BORLAND) || defined(SYM_DOS) || defined(SYM_DOSX)

    if (_dos_findfirst(lpstrFileSpec,0xFF,&lpFileFindData->stFindData) == 0)
    {
        lpFileFindData->dwFileAttributes =
            (DWORD)lpFileFindData->stFindData.attrib;

        lpFileFindData->szFileName = lpFileFindData->stFindData.name;

        return(TRUE);
    }

#endif // BORLAND || SYM_DOS || SYM_DOSX

#if defined(VC20) || defined(SYM_WIN32)

    lpFileFindData->lFindHandle = _findfirst(lpstrFileSpec,
                                             &lpFileFindData->stFindData);

    if (lpFileFindData->lFindHandle != -1)
    {
        lpFileFindData->dwFileAttributes =
            (DWORD)lpFileFindData->stFindData.attrib;

        lpFileFindData->szFileName = lpFileFindData->stFindData.name;

        return(TRUE);
    }

#endif // VC20 || SYM_WIN32

#if defined(SYM_UNIX)

    LPSTR   lpstrLastSlash;

    // Initialize to NULL

    lpFileFindData->pDir = NULL;

    // Separate into dir and file spec

    // Find first / from end

    lpstrLastSlash = strrchr(lpstrFileSpec,'/');
    if (lpstrLastSlash == NULL)
    {
        strcpy(lpFileFindData->szDirPath,"./");
        lpFileFindData->nAfterSlashIndex = 2;
    }
    else
    {
        lpFileFindData->nAfterSlashIndex = lpstrLastSlash - lpstrFileSpec + 1;

        if (lpFileFindData->nAfterSlashIndex < PATHNAME_MAX_DIR)
        {
            strncpy(lpFileFindData->szDirPath,lpstrFileSpec,
                lpFileFindData->nAfterSlashIndex);
            lpFileFindData->szDirPath[lpFileFindData->nAfterSlashIndex] = '\0';
        }
        else
        {
            // Directory name too long

            return(FALSE);
        }
    }

    // Copy the file spec

    if (lpstrLastSlash != NULL)
        strcpy(lpFileFindData->szFileSpec,lpstrLastSlash+1);
    else
        strcpy(lpFileFindData->szFileSpec,lpstrFileSpec);

    lpFileFindData->pDir = opendir(lpFileFindData->szDirPath);
    if (lpFileFindData->pDir != NULL)
    {
        while ((lpFileFindData->pDirEnt = readdir(lpFileFindData->pDir)) !=
            NULL)
        {
            if (FileSpecMatches(lpFileFindData,lpFileFindData->pDirEnt->d_name))
            {
                struct stat stStat;

                strcpy(lpFileFindData->szDirPath+
                    lpFileFindData->nAfterSlashIndex,
                    lpFileFindData->pDirEnt->d_name);

                if (stat(lpFileFindData->szDirPath,&stStat) == -1)
                {
                    // Failed to get stat

                    break;
                }

                if ((stStat.st_mode & S_IFMT) == S_IFDIR)
                    lpFileFindData->dwFileAttributes = FF_ATTRIB_DIREC;
                else
                    lpFileFindData->dwFileAttributes = FF_ATTRIB_NORMAL;

                lpFileFindData->szFileName = lpFileFindData->pDirEnt->d_name;

                return(TRUE);
            }
        }
    }

    if (lpFileFindData->pDir != NULL)
    {
        closedir(lpFileFindData->pDir);
        lpFileFindData->pDir = NULL;
    }

#endif // SYM_UNIX

    return(FALSE);
}


//*************************************************************************
//
// Function:
//  FindNextFile()
//
// Parameters:
//  lpFileFindData  Pointer to FILEFINDDATA_T structure initialized
//                  in call to FindFirstFile()
//
// Description:
//  Finds the next file matching the file specification given in a
//  previous call to FindFirstFile().
//
//  CloseFileFind() should be called after the end of the
//  FindFirstFile()/FindNextFile() sequence.
//
// Returns:
//  FALSE           If a matching file was not found
//  TRUE            If a matching file was found
//
//*************************************************************************

BOOL FindNextFile
(
    LPFILEFINDDATA lpFileFindData
)
{
#if defined(BORLAND) || defined(SYM_DOS) || defined(SYM_DOSX)

    if (_dos_findnext(&lpFileFindData->stFindData) == 0)
    {
        lpFileFindData->dwFileAttributes =
            (DWORD)lpFileFindData->stFindData.attrib;

        lpFileFindData->szFileName = lpFileFindData->stFindData.name;

        return(TRUE);
    }

#endif // BORLAND || SYM_DOS || SYM_DOSX

#if defined(VC20) || defined(SYM_WIN32)

    if (_findnext(lpFileFindData->lFindHandle,
                  &lpFileFindData->stFindData) == 0)
    {
        lpFileFindData->dwFileAttributes =
            (DWORD)lpFileFindData->stFindData.attrib;

        lpFileFindData->szFileName = lpFileFindData->stFindData.name;

        return(TRUE);
    }

#endif // VC20 || SYM_WIN32

#if defined(SYM_UNIX)
    while ((lpFileFindData->pDirEnt = readdir(lpFileFindData->pDir)) !=
        NULL)
    {
        if (FileSpecMatches(lpFileFindData,lpFileFindData->pDirEnt->d_name))
        {
            struct stat stStat;

            strcpy(lpFileFindData->szDirPath+
                lpFileFindData->nAfterSlashIndex,
                lpFileFindData->pDirEnt->d_name);

            if (stat(lpFileFindData->szDirPath,&stStat) == -1)
            {
                // Failed to get stat

                break;
            }

            if ((stStat.st_mode & S_IFMT) == S_IFDIR)
                lpFileFindData->dwFileAttributes = FF_ATTRIB_DIREC;
            else
                lpFileFindData->dwFileAttributes = FF_ATTRIB_NORMAL;

            lpFileFindData->szFileName = lpFileFindData->pDirEnt->d_name;

            return(TRUE);
        }
    }

#endif // SYM_UNIX

    return(FALSE);
}


//*************************************************************************
//
// Function:
//  CloseFileFind()
//
// Parameters:
//  lpFileFindData  Pointer to FILEFINDDATA_T structure initialized
//                  in call to FindFirstFile()
//
// Description:
//  Closes down file find sequence.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void CloseFileFind
(
    LPFILEFINDDATA lpFileFindData
)
{
    (void)lpFileFindData;

#if defined(VC20) || defined(SYM_WIN32)

    _findclose(lpFileFindData->lFindHandle);

#endif // VC20 || SYM_WIN32

#if defined(SYM_UNIX)

    if (lpFileFindData->pDir != NULL)
        closedir(lpFileFindData->pDir);

#endif
}


//*************************************************************************
//
// Function:
//  ChangeVolume()
//
// Parameters:
//  lpstrNewVolume  The volume to change to
//
// Description:
//  Makes the specified volume the new current volume.
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL ChangeVolume
(
    LPSTR       lpstrNewVolume
)
{
    int         nDrive;

    if (lpstrNewVolume == NULL)
        return(FALSE);

#ifdef BORLAND

    if (lpstrNewVolume[0] >= 'a' && lpstrNewVolume[0] <= 'z')
        nDrive = lpstrNewVolume[0] - 'a';
    else
    if (lpstrNewVolume[0] >= 'A' && lpstrNewVolume[0] <= 'Z')
        nDrive = lpstrNewVolume[0] - 'A';
    else
        return(FALSE);

    setdisk(nDrive);
    if (getdisk() == nDrive)
        return(TRUE);

#endif // BORLAND

#if defined(VC20) || defined(SYM_WIN32) || \
    defined(SYM_DOS) || defined(SYM_DOSX)

    if (lpstrNewVolume[0] >= 'a' && lpstrNewVolume[0] <= 'z')
        nDrive = lpstrNewVolume[0] - 'a' + 1;
    else
    if (lpstrNewVolume[0] >= 'A' && lpstrNewVolume[0] <= 'Z')
        nDrive = lpstrNewVolume[0] - 'A' + 1;
    else
        return(FALSE);

    // A return value of 1 indicates an error

    if (_chdrive(nDrive) == 1)
        return(FALSE);

    if (_getdrive() == nDrive)
        return(TRUE);

#endif // VC20 || SYM_WIN32 || SYM_DOS || SYM_DOSX

#ifdef SYM_UNIX

    return (TRUE);

#endif // SYM_UNIX

    return(FALSE);
}


//*************************************************************************
//
// Function:
//  ChangeDirectory()
//
// Parameters:
//  lpstrNewDirectory   The directory to change to
//
// Description:
//  Makes the specified directory the new current directory.
//
//  If lpstrNewDirectory specifies a volume that is not the current
//  volume, the current volume does not change even through the current
//  directory on that volume does change.
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL ChangeDirectory
(
    LPSTR       lpstrNewDirectory
)
{
    int         nLen;
    char        chLastChar;
    LPSTR       pLastChar;

    if (lpstrNewDirectory == NULL || *lpstrNewDirectory == '\0')
        return(FALSE);

    nLen = strlen(lpstrNewDirectory);
    pLastChar = lpstrNewDirectory + nLen - 1;
    chLastChar = *pLastChar;

    // A trailing slash causes problems on some systems

    if (chLastChar == '\\' || chLastChar == '/')
    {
        // If it is the root, leave the slash

        if (nLen > 1 && lpstrNewDirectory[nLen-2] != ':')
            *pLastChar = '\0';
    }

    if (chdir(lpstrNewDirectory) == 0)
    {
        *pLastChar = chLastChar;
        return(TRUE);
    }

    *pLastChar = chLastChar;
    return(FALSE);
}


//*************************************************************************
//
// Function:
//  ChangeFullVolDir()
//
// Parameters:
//  lpstrNewFullVolDir  The volume and directory to change to
//
// Description:
//  Makes the specified volume and directory the new current volume
//  and directory.
//
//  The parameter should point to a string containing a volume
//  and full directory path.
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL ChangeFullVolDir
(
    LPSTR       lpstrNewFullVolDir
)
{
	int			nPathContents;
    char        szVol[PATHNAME_MAX_VOLUME];
    char        szDir[PATHNAME_MAX_DIR];

    nPathContents = PathNameSplit(lpstrNewFullVolDir,
        szVol,szDir,NULL,NULL);

    if (nPathContents & PATHNAME_HAS_VOLUME)
    {
        if (ChangeVolume(szVol) == FALSE)
            return(FALSE);
    }

    if (nPathContents & PATHNAME_HAS_DIR)
    {
        if (ChangeDirectory(szDir) == FALSE)
            return(FALSE);
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  GetCurrentDirectory()
//
// Parameters:
//  lpstrCurrentDirectory   Buffer to store volume and directory
//
// Description:
//  Stores to lpstrCurrentDirectory the full volume and directory
//  path of the current working directory.
//
//  The last character of the string will be a slash.
//
// Returns:
//  FALSE           On error
//  TRUE            On success
//
//*************************************************************************

BOOL GetCurrentDirectory
(
    LPSTR       lpstrCurrentDirectory
)
{
    int         nLen;

    if (lpstrCurrentDirectory == NULL)
        return(FALSE);

    if (getcwd(lpstrCurrentDirectory,SYM_MAX_PATH) == NULL)
        return(FALSE);

    // Add a backslash to the end if necessary

    nLen = strlen(lpstrCurrentDirectory);
    if (nLen > 0)
    {
        if (lpstrCurrentDirectory[nLen-1] != '\\' &&
            lpstrCurrentDirectory[nLen-1] != '/')
        {
            lpstrCurrentDirectory[nLen] = DIR_SEPARATOR;
            lpstrCurrentDirectory[nLen+1] = '\0';
        }
    }
    else
    {
        lpstrCurrentDirectory[0] = DIR_SEPARATOR;
        lpstrCurrentDirectory[1] = '\0';
    }

    return(TRUE);
}


