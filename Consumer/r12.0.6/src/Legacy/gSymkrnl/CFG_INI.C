/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/cfg_ini.c_v   1.1   09 May 1996 12:21:28   MZAREMB  $ *
 *                                                                      *
 * Description:                                                         *
 *      Functions to manage text-INI configuration files.               *                                                                      *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/cfg_ini.c_v  $ *
// 
//    Rev 1.1   09 May 1996 12:21:28   MZAREMB
// Added SYM_WIN #ifdef to CopySection routine to avoid having the NOEM...()
// function initialize a szz buffer to a single string.
// 
//    Rev 1.0   26 Jan 1996 20:21:08   JREARDON
// Initial revision.
// 
//    Rev 1.40   11 Jul 1995 10:50:22   SCOTTP
// The getprivateprofilestring will now actually return
// and error.  Instead of just reporting nothing as before
// 
//    Rev 1.39   02 Sep 1994 12:53:24   JMILLARD
// fix hidden low-memory party in CopySection
// 
//    Rev 1.38   24 Aug 1994 16:28:04   DLEVITON
// Under Windows, use PrivateProfile APIs to avoid cached data conflict.
// 
//    Rev 1.37   24 Aug 1994 15:26:02   DLEVITON
// ConfigReg calls.
//
//    Rev 1.36   05 May 1994 12:59:40   BRAD
// Needed stdio.h
//
//    Rev 1.35   05 May 1994 12:10:46   BRAD
// Remove SYM_WIN stuff
//
//    Rev 1.34   03 May 1994 18:53:00   BRAD
// Remove platform-specific stuff
//
//    Rev 1.33   30 Mar 1994 11:49:10   PATRICKM
// Checked in from latest trunk src
//
//    Rev 1.32   15 Mar 1994 12:35:12   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.31   25 Feb 1994 12:22:48   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.30   02 Sep 1993 20:20:00   BRAD
// If NULL passed as data for CreateKeyname, then just don't create the line.
//
//    Rev 1.29   12 May 1993 09:12:16   ED
// [Fix] Dumb, dumb, dumb.
//
//    Rev 1.28   12 May 1993 09:10:28   ED
// [Fix] Advanced to the next line when searching keynames...
//
//    Rev 1.27   12 May 1993 08:33:02   ED
// [Enh] Added keyname data search support to allow keyname entry matching
// based on the specified search text.  A series of functions, all with
// "Ex" in their names, were added to allow search text to be specified,
// and several flags are available in SYMCFG.H to configure the search.
// If no search is desired, the original functions can be used, or the
// search text parameter can be set to NULL.
//
//    Rev 1.26   07 May 1993 15:51:10   EHU
// Added some includes for SYM_NLM for new platform.h, platnlm.h, file.h,
// and xapi.h.
//
//    Rev 1.25   26 Apr 1993 09:56:52   ED
// [Enh] Added support for the CFG_INI_MULTIPLE flag, which allows multiple
// entries with the same keyname to exist in a section.
//
//
//    Rev 1.24   19 Apr 1993 18:24:52   BRAD
// Fixed a few bugs in CopySection().  Most notable are:
// 1) Wouldn't work properly for the end of a file.  Would
//    run off end of buffer.
// 2) If white space in some circumstances, would be in
//    an infinite loop.
//
//
//    Rev 1.23   14 Apr 1993 12:34:34   ED
// [Fix] CopySection didn't.
//
//    Rev 1.22   12 Apr 1993 07:41:10   ED
// Modified ConfigIniFileQuerySection
//
//    Rev 1.21   22 Mar 1993 21:47:10   BRAD
// Added ability for private INI files.  Also fixed
// bug where a '\0' was being added just after the
// 'Symantec Configuration File' line.
//
//    Rev 1.20   16 Mar 1993 13:18:32   ED
// Changed CopySection so blank and commented lines are eliminated, and
// leading white space for all entries is removed.
//
//    Rev 1.19   09 Mar 1993 16:14:08   DLEVITON
// AnsitoOEM support had broken the section-delete feature, causing GPFs.
// Fixed it.
//
//
//    Rev 1.18   22 Feb 1993 10:29:22   ED
//
//    Rev 1.17   22 Feb 1993 09:05:54   ED
// Added ConfigIniQuerySection, to determine if a section exists and/or
// if the section contains any entries.
//
//    Rev 1.16   19 Feb 1993 15:39:48   ED
// Added ConfigMinimizeText, to standardize cleaning up of .INI strings
//
//    Rev 1.15   19 Feb 1993 13:08:54   ED
// Added support for dumping entire sections in a Get call if the entry
// name is set to NULL.  This works exactly like GetProfileString does
// in Windows.  See CopySection for details.
//
//    Rev 1.14   12 Feb 1993 15:25:58   ENRIQUE
// No change.
//
//    Rev 1.13   08 Feb 1993 11:08:44   MARKL
// Modified ConfigIniFileGet() and ConfigIniFileSet() to handle ANSI to OEM
// and OEM to ANSI issues in windows.
//
//    Rev 1.12   07 Feb 1993 11:39:32   DLEVITON
// Fixes for multiple concurrent users of the same file
//
//    Rev 1.11   28 Jan 1993 14:48:16   DLEVITON
// Fixed several functions which were not behaving as documented,
// particularly NextLine, which broke several others. Evidently, I am
// the first one to give this module much of a workout.
//
//
//    Rev 1.10   27 Jan 1993 18:06:22   DLEVITON
// Changed ConfigIniFileSet so that it behaves like the Windows call
// WritePrivateProfileString in the following respect: if lpEntry is
// NULL, the entire section and all its entries are deleted.
//
//
//    Rev 1.9   15 Dec 1992 06:56:30   ED
// Cast ERR to a UINT so the DOS version would compile
//
//    Rev 1.8   12 Oct 1992 16:15:34   EHU
// SYM_NLM changes.
//
//
//    Rev 1.7   05 Oct 1992 16:35:04   EHU
// Moved prototypes of exported functions to symcfg.h.
//
//
//    Rev 1.6   29 Sep 1992 16:45:04   EHU
// Made some private functions public and renamed them to be more descriptive.
// These take LPCFGFILE as an argument so you aren't limited to the built-in
// filenames.
//
//    Rev 1.5   18 Sep 1992 14:52:02   ED
//
//
//    Rev 1.4   18 Sep 1992 09:54:54   ED
//
//
//    Rev 1.3   18 Sep 1992 09:26:30   ED
// Fixed CreateSection
//
//    Rev 1.2   14 Sep 1992 14:53:12   MARKK
// Added module_name
//
//    Rev 1.1   08 Sep 1992 09:05:50   ED
// Final, version 1.0
//
//    Rev 1.0   03 Sep 1992 09:10:16   ED
// Initial revision.
 ************************************************************************/

#ifdef SYM_NLM
#include <ctype.h>
#endif

#include <stdio.h>
#include "platform.h"
#ifdef SYM_WIN32
#include "winreg.h"
#endif
#include "xapi.h"
#include "file.h"
#include "symcfg.h"

MODULE_NAME;


#define END_OF_SECTION  TRUE
#define END_OF_FILE     FALSE

#define IS_COMMENT(c)           ((c) == ';')

                                        // Header line added to newly created
                                        // .INI files.  All it does is give
                                        // us something to work with
static  char    szHeaderLine[]  = "; Symantec Configuration File\r\n";
static  char    szCRLF[]        = "\r\n";

extern  CFGFILE rLocalText;
extern  CFGFILE rGlobalText;


#define IsSearchMode(c)         (BOOL)((c)->wFlags & (CFG_INI_FINDMINIMUM | CFG_INI_FINDFILE | CFG_INI_FINDEXACT))

        // ------------------------------------------------------------
        // internal structure used to cut down on passing arguments
        // ------------------------------------------------------------

typedef struct
    {
    HGLOBAL     hBlock;                 // handle for the memory block
    LPBYTE      lpBuffer;               // pointer to current buffer position
    LPBYTE      lpBufferStart;          // pointer to start of buffer
    LPBYTE      lpBufferEnd;            // pointer to end of buffer + 1
    UINT        wSize;                  // size of the buffer
    UINT        wLineStart;             // offset of start of current line
    UINT        wTextStart;             // offset of text following '='
    UINT        wLastLine;              // offset of last line in section
    BOOL        bCreating;              // TRUE if creating an entry
    } INIREC, FAR *LPINIREC;


UINT    LOCAL PASCAL FindSection (LPINIREC lpINI, LPCFGTEXTREC lpConfig);
UINT    LOCAL PASCAL CreateSection (LPINIREC lpINI, LPCFGTEXTREC lpConfig);
void    LOCAL PASCAL GetSectionName (LPSTR lpToken, LPCFGTEXTREC lpConfig);
UINT    LOCAL PASCAL CreateKeyname (LPINIREC lpINI, LPCFGTEXTREC lpConfig,
                                LPSTR lpData, BOOL bUseLastLine);
UINT    LOCAL PASCAL GetKeynameEntry (LPINIREC lpINI, LPCFGTEXTREC lpConfig, LPSTR lpMatch);
UINT    LOCAL PASCAL SearchKeyname (LPINIREC lpINI, LPCFGTEXTREC lpConfig, LPSTR lpMatch);
UINT    LOCAL PASCAL FindKeyname (LPINIREC lpINI, LPCFGTEXTREC lpConfig);
UINT    LOCAL PASCAL NextLine (LPINIREC lpINI);
UINT    LOCAL PASCAL ModifyText (LPINIREC lpINI, UINT wOffset, LPBYTE lpData);
UINT    LOCAL PASCAL CopySection (LPINIREC lpINI, LPSTR lpData, UINT wSize);

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Initializes a text configuration file.  If it is a new file,  *
 *      the file header is created.                                     *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/19/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigTextInit (LPCFGFILE lpFile)
{
    LPBYTE      lpHeader;
    UINT        wErr = CFG_SUCCESS;
    UINT        wLen;
                                        // don't do anything for a global file
    if (lpFile->wFlags & CFG_GLOBAL_FILE)
        return (CFG_SUCCESS);

    if (lpFile->dwSize == 0L)         // is this a brand new file?
        {
        wLen = sizeof(szHeaderLine) - 1;
                                        // realloc the block to hold a header
        lpFile->hMem = MemReAlloc (lpFile->hMem, wLen, GHND);

        if (lpFile->hMem)
            {
            lpFile->dwSize = (DWORD) wLen;

                                        // copy the header line to the file,
                                        // for no other reason than to make
                                        // the size greater than zero, so
                                        // memory will lock!  Otherwise, we
                                        // can never add anything to it!
            if (lpHeader = MemLock (lpFile->hMem))
                {
                MEMCPY(lpHeader, szHeaderLine, wLen);

                MemUnlock (lpFile->hMem, lpHeader);
                }
            }
        else
            wErr = CFG_MEMORY_ERROR;
        }

    return(wErr);
}   // ConfigTextInit


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Copies the text entry that follows the keyname in a text        *
 *      configuration file.                                             *
 *                                                                      *
 *      This function actually calls ConfigIniFileGet, trying the       *
 *      local and global files in the specified order.                  *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      NOERR           Keyname found                                   *
 *      ERR             Keyname not found                               *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigIniGet (
    LPCFGTEXTREC        lpConfig,
    LPSTR               lpData,
    UINT                wSize)
{
    return ConfigIniGetEx (lpConfig, NULL, lpData, wSize);
}


UINT SYM_EXPORT WINAPI ConfigIniGetEx (
    LPCFGTEXTREC        lpConfig,
    LPSTR               lpMatch,
    LPSTR               lpData,
    UINT                wSize)
{
    BOOL        bLocal = FALSE;
    BOOL        bGlobal = FALSE;
    UINT        wErr = (UINT) ERR;

    if (lpConfig->wFlags & CFG_LOCAL_FIRST)
        goto Local_First;

    while (TRUE)
        {                               // read the global config file
        if (!bGlobal && wErr != NOERR)
            {
            if (rGlobalText.dwSize > 0)
                wErr = ConfigIniFileGetEx (&rGlobalText, lpConfig, lpMatch, lpData, wSize);

            bGlobal = TRUE;
            }
                                        // read the local config file
        else if (!bLocal && wErr != NOERR)
            {
Local_First:
            if (rLocalText.dwSize > 0)
                wErr = ConfigIniFileGetEx (&rLocalText, lpConfig, lpMatch, lpData, wSize);

            bLocal = TRUE;
            }
        else                            // we tried both, so quit
            break;
        }

    return (wErr);
}


UINT SYM_EXPORT WINAPI ConfigIniFileGet (
    LPCFGFILE           lpFile,
    LPCFGTEXTREC        lpConfig,
    LPSTR               lpData,
    UINT                wSize)
{
    return ConfigIniFileGetEx (lpFile, lpConfig, NULL, lpData, wSize);
}


UINT SYM_EXPORT WINAPI ConfigIniFileGetEx (
    LPCFGFILE           lpFile,
    LPCFGTEXTREC        lpConfig,
    LPSTR               lpMatch,
    LPSTR               lpData,
    UINT                wSize)
{
#ifdef SYM_WIN
    if (GetPrivateProfileString( lpConfig->lpSection, lpConfig->lpEntry, "", 
	     lpData, wSize, lpFile->szPath ))
	     return NOERR;
    else
	     return ERR;
#else
    INIREC      rINI;
    LPBYTE      lpTemp;
    UINT        wFileSize;
    UINT        wRet = (UINT) ERR;
    BYTE        OEMData[MAX_INI_LINE + 1];
    LPSTR       lpOEMData;
    BOOL        bTranslated = FALSE;


    rINI.hBlock = lpFile->hMem;
    rINI.wSize  = (UINT) lpFile->dwSize;
    lpOEMData   = (LPSTR) OEMData;

    if (rINI.lpBufferStart = (LPBYTE) MemLock (rINI.hBlock))
        {
        rINI.lpBuffer = rINI.lpBufferStart;
        rINI.lpBufferEnd = rINI.lpBuffer + rINI.wSize;

        if (FindSection (&rINI, lpConfig) == NOERR)
            if (NextLine (&rINI) == NOERR)
                {
                                        // if lpEntry is NULL, copy all
                                        // entries in the section to the
                                        // buffer
                if (lpConfig->lpEntry == NULL)
                    {
                    CopySection (&rINI, lpData, wSize);
                    bTranslated = TRUE; // don't OEM/ANSI again
                    wRet = NOERR;
                    }

                else if (GetKeynameEntry (&rINI, lpConfig, lpMatch) == NOERR)
                    {
                                        // point to the entry text
                    lpTemp = rINI.lpBufferStart + rINI.wTextStart;

                                        // copy the text up to the EOL, or
                                        // up to the end of the file, or
                                        // to the limit specified
                    wFileSize = rINI.wSize;
                    wSize--;
                    while (wSize-- && wFileSize-- && !IS_EOL (*lpTemp))
                        *lpOEMData++ = *lpTemp++;

                    wRet = NOERR;
                    }
                }

        MemUnlock (rINI.hBlock, rINI.lpBuffer);
        }

    if (wRet == NOERR && !bTranslated)
        {
                                // terminate the string if we changed it
                                // we promised not to touch the buffer if
                                // we fail the function
        *lpOEMData = '\0';
                                // If in windows, convert the OEM data to
                                // ANSI.
        NOemToAnsi ((LPSTR) OEMData, lpData);
        }

    return (wRet);
#endif    
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Copies all entries in a section to the specified buffer.  Each  *
 *      string will be null-terminated, with the final string ending    *
 *      with two zero-termination characters.  If the supplied          *
 *      destination buffer is too small to hold all the strings, the    *
 *      last string will be truncated and followed by two zero-         *
 *      termination characters.                                         *
 *                                                                      *
 *      NOTE: Blank and commented lines are eliminated, and leading     *
 *            white space for all entries is removed.                   *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 02/19/1993 ED Function Created.                                      *
 ************************************************************************/

UINT LOCAL PASCAL CopySection (LPINIREC lpINI, LPSTR lpData, UINT wSize)
{
    LPBYTE      lpTemp;
    BOOL        bInLeading;
    char        ch;
#ifdef  SYM_WIN
    LPSTR       lpData2 = lpData;
#endif
    UINT        wLen = 0;
    BOOL        bCopy = (BOOL)(lpData != NULL);

                                        // point to start of the section
    lpTemp = lpINI->lpBuffer;

    bInLeading = TRUE;                  // TRUE to eliminate leading white space

    wSize -= 2;                         // Need space for 2 EOSs

    while ((wLen < wSize) &&
           (lpTemp < lpINI->lpBufferEnd))
        {
        ch = *lpTemp;
                                        // burn the white space
        if (bInLeading && IS_WHITE (ch))
            {
            lpTemp++;                   // adjust the source pointer
            continue;
            }
                                        // Found EOL or commented line
        if ((bInLeading && IS_COMMENT(ch)) ||
            IS_EOL (ch))
            {
                                        // move to the next line
            if (NextLine (lpINI) == ERR)
                break;
                                        // found a section name or EOF
            if (*lpINI->lpBuffer == EOS || *lpINI->lpBuffer == '[')
                break;
                                        // if TRUE, this is a blank line
                                        // or a line with a comment
            if (!bInLeading)
                {
                if (bCopy)
                    *lpData++ = EOS;    // terminate the current string.
                wLen++;
                }

            lpTemp = lpINI->lpBuffer;   // adjust buffer pointer

            bInLeading = TRUE;          // we're back into leading white
            }
        else
            {
            if (bCopy)
                *lpData++ = *lpTemp;    // copy the character
            wLen++;                     // count the character

            lpTemp++;                   // adjust the source pointer

            bInLeading = FALSE;         // we're out of the leading white
            }
        }

    if (bCopy)
        {
        *lpData++ = EOS;                // add two null-terminators to end
        *lpData++ = EOS;
        }
                                        // Only add 2, if some characters
                                        // in buffer
    if ( wLen )
        wLen += 2;                      // count the ending null terminators

                                        // ANSI the whole buffer at once
                                        // must check for bcopy, otherwise
                                        // this parties in low memory, even
                                        // if wlen is 0! The party is the 
                                        // terminating NULL that gets written.
    if (bCopy)
        {
#ifdef  SYM_WIN
        NOemToAnsiBuff (lpData2, lpData2, wLen);
#endif        
        }

    return (wLen);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Copies the text entry that follows the keyname in a text        *
 *      configuration file.                                             *
 *                                                                      *
 *      This function actually calls ConfigIniFileSet, trying the         *
 *      local and global files in the specified order.                  *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      NOERR           Keyname found                                   *
 *      ERR             Keyname not found                               *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigIniSet (LPCFGTEXTREC lpConfig, LPSTR lpData)
{
    return (ConfigIniFileSetEx(&rLocalText, lpConfig, NULL, lpData));
}

UINT SYM_EXPORT WINAPI ConfigIniSetEx (LPCFGTEXTREC lpConfig, LPSTR lpMatch, LPSTR lpData)
{
    return (ConfigIniFileSetEx(&rLocalText, lpConfig, lpMatch, lpData));
}


UINT SYM_EXPORT WINAPI ConfigIniFileSet (
    LPCFGFILE     lpFile,
    LPCFGTEXTREC  lpConfig,
    LPSTR         lpData)
{
    return(ConfigIniFileSetEx(lpFile, lpConfig, NULL, lpData));
}


UINT SYM_EXPORT WINAPI ConfigIniFileSetEx (
    LPCFGFILE     lpFile,
    LPCFGTEXTREC  lpConfig,
    LPSTR         lpMatch,
    LPSTR         lpData)
{
#ifndef SYM_WIN
    INIREC      rINI;
    BYTE        ch;
#endif    
    BYTE        OEMData[MAX_INI_LINE + 1];
    LPSTR       lpOEMData;
                                        // If in windows, convert from ANSI
                                        // to OEM character set.
    lpOEMData = (LPSTR) OEMData;
                                        // lpData is NULL to delete section.
    if (lpData)
        {
        NAnsiToOem (lpData, lpOEMData);
        }
    else lpOEMData = NULL;

#ifdef SYM_WIN
    WritePrivateProfileString( lpConfig->lpSection, lpConfig->lpEntry, 
    	lpOEMData, lpFile->szPath );
#else
    rINI.hBlock = lpFile->hMem;
    rINI.wSize  = (UINT) lpFile->dwSize;
    rINI.bCreating = FALSE;

    if (rINI.lpBufferStart = (LPBYTE) MemLock (rINI.hBlock))
        {
        rINI.lpBuffer = rINI.lpBufferStart;
        rINI.lpBufferEnd = rINI.lpBuffer + rINI.wSize;

                                        // find the section in the buffer.
        if (FindSection (&rINI, lpConfig) == ERR && lpConfig->lpEntry)
            {
                                        // if not found, create the section
            CreateSection (&rINI, lpConfig);
                                        // create the keyname and entry
            CreateKeyname (&rINI, lpConfig, lpOEMData, END_OF_FILE);
            }
                                        // found the section
        else if (lpConfig->lpEntry)
            {
                                        // attempt to go to the next line
                                        // after the section name
            if (NextLine (&rINI) == NOERR)
                {
                                        // if we have search text, try to
                                        // find the keyname by searching
                                        // the keyname data
                if (lpMatch)
                    {
                    if (SearchKeyname (&rINI, lpConfig, lpMatch) == NOERR)
                        goto By_George_Ive_Got_It;
                    else
                        goto Its_Not_There;
                    }
                                            // if we are allowing multiple
                                            // keyname entries, don't search
                                            // for an existing entry.  just
                                            // create a new one
                else if (!(lpConfig->wFlags & CFG_INI_MULTIPLE) &&
                        FindKeyname (&rINI, lpConfig) == NOERR)
                    {
By_George_Ive_Got_It:
                                            // if the entry is NULL, delete the
                                            // line from the file
                    if (lpOEMData == NULL)
                        ModifyText (&rINI, rINI.wLineStart, lpOEMData);

                    else                    // replace the text after the keyname
                        ModifyText (&rINI, rINI.wTextStart, lpOEMData);

                    }
                else                        // create the keyname if not found
                    {
Its_Not_There:
                    CreateKeyname (&rINI, lpConfig, lpOEMData, END_OF_SECTION);
                    }
                }
                                            // otherwise, we hit the end of the
                                            // file, so tack the keyname onto
                                            // the end
            else
                CreateKeyname (&rINI, lpConfig, lpOEMData, END_OF_FILE);
            }
                                            // found the section and want to
                                            // delete it
        else
            {
                                            // delete section
            rINI.wLineStart = (UINT) (rINI.lpBuffer - rINI.lpBufferStart);
            do {
                                            // delete line
                ModifyText (&rINI, rINI.wLineStart, NULL);
                if (rINI.lpBuffer)
                    rINI.lpBuffer += rINI.wLineStart;
                                            // delete all lines following until
                                            // next section or EOF
                while (rINI.lpBuffer && rINI.lpBuffer < rINI.lpBufferEnd)
                    {
                                            // get rid of leading white space
                    if (!IS_WHITE (ch = *rINI.lpBuffer))
                        break;
                    rINI.lpBuffer++;
                    }
                if (ch == '\0' || ch == '[')    // found a section name or EOF
                    break;
                } while (rINI.lpBuffer && rINI.lpBuffer < rINI.lpBufferEnd);
            }

        MemUnlock (rINI.hBlock, rINI.lpBuffer);

        lpFile->hMem = rINI.hBlock;
        lpFile->dwSize = rINI.wSize;
        lpFile->bDirty = TRUE;           // mark the file as "dirty"
        }
#endif
    return (NOERR);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Finds a section name.                                           *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      NOERR           Keyname found                                   *
 *      ERR             Keyname not found                               *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT LOCAL PASCAL FindSection (LPINIREC lpINI, LPCFGTEXTREC lpConfig)
{
    char        szToken [MAX_INI_SECTION + 3];
    UINT        wLen;
    BYTE        ch;

                                        // get the bracketed section name
    GetSectionName ((LPSTR) szToken, lpConfig);

    wLen = STRLEN (szToken);

    while (lpINI->lpBuffer && lpINI->lpBuffer < lpINI->lpBufferEnd)
        {
        ch = *lpINI->lpBuffer;

        if (IS_WHITE (ch))              // get rid of leading white space
            {
            lpINI->lpBuffer++;
            continue;
            }

        if (ch == '[')                  // found a section name
            {
                                        // prevent a GPF by making sure
                                        // we can do a compare on the buffer
            if ((UINT) (lpINI->lpBufferEnd - lpINI->lpBuffer) >= wLen)
                {
                if (STRNICMP (lpINI->lpBuffer, szToken, wLen) == 0)
                    {
                    return (NOERR);
                    }
                }
            }

        NextLine (lpINI);
        }

    return ((UINT) ERR);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Creates a section in a configuration file                       *                                                                      *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *      NOERR           Keyname found                                   *
 *      ERR             Keyname not found                               *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT LOCAL PASCAL CreateSection (LPINIREC lpINI, LPCFGTEXTREC lpConfig)
{
    char        szToken [MAX_INI_SECTION + 3];

    STRCPY (szToken, szCRLF);           // insert a leading CR/LF

                                        // add the bracketed section name
    GetSectionName ((LPSTR) &szToken[2], lpConfig);

    lpINI->bCreating = TRUE;

    return (ModifyText (lpINI, lpINI->wSize, (LPSTR) szToken));
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Creates a bracketed section name.                               *                                                                      *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

void LOCAL PASCAL GetSectionName (LPSTR lpToken, LPCFGTEXTREC lpConfig)
{
    SPRINTF (lpToken, "[%s]", lpConfig->lpSection);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Creates a keyname entry in the file.                            *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      NOERR           Keyname found                                   *
 *      ERR             Keyname not found                               *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT LOCAL PASCAL CreateKeyname (
    LPINIREC            lpINI,
    LPCFGTEXTREC        lpConfig,
    LPSTR               lpData,
    BOOL                bUseLastLine)
{
    char        szToken [MAX_INI_LINE + 1];
    UINT        wOffset;


                                        // If no value to write, then return
    if ( lpData == NULL )
        return (NOERR);

    SPRINTF (szToken, "%s=%s", lpConfig->lpEntry, lpData);

                                        // do we tack it on at the current
                                        // position, or at the end of the file?
    wOffset = (bUseLastLine == END_OF_SECTION) ? lpINI->wLastLine :
                                                 lpINI->wSize;

    lpINI->lpBuffer = lpINI->lpBufferStart + wOffset;
    lpINI->bCreating = TRUE;

    return (ModifyText (lpINI, wOffset, (LPSTR) szToken));
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Finds a keyname entry in a section.  The buffer pointer must    *
 *      point to the first line after the section name.                 *
 *                                                                      *
 *      Sets the following fields in the INIREC if found:               *
 *                                                                      *
 *      wTextStart      Offset of the text entry (points after the '=') *
 *      wLineStart      Offset of the line                              *
 *      wLastLine       Offset of the last line before the found line.  *
 *                                                                      *
 *      Sets the following fields in the INIREC if not found:          *
 *                                                                      *
 *      wTextStart      -1                                              *
 *      wLineStart      -1                                              *
 *      wLastLine       Offset of the last line in the section, which   *
 *                      is where new entries should be added.           *                               *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      NOERR           Keyname found                                   *
 *      ERR             Keyname not found                               *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT LOCAL PASCAL GetKeynameEntry (LPINIREC lpINI, LPCFGTEXTREC lpConfig, LPSTR lpMatch)
{
    if (lpMatch)
        return SearchKeyname (lpINI, lpConfig, lpMatch);
    else
        return FindKeyname (lpINI, lpConfig);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Finds a keyname entry in a section by comparing the text after  *
 *      after a keyname to the specified search text.  This function    *
 *      is most valuable in the case of multiple section entries with   *
 *      the same keyname.                                               *
 *                                                                      *
 * Parameters:                                                          *
 *      See GetKeynameEntry.                                            *
 *                                                                      *
 * Return Value:                                                        *
 *      See GetKeynameEntry.                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 05/12/1993 ED Function Created.                                      *
 ************************************************************************/

UINT LOCAL PASCAL SearchKeyname (LPINIREC lpINI, LPCFGTEXTREC lpConfig, LPSTR lpMatch)
{
    LPBYTE      lpTemp;
    UINT        wFileSize;
    UINT        wMatchLen;
    UINT        wSize, x;
    char        szToken [MAX_INI_LINE + 1];
    char        szFile [SYM_MAX_PATH + 1];
    BOOL        bResult = FALSE;

    wMatchLen = STRLEN (lpMatch);

    while (FindKeyname (lpINI, lpConfig) == NOERR)
        {
                                        // point to the entry text
        lpTemp = lpINI->lpBufferStart + lpINI->wTextStart;

                                        // copy the text up to the EOL, or
                                        // up to the end of the file, or
                                        // to the limit specified
        wFileSize = lpINI->wSize;
        wSize = sizeof(szToken);
        x = 0;
        while (wSize-- && wFileSize-- && !IS_EOL (*lpTemp))
            szToken [x++] = *lpTemp++;
        szToken [x] = '\0';

                        // search for data containing the minimum amount
                        // of search text, i.e., "ed" would match:
                        //
                        //      master=ed
                        //      master=edward
                        //      master=Edward James Carlin

        if (lpConfig->wFlags & CFG_INI_FINDMINIMUM)
            {
            bResult = (BOOL)(STRNICMP (lpMatch, szToken, wMatchLen) == 0);
            }

                        // search for filenames that may be preceded by
                        // a pathname, i.e., "VNAVD.386" would match:
                        //
                        //      device=C:\NAV\VNAVD.386
                        //      device=vnavd.386
                        //
        else if (lpConfig->wFlags & CFG_INI_FINDFILE)
            {
            NameReturnFile (szToken, szFile);

            bResult = (BOOL)(STRICMP (lpMatch, szFile) == 0);
            }

                        // search for data containing the exact amount
                        // of search text, i.e., "ed" would match:
                        //
                        //      master=ed
                        //
                        // and would not match:
                        //
                        //      master=edward
                        //      master=Edward James Carlin

//        else if (lpConfig->wFlags & CFG_INI_FINDEXACT)
        else            // default to exact search
            {
            bResult = (BOOL)(STRICMP (lpMatch, szToken) == 0);
            }

        if (bResult)
            break;

        if (NextLine (lpINI) == ERR)
            break;
        }

    return (bResult) ? NOERR : ERR;
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Finds the next keyname entry in a section.                      *
 *                                                                      *
 * Parameters:                                                          *
 *      See GetKeynameEntry.                                            *
 *                                                                      *
 * Return Value:                                                        *
 *      See GetKeynameEntry.                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 05/12/1993 ED Function Created.                                      *
 ************************************************************************/

UINT LOCAL PASCAL FindKeyname (LPINIREC lpINI, LPCFGTEXTREC lpConfig)
{
    UINT        wLen;
    UINT        wChars;
    BYTE        ch, key;
    BOOL        bFound;
    LPBYTE      lpTemp;
    LPBYTE      lpLastLine = lpINI->lpBuffer;

    wLen = STRLEN (lpConfig->lpEntry);

    key = (BYTE) CharToLower (*lpConfig->lpEntry);

    while (lpINI->lpBuffer && lpINI->lpBuffer < lpINI->lpBufferEnd)
        {
        ch = *lpINI->lpBuffer;

        if (IS_WHITE (ch))              // get rid of leading white space
            {
            lpINI->lpBuffer++;
            continue;
            }

        if (IS_COMMENT(ch))             // found a commented line
            goto SkipComment;

        if (ch == '[')                  // found a section name
            break;

        if (CharToLower (ch) == key)    // could be a match
            {
                                        // find out how many characters
                                        // are left, so we don't cause a
                                        // GPF by walking off the end
            wChars = lpINI->lpBufferEnd - lpINI->lpBuffer;

            if (wChars > wLen)
                {                       // check for a match.  this may not
                                        // be the one we want because there
                                        // could be entries that start with
                                        // the same letters (i.e., Mouse=,
                                        // MouseSpeed=, etc.)
                if (STRNICMP (lpINI->lpBuffer, lpConfig->lpEntry, wLen) == 0)
                    {
                    lpTemp = lpINI->lpBuffer;
                    bFound = FALSE;

                    while (wChars--)    // scan for the '=' sign
                        {
                        ch = *lpTemp;

                                        // hit the end of the line, so this
                                        // is not the one we want
                        if (IS_EOL (ch))
                            break;

                        if (ch == '=')  // found the equals sign
                            {
                                        // point to the keyname text
                            lpINI->wTextStart = (UINT) ((lpTemp + 1) - lpINI->lpBufferStart);
                            bFound = TRUE;
                            break;
                            }

                        lpTemp++;       // next char
                        }    // while

                    if (bFound)         // did we find an equals sign?
                        {
                        lpTemp--;
                                        // strip trailing white space
                        while (IS_WHITE (*lpTemp))
                            lpTemp--;

                        lpTemp++;

                                        // if the remaining text is the
                                        // same size as the keyname, we
                                        // have a good match
                        if ((UINT) (lpTemp - lpINI->lpBuffer) == wLen)
                            {
                            lpINI->wLineStart = (UINT) (lpINI->lpBuffer - lpINI->lpBufferStart);
                            lpINI->wLastLine = (UINT) (lpLastLine - lpINI->lpBufferStart);
                            return (NOERR);
                            }
                        }
                    }    // fstrnicmp
                }    // wChars > wLen
            }

        lpLastLine = lpINI->lpBuffer;

SkipComment:                            // don't change last line for comments
        NextLine (lpINI);
        }

    lpINI->wLineStart = 0xFFFF;
    lpINI->wTextStart = 0xFFFF;
    lpINI->wLastLine = (UINT) (lpLastLine - lpINI->lpBufferStart);

    return ((UINT) ERR);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Advances the buffer pointer to point to the next line.  This    *
 *      function handles the end-of-line characters CR, LF, CR/LF, and  *
 *      LF/CR.  If no next line, the buffer pointer is set to NULL.     *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      NOERR           No problem                                      *
 *      ERR             No next line                                    *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT LOCAL PASCAL NextLine (LPINIREC lpINI)
{
    char    ch;
                                        // do until the end of the buffer
    while (lpINI->lpBuffer < lpINI->lpBufferEnd)
        {
                                        // if not a CR or a LF, keep going
        ch = *lpINI->lpBuffer++;        // pass this char
        if (IS_EOL (ch))
            {
                                        // don't check if it is past the end
                                        // of the buffer.  Otherwise, we'll
                                        // get a GPF under Windows.
            if (lpINI->lpBuffer < lpINI->lpBufferEnd)
                {                       // if CR-LF/LF-CR pair, advance twice
                if ((ch == 0x0D && *lpINI->lpBuffer == 0x0A) ||
                    (ch == 0x0A && *lpINI->lpBuffer == 0x0D))
                    lpINI->lpBuffer++;
                }
            return (NOERR);
            }
        }   // while

    lpINI->lpBuffer = NULL;             // behave as spec'd
    return ((UINT) ERR);                // not found
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Inserts or deletes text from a INI file buffer.  A new buffer   *
 *      is allocated in this function and the old buffer is freed.      *
 *                                                                      *
 * Parameters:                                                          *
 *      lpINI          Initialized INI record.                         *
 *                                                                      *
 *      wOffset         The insertion point for new text.  Text from    *
 *                      this point up to the end of the line is either  *
 *                      replaced or deleted.                            *
 *                                                                      *
 *      lpData          Pointer to the text that replaces the previous  *
 *                      text.  If this value is NULL, no text is        *
 *                      inserted, so the result is deletion of the      *
 *                      text from wOffset to the end of the line.       *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT LOCAL PASCAL ModifyText (LPINIREC lpINI, UINT wFirstPiece, LPBYTE lpData)
{
    LPBYTE      lpNewBlock;
    LPBYTE      lpTemp;
    LPBYTE      lpLineEnd = NULL;
    UINT        wMiddlePiece;
    UINT        wLastPiece = 0;
    char        szInsertText[128];
    HGLOBAL     hNewBlock;

                                        //-----------------------------------
                                        // if not tacking onto the end, get
                                        // the length from the current buffer
                                        // position up to the end of the line.
                                        //-----------------------------------
    if (wFirstPiece != lpINI->wSize)
        {
                // if creating a new entry, we aren't going to delete
                // anything, so set the line end to the line start.

                // if the current position is an EOL, keep it instead
                // of trying to find one.

                // Exception: if we're deleting an entry, delete its EOL also.

        if (!lpINI->bCreating && (!lpData || !IS_EOL (*lpINI->lpBuffer)))
            {
            lpTemp = lpINI->lpBuffer;
            NextLine (lpINI);           // get the next line
            lpLineEnd = lpINI->lpBuffer;
            lpINI->lpBuffer = lpTemp;
            }
        else
            lpLineEnd = lpINI->lpBuffer;
        }
                                        //-----------------------------------
                                        // determine the size of the text
                                        // about to be added, if any
                                        //-----------------------------------
    if (lpData)
        {
        STRCPY (szInsertText, lpData);
        STRCAT (szInsertText, szCRLF);     // add CR/LF
        wMiddlePiece = STRLEN (szInsertText);
        }
    else
        wMiddlePiece = 0;
                                        //-----------------------------------
                                        // determine the size of the memory
                                        // block that will result from the
                                        // changes being made
                                        //-----------------------------------

    if (lpLineEnd != NULL)
        wLastPiece = lpINI->lpBufferEnd - lpLineEnd;

    lpINI->wSize = wFirstPiece + wMiddlePiece + wLastPiece;

                                        //-----------------------------------
                                        // allocate a new memory block of the
                                        // new size and lock it down
                                        //-----------------------------------

    if ((hNewBlock = MemAlloc (GHND|GMEM_DDESHARE, (DWORD) lpINI->wSize)) == NULL)
        return ((UINT) ERR);

    if ((lpNewBlock = (LPBYTE) MemLock (hNewBlock)) == NULL)
        {
        MemFree (hNewBlock);
        return ((UINT) ERR);
        }
                                        //-----------------------------------
                                        // copy the text in the old block to
                                        // the new block, removing the
                                        // appropriate text and inserting
                                        // the new text (if any).
                                        //-----------------------------------
    lpTemp = lpNewBlock;

    if (wFirstPiece)
        {
        MEMCPY(lpTemp, lpINI->lpBufferStart, wFirstPiece);
        lpTemp += wFirstPiece;
        }

    if (wMiddlePiece)
        {
        MEMCPY(lpTemp, szInsertText, wMiddlePiece);
        lpTemp += wMiddlePiece;
        }

    if (wLastPiece)
        MEMCPY(lpTemp, lpLineEnd, wLastPiece);

                                        //-----------------------------------
                                        // kill off the old block, since it
                                        // is no longer useful
                                        //-----------------------------------
    MemUnlock (lpINI->hBlock, lpINI->lpBuffer);
    MemFree (lpINI->hBlock);
                                        //-----------------------------------
                                        // update the INI record for the new
                                        // pointers and the block handle
                                        //-----------------------------------
    lpINI->hBlock = hNewBlock;
    lpINI->lpBuffer = lpINI->lpBufferStart = lpNewBlock;
    lpINI->lpBufferEnd = lpNewBlock + lpINI->wSize;

    return (NOERR);
}   // ModifyText


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Reduces a string to the minimum necessary text.  Basically      *
 *      all white space is removed, unless it is inside quotes.  If     *
 *      there are quotes, they are removed, but any text inside the     *
 *      quotes remains untouched.  This makes it easy to parse and      *
 *      otherwise deal with strings in .INI files.                      *
 *                                                                      *
 *      Examples:                                                       *
 *                                                                      *
 *      Before: "    caption   = "Windows Setup""                       *
 *      After:  "caption=Windows Setup"                                 *
 *                                                                      *
 *      Before: "    6 =. ,"Microsoft Windows 3.1 Disk #6",disk6        *
 *      After:  "6=.,Microsoft Windows 3.1 Disk #6,disk6                *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 02/19/1993 ED Function Created.                                      *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigMinimizeText (LPSTR lpSrc)
{
    LPSTR       lpDest;
    BOOL        bInQuote = FALSE;

                                        // walk through the original string
    for (lpDest = lpSrc; *lpSrc; lpSrc++)
        {
        if (*lpSrc == '\"')             // is this a quote?
            {
                                        // toggle the quote state
            bInQuote = (BOOL) !bInQuote;
            continue;                   // eat the character
            }
                                        // if this is white space, eat it,
                                        // unless it is inside quotes
        if (!bInQuote && (*lpSrc == ' ' || *lpSrc == '\t'))
            continue;

        *lpDest++ = *lpSrc;             // if we get here, copy the character
        }

    *lpDest = '\0';

    return (NOERR);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Determines the size of the entries in a section.                *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      The total size of the section data, in bytes.  Comments and     *
 *      blank lines are not included in the size, nor is leading        *
 *      white space.                                                    *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 02/22/1993 ED Function Created.                                      *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigIniQuerySection (LPCFGTEXTREC lpConfig)
{
    BOOL        bLocal = FALSE;
    BOOL        bGlobal = FALSE;
    UINT        wRet = 0;

    if (lpConfig->wFlags & CFG_LOCAL_FIRST)
        goto Local_First;

    while (TRUE)
        {                               // read the global config file
        if (!bGlobal && wRet == 0)
            {
            if (rGlobalText.dwSize > 0)
                wRet = ConfigIniFileQuerySection (&rGlobalText, lpConfig);

            bGlobal = TRUE;
            }
                                        // read the local config file
        else if (!bLocal && wRet == 0)
            {
Local_First:
            if (rLocalText.dwSize > 0)
                wRet = ConfigIniFileQuerySection (&rLocalText, lpConfig);

            bLocal = TRUE;
            }
        else                            // we tried both, so quit
            break;
        }

    return (wRet);
}


UINT SYM_EXPORT WINAPI ConfigIniFileQuerySection (LPCFGFILE lpFile, LPCFGTEXTREC lpConfig)
{
    INIREC      rINI;
    UINT        wRet = 0;

    rINI.hBlock = lpFile->hMem;
    rINI.wSize  = (UINT) lpFile->dwSize;

    if (rINI.lpBufferStart = (LPBYTE) MemLock (rINI.hBlock))
        {
        rINI.lpBuffer = rINI.lpBufferStart;
        rINI.lpBufferEnd = rINI.lpBuffer + rINI.wSize;

        if (FindSection (&rINI, lpConfig) == NOERR)
            if (NextLine (&rINI) == NOERR)
                wRet = CopySection (&rINI, NULL, 0xFFFF);

        MemUnlock (rINI.hBlock, rINI.lpBuffer);
        }

    return (wRet);
}



