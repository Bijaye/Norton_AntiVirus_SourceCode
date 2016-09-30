// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

// Evil hack (#include and externs)
#include "winBaseTypes.h"
#include "nlm.h"
#include <errno.h>
#include <sys/stat.h>
#include "nwreg.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"
#include "UserAccounts.h"

#define INVALID_FILE_HANDLE_VALUE (-1)

#ifdef SYM_LIBC_NLM
#error "Not safe for libc builds"
#endif // SYM_LIBC_NLM

extern "C"
{
    BOOL RemoveDirectory( LPCTSTR directoryName );
    DWORD GetCurrentDirectory( DWORD size, LPSTR path );
    BOOL CopyFile( LPCTSTR ExistingFile, LPCTSTR NewFile, BOOL FailIfExists );
    BOOL MoveFile( LPCTSTR lpszExisting, LPCTSTR lpszNew);
    void MakeWriteable (const char *file,DWORD mask);
    int DeletePurgeFile (const char *file);
}

#ifdef NLM
    #define NETDB_USE_INTERNET
	 
	 #include <stdio.h>
    #include "winThreads.h"
    #include "winStrings.h"
    #include "winFunctions.h"
    #include "nlm/nwfinfo.h"
    #include <socket.h>
    #include <netdb.h>
    #include <nwnamspc.h>
    #include <nwfileio.h>

    typedef struct hostent HOSTENT;
    typedef struct hostent *PHOSTENT;
    typedef struct hostent *LPHOSTENT;

    #include <nwconio.h>
#else
    #include "winsock2.h"
#endif
#include "ScsCommsDefs.h"
#include "ScsCommsUtils.h"
#include "CertIssueDefaults.h"
#include "IAuth.h"
#include "CertVector.h"
#include "nwdebug.h"
#include "SavSecureCA.h"
#include "password.h"
#include "SCSSecureLogin.h"
#include <stdio.h>
#include "slash.h"

#ifdef CreateDirectory
    #undef CreateDirectory
#endif

//#define array_sizeof(x) (sizeof(x) / sizeof(x[0]))
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define GET_SERVER_GROUP_GUID_BYTE_SIZE     33

#ifdef NLM
    #define dprintf SAVSecureCAdprintfInterface
#else
    #define dprintf MsiGlobalLogMessage
#endif

#ifdef NLM
	#define WIN32_CLIENT_INSTALL_DIR  "Clt-inst" sSLASH "WIN32"
#endif

#ifdef NLM
    // All of the following in this block should be moved to the compatibility layer
    // Evil hack
    // Define this for CLIB clients
    #ifndef P_tmpdir
        #define P_tmpdir "sys:/tmp"
    #endif

    BOOL GetComputerName(
      LPTSTR lpBuffer,
      LPDWORD lpnSize
    )
    {
        // Validate parameters
        if (lpnSize == NULL)
            return FALSE;
        if (lpBuffer == NULL)
            return FALSE;
        if (*lpnSize < IMAX_PATH)
            return FALSE;

        NTSGetComputerName(lpBuffer, NULL);
        return TRUE;
    }

    DWORD GetTempPath(
      DWORD nBufferLength,
      LPTSTR lpBuffer
    )
    {
        if (lpBuffer == NULL)
            return 0;
        if (nBufferLength >= (_tcsclen(P_tmpdir)+1))
        {
            _tcsncpy(lpBuffer, P_tmpdir, nBufferLength);
            return _tcsclen(P_tmpdir);
        }
        else
        {
            return 0;
        }
    }
#endif

BOOL Find_FQDN( LPTSTR pszName, LPTSTR pszFQDN );
ScsSecureComms::RESULT IssueAndSaveSelfSignedCert( LPCTSTR szComName, LPCTSTR szLocalHostName, ScsSecureComms::IAuth::CERT_TYPE eCertType, ScsSecureComms::ICert *&pNewCert, LPCTSTR pkiBaseDirectory );
ScsSecureComms::RESULT IssueSelfSignedCert( LPCTSTR szComName, LPCTSTR szLocalHostName, ScsSecureComms::IAuth::CERT_TYPE eCertType, ScsSecureComms::ICertSigningRequest** ppNewSignReq, ScsSecureComms::ICert*& pNewCert );
ScsSecureComms::RESULT IssueLocalChainedCert( LPCTSTR szComName, LPCTSTR szLocalHostName, ScsSecureComms::IAuth::CERT_TYPE eCertType, LPCTSTR pkiBaseDirectory );
ScsSecureComms::RESULT SetupCaCertChain( ScsSecureComms::IAuth::CERT_TYPE eCertType, std::vector<ScsSecureComms::ICertPtr>& vecCertChain, LPCTSTR pkiBaseDirectory );
ScsSecureComms::RESULT IssueCert( LPCTSTR szUserName, ScsSecureComms::IRoleVector* pRoles, ScsSecureComms::IAuth::CERT_TYPE eCertType, ScsSecureComms::ICertSigningRequest* pCertSigningReq, ScsSecureComms::ICertVector*& ivecNewCertChain, LPCTSTR pkiBaseDirectory );
ScsSecureComms::RESULT UpdateIssuer( HKEY hScsCommsKey, LPCTSTR szUserName, ScsSecureComms::IRoleVector* pRoles, ScsSecureComms::IAuth::CERT_TYPE eCertType, std::vector<ScsSecureComms::ICertPtr>* pvecCertChain, ScsSecureComms::ICertIssuer* pIssuer);
ScsSecureComms::RESULT GetAndIncrementSerialLastNum( HKEY hScsCommsKey, ScsSecureComms::IAuth::CERT_TYPE  eCertType, DWORD* pnSerialNum );
DWORD GetServerGroupGuid( LPTSTR szGuid, int nNumBytes );
DWORD CheckRolesOfLoginCert(ScsSecureComms::IAuth *&pAuth, BOOL *pFullAccess);
BOOL CreatePKITargetPaths( LPCTSTR strPrimaryServerName,
                            LPTSTR strCertFilePath,
                            LPTSTR strPrivateKeyFilePath,
                            LPTSTR strRootCertFilePath,
                            LPTSTR strCSRFilePath,
                            LPTSTR szName,
                            LPTSTR szServerGroupGuid,
                            LPTSTR szCert,
                            LPTSTR szKey,
                            LPTSTR szRootCert,
                            LPTSTR szCSR );

ScsSecureComms::RESULT GetAndIncrementSerialLastNum( HKEY hScsCommsKey, ScsSecureComms::IAuth::CERT_TYPE  eCertType, DWORD* pnSerialNum )
{
    HKEY hTempKey = NULL;
    TCHAR message[ MAX_PATH] = {0};

    switch (eCertType)
    {
        case ScsSecureComms::IAuth::SERVER_EE_CERT_TYPE:
        case ScsSecureComms::IAuth::LOGIN_CA_CERT_TYPE:
            if( RegCreateKeyEx (hScsCommsKey, szReg_Key_ServerGroupData, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hTempKey, NULL ) != ERROR_SUCCESS )
                return ScsSecureComms::RTNERR_FAILED;
            break;

        case ScsSecureComms::IAuth::LOGIN_EE_CERT_TYPE:
            // Login end-entity certs -- the serial number must go in a LocalData hive
            // under the caller's hive, do that it doesn't get moved around to other servers
            // if we should be involved in a primary server promote/demote operation.
            if( RegCreateKeyEx (hScsCommsKey, szReg_Key_LocalData, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hTempKey, NULL ) == ERROR_SUCCESS )
                return ScsSecureComms::RTNERR_FAILED;
            break;

        default:
            dprintf("GetAndIncrementSerialLastNum:  Error issuing new cert. Unknown type: %d", eCertType);
            return ScsSecureComms::RTNERR_INVALIDARG;
            break;
    }

    DWORD size = sizeof( DWORD );
    if( ERROR_SUCCESS != SymSaferRegQueryValueEx( hTempKey, szReg_Val_ServerGroupCaIssueSerialNum, 0, NULL, (LPBYTE)*pnSerialNum, &size ) )
        *pnSerialNum = 0;
    ++(*pnSerialNum);
    if( ERROR_SUCCESS != RegSetValueEx( hTempKey, szReg_Val_ServerGroupCaIssueSerialNum, 0, REG_DWORD, (LPBYTE)pnSerialNum, sizeof( DWORD ) ) )
    {
        dprintf(_T("Unable to update serial number in registry"));
        return ScsSecureComms::RTNERR_FAILED;
    }

    if( hTempKey )
        RegCloseKey( hTempKey );

    dprintf("GetAndIncrementSerialLastNum:  Increment serial num: %lu", *pnSerialNum);

    return ScsSecureComms::RTN_OK;
}

ScsSecureComms::RESULT UpdateIssuer( HKEY hScsCommsKey, LPCTSTR szUserName, ScsSecureComms::IRoleVector* pRoles, ScsSecureComms::IAuth::CERT_TYPE eCertType, std::vector<ScsSecureComms::ICertPtr>* pvecCertChain, ScsSecureComms::ICertIssuer* pIssuer)
{
    ScsSecureComms::RESULT      returnValSCSSR      = ScsSecureComms::RTN_OK;
    TCHAR                       message[MAX_PATH]   = {""};

    // We must have a valid issuer object.
    SAVASSERT (pIssuer);

    if (pvecCertChain)
    {
        ScsSecureComms::ICertPtr ptrCaCert (pvecCertChain->at (0));
        returnValSCSSR = pIssuer->SetIssuingCA (ptrCaCert);
        if (SYM_FAILED (returnValSCSSR))
        {
            dprintf("UpdateIssuer:  Failed setting the issuing CA: 0x%08X", returnValSCSSR);
            return returnValSCSSR;
        }
    }

    int         nLifetime = ScsSecureComms::SERVER_GROUP_CA_DEFAULT_LIFETIME;
    int         nPreDate  = ScsSecureComms::SERVER_GROUP_CA_DEFAULT_PREDATE;
    int         nPathLen  = 0;
    int         nNumRoles = 0;
    bool        bIsCa     = false;
    bool        bUseInputRoleList = false;
    const char *roles[10] = {0};

    // specified data into the Cert Signing Request.
    switch (eCertType)
    {
        case ScsSecureComms::IAuth::SERVER_GROUP_CA_CERT_TYPE:
            nPathLen  = ScsSecureComms::SERVER_GROUP_CA_DEFAULT_PATH_LEN;
            bIsCa     = true;
            break;

        case ScsSecureComms::IAuth::SERVER_EE_CERT_TYPE:
            roles[nNumRoles++] = ScsSecureComms::BUILTIN_ROLE_SERVER;
            break;

        case ScsSecureComms::IAuth::LOGIN_CA_CERT_TYPE:
            nPathLen  = ScsSecureComms::LOGIN_CA_CERT_DEFAULT_PATH_LEN;
            bIsCa     = true;
            break;

        case ScsSecureComms::IAuth::LOGIN_EE_CERT_TYPE:
            bUseInputRoleList = true;
            break;

        default:
            dprintf("UpdateIssuer:  Error issuing new cert. Unknown type: %d", eCertType);
            return ScsSecureComms::RTNERR_INVALIDARG;
            break;
    }

    // Setup the expiration timing.
    returnValSCSSR = pIssuer->SetValidityPeriod (nLifetime);
    if (SYM_FAILED (returnValSCSSR))
    {
        dprintf("UpdateIssuer:  Failed setting the validity period: 0x%08X", returnValSCSSR);
        return returnValSCSSR;
    }

    // Set the pre dating.  This allows an Admin to accommodate time differences in his environment.
    returnValSCSSR = pIssuer->SetValidityPreDatePeriod (nPreDate);
    if (SYM_FAILED (returnValSCSSR))
    {
        dprintf("UpdateIssuer:  Failed setting the validity pre-date period: 0x%08X", returnValSCSSR);
        return returnValSCSSR;
    }

    // this is for the debug statement below
    LPCTSTR role = NULL;

    // Encode roles, which give privs.
    if (bUseInputRoleList)
    {
        nNumRoles = pRoles ? pRoles->size() : 0;
        if (pRoles && nNumRoles)
            role = pRoles->at(0);
        returnValSCSSR = pIssuer->SetRoles(pRoles);
    }
    else
    {
        returnValSCSSR = pIssuer->SetRoles(roles, nNumRoles);
        role = roles[0];
    }

    if (SYM_FAILED (returnValSCSSR))
    {
        dprintf("UpdateIssuer:  Failed setting the validity pre-date period: 0x%08X", returnValSCSSR);
        return returnValSCSSR;
    }

    // Set certificate basic constraints, CA and path length.
    returnValSCSSR = pIssuer->SetBasicConstraints (bIsCa, nPathLen);
    if (SYM_FAILED (returnValSCSSR))
    {
        dprintf("UpdateIssuer:  Failed setting the basic constraints: 0x%08X", returnValSCSSR);
        return returnValSCSSR;
    }

    dprintf("UpdateIssuer:  Issue cert params: [user: %s] type: %d, life: %d, predate: %d, ca: %d, pathlen: %d, roles: %d, first role: %s", szUserName != NULL ? szUserName : "<none>", eCertType, nLifetime, nPreDate, bIsCa, nPathLen, nNumRoles, (nNumRoles > 0 ? role : "<none>"));

    return ScsSecureComms::RTN_OK;
}

ScsSecureComms::RESULT SetupCaCertChain( ScsSecureComms::IAuth::CERT_TYPE eCertType, std::vector<ScsSecureComms::ICertPtr>& vecCertChain, LPCTSTR pkiBaseDirectory )
{
    ScsSecureComms::RESULT returnValSCSSR = ScsSecureComms::RTN_OK;

    vecCertChain.clear();

    // If we are issuing a Login End-Entity cert, add in the Login CA cert
    // into the front of the cert chain.
    if (eCertType == ScsSecureComms::IAuth::LOGIN_EE_CERT_TYPE)
    {
        char szComputerName[IMAX_PATH];
        Find_FQDN(szComputerName,NULL);

        ScsSecureComms::ICertPtr ptrCaCert;
        returnValSCSSR = GetNewestLoginCaCert(pkiBaseDirectory, szComputerName, NULL, true, ptrCaCert.m_p);
        if (returnValSCSSR == ScsSecureComms::RTNERR_FILE_NOT_FOUND)
        {
            dprintf(_T("SetupCaCertChain:  Failed to find the required login ca and private key"));
            return ScsSecureComms::RTNERR_FILE_NOT_FOUND;
        }
        else if (SYM_FAILED (returnValSCSSR))
        {
            dprintf("SetupCaCertChain:  Error loading login ca object: 0x%08X", returnValSCSSR);
            return returnValSCSSR;
        }

        vecCertChain.push_back (ptrCaCert);
    }

    // We will need the private key for this server group ca when we are issuing
    // certs off it.
    bool bRequireSvrGrpCaPrivateKey = (eCertType == ScsSecureComms::IAuth::SERVER_EE_CERT_TYPE ||
                                       eCertType == ScsSecureComms::IAuth::LOGIN_CA_CERT_TYPE);

    ScsSecureComms::ICertPtr ptrSrvGrpCaCert;
    returnValSCSSR = GetNewestServerGroupCaCert(pkiBaseDirectory, bRequireSvrGrpCaPrivateKey, NULL, ptrSrvGrpCaCert.m_p);

    if (bRequireSvrGrpCaPrivateKey && returnValSCSSR == ScsSecureComms::RTNERR_FILE_NOT_FOUND)
    {
        dprintf(_T("SetupCaCertChain:  No required server group ca and private key"));
        return ScsSecureComms::RTNERR_FILE_NOT_FOUND;
    }
    else if (returnValSCSSR == ScsSecureComms::RTNERR_FILE_NOT_FOUND)
    {
        // When the private key is not required, there is another CA actually
        // issuing the cert. The Server Group CA is provide for convenience.
        // Ignore failures for this case.
        dprintf(_T("SetupCaCertChain:  No server group ca certificate. We must be bootstrapping"));
    }
    else if (SYM_FAILED (returnValSCSSR))
    {
        dprintf("SetupCaCertChain:  Error loading server group root cert object: 0x%08X", returnValSCSSR);
        return returnValSCSSR;
    }
    else // success
    {
        vecCertChain.push_back(ptrSrvGrpCaCert);
    }

    return ScsSecureComms::RTN_OK;
}

ScsSecureComms::RESULT IssueCert( LPCTSTR szUserName, ScsSecureComms::IRoleVector* pRoles, ScsSecureComms::IAuth::CERT_TYPE eCertType, ScsSecureComms::ICertSigningRequest* pCertSigningReq, ScsSecureComms::ICertVector*& ivecNewCertChain, LPCTSTR pkiBaseDirectory )
{
    HKEY                        scsCommsKeyHandle   = NULL;
    ScsSecureComms::ICertIssuerPtr ptrIssuer;
    std::vector<ScsSecureComms::ICertPtr> vecCertChain;
    unsigned long               nSerialNum          = 0;
    ScsSecureComms::ICertPtr    ptrNewCert;
    DWORD                       returnValDW         = ERROR_FILE_NOT_FOUND;
    SYMRESULT                   returnValSR         = SYMERR_UNKNOWN;
    ScsSecureComms::RESULT      returnValSCSSR      = ScsSecureComms::RTNERR_FAILED;


    // We have now authenticated the credentials. Start creating the certificate.
    returnValDW = RegCreateKey(HKEY_LOCAL_MACHINE, REGHEADER _T("\\") szReg_Key_ScsComms, &scsCommsKeyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        returnValSR = g_objCertIssuerLdr.CreateObject(&ptrIssuer);
        if (SYM_SUCCEEDED(returnValSR))
        {
            // Load up the CA (if any) and root certificate. The new certificate
            // is added to the chain later. Position 0 in the vector contains the
            // issuing CA which may or may not also be the root.
            returnValSCSSR = SetupCaCertChain(eCertType, vecCertChain, pkiBaseDirectory);
            if (SYM_SUCCEEDED(returnValSCSSR))
            {
                // Update the Issuer with server specified information based on the
                // certificate type.
                returnValSCSSR = UpdateIssuer(scsCommsKeyHandle, szUserName, pRoles, eCertType, &vecCertChain, ptrIssuer);
                if (SYM_SUCCEEDED(returnValSCSSR))
                {
                    dprintf(_T("IssueCert:  Issuer updated"));

                    returnValSCSSR = GetAndIncrementSerialLastNum(scsCommsKeyHandle, eCertType, &nSerialNum);
                    if (SYM_SUCCEEDED(returnValSCSSR))
                    {
                        dprintf(_T("IssueCert:  Last serial number updated"));

                        returnValSCSSR = ptrIssuer->IssueCert(nSerialNum, pCertSigningReq, ptrNewCert.m_p);
                        if (SYM_SUCCEEDED(returnValSCSSR))
                        {
                            // Put the new end-entity cert at the front of the cert chain.
                            vecCertChain.insert(vecCertChain.begin(), ptrNewCert);

                            // Convert our STL cert vector to an ICertVector instance.
                            ivecNewCertChain = new CCertVector (vecCertChain);
                            ivecNewCertChain->AddRef();
                            dprintf(_T("IssueCert:  Certificate issued"));
                        }
                        else
                        {
                            dprintf("IssueCert:  Failed creating new cert: 0x%08X", returnValSCSSR);
                        }
                    }
                    else
                    {
                        dprintf("IssueCert:  GetAndIncrementSerialLastNum failed sym error 0x%08x.", returnValSCSSR);
                    }
                }
                else
                {
                    dprintf("IssueCert:  Error 0x%08x updating issuer.", returnValSCSSR);
                }
            }
            else
            {
                dprintf(_T("IssueCert:  CA cert chain set up"));
            }
        }
        else
        {
            dprintf("IssueCert:  Failed creating a cert issuer object: sym 0x%08X", returnValSR);
            returnValSCSSR = ScsSecureComms::RTNERR_FAILED;
        }

        RegCloseKey (scsCommsKeyHandle);
    }
    else
    {
        dprintf("IssueCert:  Error 0x%08x opening ScsComms key.", returnValSCSSR);
    }

    return returnValSCSSR;
}

ScsSecureComms::RESULT IssueLocalChainedCert( LPCTSTR szComName, LPCTSTR szLocalHostName, ScsSecureComms::IAuth::CERT_TYPE eCertType, LPCTSTR pkiBaseDirectory )
{
    ScsSecureComms::ICertSigningRequestPtr      ptrSignReq;
    ScsSecureComms::IKeyPtr                     ptrPrivateKey;
    ScsSecureComms::ICertVectorPtr              vecCertChain;
    ScsSecureComms::ICertPtr                    ptrNewCert;
    ScsSecureComms::RESULT      returnValSCSSR          = ScsSecureComms::RTNERR_FAILED;
    DWORD err = ERROR_SUCCESS;

    returnValSCSSR = GenerateCertSigningRequest(eCertType, "", szLocalHostName, ptrSignReq.m_p, ptrPrivateKey.m_p, NULL);
    if (SYM_SUCCEEDED(returnValSCSSR))
    {
        returnValSCSSR = IssueCert("", NULL, eCertType, ptrSignReq, vecCertChain.m_p, pkiBaseDirectory);
        if (SYM_SUCCEEDED(returnValSCSSR))
        {
            ptrNewCert.Attach (vecCertChain->at (0));
            returnValSCSSR = ptrNewCert->SetPrivateKey(ptrPrivateKey);
            if (SYM_SUCCEEDED(returnValSCSSR))
            {
                // Associated the private key with the cert.
                if (eCertType == ScsSecureComms::IAuth::SERVER_EE_CERT_TYPE)
                    returnValSCSSR = SaveServerEeCertChain(pkiBaseDirectory, szLocalHostName, NULL, vecCertChain);
                else if (eCertType == ScsSecureComms::IAuth::LOGIN_CA_CERT_TYPE)
                    returnValSCSSR = SaveLoginCaCertChain(pkiBaseDirectory, szLocalHostName, NULL, vecCertChain);
                else
                    returnValSCSSR = ScsSecureComms::RTNERR_INVALIDARG;

                if (SYM_FAILED(returnValSCSSR))
                    dprintf("IssueLocalChainedCert:  %s: Failed to save the issued cert chain: 0x%08X", szComName, returnValSCSSR );
            }
            else
            {
                dprintf("IssueLocalChainedCert:  %s: Failed set private key on newly issued cert: 0x%08X", szComName, returnValSCSSR);
            }
        }
        else
        {
            dprintf("IssueLocalChainedCert:  %s:  IssueCert returned error 0x%08x", szComName, returnValSCSSR);
        }
    }
    else
    {
        dprintf("IssueLocalChainedCert:  GenerateCertSigningRequest returned error 0x%08x", returnValSCSSR);
    }

    return returnValSCSSR;
}

ScsSecureComms::RESULT IssueSelfSignedCert( LPCTSTR szComName, LPCTSTR szLocalHostName, ScsSecureComms::IAuth::CERT_TYPE eCertType, ScsSecureComms::ICertSigningRequest** ppNewSignReq, ScsSecureComms::ICert*& pNewCert )
{
    HKEY                                        scsCommsKeyHandle       = NULL;
    ScsSecureComms::ICertSigningRequestPtr      ptrSignReq;
    ScsSecureComms::IKeyPtr                     ptrPrivateKey;
    ScsSecureComms::ICertIssuerPtr              ptrIssuer;
    ScsSecureComms::ICertPtr                    ptrNewCert;
    DWORD                                       returnValDW             = ERROR_FILE_NOT_FOUND;
    SYMRESULT                                   returnValSR             = SYMERR_UNKNOWN;
    ScsSecureComms::RESULT                      returnValSCSSR          = ScsSecureComms::RTNERR_FAILED;

    returnValDW = RegCreateKey(HKEY_LOCAL_MACHINE, REGHEADER _T("\\") szReg_Key_ScsComms, &scsCommsKeyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        returnValSCSSR = GenerateCertSigningRequest(eCertType, NULL, szLocalHostName, ptrSignReq.m_p, ptrPrivateKey.m_p, NULL);
        if (SYM_SUCCEEDED(returnValSCSSR))
        {
            returnValSR = g_objCertIssuerLdr.CreateObject(&ptrIssuer);
            if (SYM_SUCCEEDED(returnValSR))
            {
                returnValSCSSR = UpdateIssuer(scsCommsKeyHandle, "", NULL, eCertType, NULL, ptrIssuer);
                if (SYM_SUCCEEDED(returnValSCSSR))
                {
                    returnValSCSSR = ptrIssuer->IssueCertSelfSigned(0, ptrSignReq, ptrPrivateKey, ptrNewCert.m_p);
                    if (SYM_SUCCEEDED(returnValSCSSR))
                    {
                        returnValSCSSR = ptrNewCert->SetPrivateKey(ptrPrivateKey);
                        if (SYM_FAILED(returnValSCSSR))
                            dprintf("IssueSelfSignedCert:  %s: Failed setting %d private key: 0x%08x", szComName, (DWORD) eCertType, returnValSCSSR);
                    }
                    else
                    {
                        dprintf("IssueSelfSignedCert:  %s: Failed issuing %d cert: 0x%08x", szComName, (DWORD) eCertType, returnValSCSSR);
                    }
                }
                else
                {
                }
            }
            else
            {
                dprintf("IssueSelfSignedCert:  %s: Failed creating a %d cert issuer object: 0x%08x", szComName, (DWORD) eCertType, returnValSR);
                returnValSCSSR = ScsSecureComms::RTNERR_FAILED;
            }
        }
        else
        {
            dprintf("IssueSelfSignedCert:  %s: generate %d cert signing request failed: 0x%08x", szComName, (DWORD) eCertType, returnValSCSSR);
        }

        // Return result and shutdown
        if (SYM_SUCCEEDED(returnValSCSSR))
        {
            pNewCert = ptrNewCert;
            pNewCert->AddRef();
            if (ppNewSignReq)
            {
                *ppNewSignReq = ptrSignReq;
                (*ppNewSignReq)->AddRef();
            }
        }
        RegCloseKey(scsCommsKeyHandle);
        scsCommsKeyHandle = NULL;
    }
    else
    {
        dprintf("IssueSelfSignedCert:  Error 0x%08x opening ScsComms key.", returnValSCSSR);
    }

    return returnValSCSSR;
}

ScsSecureComms::RESULT IssueAndSaveSelfSignedCert( LPCTSTR szComName, LPCTSTR szLocalHostName, ScsSecureComms::IAuth::CERT_TYPE eCertType, ScsSecureComms::ICert *&pNewCert, LPCTSTR pkiBaseDirectory )
{
//------------------------------------------------------------------------
// Create a new Server Group CA certificate and private key; write them
// out to the hard drive.
//------------------------------------------------------------------------
    ScsSecureComms::RESULT      returnValSCSSR          = ScsSecureComms::RTN_OK;

    ScsSecureComms::ICertSigningRequestPtr ptrSignReq;
    returnValSCSSR = IssueSelfSignedCert(szComName, szLocalHostName, eCertType, &ptrSignReq, pNewCert);
    if (SYM_FAILED(returnValSCSSR))
        return returnValSCSSR;

    // Put the self-issued cert into a vector ... sometimes the save function
    // needs a vector of certs to save.
    CCertVector objCertVector;
    returnValSCSSR = objCertVector.push_back (pNewCert);
    if (SYM_FAILED(returnValSCSSR))
    {
        dprintf("%s: Failed converting self-issued %d cert to cert vector: 0x%08X", szComName, eCertType, returnValSCSSR);
        return returnValSCSSR;
    }

    switch (eCertType)
    {
        case ScsSecureComms::IAuth::SERVER_GROUP_CA_CERT_TYPE:
            // Write the new Server Group CA and private key to the hard drive.
            returnValSCSSR = SaveServerGroupCaCert(pkiBaseDirectory, NULL, pNewCert);
            if (SYM_FAILED(returnValSCSSR))
            {
                dprintf("IssueAndSaveSelfSignedCert:  %s: Failed saving new server group CA: 0x%08X", szComName, returnValSCSSR);
                return returnValSCSSR;
            }

            // We only save CSRs for the Server Group CA certs. This allows
            // administrator to re-issue the server group CA cert as a subordinate
            // CA from an enterprise cert chain.
            returnValSCSSR = SaveServerGroupCaCsr(pkiBaseDirectory, NULL, ptrSignReq);
            if (SYM_FAILED(returnValSCSSR))
            {
                dprintf("IssueAndSaveSelfSignedCert:  %s: Failed saving server group CA cert signing request: 0x%08X", szComName, returnValSCSSR);
                return returnValSCSSR;
            }
            break;

        case ScsSecureComms::IAuth::SERVER_EE_CERT_TYPE:
            returnValSCSSR = SaveServerEeCertChain(pkiBaseDirectory, szLocalHostName, NULL, &objCertVector);
            if (SYM_FAILED(returnValSCSSR))
            {
                dprintf("IssueAndSaveSelfSignedCert:  %s: Failed saving new server cert: 0x%08X", szComName, returnValSCSSR);
                return returnValSCSSR;
            }
            break;

        case ScsSecureComms::IAuth::LOGIN_CA_CERT_TYPE:
            returnValSCSSR = SaveLoginCaCertChain(pkiBaseDirectory, szLocalHostName, NULL, &objCertVector);
            if (SYM_FAILED(returnValSCSSR))
            {
                dprintf("IssueAndSaveSelfSignedCert:  %s: Failed saving new login CA cert: 0x%08X", szComName, returnValSCSSR);
                return returnValSCSSR;
            }
            break;

        default:
            dprintf("IssueAndSaveSelfSignedCert:  %s: Failed saving new cert, unknown cert type.", szComName);
            return ScsSecureComms::RTNERR_INVALIDARG;
    }

    return returnValSCSSR;
}

ScsSecureComms::RESULT CreateRootCertificates( LPCTSTR pkiBaseDirectory )
{
    LPTSTR                      szComName                       = NULL;
    TCHAR                       szComputerName[MAX_PATH]        = {""};
    ScsSecureComms::ICertPtr    pRootCert;
    ScsSecureComms::RESULT      returnValSCSSR                  = ScsSecureComms::RTNERR_FAILED;

    if (Find_FQDN(szComputerName, NULL))
    {
        // Issue the new Server Group CA and write it to disk.
        returnValSCSSR = IssueAndSaveSelfSignedCert(szComName, szComputerName, ScsSecureComms::IAuth::SERVER_GROUP_CA_CERT_TYPE, pRootCert, pkiBaseDirectory);
        if (SYM_SUCCEEDED(returnValSCSSR))
        {
            dprintf(_T("CreateRootCertificates:  Issued root certificate"));

            // Issue the Server End-Entity and write it to disk.
            returnValSCSSR = IssueLocalChainedCert(szComName, szComputerName, ScsSecureComms::IAuth::SERVER_EE_CERT_TYPE, pkiBaseDirectory);
            if (SYM_SUCCEEDED(returnValSCSSR))
            {
                dprintf(_T("CreateRootCertificates:  Issued Server End-Entity certificate"));

                // Issue the Login CA and write it to disk.
                returnValSCSSR = IssueLocalChainedCert(szComName, szComputerName, ScsSecureComms::IAuth::LOGIN_CA_CERT_TYPE, pkiBaseDirectory);
                if (SYM_SUCCEEDED(returnValSCSSR))
                    dprintf(_T("CreateRootCertificates:  Issued Login CA certificate"));
                else
                    dprintf(_T("CreateRootCertificates:  Unable to issue Login CA certificate, error sym 0x08%x"), returnValSCSSR);
            }
            else
            {
                dprintf(_T("CreateRootCertificates:  Unable to issue Server End-Entity certificate, error sym 0x08%x"), returnValSCSSR);
            }
        }
        else
        {
            dprintf(_T("CreateRootCertificates:  Unable to issue root certificate, error sym 0x08%x"), returnValSCSSR);
        }
    }
    else
    {
        dprintf("CreateRootCertificates:  Find_FQDN failed");
    }

    return returnValSCSSR;
}

UINT CreateServerPKI( bool login,
                       bool use_root_auth,
                       LPCTSTR strPkiBasePath,
                       LPCTSTR strPrimaryServerName,
                       LPCTSTR strUserName,
                       LPCTSTR strPassword )
{
    TCHAR   szName [MAX_PATH]                           ={0};
    TCHAR   szFQDN [MAX_PATH]                           ={0};
    TCHAR   szServerGroupGuid [MAX_PATH]                ={0};
    TCHAR   szMessString [MAX_PATH*4]                   ={0};
    TCHAR   strTemp[MAX_PATH*4]                         ={0};
    DWORD   nError                                      =S_OK;

    TCHAR   szCertPath [MAX_PATH]                       ={0};
    TCHAR   szKeyPath [MAX_PATH]                        ={0};
    TCHAR   szRootCertPath [MAX_PATH]                   ={0};
    TCHAR   szCSRPath [MAX_PATH]                        ={0};

    TCHAR   szCertName [MAX_PATH]                       ={0};
    TCHAR   szKeyName [MAX_PATH]                        ={0};
    TCHAR   szRootCertName [MAX_PATH]                   ={0};
    TCHAR   szCSRName [MAX_PATH]                        ={0};

    // Build base paths.
    _tcsncpy( szCertPath, strPkiBasePath, array_sizeof(szCertPath) );
    PathAddBackslash( szCertPath );
    _tcsncat( szCertPath, SYM_SCSCOMM_DIR_CER_CERTS, array_sizeof(szCertPath) - _tcslen(szCertPath) );
    szCertPath[ array_sizeof(szCertPath) - 1 ] = 0;

    _tcsncpy( szKeyPath, strPkiBasePath, array_sizeof(szKeyPath) );
    PathAddBackslash( szKeyPath );
    _tcsncat( szKeyPath, SYM_SCSCOMM_DIR_CER_PKEY, array_sizeof(szKeyPath) - _tcslen(szKeyPath) );
    szKeyPath[ array_sizeof(szKeyPath) - 1 ] = 0;

    _tcsncpy( szRootCertPath, strPkiBasePath, array_sizeof(szRootCertPath) );
    PathAddBackslash( szRootCertPath );
    _tcsncat( szRootCertPath, SYM_SCSCOMM_DIR_CER_ROOT, array_sizeof(szRootCertPath) - _tcslen(szRootCertPath) );
    szRootCertPath[ array_sizeof(szRootCertPath) - 1 ] = 0;

    _tcsncpy( szCSRPath, strPkiBasePath, array_sizeof(szCSRPath) );
    PathAddBackslash( szCSRPath );
    _tcsncat( szCSRPath, SYM_SCSCOMM_DIR_CER_CSR, array_sizeof(szCSRPath) - _tcslen(szCSRPath) );
    szCSRPath[ array_sizeof(szCSRPath) - 1 ] = 0;


    try
    {
        ScsSecureComms::ICertSigningRequestPtr  ptrCertSigningRequest;
        ScsSecureComms::IKeyPtr                 privateKey;
        ScsSecureComms::ICertIssuerPtr          ptrCertIssuer;
        ScsSecureComms::ICertPtr                ptrCert;
        ScsSecureComms::ICertVectorPtr          ptrCerts;
        LPCTSTR rolesServer[] =
        {
            ScsSecureComms::BUILTIN_ROLE_SERVER
        };
        SYMRESULT                               symres                  = SYM_OK;
        ScsSecureComms::RESULT                  nCommsRes               = ScsSecureComms::RTN_OK;

        // Get a CSR object.
        symres = g_objCertSigningRequestLdr.CreateObject( &ptrCertSigningRequest );
        if (SYM_FAILED (symres))
        {
            nError=symres;
            throw( _T("CreateServerPKI:  CreateObject() for ICertSigningRequestPtr failed:") );
        }

        // Create a cert issuer..
        symres = g_objCertIssuerLdr.CreateObject( &ptrCertIssuer );
        if (SYM_FAILED (symres))
        {
            nError=symres;
            throw( _T("CreateServerPKI:  CreateObject() for ICertIssuerPtr failed:") );
        }

        int ret = 0;

        // Get the Server's name and FQDN -- we need thois for both Server and Login CA certs.

        if (FALSE == Find_FQDN(szName, szFQDN))
        {
            throw( _T("CreateServerPKI:  Find_FQDN() failed:") );
        }

        if( login == false )
        {
            // Issue a server cert.

            // Server key pair.
            nCommsRes = ptrCertSigningRequest->CreateKeyPair( ScsSecureComms::SERVER_CERT_KEY_LEN, privateKey.m_p );
            if (SYM_FAILED (nCommsRes))
            {
                nError=nCommsRes;
                throw( _T("CreateServerPKI:  CreateKeyPair() on server csr failed:") );
            }

            // Create the CSR
            nCommsRes = ptrCertSigningRequest->Create( szName,
                                                       NULL,
                                                       ScsSecureComms::BUILTIN_CERTTYPE_SERVER,
                                                       szFQDN );
            if (SYM_FAILED (nCommsRes))
            {
                nError=nCommsRes;
                throw( _T("CreateServerPKI:  Create() on server csr failed:") );
            }

            // Set issuer params.
            nCommsRes = ptrCertIssuer->SetValidityPeriod( ScsSecureComms::SERVER_CERT_DEFAULT_LIFETIME );
            if (SYM_FAILED (nCommsRes))
            {
                nError=nCommsRes;
                throw( _T("ERROR: SetValidityPeriod() on server cert failed") );
            }
            nCommsRes = ptrCertIssuer->SetValidityPreDatePeriod( ScsSecureComms::SERVER_GROUP_CA_DEFAULT_PREDATE );
            if (SYM_FAILED (nCommsRes))
            {
                nError=nCommsRes;
                throw( _T("ERROR: SetValidityPreDatePeriod() on server cert failed") );
            }

            nCommsRes = ptrCertIssuer->SetRoles( rolesServer, array_sizeof(rolesServer) );
            if (SYM_FAILED (nCommsRes))
            {
                nError=nCommsRes;
                throw( _T("CreateServerPKI:  SetRoles() on server cert failed:") );
            }

            nCommsRes = ptrCertIssuer->SetBasicConstraints( false, 0 );
            if (SYM_FAILED (nCommsRes))
            {
                nError=nCommsRes;
                throw( _T("CreateServerPKI:  SetBasicConstraints() on server cert failed:") );
            }

            _tcsncpy( szCertName, SYM_SCSCOMM_SERVER_CERT,  array_sizeof(szCertName) );
            szCertName[ array_sizeof(szCertName) - 1 ] = 0;
            _tcsncat( szCertName, SYM_SCSCOMM_CER_EXT,      array_sizeof(szCertName) - _tcslen(szCertName) - 1 );
            szCertName[ array_sizeof(szCertName) - 1 ] = 0;

            _tcsncpy( szKeyName, SYM_SCSCOMM_SERVER_CERT,   array_sizeof(szKeyName) );
            szKeyName[ array_sizeof(szKeyName) - 1 ] = 0;
            _tcsncat( szKeyName, SYM_SCSCOMM_PRIV_KEY_EXT,  array_sizeof(szKeyName) - _tcslen(szKeyName) - 1 );
            szKeyName[ array_sizeof(szKeyName) - 1 ] = 0;

            _tcsncpy( szCSRName, SYM_SCSCOMM_SERVER_CERT,   array_sizeof(szCSRName) );
            szCSRName[ array_sizeof(szCSRName) - 1 ] = 0;
            _tcsncat( szCSRName, SYM_SCSCOMM_CERT_SIGNING_REQUEST_EXT, array_sizeof(szCSRName) - _tcslen(szCSRName) - 1 );
            szCSRName[ array_sizeof(szCSRName) - 1 ] = 0;
        }
        else
        {
            // Issue a Login CA.

            // Login CA key pair.

            nCommsRes = ptrCertSigningRequest->CreateKeyPair( ScsSecureComms::LOGIN_CA_CERT_KEY_LEN, privateKey.m_p );
            if (SYM_FAILED (nCommsRes))
            {
                nError=nCommsRes;
                throw( _T("CreateServerPKI:  CreateKeyPair() on login ca csr failed:") );
            }

            // Create the CSR
            nCommsRes = ptrCertSigningRequest->Create( szName,
                                                       NULL,
                                                       ScsSecureComms::BUILTIN_CERTTYPE_LOGINCA,
                                                       NULL );
            if (SYM_FAILED (nCommsRes))
            {
                nError=nCommsRes;
                throw( _T("CreateServerPKI:  Create() on login ca csr failed:") );
            }

            // Set issuer params.
            nCommsRes = ptrCertIssuer->SetValidityPeriod( ScsSecureComms::LOGIN_CA_CERT_DEFAULT_LIFETIME );
            if (SYM_FAILED (nCommsRes))
            {
                nError=nCommsRes;
                throw( _T("ERROR: SetValidityPeriod() on server cert failed") );
            }
            nCommsRes = ptrCertIssuer->SetValidityPreDatePeriod( ScsSecureComms::SERVER_GROUP_CA_DEFAULT_PREDATE );
            if (SYM_FAILED (nCommsRes))
            {
                nError=nCommsRes;
                throw( _T("ERROR: SetValidityPreDatePeriod() on server cert failed") );
            }

            nCommsRes = ptrCertIssuer->SetBasicConstraints( true, ScsSecureComms::LOGIN_CA_CERT_DEFAULT_PATH_LEN );
            if (SYM_FAILED (nCommsRes))
            {
                nError=nCommsRes;
                throw( _T("CreateServerPKI:  SetBasicConstraints() on login ca certfailed:") );
            }

            _tcsncpy( szCertName, SYM_SCSCOMM_LOGINCA_CERT, array_sizeof(szCertName) );
            szCertName[ array_sizeof(szCertName) - 1 ] = 0;
            _tcsncat( szCertName, SYM_SCSCOMM_CER_EXT,      array_sizeof(szCertName) - _tcslen(szCertName) - 1 );
            szCertName[ array_sizeof(szCertName) - 1 ] = 0;

            _tcsncpy( szKeyName, SYM_SCSCOMM_LOGINCA_CERT,  array_sizeof(szKeyName) );
            szKeyName[ array_sizeof(szKeyName) - 1 ] = 0;
            _tcsncat( szKeyName, SYM_SCSCOMM_PRIV_KEY_EXT,  array_sizeof(szKeyName) - _tcslen(szKeyName) - 1 );
            szKeyName[ array_sizeof(szKeyName) - 1 ] = 0;

            _tcsncpy( szCSRName, SYM_SCSCOMM_LOGINCA_CERT,  array_sizeof(szCSRName) );
            szCSRName[ array_sizeof(szCSRName) - 1 ] = 0;
            _tcsncat( szCSRName, SYM_SCSCOMM_CERT_SIGNING_REQUEST_EXT, array_sizeof(szCSRName) - _tcslen(szCSRName) - 1 );
            szCSRName[ array_sizeof(szCSRName) - 1 ] = 0;
        }

        _tcsncpy( szRootCertName, SYM_SCSCOMM_SERVER_GROUP_CA_CERT, array_sizeof(szRootCertName) );
        szRootCertName[ array_sizeof(szRootCertName) - 1 ] = 0;
        _tcsncat( szRootCertName, SYM_SCSCOMM_CER_EXT, array_sizeof(szRootCertName) - _tcslen(szRootCertName) - 1 );
        szRootCertName[ array_sizeof(szRootCertName) - 1 ] = 0;

        if (CreatePKITargetPaths( strPrimaryServerName, szCertPath, szKeyPath, szRootCertPath, szCSRPath, szName, szServerGroupGuid, szCertName, szKeyName, szRootCertName, szCSRName))
        {
            dprintf("CreateServerPKI:  Cert path: %s", szCertPath);
            dprintf("CreateServerPKI:  key path: %s", szKeyPath);
            dprintf("CreateServerPKI:  root cert path: %s", szRootCertPath);
            dprintf("CreateServerPKI:  csr path: %s", szCSRPath);
            dprintf("CreateServerPKI:  GUID:  %s", szServerGroupGuid);
        }
        else
        {
            throw(_T("CreateServerPKI:  CreatePKITargetPaths failed."));
        }

        // Send a request via Transman to the Primary Server to issue us a Server cert and Login CA cert.
        // If we don't have a Primary Server name to use, then we are installing as a stand-alone
        // Server, and we should creates self-signed keys.

        if( strPrimaryServerName == NULL || _tcslen( strPrimaryServerName ) == 0 )
        {
            nCommsRes = ptrCertIssuer->IssueCertSelfSigned( 0, ptrCertSigningRequest, privateKey, ptrCert.m_p );
            if (SYM_FAILED (nCommsRes))
            {
                nError=nCommsRes;
                throw( _T("CreateServerPKI:  IssueCertSelfSigned() failed:") );
            }

            // Match the cert and the key.
            dprintf("CreateServerPKI:  No Primary SetPrivateKey...");
            nCommsRes = ptrCert->SetPrivateKey( privateKey );
            if (SYM_FAILED(nCommsRes))
            {
                nError=nCommsRes;
                throw( _T("CreateServerPKI:  SetPrivateKey() after self-signed cert generation failed:") );
            }

            // Save the CSR (and Admin might need it for reissuing from an Enterprise root cert).
            nCommsRes = ptrCertSigningRequest->Save( szCSRPath );
            if (SYM_FAILED (nCommsRes))
            {
                nError=nCommsRes;
                throw( _T("CreateServerPKI:  CSR save failed:") );
            }
        }
        else
        {
            ScsSecureComms::IAuthPtr ptrAuth;

            // Get an IAuth object to represent the crenedtials we need for the remote connection.
            dprintf("CreateServerPKI:  Secondary create key.");
            symres = g_objAuthServerLdr.CreateObject( &ptrAuth );
            if (SYM_FAILED (symres))
            {
                nError=symres;
                throw( _T("CreateServerPKI:  CreateObject() for IAuthPtr failed:") );
            }

            if( use_root_auth == true )
            {
					 ptrAuth->SetAuthTypeRemote( ScsSecureComms::IAuth::ROOT_AUTH );
            }
            else
            {
                ptrAuth->SetAuthTypeRemote( ScsSecureComms::IAuth::NAME_AUTH );
            }

            DWORD cc = ERROR_GENERAL;

            dprintf("Calling SendCOM_ISSUE_CERT_FROM_CSR...\n");
            if( SendCOM_ISSUE_CERT_FROM_CSR != NULL )
            {
                cc = SendCOM_ISSUE_CERT_FROM_CSR( ptrAuth,
                                                  strPrimaryServerName,
                                                  SENDCOM_REMOTE_IS_SERVER | SENDCOM_REMOTE_USES_SECURE_COMM,
                                                  strUserName,
                                                  strPassword,
                                                  (login == false ? ScsSecureComms::IAuth::SERVER_EE_CERT_TYPE : ScsSecureComms::IAuth::LOGIN_CA_CERT_TYPE),
                                                  ptrCertSigningRequest,
                                                  ptrCerts.m_p );
            }
            else
            {
                throw( _T("CreateServerPKI:  SendCOM_ISSUE_CERT_FROM_CSR function pointer invalid.") );
            }
            dprintf("Back from SendCOM_ISSUE_CERT_FROM_CSR, returned %d.\n", cc);

            if( cc != ERROR_SUCCESS )
            {
                nError=cc;
                throw( _T("CreateServerPKI:  SendCOM_ISSUE_CERT_FROM_CSR failed:") );
            }

            if( ptrCerts->size() == 0 )
            {
                nError=0;
                throw( _T("CreateServerPKI:  SendCOM_ISSUE_CERT_FROM_CSR failed -- no certs") );
            }
            else
            {
                dprintf("CreateServerPKI:  Cert submission suceeded: %lu in the chain", ptrCerts->size());
            }

            // Match the cert and the key.
            ptrCert.Attach( ptrCerts->at(0) );

            nCommsRes = ptrCert->SetPrivateKey( privateKey );
            if (SYM_FAILED(nCommsRes))
            {
                nError=nCommsRes;
                throw( _T("CreateServerPKI:  SetPrivateKey() after cert generation failed:") );
            }

            // Save the root, if we got one.

            if( ptrCerts->size() > 1 )
            {
                ScsSecureComms::ICertPtr ptrRootCert;

                ptrRootCert.Attach( ptrCerts->at(1) );

                nCommsRes = ptrRootCert->Save( szRootCertPath );
                if (SYM_FAILED (nCommsRes))
                {
                    // Non fatal error.
                    dprintf("CreateServerPKI:  ERROR: Root cert save failed (Error #%d)", nError);
                }
            }
            else
            {
                dprintf("CreateServerPKI:  No root cert to save." );
            }
        }

        // Save the cert.
        nCommsRes = ptrCert->Save( szCertPath );
        if (SYM_FAILED (nCommsRes))
        {
            nError=nCommsRes;
            throw( _T("CreateServerPKI:  Cert save failed:") );
        }

        // Save the private key.
        nCommsRes = privateKey->Save( szKeyPath );
        if (SYM_FAILED (nCommsRes))
        {
            nError=nCommsRes;
            throw( _T("CreateServerPKI:  Private key save failed:") );
        }
    }
    catch( const TCHAR *szErr )
    {
        dprintf("CreateServerPKI:  %s (Error #%d)", szErr, nError);
    }
    catch(...)
    {
        dprintf("CreateServerPKI:  Unknown exception caught.");
    }

    return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Function: Find_FQDN
//
//  Create and returns the hostname and fully qualified domain name (FQDN).
//
//  08/21/2004 Daniel Kowalyshyn - Function created.
//
///////////////////////////////////////////////////////////////////////////////
BOOL Find_FQDN( LPTSTR pszName, LPTSTR pszFQDN )
{
    WORD                wVersionRequested;
#ifndef NLM
    WSADATA             wsaData;
#endif
    TCHAR               szMessString [MAX_PATH]     ={0};
    TCHAR               szName [MAX_PATH]           ={0};
    TCHAR               szFQDN [MAX_PATH]           ={0};
    HOSTENT*            hostinfo                    = NULL;
    INT                 rtn=TRUE;

#ifndef NLM
    wVersionRequested = MAKEWORD( 2, 0 );
#endif //NLM
    dprintf("Find_FQDN:  before:  pszName:%s  pszFQDNHost:%s  szName:%s  szFQDN:%s",pszName, (pszFQDN?pszFQDN:"(null)"), szName, szFQDN);

#ifndef NLM
    if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
    {
#endif //NLM
        if( NTSGetComputerName( szName, NULL ) > 0 )
        {
            if( (hostinfo = gethostbyname(szName)) != NULL )
            {
                _tcscpy( szFQDN, hostinfo->h_name );
                dprintf("Find_FQDN:  Host: %s Find_FQDN: %s",szName, szFQDN);
            }
            else
            {
                rtn=FALSE;
                dprintf("Find_FQDN:  gethostbyname failed");
            }
        }
        else
        {
            rtn=FALSE;
            dprintf("Find_FQDN:  gethostname failed.");
        }
#ifndef NLM
        WSACleanup( );
    }
    else
    {
        rtn=FALSE;
        dprintf("Find_FQDN:  gethostname failed.");
    }
#endif //NLM
    
// Make sure we were passed buffers
    if( pszName )
    {
        strcpy( pszName, szName ); 
        char* l = strchr( pszName, '.' ); 
        if( l )
        *l = '\0';
    }
    if( pszFQDN )
    {
        strcpy( pszFQDN, szFQDN );
    }

    dprintf("Find_FQDN:  after:  pszName:%s pszFQDNHost:%s szName:%s szFQDN:%s",pszName, (pszFQDN?pszFQDN:"(null)"), szName, szFQDN);

#ifdef NLM
    if (rtn == FALSE) ConsolePrintf( LS(IDS_FAILED_HOSTNAME_LOOKUP) );
#endif	 
	 return rtn;
}

///////////////////////////////////////////////////////////////////////////
//  Name:
//      CreatePKITargetPaths
//  Purpose:
//      Create the PKI outputfile targetpaths.
//
//  Return Values:
//
//  08/24/2004 Daniel Kowalyshyn - Function created.
//
///////////////////////////////////////////////////////////////////////////
BOOL CreatePKITargetPaths( LPCTSTR strPrimaryServerName,
                            LPTSTR strCertFilePath,
                            LPTSTR strPrivateKeyFilePath,
                            LPTSTR strRootCertFilePath,
                            LPTSTR strCSRFilePath,
                            LPTSTR szName,
                            LPTSTR szServerGroupGuid,
                            LPTSTR szCert,
                            LPTSTR szKey,
                            LPTSTR szRootCert,
                            LPTSTR szCSR )
{
    TCHAR   szMessString [MAX_PATH+1] = {0};
    TCHAR   szGenericFileName [MAX_PATH+1] = {0};
    DWORD   returnValDW = ERROR_OUT_OF_PAPER;

    PathAddBackslash( strCertFilePath );
    PathAddBackslash( strPrivateKeyFilePath );
    PathAddBackslash( strRootCertFilePath );
    PathAddBackslash( strCSRFilePath );

    // GetServerGroupGuid

    returnValDW = GetServerGroupGuid ( szServerGroupGuid, GET_SERVER_GROUP_GUID_BYTE_SIZE );
    if (returnValDW != ERROR_SUCCESS)
    {
        dprintf("CreatePKITargetPaths:  No domain GUID entry found - cannot proceed!");
        return FALSE;
    }

    _tcscpy ( szGenericFileName, szServerGroupGuid );   // copy the server group guid
    _tcscat ( szGenericFileName, "." );                 // add the period
    _tcscat ( szGenericFileName, "0.");                 // zero counter and add the period

    // concatenate the server name

    _tcscat ( strCertFilePath, szName );
    _tcscat ( strPrivateKeyFilePath, szName );
    _tcscat ( strCSRFilePath, szName );

    // add a period

    _tcscat ( strCertFilePath, "." );
    _tcscat ( strPrivateKeyFilePath, "." );
    _tcscat ( strCSRFilePath, "." );

    // concatenate the genericfile name

    _tcscat ( strCertFilePath, szGenericFileName );
    _tcscat ( strPrivateKeyFilePath, szGenericFileName );
    _tcscat ( strRootCertFilePath, szGenericFileName );
    _tcscat ( strCSRFilePath, szGenericFileName );

    // concatenate object names

    _tcscat ( strCertFilePath, szCert );
    _tcscat ( strPrivateKeyFilePath, szKey );
    _tcscat ( strRootCertFilePath, szRootCert );
    _tcscat ( strCSRFilePath, szCSR );

    dprintf("CreatePKITargetPaths:  strCertFilePath %s.", strCertFilePath);
    dprintf("CreatePKITargetPaths:  strPrivateKeyFilePath %s.", strPrivateKeyFilePath);
    dprintf("CreatePKITargetPaths:  strRootCertFilePath %s.", strRootCertFilePath);
    dprintf("CreatePKITargetPaths : strCSRFilePath %s.", strCSRFilePath);

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////
//  Purpose:
//     Pull the Server Group GUID out of the registry.
//
//  Parameters:
//    szGUID    [out] The server group guid
//    nNumBytes [in]  Number of bytes available in szDir.
//
//  Return Values:
//    ERROR_SUCCESS; ERROR_REG_FAIL
///////////////////////////////////////////////////////////////////////////
DWORD GetServerGroupGuid( LPTSTR szGuid, int nNumBytes )
{
    HKEY  hKey = NULL;
    DWORD nRtn = ERROR_REG_FAIL;

    SAVASSERT (szGuid);
    SAVASSERT (nNumBytes > 0);

    // Initialize our [out] params.
    szGuid[0] = '\0';

    // Decrement the nNumBytes byte one to ensure we always have room for the
    // terminating '\0' character.
    --nNumBytes;

    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main), 0, KEY_READ, &hKey))
    {
        unsigned char buf[64];
        DWORD         size = sizeof (buf);

        // if we can retrieve the path value then we are good to go
        if (SymSaferRegQueryValueEx(hKey,_T(szReg_Val_DomainGUID),0,NULL,buf,&size) == ERROR_SUCCESS)
        {
            if (size > 0)
            {
                // Covert the byte array to a string of hex characters.
                for (int xx = 0; static_cast<DWORD>(xx) < size && nNumBytes > 0; ++xx)
                {
                    unsigned char hi         = (unsigned char)((buf[xx] & 0xf0) >> 4);
                    unsigned char lo         = (unsigned char) (buf[xx] & 0x0f);
                    unsigned char nibbles[2] = { hi, lo };

                    for (int cnt = 0; cnt < 2; ++cnt)
                    {
                        if (nNumBytes > 0)
                        {
                            if (nibbles[cnt] < 10)
                                *szGuid = (char)(nibbles[cnt] + '0');
                            else
                                *szGuid = (char)((nibbles[cnt] - 10) + 'a');
                            ++szGuid;

                            --nNumBytes;
                        }
                    }
                }
                *szGuid = _T('\0');
                nRtn = ERROR_SUCCESS;
            }
        }

        RegCloseKey (hKey);
    }

    return nRtn;
}



void NukeDir( LPCTSTR lpszPath, BOOL fRecurse )
//Prunes a directory
//Ripped from NWS PCUTIL
{
   //First, search out all the links and verify they are correct.
   WIN32_FIND_DATA w32FD;
   TCHAR szSearch[MAX_PATH];
   lstrcpy(szSearch, lpszPath);

   lstrcat(szSearch, _T("\\*.*"));

   HANDLE hFind = FindFirstFile(szSearch, &w32FD);

   if (hFind != INVALID_HANDLE_VALUE)
   {
      do
      {

         if ( w32FD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            continue;

         TCHAR szFileName[MAX_PATH+16];
         lstrcpy( szFileName, lpszPath );
         lstrcat( szFileName, _T("\\") );
         lstrcat( szFileName, w32FD.cFileName );

#ifdef WIN32
         SetFileAttributes( szFileName, FILE_ATTRIBUTE_NORMAL );
#endif
         DeleteFile( szFileName );
      }
      while (FindNextFile(hFind, &w32FD));

      FindClose(hFind);
   }

   if (fRecurse)
   {
      //Enum through the directories and search them out for links
      lstrcpy(szSearch, lpszPath);
      lstrcat(szSearch, _T("\\*.*"));
      hFind = FindFirstFile(szSearch, &w32FD);
      if (hFind != INVALID_HANDLE_VALUE)
      {
         do
         {
            if (w32FD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && lstrcmp(w32FD.cFileName, _T(".")) && lstrcmp(w32FD.cFileName, _T("..")))
            {
               //Got a Directory, check it out.
               TCHAR szFullDir[MAX_PATH + 1];

               lstrcpy(szFullDir, lpszPath);
               lstrcat(szFullDir, _T("\\"));
               lstrcat(szFullDir, w32FD.cFileName);

               LPCSTR lpszFullPath = (LPCSTR) _tcschr(szFullDir, _T('\\'));
               if ( lpszFullPath )
               {
                  TCHAR szRecyclePath[32];
                  lpszFullPath = CharNext( lpszFullPath );
                  lstrcpyn( szRecyclePath, lpszFullPath, sizeof( _T("RECYCLED")+1 ));
                  if ( lstrcmpi( szRecyclePath, _T("RECYCLED") ) == 0 )
                     continue;
               }
               NukeDir( szFullDir, TRUE );
            }
         }
         while (FindNextFile(hFind, &w32FD));

         FindClose(hFind);
      }
   }

   //Try to remove the actual directory now.
   RemoveDirectory(lpszPath);
}

HRESULT CreateDomainGuid( LPCTSTR primaryServerName )
// Sets the HLM\SW\Intel\LANdesk\VirusProtect6\CurrentVersion\DomainGUID entry if needed
{
    GUID        domainGUID;
    HKEY        keyHandle                       = NULL;
    LPCTSTR     guidStringRepresentation        = NULL;
    DWORD       domainGUIDsize                  = 0;
    DWORD       returnValDW                     = ERROR_FILE_NOT_FOUND;
    HRESULT     returnValHR                     = E_FAIL;


    // Are we installing to an existing server group?
    if (_tcslen(primaryServerName) > 0)
    {
        TCHAR   szTemp[MAX_PATH+1]  = {""};
        HKEY    hKey                = NULL;
        DWORD   valueDataSize       = 0;

        // Yes, use it's domain GUID
        sssnprintf(szTemp, sizeof(szTemp), "%s\\%s\\%s", _T(szReg_Key_Main), _T(szReg_Key_AddressCache), primaryServerName);
        returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szTemp, 0, KEY_READ, &keyHandle);
        if (returnValDW == ERROR_SUCCESS)
        {
            valueDataSize = sizeof(domainGUID);
            returnValDW = SymSaferRegQueryValueEx(keyHandle, _T(szReg_Val_DomainGUID), 0, NULL, (BYTE*) &domainGUID, &valueDataSize);
            if (returnValDW == ERROR_SUCCESS )
                dprintf("CreateDomainGuid: Got GUID from %s AddressCache entry", primaryServerName);
            else
                dprintf("CreateDomainGuid: Failed to get GUID from %s (%s) AddressCache: %d", primaryServerName, szTemp, returnValDW);
            RegCloseKey(keyHandle);
        }
        else
        {
            dprintf("CreateDomainGuid: Failed to open %s AddressCache entry: %d", primaryServerName, returnValDW);
        }
    }
    else
    {
        // No, create a new one
        UuidCreateFake(&domainGUID);
        returnValDW = ERROR_SUCCESS;
    }
    returnValHR = HRESULT_FROM_WIN32(returnValDW);

    // Write the GUID we got to our DomainGUID reg value.
    if (returnValDW == ERROR_SUCCESS)
    {
        returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(szReg_Key_Main), 0, KEY_WRITE, &keyHandle);
        if (returnValDW == ERROR_SUCCESS)
        {
            domainGUIDsize = sizeof(domainGUID);
            returnValDW = SymSaferRegQueryValueEx(keyHandle, _T(szReg_Val_DomainGUID), 0, NULL, (LPBYTE) &domainGUID, &domainGUIDsize);
            if ((returnValDW == ERROR_SUCCESS) && (memcmp((void*) &domainGUID, (void*) &GUID_NULL, domainGUIDsize) != 0))
            {
                UuidToString(&domainGUID, (unsigned char**) &guidStringRepresentation);
                dprintf("CreateDomainGuid:  found existing DomainGUID:  %s", (guidStringRepresentation == NULL ? "" : guidStringRepresentation));                
                returnValHR = S_FALSE;
            }
            else
            {
                UuidToString(&domainGUID, (unsigned char**) &guidStringRepresentation);
                returnValDW = RegSetValueEx(keyHandle, szReg_Val_DomainGUID, 0, REG_BINARY, (LPBYTE) &domainGUID, sizeof(domainGUID));
                if (returnValDW == ERROR_SUCCESS)
                    dprintf("CreateDomainGuid:  reg set DomainGUID success:  %s", (guidStringRepresentation == NULL ? "" : guidStringRepresentation));
                else
                    dprintf("CreateDomainGuid:  reg set DomainGUID failed error %d (generated %s)", returnValDW, (guidStringRepresentation == NULL ? "" : guidStringRepresentation));
                returnValHR = HRESULT_FROM_WIN32(returnValDW);
            }
        }
        else
        {
            dprintf("CreateDomainGuid:  RegOpenKeyEx (%s) failed: %d", szReg_Key_Main, returnValDW);
            returnValHR = HRESULT_FROM_WIN32(returnValDW);
        }

        if (guidStringRepresentation != NULL)
            RpcStringFree((unsigned char**) &guidStringRepresentation);
    }

    return returnValHR;
}

DWORD CreateInitialAccount( LPCTSTR username, LPCTSTR password )
// Creates the initial admin account.  Password is the clear text password.
// Returns ERROR_SUCCESS on success, something else on error.
{
    HKEY                        keyHandle               = NULL;
    DWORD                       returnValDW             = ERROR_FILE_NOT_FOUND;

    returnValDW = RegCreateKey(HKEY_LOCAL_MACHINE, szReg_Key_Main "\\" szReg_Key_Accounts, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        returnValDW = CreateUserAccount(keyHandle, username, ScsSecureComms::BUILTIN_ROLE_ADMIN, password);
        if(returnValDW != ERROR_SUCCESS)
        {
            dprintf("CreateInitialAccount for user %s:  %d.", username, returnValDW);
        }

        RegCloseKey(keyHandle);
    }

    return returnValDW;
}

HRESULT CreatePKIdirectories( LPCTSTR baseDirectory )
// Creates all PKI store directories in baseDirectory (normally main program directory)
// if they do not already exist
// Returns S_OK on success, else the error code of the first error to occur
{
    TCHAR       pathBuffer[MAX_PATH+1]      = {""};
    DWORD       returnValDW                 = ERROR_FILE_NOT_FOUND;
    HRESULT     returnValHR                 = E_FAIL;

    // Validate parameters
    if (baseDirectory == NULL)
        return E_POINTER;

    // Initialize
    returnValHR = S_OK;

    sssnprintf(pathBuffer, sizeof(pathBuffer), "%s%s%s", baseDirectory, sSLASH, SYM_SCSCOMM_DIR_ROOT);
    returnValDW = mkdir(pathBuffer);
    if ((returnValDW != 0) && (errno != EEXIST))
    {
        dprintf("CreatePKIdirectories:  Error %d creating %s.", errno, pathBuffer);
        returnValHR = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    }

    sssnprintf(pathBuffer, sizeof(pathBuffer), "%s%s%s", baseDirectory, sSLASH, SYM_SCSCOMM_DIR_CER_ROOT);
    returnValDW = mkdir(pathBuffer);
    if ((returnValDW != 0) && (errno != EEXIST))
    {
        dprintf("CreatePKIdirectories:  Error %d creating %s.", errno, pathBuffer);
        returnValHR = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    }

    sssnprintf(pathBuffer, sizeof(pathBuffer), "%s%s%s", baseDirectory, sSLASH, SYM_SCSCOMM_DIR_CER_PKEY);
    returnValDW = mkdir(pathBuffer);
    if ((returnValDW != 0) && (errno != EEXIST))
    {
        dprintf("CreatePKIdirectories:  Error %d creating %s.", errno, pathBuffer);
        returnValHR = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    }

    sssnprintf(pathBuffer, sizeof(pathBuffer), "%s%s%s", baseDirectory, sSLASH, SYM_SCSCOMM_DIR_CER_CSR);
    returnValDW = mkdir(pathBuffer);
    if ((returnValDW != 0) && (errno != EEXIST))
    {
        dprintf("CreatePKIdirectories:  Error %d creating %s.", errno, pathBuffer);
        returnValHR = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    }

    sssnprintf(pathBuffer, sizeof(pathBuffer), "%s%s%s", baseDirectory, sSLASH, SYM_SCSCOMM_DIR_CER_CERTS);
    returnValDW = mkdir(pathBuffer);
    if ((returnValDW != 0) && (errno != EEXIST))
    {
        dprintf("CreatePKIdirectories:  Error %d creating %s.", errno, pathBuffer);
        returnValHR = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    }

//setup dirs for client install
    sssnprintf(pathBuffer, sizeof(pathBuffer), "%s%s%s%s%s", baseDirectory, sSLASH, WIN32_CLIENT_INSTALL_DIR, sSLASH, SYM_SCSCOMM_DIR_ROOT);
    returnValDW = mkdir(pathBuffer);
    if ((returnValDW != 0) && (errno != EEXIST))
    {
        dprintf("CreatePKIdirectories:  Error %d creating %s.", errno, pathBuffer);
        returnValHR = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    }

    sssnprintf(pathBuffer, sizeof(pathBuffer), "%s%s%s%s%s", baseDirectory, sSLASH, WIN32_CLIENT_INSTALL_DIR, sSLASH, SYM_SCSCOMM_DIR_CER_ROOT);
    returnValDW = mkdir(pathBuffer);
    if ((returnValDW != 0) && (errno != EEXIST))
    {
        dprintf("CreatePKIdirectories:  Error %d creating %s.", errno, pathBuffer);
        returnValHR = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    }

    sssnprintf(pathBuffer, sizeof(pathBuffer), "%s%s%s%s%s", baseDirectory, sSLASH, WIN32_CLIENT_INSTALL_DIR, sSLASH, SYM_SCSCOMM_DIR_CER_PKEY);
    returnValDW = mkdir(pathBuffer);
    if ((returnValDW != 0) && (errno != EEXIST))
    {
        dprintf("CreatePKIdirectories:  Error %d creating %s.", errno, pathBuffer);
        returnValHR = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    }

    sssnprintf(pathBuffer, sizeof(pathBuffer), "%s%s%s%s%s", baseDirectory, sSLASH, WIN32_CLIENT_INSTALL_DIR, sSLASH, SYM_SCSCOMM_DIR_CER_CSR);
    returnValDW = mkdir(pathBuffer);
    if ((returnValDW != 0) && (errno != EEXIST))
    {
        dprintf("CreatePKIdirectories:  Error %d creating %s.", errno, pathBuffer);
        returnValHR = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    }

    sssnprintf(pathBuffer, sizeof(pathBuffer), "%s%s%s%s%s", baseDirectory, sSLASH, WIN32_CLIENT_INSTALL_DIR, sSLASH, SYM_SCSCOMM_DIR_CER_CERTS);
    returnValDW = mkdir(pathBuffer);
    if ((returnValDW != 0) && (errno != EEXIST))
    {
        dprintf("CreatePKIdirectories:  Error %d creating %s.", errno, pathBuffer);
        returnValHR = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
    }

    return returnValHR;
}

// Evil hack
BOOL RemoveDirectory( LPCTSTR directoryName )
{
    if (rmdir(directoryName) == 0)
        return TRUE;
    else
        return FALSE;
}

DWORD GetCurrentDirectory( DWORD size, LPSTR path )
{

    getcwd(path, size);

    return 0;
}

BOOL CopyFile( LPCTSTR ExistingFile, LPCTSTR NewFile, BOOL FailIfExists )
{

    int sHan, dHan;
    int ret = FALSE;

    sHan = open(ExistingFile, O_BINARY|O_RDONLY, 0);
    if ( sHan != INVALID_FILE_HANDLE_VALUE )
    {
        dHan = open(NewFile, O_WRONLY|O_TRUNC|O_BINARY|(FailIfExists?O_EXCL:0)|O_CREAT, S_IREAD|S_IWRITE);
        if ( dHan != INVALID_FILE_HANDLE_VALUE )
        {
            LONG size = lseek(sHan, 0, SEEK_END);
            LONG out=0;
            lseek(sHan, 0, SEEK_SET);
            ret = FileServerFileCopy(sHan, dHan, 0, 0, size, &out);
            if (!ret)
                ret = TRUE;
            close(dHan);
        }
        close(sHan);
    }

    return(BOOL)ret;
}

BOOL MoveFile( LPCTSTR lpszExisting, LPCTSTR lpszNew )
{

    int ret;

    if (CopyFile(lpszExisting,lpszNew,TRUE))
    {
        MakeWriteable(lpszExisting, 0xfffffffc);
        ret = DeletePurgeFile(lpszExisting);
        if (!ret)
            return TRUE;
        DeletePurgeFile(lpszNew);
    }

    return FALSE;
}
