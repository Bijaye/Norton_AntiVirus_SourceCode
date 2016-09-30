// Copyright 1995 Symantec Corporation
//========================================================================
//
// $Header:   S:/SYMKRNL/VCS/cfg_prof.c_v   1.10   01 Nov 1996 11:18:18   PMARTIN  $
//
// Description:     Functions for manipulation information in Windows-
//                  style .INI files.
//
//                  They are functionally equivalent to the ones found in
//                  the Windows and OS/2 APIs, but work with UNC names,
//                  server names longer than 11 characters, and do not
//                  cause sharing violations when multiple workstations
//                  access the same file.
//
// See Also:
//
//------------------------------------------------------------------------
// $Log:   S:/SYMKRNL/VCS/cfg_prof.c_v  $
// 
//    Rev 1.10   01 Nov 1996 11:18:18   PMARTIN
// Use dummy symnet handle when calling symnet functions.
//
//    Rev 1.9   31 Oct 1996 11:07:36   PMARTIN
// Changed _open() to FileOpen() in CreateIniCache().
//
//    Rev 1.8   01 Oct 1996 12:39:06   PMARTIN
// Moved balance of ini cache functions (3) outside if #ifdef's.
//
//    Rev 1.7   01 Oct 1996 10:43:18   PMARTIN
// GetCacheSection() and GetCacheSectionNames() moved outside all #ifdef's.
//
//    Rev 1.6   30 Sep 1996 11:23:12   PMARTIN
// Added GetCacheSection() and GetCacheSectionNames() from Nail.
//
//    Rev 1.5   20 Sep 1996 15:00:38   SSAWKIN
// Fixed problem with SYMNET calls under WIN platform
//     HSYMNET is 4 bytes, but only 2 were being pushed.
//
//    Rev 1.4   03 May 1996 17:24:48   BMCCORK
// Fixed compile warnings under WIN platform
//
//    Rev 1.3   28 Feb 1996 12:40:10   SSAWKIN
// Changed nHandle to hIniFileHandle to follow the typical naming convention
//     for Win32
// dwBufferLength is now only defined for Win32 to avoid "variable not used"
//     warning on Win16 platform
//
//    Rev 1.2   27 Feb 1996 18:22:56   SSAWKIN
// Changed SYM_W32 to SYM_WIN32, since SYM_W32 is no longer used.
//
//    Rev 1.1   27 Feb 1996 14:49:54   SSAWKIN
// Fixed CreateINICache for WIN32 platform
// Old code was doing sopen/lread which won't work for WIN32
// Other platforms are unaffected
//
//    Rev 1.0   26 Jan 1996 20:21:44   JREARDON
// Initial revision.
//
//    Rev 1.3   09 Jan 1996 15:41:54   kkiyohar
// Modified GetSymnetFunctions to use proper Win-16/32 Symnet module name
//
//    Rev 1.2   02 Aug 1995 01:28:00   AWELCH
// Merge changes from Quake 7.
//
//    Rev 1.1.1.17   26 Jun 1995 18:19:38   PGRAVES
// Fixed a typo in GetCacheEntry that was causing a hang when a comment line
// was encountered.
//
//    Rev 1.1.1.16   31 May 1995 10:35:12   DLEVITON
// Fixed bugs found when building for SYM_WIN32.
// PLEASE SEE ME BEFORE PORTING THIS MODULE TO THE TRUNK.--Dan'l
//
//    Rev 1.1.1.15   23 May 1995 15:24:04   TORSTEN
// Beefed up the error checking, changed GetLine() so that it can work
// with a smaller buffer if the initial allocation attempt failed -- this
// is less efficient, but beats not being able to read .INI files.
//
//    Rev 1.1.1.14   15 May 1995 23:15:44   JMCILWAI
// Minor adjustments to cachegetentry
//
//    Rev 1.1.1.13   15 May 1995 18:59:26   MBARNES
// Work in progress: firs tpass at INI cache functions
//
//    Rev 1.1.1.12   14 May 1995 01:18:12   MBARNES
// Updated Symkrnl***() functions to Sym***() functions for DOS, X86 and OS/2
//
//    Rev 1.1.1.11   13 May 1995 23:42:44   MBARNES
// Added covers for UNC names longer than 11 characters to the
// Sym*PrivateProfileString() functions; note we changed their names again!
//
//    Rev 1.1.1.10   12 May 1995 01:11:50   TORSTEN
// Modified SymkrnlGetPrivateProfileString() to support lpszEntry == NULL.
//
//    Rev 1.1.1.9   11 May 1995 23:10:38   TORSTEN
// Fix for ill-behaved callers who pass in a larger cbReturnBuffer than
// they actually have.
//
//    Rev 1.1.1.8   11 May 1995 00:19:02   MBARNES
// Changed names of functions from Sym**ProfileString to Symkrnl**ProfileString()
//
//    Rev 1.1.1.7   10 May 1995 23:02:20   TORSTEN
// Added an _unlink() call prior to attempting to rename -- Duh!
//
//    Rev 1.1.1.6   10 May 1995 16:02:46   TORSTEN
// Argh! Changed the read() to _lread() for SYM_WIN and SYM_W32.
//
//    Rev 1.1.1.5   10 May 1995 15:57:18   TORSTEN
// Changed the FileRead() call to read().
//
//    Rev 1.1.1.4   10 May 1995 14:53:32   PATRICKM
// Redefined some macros for DOS and changed _lread to FileRead so it will link
// for DOS
//
//    Rev 1.1.1.3   10 May 1995 13:24:02   TORSTEN
// Replaced lread() with read() for OS/2 only.
//
//    Rev 1.1.1.2   10 May 1995 12:05:38   TORSTEN
// Made this module OS/2 compatible.
//
//    Rev 1.1.1.1   10 May 1995 11:46:30   TORSTEN
// Removed all statically allocated buffers, and replaced them with
// explicit near allocations. Added the GetLine() function to replace the
// unbuffered character I/O used to read the .INI files.
//
//    Rev 1.1.1.0   10 May 1995 09:54:10   JREARDON
// Branch base for version QAK7
//
//    Rev 1.1   10 May 1995 06:43:20   MBARNES
// Fixed the Sym*Profile() functions so that they work correctly with the /Alnw
// memory model (the solution is gross, but then again, so is the memory model).
//
//    Rev 1.0   09 May 1995 21:55:48   TORSTEN
// Initial revision.
//------------------------------------------------------------------------



#include    "platform.h"                // Quakeish things
#ifdef SYM_OS2
#   include <dir.h>
#endif
#include    "xapi.h"
#include    "file.h"

#ifdef EOF
#undef EOF
#endif

#include    <fcntl.h>
#include    <io.h>
#include    <share.h>
#include    <sys\types.h>
#include    <sys\stat.h>
#include    <stdlib.h>
#include    <malloc.h>


//========================================================================
// module constraints and misc. constants
//========================================================================

#define MAX_INI_LINE        256

#define EOF_CHAR            0x1A
#define CR_CHAR             0x0D
#define LF_CHAR             0x0A
#define COMMENT_CHAR        ';'
#define SECTIONSTART_CHAR   '['
#define SECTIONEND_CHAR     ']'
#define SPACE_CHAR          ' '
#define TAB_CHAR            '\t'
#define EOS_CHAR            '\0'
#define EQUAL_CHAR          '='
#define QUOTE_CHAR          '"'



// ##IPE BEGIN  DO NOT ALTER OR TRANSLATE THE FOLLOWING QUOTED STRINGS
static  char    szSectionStart[] = "\r\n\r\n[";
static  char    szSectionEnd[]   = "]\r\n";
static  char    szCRLF[]         = "\r\n";
static  char    szEqual[]        = "=";
static  char    szTMP[]          = ".TMP";
static  char    szBlank[]        = "";
// ##IPE END


#if defined(SYM_WIN) || defined(SYM_WIN32)
#define ANSITOOEM(szFrom, szTo)     AnsiToOem(szFrom, szTo)
#define MALLOC(size)                _nmalloc(size)
#define FREE(pointer)               _nfree(pointer)
#else
#define ANSITOOEM(szFrom, szTo)     STRCPY(szTo, szFrom)
#define MALLOC(size)                _fmalloc(size)
#define FREE(pointer)               _ffree(pointer)
#endif

#ifdef SYM_DOS
#undef MALLOC
#undef FREE
#define MALLOC(size)                MemAllocFixed(size)
#define FREE(pointer)               MemFreeFixed(pointer)
#endif


#ifndef SYM_WIN
//========================================================================
// The following code now applies to DOS and OS/2 only
//========================================================================
//------------------------------------------------------------------------
// SharedOpen
//
// This is a cover to the _sopen function for DOS libraries without the
// CDECL parameter.  It's main purpose is to provide AnsitoOem translations
//
// Returns: file handle _sopen return values
//------------------------------------------------------------------------
static int WINAPI SharedOpen (
                              LPCSTR  lpszFileName,               // [in]     File Name
                              int     nFlags,                     // [in]     Flags
                              int     nShFlags,                   // [in]     Share flags
                              int     nPMode)                     // [in]     pMode Flags
{
    auto    char *  lpszOEM;
    auto    int     nHandle;

    if (!(lpszOEM = MALLOC(MAX_INTL_STR + 1)))
        return (-1);

#ifndef SYM_OS2                         //  All OS/2 text is OEM only.
    // Perform  AnsiToOem conversion
    ANSITOOEM (lpszFileName, lpszOEM);
    nHandle = _sopen (lpszOEM, nFlags, nShFlags, nPMode);
#else
    // Pass on to the _sopen call
    STRCPY(lpszOEM, lpszFileName);
    nHandle = sopen (lpszOEM, nFlags, nShFlags, nPMode);
#endif
    FREE(lpszOEM);

    return (nHandle);
}

//------------------------------------------------------------------------
// SharedFileExists
//
// This is a cover to the _access function for DOS libraries without the
// CDECL parameter.  It's main purpose is to provide AnsitoOem translations
//
// Returns: file handle _access return values
//------------------------------------------------------------------------
static int WINAPI SharedFileExists (
                                    LPCSTR  lpszFileName,               // [in]     File Name
                                    int     nMode)                      // [in]     Mode
{
    auto    char *  lpszOEM;
    auto    int     nRetVal;

    if (!(lpszOEM = MALLOC(MAX_INTL_STR + 1)))
        return (-1);

#ifndef SYM_OS2                         //  All OS/2 text is OEM only.
    // Perform  AnsiToOem conversion
    ANSITOOEM (lpszFileName, lpszOEM);
#else
    STRCPY(lpszOEM, lpszFileName);
#endif

    // Pass on to the _access call
    nRetVal = _access (lpszOEM, nMode);

    FREE(lpszOEM);

    return (nRetVal);
}


//========================================================================
//
// Description:     Perform buffered line-reads on a handle
//
// Parameters:           nHandle - The handle to read from
//                  szLineBuffer - The buffer to be filled with a NULL-
//                                 terminated string from nHandle
//
// Return Value:    TRUE if a line was read, FALSE otherwise
//
// See Also:
//
//------------------------------------------------------------------------
// 05/10/1995 TORSTEN Function Created.
//------------------------------------------------------------------------

static BOOL WINAPI GetLine(int nHandle, char * szLineBuffer)
{
#define    MAX_LINE_BUFFER    1024

    static char *   byInputBuffer = NULL;
    static int      nProcessed  = 0;
    static int      nTotal      = 0;
    static int      nBufferSize = 0;

                                        // free the buffer if needed
    if (nHandle == -1)
        {
        if (byInputBuffer)
            FREE(byInputBuffer);

        nProcessed    = 0;
        nTotal        = 0;
        byInputBuffer = NULL;

        return (FALSE);
        }

                                        // allocate the buffer, and remember
                                        // the buffer size...
    if (!byInputBuffer)
        {
        nBufferSize = MAX_LINE_BUFFER;

        do {
            if (byInputBuffer = MALLOC(MAX_LINE_BUFFER))
                break;

            nBufferSize = nBufferSize / 2;

                                        // this is getting somewhat
                                        // ridiculous, so let's bail...
            if (nBufferSize < 32)
                return (FALSE);

            } while (TRUE);
        }

TOP:

    if (nProcessed >= nTotal)
        {
#if defined(SYM_WIN) || defined(SYM_WIN32)
        nTotal = _lread(nHandle, byInputBuffer, nBufferSize);
#else
        nTotal = read(nHandle, byInputBuffer, nBufferSize);
#endif

        if (nTotal <= 0)
            return (FALSE);
        nProcessed = 0;
        }

    while (nProcessed < nTotal &&
           byInputBuffer[nProcessed] != CR_CHAR  &&
           byInputBuffer[nProcessed] != EOF_CHAR &&
           byInputBuffer[nProcessed] != EOS_CHAR)
        *(szLineBuffer++) = byInputBuffer[nProcessed++];

    if (byInputBuffer[nProcessed] == CR_CHAR  ||
        byInputBuffer[nProcessed] == EOF_CHAR ||
        byInputBuffer[nProcessed] == EOS_CHAR)
        {
        nProcessed++;
        if (byInputBuffer[nProcessed] == LF_CHAR)
            nProcessed++;
        }
    else
        if (!_eof(nHandle))
            goto TOP;

    *szLineBuffer = EOS_CHAR;

    return (TRUE);
}

int SYM_EXPORT WINAPI SymGetPrivateProfileString(
    LPCSTR      lpszSection,
    LPCSTR      lpszEntry,
    LPCSTR      lpszDefault,
    LPSTR       lpszReturnBuffer,
    int         cbReturnBuffer,
    LPCSTR      lpszFileName
    )
{
    auto    int     nFileHandle;
    auto    LPSTR   lpLineStart;
    auto    LPSTR   lpWalkPtr;
    auto    LPSTR   lpLineEnd;
    auto    LPSTR   lpBlock;
    auto    int     nBlockLength;
    auto    char *  lpszLineBuffer;
    auto    BOOL    bFoundSection;

    if ((nFileHandle = SharedOpen(lpszFileName, _O_BINARY | _O_RDONLY, _SH_DENYWR, _S_IREAD)) != -1)
    {
        if (!(lpszLineBuffer = MALLOC(MAX_INI_LINE + 1)))
            {
#if defined(SYM_OS2)
            close(nFileHandle);
#else
            _close(nFileHandle);
#endif

            MEMCPY(lpszReturnBuffer, lpszDefault, min((int) STRLEN(lpszDefault) + 1, cbReturnBuffer));
            return (STRLEN(lpszReturnBuffer));
            }

        bFoundSection = (BOOL) (lpszSection == NULL);
        lpBlock       = lpszReturnBuffer;
        nBlockLength  = 0;

        GetLine(-1, NULL);

        // process the entire file
        while (GetLine(nFileHandle, lpszLineBuffer))
        {

            // ignore leading whitespace
            lpLineStart = lpszLineBuffer;
            while (*lpLineStart == SPACE_CHAR || *lpLineStart == TAB_CHAR)
                lpLineStart++;

            // ignore blank lines
            if (!(*lpLineStart))
                continue;

            // if this line contains a comment,
            // continue
            if (*lpLineStart == COMMENT_CHAR)
                continue;

            // is this the start of a new section?
            if (*lpLineStart == SECTIONSTART_CHAR)
            {

                if (lpszSection)
                {
                    // we went past the section
                    if (bFoundSection)
                    {
#if defined(SYM_OS2)
                        close(nFileHandle);
#else
                        _close(nFileHandle);
#endif

                        if (lpszEntry)
                        {

                            // return the default
                            MEMCPY(lpszReturnBuffer, lpszDefault, min((int) STRLEN(lpszDefault) + 1, cbReturnBuffer));
                            FREE(lpszLineBuffer);
                            GetLine(-1, NULL);
                            return (STRLEN(lpszReturnBuffer));
                        }
                        else
                        {
                            nBlockLength += 2;
                            *(++lpBlock) = EOS_CHAR;
                            *(++lpBlock) = EOS_CHAR;

                            FREE(lpszLineBuffer);
                            GetLine(-1, NULL);

                            return (nBlockLength);
                        }
                    }

                    lpLineStart++;
                    lpWalkPtr = lpLineStart;
                    while (*lpWalkPtr)
                    {
                        if (*lpWalkPtr == SECTIONEND_CHAR)
                        {
                            *lpWalkPtr = EOS_CHAR;
                            break;
                        }
                        else
                            lpWalkPtr++;
                    }

                    // is this the section we are looking
                    // for?
                    bFoundSection = (BOOL) (STRICMP(lpLineStart, lpszSection) == 0);
                }
            }
            else
            {
                // if we are interested in this
                // section, check the keys
                if (bFoundSection)
                {
                    while (*lpLineStart == SPACE_CHAR || *lpLineStart == TAB_CHAR)
                        lpLineStart++;

                    lpWalkPtr = lpLineStart;

                    while (*lpWalkPtr)
                    {
                        if (*lpWalkPtr == EQUAL_CHAR)
                        {
                            *lpWalkPtr = EOS_CHAR;
                            lpLineEnd = lpWalkPtr;
                            while (*(lpLineEnd - 1) == SPACE_CHAR || *(lpLineEnd - 1) == TAB_CHAR)
                            {
                                *(lpLineEnd - 1) = EOS_CHAR;
                                lpLineEnd--;
                            }
                            break;
                        }
                        else
                            lpWalkPtr++;
                    }

                    if (lpszEntry)
                    {
                        // if this is the entry we want, use
                        // it!
                        if (STRICMP(lpLineStart, lpszEntry) == 0)
                        {
#if defined(SYM_OS2)
                            close(nFileHandle);
#else
                            _close(nFileHandle);
#endif

                            lpWalkPtr++;
                            while (*lpWalkPtr == SPACE_CHAR || *lpWalkPtr == TAB_CHAR)
                                lpWalkPtr++;

                            if (*lpWalkPtr == QUOTE_CHAR)
                                lpWalkPtr++;

                            lpLineEnd = lpWalkPtr + STRLEN(lpWalkPtr);
                            if (lpWalkPtr != lpLineEnd)
                            {
                                while (*(lpLineEnd - 1) == SPACE_CHAR || *(lpLineEnd - 1) == TAB_CHAR)
                                {
                                    *(lpLineEnd - 1) = EOS_CHAR;
                                    lpLineEnd--;
                                }
                            }

                            if (*(lpLineEnd - 1) == QUOTE_CHAR)
                                *(lpLineEnd - 1) = EOS_CHAR;

                            MEMCPY(lpszReturnBuffer, lpWalkPtr, min((int) STRLEN(lpWalkPtr) + 1, cbReturnBuffer));

                            FREE(lpszLineBuffer);
                            GetLine(-1, NULL);
                            return (STRLEN(lpszReturnBuffer));
                        }
                    }
                    else
                    {
                        if (STRLEN(lpLineStart))
                        {
                            STRCPY(lpBlock, lpLineStart);

                            nBlockLength += STRLEN(lpBlock) + 1;
                            lpBlock      += STRLEN(lpBlock) + 1;
                        }
                    }
                }
            }

        }
        FREE(lpszLineBuffer);

#if defined(SYM_OS2)
        close(nFileHandle);
#else
        _close(nFileHandle);
#endif
        GetLine(-1, NULL);
    }

    // return the default
    MEMCPY(lpszReturnBuffer, lpszDefault, min((int) STRLEN(lpszDefault) + 1, cbReturnBuffer));
    return (STRLEN(lpszReturnBuffer));
}

BOOL SYM_EXPORT WINAPI SymWritePrivateProfileString(
    LPCSTR      lpszSection,
    LPCSTR      lpszEntry,
    LPCSTR      lpszString,
    LPCSTR      lpszFileName
    )
{
    auto    int     nInputHandle;
    auto    int     nOutputHandle;
    auto    LPSTR   lpLineStart;
    auto    LPSTR   lpWalkPtr;
    auto    LPSTR   lpLineEnd;
    auto    char *  lpszNearFileName;
    auto    char *  lpszNewFileName;
    auto    char *  lpszLineBuffer;
    auto    char *  lpszNearSection;
    auto    char *  lpszNearEntry;
    auto    char *  lpszNearString;
    auto    BOOL    bFoundSection;
    auto    BOOL    bAdded  = FALSE;
    auto    BOOL    bRetVal = FALSE;

    lpszNewFileName  = MALLOC(MAX_INTL_STR + 1);
    lpszNearFileName = MALLOC(MAX_INTL_STR + 1);
    lpszLineBuffer   = MALLOC(MAX_INI_LINE + 1);
    lpszNearSection  = MALLOC(MAX_INI_LINE + 1);
    lpszNearEntry    = MALLOC(MAX_INI_LINE + 1);
    lpszNearString   = MALLOC(MAX_INI_LINE + 1);

    if (!lpszNewFileName || !lpszNearFileName || !lpszLineBuffer || !lpszNearSection || !lpszNearEntry || !lpszNearString)
        {
        FREE(lpszNewFileName);
        FREE(lpszNearFileName);
        FREE(lpszLineBuffer);
        FREE(lpszNearSection);
        FREE(lpszNearEntry);
        FREE(lpszNearString);

        return (bRetVal);
        }

    STRCPY(lpszNearFileName, lpszFileName);
    STRCPY(lpszNearSection,  lpszSection);
    STRCPY(lpszNearEntry,    lpszEntry);
    STRCPY(lpszNearString,   lpszString);



    // Using the 'lpszLineBuffer', find
    // out if the directory for this
    // file exists.
    STRCPY(lpszLineBuffer, lpszFileName);
    NameStripFile (lpszLineBuffer);
    // Make sure it's not the root dir
    if (STRLEN(lpszLineBuffer) > 3)
        {
#ifdef SYM_OS2
        mkdir (lpszLineBuffer );
#else
        // Create the dir(s) necessary
        DirMakePath(lpszLineBuffer);
#endif
        }

    if (SharedFileExists(lpszFileName, 0))
        {
        if ((nOutputHandle = SharedOpen(lpszFileName, _O_CREAT | _O_TRUNC | _O_BINARY | _O_WRONLY, _SH_DENYRW, _S_IREAD | _S_IWRITE)) == -1)
            {
            FREE(lpszNearFileName);
            FREE(lpszNewFileName);
            FREE(lpszLineBuffer);
            FREE(lpszNearSection);
            FREE(lpszNearEntry);
            FREE(lpszNearString);
            return (FALSE);
            }
#if defined(SYM_OS2)
        close(nOutputHandle);
#else
        _close(nOutputHandle);
#endif
        }

    if ((nInputHandle = SharedOpen(lpszFileName, _O_BINARY | _O_RDONLY, _SH_DENYWR, _S_IREAD)) != -1)
        {
        STRCPY(lpszNewFileName, lpszFileName);
        NameStripExtension(lpszNewFileName);
        STRCAT(lpszNewFileName, szTMP);

        if ((nOutputHandle = SharedOpen(lpszNewFileName, _O_CREAT | _O_TRUNC | _O_BINARY | _O_WRONLY, _SH_DENYRW, _S_IREAD | _S_IWRITE)) == -1)
            {
            FREE(lpszNearFileName);
            FREE(lpszNewFileName);
            FREE(lpszLineBuffer);
            FREE(lpszNearSection);
            FREE(lpszNearEntry);
            FREE(lpszNearString);
#if defined(SYM_OS2)
            close(nInputHandle);
#else
            _close(nInputHandle);
#endif
            return (FALSE);
            }

        bFoundSection = (BOOL) (lpszSection == NULL);

        GetLine(-1, NULL);

        // process the entire file
        while (GetLine(nInputHandle, lpszLineBuffer))
            {

            // ignore leading whitespace
            lpLineStart = lpszLineBuffer;
            while (*lpLineStart == SPACE_CHAR || *lpLineStart == TAB_CHAR)
                lpLineStart++;

            // ignore blank lines
            if (!(*lpLineStart))
                {
#if defined(SYM_OS2)
                write(nOutputHandle, szCRLF, STRLEN(szCRLF));
#else
                _write(nOutputHandle, szCRLF, STRLEN(szCRLF));
#endif
                continue;
                }

            // if this line contains a comment,
            // continue
            if (*lpLineStart == COMMENT_CHAR)
                {
                _write(nOutputHandle, lpszLineBuffer, STRLEN(lpszLineBuffer));
                _write(nOutputHandle, szCRLF, STRLEN(szCRLF));
                continue;
                }

            // is this the start of a new section?
            if (*lpLineStart == SECTIONSTART_CHAR)
                {
                _write(nOutputHandle, lpszLineBuffer, STRLEN(lpszLineBuffer));
                _write(nOutputHandle, szCRLF, STRLEN(szCRLF));

                lpLineStart++;
                lpWalkPtr = lpLineStart;
                while (*lpWalkPtr)
                    {
                    if (*lpWalkPtr == SECTIONEND_CHAR)
                        {
                        *lpWalkPtr = EOS_CHAR;
                        break;
                        }
                    else
                        lpWalkPtr++;
                    }

                // is this the section we are looking
                // for?
                if (lpszSection)
                    {
                    bFoundSection = (BOOL) (STRICMP(lpLineStart, lpszSection) == 0);
                    }

                if (bFoundSection && lpszString)
                    {
                    _write(nOutputHandle, lpszNearEntry,  STRLEN(lpszEntry));
                    _write(nOutputHandle, szEqual,        STRLEN(szEqual));
                    _write(nOutputHandle, lpszNearString, STRLEN(lpszString));
                    _write(nOutputHandle, szCRLF,         STRLEN(szCRLF));
                    bAdded = TRUE;
                    }
                }
            else
                {
                // if we are interested in this
                // section, check the keys
                if (bFoundSection)
                    {

                    while (*lpLineStart == SPACE_CHAR || *lpLineStart == TAB_CHAR)
                        lpLineStart++;

                    lpWalkPtr = lpLineStart;

                    while (*lpWalkPtr)
                        {
                        if (*lpWalkPtr == EQUAL_CHAR)
                            {
                            *lpWalkPtr = EOS_CHAR;
                            lpLineEnd = lpWalkPtr;
                            while (*(lpLineEnd - 1) == SPACE_CHAR || *(lpLineEnd - 1) == TAB_CHAR)
                                {
                                *(lpLineEnd - 1) = EOS_CHAR;
                                lpLineEnd--;
                                }
                            break;
                            }
                        else
                            lpWalkPtr++;
                        }

                    // if this is the entry we need to
                    // write, remove it!
                    if (STRICMP(lpLineStart, lpszEntry))
                        {
                        *lpWalkPtr = EQUAL_CHAR;
                        _write(nOutputHandle, lpszLineBuffer, STRLEN(lpszLineBuffer));
                        _write(nOutputHandle, szCRLF, STRLEN(szCRLF));
                        }
                    }
                else
                    {
                    _write(nOutputHandle, lpszLineBuffer, STRLEN(lpszLineBuffer));
                    _write(nOutputHandle, szCRLF, STRLEN(szCRLF));
                    }
                }
            }

        // we didn't find the section, so we
        // add it and the key...
        if (!bAdded && lpszString)
            {
            _write(nOutputHandle, szSectionStart,   STRLEN(szSectionStart));
            _write(nOutputHandle, lpszNearSection,  STRLEN(lpszNearSection));
            _write(nOutputHandle, szSectionEnd,     STRLEN(szSectionEnd));
            _write(nOutputHandle, lpszNearEntry,    STRLEN(lpszNearEntry));
            _write(nOutputHandle, szEqual,          STRLEN(szEqual));
            _write(nOutputHandle, lpszNearString,   STRLEN(lpszNearString));
            _write(nOutputHandle, szCRLF,           STRLEN(szCRLF));
            }

#if defined(SYM_OS2)
        close(nInputHandle);
        close(nOutputHandle);
#else
        _close(nInputHandle);
        _close(nOutputHandle);
#endif

        _unlink(lpszNearFileName);
        bRetVal = (BOOL) (rename(lpszNewFileName, lpszNearFileName) == 0);

        GetLine(-1, NULL);
        }

    FREE(lpszNearFileName);
    FREE(lpszNewFileName);
    FREE(lpszLineBuffer);
    FREE(lpszNearSection);
    FREE(lpszNearEntry);
    FREE(lpszNearString);

    return (bRetVal);
}


UINT SYM_EXPORT WINAPI SymGetPrivateProfileInt(
    LPCSTR      lpszSection,
    LPCSTR      lpszEntry,
    int         nDefault,
    LPCSTR      lpszFileName
    )
{
    auto    char *  lpszBuffer;
    auto    char *  lpszDefault;
    auto    int     nInt = nDefault;

    lpszBuffer  = MALLOC(MAX_INI_LINE + 1);
    lpszDefault = MALLOC(20);

    if (lpszBuffer && lpszDefault)
        {
#if defined(SYM_WIN) || defined(SYM_WIN32)
        _itoa(nDefault, lpszDefault, 10);
#else
        itoa(nDefault, lpszDefault, 10);
#endif

        SymGetPrivateProfileString (lpszSection, lpszEntry, lpszDefault, lpszBuffer, 20, lpszFileName);

        nInt = atoi(lpszBuffer);
        }

    FREE(lpszBuffer);
    FREE(lpszDefault);

    return (nInt);
}

#else /* #ifndef SYM_WIN */
//========================================================================
// This code applies to Windows only
//========================================================================
//
// Wrapper functions for Windows profile string functions.
//
// these functions work around problems that the original
// Windows functions have with UNC pathnames containing
// server names longer than 11 characters. the basic idea
// is to map a drive to the server so that the path can be
// represented with a drive letter instead of a UNC path.
//
//===========================================

#include    "quakever.h"
#include    "symnet.h"

static  BOOL    bBreakConnection = FALSE;
static  char    szMappedDrive[10];
static  HMODULE hSymnet = (HMODULE) NULL;
static  WORD    (WINAPI *pfnNetDrivePathFromNetPath)();
static  WORD    (WINAPI *pfnNetGetAvailableDrive)();
static  WORD    (WINAPI *pfnNetAddConnection)();
static  WORD    (WINAPI *pfnNetCancelConnection)();

#if defined(SYM_WIN32)
   #define SYMNET_MODULE_NAME (QMODULE_SYMNET_32 ".DLL")

#elif defined(SYM_WIN)
   #define SYMNET_MODULE_NAME (QMODULE_SYMNET_16 ".DLL")

#endif

static BOOL GetSymnetFunctions(void)
{
    /* Find the SYMNET module ... */
    hSymnet = LoadLibrary(SYMNET_MODULE_NAME);
    if (hSymnet < (HINSTANCE)HINSTANCE_ERROR)
	{
        #if defined(SYM_DEBUG)
            char szBuf[256];

			wsprintf( szBuf, "%s.%d: LoadLibrary(%s) failed (%d)\n",
					  __FILE__, __LINE__, SYMNET_MODULE_NAME, hSymnet);
            OutputDebugString(szBuf);
        #endif

        return(FALSE);
    }

    /* Get the function addresses ... */
    (FARPROC) pfnNetDrivePathFromNetPath = GetProcAddress(hSymnet, "NetDrivePathFromNetPath");
    (FARPROC) pfnNetGetAvailableDrive = GetProcAddress(hSymnet, "NetGetAvailableDrive");
    (FARPROC) pfnNetAddConnection = GetProcAddress(hSymnet, "NetAddConnection");
    (FARPROC) pfnNetCancelConnection = GetProcAddress(hSymnet, "NetCancelConnection");
    if ((pfnNetDrivePathFromNetPath == NULL) ||
        (pfnNetGetAvailableDrive == NULL) ||
        (pfnNetAddConnection == NULL) ||
        (pfnNetCancelConnection == NULL))
        return(FALSE);

    return(TRUE);
}


static void FreeSymnet(void)
{
    /* Make sure Symnet was loaded ... */
    if (hSymnet == (HMODULE) NULL)
        return;

    /* Free it and return ... */
    FreeLibrary(hSymnet);
    hSymnet = (HMODULE) NULL;
    return;
}


static VOID GetWorkingPath ( LPCSTR lpOriginal, LPSTR lpNew )
{
    BOOL    bLongUNCPath = FALSE;
    LPSTR   lpTemp;

    /* Try and get pointers to the symnet functions ... */
    if (!GetSymnetFunctions())
    {
        _fstrcpy ( lpNew, lpOriginal );
        return;
    }

    bBreakConnection = FALSE;
    if ( lpOriginal[0] == '\\'  && lpOriginal[1] == '\\' )
    {
        // we have a UNC path. see if
        // the server name is longer
        // than 11 characters.
        lpTemp = _fstrchr ( &lpOriginal[2], '\\' );
        if ( lpTemp )
        {
            if ( lpTemp - lpOriginal > 13 )
                bLongUNCPath = TRUE;
        }
    }

    // if we don't have a long UNC path
    // then use the original path name.
    if ( ! bLongUNCPath )
    {
        _fstrcpy ( lpNew, lpOriginal );
        return;
    }
    else
    {
        // we have a UNC name. if there is
        // a drive letter already mapped,
        // then the next call will do what
        // we need.
        if ( ! pfnNetDrivePathFromNetPath ( (HSYMNET)0xffffffff, (LPSTR) lpOriginal, lpNew, 255 ) )
        {
            // it worked.
            return;
        }
        else
        {
            // we need to try mapping a drive.
            if ( ! pfnNetGetAvailableDrive ((HSYMNET)0xffffffff, (char far *) szMappedDrive ) )
            {
                // we've got a drive letter. try
                // adding the connection.
                _fstrcpy ( lpNew, lpOriginal );
                NameStripFile ( lpNew );
                if ( ! pfnNetAddConnection ((HSYMNET)0xffffffff, lpNew, (char far *) szBlank, (char far *) szMappedDrive ) )
                {                                       // it worked. convert the name
                    // to use the drive letter.
                    bBreakConnection = TRUE;
                    if ( ! pfnNetDrivePathFromNetPath ( (HSYMNET)0xffffffff,
                        (LPSTR) lpOriginal, lpNew, 255 ) )
                    {
                        return;
                    }
                }
            }
        }
    }
    // if we get here, it's because
    // one of the calls to transform
    // the UNC path failed. fallback
    // position is to use the name
    // we were given.
    _fstrcpy ( lpNew, lpOriginal );
}

static VOID FreeWorkingPath ( )
{
    if ( bBreakConnection )
    {
        pfnNetCancelConnection ((HSYMNET)0xffffffff, (char far *) szMappedDrive, TRUE );
    }
    FreeSymnet();
    bBreakConnection = FALSE;
}

BOOL SYM_EXPORT WINAPI
   SymWritePrivateProfileString ( LPCSTR lpSection, LPCSTR lpEntry,
                                  LPCSTR lpValue, LPCSTR lpFile )
{
    BOOL    bRet;
    char    szNewPath[256];

    GetWorkingPath ( lpFile, szNewPath );
    bRet = WritePrivateProfileString ( lpSection, lpEntry,
                                       lpValue, szNewPath );
    FreeWorkingPath ( );
    return bRet;
}


UINT SYM_EXPORT WINAPI
   SymGetPrivateProfileInt ( LPCSTR lpSection, LPCSTR lpEntry, int nDefault,
                             LPCSTR lpFile )
{
    int     nRet;
    char    szNewPath[256];

    GetWorkingPath ( lpFile, szNewPath );
    nRet = GetPrivateProfileInt ( lpSection, lpEntry, nDefault,
                                  szNewPath );
    FreeWorkingPath ( );
    return nRet;
}


int SYM_EXPORT WINAPI
   SymGetPrivateProfileString ( LPCSTR lpSection, LPCSTR lpEntry, LPCSTR lpDefault,
                                LPSTR lpReturnBuffer, int cbReturnBuffer, LPCSTR lpFile )
{
    int     nRet;
    char    szNewPath[256];

    GetWorkingPath ( lpFile, szNewPath );
    nRet = GetPrivateProfileString ( lpSection, lpEntry, lpDefault,
                                     lpReturnBuffer, cbReturnBuffer, szNewPath );
    FreeWorkingPath ( );
    return nRet;
}

#endif /* #ifndef SYM_WIN */


//========================================================================
// INI Cache functions
// These functions are to load an INI file into a memory buffer and then
// use the values in the memory buffer, rather than repeatedly calling
// the Windows functions to hit the file on disk.  Other than for reasons
// of performance, these functions should always be used for shared INI
// files that can be accessed by multiple processes on a network.  The
// reason for this is that, when a program uses the GetPrivateProfileString()
// API, Windows will
//     first, attempt to open the file READ, WRITE, DENY_WRITE
//     second, if the first attempt fails, attempt to open the file
//         READ, DENY_WRITE
//     finally, if the second attempt fails, attempt to open the file
//         in EXCLUSIVE mode.
// If the third attempt succeeds, any other workstation attempting to access
// the file using the GetPrivateProfileString() API at that time will get a
// sharing violation dialog from Windows (not real nice).
//
// This implementation of these functions is not terribly sophisticated.
// It should be enhanced so that the cache can be time-stamped and
// refreshed if it is too old, and to include write operations.
//========================================================================
//------------------------------------------------------------------------
// CreateINICache
//------------------------------------------------------------------------
LPVOID SYM_EXPORT WINAPI CreateINICache(char far *pszFileName)
{
    auto    char    *lpszOEM;
    auto    WORD    wBufferLength;
    auto    int     nHandle;
    auto    char    far *pData;

    /* Convert filename to OEM and attempt to open ... */
    if (!(lpszOEM = MALLOC(MAX_INTL_STR + 1)))
        return(NULL);

    ANSITOOEM (pszFileName, lpszOEM);

#ifndef SYM_OS2
    nHandle = FileOpen(lpszOEM, OF_READ | OF_SHARE_DENY_NONE);//_O_BINARY | _O_RDONLY |
//	nHandle = _sopen (lpszOEM, _O_BINARY | _O_RDONLY, _SH_DENYNO, _S_IREAD);
#else
	nHandle = sopen (lpszOEM, _O_BINARY | _O_RDONLY, SH_DENYNO, _S_IREAD);
#endif
	FREE(lpszOEM);
	if (nHandle == -1)
		return(NULL);

/*
#ifdef SYM_WIN32
    hIniFileHandle = CreateFile(lpszOEM, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
#else
    nHandle = _sopen (lpszOEM, _O_BINARY | _O_RDONLY, _SH_DENYNO, _S_IREAD);
#endif

    FREE((char near *)lpszOEM);

#ifdef SYM_WIN32
    if (hIniFileHandle == INVALID_HANDLE_VALUE)
#else
    if (nHandle == -1)
#endif
        return(NULL);
*/

	/* Get the length of the file and allocate a buffer for it ... */
	wBufferLength = (WORD) _filelength(nHandle);
	if (wBufferLength == -1)
	{
#ifndef SYM_OS2
        FileClose(nHandle);
//		_close(nHandle);
#else
		close(nHandle);
#endif
		return(NULL);
	}

/*
#ifdef SYM_WIN32
    dwBufferLength = GetFileSize(hIniFileHandle, NULL);
    wBufferLength = (WORD) dwBufferLength;
    if (dwBufferLength == 0xFFFFFFFF)
    {
        CloseHandle(hIniFileHandle);
        return(NULL);
    }
#else
    wBufferLength = (WORD) _filelength(nHandle);
    if (wBufferLength == -1)
    {
        _close(nHandle);
        return(NULL);
    }
#endif
*/

    pData = _fmalloc(wBufferLength + 1);
    if (pData == NULL)
    {
#ifndef SYM_OS2
        FileClose(nHandle);
//		_close(nHandle);
#else
        close(nHandle);
#endif
/*
#ifdef SYM_WIN32
        CloseHandle(hIniFileHandle);
#else
        _close(nHandle);
#endif
*/
        return(NULL);
    }

    /* Load the entire file into the cache, close and bail ... */
    _fmemset(pData, 0x00, wBufferLength + 1);

	FileRead(nHandle, pData, wBufferLength);
//    ReadFile(hIniFileHandle, pData, (DWORD)wBufferLength, &dwBufferLength, NULL);

#ifndef SYM_OS2
    FileClose(nHandle);
//	_close(nHandle);
#else
    close(nHandle);
#endif
/*
#ifdef SYM_WIN32
    CloseHandle(hIniFileHandle);
#else
    _close(nHandle);
#endif
*/
    return(pData);
}


//------------------------------------------------------------------------
// GetINIEntry
//------------------------------------------------------------------------
int SYM_EXPORT WINAPI GetCacheEntry( LPSTR lpszSection, LPSTR lpszEntry,
                                     LPSTR lpszDefault, LPSTR lpszReturnBuffer,
                                     int cbReturnBuffer, void far *pCacheHandle )
{
    auto    LPSTR   lpCachePtr;
    auto    LPSTR   lpLineStart;
    auto    LPSTR   lpWalkPtr;
    auto    LPSTR   lpLineEnd;
    auto    LPSTR   lpBlock;
    auto    int     nBlockLength;
    auto    char *  lpszLineBuffer;
    auto    char *  lpLineBuffer;
    auto    BOOL    bFoundSection;

    lpCachePtr      = pCacheHandle;
    lpszLineBuffer  = MALLOC(MAX_INI_LINE + 1);
    bFoundSection   = (BOOL) (lpszSection == NULL);
    lpBlock         = lpszReturnBuffer;
    nBlockLength    = 0;

    while (*lpCachePtr != EOF_CHAR &&
           *lpCachePtr != '\0')
    {
        /* Get the next line of the file in C string format ... */
        lpLineBuffer = lpszLineBuffer;
        while (*lpCachePtr != CR_CHAR &&
               *lpCachePtr != EOF_CHAR)
            *(lpLineBuffer++) = *(lpCachePtr++);

        if (*lpCachePtr == CR_CHAR)
            lpCachePtr++;
        if (*lpCachePtr == LF_CHAR)
            lpCachePtr++;
        *lpLineBuffer = EOS_CHAR;

        // process the next line ...
        while ( *lpszLineBuffer != EOS )
        {
            // ignore leading whitespace
            lpLineStart = lpszLineBuffer;
            while (*lpLineStart == SPACE_CHAR || *lpLineStart == TAB_CHAR)
                lpLineStart++;

            // ignore blank lines
            if (!(*lpLineStart))
                continue;

            // if this line contains a comment, continue
            if (*lpLineStart == COMMENT_CHAR)
                break;

            // is this the start of a new section?
            if (*lpLineStart == SECTIONSTART_CHAR)
            {
                if (lpszSection)
                {
                    // we went past the section
                    if (bFoundSection)
                    {
                        if (lpszEntry)
                        {
                            // return the default
                            MEMCPY(lpszReturnBuffer, lpszDefault, min((int) STRLEN(lpszDefault) + 1, cbReturnBuffer));
                            FREE((char near *)lpszLineBuffer);
                            return (STRLEN(lpszReturnBuffer));
                        }
                        else
                        {
                            nBlockLength += 2;
                            *(++lpBlock) = EOS_CHAR;
                            *(++lpBlock) = EOS_CHAR;
                            FREE((char near *)lpszLineBuffer);
                            return (nBlockLength);
                        }
                    }

                    lpLineStart++;
                    lpWalkPtr = lpLineStart;
                    while (*lpWalkPtr)
                    {
                        if (*lpWalkPtr == SECTIONEND_CHAR)
                        {
                            *lpWalkPtr = EOS_CHAR;
                            break;
                        }
                        else
                            lpWalkPtr++;
                    }

                    // is this the section we are looking for?
                    bFoundSection = (BOOL) (STRICMP(lpLineStart, lpszSection) == 0);
                    break;
                }
            }
            else
            {
                // if we are interested in this section, check the keys
                if (bFoundSection)
                {
                    while (*lpLineStart == SPACE_CHAR || *lpLineStart == TAB_CHAR)
                        lpLineStart++;

                    lpWalkPtr = lpLineStart;
                    while (*lpWalkPtr)
                    {
                        if (*lpWalkPtr == EQUAL_CHAR)
                        {
                            *lpWalkPtr = EOS_CHAR;
                            lpLineEnd = lpWalkPtr;
                            while (*(lpLineEnd - 1) == SPACE_CHAR || *(lpLineEnd - 1) == TAB_CHAR)
                            {
                                *(lpLineEnd - 1) = EOS_CHAR;
                                lpLineEnd--;
                            }
                            break;
                        }
                        else
                            lpWalkPtr++;
                    }

                    if (lpszEntry)
                    {
                        // if this is the entry we want, use it!
                        if (STRICMP(lpLineStart, lpszEntry) == 0)
                        {
                            lpWalkPtr++;
                            while (*lpWalkPtr == SPACE_CHAR || *lpWalkPtr == TAB_CHAR)
                                lpWalkPtr++;

                            if (*lpWalkPtr == QUOTE_CHAR)
                                lpWalkPtr++;

                            lpLineEnd = lpWalkPtr + STRLEN(lpWalkPtr);
                            if (lpWalkPtr != lpLineEnd)
                            {
                                while (*(lpLineEnd - 1) == SPACE_CHAR || *(lpLineEnd - 1) == TAB_CHAR)
                                {
                                    *(lpLineEnd - 1) = EOS_CHAR;
                                    lpLineEnd--;
                                }
                            }

                            if (*(lpLineEnd - 1) == QUOTE_CHAR)
                                *(lpLineEnd - 1) = EOS_CHAR;

                            MEMCPY(lpszReturnBuffer, lpWalkPtr, min((int) STRLEN(lpWalkPtr) + 1, cbReturnBuffer));
                            FREE((char near *)lpszLineBuffer);
                            return (STRLEN(lpszReturnBuffer));
                        }
                        else
                            break;
                    }
                    else
                    {
                        if (STRLEN(lpLineStart))
                        {
                            STRCPY(lpBlock, lpLineStart);
                            nBlockLength += STRLEN(lpBlock) + 1;
                            lpBlock      += STRLEN(lpBlock) + 1;
                        }
                    }
                }
                else
                    break;
            }
        }                               // while ( *lpszLineBuffer != EOS )
    }

    // return the default
    FREE((char near *)lpszLineBuffer);
    MEMCPY(lpszReturnBuffer, lpszDefault, min((int) STRLEN(lpszDefault) + 1, cbReturnBuffer));
    return (STRLEN(lpszReturnBuffer));
}

//------------------------------------------------------------------------
// ReleaseINICache
//------------------------------------------------------------------------
void SYM_EXPORT WINAPI ReleaseINICache(void far *pCacheHandle)
{
    _ffree(pCacheHandle);
    return;
}

//------------------------------------------------------------------------
// GetCacheSection
//
// Description:  This routine retrieves all the entries for a section.
// Each entry is terminated by the null character in the return buffer.
// An extra null character designates the end of the entries.
//------------------------------------------------------------------------
DWORD SYM_EXPORT WINAPI GetCacheSection(
	LPCSTR	 lpszSection,
	LPSTR	 lpReturnString,
	DWORD	 dwSize,
	void far *pCacheHandle
)
{
	LPSTR lpTempBuffer = NULL;
	LPSTR lpEntryPtr;
	LPSTR lpWalkPtr;
	char  szSection[256];
	DWORD dwBytesWritten = 0;
	LPSTR lpTempBuffPtr;
	WORD  wCacheSize = STRLEN(pCacheHandle)+1;

	lpTempBuffer = MemAllocPtr(LPTR, wCacheSize);

	if (!lpTempBuffer)
		return 0;

	MEMSET(lpTempBuffer, 0, wCacheSize);
	lpTempBuffPtr = lpTempBuffer;
										// Enclose the section name in
										// brackets and look for it
	SPRINTF(szSection, "[%s]", lpszSection);

	lpWalkPtr = STRSTR(pCacheHandle, szSection);

	if (!lpWalkPtr)
		goto Exit;
										// Check for '\r', '\n', and '\0'
	while ((*lpWalkPtr != 0x0D) && (*lpWalkPtr != 0x0A) &&
		   (*lpWalkPtr != 0x00))
		++lpWalkPtr;
										// We hit one of the special
										// characters.	Stop and decide what
										// to do.
StartAgain:
	if (*lpWalkPtr == 0x0D) 			// '\r': Check to see that the next
										// next character is '\n'
	{
		if (*(++lpWalkPtr) != 0xA)
			goto Exit;
										// Go to the start of the entry
		lpEntryPtr = ++lpWalkPtr;
		while ((*lpWalkPtr != 0x0D) && (*lpWalkPtr != 0x0A) &&
			   (*lpWalkPtr != 0x00) && (*lpWalkPtr != 0x5B) &&
			   (*lpWalkPtr != 0x5D))
			++lpWalkPtr;

		dwBytesWritten += (DWORD) (lpWalkPtr - lpEntryPtr);
		STRNCPY(lpTempBuffPtr, lpEntryPtr,
				(size_t)(DWORD)(lpWalkPtr - lpEntryPtr));
		lpTempBuffPtr += lpWalkPtr - lpEntryPtr;
		*lpTempBuffPtr = '\0';
		++lpTempBuffPtr;
		++dwBytesWritten;

		if ((*lpWalkPtr == 0x00) || (*lpWalkPtr == 0x5B) ||
			(*lpWalkPtr == 0x5D))
			goto Exit;

		goto StartAgain;
	}
	else if (*lpWalkPtr == 0x0A)		// '\n'
		goto Exit;
	else if (*lpWalkPtr == 0x00)			// '\0'
	{
		*lpTempBuffer = '\0';
		goto Exit;
	}

Exit:
										// If we exceeded the limit
										// truncate the return string
	if (dwBytesWritten > dwSize)
	{
		MEMCPY(lpReturnString, lpTempBuffer, (size_t)dwSize);
		lpReturnString[dwSize-2] = '\0';
		lpReturnString[dwSize-1] = '\0';
	}
	else
		MEMCPY(lpReturnString, lpTempBuffer, (size_t)dwBytesWritten);

	MemFreePtr(lpTempBuffer);
	return dwBytesWritten;
}

//------------------------------------------------------------------------
// GetCacheSectionNames
//
// Description:  This routine retrieves all the section names in the
// cache.  The section names are stripped of the brackets and delimited
// by the null character in the return buffer.	An extra null character
// designates the end of the entries.
//------------------------------------------------------------------------
DWORD SYM_EXPORT WINAPI GetCacheSectionNames(
	LPSTR	 lpReturnString,
	DWORD	 dwSize,
	void far *pCacheHandle
)
{
	LPSTR lpTempBuffer = NULL;
	LPSTR lpEntryPtr;
	LPSTR lpWalkPtr;
	DWORD dwBytesWritten = 0;
	DWORD dwCopyBytes;
	LPSTR lpTempBuffPtr;
	WORD  wCacheSize = STRLEN(pCacheHandle)+1;

	lpTempBuffer = MemAllocPtr(LPTR, wCacheSize);

	if (!lpTempBuffer)
		return 0;

	MEMSET(lpTempBuffer, 0, wCacheSize);
	lpTempBuffPtr = lpTempBuffer;

										// Init the entry and walk
										// pointers
	lpEntryPtr = pCacheHandle;
	lpWalkPtr  = pCacheHandle;

	while (*lpWalkPtr)
	{
		lpWalkPtr = STRSTR(lpWalkPtr, "[");

		if (!lpWalkPtr)
			break;

		lpEntryPtr = lpWalkPtr;

		lpWalkPtr = STRSTR(lpWalkPtr, "]");

		if (!lpWalkPtr)
			break;

		dwCopyBytes = ((DWORD) (lpWalkPtr - lpEntryPtr)) - 1;

		STRNCPY(lpTempBuffPtr, ++lpEntryPtr, (size_t)dwCopyBytes);

		lpTempBuffPtr += lpWalkPtr - lpEntryPtr;
		*lpTempBuffPtr = '\0';
		++lpTempBuffPtr;
		dwBytesWritten += dwCopyBytes + 1;
	}
										// Terminate with an extra null
	*(++lpTempBuffPtr) = '\0';
	++dwBytesWritten;

										// If we exceeded the limit
										// truncate the return string
	if (dwBytesWritten > dwSize)
	{
		MEMCPY(lpReturnString, lpTempBuffer, (size_t)dwSize);
		lpReturnString[dwSize-2] = '\0';
		lpReturnString[dwSize-1] = '\0';
	}
	else
		MEMCPY(lpReturnString, lpTempBuffer, (size_t)dwBytesWritten);

	MemFreePtr(lpTempBuffer);
	return dwBytesWritten;
}
