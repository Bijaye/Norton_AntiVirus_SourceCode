/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/cfg_swit.c_v   1.3   26 Oct 1996 11:20:18   JBRENNA  $ *
 *                                                                      *
 * Description:                                                         *
 *      Switch processing routines.  This code, and the accompanying        *
 *        startup hook module can be used separately from the block        *
 *        and text configuration manager modules.                                *
 *                                                                      *
 * Contains:                                                            *
 *      ConfigProcessCmdLine                                            *
 *      ConfigParamCount                                                *
 *      ConfigSwitchCount                                               *
 *      ConfigParamGet                                                  *
 *      ConfigParamToUpper                                              *
 *      ConfigSwitchSet                                                 *
 *      ConfigSwitchGetValue                                            *
 *      ConfigSwitchGetText                                             *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/cfg_swit.c_v  $ *
// 
//    Rev 1.3   26 Oct 1996 11:20:18   JBRENNA
// Port QAKF changes:
// Add ConfigParamToUpperNonDbcs(). This function upper cases all non-DBCS
// characters in a CFG_PARAM_ARG option. This ensures that we do not upper
// case a double byte english character (You can input a DB lower case "a"
// on Japanese machines. If we upper case it, the DB character becomes "A".
// And then we cannot find the file.)
// 
//    Rev 1.2   21 Feb 1996 14:18:08   SRUIZ
// Restored commented out code in ConfigProcessCmdLineEx()
// that uses a word instead of a byte to store the
// cmdline length in Win32 (which can greater than 256 chars).
// 
//    Rev 1.1   15 Feb 1996 15:00:44   JHONG
// Removed #ifdef SYM_WIN32 in ConfigProcessCmdLineEx for getting the length
// of command line argument
// 
//    Rev 1.0   26 Jan 1996 20:22:12   JREARDON
// Initial revision.
// 
//    Rev 1.40   22 Dec 1995 14:25:40   GDZIECI
// Removed previous change, which broke cmd line processing. Applications
// calling ConfigProcessCmdLine() must also call ConfigProcessCmdLineEnd()
// to free allocated memory.
// 
//    Rev 1.39   01 Dec 1995 14:26:42   GDZIECI
// Fixed memory leak in ConfigProcessCmdLine(). Now it calls
// ConfigProcessCmdLineEnd() to free up memory allocated by
// ConfigProcessCmdLineEx().
// 
//    Rev 1.38   29 Jun 1995 18:55:38   KEITH
// Handle a module name (argv[0]) that contains spaces 
// in ConfigProcessCmdLine().
// 
//    Rev 1.37   27 Apr 1995 16:02:42   MFALLEN
// Platforms broke from previous change. Fixed.
// 
//    Rev 1.36   27 Apr 1995 14:44:26   DDREW
// Enabled bFirstTime flags to work on WIN16 platform.
// 
//    Rev 1.35.1.0   24 Apr 1995 19:47:04   SCOTTP
// Branch base for version QAK7
// 
//    Rev 1.35   07 Feb 1995 22:31:18   MFALLEN
// Debugged for W32 and DOS
// 
//    Rev 1.33   07 Feb 1995 19:49:00   MFALLEN
// Overseen a small detail.
// 
//    Rev 1.32   07 Feb 1995 18:25:04   MFALLEN
// Assertions were being too smart.
// 
//    Rev 1.31   07 Feb 1995 18:12:30   MFALLEN
// Code was assuming that command line parameters can never be larger than
// SYM_MAX_PATH. This is not valid for W32 so buffer must be allocated at run-time.
// This also forces us to use a new function called ConfigProcessCmdLineEnd() to
// free memory that was allocated for command line processing. Applications that
// call ConfigProcessCmdLineEx() must call ConfigProcessCmdLineEnd().
// 
// 
//    Rev 1.30   23 Jan 1995 11:00:14   TORSTEN
// Fixed ConfigProcessCmdLineEx() so that it properly processes switches
// of the form /SWITCH:foo/bar, e.g. non-quoted parameters which contain
// the switch character. This is needed for passing dates as parameters.
// 
//    Rev 1.29   31 Aug 1994 16:01:36   JMILLARD
// fix problem in last fix that always turned the LFN stuff on (flag test was
// | instead of & )
// 
// 
//    Rev 1.28   24 Aug 1994 15:26:14   DLEVITON
// ConfigReg calls.
//
//    Rev 1.27   03 Aug 1994 17:14:06   MFALLEN
// Added LFN support.
//
//    Rev 1.26   14 May 1994 14:48:14   BRAD
// Added WIN32 support
//
//    Rev 1.25   13 May 1994 14:52:16   BRAD
// Use new DOS macro for Dos3Call vs. DOS
//
//    Rev 1.24   05 Apr 1994 19:48:20   DLEVITON
// Undid last change. The bug was in platform.h defining STRNCPY to be
// strncpy instead of _fstrncpy.
//
//    Rev 1.23   05 Apr 1994 19:16:48   DLEVITON
// Don't call STRNCPY to copy 0 bytes.
//
//    Rev 1.22   15 Mar 1994 12:33:36   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.21   04 Mar 1994 08:42:42   JMILLARD
// add ConfigSwitchGetLongValue and ConfigSwitchGetLongValueEx
//
//    Rev 1.20   25 Feb 1994 15:05:00   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.19   25 Feb 1994 12:22:44   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.18   17 Feb 1994 21:24:44   PGRAVES
// Win32.
//
//    Rev 1.17   14 Feb 1994 15:34:08   PGRAVES
// Merged QAK2 changes into trunk.
//
//    Rev 1.16   03 Feb 1994 22:15:28   ENRIQUE
// Removed the macro for memmove.
//
//    Rev 1.15   02 Feb 1994 15:24:06   PETERD
// Fixed quote handling in switch parameters.
//
//    Rev 1.14   26 Jan 1994 12:37:40   TONY
// Initial OS/2 port.  Switch-char is always assummed to be /
//
//    Rev 1.13   12 Oct 1993 17:49:36   EHU
// Ifdef'd out unused code for SYM_NLM.
//
//    Rev 1.12   16 Jun 1993 08:01:32   ED
// [Enh] Added an 'Ex' layer of functions to provide the ability to process
// and parse command lines other than the startup command line in the PSP.
//
// [Enh] Removed bySwitchChar from the CMDLINEDATA structure and made it a
// static.
//
// [Enh] Converted all BYTEs to chars or BOOLs as appropriate.
//
//    Rev 1.11   21 Feb 1993 18:31:30   BRAD
// No change.
//
//    Rev 1.10   12 Feb 1993 15:25:54   ENRIQUE
// No change.
//
//    Rev 1.9   15 Dec 1992 14:27:48   CRAIG
// Handle YY/MM/DD date format correctly (#ifdef'd out... so what...)
//
//    Rev 1.8   12 Oct 1992 16:15:44   EHU
// SYM_NLM changes.
//
//
//    Rev 1.7   02 Oct 1992 14:18:32   ED
// Fixed ConfigSwitchSet
//
//    Rev 1.6   02 Oct 1992 09:22:24   ED
// Changed the GetSwitch functions to return BOOLs
//
//    Rev 1.5   30 Sep 1992 09:38:34   MARKK
// Added null check in ConfigSwitchSet
//
//    Rev 1.4   18 Sep 1992 15:42:42   ED
// Stupid
//
//    Rev 1.3   18 Sep 1992 15:40:46   ED
// optimize pragma
//
//    Rev 1.2   15 Sep 1992 10:49:22   ED
// Fixed minor problem
//
//    Rev 1.1   08 Sep 1992 09:06:18   ED
// Final, version 1.0
//
//    Rev 1.0   03 Sep 1992 09:10:14   ED
// Initial revision.
 ************************************************************************/

#include "platform.h"
#ifdef SYM_WIN32
#include "winreg.h"
#endif
#include "xapi.h"
#include "file.h"
#include "symcfg.h"
#include "dbcs.h"

BOOL    LOCAL PASCAL IsQuoted (char ch);
BOOL    LOCAL PASCAL IsBogus (LPCSTR line);
char    LOCAL PASCAL GetSwitchChar (void);
LPSTR   LOCAL PASCAL GetSwitchText (LPCFGCMDLINEDATA lpLineData, LPCFGTEXTREC lpConfig);


//#if defined(SYM_WIN32)    // also a problem for WIN16
static  BOOL            bFirstTime = TRUE;
//#endif
static  CFGCMDLINEDATA  s_rCmdLine;
static  char            s_cSwitchChar;

/* **************************************************************************
@Name:
void SYM_EXPORT WINAPI ConfigProcessCmdLine (
    LPSTR   lpLineBuffer                // [in] optional command line params.
    );

@Description:
Copies and parses up a command line containing switches and parameters.

@Parameters:
$lpLineBuffer$  Can be set to NULL only of the caller is w Win32 app otherwise
                it must be a valid command line string. For DOS apps this must
                point to PSP:0080
                

@Include: symcfg.h
************************************************************************** */

void SYM_EXPORT WINAPI ConfigProcessCmdLine (
    LPSTR   lpLineBuffer                // [in] optional command line params.
    )
{
#ifdef SYM_WIN32
                                        // -----------------------------------
                                        // In Win95 command line param length
                                        // can be larger than SYM_MAX_PATH.
                                        // Due to this 'feature' we must 
                                        // allocate memory at run-time.
                                        // -----------------------------------

    auto    STATUS      Status = NOERR;
    auto    UINT        uCmdLineLength = 0;
    auto    LPSTR       lpCmdLine = NULL;

    if ( bFirstTime == TRUE )
        {
        bFirstTime = FALSE;
                                        // -----------------------------------
                                        // Check the length of command line
                                        // parameters stored by Windows.
                                        // -----------------------------------

        uCmdLineLength = ( STRLEN ( GetCommandLine() ) + 1 );

                                        // -----------------------------------
                                        // Temporary buffer must be also allo-
                                        // cated at runtime.
                                        // -----------------------------------

        lpCmdLine = (LPSTR) MemAllocPtr ( GHND, uCmdLineLength );

        if ( !lpCmdLine )
            Status = ERR;
        else if ( lpLineBuffer == NULL )
            {
            char    chEndOfName;

            lpLineBuffer = GetCommandLine();

                                        // -----------------------------------
                                        // Skip over the file name.
                                        // Win95 allows spaces, so we have
                                        // to account for that in the 
                                        // executable name here.
                                        // -----------------------------------
            if ( *lpLineBuffer == '\"' )
                {
                chEndOfName = '"';
                lpLineBuffer++;
                }
            else
                chEndOfName = ' ';

            while ( *lpLineBuffer != chEndOfName && *lpLineBuffer != EOS )
                lpLineBuffer++;

                                        // -----------------------------------
                                        // We are either at a space, EOS, or
                                        // the ending Quote of the executable
                                        // name. Skip past the Quote only.
                                        // -----------------------------------
            if ( *lpLineBuffer == '\"' )
                lpLineBuffer++;

                                        // -----------------------------------
                                        // Set the first byte to the size of
                                        // remaining string
                                        // -----------------------------------
            STRCPY ( lpCmdLine + sizeof(WORD), lpLineBuffer );

            *(LPWORD)lpCmdLine = STRLEN(lpLineBuffer);

            ConfigProcessCmdLineEx ( &s_rCmdLine, lpCmdLine );
            }
        else
            {
            ConfigProcessCmdLineEx ( &s_rCmdLine, lpLineBuffer );
            }

                                        // -----------------------------------
                                        // Free temporary memory.
                                        // -----------------------------------
        if ( lpCmdLine )
            {
            MemFreePtr ( lpCmdLine );
            }
        }
#else
    if ( bFirstTime == TRUE )
        {
        bFirstTime = FALSE;
        s_rCmdLine.lpLineBuffer = NULL;
        }    
        
    ConfigProcessCmdLineEx ( &s_rCmdLine, lpLineBuffer );
#endif

}

/* **************************************************************************
@Name:
VOID SYM_EXPORT WINAPI ConfigProcessCmdLineEnd (
    CFGCMDLINEDATA FAR *lpLineData      // [in] pointer to use for cleanup
    );

@Description:
 This function is called to free up memory allocated to process command lines.
 Should only be called if the application provided it's own CFGCMDLINEDATA
 structure.

@Parameters:
$lpLineData$    Pointer to CFGCMDLINEDATA structure that should be used for
                cleaning up.

@Returns:       Nothing

@Include: symcfg.h
************************************************************************** */

VOID SYM_EXPORT WINAPI ConfigProcessCmdLineEnd (
    CFGCMDLINEDATA FAR *lpLineData      // [in] pointer to use for cleanup
    )
{
    auto    CFGCMDLINEDATA FAR *    lpCmdData;

                                        // ----------------------------------
                                        // If function called with NULL that
                                        // indicates that we should use our
                                        // static global variable.
                                        // ----------------------------------
    if ( lpLineData )
        lpCmdData = lpLineData;
    else
        lpCmdData = &s_rCmdLine;

                                        // ----------------------------------
                                        // If there's a local copy of the 
                                        // command line, free buffer.
                                        // ----------------------------------
                                        // only dos vers allocs this, so...
                                        // ----------------------------------
#ifndef SYM_DOS
    if ( lpCmdData->lpLineBuffer )
        {
        MemFreePtr ( lpCmdData->lpLineBuffer );

                                        // ----------------------------------
                                        // Zero out to make sure it's not used
                                        // anymore.
                                        // ----------------------------------
        lpCmdData->lpLineBuffer = NULL;
        }
#else
    lpCmdData->lpLineBuffer = NULL;
#endif
}


/* **************************************************************************
@Name:
VOID SYM_EXPORT WINAPI ConfigProcessCmdLineEx (
    CFGCMDLINEDATA FAR *lpLineData,
    LPCSTR      lpLineBuffer
    );

@Description:
This function parses the provided command line and initializes the
CFGCMDLINEDATA structure that was provided to the function. If the calling
app calls this function directly it should also call ConfigProcessCmdLineEnd()
before exiting to free up memory that was allocated for command line processing

@Parameters:
$lpLineData$    Pointer to CFGCMDLINEDATA structure that should be initialized
$lpLineBuffer$  Pointer to command line that should be parsed.

@Returns:       Nothing.

@Include: symcfg.h
************************************************************************** */

VOID SYM_EXPORT WINAPI ConfigProcessCmdLineEx (
    CFGCMDLINEDATA FAR *lpLineData,
    LPCSTR      lpLineBuffer
    )
{
    LPSTR       lpCmdLine;
    LPSTR       lpQuoteStart;
    char        byNewQuote;
    BOOL        bNoSpace = FALSE;
    char        byType;
    UINT        wLen;
    BOOL        bHasParam;

                                        // -----------------------------------
                                        // Must be something otherwise what are
                                        // we doing in here?
                                        // -----------------------------------
    SYM_ASSERT ( lpLineData );
    SYM_ASSERT ( lpLineBuffer );
                                        // -----------------------------------
                                        // If called multiple times make sure
                                        // free memory before destroying ptr.
                                        // -----------------------------------
    if ( lpLineData->lpLineBuffer )
        ConfigProcessCmdLineEnd ( lpLineData );

                                        // -----------------------------------
                                        // clear out the record
                                        // -----------------------------------

    MEMSET ( lpLineData, 0, sizeof (CFGCMDLINEDATA) );

                                        // -----------------------------------
                                        // get the command line length
                                        // -----------------------------------
#ifdef SYM_WIN32
    wLen = *(LPWORD)lpLineBuffer;

    lpLineBuffer += sizeof(WORD);
#else
    wLen = *lpLineBuffer++;
#endif
                                        // -----------------------------------
                                        // Allocate memory and indicate error
                                        // if out of memory.
                                        // -----------------------------------
#ifndef SYM_DOS
    lpLineData->lpLineBuffer = (LPSTR) MemAllocPtr ( GHND, wLen + 1 );
#else
    {
    static char dosLineBuffer [SYM_MAX_PATH];

    lpLineData->lpLineBuffer = dosLineBuffer;
    }
#endif

    if ( !lpLineData->lpLineBuffer )
        return;

    lpCmdLine = lpLineData->lpLineBuffer;

                                        // -----------------------------------
                                        // copy the command line
                                        // -----------------------------------

    STRNCPY (lpCmdLine, lpLineBuffer, wLen);

                                        // -----------------------------------
                                        // get the current switch character
                                        // -----------------------------------
    s_cSwitchChar = GetSwitchChar();

    while (*lpCmdLine)
        {
                                          /* scan past any leading blanks        */
        while (IsBogus (lpCmdLine))
            lpCmdLine++;

        if (*lpCmdLine == '\0')
            break;

        byType = CFG_PARAM_ARG;

        if (*lpCmdLine == s_cSwitchChar)
            {
            *lpCmdLine = '\0';           // Remove the switch character
            lpCmdLine++;
            byType = CFG_SWITCH_ARG;
            }

        if (lpLineData->argc >= CFG_MAX_ARGS)  // are we done yet?
            break;
                                        // record the argument
        lpLineData->argt [lpLineData->argc]   = byType;
        lpLineData->argv [lpLineData->argc++] = lpCmdLine;

        if (byType == CFG_PARAM_ARG)    // count the arguments
            lpLineData->wParameters++;
        else
            lpLineData->wSwitches++;

        if (byType == CFG_SWITCH_ARG)   // If switch, move forward
            {                           // in case parameter is quoted.
            while ((*lpCmdLine) && (*lpCmdLine != ':') &&
                    !IsQuoted(*lpCmdLine) && !IsBogus(lpCmdLine))
                {
                ++lpCmdLine;
                }
            }

                                        // does this switch have a parameter?
        bHasParam = (BOOL) (*lpCmdLine == ':');

        if (IsQuoted (*lpCmdLine))      /* start of quoted parameter */
            {
            byNewQuote = *lpCmdLine;    /* save starting quoted character */
quoted:
            lpQuoteStart = lpCmdLine++; /* save start value, in case not "quoted" */

            while ((*lpCmdLine) && (*lpCmdLine != byNewQuote))
                lpCmdLine++;            /* search for end of quoted string */

                                        /* If end of string, ... */
            if (!(*lpCmdLine))
                {
                lpCmdLine = lpQuoteStart;/* reset start of where we found " */
                if (bNoSpace)
                    {
                    *lpCmdLine++ = byNewQuote;/* reset to previous value         */
                    lpLineData->argc--; /* false parameter found         */
                    bNoSpace = FALSE;
                    }
                }
            else                        /* quoted string found                 */
                {
                UINT uSize;

                bNoSpace = FALSE;
                uSize = (UINT)(lpCmdLine - lpQuoteStart);

                *lpCmdLine++ = '\0';                /* set end of quoted string */

                MEMMOVE(lpQuoteStart, lpQuoteStart+1, uSize);

                while (IsBogus (lpCmdLine))  /* skip over delimiters */
                    lpCmdLine++;

                continue;               /* get the next parameter */
                }
            }

        if ( lpLineData->wFlags & CFG_FLAG_ENABLE_LFN )
            {
            while ( (*lpCmdLine) )
                {
                if ( *lpCmdLine == ' ' )
                    {
                    if ( IsQuoted (*(lpCmdLine + 1)) ||
                         *(lpCmdLine + 1) == s_cSwitchChar ||
                         *(lpCmdLine + 2) == ':' ||
                        ( *(lpCmdLine + 1) == '\\' &&
                          *(lpCmdLine + 2) == '\\' ) )
                        {
                        break;
                        }
                    }

                lpCmdLine++;
                }
            }
        else
            {
                                              /* scan to end of parameter  */
            if (bHasParam)
                {
                while ((*lpCmdLine) && !IsBogus (lpCmdLine) && !IsQuoted (*lpCmdLine))
                    lpCmdLine++;
                }
            else
                {
                while ((*lpCmdLine) && (*lpCmdLine != s_cSwitchChar) &&
                    !IsBogus (lpCmdLine) && !IsQuoted (*lpCmdLine))
                    lpCmdLine++;
                }
            }

        if (IsQuoted (*lpCmdLine))
            {
            byNewQuote = *lpCmdLine;              /* mark the type of quote         */
            *lpCmdLine = '\0';                    /* terminate this parameter         */
            if (lpLineData->argc >= CFG_MAX_ARGS) /* mark as another parameter found */
                break;

            lpLineData->argt [lpLineData->argc] = CFG_PARAM_ARG;
            lpLineData->argv [lpLineData->argc++] = lpCmdLine;

            bNoSpace = TRUE;
            goto quoted;
            }

        if (IsBogus (lpCmdLine)) /* if regular delimiter, then ASCIIZ it        */
            {
            *lpCmdLine++ = '\0';
            }
        }
}


BOOL LOCAL PASCAL IsQuoted (char ch)
{
    return ((ch == '\"') || (ch == '\''));
}


BOOL LOCAL PASCAL IsBogus (LPCSTR line)
{
    char        ch = *line;

    return ((ch == ' ') || (ch == ',') || (ch == ';') || (ch == '\t'));
}

#pragma optimize ("clegt",off)         // turn off global optimization
char LOCAL PASCAL GetSwitchChar (void)
{
    char        bySwitchChar;

#if defined(SYM_NLM) || defined(SYM_OS2) || defined(SYM_WIN32)
    bySwitchChar = '/';
#else
    _asm                                // get the current switch character
        {
        mov     ax,3700h
        DOS
        mov     bySwitchChar,dl
        }
#endif

    return (bySwitchChar);
}
#pragma optimize ("", on)

#ifndef SYM_NLM
/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Returns the number of parameters found on the command line      *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      Number of parameters                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/21/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigParamCount (void)
{
#if defined(SYM_WIN32)
    ConfigProcessCmdLine(NULL);
#endif
    return (s_rCmdLine.wParameters);
}


UINT SYM_EXPORT WINAPI ConfigParamCountEx (LPCFGCMDLINEDATA lpLineData)
{
    return (lpLineData->wParameters);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Returns the number of switches found on the command line        *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      Number of switches                                              *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/21/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigSwitchCount (void)
{
#if defined(SYM_WIN32)
    ConfigProcessCmdLine(NULL);
#endif
    return (s_rCmdLine.wSwitches);
}


UINT SYM_EXPORT WINAPI ConfigSwitchCountEx (LPCFGCMDLINEDATA lpLineData)
{
    return (lpLineData->wSwitches);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Copies the parameter at the specified location on the command   *
 *      line.  The location is relative to parameters only, and does    *
 *      not include switches.                                           *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      ERR             Parameter was not found                         *
 *      NOERR           Parameter was found                             *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/21/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigParamGet (UINT wParam, LPSTR lpBuffer, UINT wSize)
{
#if defined(SYM_WIN32)
    ConfigProcessCmdLine(NULL);
#endif
    return ConfigParamGetEx (&s_rCmdLine, wParam, lpBuffer, wSize);
}


UINT SYM_EXPORT WINAPI ConfigParamGetEx (LPCFGCMDLINEDATA lpLineData, UINT wParam, LPSTR lpBuffer, UINT wSize)
{
    UINT        i, wCount = 0;

    for (i = 0; i < lpLineData->argc; i++)
        {
        if (lpLineData->argt [i] == CFG_PARAM_ARG)
            {
            if (wCount == wParam)
                {
                StringMaxCopy (lpBuffer, lpLineData->argv [i], wSize);
                return (NOERR);
                }
            wCount++;
            }
        }

    return ((UINT) ERR);                /* Not found                        */
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Uppercases all parameters on the command line.  Switches are    *
 *      not affected.                                                   *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      NOERR                                                           *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/21/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigParamToUpper (void)
{
#if defined(SYM_WIN32)
    ConfigProcessCmdLine(NULL);
#endif
    return ConfigParamToUpperEx (&s_rCmdLine);
}


UINT SYM_EXPORT WINAPI ConfigParamToUpperEx (LPCFGCMDLINEDATA lpLineData)
{
    UINT        i;

    for (i = 0; i < lpLineData->argc; i++)
        {
        if (lpLineData->argt [i] == CFG_PARAM_ARG)
            STRUPR (lpLineData->argv[i]);
        }

    return (NOERR);
}

// Convert all non-DBCS characters to upper case.
static void NonDbcsUpperStr (LPSTR str)
{
    if (!DBCSInstalled())
        STRUPR (str);
    else
        {
        char   szBuf[4];

        szBuf[1] = EOS;
        for (; *str; str = AnsiNext (str))
            {
            if (!DBCSIsLeadByte (*str))
                {
                szBuf[0] = *str;
                STRUPR (szBuf);
                }
            }
        }
}

// Convert all non-DBCS characters on the command line parameters to upper case.
// Use this when you need to keep the case of DBCS characters unchanged.
UINT SYM_EXPORT WINAPI ConfigParamToUpperNonDbcs (void)
{
    UINT        i;

    for (i = 0; i < s_rCmdLine.argc; i++)
        {
        if (s_rCmdLine.argt [i] == CFG_PARAM_ARG)
            NonDbcsUpperStr (s_rCmdLine.argv[i]);
        }

    return (NOERR);
}
#endif  // ifndef SYM_NLM


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Determines if a command line switch is set, and the specified   *
 *      state of the switch if it exists.                               *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 *      lpConfig        Pointer to an intialized CFGTEXTREC.            *
 *                                                                      *
 *      lpwResult       Receives the result, which is:                  *
 *                                                                      *
 *                      CFG_SWITCH_NOT_SET                              *
 *                      CFG_SWITCH_SET                                  *
 *                      CFG_SWITCH_PLUS                                 *
 *                      CFG_SWITCH_MINUS                                *
 *                                                                      *
 * Return Value:                                                        *
 *      FALSE           Switch was not found                            *
 *      TRUE            Switch was found                                *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/21/1992 ED Created.                                               *
 ************************************************************************/

BOOL SYM_EXPORT WINAPI ConfigSwitchSet (LPCFGTEXTREC lpConfig, UINT FAR *lpwResult)
{
#if defined(SYM_WIN32)
    ConfigProcessCmdLine(NULL);
#endif
    return ConfigSwitchSetEx (&s_rCmdLine, lpConfig, lpwResult);
}


BOOL SYM_EXPORT WINAPI ConfigSwitchSetEx (LPCFGCMDLINEDATA lpLineData, LPCFGTEXTREC lpConfig, UINT FAR *lpwResult)
{
    LPSTR       lpText;
    BOOL        bResult = FALSE;

                                        // find the switch and return the
                                        // last character after the switch
                                        // text
    lpText = GetSwitchText (lpLineData, lpConfig);

    if (lpText)
        {
        if (lpwResult)
            {
            switch (*lpText)
                {
                case '+':                   // switch has '+' sign
                    *lpwResult = CFG_SWITCH_PLUS;
                    break;

                case '-':                   // switch has '-' sign
                    *lpwResult = CFG_SWITCH_MINUS;
                    break;

                default:
                    *lpwResult = CFG_SWITCH_SET;
                    break;
                }
            }

        bResult = TRUE;                 // found the switch
        }

    else if (lpwResult)
        *lpwResult = CFG_SWITCH_NOT_SET;

    return (bResult);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Returns the numerical value specified with a command line       *
 *      switch.  The form of the switch must be:                        *
 *                                                                      *
 *              /SWITCH:value                                           *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 *      lpConfig        Pointer to an intialized CFGTEXTREC.            *
 *                                                                      *
 *      lpwValue        Receives the result.  The value is not          *
 *                      altered if the switch is not found.             *
 *                                                                      *
 * Return Value:                                                        *
 *      FALSE           Switch was not found                            *
 *      TRUE            Switch was found                                *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/21/1992 ED Created.                                               *
 ************************************************************************/

BOOL SYM_EXPORT WINAPI ConfigSwitchGetValue (LPCFGTEXTREC lpConfig, UINT FAR *lpwValue)
{
#if defined(SYM_WIN32)
    ConfigProcessCmdLine(NULL);
#endif
    return ConfigSwitchGetValueEx (&s_rCmdLine, lpConfig, lpwValue);
}


BOOL SYM_EXPORT WINAPI ConfigSwitchGetValueEx (LPCFGCMDLINEDATA lpLineData, LPCFGTEXTREC lpConfig, UINT FAR *lpwValue)
{
    LPSTR       lpText;
    BOOL        bResult = FALSE;

                                        // find the switch and return the
                                        // last character after the switch
                                        // text
    lpText = GetSwitchText (lpLineData, lpConfig);

    if (lpText && *lpText == ':')
        {
        ConvertStringToWord (lpwValue, ++lpText);
        bResult = TRUE;
        }

//+++++ SHOULDN'T WE RETURN TRUE EVEN IF THERE IS NO TEXT? (EJC, 6-16-93)

    return (bResult);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Returns the numerical value specified with a command line       *
 *      switch.  The form of the switch must be:                        *
 *                                                                      *
 *              /SWITCH:value                                           *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 *      lpConfig        Pointer to an intialized CFGTEXTREC.            *
 *                                                                      *
 *      lpdwValue       Receives the result.  The value is not          *
 *                      altered if the switch is not found.             *
 *                                                                      *
 * Return Value:                                                        *
 *      FALSE           Switch was not found                            *
 *      TRUE            Switch was found                                *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/21/1992 ED Created.                                               *
 ************************************************************************/

BOOL SYM_EXPORT WINAPI ConfigSwitchGetLongValue (LPCFGTEXTREC lpConfig, DWORD FAR *lpdwValue)
{
#if defined(SYM_WIN32)
    ConfigProcessCmdLine(NULL);
#endif
    return ConfigSwitchGetLongValueEx (&s_rCmdLine, lpConfig, lpdwValue);
}


BOOL SYM_EXPORT WINAPI ConfigSwitchGetLongValueEx (LPCFGCMDLINEDATA lpLineData, LPCFGTEXTREC lpConfig, DWORD FAR *lpdwValue)
{
    LPSTR       lpText;
    BOOL        bResult = FALSE;

                                        // find the switch and return the
                                        // last character after the switch
                                        // text
    lpText = GetSwitchText (lpLineData, lpConfig);

    if (lpText && *lpText == ':')
        {
        ConvertStringToLong (lpdwValue, ++lpText);
        bResult = TRUE;
        }

//+++++ SHOULDN'T WE RETURN TRUE EVEN IF THERE IS NO TEXT? (EJC, 6-16-93)

    return (bResult);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Returns the text following a switch specified on the command    *
 *      line.  The form of the switch must be:                          *
 *                                                                      *
 *              /SWITCH:text                                            *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 *      lpConfig        Pointer to an intialized CFGTEXTREC.            *
 *                                                                      *
 *      lpBuffer        Receives the result.  The value is not          *
 *                      altered if the switch is not found.             *
 *                                                                      *
 *      wSize           Maximum number of bytes to copy.                *
 *                                                                      *
 * Return Value:                                                        *
 *      FALSE           Switch was not found                            *
 *      TRUE            Switch was found                                *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/21/1992 ED Created.                                               *
 ************************************************************************/

BOOL SYM_EXPORT WINAPI ConfigSwitchGetText (LPCFGTEXTREC lpConfig, LPSTR lpBuffer, UINT wSize)
{
#if defined(SYM_WIN32)
    ConfigProcessCmdLine(NULL);
#endif
    return ConfigSwitchGetTextEx (&s_rCmdLine, lpConfig, lpBuffer, wSize);
}


BOOL SYM_EXPORT WINAPI ConfigSwitchGetTextEx (LPCFGCMDLINEDATA lpLineData, LPCFGTEXTREC lpConfig, LPSTR lpBuffer, UINT wSize)
{
    LPSTR       lpText;
    BOOL        bResult = FALSE;
                                        // find the switch and return the
                                        // last character after the switch
                                        // text
    lpText = GetSwitchText (lpLineData, lpConfig);

    if (lpText && *lpText == ':')
        {
        StringMaxCopy (lpBuffer, ++lpText, wSize);
        bResult = TRUE;
        }

//+++++ SHOULDN'T WE RETURN TRUE EVEN IF THERE IS NO TEXT? (EJC, 6-16-93)

    return (bResult);
}

// -----------------------------------------------------------------------
// Helper function to find a switch and return the character after the
// switch text.  Used by ConfigSwitchSet, ConfigSwitchGetValue, and
// ConfigSwitchGetText
// -----------------------------------------------------------------------

LPSTR LOCAL PASCAL GetSwitchText (LPCFGCMDLINEDATA lpLineData, LPCFGTEXTREC lpConfig)
{
    LPSTR       lpArgText;
    LPSTR       lpSwitchText;
    UINT        i;
    char        ch;
    LPSTR       lpReturn = NULL;
    UINT        wLen;

    lpSwitchText = lpConfig->lpSwitch;                // get switch to match

    wLen = STRLEN (lpSwitchText);               // size the switch text

    for (i = 0; i < lpLineData->argc; i++)
        {
                                                // only do switches
        if (lpLineData->argt [i] != CFG_SWITCH_ARG)
            continue;

        lpArgText = lpLineData->argv [i];          // get the switch text

                                                // not even close
        if (STRNICMP (lpSwitchText, lpArgText, wLen) != 0)
            continue;

                                                // if we have a match but
                                                // we are not doing an exact
                                                // search, move to the end
                                                // of the text.
        ch = lpArgText [wLen];

        if (!(lpConfig->wFlags & CFG_SW_EXACT))
            {
            while (TRUE)
                {
                if (ch == '\0' || ch == ':' || ch == '+' || ch == '-')
                    break;

                ch = lpArgText [++wLen];
                }
            }
                                            // if we are doing an exact search,
                                        // and the final character is not
                                        // a terminating character, then
                                        // try again, batman.
        else if (ch != '\0' && ch != ':' && ch != '+' && ch != '-')
            continue;

                                        // return the character after the
                                        // switch text.
        lpReturn = &lpArgText [wLen];
        break;
        }

    return (lpReturn);
}


#if 0
/* ------------------------------------------------------------------------
 * ConfigFixupLineBufferDate
 *
 * NOTE:  This function must be called BEFORE ConfigProcessLineBuffer.
 *
 * This function converts any date separators that are the same as the
 * switch character to a designated replacement value.  This mostly
 * concerns users who enter dates using the forward slash (/) as the
 * digit separator, which is also the default switch character for DOS.
 *
 * Example:  In FD, a user may enter the following command line:
 *
 *        FD *.* /D12/11/91 /T1:23
 *
 *         If the following code is used, the /D switch will be converted
 *        to /D12-11-90:
 *
 *        void main (Byte *args)
 *        {
 *            extern        SwitchRec        SW_DATE;
 *
 *            ConfigFixupLineBufferDate (args, &SW_DATE, '-');
 *            ConfigProcessLineBuffer (args);
 *            ...etc.
 *
 * Parameters:
 *        args                The unaltered command line, as passed to main()
 *        sw                The switch record that contains a date
 *        replace                The character to replace the switch character
 *
 * Returns:
 *        Nothing
 *
 * NOTE: A standard three-value date (month, day, and year, in any order)
 *       is assumed.
 * ------------------------------------------------------------------------ */

void SYM_EXPORT WINAPI ConfigFixupLineBufferDate (LPSTR args, LPCFGTEXTREC lpConfig, char replace)
{
    auto        int                len;
    auto        int                count;


    count = 2;                                        // should be two changes
    len = STRLEN (lpConfig->lpSwitch);

    while (*args && count != 0)
        {
        if (*args == s_cSwitchChar)                // search for any switch
            {
                                                    // found one, but is it ours?
            if (StringMatch (args + 1, lpConfig->lpSwitch, len) == 0)
                {
                args++;                                // jump over switch character
                args += len;                        // jump over switch text

                if (*args == ':')
                    args++;                        // jump over switch colon

                while (TRUE)
                    {                                // jump over valid numbers
                    while (*args && CharIsNumeric (*args))
                        args++;

                    if (*args == s_cSwitchChar)        // replace switch char with
                        {
                        *args++ = replace;        //   our replacement char
                        count--;
                        }
                    else
                            {
                        count = 0;
                            break;                         // break out if anything else
                        }
                    }
                }
            }

        args++;
        }
}

/* -------------------------------------------------------------------------
 * ConvertStringToEncodedDate
 *
 * Converts a text string into encoded date format.  The string should
 * contain a year, month, and day value separated by the designated
 * separator.  International formats are accounted for.
 *
 * Parameters:
 *        string                text string containing a complete date
 *        dateChar        separator for the date values
 *
 * Returns:
 *        Date in encoded format
 *
 * NOTE: Zero is assumed for any missing values.
 * ------------------------------------------------------------------------- */

UINT SYM_EXPORT WINAPI ConvertStringToEncodedDate (LPSTR string, char dateChar)
{
    auto        UINT                first, second, third;
    auto        UINT                date;


    first = second = third = 0;                        // start with nothing

    first = __ConvertStringToWord (string);        // get first value

    while (*string && *string != dateChar)
        string++;

    if (*string)
        second = __ConvertStringToWord (++string);        // get second value

    while (*string && *string != dateChar)
        string++;

    if (*string)
        third = __ConvertStringToWord (++string);        // get third value

                                        // account for international formats
    switch (international.dateFormat)
        {
        case 0:                                // MM/DD/YYYY
            if (third < 80)
                third += 2000;
            else if (third < 208)
                third += 1900;

            date = ConvertDateToEncoded (first, second, third);
            break;

        case 1:                                // DD/MM/YYYY
            if (third < 80)
                third += 2000;
            else if (third < 208)
                third += 1900;

            date = ConvertDateToEncoded (second, first, third);
            break;

        case 2:                                // YYYY/MM/DD
            if (first < 80)
                first += 2000;
            else if (first < 208)
                first += 1900;

            date = ConvertDateToEncoded (second, third, first);
            break;
        }

    return (date);
}

char LOCAL StringMatch (LPSTR string1, LPSTR string2, UINT characters)
{
    register LPSTR s = string1;
    register LPSTR d = string2;
    UINT i = 1;

    for (; CharToLower (*s) == CharToLower (*d); s++, d++, i++)
        if ((*s == '\0') || (i == characters))
            return(0);                        /* Strings matched                */

    return(*s - *d);                        /* Strings didn't match                */
}

#endif
