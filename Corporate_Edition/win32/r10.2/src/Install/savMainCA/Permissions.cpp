// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"
#include "Permissions.h"

//
// Notes: This function should be run in defered mode in system context.
//			it should not be called untill after the standard action WriteRegistryValues
extern UINT __stdcall SetFolderPermissions( MSIHANDLE hInstall )
{
	UINT nRet = ERROR_SUCCESS;

	try
	{
		CString strData = GetMSIProperty( hInstall, "CustomActionData" );
		if( strData.GetLength() == 0 )
			throw( _T("No CustomActionData present.") );

		// Parse the Quarantine folder out
		int nPos = strData.Find( ';' );
		if( nPos < 0 )
			throw( _T("CustomActionData formatted incorrectly.") );
		CString strQuar = strData.Left( nPos );
		strData = strData.Right( (strData.GetLength() - nPos) - 1 );

		// Parse the APTemp folder out
		nPos = strData.Find( ';' );
		if( nPos < 0 )
			throw( _T("CustomActionData formatted incorrectly.") );
		CString strAPTemp = strData.Left( nPos );
		strData = strData.Right( (strData.GetLength() - nPos) - 1 );

		// Parse the I2LDVP folder out
		nPos = strData.Find( ';' );
		if( nPos < 0 )
			throw( _T("CustomActionData formatted incorrectly.") );
		CString strI2LDVP = strData.Left( nPos );

		// Parse the Logs folder out
		CString strLogs = strData.Right( (strData.GetLength() - nPos) - 1 );


		OutputDebugString( strQuar );
		OutputDebugString( strAPTemp );
		OutputDebugString( strI2LDVP );
		OutputDebugString( strLogs );

		if( ERROR_SUCCESS != SetDirectoryAccessAttributes( strQuar.LockBuffer() ) )
			MSILogMessage( hInstall, _T("Error setting Quarantine folder access rights.") );
		strQuar.ReleaseBuffer();
		
		if( ERROR_SUCCESS != SetDirectoryAccessAttributes( strAPTemp.LockBuffer() ) )
			MSILogMessage( hInstall, _T("Error setting APTemp folder access rights.") );
		strAPTemp.ReleaseBuffer();

		if( ERROR_SUCCESS != SetDirectoryAccessAttributes( strI2LDVP.LockBuffer() ) )
			MSILogMessage( hInstall, _T("Error setting I2_LDVP folder access rights.") );
		strI2LDVP.ReleaseBuffer();

		if( ERROR_SUCCESS != SetDirectoryAccessAttributes( strLogs.LockBuffer() ) )
			MSILogMessage( hInstall, _T("Error setting Logs folder access rights.") );
		strLogs.ReleaseBuffer();
	}
	catch( TCHAR *szErr )
	{
		MSILogMessage( hInstall, szErr );
	}

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: SetDirRestAccessAttributes
//
// Set the security attributes for the Everyone group to read and exectue access SYSTEM and
// BUILTIN\Administrators to full access
//
//////////////////////////////////////////////////////////////////////////
DWORD __stdcall SetDirectoryRestrictedAccessAttributes(LPTSTR lpszDirectory)
{
    DWORD               dwError = ERROR_SUCCESS;
    BOOL                bRet;
    BOOL                bIsNTFS = FALSE;
    SECURITY_DESCRIPTOR absSecurityDesc;
    PSID                pEveryoneGrpSID = NULL;
	PSID                pAdminSID = NULL;
	PSID                pSystemSID = NULL;
    PACL                pNewACL = NULL;
    DWORD               newACLSize;
	LPVOID				pACE;

    SID_IDENTIFIER_AUTHORITY IdentifierAuthority = SECURITY_WORLD_SID_AUTHORITY;  //Everyone group
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;

	 // See if this is an NTFS volume
    VolumeIsNTFS( lpszDirectory, &bIsNTFS );

    if ( !bIsNTFS )
    {
        // This isn't NTFS. Skip everything and say we completed ok.
		OutputDebugString( "Not NTFS volume." );
        goto All_Done;
    }

    // mmendon  10-25-2000:  Fix for German/French NT AP start problem
    //                       Getting the SID for the everyone group using #defines
    //                       and the API to allocate memory and init the SID.
    bRet = AllocateAndInitializeSid(&IdentifierAuthority, (BYTE)1,SECURITY_WORLD_RID,0,0,0,0,0,0,0, &pEveryoneGrpSID);

    // Errors?
    if ( !bRet )
    {
		OutputDebugString( "Error getting SID." );
        dwError = GetLastError();
        pEveryoneGrpSID = NULL;
		goto All_Done;
    }

	// Create a SID for the BUILTIN\Administrators group.
	if(! AllocateAndInitializeSid( &SIDAuthNT, (BYTE)2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdminSID) )
	{
		OutputDebugString( "Error getting SID." );
		dwError = GetLastError();
        pAdminSID = NULL;
		goto All_Done;
	}

	// Create a SID for the SYSTEM group.
	if(! AllocateAndInitializeSid( &SIDAuthNT, (BYTE)1, SECURITY_LOCAL_SYSTEM_RID , 0, 0, 0, 0, 0, 0, 0, &pSystemSID) )
	{
		OutputDebugString( "Error getting SID." );
		dwError = GetLastError();
        pAdminSID = NULL;
		goto All_Done;
	}

    // Initialize the absolute security descriptor
    bRet = InitializeSecurityDescriptor( &absSecurityDesc, SECURITY_DESCRIPTOR_REVISION );

    if ( !bRet )
    {
		OutputDebugString( "Error initializing descriptor." );
        dwError = GetLastError();
		goto All_Done;
    }

    // compute size of new DACL
	if(IsValidSid(pEveryoneGrpSID) && (IsValidSid(pAdminSID)) && (IsValidSid(pSystemSID)) )
	{
		// tmm: why 6 extra bytes?  I don't know... it works...
	    //newACLSize = sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(pEveryoneGrpSID) + GetLengthSid(pAdminSID) + GetLengthSid(pSystemSID) + 6 + 6 + 6 + 6;
		DWORD cbAce = sizeof (ACCESS_ALLOWED_ACE) - sizeof (DWORD);
        cbAce += GetLengthSid (pEveryoneGrpSID);
		cbAce += sizeof (ACCESS_ALLOWED_ACE) - sizeof (DWORD);
		cbAce += GetLengthSid (pAdminSID);
		cbAce += sizeof (ACCESS_ALLOWED_ACE) - sizeof (DWORD);
		cbAce += GetLengthSid (pSystemSID);
		newACLSize = sizeof (ACL);
		newACLSize += cbAce;
	}
	else
    {
		OutputDebugString( "Error validating SID." );
		dwError = ERROR_INVALID_SID;
		goto All_Done;
	}

    // allocate memory
    pNewACL = (PACL) GlobalAlloc( GPTR, newACLSize );

    if ( !pNewACL )
    {
        dwError = ERROR_NOT_ENOUGH_MEMORY;
		goto All_Done;
    }

    // initialize the new DACL
    bRet = InitializeAcl( pNewACL, newACLSize, ACL_REVISION );
    if ( !bRet )
    {
		OutputDebugString( "Error initializing ACL." );
        dwError = GetLastError();
		goto All_Done;
    }

    //  replace the DACL

	// Add an entry for the Admin group
    bRet = AddAccessAllowedAce( pNewACL, ACL_REVISION, FILE_ALL_ACCESS, pAdminSID );
    if ( !bRet )
    {
        dwError = GetLastError();
		goto All_Done;
    }

	// Add an entry for the System group
    bRet = AddAccessAllowedAce( pNewACL, ACL_REVISION, FILE_ALL_ACCESS, pSystemSID );
    if ( !bRet )
    {
        dwError = GetLastError();
		goto All_Done;
    }

    // Add an entry for the Everyone group
    bRet = AddAccessAllowedAce( pNewACL, ACL_REVISION, FILE_GENERIC_READ|FILE_GENERIC_EXECUTE, pEveryoneGrpSID );
    if ( !bRet )
    {
        dwError = GetLastError();
		goto All_Done;
    }

	// get the first Admin ACE in the list
	if (GetAce(pNewACL, 0, &pACE))
	{
		// and set the ACE so that child folders/objects will inherit this ACE
		((ACCESS_ALLOWED_ACE*)pACE)->Header.AceFlags = CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;
	}
	else
    {
	    dwError = ERROR_INVALID_ACL;
		goto All_Done;
	}

	// get the second System ACE in the list
	if (GetAce(pNewACL, 1, &pACE))
	{
		// and set the ACE so that child folders/objects will inherit this ACE
		((ACCESS_ALLOWED_ACE*)pACE)->Header.AceFlags = CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;
	}
	else
    {
	    dwError = ERROR_INVALID_ACL;
		goto All_Done;
	}

	// get the third Everyone ACE in the list
	if (GetAce(pNewACL, 2, &pACE))
	{
		// and set the ACE so that child folders/objects will inherit this ACE
		((ACCESS_ALLOWED_ACE*)pACE)->Header.AceFlags = CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;
	}
	else
    {
	    dwError = ERROR_INVALID_ACL;
		goto All_Done;
	}

    // set the new DACL in the absolute SD
    bRet = SetSecurityDescriptorDacl( &absSecurityDesc, TRUE, pNewACL, FALSE );

    if ( !bRet )
    {
        dwError = GetLastError();
		goto All_Done;
    }

    // Did we build the new security descriptor correctly?
    bRet = IsValidSecurityDescriptor( &absSecurityDesc );

    if ( !bRet )
    {
        dwError = GetLastError();
		goto All_Done;
    }

    // Finally set the new DACL for the directory
    SetFileSecurity(lpszDirectory, DACL_SECURITY_INFORMATION, &absSecurityDesc);

All_Done:
    // Free the new ACL buffer
    if ( pNewACL )
    {
        GlobalFree( pNewACL );
    }

    // Free the Everyone group SID buffer
    if ( pEveryoneGrpSID )
    {
        FreeSid( pEveryoneGrpSID );
    }

	// Free the Admin group SID buffer
    if ( pAdminSID )
    {
        FreeSid( pAdminSID );
    }

	// Free the System group SID buffer
    if ( pSystemSID )
    {
        FreeSid( pSystemSID );
    }

    return dwError;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: SetDirectoryAccessAttributes
//
// Set the security attributes for the Everyone group to full access
//
//////////////////////////////////////////////////////////////////////////
DWORD __stdcall SetDirectoryAccessAttributes(LPTSTR lpszDirectory)
{
    DWORD               dwError = ERROR_SUCCESS;
    BOOL                bRet;
    BOOL                bIsNTFS = FALSE;
    SECURITY_DESCRIPTOR absSecurityDesc;
    PSID                pEveryoneGrpSID = NULL;
    PACL                pNewACL = NULL;
    DWORD               newACLSize;
	LPVOID				pACE;
    // mmendon  10-25-2000:  Fix for German/French NT AP start problem
    SID_IDENTIFIER_AUTHORITY IdentifierAuthority = SECURITY_WORLD_SID_AUTHORITY;  //Everyone group
    // mmendon  10-25-2000:  End fix for German/French NT AP start problem


    // See if this is an NTFS volume
    VolumeIsNTFS( lpszDirectory, &bIsNTFS );

    if ( !bIsNTFS )
    {
        // This isn't NTFS. Skip everything and say we completed ok.
        goto All_Done;
    }

    // mmendon  10-25-2000:  Fix for German/French NT AP start problem
    //                       Getting the SID for the everyone group using #defines
    //                       and the API to allocate memory and init the SID.
    bRet = AllocateAndInitializeSid(&IdentifierAuthority, (BYTE)1,SECURITY_WORLD_RID,0,0,0,0,0,0,0, &pEveryoneGrpSID);

    // Errors?
    if ( !bRet )
    {
        dwError = GetLastError();
        pEveryoneGrpSID = NULL;
        goto All_Done;
    }
    // mmendon  10-25-2000:  End fix for German/French NT AP start problem

    // Initialize the absolute security descriptor
    bRet = InitializeSecurityDescriptor( &absSecurityDesc, SECURITY_DESCRIPTOR_REVISION );

    if ( !bRet )
    {
        dwError = GetLastError();
        goto All_Done;
    }

    // compute size of new DACL
	if(IsValidSid(pEveryoneGrpSID))
		// tmm: why 6 extra bytes?  I don't know... it works...
	    newACLSize = sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(pEveryoneGrpSID) + 6;
	else
	    {
	    dwError = ERROR_INVALID_SID;
		goto All_Done;
		}

    // allocate memory
    pNewACL = (PACL) GlobalAlloc( GPTR, newACLSize );

    if ( !pNewACL )
    {
        dwError = ERROR_NOT_ENOUGH_MEMORY;
        goto All_Done;
    }

    // initialize the new DACL
    bRet = InitializeAcl( pNewACL, newACLSize, ACL_REVISION );
    if ( !bRet )
    {
        dwError = GetLastError();
        goto All_Done;
    }

    //  replace the DACL
    // Add an entry for the Everyone group
    bRet = AddAccessAllowedAce( pNewACL, ACL_REVISION, FILE_ALL_ACCESS, pEveryoneGrpSID );
    if ( !bRet )
    {
        dwError = GetLastError();
        goto All_Done;
    }

	// get the first (and only) ACE in the list
	if (GetAce(pNewACL, 0, &pACE))
		{
		// and set the ACE so that child folders/objects will inherit this ACE
		((ACCESS_ALLOWED_ACE*)pACE)->Header.AceFlags = CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;
		}
	else
	    {
	    dwError = ERROR_INVALID_ACL;
		goto All_Done;
		}

    // set the new DACL in the absolute SD
    bRet = SetSecurityDescriptorDacl( &absSecurityDesc, TRUE, pNewACL, FALSE );

    if ( !bRet )
    {
        dwError = GetLastError();
        goto All_Done;
    }

    // Did we build the new security descriptor correctly?
    bRet = IsValidSecurityDescriptor( &absSecurityDesc );

    if ( !bRet )
    {
        dwError = GetLastError();
        goto All_Done;
    }

    // Finally set the new DACL for the directory
    SetFileSecurity(lpszDirectory, DACL_SECURITY_INFORMATION, &absSecurityDesc);

All_Done:

    // Free the new ACL buffer
    if ( pNewACL )
    {
        GlobalFree( pNewACL );
    }

    // Free the Everyone group SID buffer
    if ( pEveryoneGrpSID )
    {
        // mmendon  10-25-2000:  Fix for German/French NT AP start problem
        FreeSid( pEveryoneGrpSID );
        // mmendon  10-25-2000:  End fix for German/French NT AP start problem
    }

    return dwError;
}

DWORD VolumeIsNTFS(LPTSTR lpPath, LPBOOL lpbIsNTFS)
{
    DWORD   dwError = FALSE; // Assuming the worst is better than random stack data!
    LPTSTR  lpRoot = NULL;
    TCHAR   szRootDir[MAX_PATH] = {0};
    TCHAR   szVolumeName[MAX_PATH] = {0};
    DWORD   dwVolumeSerialNumber = 0;
    DWORD   dwMaxComponentLength = 0;
    DWORD   dwFileSystemFlags = 0;
    TCHAR   szFileSystemName[MAX_PATH] = {0};


    // Copy the path/file name to our temp buffer
    vpstrncpy (szRootDir, lpPath, sizeof (szRootDir));

    // Get the drive letter from the string
    lpRoot = _tcschr( szRootDir, '\\' );

    // Do we have a drive letter delimiter?
    if ( lpRoot )
    {
        // Yep. Terminate the string
        lpRoot = CharNext( lpRoot );
        *lpRoot = '\0';

        // Get the Volume info for this drive letter
        if ( GetVolumeInformation( szRootDir,                  // root directory
                                   szVolumeName,               // volume name buffer
                                   sizeof(szVolumeName),       // length of name buffer
                                   &dwVolumeSerialNumber,      // volume serial number
                                   &dwMaxComponentLength,      // maximum file name length
                                   &dwFileSystemFlags,         // file system options
                                   szFileSystemName,           // file system name buffer
                                   sizeof(szFileSystemName)) ) // length of file system name buffer

        {
            // See if the volume info says we're NTFS
            if ( _tcsstr(szFileSystemName, _T("NTFS")) )
                *lpbIsNTFS = TRUE;
            else
                *lpbIsNTFS = FALSE;

            dwError = ERROR_SUCCESS;
        }
        else
        {
            dwError = GetLastError();
        }
    }
    else
    {
        dwError = GetLastError();
    }

    return dwError;
}
