/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/CFG_TEXT.C_v   1.0   26 Jan 1996 20:22:06   JREARDON  $ *
 *                                                                      *
 * Description:                                                         *
 *      API Functions to manage the text configuration functions,       *
 *      which include command line switches and text-based .INI files.  *
 *                                                                      *
 * Contains:                                                            *
 *              ConfigReadValue                                         *
 *              ConfigReadString                                        *
 *              ConfigWriteString                                       *
 *              ConfigWriteValue                                        *
 *              ConfigSetState                                          *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/CFG_TEXT.C_v  $ *
// 
//    Rev 1.0   26 Jan 1996 20:22:06   JREARDON
// Initial revision.
// 
//    Rev 1.12   24 Aug 1994 15:26:20   DLEVITON
// ConfigReg calls.
//
//    Rev 1.11   15 Mar 1994 12:34:16   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.10   25 Feb 1994 12:23:10   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.9   27 Jul 1993 10:54:06   ED
// The return value of the ConfigSwitchGetText call in ConfigFileReadString
// was being interpreted backwards.
//
//    Rev 1.8   16 Jul 1993 12:01:32   PETERD
// Added ConfigReadValueArray() and ConfigWriteValueArray().
//
//    Rev 1.7   07 May 1993 15:51:16   EHU
// Added some includes for SYM_NLM for new platform.h, platnlm.h, file.h,
// and xapi.h.
//
//    Rev 1.6   22 Mar 1993 21:47:58   BRAD
// Added ability to have Private INI files.
//
//    Rev 1.5   15 Mar 1993 20:06:02   JMILLARD
// Change _ultoa to ULTOA - SYM_NLM uses ultoa
// moved stdlib.h to front of platform.h - collides with max/min macros
//
//    Rev 1.4   28 Feb 1993 16:03:50   DLEVITON
// Fixed ConfigWriteValue so it doesn't write data with commas (thousands
// separators) in data, because ConfigReadValue loses data after the comma.
//
//
//    Rev 1.3   12 Feb 1993 15:25:56   ENRIQUE
// No change.
//
//    Rev 1.2   15 Dec 1992 06:56:14   ED
// Cast ERR to a UINT so the DOS version would compile
//
//    Rev 1.1   08 Sep 1992 09:06:14   ED
// Final, version 1.0
//
//    Rev 1.0   03 Sep 1992 09:10:14   ED
// Initial revision.
 ************************************************************************/

#include <stdlib.h>

#ifdef SYM_NLM
#include <ctype.h>
#endif

#include "platform.h"
#ifdef SYM_WIN32
#include "winreg.h"
#endif
#include "xapi.h"
#include "file.h"
#include "symcfg.h"

static char        szValue [MAX_INI_LINE + 1];

void    LOCAL PASCAL ConvertBoolean (UINT FAR *lpwValue, LPSTR lpEnd);

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      This function retrieves a value from an entry in the text INI   *
 *      files, or from a command line switch.  The switch is checked    *
 *      first.  Boolean strings (TRUE, FALSE, YES, NO, ON, OFF) in      *
 *      INI files are converted to TRUE/FALSE values.                   *
 *                                                                      *
 * Parameters:                                                          *
 *      lpConfig        The text config record.                         *
 *                                                                      *
 *      lpwValue        If not NULL, returns the value of the setting.  *
 *                                                                      *
 *      lpwState        If not NULL, returns the state of the setting   *
 *                      or switch, which can be one of the following:   *
 *                                                                      *
 *                      CFG_SWITCH_NOT_SET                              *
 *                      CFG_SWITCH_SET                                  *
 *                      CFG_SWITCH_PLUS                                 *
 *                      CFG_SWITCH_MINUS                                *
 *                                                                      *
 * Return Value:                                                        *
 *      TRUE if the setting exists as a command line switch or as a     *
 *      setting in the .INI file.  Otherwise, it is FALSE.              *                                                                      *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigReadValue (
    LPCFGTEXTREC        lpConfig,
    UINT FAR            *lpwValue,
    UINT FAR            *lpwState)
{
    return( ConfigFileReadValue (NULL, lpConfig, lpwValue, lpwState) );
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      This function retrieves a value from an entry in the text INI   *
 *      files, or from a command line switch.  The switch is checked    *
 *      first.  Boolean strings (TRUE, FALSE, YES, NO, ON, OFF) in      *
 *      INI files are converted to TRUE/FALSE values.                   *
 *                                                                      *
 * Parameters:                                                          *
 *      lpConfig        The text config record.                         *
 *                                                                      *
 *      lpwValue        If not NULL, returns the value of the setting.  *
 *                                                                      *
 *      lpwState        If not NULL, returns the state of the setting   *
 *                      or switch, which can be one of the following:   *
 *                                                                      *
 *                      CFG_SWITCH_NOT_SET                              *
 *                      CFG_SWITCH_SET                                  *
 *                      CFG_SWITCH_PLUS                                 *
 *                      CFG_SWITCH_MINUS                                *
 *                                                                      *
 * Return Value:                                                        *
 *      TRUE if the setting exists as a command line switch or as a     *
 *      setting in the .INI file.  Otherwise, it is FALSE.              *                                                                      *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigFileReadValue (
    LPCFGFILE           lpFile,
    LPCFGTEXTREC        lpConfig,
    UINT FAR            *lpwValue,
    UINT FAR            *lpwState)
{
    UINT        wErr = (UINT) ERR;
    LPSTR       lpEnd;

                                        // get the string value for the
                                        // setting
    if (ConfigFileReadString (lpFile, lpConfig, (LPSTR) szValue,
                                                     sizeof(szValue)) != ERR)
        {
        wErr = NOERR;                   // we got something

        if (lpwState)                   // do we care about the state?
            {
                                        // get the end of the string
            lpEnd = StringGetEnd ((LPSTR) szValue) - 1;

            if (*lpEnd == '+')           // anything special?
                *lpwState = CFG_SWITCH_PLUS;
            else if (*lpEnd == '-')
                *lpwState = CFG_SWITCH_MINUS;
            else
                *lpwState = CFG_SWITCH_SET;
            }

        if (lpwValue)                   // do we really care about the value?
            {
            lpEnd = (LPSTR) szValue;

            while (IS_WHITE(*lpEnd))    // clear leading white space
                lpEnd++;

                                        // convert the numerical value, or
                                        // convert a boolean string value to
                                        // a zero or one.
            if (CharIsNumeric (*lpEnd))
                ConvertStringToWord (lpwValue, lpEnd);
            else
                ConvertBoolean (lpwValue, lpEnd);
            }
        }
    else                                // not found
        {
        wErr = (UINT) ERR;                     // nope, wasn't there

        if (lpwState)                   // do we care about the state?
            *lpwState = CFG_SWITCH_NOT_SET;

        if (lpwValue)                   // do we care about the value?
            *lpwValue = FALSE;
        }

    return (wErr);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Returns the text string associated with a entry in a text       *
 *      .INI file or from a command line switch.                        *
 *                                                                      *
 * Parameters:                                                          *
 *      lpConfig        The text config record.                         *
 *                                                                      *
 *      lpBuffer        Pointer to the return buffer.                   *
 *                                                                      *
 *      wSize           Maximum number of characters to return.         *
 *                                                                      *
 * Return Value:                                                        *
 *      The number of characters copied to the buffer, or ERR if        *
 *      the setting wasn't found.                                       *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigReadString (LPCFGTEXTREC lpConfig, LPSTR lpBuffer, UINT wSize)
{
    return( ConfigFileReadString(NULL, lpConfig, lpBuffer, wSize) );
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Returns the text string associated with a entry in a text       *
 *      .INI file or from a command line switch.                        *
 *                                                                      *
 * Parameters:                                                          *
 *      lpConfig        The text config record.                         *
 *                                                                      *
 *      lpBuffer        Pointer to the return buffer.                   *
 *                                                                      *
 *      wSize           Maximum number of characters to return.         *
 *                                                                      *
 * Return Value:                                                        *
 *      The number of characters copied to the buffer, or ERR if        *
 *      the setting wasn't found.                                       *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigFileReadString (
    LPCFGFILE           lpFile,
    LPCFGTEXTREC        lpConfig,
    LPSTR               lpBuffer,
    UINT                wSize)
{

    if (lpConfig->lpSwitch)             // check the command line
        {
        if (ConfigSwitchGetText (lpConfig, lpBuffer, wSize))
            {
            return (STRLEN (lpBuffer));
            }
        }

    if (lpConfig->lpSection)            // check the text .INI file
        {
        if ( lpFile == NULL )           // Use SYMCFG files
            {
            if (ConfigIniGet (lpConfig, lpBuffer, wSize) == NOERR)
                return (STRLEN (lpBuffer));
            }
        else                            // Use Private files
            {
            if (ConfigIniFileGet (lpFile, lpConfig, lpBuffer, wSize) == NOERR)
                return (STRLEN (lpBuffer));
            }
        }

    return ((UINT) ERR);
}

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Returns the array of integers associated with a entry in a text *
 *      .INI file or from a command line switch.                        *
 *                                                                      *
 * Parameters:                                                          *
 *      lpConfig        The text config record.                         *
 *                                                                      *
 *      lpwArray        Pointer to the return buffer.                   *
 *                                                                      *
 *      wSize           Maximum number of integers to return.           *
 *                                                                      *
 * Return Value:                                                        *
 *      The number of integers copied to the buffer, or ERR if          *
 *      the setting wasn't found.                                       *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/16/1993 PETERD Created.                                           *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigReadValueArray (
    LPCFGTEXTREC        lpConfig,
    UINT FAR            *lpwValue,
    UINT                wSize)
{
    return( ConfigFileReadValueArray (NULL, lpConfig, lpwValue, wSize) );
}

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      This function retrieves an array of values from an entry in the *
 *      text INI files, or from a command line switch.  The switch is   *
 *      checked first.  Boolean strings (TRUE, FALSE, YES, NO, ON, OFF) *
 *      in INI files are converted to TRUE/FALSE values.                *
 *                                                                      *
 * Parameters:                                                          *
 *      lpConfig        The text config record.                         *
 *                                                                      *
 *      lpwValue        Returns the array of values of the setting.     *
 *                                                                      *
 *      wSize           The number of integers in the array             *
 *                                                                      *
 * Return Value:                                                        *
 *      The number of integers copied to the buffer, or ERR if          *
 *      the setting wasn't found.                                       *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/16/1993 PETERD Created.                                           *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigFileReadValueArray (
    LPCFGFILE           lpFile,
    LPCFGTEXTREC        lpConfig,
    UINT FAR            *lpwValue,
    UINT                wSize)
{
    UINT        wErr = (UINT) ERR;
    LPSTR       lpEnd;

                                        // get the string value for the
                                        // setting
    if (ConfigFileReadString (lpFile, lpConfig, (LPSTR) szValue,
                                                     sizeof(szValue)) != ERR)
        {
        lpEnd = (LPSTR)szValue;

        while (IS_WHITE(*lpEnd))        // clear leading white space
            lpEnd++;

        for (wErr = 0; wErr < wSize && *lpEnd != '\0'; ++wErr, ++lpwValue)
            {
            if (CharIsNumeric (*lpEnd))
                ConvertStringToWord (lpwValue, lpEnd);
            else
                ConvertBoolean (lpwValue, lpEnd);

            while (!IS_WHITE(*lpEnd) && *lpEnd != ',' && *lpEnd != '\0')
                lpEnd++;

            while (IS_WHITE(*lpEnd))
                lpEnd++;

            if (*lpEnd == ',')
                {
                lpEnd++;

                while (IS_WHITE(*lpEnd))
                    lpEnd++;
                }
            }
        }
    else                                // not found
        {
        wErr = (UINT) ERR;              // nope, wasn't there
        }

    return (wErr);
}

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Writes a string to the text INI file.                           *
 *                                                                      *
 * Parameters:                                                          *
 *      lpConfig        The text config record.                         *
 *                                                                      *
 *      lpBuffer        Pointer to string to write.                     *
 *                                                                      *
 * Return Value:                                                        *
 *      NOERR   Function was successful                                 *
 *      ERR     Function failed                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigWriteString (LPCFGTEXTREC lpConfig, LPSTR lpBuffer)
{
    return ( ConfigFileWriteString(NULL, lpConfig, lpBuffer) );
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Writes a string to the text INI file.                           *
 *                                                                      *
 * Parameters:                                                          *
 *      lpConfig        The text config record.                         *
 *                                                                      *
 *      lpBuffer        Pointer to string to write.                     *
 *                                                                      *
 * Return Value:                                                        *
 *      NOERR   Function was successful                                 *
 *      ERR     Function failed                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigFileWriteString (
    LPCFGFILE           lpFile,
    LPCFGTEXTREC        lpConfig,
    LPSTR lpBuffer)
{
    UINT        wRet = NOERR;

    if (lpConfig->lpSection)            // check the text .INI file
        {
        if ( lpFile == NULL )
            wRet = ConfigIniSet (lpConfig, lpBuffer);
        else
            wRet = ConfigIniFileSet (lpFile, lpConfig, lpBuffer);
        }

    return (wRet);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Writes a numerical value to the text INI file.  The value       *
 *      is converted to a string before it is written.                  *
 *                                                                      *
 * Parameters:                                                          *
 *      lpConfig        The text config record.                         *
 *                                                                      *
 *      wValue          Value to write to the file.                     *
 *                                                                      *
 * Return Value:                                                        *
 *      NOERR   Function was successful                                 *
 *      ERR     Function failed                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigWriteValue (LPCFGTEXTREC lpConfig, UINT wValue)
{
    return ( ConfigFileWriteValue(NULL, lpConfig, wValue) );
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Writes a numerical value to the text INI file.  The value       *
 *      is converted to a string before it is written.                  *
 *                                                                      *
 * Parameters:                                                          *
 *      lpConfig        The text config record.                         *
 *                                                                      *
 *      wValue          Value to write to the file.                     *
 *                                                                      *
 * Return Value:                                                        *
 *      NOERR   Function was successful                                 *
 *      ERR     Function failed                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigFileWriteValue (
    LPCFGFILE           lpFile,
    LPCFGTEXTREC        lpConfig,
    UINT                wValue)
{
    // Can't use ConvertWordToString because ConvertStringToWord is not
    // its inverse, due to insertion of thousands separator character.
    ULTOA( (DWORD)wValue, szValue, 10 );

    return (ConfigFileWriteString (lpFile, lpConfig, (LPSTR) szValue));
}

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Writes a numerical value to the text INI file.  The value       *
 *      is converted to a string before it is written.                  *
 *                                                                      *
 * Parameters:                                                          *
 *      lpConfig        The text config record.                         *
 *                                                                      *
 *      wValue          Value to write to the file.                     *
 *                                                                      *
 *      wSize           Number of UINTs to write                        *
 *                                                                      *
 * Return Value:                                                        *
 *      NOERR   Function was successful                                 *
 *      ERR     Function failed                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/16/1993 PETERD Created.                                           *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigWriteValueArray (
    LPCFGTEXTREC    lpConfig,
    UINT FAR *      lpwValue,
    UINT            wSize)
{
    return ( ConfigFileWriteValueArray(NULL, lpConfig, lpwValue, wSize) );
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Writes an array of numerical values to the text INI file.  The  *
 *      The values are converted to a string (decimal ints seperated by *
 *      commas) before it is written.                                   *
 *                                                                      *
 * Parameters:                                                          *
 *      lpConfig        The text config record.                         *
 *                                                                      *
 *      lpwValue        Array to write to the file.                     *
 *                                                                      *
 *      wSize           Number of UINTs in the array                    *
 *                                                                      *
 * Return Value:                                                        *
 *      NOERR   Function was successful                                 *
 *      ERR     Function failed                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/16/1993 PETERD Created.                                           *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigFileWriteValueArray (
    LPCFGFILE           lpFile,
    LPCFGTEXTREC        lpConfig,
    UINT FAR *          lpwValue,
    UINT                wSize)
{
    UINT wCnt;

    szValue[0] = '\0';

    for (wCnt = 0; wCnt < wSize; ++wCnt, ++lpwValue)
        {
        if (wCnt != 0)
            STRCAT(szValue, ",");

        // Can't use ConvertWordToString because ConvertStringToWord is not
        // its inverse, due to insertion of thousands separator character.
        ULTOA( (DWORD)*lpwValue, &szValue[STRLEN(szValue)], 10 );
        }

    return (ConfigFileWriteString (lpFile, lpConfig, (LPSTR) szValue));

}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      This function retrieves a value from an entry in the text INI   *
 *      files, or from a command line switch.  The switch is checked    *
 *      first.  Boolean strings (TRUE, FALSE, YES, NO, ON, OFF) in      *
 *      INI files are converted to TRUE/FALSE values.                   *
 *                                                                      *
 * Parameters:                                                          *
 *      lpConfig        The text config record.                         *
 *                                                                      *
 *      lpwValue        If not NULL, returns the value of the setting.  *
 *                                                                      *
 *      lpwState        If not NULL, returns the state of the setting   *
 *                      or switch, which can be one of the following:   *
 *                                                                      *
 *                      CFG_SWITCH_NOT_SET                              *
 *                      CFG_SWITCH_SET                                  *
 *                      CFG_SWITCH_PLUS                                 *
 *                      CFG_SWITCH_MINUS                                *
 *                                                                      *
 * Return Value:                                                        *
 *      TRUE if the setting exists as a command line switch or as a     *
 *      setting in the .INI file.  Otherwise, it is FALSE.              *                                                                      *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigSetState (LPCFGTEXTREC lpConfig, UINT wState)
{
    return ( ConfigFileSetState(NULL, lpConfig, wState) );
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      This function retrieves a value from an entry in the text INI   *
 *      files, or from a command line switch.  The switch is checked    *
 *      first.  Boolean strings (TRUE, FALSE, YES, NO, ON, OFF) in      *
 *      INI files are converted to TRUE/FALSE values.                   *
 *                                                                      *
 * Parameters:                                                          *
 *      lpConfig        The text config record.                         *
 *                                                                      *
 *      lpwValue        If not NULL, returns the value of the setting.  *
 *                                                                      *
 *      lpwState        If not NULL, returns the state of the setting   *
 *                      or switch, which can be one of the following:   *
 *                                                                      *
 *                      CFG_SWITCH_NOT_SET                              *
 *                      CFG_SWITCH_SET                                  *
 *                      CFG_SWITCH_PLUS                                 *
 *                      CFG_SWITCH_MINUS                                *
 *                                                                      *
 * Return Value:                                                        *
 *      TRUE if the setting exists as a command line switch or as a     *
 *      setting in the .INI file.  Otherwise, it is FALSE.              *                                                                      *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 08/31/1992 ED Created.                                               *
 ************************************************************************/

UINT SYM_EXPORT WINAPI ConfigFileSetState (
    LPCFGFILE           lpFile,
    LPCFGTEXTREC        lpConfig,
    UINT wState)
{
    LPSTR       lpEnd;

                                        // get the string value for the
                                        // setting
    if (ConfigFileReadString (lpFile, lpConfig, (LPSTR) szValue, sizeof(szValue)) != ERR)
        {
        lpEnd = StringGetEnd ((LPSTR) szValue);

                                        // kill the existing state
        if (*(lpEnd - 1) == '+' || *(lpEnd - 1) == '-')
            *(--lpEnd) = '\0';
                                        // tack on the new setting
        if (wState == CFG_SET_PLUS)
            *lpEnd++ = '+';
        else if (wState == CFG_SET_MINUS)
            *lpEnd++ = '-';

        *lpEnd = '\0';

        ConfigFileWriteString (lpFile, lpConfig, (LPSTR) szValue);
        }

    return (NOERR);
}


static struct
    {
    LPSTR       lpName;
    UINT        wValue;
    UINT        wLength;
    } rBoolRec[] =
        {
            { "TRUE",   TRUE  , 1},
            { "FALSE",  FALSE , 1},
            { "ON",     TRUE  , 2},
            { "OFF",    FALSE , 2},
            { "YES",    TRUE  , 1},
            { "NO",     FALSE , 1},
            { NULL }
        };

void LOCAL PASCAL ConvertBoolean (UINT FAR *lpwValue, LPSTR lpEnd)
{
    UINT        x = 0;

    STRUPR (lpEnd);                     // uppercase for ease

    while (rBoolRec[x].lpName)
        {
        if (STRNCMP(rBoolRec[x].lpName, lpEnd, rBoolRec[x].wLength) == 0)
            {
            *lpwValue = rBoolRec[x].wValue;
            return;
            }

        x++;
        }

    *lpwValue = FALSE;
}


