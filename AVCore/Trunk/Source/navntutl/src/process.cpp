////////////////////////////////////////////////////////////////////////////
//
// Process.cpp - Routines to return specific information about a running process
//
////////////////////////////////////////////////////////////////////////////
#include "windows.h"
#include "aclapi.h"
#include "navntutl.h"
#include "tchar.h"


////////////////////////////////////////////////////////////////////////////
// Function name    : ProcessIsInUserContext
//
// Description      : Check to see if this process is a normal user. We
//                    get an access token and go through the list of groups
//                    to see if it belongs to DOMAIN_ALIAS_RID_USERS
//
//                    The process is considered in user context if it is a
//                    a member of DOMAIN_ALIAS_RID_USERS.
//
// Return type		: DWORD - Standard error codes
//
// Argument         : ULONG * lpbUserContext - Pointer to a BOOL 
//
////////////////////////////////////////////////////////////////////////////
// 9/17/2002 TCASHIN/EALEX(Updated) - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
DWORD ProcessIsInUserContext( ULONG *  lpbUserContext )
{ 
    DWORD   dwRetValue      = ERROR_SUCCESS;
    DWORD   dwSizeNeeded    = 0;
    HANDLE  hCurrentProcess = NULL;
    HANDLE  hCurrProcToken  = NULL;
    int     i;

    TCHAR   szBuffer1[MAX_PATH] = {0};
    TCHAR   szBuffer2[MAX_PATH] = {0};
    DWORD   dwBuffer1Size       = sizeof(szBuffer1);
    DWORD   dwBuffer2Size       = sizeof(szBuffer2);

    SID_NAME_USE                sidType;
    TOKEN_INFORMATION_CLASS     stTokenInfo     = TokenGroups;
    PTOKEN_GROUPS               lpTokenGroups   = NULL;
    SID_IDENTIFIER_AUTHORITY    SIDAuth         = SECURITY_NT_AUTHORITY;
    BYTE sidBuffer[100];
    PSID lpSID = (PSID)&sidBuffer;


    //
    // See if we have valid parameters
    //
    if ( !lpbUserContext )
    {
        dwRetValue = ERROR_INVALID_PARAMETER;
        goto All_Done;
    }

    //
    // Get the handle of the current process
    //
    hCurrentProcess = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId() );
    if ( !hCurrentProcess )
    {
        dwRetValue = GetLastError();
        goto All_Done;
    }

    //
    // Get a handle for the access token for the current process
    //
    if ( !OpenProcessToken( hCurrentProcess, TOKEN_QUERY, &hCurrProcToken) )
    {
        dwRetValue = GetLastError();
        goto Clean_Up;
    }

    //
    // Call to get the size of the buffer we'll need 
    //
    GetTokenInformation(hCurrProcToken, stTokenInfo, NULL, 0, &dwSizeNeeded);

    //
    // Allocate a buffer for the security descriptor
    //
    lpTokenGroups = (TOKEN_GROUPS *) GlobalAlloc (GPTR, dwSizeNeeded);
    if ( !lpTokenGroups )
    {
        dwRetValue = GetLastError();
        goto Clean_Up;
    }

    //
    // Get the list of groups from the token
    //
    if ( !GetTokenInformation(hCurrProcToken, stTokenInfo, lpTokenGroups, dwSizeNeeded, &dwSizeNeeded) )
    {
        dwRetValue = GetLastError();
        goto Clean_Up;
    }

    //
    // Create a SID for the Users group.
    //
    if(! AllocateAndInitializeSid( &SIDAuth, 1,
                                   SECURITY_INTERACTIVE_RID,
								   0, 0, 0, 0, 0, 0, 0,
                                   &lpSID) ) 
    {
        dwRetValue = GetLastError();
        goto Clean_Up;
    }

    //
    // Set the default to a non-user context
    //
    *lpbUserContext = FALSE;

    //
    // Loop through the list of group SIDs
    //

    for ( i=0; i<(int)lpTokenGroups->GroupCount; i++ )
    {
        //
        // Reset the buffer sizes
        //
        dwBuffer1Size = sizeof(szBuffer1);
        dwBuffer2Size = sizeof(szBuffer2);

        //
        // Get the name and domain from the groups SID
        //
        if ( LookupAccountSid( NULL, 
                                lpTokenGroups->Groups[i].Sid, 
                                szBuffer1, 
                                &dwBuffer1Size, 
                                szBuffer2, 
                                &dwBuffer1Size, 
                                &sidType) )
        {
            //
            // If the current SID is the user and it is enabled,
            // then we're good to go.
            //

            if ( EqualSid( lpSID, lpTokenGroups->Groups[i].Sid) && 
                 (lpTokenGroups->Groups[i].Attributes & SE_GROUP_ENABLED) )
            {
                *lpbUserContext = TRUE;

            }
        }
    }

Clean_Up:

    //
    // Free the memory we allocated for the token info
    //
    if( lpTokenGroups && GlobalFree(lpTokenGroups) )
    {
        dwRetValue = GetLastError();
    }

    //
    // Close the handle for the process token
    //
    if ( hCurrProcToken )
    {
        CloseHandle( hCurrProcToken );
        hCurrProcToken = NULL;
    }

    //
    // Close the handle for the current process
    //
    if ( hCurrentProcess )
    {
        CloseHandle( hCurrentProcess );
        hCurrentProcess = NULL;
    }

    //
    // Free the SID we allocated
    //
    if ( lpSID )
        FreeSid( lpSID );

All_Done:

    return dwRetValue;

}


////////////////////////////////////////////////////////////////////////////
// Function name    : ProcessIsNormalUser
//
// Description      : Check to see if this process is a normal user. We
//                    get an access token and go through the list of groups
//                    to see if it belongs to Adminstrators/BUILTINS
//
//                    The process is considered a normal user if it is NOT
//                    a member of Adminstrators/BUILTINS.
//
// Return type		: DWORD - Standard error codes
//
// Argument         : ULONG * lpbNormalUser - Pointer to a BOOL 
//
////////////////////////////////////////////////////////////////////////////
// 2/1/2000 TCASHIN - Function created / Header added.
// 2/8/2003 TCASHIN - Port fix to STS defect #355757 )accessing the 
//                    Primary Domain Controller unnecessarily)
////////////////////////////////////////////////////////////////////////////
DWORD ProcessIsNormalUser( ULONG *  lpbNormalUser )
{ 
    BOOL    bIsMember       = FALSE;
    DWORD   dwRetValue      = ERROR_SUCCESS;
    DWORD   dwSizeNeeded    = 0;
    HANDLE  hCurrentProcess = NULL;
    HANDLE  hCurrProcToken  = NULL;
    int     i;

    TOKEN_INFORMATION_CLASS     stTokenInfo     = TokenGroups;
    PTOKEN_GROUPS               lpTokenGroups   = NULL;
    SID_IDENTIFIER_AUTHORITY    SIDAuth         = SECURITY_NT_AUTHORITY;
    BYTE sidBuffer[100];
    PSID lpSID = (PSID)&sidBuffer;


    //
    // See if we have valid parameters
    //
    if ( !lpbNormalUser )
    {
        dwRetValue = ERROR_INVALID_PARAMETER;
        goto All_Done;
    }

    //
    // Get the handle of the current process
    //
    hCurrentProcess = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId() );
    if ( !hCurrentProcess )
    {
        dwRetValue = GetLastError();
        goto All_Done;
    }

    //
    // Get a handle for the access token for the current process
    //
    if ( !OpenProcessToken( hCurrentProcess, TOKEN_QUERY, &hCurrProcToken) )
    {
        dwRetValue = GetLastError();
        goto Clean_Up;
    }

    //
    // Call to get the size of the buffer we'll need 
    //
    GetTokenInformation(hCurrProcToken, stTokenInfo, NULL, 0, &dwSizeNeeded);

    //
    // Allocate a buffer for the security descriptor
    //
    lpTokenGroups = (TOKEN_GROUPS *) GlobalAlloc (GPTR, dwSizeNeeded);
    if ( !lpTokenGroups )
    {
        dwRetValue = GetLastError();
        goto Clean_Up;
    }

    //
    // Get the list of groups from the token
    //
    if ( !GetTokenInformation(hCurrProcToken, stTokenInfo, lpTokenGroups, dwSizeNeeded, &dwSizeNeeded) )
    {
        dwRetValue = GetLastError();
        goto Clean_Up;
    }

    //
    // Create a SID for the BUILTIN\Administrators group.
    //
    if(! AllocateAndInitializeSid( &SIDAuth, 2,
                                   SECURITY_BUILTIN_DOMAIN_RID,
                                   DOMAIN_ALIAS_RID_ADMINS,
                                   0, 0, 0, 0, 0, 0,
                                   &lpSID) ) 
    {
        dwRetValue = GetLastError();
        goto Clean_Up;
    }

    //
    // Set the default to a normal user
    //
    *lpbNormalUser = TRUE;

    //
    // Loop through the list of group SIDs
    //
    for ( i=0; i<(int)lpTokenGroups->GroupCount; i++ )
    {
        // If the current SID is the admins and it is enabled,
        // then we're good to go.

        if ( EqualSid( lpSID, lpTokenGroups->Groups[i].Sid) && 
            (lpTokenGroups->Groups[i].Attributes & SE_GROUP_ENABLED) )
        {
            *lpbNormalUser = FALSE;
            break;
        }
    }

Clean_Up:

    //
    // Free the memory we allocated for the token info
    //
    if( lpTokenGroups && GlobalFree(lpTokenGroups) )
    {
        dwRetValue = GetLastError();
    }
    
    //
    // Close the handle for the process token
    //
    if ( hCurrProcToken )
    {
        CloseHandle( hCurrProcToken );
        hCurrProcToken = NULL;
    }

    //
    // Close the handle for the current process
    //
    if ( hCurrentProcess )
    {
        CloseHandle( hCurrentProcess );
        hCurrentProcess = NULL;
    }

    //
    // Free the SID we allocated
    //
    if ( lpSID )
        FreeSid( lpSID );

All_Done:

    return dwRetValue;

}


////////////////////////////////////////////////////////////////////////////
// Function name    : ProcessCanUpdateRegKey
//
// Description      : Determines is the process can update the specified reg
//                    key. If the process handle is NULL then we use the 
//                    current process.
//
// Parameters:      : [in]  - HANDLE hProcessToken - handle of the process
//                            to check
//                    [in]  - HKEY hMainKey - Any valid HKEY
//
//                    [in]  - HKEY hMainKey - Any valid HKEY
//
//                    [out] - ULONG * lpbCanUpdate - Pointer to a BOOL indicating if
//                            the current user can write to the specified key
//
// Return type		: DWORD - Standard error codes
//
////////////////////////////////////////////////////////////////////////////
// 3/20/2000 TCASHIN - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
DWORD ProcessCanUpdateRegKey( HANDLE hProcessToken, HANDLE hMainKey, LPCTSTR lpszRegKey, ULONG * lpbCanUpdate)
{
    DWORD           dwRetValue      = ERROR_SUCCESS;
    DWORD           dwSizeNeeded    = 0;
    HKEY            hKey            = NULL;
    LONG            lResult         = 0;
    HANDLE          hCurrentProcess = NULL;
    HANDLE          hCurrProcToken  = NULL;
    HANDLE          hDummyToken     = NULL;

    PRIVILEGE_SET   PrivilegeSet;
    DWORD           dwPrivSetSize   = sizeof( PRIVILEGE_SET );
    BOOL            bAccessGranted  = FALSE;

    DWORD           dwAccessAllowed = 0;
    DWORD           dwAccessDesired = 0;

    GENERIC_MAPPING Generic         = { STANDARD_RIGHTS_READ,
                                        KEY_WRITE,
                                        STANDARD_RIGHTS_EXECUTE,
                                        STANDARD_RIGHTS_ALL };

    PSECURITY_DESCRIPTOR            lpSecurityDescriptor = NULL;


    //
    // See if we have valid parameters
    //
    if ( !hMainKey || !lpszRegKey || !lpbCanUpdate )
    {
        dwRetValue = ERROR_INVALID_PARAMETER;
        goto All_Done;
    }

    //
    // Was a handle for a process token passed in?
    //
    if ( hProcessToken )
    {
        //
        // Yep. Use it.
        //
        hCurrProcToken = hProcessToken;
    }
    else
    {
        //
        // No. Get the handle of the current process
        //
        hCurrentProcess = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId() );
        if ( !hCurrentProcess )
        {
            dwRetValue = GetLastError();
            goto All_Done;
        }

        //
        // Get a handle for the access token for the current process
        //
        if ( !OpenProcessToken( hCurrentProcess, TOKEN_DUPLICATE, &hCurrProcToken) )
        {
            dwRetValue = GetLastError();
            goto All_Done;
        }

    }

    // 
    // Duplicate the token process token
    //
    if ( !DuplicateToken(hCurrProcToken, SecurityImpersonation, &hDummyToken) )
    {
        dwRetValue = GetLastError();
        goto All_Done;
    } 

    //
    // Open the key main key
    //
    lResult = RegOpenKeyEx((HKEY)hMainKey, lpszRegKey, 0, KEY_READ, &hKey);

    if ( lResult != ERROR_SUCCESS )
    {
        dwRetValue = (DWORD)lResult;
        goto All_Done;
    }

    //
    // Check to see how much memory we'll need for the security info
    //
    lResult = RegGetKeySecurity( hKey,
                                 OWNER_SECURITY_INFORMATION |
                                 GROUP_SECURITY_INFORMATION |
                                 DACL_SECURITY_INFORMATION,
                                 NULL, 
                                 &dwSizeNeeded);

    if ( lResult != ERROR_INSUFFICIENT_BUFFER)
    {   
        // Some big problem came up ...
        dwRetValue = (DWORD)lResult;
        goto All_Done;
    }

    //
    // Grab a buffer
    //
    lpSecurityDescriptor =(SECURITY_DESCRIPTOR *) GlobalAlloc(GPTR, dwSizeNeeded);
    if ( !lpSecurityDescriptor )
    {
        dwRetValue = ERROR_NOT_ENOUGH_MEMORY;
        goto All_Done;
    }

    //
    // Get the security descriptor
    //
    lResult = RegGetKeySecurity( hKey,
                                 OWNER_SECURITY_INFORMATION |
                                 GROUP_SECURITY_INFORMATION |
                                 DACL_SECURITY_INFORMATION,
                                 lpSecurityDescriptor, 
                                 &dwSizeNeeded);
    if (lResult != ERROR_SUCCESS)
    {
        dwRetValue = (DWORD)lResult;
        goto All_Done;
    }    

    //
    // We want to see if we can set a value in this reg key
    //
    dwAccessDesired = KEY_SET_VALUE; 

    //
    // Check the client's access rights.
    //
    if( !AccessCheck( 
        lpSecurityDescriptor,// reg key security descriptor
        hDummyToken,         // access token
        dwAccessDesired,     // requested access rights
        &Generic,            // pointer to GENERIC_MAPPING
        &PrivilegeSet,       // receives privileges used in check
        &dwPrivSetSize,      // size of PrivilegeSet buffer
        &dwAccessAllowed,    // receives mask of allowed access rights
        &bAccessGranted ))   // receives results of access check
    {
        // If the call fails, set the error
        dwRetValue = GetLastError();
    }

All_Done:

    //
    // Are we able to get the access we want?
    //
    if ( (dwAccessAllowed & KEY_SET_VALUE) && bAccessGranted )
    {
        if ( lpbCanUpdate )
        {
            *lpbCanUpdate = TRUE;
        }
    }
    else
    {
        if ( lpbCanUpdate )
        {
            *lpbCanUpdate = FALSE;
        }
    }


    //
    // Close up shop ...
    //
    if ( hKey )
    {
        RegCloseKey( hKey );
        hKey = NULL;
    }

    //
    // Free the buffer for the security descriptor
    //
    if( lpSecurityDescriptor )
    {
        GlobalFree( lpSecurityDescriptor );
        lpSecurityDescriptor = NULL;
    }

    //
    // Close the handle for the current process
    //
    if ( hCurrentProcess )
    {
        CloseHandle( hCurrentProcess );
        hCurrentProcess = NULL;
    }

    //
    // Close the access token handle if it was not passed in.
    // If hProcessToken is NULL then we created one ourselves.
    //
    if ( !hProcessToken )
    {
        //
        // Make sure we have one to close
        //
        if ( hCurrProcToken )
        {
            CloseHandle( hCurrProcToken );
            hCurrProcToken = NULL;
        }
    }

    //
    // Close the dummy/duplicate token
    //
    if ( hDummyToken )
    {
        CloseHandle( hDummyToken );
        hDummyToken = NULL;
    }


    return dwRetValue;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: ProcessTakeFileOwnership
//
// Description  : Give ownership of a file to LocalSystem
//
// DALLEE 2003.02.18 - ***WARNING!*** As implemented, this function actually
//      removes the DACL from a file. If necessary, ownership of the file is
//      set to local system, but only as a means to remove the DACL.
//
// Return Values: Standard error codes
//
// Argument     : [in] LPTSTR lpFileName - File to change ownership
//
///////////////////////////////////////////////////////////////////////////////
// 6/2/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD ProcessTakeFileOwnership(LPTSTR lpFileName) 
{ 

    HANDLE  hToken = NULL; 
    BYTE    sidBuffer[100]  = {0};
    PSID    pSID = (PSID)&sidBuffer;
    DWORD   dwRetValue = ERROR_SUCCESS;

    SID_IDENTIFIER_AUTHORITY SIDAuth = SECURITY_NT_AUTHORITY;


    // Try to remove the object's DACL.

    dwRetValue = SetNamedSecurityInfo ( lpFileName,  SE_FILE_OBJECT, 
                                    DACL_SECURITY_INFORMATION, NULL, NULL, NULL, NULL); 

    if ( dwRetValue == ERROR_SUCCESS )
        return TRUE; 
    else if (dwRetValue != ERROR_ACCESS_DENIED) 
    {
        // We can deal with access denied. For any other errors we're done.
        goto Cleanup;
    }

    // If the preceding call failed because access was denied, 
    // enable the SE_TAKE_OWNERSHIP_NAME privilege, create a SID for 
    // the Administrators group, take ownership of the object, and 
    // disable the privilege. Then try again to set the object's DACL.

    // Open a handle to the access token for the calling process.

    if ( !OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) )
    {
        goto Cleanup;
    } 

    // Enable the SE_TAKE_OWNERSHIP_NAME privilege.
 
    if ( ERROR_SUCCESS != ProcessSetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, TRUE) ) 
    {
        goto Cleanup;
    }

    // Create a SID for the Local System.

    if( !AllocateAndInitializeSid( &SIDAuth, 2, 
                                   SECURITY_BUILTIN_DOMAIN_RID,
                                   SECURITY_LOCAL_SYSTEM_RID,
                                   0, 0, 0, 0, 0, 0, &pSID) ) 
    {
        goto Cleanup;
    }

    // Set the owner in the object's security descriptor.

    dwRetValue = SetNamedSecurityInfo ( lpFileName, SE_FILE_OBJECT, 
                                   OWNER_SECURITY_INFORMATION, pSID, NULL, NULL, NULL); 

    if (pSID)
    {
        FreeSid(pSID); 
        pSID = NULL;
    }

    if (dwRetValue != ERROR_SUCCESS) 
    {
        goto Cleanup;
    }

    // Disable the SE_TAKE_OWNERSHIP_NAME privilege.
 
    if ( ERROR_SUCCESS != ProcessSetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, FALSE) ) 
    {
        goto Cleanup;
    }

    // Try again to remove the object's DACL, now that we are the owner.

    dwRetValue = SetNamedSecurityInfo( lpFileName, SE_FILE_OBJECT, 
                                  DACL_SECURITY_INFORMATION, NULL, NULL, NULL, NULL);

Cleanup:

    if ( hToken )
    {
        CloseHandle( hToken );
        hToken = NULL;
    }

    return dwRetValue;

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: SetPrivilege
//
// Description  : Set the specified privilege 
//
// Return Values: Standard error codes
//
// Argument     : [in] HANDLE hToken - Process token to change privilege for
// Argument     : [in] LPCTSTR lpszPrivilege - String specifying the privilege
// Argument     : [in] BOOL bEnablePriv - TRUE if the privilege should be enabled.
//
///////////////////////////////////////////////////////////////////////////////
// 6/10/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD ProcessSetPrivilege ( HANDLE hToken, LPCTSTR lpszPrivilege, USHORT bEnablePriv ) 
{
    TOKEN_PRIVILEGES    sTokenPrivs = {0};
    LUID                luid = {0};
    DWORD               dwRetValue = ERROR_SUCCESS;


    if ( !hToken || !lpszPrivilege )
    {
        dwRetValue = ERROR_INVALID_PARAMETER;
    }

    if ( !LookupPrivilegeValue(NULL, lpszPrivilege, &luid) )
    {
        dwRetValue = GetLastError(); 
    }
    else
    {
        sTokenPrivs.PrivilegeCount = 1;
        sTokenPrivs.Privileges[0].Luid = luid;

        if (bEnablePriv)
            sTokenPrivs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        else
            sTokenPrivs.Privileges[0].Attributes = 0;

        // Enable the privilege or disable all privileges.

        if ( !AdjustTokenPrivileges( hToken, FALSE, &sTokenPrivs, sizeof(TOKEN_PRIVILEGES), 
                                     (PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL) )
        {
            dwRetValue = GetLastError(); 
        } 
    }

    return dwRetValue;

}



