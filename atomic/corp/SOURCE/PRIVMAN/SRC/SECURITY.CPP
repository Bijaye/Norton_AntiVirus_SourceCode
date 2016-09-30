// Copyright 1997 Symantec Corporation, Peter Norton Product Group
//***************************************************************************
//
//	$Header:   S:/PRIVMAN/VCS/SECURITY.CPv   1.1   13 May 1998 19:45:34   jtaylor  $
//
//	Security.CPP
//
//		Functions to access LSA Database and grant/remove privileges
//		for a given user on a given machine
//
//***************************************************************************
//	$Log:   S:/PRIVMAN/VCS/SECURITY.CPv  $
// 
//    Rev 1.1   13 May 1998 19:45:34   jtaylor
// Made error strings be loaded from the string table.
//
//    Rev 1.0   25 Jun 1997 16:07:14   DSACKIN
// Initial revision.
//
//    Rev 1.1   24 Apr 1997 14:31:40   pvenkat
// Displaying better error messages.  Processing RPC_S_SERVER_UNAVAILABLE &
// ERROR_ACCESS_DENIED errors only now.
//
//
//    Rev 1.0   16 Apr 1997 18:00:00   pvenkat
// Initial revision.
//***************************************************************************


// Include files.
#include	<stdafx.h>
#include	"ntsecapi.h"		// From NTDDK for Security functions
#include    "resource.h"


VOID
DisplayError (DWORD dwErr)
{
    CString strErrorText;
    CString strErrorTitle;

    switch (dwErr)
    {
        case    RPC_S_SERVER_UNAVAILABLE:       // 1722: Unable to access given machine
            strErrorText.LoadString(IDS_COMP_UNAVAIL);
            strErrorTitle.LoadString(IDS_GRANT_PRIVILEGES);
            MessageBox (NULL, strErrorText, strErrorTitle, MB_OK);
            break;

        case    ERROR_ACCESS_DENIED:
            strErrorText.LoadString(IDS_ACCESS_DENIED);
            strErrorTitle.LoadString(IDS_GRANT_PRIVILEGES);
            MessageBox (NULL, strErrorText, strErrorTitle, MB_OK);
            break;
    }
}


//*********************************************************************
//
//  InitLsaString
//
//      To initilialise given LSA_UNICODE_STRING by given UNICODE string
//
//  Parameters:
//      LsaString       PLSA_UNICODE_STRING
//      lpszString      LPWSTR
//
//  Retruns:
//      -None-
//
//*********************************************************************
VOID
InitLsaString (PLSA_UNICODE_STRING pLsaString, LPWSTR lpszString)
{
	DWORD	dwStringLength;

    // if the given string is NULL, then just fill with NULLs.
	if (lpszString == NULL)
	{
		pLsaString->Buffer = NULL;
		pLsaString->Length = 0;
		pLsaString->MaximumLength = 0;
	}
    else
    {
    	dwStringLength = wcslen (lpszString);
	    pLsaString->Buffer = lpszString;
    	pLsaString->Length = (USHORT) dwStringLength * sizeof (WCHAR);
	    pLsaString->MaximumLength = (USHORT)
	                            (dwStringLength + 1) * sizeof (WCHAR);
    }
}


//*********************************************************************
//
// OpenPolicy
//
//      To open policy database
//
// Parameters:
//      lpszServer  LPWSTR      Name of the Target computer
//      dwAccess    DWORD       Desired access to database
//      phPolicy    PLSA_HANDLE Buffer to return Handle to DB
//
//  Returns:
//      ERROR_SUCCESS   on success and appropriate error code
//
//*********************************************************************
DWORD
OpenPolicy (LPWSTR lpszServer, DWORD dwAccess, PLSA_HANDLE phPolicy)
{
	DWORD					dwError;
	LSA_OBJECT_ATTRIBUTES	dwObjectAttributes;
	LSA_UNICODE_STRING		ServerString;
	PLSA_UNICODE_STRING		pServer = NULL;

	//
	// Always initialize the object attributes to all zeros.
	//
	ZeroMemory (&dwObjectAttributes, sizeof (dwObjectAttributes));
	if (lpszServer != NULL)
	{
		// Make LSA_UNICODE_STRING out of LPWSTR passed in
		InitLsaString (&ServerString, lpszServer);
		pServer = &ServerString;
	}

	// Attempt to open the policy for all access.
	dwError = LsaNtStatusToWinError (LsaOpenPolicy (pServer,
											&dwObjectAttributes,
											dwAccess,
											phPolicy));
	return dwError;
}

PSID
GetAccountSid (LSA_HANDLE hPolicy, LPWSTR lpszAccount)
{
	PLSA_TRANSLATED_SID			pTranslatedSid;
	PSID						pSid;
	PSID						pDomainSid;
	PLSA_REFERENCED_DOMAIN_LIST	pDomains;
	DWORD						dwError;
	LSA_UNICODE_STRING			AccountString;
	DWORD						dwNewSidLength;
	DWORD						dwSubAuthorityCount;

	// Convert the string to a LSA_UNICODE_STRING
	InitLsaString (&AccountString, lpszAccount);

	// Call the LSA to lookup the name
	dwError = LsaNtStatusToWinError (LsaLookupNames (hPolicy,
													1,
													&AccountString,
													&pDomains,
													&pTranslatedSid));
	if (dwError == ERROR_SUCCESS)
	{
	    // Build a SID from Domain Sid and Account Sid
    	pDomainSid = pDomains->Domains [pTranslatedSid->DomainIndex].Sid;

	    // Compare the length of the new SID.  This is the length required for the
    	// number of subauthorities in the domain sid plus one for the user SID
	    dwSubAuthorityCount = *GetSidSubAuthorityCount (pDomainSid);
    	dwNewSidLength = GetSidLengthRequired ( (UCHAR) (dwSubAuthorityCount + 1) );

	    pSid = GlobalAlloc (GPTR, dwNewSidLength);

    	if (pSid != NULL)
        {
    	    // Build the Sid by copying the domain SID and, increasing sub-
        	// authority count in the new sid by one and setting the last
	        // subauthority to be the relative id of the user.
        	CopySid (dwNewSidLength, pSid, pDomainSid);

	        *GetSidSubAuthorityCount (pSid) = (UCHAR) dwSubAuthorityCount + 1;
    	    *GetSidSubAuthority (pSid, dwSubAuthorityCount) = pTranslatedSid->RelativeId;
//            pSid = NULL;
        }
	    LsaFreeMemory (pDomains);
    	LsaFreeMemory (pTranslatedSid);
    }
    else
        pSid = NULL;

	return pSid;
}

DWORD
AddUserRightToAccount (LSA_HANDLE hPolicy, LPWSTR lpszAccount, LPWSTR lpszRight)
{
	DWORD				dwError;
	PSID				pAccountSid;
	LSA_UNICODE_STRING	UserRightString;

	pAccountSid = GetAccountSid (hPolicy, lpszAccount);
	if (pAccountSid != NULL)
	{

    	// Create a LSA_UNICODE_STRING for right name.
	    InitLsaString (&UserRightString, lpszRight);
    	dwError = LsaNtStatusToWinError (LsaAddAccountRights (hPolicy,
														pAccountSid,
														&UserRightString,
														1));
		if (dwError != ERROR_SUCCESS)
			DisplayError (dwError);

	    GlobalFree (pAccountSid);
	}
	return dwError;
}

DWORD
RemoveUserRightToAccount (LSA_HANDLE hPolicy, LPWSTR lpszAccount, LPWSTR lpszRight)
{
	DWORD				dwError;
	PSID				pAccountSid;
	LSA_UNICODE_STRING	UserRightString;

	pAccountSid = GetAccountSid (hPolicy, lpszAccount);
	if (pAccountSid != NULL)
    {
	    // Create a LSA_UNICODE_STRING for right name.
	    InitLsaString (&UserRightString, lpszRight);
	    dwError = LsaNtStatusToWinError (LsaRemoveAccountRights (hPolicy,
														pAccountSid,
														FALSE,
														&UserRightString,
														1));
		if (dwError != ERROR_SUCCESS)
			DisplayError (dwError);
    }

	GlobalFree (pAccountSid);
	return dwError;
}

BOOL
GrantPrivileges (LPTSTR lpszUser, LPTSTR lpszServer)
{
	DWORD		dwErr;
	LSA_HANDLE	hPolicy;
	TCHAR		szServer[MAX_PATH + 1];

	wsprintf (szServer, TEXT ("\\\\%s"), lpszServer);

	dwErr = OpenPolicy (szServer, POLICY_ALL_ACCESS, &hPolicy);
	if (dwErr == ERROR_SUCCESS)
	{
		dwErr = AddUserRightToAccount (hPolicy,
								lpszUser,
								SE_TCB_NAME);
		dwErr = AddUserRightToAccount (hPolicy,
								lpszUser,
								SE_CREATE_TOKEN_NAME);
		dwErr = AddUserRightToAccount (hPolicy,
								lpszUser,
								SE_INCREASE_QUOTA_NAME);
		dwErr = AddUserRightToAccount (hPolicy,
								lpszUser,
								SE_BATCH_LOGON_NAME);
		dwErr = AddUserRightToAccount (hPolicy,
								lpszUser,
								SE_ASSIGNPRIMARYTOKEN_NAME);
		LsaClose (hPolicy);
	}
	else
		DisplayError (dwErr);

	return (dwErr == ERROR_SUCCESS);
}


BOOL
RevokePrivileges (LPTSTR lpszUser, LPTSTR lpszServer)
{
	DWORD		dwErr;
	LSA_HANDLE	hPolicy;

	dwErr = OpenPolicy (lpszServer, POLICY_ALL_ACCESS, &hPolicy);
	if (dwErr == ERROR_SUCCESS)
	{
		dwErr = RemoveUserRightToAccount (hPolicy,
								lpszUser,
								SE_TCB_NAME);
		dwErr = RemoveUserRightToAccount (hPolicy,
								lpszUser,
								SE_CREATE_TOKEN_NAME);
		dwErr = RemoveUserRightToAccount (hPolicy,
								lpszUser,
								SE_INCREASE_QUOTA_NAME);
		dwErr = RemoveUserRightToAccount (hPolicy,
								lpszUser,
								SE_BATCH_LOGON_NAME);
		dwErr = RemoveUserRightToAccount (hPolicy,
								lpszUser,
								SE_ASSIGNPRIMARYTOKEN_NAME);
	}

	LsaClose (hPolicy);
	return (dwErr == ERROR_SUCCESS);
}

