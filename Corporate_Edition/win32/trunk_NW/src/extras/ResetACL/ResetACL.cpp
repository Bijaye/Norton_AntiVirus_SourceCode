// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ResetACL.cpp : Defines the entry point for the ResetACL console application.
//
// This program revokes default access rights to the LDVP registry keys for the Everyone group
// and replaces them with read only rights. This program is only designed to run on NT 4.0
// where by default Everyone has write access to keys created in the LOCAL_MACHINE hive.
//

#include "stdafx.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <aclapi.h>

#include "CLogger.h"
#include "SymSaferStrings.h"

TCHAR main_reg_key[] = _T("SOFTWARE\\Intel\\LANDesk\\VirusProtect6\\CurrentVersion");

#define MAX_ERR_OUTPUT 1024
TCHAR err_output[MAX_ERR_OUTPUT+1];

CLogger *theLog = NULL;

#ifdef _DEBUG
    #define DebugOutputAclEntries( x )  OutputAclEntries( x )
    void OutputAclEntries( PACL pAcl );
#else
    #define DebugOutputAclEntries( x )
#endif


//===========================================================================

BOOL IsWinNT4(void)
{
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&os);
    return (BOOL)((os.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
		(os.dwMajorVersion == 4) &&
		(os.dwMinorVersion == 0));
}

//===========================================================================
// This function limits the access allowed for the Everyone group to KEY_READ
// for the specified key.
//
// Implementation notes:
// This routine iterates through the ACEs in the current DACL.
// For any access allowed ACE for the Everyone group, the permissions granted are
// masked with KEY_READ.
// 
// This routine previously used SetEntriesInAcl() to modify the ACL. However,
// SetEntriesInAcl() has unexpected/undocumented behavior -- Existing ACEs marked
// as inherit only in the ACL are nuked regardless of TRUSTEE. Also, when adding
// an entry with the CONTAINER_INHERIT_ACE flag, existing entries for the same TRUSTEE
// without inheritance flags were left unmodified.

long UpdateRegKeyACL( TCHAR *keyname, HKEY hKey )
{
    SID_IDENTIFIER_AUTHORITY    sidAuthorityWorld = SECURITY_WORLD_SID_AUTHORITY;
    PSID    pSidEveryone;

    ACCESS_ALLOWED_ACE  *pAce;
    SECURITY_DESCRIPTOR *pSecurity;
    PACL    pDacl;

    int     nAceIndex;
    long    err;

    pSidEveryone = NULL;

    pSecurity = NULL;
    pDacl     = NULL;

	theLog->LogStr(keyname);

    // Retrieve key's security descriptor and DACL
    err = GetSecurityInfo( hKey, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION, NULL, NULL, &pDacl, NULL, (PSECURITY_DESCRIPTOR *)&pSecurity );

    if ( ERROR_SUCCESS != err )
        {
        theLog->LogStr( _T("ERROR: GetSecurityInfo") );
        goto getout;
        }

    DebugOutputAclEntries( pDacl );

    // Get SID for well-known group Everyone
    if ( FALSE == AllocateAndInitializeSid( &sidAuthorityWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pSidEveryone ) )
        {
        err = GetLastError();

        theLog->LogStr( _T("ERROR: AllocateAndInitializeSid") );
        goto getout;
        }

    // Examine all ACEs. Limit access allowed to the Everyone group to KEY_READ
    for ( nAceIndex = 0; nAceIndex < pDacl->AceCount; ++nAceIndex )
        {
        if ( FALSE != GetAce( pDacl, nAceIndex, (LPVOID *)&pAce ) )
            {
            if ( ( ACCESS_ALLOWED_ACE_TYPE == pAce->Header.AceType ) &&
                 EqualSid( pSidEveryone, &pAce->SidStart ) )
                {
                pAce->Mask &= KEY_READ;

                // If we modified this ACE so that it now grants no permissions,
                // remove it since it is no longer valid.
                if ( 0 == pAce->Mask )
                    {
                    if ( FALSE != DeleteAce( pDacl, nAceIndex ) )
                        {
                        // Removed ACE, so backtrack one.
                        // Note: pDacl->AceCount has been updated for us.
                        --nAceIndex;
                        }
                    }
                }
            }
        }

    // check the new ACL

    if ( !IsValidAcl( pDacl ) )
        {
        err = ERROR_INVALID_ACL;

        theLog->LogStr( _T("ERROR: invalid new pDacl") );
        goto getout;
        }

    DebugOutputAclEntries( pDacl );

    err = SetSecurityInfo( hKey, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION, NULL, NULL, pDacl, NULL );

    if ( ERROR_SUCCESS != err )
        {
        theLog->LogStr( _T("ERROR: SetSecurityInfo") );
        goto getout;
        }

getout:
    if ( NULL != pSidEveryone )
        FreeSid( pSidEveryone );

    if ( NULL != pSecurity )
        LocalFree( pSecurity );

    // pDacl -- no need to free, merely references data inside pSecurity

    return ( err );
} // UpdateRegKeyACL()


//===========================================================================
// This function calls UpdateRegKeyACL to reset the access rights to all
// subkeys of the specified key.
//
// Notes:
// ONLY subkeys are updated, the ACL of the specified key is not changed.

long ProcessSubKeys(TCHAR *current_path, HKEY current_hkey)
{
	DWORD index = 0;
	TCHAR new_path[MAX_PATH + 1] = _T("");
	TCHAR keyname[MAX_PATH + 1] = _T("");

	HKEY hkey = NULL;
	long werr = 0;
	long err = 0;

	while ((werr = RegEnumKey(current_hkey, index++, keyname, MAX_PATH + 1)) == ERROR_SUCCESS)
		{
		_tcscpy(new_path, current_path);
		_tcscat(new_path, _T("\\"));
		_tcscat(new_path, keyname);

		if ((err = RegOpenKeyEx(current_hkey, keyname, 0, KEY_ALL_ACCESS, &hkey)) == ERROR_SUCCESS)
			{
			if ((err = UpdateRegKeyACL(new_path, hkey)) == ERROR_SUCCESS)
				{
				if (err = ProcessSubKeys(new_path, hkey) != ERROR_SUCCESS)
					break;
				}
			else
				{
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, err_output, MAX_ERR_OUTPUT, NULL);
				theLog->LogStr(_T("ERROR: updating ACL for reg key:"));
				theLog->LogStr(new_path);
				theLog->LogStr(err_output);
				}

			RegCloseKey(hkey);
			hkey = NULL;
			}
		else
			{
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, err_output, MAX_ERR_OUTPUT, NULL);
			theLog->LogStr(_T("ERROR: opening reg key:"));
			theLog->LogStr(new_path);
			theLog->LogStr(err_output);
			}
		}

	if (((werr != ERROR_SUCCESS)) && (werr != ERROR_NO_MORE_ITEMS))
		{
		err = werr;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, err_output, MAX_ERR_OUTPUT, NULL);
		theLog->LogStr(_T("ERROR: enumerating reg key:"));
		theLog->LogStr(current_path);
		theLog->LogStr(err_output);
		}

	return err;
}

//===========================================================================

int main(int argc, char * argv[])
{
	HKEY hkey = NULL;
	long err = 0;

	theLog = new CLogger(_T("ResetACL.log"));

	theLog->LogStr(_T("============================= Start ResetACL.log ============================="));

	if (!IsWinNT4())
		{
		err = ERROR_APP_WRONG_OS;
		theLog->LogStr(_T("ERROR: this tool only runs on WinNT v4"));
		}
	else if ((err = RegOpenKeyEx(HKEY_LOCAL_MACHINE, main_reg_key, 0, KEY_ALL_ACCESS, &hkey)) == ERROR_SUCCESS)
		{
		if ((err = UpdateRegKeyACL(main_reg_key, hkey)) == ERROR_SUCCESS)
			err = ProcessSubKeys(main_reg_key, hkey);
		else
			{
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, err_output, MAX_ERR_OUTPUT, NULL);
			theLog->LogStr(_T("ERROR: updating ACL for reg key:"));
			theLog->LogStr(main_reg_key);
			theLog->LogStr(err_output);
			}

		RegCloseKey(hkey);
		hkey = NULL;
		}
	else
		{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, err_output, MAX_ERR_OUTPUT, NULL);
		theLog->LogStr(_T("ERROR: opening reg key:"));
		theLog->LogStr(main_reg_key);
		theLog->LogStr(err_output);
		}

	theLog->LogStr(_T("============================== End ResetACL.log =============================="));

	delete theLog;

	return err;
}

// Debugging functions
#ifdef _DEBUG

// This routine outputs and logs the ACE's of the specified ACL.
//
void OutputAclEntries( PACL pAcl )
{
    ULONG   uEntry;
    ULONG   uNumEntries;
    PEXPLICIT_ACCESS    pEA;

    TCHAR   szAccountName [ 128 ];
    TCHAR   szDomainName [ 128 ];
    DWORD   cbAccountName;
    DWORD   cbDomainName;
    SID_NAME_USE eNameUse;

    LPCTSTR pszAccessMode;
    TCHAR   szAceInfo [ 1024 ];
    
    uNumEntries = 0;
    pEA = NULL;

	theLog->LogStr( _T("ACL Access Control Entries:") );

    if ( ERROR_SUCCESS == GetExplicitEntriesFromAcl( pAcl, &uNumEntries, &pEA ) )
        {
        for ( uEntry = 0; uEntry < uNumEntries; ++uEntry )
            {
            if ( TRUSTEE_IS_NAME == pEA[uEntry].Trustee.TrusteeForm )
                {
                _tcscpy( szAceInfo, pEA[uEntry].Trustee.ptstrName );
                }
            else
                {
                szAccountName[0] = _T('\0');
                szDomainName[0]  = _T('\0');
                cbAccountName = sizeof ( szAccountName ) / sizeof ( *szAccountName );
                cbDomainName  = sizeof ( szDomainName ) / sizeof ( *szDomainName );
                
                LookupAccountSid( NULL, 
                                  (PSID)pEA[uEntry].Trustee.ptstrName,
                                  szAccountName,
                                  &cbAccountName,
                                  szDomainName,
                                  &cbDomainName,
                                  &eNameUse );

                sssnprintf( szAceInfo, sizeof(szAceInfo), _T("%s/%s"), szDomainName, szAccountName );
                }

            switch ( pEA[uEntry].grfAccessMode )
                {
            default:
            case NOT_USED_ACCESS:
                pszAccessMode = _T("NOT_USED_ACCESS");
                break;
            case GRANT_ACCESS:
                pszAccessMode = _T("GRANT_ACCESS");
                break;
            case SET_ACCESS:
                pszAccessMode = _T("SET_ACCESS");
                break;
            case DENY_ACCESS:
                pszAccessMode = _T("DENY_ACCESS");
                break;
            case REVOKE_ACCESS:
                pszAccessMode = _T("REVOKE_ACCESS");
                break;
            case SET_AUDIT_SUCCESS:
                pszAccessMode = _T("SET_AUDIT_SUCCESS");
                break;
            case SET_AUDIT_FAILURE:
                pszAccessMode = _T("SET_AUDIT_FAILURE");
                break;
                }

            _tcscat( szAceInfo, _T(", ") );
            _tcscat( szAceInfo, pszAccessMode );

            _tcscat( szAceInfo, _T(", ") );

            int i;

            for ( i = 0; i < 32; i++ )
                {
                if ( i && ( 0 == i % 8 ) )
                    {
                    _tcscat( szAceInfo, _T(":") );
                    }

                if ( ( 1 << (31 - i) ) & pEA[uEntry].grfAccessPermissions )
                    {
                    _tcscat( szAceInfo, _T("1") );
                    }
                else
                    {
                    _tcscat( szAceInfo, _T("0") );
                    }
                }

            for ( i = 0; i < 32; i ++ )
                {
                switch ( ( 1 << i ) & pEA[uEntry].grfInheritance )
                    {
                case CONTAINER_INHERIT_ACE:
                    _tcscat( szAceInfo, _T(", ") );
                    _tcscat( szAceInfo, _T("CONTAINER_INHERIT_ACE") );
                    break;
                case INHERIT_ONLY_ACE:
                    _tcscat( szAceInfo, _T(", ") );
                    _tcscat( szAceInfo, _T("INHERIT_ONLY_ACE") );
                    break;
                case NO_PROPAGATE_INHERIT_ACE:
                    _tcscat( szAceInfo, _T(", ") );
                    _tcscat( szAceInfo, _T("NO_PROPAGATE_INHERIT_ACE") );
                    break;
                case OBJECT_INHERIT_ACE:
                    _tcscat( szAceInfo, _T(", ") );
                    _tcscat( szAceInfo, _T("OBJECT_INHERIT_ACE") );
                    break;
                case SUB_CONTAINERS_AND_OBJECTS_INHERIT:
                    _tcscat( szAceInfo, _T(", ") );
                    _tcscat( szAceInfo, _T("SUB_CONTAINERS_AND_OBJECTS_INHERIT") );
                    break;
                case 0:
                    // Flag not set.
                    break;
                default:
                    sssnprintfappend( szAceInfo, sizeof( szAceInfo ), _T(", INHERITANCE %d"), i );
                    break;
                    }
                }

    		theLog->LogStr( szAceInfo );
            }

        LocalFree( pEA );
        }
}

#endif // _DEBUG

/* end source file */
