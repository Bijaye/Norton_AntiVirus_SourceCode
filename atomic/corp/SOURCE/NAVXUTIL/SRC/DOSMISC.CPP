// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/dosmisc.cpv   1.1   24 Feb 1997 13:03:34   DALLEE  $
//
// Description:
//      These are the miscellaneous support functions for DOS NAV 3.0
//
// Contains:
//      CutString()
//************************************************************************
// $Log:   S:/NAVXUTIL/VCS/dosmisc.cpv  $
// 
//    Rev 1.1   24 Feb 1997 13:03:34   DALLEE
// Port from HYDRA.15:
// 
//            Rev 1.6   25 Oct 1996 18:02:46   CCOTRIL
//         Turned net support off because DX platform will need additional
//         work before net network alerts function properly - this due to
//         lack of proper protected mode support in the Novell libraries.
// 
//            Rev 1.5   23 Sep 1996 17:18:30   PSPEAR
//         Reenabled network alerting
// 
//    Rev 1.0   06 Feb 1997 21:05:06   RFULLER
// Initial revision
// 
//    Rev 1.2   22 Oct 1996 17:02:24   MKEATIN
// DBCS enabled TEExtensionFilter().
// 
//    Rev 1.1   30 Sep 1996 13:36:16   JALLEE
// Minor DBCS changes, CutString().
// 
//    Rev 1.0   30 Jan 1996 15:56:18   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:51:20   BARRY
// Initial revision.
// 
//    Rev 1.4   06 Jun 1995 20:28:20   BARRY
// Commented out NETWORKALERT code for Luigi
// 
//    Rev 1.3   08 Mar 1995 16:07:38   DALLEE
// Moved DOS LoadString() cover here from NAVAPLOG.CPP
// 
//    Rev 1.2   29 Dec 1994 19:01:48   DALLEE
// Moved DOSNetGetCurrentUserName() from netalert.cpp to dosmisc.cpp.
// 
//    Rev 1.1   20 Dec 1994 17:46:06   MFALLEN
// SYMINTEG.H is needed since this was removed from NONAV.H
// 
//    Rev 1.0   16 Dec 1994 10:37:08   MFALLEN
// Initial revision.
// 
//    Rev 1.4   15 Dec 1994 20:13:34   MFALLEN
// From NIRVANA
// 
//    Rev 1.3   07 Sep 1993 23:11:32   BARRY
// Exclude  in extensions for now because it causes problems
// 
//    Rev 1.2   02 Sep 1993 18:24:16   BARRY
// Fixed String_Get buffer overrun bug
// 
//    Rev 1.1   27 Aug 1993 09:52:10   DALLEE
// Look for '\n' in CutString().
// 
//    Rev 1.0   12 Aug 1993 11:54:26   MFALLEN
// Initial Revision
// 
//    Rev 1.5   11 Aug 1993 19:10:52   DALLEE
// Moved NAVDWriteLog() to NAVDUTIL.C (NAVNET unresolved extern...)
// 
//    Rev 1.4   11 Aug 1993 18:11:46   DALLEE
// Added NAVDWriteLog() displays progress dialog before writing log records.
// 
//    Rev 1.3   23 Jul 1993 05:47:56   DALLEE
// String_Get cover for DOS.
// 
//    Rev 1.2   24 May 1993 15:38:32   DALLEE
// Added/Updated TE filters.
// 
//    Rev 1.1   06 Apr 1993 17:08:24   DALLEE
// Check for NULL ptr passed as source to CutString()
// 
//    Rev 1.0   06 Apr 1993 16:04:04   DALLEE
// Initial revision.
//************************************************************************

#define USE_NETWORKALERTS 0		// Note: this is disabled because
					// DOSNetGetCurrentUserName does not
					// function correctly on DX platform.
					// The routines it calls are not OK
					// for protected mode.

#include "platform.h"
#ifndef SYM_WIN
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"
#include "dosnet.h"

                                        // Symnet/Dosnet conflict.  Symnet.h
                                        // comes in from navutil.h ....
#define BINDERY_OBJECT_TIME     FAKE_BINDERY_OBJECT_TIME
 #include "symnet.h"
#undef BINDERY_OBJECT_TIME

#include "navutil.h"
#include "navdprot.h"
#include "syminteg.h"
#include "nonav.h"
#include "cmbstr.h"
#include "dbcs.h"

//************************************************************************
// CutString()
//
// This routine searches for the last white space in a string before
// wLength characters and copies that portion of szSource to szDest.
// If no white space is found, wLength characters are copied.
// The return value is a pointer to the next non-white character or NULL
// if the end of the string was reached.
//
// Parameters:
//      LPSTR   szDest                  Minimum (wLength + 1) size buffer
//                                        to receive shortened string
//      LPSTR   szSource                The original string
//      WORD    wLength                 Maximum length of shortened string
//
// Returns:
//      LPSTR   Pointer to the next non-white character in the source
//                string.
//              NOTE: if szSource is shorter than wLength, the return
//                value will be a pointer to the EOS.
//************************************************************************
// 3/30/93 DALLEE, Function created.
//************************************************************************

LPSTR PASCAL CutString (LPSTR szDest, LPSTR szSource, WORD wLength)
{
    auto    WORD    wIndex;
    auto    WORD    wSpace;

                                        // If we're passed a NULL pointer
                                        // for our source (as would happen
                                        // if the return value wasn't checked
                                        // and this function was called again
                                        // to get the next wLength characters),
                                        // zero the destination string and
                                        // return NULL.
    if (szSource == NULL)
        {
        szDest[0] = EOS;
        return (NULL);
        }

                                        // If szSource is short enough,
                                        // Just copy it and return.
    if (STRLEN(szSource) <= wLength)
        {
        STRCPY(szDest, szSource);
        return (NULL);
        }

                                        // Search for a '\n' first.
    wIndex = 0;
    while ((wIndex < wLength) && (szSource[wIndex] != '\n'))
        {
                                        // Skip double byte characters.
        if (DBCSIsLeadByte(szSource[wIndex]))
            {
            wIndex += 2;
            }
        else
            {
            wIndex++;
            }
        }


                                        // Find the last space before wLength
                                        // characters.
    if (szSource[wIndex] != '\n')
        {
        wIndex = 0;
        wSpace = 0;
        while (wIndex < wLength)
            {
            if (szSource[wIndex] == ' ')
                {
                    wSpace = wIndex;
                }
            if (DBCSIsLeadByte(szSource[wIndex]))
                {
                wIndex += 2;
                }
            else
                {
                wIndex++;
                }
            }
        wIndex = wSpace;
        }

    if (szSource[wIndex] != ' ' && szSource[wIndex] != '\n')
        {
        wIndex = wLength;
        }

                                        // Copy and NULL terminate the section
                                        // of szSource that we want.
    STRNCPY(szDest, szSource, wIndex);
    szDest[wIndex] = EOS;

    if (szSource[wIndex] == '\n')
        {
        wIndex++;
        }

    return (StringSkipWhite(szSource + wIndex));
} // End CutString()


/**************************************************************************
 * TEExtensionFilter
 *
 * DESCRIPTION  :
 *
 * IN      :
 *
 * RETURN VALUES:
 *************************************************************************
 * SKURTZ : 05-10-93 at 06:44:42pm Function created
 * DALLEE : Modified, filter path separators, else TEUpperPathWilcardFilter()
 * BarryG : 09-07-93 Exclude % (for now) (sometimes parsed like printf)
 **************************************************************************/

UINT TEExtensionFilter (TERec *te, UINT *c)
{
    auto        BYTE            byChar = (BYTE) *c;

                                        // Skip out on DBCS trail bytes.
    if (DBCSGetByteType(&(te->string[0]), 
                        &(te->string[te->insert_pt])) == _MBC_TRAIL)
        return (EVENT_AVAIL);

    if ( (byChar == '\\') || (byChar == '/') || (byChar == '.') ||
         (byChar == '%') )
        {
        Blip();
        return (EVENT_USED);
        }
    else
        {
        return (TEUpperPathWildcardFilter(te, c));
        }
} // End TEExtensionFilter()


//************************************************************************
// TEUpperPathWildcardFilter()
//
// This routine accepts only filename characters (translated to uppercase),
// path separators, and wildcard characters.
//
// Parameters:
//      TERec   *te
//      UINT    *c
//
// Returns:
//      EVENT_USED                      Ignore character.
//      EVENT_AVAIL                     Use this character.
//************************************************************************
// 5/21/93 DALLEE, Function created.
//************************************************************************

UINT TEUpperPathWildcardFilter (TERec *te, UINT *c)
{
    if ((*c == '*') || (*c == '?'))
        {
        return (EVENT_AVAIL);
        }

    return (TEUpperPathCharsOnlyFilter(te, c));
} // End TEUpperPathWildcardFilter()


//************************************************************************
// String_Get()
//
// DOS cover for the Windows routine.
//
// Parameters:
//      LPSTR   lpSource                Source string.
//      LPSTR   lpDest                  Destination buffer.
//      UINT    nNum                    Size of buffer (including EOS)
//
// Returns:
//      UINT                            Number of characters copied.
//************************************************************************
// 7/23/93 DALLEE, Function created.
//************************************************************************

UINT WINAPI String_Get(LPSTR lpSource, LPSTR lpDest, UINT uNum)
{
    STRNCPY(lpDest, lpSource, uNum);
    lpDest[uNum-1] = EOS;

    return (STRLEN(lpDest));
} // End String_Get()


//***************************************************************************
// LoadString()
// 
// Description:
//      This routine emulates the Windows LoadString routine for the
//      DOS components.
//
// Parameters:
//      hInstance                       - [IN] Not used
//      uID                             - [IN] Id of string to use
//      lpszBuffer                      - [OUT] Buffer to receive string
//      nBufferMax                      - [IN] Max size of buffer
//
// Return Value: 
//      UINT                            - Number of bytes in returned 
//                                        message string
// 
//***************************************************************************
// 12/28/1994 MARKL Function Created.                                   
//***************************************************************************

EXTERNC DOS_STRINGS DOS_Strings[];

int PASCAL LoadString (                 // Emulates Windows' LoadString()
    HINSTANCE   hInstance,              // [IN] Not used
    UINT        uID,                    // [IN] Id of string to use
    LPSTR       lpszBuffer,             // [OUT] Buffer to receive string
    int         nBufferMax)             // [IN] Max size of buffer
{
    SYM_ASSERT ( uID );                 // Must be a valid ID
    SYM_ASSERT ( lpszBuffer );          // Must be a valid pointer
    SYM_ASSERT ( nBufferMax );          // Can't be zero length

                                        // Initialize our return buffer.
    *lpszBuffer = EOS;

                                        // -----------------------------------
                                        // Stay in this loop while there
                                        // are strings to be searched and
                                        // try to fing the string that we're
                                        // looking for.
                                        // -----------------------------------

    for ( WORD wCurrent = 0; DOS_Strings[ wCurrent ].uID ; wCurrent++ )
        {
                                        // -----------------------------------
                                        // Is this the string ID that we're
                                        // looking for ?
                                        // -----------------------------------

        if ( DOS_Strings [ wCurrent ].uID == uID )
            {
                                        // -----------------------------------
                                        // We found the string we wanted,
                                        // now copy it to the buffer if there
                                        // is room.
                                        // -----------------------------------

            if ( STRLEN ( DOS_Strings[wCurrent].lpszString ) < nBufferMax )
                {
                STRCPY ( lpszBuffer, DOS_Strings[wCurrent].lpszString );
                }

            break;
            }
        }
                                        // Return to caller
    return (STRLEN (lpszBuffer));
} // LoadString()


//************************************************************************
// DOSNetGetCurrentUserName()
//
// NAV DOS function cover for SYMNET.LIB routine used by windows.
// The DONET.LIB is used in DOS.
//
// Parameters:
//
// Returns:
//      NOERR                           Success.
//      ERR                             Failure.
//************************************************************************
//  7/23/93 DALLEE, Function created.
// 12/29/94 DALLEE, moved from netalert.cpp
//************************************************************************

WORD PASCAL DOSNetGetCurrentUserName (LPSTR  lpszServer,
                                      LPSTR  lpszUser,
                                      WORD   wBufferSize)
{
#if USE_NETWORKALERTS                   // Real fix would be to use W95 name
    auto    WORD            wResult = ERR;
    auto    WORD            wConnectNumber;
    auto    CONNECTION_INFO rConnectInfo;

    if ( (0 != (wConnectNumber = NovellGetConnectionNumber())) &&
         (NOERR == NWGetConnectionInfo(wConnectNumber, &rConnectInfo)) )
        {
        STRNCPY(lpszUser, (LPSTR) rConnectInfo.szObjectName, wBufferSize);
        lpszUser [wBufferSize - 1] = EOS;
        }

    return (wResult);
#else
    lpszUser[0] = EOS;
    return (NOERR);
    
#endif

} // End DOSNetGetCurrentUserName()


#endif  // (#ifndef SYM_WIN)

