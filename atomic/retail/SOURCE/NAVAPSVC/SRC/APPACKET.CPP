// Copyright 1996 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAPSVC/VCS/appacket.cpv   1.1   18 Apr 1997 13:59:12   DSACKIN  $
//
// Description:
//      This file defines helper functions for working with NAVAP packets.
//
// Contains:
//      DecodeNAVAPUserInfoSubPacket()
//
// See Also:
//      appacket.h
//
//***************************************************************************
// $Log:   S:/NAVAPSVC/VCS/appacket.cpv  $
// 
//    Rev 1.1   18 Apr 1997 13:59:12   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:12:16   RFULLER
// Initial revision
// 
//    Rev 1.1   24 Dec 1996 18:52:30   DSACKIN
// Include afxtmpl.h
//
//    Rev 1.0   08 Nov 1996 12:16:12   DKEEFER
// Initial revision
//
//    Rev 1.4   28 Jul 1996 10:36:56   PVENKAT
// Getting the UserName/DomainName from Registry if LookupSid fails.
//
//
//    Rev 1.3   10 Jul 1996 10:35:52   PVENKAT
// Modified to return "Unknown User" / "Unknwon domain" if LookupAccountSid
// fails with AP Driver's SID.  (This can happen on Cahed account loggin to
// a Domain when Domain is down and as this service is started on SYSTEM
// account).
//
//
//    Rev 1.2   17 Jun 1996 20:19:06   DSACKIN
// Included apscomm.h for a definition needed for navapsvc.h
//
//    Rev 1.1   16 May 1996 20:31:16   DSACKIN
// Added this to the APService
//
//    Rev 1.0   16 May 1996 10:26:08   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "afxtempl.h"
#include "platform.h"
#include "drvcomm.h"
#include "apcomm.h"
#include "appacket.h"

#include "navalog.h"
#include "navaplog.h"
#include "nonav.h"

#include "apscomm.h"
#include "navapsvc.h"
#include "apsvcstr.h"
//---------------------------------------------------------------------------
//
// BOOL DecodeNAVAPUserInfoSubPacket (
//      PNAVAP_USERINFOSUBPACKET prPacket,
//      PTSTR                    pszUser,
//      PDWORD                   pcchUser,
//      PTSTR                    pszDomain,
//      PDWORD                   pcchDomain
//      );
//
// This function decodes a NAVAP user info subpacket.  This "subpacket" is
// sent as part of some NAVAP's packets when user information is required
// for their processing.  Please refer to APCOMM.H on what the subpacket
// exactly contains.
//
// Entry:
//      prPacket   - pointer to a subpacket, as sent by NAVAP.
//      pszUser    - pointer to a buffer to receive the name of the user
//                   associated with prPacket.  This parameter can be set to
//                   NULL if the client does not need to retrieve that name.
//      pcchUser   - pointer to DWORD containing the size (in characters) of
//                   the memory block pointed to by pszUser.  This parameter
//                   is ignored if pszUser is NULL.  If pcchUser is NULL,
//                   or *pcchUser is 0, then nothing will be copied to
//                   pszUser, regardless of what pszUser points to.
//      pszDomain  - pointer to a buffer to receive the name of the
//                   computer or domain associated with prPacket.  This
//                   parameter can be set to NULL if the client does not
//                   need to retrieve the domain/computer name.
//      pcchDomain - pointer to DWORD containing the size (in characters) of
//                   the memory block pointed to by pszDomain.  This
//                   parameter is ignored if pszDomain is NULL.  If
//                   pcchDomain is NULL, or *pcchDomain is 0, then nothing
//                   will be copied to pszDomain, regardless of what
//                   pszDomain points to.
//
// Exit:
//      Returns TRUE if successfull, FALSE otherwise.
//
//      Upon successful completion of this function, *pcchUser (if
//      applicable) will be set to the number of characters copied to
//      pszUser, and *pcchDomain (if applicable) will be set to the number
//      of characters copied to pszDomain.
//
//      If this function fails, the status of the return buffers is
//      undefined.
//
//---------------------------------------------------------------------------
BOOL CNavAPService::DecodeNAVAPUserInfoSubPacket (
    PNAVAP_USERINFOSUBPACKET prPacket,
    PTSTR                    pszUser,
    PDWORD                   pcchUser,
    PTSTR                    pszDomain,
    PDWORD                   pcchDomain
    )
    {
    auto DWORD  cchTempUser, cchTempDomain;
    TCHAR       szDefUser[SYM_MAX_PATH + 1], szDefDomain[SYM_MAX_PATH + 1] ;
	DWORD		dwUser, dwDomain;

                                        // Validate input parameters.

    SYM_ASSERT ( prPacket );
    SYM_VERIFY_BUFFER ( prPacket, sizeof(*prPacket) );

                                        // Adjust parameters to make it easy
                                        // on us when handling all different
                                        // cases of present and not-present
                                        // arguments.  At the same time
                                        // initialize local variables.

    if ( !pszUser || !pcchUser || !*pcchUser )
        {
        pszUser = NULL;

        pcchUser && ( *pcchUser = 0 );

        pcchUser = &cchTempUser;

        cchTempUser = 0;
        }
    else
        {
        cchTempUser = *pcchUser;

        SYM_VERIFY_BUFFER ( pszUser, cchTempUser * sizeof(TCHAR) );
        }

    if ( !pszDomain || !pcchDomain || !*pcchDomain )
        {
        pszDomain = NULL;

        pcchDomain && ( *pcchDomain = 0 );

        pcchDomain = &cchTempDomain;

        cchTempDomain = 0;
        }
    else
        {
        cchTempDomain = *pcchDomain;

        SYM_VERIFY_BUFFER ( pszDomain, cchTempDomain * sizeof(TCHAR) );
        }

                                        // In order for us to cut down on
                                        // calls to LookupAccountSid(), we
                                        // will make sure the client has
                                        // requested at least one user data
                                        // string.

    if ( !pszUser && !pszDomain )
        {
        return ( TRUE );
        }

                                        // See what kind of packet we have
                                        // here.

    if ( prPacket->bUseAsSIDInsteadOfString )
        {
                                        // We have a SID.

        auto SID_NAME_USE eUse;

                                        // Calculate how much memory we need
                                        // for SID conversion...  If this
                                        // initial conversion succeeds, this
                                        // means that the user requested
                                        // both user and computer/domain
                                        // names and the supplied buffers
                                        // were large enough to hold them.

        if ( LookupAccountSid ( NULL,
                                &prPacket->rUserInfo.Sid,
                                pszUser,
                                pcchUser,
                                pszDomain,
                                pcchDomain,
                                &eUse ) )
            {
            return ( TRUE );
            }
        else
            {
            auto BOOL bResult;
            DWORD dwErr = GetLastError ();

                                        // Prepare to bail out at any time

            bResult = FALSE;

                                        // Make sure the reason for the
                                        // failure was a missing buffer.

            if ( GetLastError() == ERROR_INSUFFICIENT_BUFFER &&

                                        // Check if the user supplied buffers
                                        // are large enough for the SID
                                        // conversion.

                 ( !pszUser || ( cchTempUser >= *pcchUser ) ) &&
                 ( !pszDomain || ( cchTempDomain >= *pcchDomain ) ) &&

                                        // Allocate necessary memory

                 ( pszUser || ( pszUser = (PTSTR) MemAllocPtr (
                                GHND, cchTempUser * sizeof(TCHAR) ) ) ) &&
                 ( pszDomain || ( pszDomain = (PTSTR) MemAllocPtr (
                                  GHND, cchTempDomain * sizeof(TCHAR) ) ) )
               )
                {
                                        // Convert SID

                if ( LookupAccountSid ( NULL,
                                        &prPacket->rUserInfo.Sid,
                                        pszUser,
                                        pcchUser,
                                        pszDomain,
                                        pcchDomain,
                                        &eUse ) )
                    {
                    bResult = TRUE;
                    }

                                        // Release allocated memory.

                if ( pcchUser == &cchTempUser )
                    {
                    MemFreePtr ( pszUser );
                    }

                if ( pcchDomain == &cchTempDomain )
                    {
                    MemFreePtr ( pszDomain );
                    }
                }
            else // LookupAccountSid Failed and buffer sizes seems to be ok
            {
                //
                // Services get started in "SYSTEM" account and in this
                // situation where we log on to Domain normally, and if the
                // domain is down we log on to Cached account.  In this case
                // LookupAccountSid will fail with ERROR_NONE_MAPPED and will
                // not return the User name and domain name, so we give
                // the default "UNKNWON".

                // This problem can be repeated also with AP Agent if it is
                // started using scheduler (in which case starts on SYSTEM
                // account).

                // Check up with RAD before modifying this section.

				dwUser = sizeof (szDefUser);
				dwDomain = sizeof (szDefDomain);

				if (!GetRegUserName (szDefUser, &dwUser, szDefDomain, &dwDomain))
				{
                	MEMSET (szDefUser, 0, sizeof (szDefUser));
                	MEMSET (szDefDomain, 0, sizeof (szDefDomain));

                	LoadString (m_hInst, 
                                IDS_NAVAP_DEFUSERNAME, 
                                szDefUser, 
                                sizeof (szDefUser) / sizeof (szDefUser[0]) );
                	LoadString (m_hInst,
                                IDS_NAVAP_DEFDOMAINNAME,
                                szDefDomain,
                                sizeof (szDefDomain) / 
                                        sizeof (szDefDomain[0]));
				}

                STRCPY (pszUser, szDefUser);
                STRCPY (pszDomain, szDefDomain);

                bResult = TRUE;
                return bResult;
            }

            return ( bResult );
            }
        }

                                        // We have a UNC-style string.

    auto PWSTR pszStart, pszEnd, pszLastSeparator;
    auto DWORD cchMisc;

                                        // Get the status of the separators
                                        // in the string.

    for ( pszStart = pszEnd = prPacket->rUserInfo.szUserID, cchMisc = 0;
          *pszEnd;
          pszEnd++ )
        {
        if ( *pszEnd == '\\' )
            {
            pszLastSeparator = pszEnd;

            cchMisc++;
            }
        }

                                        // Make sure we have a valid string.
                                        // That is "\\domain\user" or "user".
                                        // Note, that we will allow empty
                                        // strings for domain or user names.

    if ( cchMisc &&
         ( cchMisc != 3 || pszStart[0] != '\\' || pszStart[1] != '\\' )
       )
        {
        return ( FALSE );
        }

#if defined(UNICODE)

    auto DWORD cchLength;

                                        // Copy the user name.

    if ( pszUser )
        {
        cchLength = pszEnd - ( cchMisc ? pszLastSeparator + 1 :
                                         pszStart );

        if ( cchLength < cchTempUser )
            {
            MEMCPY ( pszUser,
                     cchMisc ? pszLastSeparator + 1 : pszStart,
                     cchLength * sizeof(WCHAR) );

            pszUser[cchLength] = '\0';

            *pcchUser = cchLength + 1;
            }
        else
            {
            return ( FALSE );
            }
        }

                                        // Copy the domain/server name

    if ( pszDomain )
        {
        if ( cchMisc )
            {
            pszStart += 2;

            cchLength = pszLastSeparator - pszStart;

            if ( cchLength < cchTempDomain )
                {
                MEMCPY ( pszDomain,
                         pszStart,
                         cchLength * sizeof(WCHAR) );

                pszDomain[cchLength] = '\0';

                *pcchDomain = cchLength + 1;
                }
            else
                {
                return ( FALSE );
                }
            }
        else
            {
            *pszDomain = '\0';

            *pcchDomain = 1;
            }
        }

    return ( TRUE );

#else

    return ( FALSE );

#endif
    }

BOOL	
CNavAPService::GetRegUserName (LPTSTR lpszUserName, 
								LPDWORD lpdwUserLen, 
								LPTSTR lpszDomainName, 
								LPDWORD lpdwDomainLen)
{
	TCHAR		szRegKey [ 256 ] ;
	TCHAR		szUserKey [ 256 ] ;
	TCHAR		szDomainKey [256];
	DWORD		dwType, dwErr;
	HKEY		hKey;

	LoadString ( m_hInst,
				IDS_NAVAP_REGKEY,
				szRegKey,
				sizeof ( szRegKey ) ) ;
	LoadString ( m_hInst,
				IDS_NAVAP_USERKEY,
				szUserKey,
				sizeof ( szUserKey ) ) ;
	LoadString ( m_hInst,
				IDS_NAVAP_DOMAINKEY,
				szDomainKey,
				sizeof ( szDomainKey ) ) ;

	dwErr = RegOpenKeyEx (HKEY_LOCAL_MACHINE,
							szRegKey,
							0,
							KEY_QUERY_VALUE,
							&hKey);

	if (dwErr == ERROR_SUCCESS)
	{
		dwErr = RegQueryValueEx ( hKey, 
									szUserKey, 
									0,
									&dwType,
									(LPBYTE) lpszUserName,
									lpdwUserLen);
		if (dwErr == ERROR_SUCCESS)
		{
			dwErr = RegQueryValueEx ( hKey,
										szDomainKey,
										0,
										&dwType,
										(LPBYTE) lpszDomainName,
										lpdwDomainLen);
		}
	}

	return (dwErr == ERROR_SUCCESS) ? TRUE : FALSE;
}
