// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef NLM
    #include <stdio.h>
    #include "winThreads.h"
#endif
#include "pscan.h"
#include "SecureCommsMisc.h"

#ifdef WIN32
    #include <msi.h>
    #include <msiquery.h>
#endif
#include <string>

#ifdef UNICODE
#define tstring std::w_string
#else
#define tstring std::string
#endif

// Creates all PKI store directories in baseDirectory (normally main program directory)
HRESULT CreatePKIdirectories( LPCTSTR baseDirectory );
ScsSecureComms::RESULT CreateRootCertificates( LPCTSTR pkiBaseDirectory );
UINT CreateServerPKI( bool login, bool use_root_auth, LPCTSTR strPkiBasePath, LPCTSTR strPrimaryServerName, LPCTSTR strUserName, LPCTSTR strPassword );
// Sets the HLM\SW\Intel\LANdesk\VirusProtect6\CurrentVersion\DomainGUID entry
HRESULT CreateDomainGuid( LPCTSTR primaryServerName );
// Creates the initial admin account.  Password is the already-encrypted password.
// Returns S_OK on success, else the error code of the failure.
DWORD CreateInitialAccount( LPCTSTR username, LPCTSTR password );

void NukeDir( LPCTSTR lpszPath, BOOL fRecurse );
BOOL Find_FQDN( LPTSTR pszName, LPTSTR pszFQDN );

void SAVSecureCAdprintfInterface( LPCTSTR format, ... );
