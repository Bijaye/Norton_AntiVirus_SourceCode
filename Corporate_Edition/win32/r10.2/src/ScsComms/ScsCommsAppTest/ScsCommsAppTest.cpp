// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ScsCommsAppTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma warning (disable: 4786) // identifier was truncated to '255' characters in the debug information

#define INITIIDS

#define array_sizeof(x) (sizeof(x) / sizeof(x[0]))

#include <winsock2.h>

#include <iostream>
#include <process.h>
#include <crtdbg.h>
#include <time.h>
#include <vector>
#include <string>

#include "SymInterface.h"
#include "SymInterfaceLoader.h"
#include "IUser.h"
#include "IAuth.h"
#include "IByteStreamer.h"
#include "ICertSigningRequest.h"
#include "ICertIssuer.h"
#include "IMessageManager.h"
#include "..\ScsCommsTest\example_cert_store\example_cert_store.h"
#include "CertIssueDefaults.h"
#include "RoleVector.h"

const char g_szPathToScsCommsDll[]        = "ScsComms.dll";
const char g_szPathToScsCommsAppTestExe[] = "ScsCommsAppTest.exe";

std::string g_szPathToCertStore          = "../ScsCommsTest/example_cert_store/";
std::string g_szPathToRootCert           = g_szPathToCertStore + g_szRelativePathToRootCert;
std::string g_szPathToRootKey            = g_szPathToCertStore + g_szRelativePathToRootKey;
std::string g_szPathToLoginCACert        = g_szPathToCertStore + g_szRelativePathToLoginCACert;
std::string g_szPathToLoginCAKey         = g_szPathToCertStore + g_szRelativePathToLoginCAKey;
std::string g_szPathToLoginEeCert        = g_szPathToCertStore + g_szRelativePathToLoginEeCert;
std::string g_szPathToServerCert         = g_szPathToCertStore + g_szRelativePathToServerCert;
std::string g_szPathToServerKey          = g_szPathToCertStore + g_szRelativePathToServerKey;

using namespace ScsSecureComms;

SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

static void print_usage()
{
    std::cout << "ScsCommsAppTest: Application API Test framework for the ScsComms component.\n";
    std::cout << std::endl;
}

static void printRoles (ICert *pCert)
{
    unsigned int    num_roles = 0;
    IRoleVectorPtr  ptrRoles;

    RESULT symres = pCert->GetRoles (*(&ptrRoles));
    if (SYM_FAILED (symres))
    {
        std::cout << "ERROR: GetRoles() failed: 0x" << std::hex << symres << ".\n";
        return;
    }

    std::cout << "Roles (" << std::dec << ptrRoles->size() << "): ";
    for (unsigned int xx = 0; xx < ptrRoles->size(); ++xx)
    {
        const char *szRole = ptrRoles->at (xx);
        if (xx != 0)
            std::cout << ", ";
        std::cout << szRole;
    }
    std::cout << "\n";
}

static int app_test()
{
    RESULT nCommsRes = RTNERR_FAILED;

    typedef CSymInterfaceHelper <CSymInterfaceLoader,
                                 IUser,
                                 &SYMOBJECT_User,
                                 &IID_User> CUserLoader;
    typedef CSymInterfaceHelper <CSymInterfaceLoader,
                                 ICert,
                                 &SYMOBJECT_Cert,
                                 &IID_Cert> CCertLoader;
    typedef CSymInterfaceHelper <CSymInterfaceLoader,
                                 IAuth,
                                 &SYMOBJECT_AuthLogin,
                                 &IID_Auth> CAuthLoginLoader;
    typedef CSymInterfaceHelper <CSymInterfaceLoader,
                                 IAuth,
                                 &SYMOBJECT_AuthServer,
                                 &IID_Auth> CAuthServerLoader;
    typedef CSymInterfaceHelper <CSymInterfaceLoader,
                                 IAuth,
                                 &SYMOBJECT_AuthRemote,
                                 &IID_Auth> CAuthRemoteLoader;
    typedef CSymInterfaceHelper <CSymInterfaceLoader,
                                 ICertSigningRequest,
                                 &SYMOBJECT_CertSigningRequest,
                                 &IID_CertSigningRequest> CCertSigningRequest;
    typedef CSymInterfaceHelper <CSymInterfaceLoader,
                                 ICertIssuer,
                                 &SYMOBJECT_CertIssuer,
                                 &IID_CertIssuer> CCertIssuer;
    typedef CSymInterfaceHelper <CSymInterfaceLoader,
                                 IKey,
                                 &SYMOBJECT_Key,
                                 &IID_Key> CKeyLoader;
    typedef CSymInterfaceHelper <CSymInterfaceLoader,
                                 IMessageManager,
                                 &SYMOBJECT_MessageManager,
                                 &IID_MessageManager> CMsgMgrLoader;

    typedef CSymInterfaceHelper< CSymInterfaceLoader,
                                 ICertVector,
                                 &SYMOBJECT_CertVector,
                                 &IID_CertVector > CertVectorLoader;


    CMsgMgrLoader      objMsgMgrLgr;
    CUserLoader        objUserLdr;
    CCertLoader        objCertLdr;
    IUserPtr           ptrUser;
    IAuthPtr           ptrAuth;
    CAuthLoginLoader   objAuthLoginLdr;
    CAuthServerLoader  objAuthServerLdr;
    CAuthRemoteLoader  objAuthRemoteLdr;
    IAuthPtr           ptrAuthServer;

    CCertSigningRequest     objCertSigningRequestLdr;
    CCertIssuer             objCertIssuerLdr;
    CKeyLoader              objKeyLdr;
    CertVectorLoader        objCertVectorLdr;

    SYMRESULT symres;

    // Initialize the path for all of our loader objects.

    symres = objMsgMgrLgr.Initialize( g_szPathToScsCommsDll );
    if (SYM_FAILED (symres))
        return 1;

    symres = objUserLdr.Initialize( g_szPathToScsCommsDll );
    if (SYM_FAILED (symres))
        return 1;

    symres = objCertLdr.Initialize( g_szPathToScsCommsDll );
    if (SYM_FAILED (symres))
        return 1;

    symres = objAuthLoginLdr.Initialize( g_szPathToScsCommsDll );
    if (SYM_FAILED (symres))
        return 1;

    symres = objAuthServerLdr.Initialize( g_szPathToScsCommsDll );
    if (SYM_FAILED (symres))
        return 1;

    symres = objAuthRemoteLdr.Initialize( g_szPathToScsCommsDll );
    if (SYM_FAILED (symres))
        return 1;

    symres = objCertSigningRequestLdr.Initialize( g_szPathToScsCommsDll );
    if (SYM_FAILED (symres))
        return 1;

    symres = objCertIssuerLdr.Initialize( g_szPathToScsCommsDll );
    if (SYM_FAILED (symres))
        return 1;

    symres = objKeyLdr.Initialize( g_szPathToScsCommsDll );
    if (SYM_FAILED (symres))
        return 1;

    symres = objCertVectorLdr.Initialize( g_szPathToScsCommsDll );
    if (SYM_FAILED (symres))
        return 1;

    ICertPtr        ptrTempCert;
    IKeyPtr         ptrTempKey;

    // Create our first object.
    symres = objUserLdr.CreateObject( &ptrUser );
    if (SYM_FAILED (symres))
        return 1;


    if (SYM_SUCCEEDED (symres))
    {
        std::string strUser ("admin");
        char szTemp[32];
        nCommsRes = ptrUser->SetName (strUser.c_str());
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetName() failed: 0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrUser->GetName (szTemp, sizeof (szTemp));
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: GetName() failed: 0x" << std::hex << nCommsRes << ".\n";

        if (strcmp (szTemp, strUser.c_str()) != 0)
        {
            std::cout << "ERROR: user name string mismatch.\n";
            return 1;
        }

        std::string strPwd ("abcdef");
        nCommsRes = ptrUser->SetPassword (strPwd.c_str());
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetPassword() failed: 0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrUser->GetPassword (szTemp, sizeof (szTemp));
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: GetPassword() failed: 0x" << std::hex << nCommsRes << ".\n";

        if (strcmp (szTemp, strPwd.c_str()) != 0)
        {
            std::cout << "ERROR: password string mismatch.\n";
            return 1;
        }

        // Test password hashing.

        unsigned char szSalt[4];
        unsigned char szHashPwd1[IUSER_HASH_LEN + 1];
        unsigned char szHashPwd2[IUSER_HASH_LEN + 1];
        unsigned long nHashPwdLen;

        std::string strPasswordForHash;

        // Password with salt 1.

        nCommsRes = ptrUser->GetSalt (szSalt, sizeof (szSalt));
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: GetSalt() failed: 0x" << std::hex << nCommsRes << ".\n";

        strPasswordForHash.assign( (char*)szSalt, sizeof(szSalt) );
        strPasswordForHash += strUser;
        strPasswordForHash += "00112233445566778899AABBCCDDEEFF"; // a server group GUID
        strPasswordForHash += strPwd;

        nCommsRes = ptrUser->SetPassword (strPasswordForHash.c_str());
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetPassword() failed: 0x" << std::hex << nCommsRes << ".\n";

        nHashPwdLen = sizeof(szHashPwd1);
        nCommsRes = ptrUser->GetPasswordHash (szHashPwd1, &nHashPwdLen);
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: GetSalt() failed: 0x" << std::hex << nCommsRes << ".\n";

        // Password with salt 2.

        nCommsRes = ptrUser->GetSalt (szSalt, sizeof (szSalt));
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: GetSalt() failed: 0x" << std::hex << nCommsRes << ".\n";

        strPasswordForHash.assign( (char*)szSalt, sizeof(szSalt) );
        strPasswordForHash += strUser;
        strPasswordForHash += "00112233445566778899AABBCCDDEEFF";
        strPasswordForHash += strPwd;

        nCommsRes = ptrUser->SetPassword (strPasswordForHash.c_str());
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetPassword() failed: 0x" << std::hex << nCommsRes << ".\n";

        nHashPwdLen = sizeof(szHashPwd2);
        nCommsRes = ptrUser->GetPasswordHash (szHashPwd2, &nHashPwdLen);
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: GetSalt() failed: 0x" << std::hex << nCommsRes << ".\n";

        // Should be different.

        if( memcmp( szHashPwd1, szHashPwd2, sizeof(szHashPwd1) ) == 0 )
            std::cout << "ERROR: Passwords with different salt are idnetical" << std::hex << nCommsRes << ".\n";

        // Set a non-admin user name.

        std::string strUser2 ("readonly");
        nCommsRes = ptrUser->SetName (strUser2.c_str());
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetName() failed: 0x" << std::hex << nCommsRes << ".\n";

        ICertPtr ptrCert;

        if (SYM_SUCCEEDED (symres))
            symres = objCertLdr.CreateObject( &ptrCert );

        nCommsRes = ptrCert->Load (g_szPathToLoginEeCert.c_str());
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: Cert load failure: 0x" << std::hex << nCommsRes << "\n";
            return 1;
        }

        printRoles (ptrCert);
    }

    if (SYM_SUCCEEDED (symres))
        symres = objAuthRemoteLdr.CreateObject( &ptrAuth );

    if (SYM_SUCCEEDED (symres))
    {
        nCommsRes = ptrAuth->SetAuthTypeRemote (IAuth::NAME_AUTH);
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetAuthTypeRemote() failed: 0x" << std::hex << nCommsRes << ".\n";

        IAuth::AUTH_TYPE type = ptrAuth->GetAuthTypeRemote ();

        if (type != IAuth::NAME_AUTH)
            std::cout << "ERROR: GetAuthType() result mismatch.\n";

        ICertPtr ptrCert;

        if (SYM_SUCCEEDED (symres))
            symres = objCertLdr.CreateObject( &ptrCert );

        nCommsRes = ptrCert->Load (g_szPathToLoginEeCert.c_str());
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: Cert load failure: 0x" << std::hex << nCommsRes << "\n";
            return 1;
        }
        nCommsRes = ptrAuth->SetCertChain (&ptrCert, 1);
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetCertChain() failed: 0x" << std::hex << nCommsRes << ".\n";

        printRoles (ptrCert);
        nCommsRes = ptrAuth->IsOperationAllowed (COMMS_ACCESS_FILE_WRITE);
        if (nCommsRes == RTN_OK)
            std::cout << "ACCESS_WRITE allowed.\n";
        else if (nCommsRes == RTN_FALSE)
            std::cout << "ACCESS_WRITE not allowed.\n";
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: IsOperationAllowed() failed: 0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrAuth->IsOperationAllowed (COMMS_ACCESS_FILE_READ);
        if (nCommsRes == RTN_OK)
            std::cout << "ACCESS_READ allowed.\n";
        else if (nCommsRes == RTN_FALSE)
            std::cout << "ACCESS_READ not allowed.\n";
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: IsOperationAllowed() failed: 0x" << std::hex << nCommsRes << ".\n";


        ICertVectorPtr  ptrCertVector;
        nCommsRes = ptrAuth->GetCertChain (*(&ptrCertVector));
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: GetCertChain() failed: 0x" << std::hex << nCommsRes << ".\n";
            return 1;
        }

        std::cout << "Certs (" << ptrCertVector->size() << "):\n";
        for (unsigned int xx = 0; xx < ptrCertVector->size(); ++xx)
        {
            char     szName[128];
            
            ptrTempCert.Attach (ptrCertVector->at (xx));
            
            nCommsRes = ptrTempCert->GetIssuerName (szName, sizeof (szName));
            if (SYM_FAILED (nCommsRes))
                std::cout << "ERROR: GetIssuerName() failed:  0x" << std::hex << nCommsRes << ".\n";
            else
                std::cout << xx << ". IssuerName: " << szName << "\n";

            nCommsRes = ptrTempCert->GetSubjectName (szName, sizeof (szName));
            if (SYM_FAILED (nCommsRes))
                std::cout << "ERROR: GetSubjectName() failed:  0x" << std::hex << nCommsRes << ".\n";
            else
                std::cout << "   SubjectName: " << szName << "\n";

            nCommsRes = ptrTempCert->GetIssuerCommonName (szName, sizeof (szName));
            if (SYM_FAILED (nCommsRes))
                std::cout << "ERROR: GetIssuerCommonName() failed:  0x" << std::hex << nCommsRes << ".\n";
            else
                std::cout << "   Issuer CommonName: " << szName << "\n";

            nCommsRes = ptrTempCert->GetSubjectCommonName (szName, sizeof (szName));
            if (SYM_FAILED (nCommsRes))
                std::cout << "ERROR: GetSubjectCommonName() failed:  0x" << std::hex << nCommsRes << ".\n";
            else
                std::cout << "   Subject CommonName: " << szName << "\n";

            unsigned long ulSerNum = 0;

            nCommsRes = ptrTempCert->GetSerialNumber (ulSerNum);
            if (SYM_FAILED (nCommsRes))
                std::cout << "ERROR: GetSerialNumber() failed:  0x" << std::hex << nCommsRes << ".\n";
            else
                std::cout << "   SerialNumber: " << ulSerNum << "\n";
        }
    }

    if (SYM_SUCCEEDED (symres))
        symres = objAuthServerLdr.CreateObject( &ptrAuthServer );

    if (SYM_SUCCEEDED (symres))
    {
        ICertPtr ptrCert;

        if (SYM_SUCCEEDED (symres))
            symres = objCertLdr.CreateObject( &ptrCert );

        if (SYM_SUCCEEDED (symres))
        {
            nCommsRes = ptrCert->Load (g_szPathToRootCert.c_str());
            if (SYM_FAILED(nCommsRes))
            {
                std::cout << "ERROR: Cert load failure: 0x" << std::hex << nCommsRes << "\n";
                return 1;
            }

            nCommsRes = ptrAuthServer->SetCertChain (&ptrCert, 1);
            if (SYM_FAILED(nCommsRes))
            {
                std::cout << "ERROR: SetServerCert() failed: 0x" << std::hex << nCommsRes << "\n";
                return 1;
            }
            ptrCert.Release();

            ICertVectorPtr  ptrCertVector;
            nCommsRes = ptrAuth->GetCertChain (*(&ptrCertVector));
            if (SYM_FAILED(nCommsRes))
            {
                std::cout << "ERROR: SetServerCert() failed: 0x" << std::hex << nCommsRes << "\n";
                return 1;
            }

            char szName[128];

            ptrCert.Attach (ptrCertVector->at (0));
            nCommsRes = ptrCert->GetIssuerName (szName, sizeof (szName));
            if (SYM_FAILED (nCommsRes))
                std::cout << "ERROR: GetIssuerName() failed:  0x" << std::hex << nCommsRes << ".\n";
            else
                std::cout << "Cert IssuerName: " << szName << "\n";

            nCommsRes = ptrCert->GetSubjectName (szName, sizeof (szName));
            if (SYM_FAILED (nCommsRes))
                std::cout << "ERROR: GetSubjectName() failed:  0x" << std::hex << nCommsRes << ".\n";
            else
                std::cout << "Cert SubjectName: " << szName << "\n";
        }
    }

    // Test for IAuth equality.

    IAuthPtr ptrAuthEquality;

    if (SYM_SUCCEEDED (symres))
        symres = objAuthServerLdr.CreateObject( &ptrAuthEquality );

    if (SYM_SUCCEEDED (symres))
    {
        nCommsRes = ptrAuth->IsEqual( ptrAuthServer );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: IAuth::IsEqual() failed:  0x" << std::hex << nCommsRes << ".\n";
        else if (nCommsRes == RTN_OK)
            std::cout << "ERROR: IAuth::IsEqual() returned RTN_OK on unequal objects.\n";

        nCommsRes = ptrAuth->IsEqual( ptrAuth );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: IAuth::IsEqual() failed:  0x" << std::hex << nCommsRes << ".\n";
        else if (nCommsRes == RTN_FALSE)
            std::cout << "ERROR: IAuth::IsEqual() returned RTN_FALSE on equal objects.\n";

        // Set ptrAuthEquality to equal 
        ptrAuthEquality->SetAuthTypeRemote( ptrAuth->GetAuthTypeRemote() );

        ICertVectorPtr ptrTempCertVector;

        nCommsRes = ptrAuth->GetCertChain (*(&ptrTempCertVector));

        ptrAuthEquality->SetCertChain( ptrTempCertVector );

        nCommsRes = ptrAuthEquality->IsEqual( ptrAuth );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: IAuth::IsEqual() failed:  0x" << std::hex << nCommsRes << ".\n";
        else if (nCommsRes == RTN_FALSE)
            std::cout << "ERROR: IAuth::IsEqual() returned RTN_FALSE on equal objects.\n";

        try
        {
            ICert **cert_array = new ICert *[ptrTempCertVector->size()];

            for( DWORD n = 0; n < ptrTempCertVector->size(); ++n )
                cert_array[n] = ptrTempCertVector->at( n );

            ptrAuthEquality->SetCertChain( cert_array, ptrTempCertVector->size() );

            nCommsRes = ptrAuthEquality->IsEqual( ptrAuth );
            if (SYM_FAILED (nCommsRes))
                std::cout << "ERROR: IAuth::IsEqual() failed:  0x" << std::hex << nCommsRes << ".\n";
            else if (nCommsRes == RTN_FALSE)
                std::cout << "ERROR: IAuth::IsEqual() returned RTN_FALSE on equal objects.\n";

            for( n = 0; n < ptrTempCertVector->size(); ++n )
                cert_array[n]->Release();

            delete[] cert_array;
        }
        catch (std::bad_alloc &)
        {
        }
    }

    ICertSigningRequestPtr  ptrCertSigningRequestUserLogin;
    ICertSigningRequestPtr  ptrCertSigningRequestServer;
    ICertSigningRequestPtr  ptrCertSigningRequestServerHiAscii;
    ICertSigningRequestPtr  ptrCertSigningRequestSGRoot;
    IKeyPtr privateKeyUserLogin;
    IKeyPtr privateKeyServer;
    IKeyPtr privateKeyServerHiAscii;
    IKeyPtr privateKeySGRoot;

    if (SYM_SUCCEEDED (symres))
        symres = objCertSigningRequestLdr.CreateObject( &ptrCertSigningRequestUserLogin );
    if (SYM_SUCCEEDED (symres))
        symres = objCertSigningRequestLdr.CreateObject( &ptrCertSigningRequestServer );
    if (SYM_SUCCEEDED (symres))
        symres = objCertSigningRequestLdr.CreateObject( &ptrCertSigningRequestServerHiAscii );
    if (SYM_SUCCEEDED (symres))
        symres = objCertSigningRequestLdr.CreateObject( &ptrCertSigningRequestSGRoot );

    if (SYM_SUCCEEDED (symres))
    {
        // User cert

        nCommsRes = ptrCertSigningRequestUserLogin->CreateKeyPair( 1024, privateKeyUserLogin.m_p );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: CreateKeyPair() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertSigningRequestUserLogin->Create( "joe_blow", NULL, ScsSecureComms::BUILTIN_CERTTYPE_LOGINEE, NULL );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: Create() failed:  0x" << std::hex << nCommsRes << ".\n";

        // Server cert

        nCommsRes = ptrCertSigningRequestServer->CreateKeyPair( 1024, privateKeyServer.m_p );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: CreateKeyPair() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertSigningRequestServer->Create( "joesmachine", NULL, ScsSecureComms::BUILTIN_CERTTYPE_SERVER, "joesmachine.symantec.com" );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: Create() failed:  0x" << std::hex << nCommsRes << ".\n";

        // Server cert - hi ascii

        nCommsRes = ptrCertSigningRequestServerHiAscii->CreateKeyPair( 1024, privateKeyServerHiAscii.m_p );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: CreateKeyPair() failed:  0x" << std::hex << nCommsRes << ".\n";

        // This is what you get if you hit "abcdef" on a keyboard mapped for cyrillic.
        unsigned char szHostNameHiAscii[] = { 0xD4, 0xC8, 0xD1, 0xC2, 0xD3, 0xC0, 0x00 };

        std::string strDNSNameHiAscii;
        strDNSNameHiAscii = reinterpret_cast<const char*>(szHostNameHiAscii);
        strDNSNameHiAscii += ".symantec.com";

        nCommsRes = ptrCertSigningRequestServerHiAscii->Create( reinterpret_cast<const char*>(szHostNameHiAscii), NULL, ScsSecureComms::BUILTIN_CERTTYPE_SERVER, strDNSNameHiAscii.c_str() );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: Create() failed:  0x" << std::hex << nCommsRes << ".\n";

        // Root cert

        nCommsRes = ptrCertSigningRequestSGRoot->CreateKeyPair( 1024, privateKeySGRoot.m_p );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: CreateKeyPair() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertSigningRequestSGRoot->Create( "00112233445566778899AABBCCDDEEFF", NULL, ScsSecureComms::BUILTIN_CERTTYPE_SERVERGROUPROOTCA, NULL );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: Create() failed:  0x" << std::hex << nCommsRes << ".\n";

        // Test the CSR save and load stuff.

        const char* szTempSGRootCSRFileName = "c:\\tempsgrootcert.csr";

        nCommsRes = ptrCertSigningRequestSGRoot->Save( szTempSGRootCSRFileName );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: Save() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertSigningRequestSGRoot->Load( szTempSGRootCSRFileName );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: Load() failed:  0x" << std::hex << nCommsRes << ".\n";

        remove( szTempSGRootCSRFileName );
    }

    ICertIssuerPtr  ptrCertIssuer;
    IKeyPtr         ptrRootKey;
    IKeyPtr         ptrLoginCAKey;
    ICertPtr        ptrRootCert;
    ICertPtr        ptrRootCert2;
    ICertPtr        ptrLoginCACert;

    ICertPtr        ptrServerCert;
    IKeyPtr         ptrServerKey;
    ICertPtr        ptrServerCertHiAscii;
    IKeyPtr         ptrServerKeyHiAscii;

    ICertPtr        ptrUserCert;
    ICertPtr        ptrSGRootCert;

    // Issue the certs.
    if (SYM_SUCCEEDED (symres))
        symres = objCertIssuerLdr.CreateObject( &ptrCertIssuer );

    if (SYM_SUCCEEDED (symres))
        symres = objKeyLdr.CreateObject( &ptrRootKey );
    if (SYM_SUCCEEDED (symres))
        symres = objKeyLdr.CreateObject( &ptrLoginCAKey );
    if (SYM_SUCCEEDED (symres))
        symres = objCertLdr.CreateObject( &ptrRootCert );
    if (SYM_SUCCEEDED (symres))
        symres = objCertLdr.CreateObject( &ptrRootCert2 );
    if (SYM_SUCCEEDED (symres))
        symres = objCertLdr.CreateObject( &ptrLoginCACert );
    if (SYM_SUCCEEDED (symres))
        symres = objKeyLdr.CreateObject( &ptrServerKey );
    if (SYM_SUCCEEDED (symres))
        symres = objCertLdr.CreateObject( &ptrServerCert );
    if (SYM_SUCCEEDED (symres))
        symres = objKeyLdr.CreateObject( &ptrServerKeyHiAscii );
    if (SYM_SUCCEEDED (symres))
        symres = objCertLdr.CreateObject( &ptrServerCertHiAscii );

    if (SYM_SUCCEEDED (symres))
    {
        // Load key and cert objects.

        nCommsRes = ptrRootCert->Load (g_szPathToRootCert.c_str());
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: Cert load failure: 0x" << std::hex << nCommsRes << "\n";
            return 1;
        }

        nCommsRes = ptrRootKey->Load (g_szPathToRootKey.c_str());
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: Key load failure: 0x" << std::hex << nCommsRes << "\n";
            return 1;
        }

        nCommsRes = ptrRootCert->SetPrivateKey( ptrRootKey );
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: SetPrivateKey() failed: 0x" << std::hex << nCommsRes << "\n";
            return 1;
        }

        nCommsRes = ptrLoginCACert->Load (g_szPathToLoginCACert.c_str());
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: Cert load failure: 0x" << std::hex << nCommsRes << "\n";
            return 1;
        }

        nCommsRes = ptrLoginCAKey->Load (g_szPathToLoginCAKey.c_str());
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: Key load failure: 0x" << std::hex << nCommsRes << "\n";
            return 1;
        }

        nCommsRes = ptrLoginCACert->SetPrivateKey( ptrLoginCAKey );
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: SetPrivateKey() failed: 0x" << std::hex << nCommsRes << "\n";
            return 1;
        }

        nCommsRes = ptrServerCert->Load (g_szPathToServerCert.c_str());
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: Cert load failure: 0x" << std::hex << nCommsRes << "\n";
            return 1;
        }

        nCommsRes = ptrServerKey->Load (g_szPathToServerKey.c_str());
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: Key load failure: 0x" << std::hex << nCommsRes << "\n";
            return 1;
        }

        nCommsRes = ptrServerCert->SetPrivateKey( ptrServerKey );
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: SetPrivateKey() failed: 0x" << std::hex << nCommsRes << "\n";
            return 1;
        }

        time_t expiry = 0;
        time_t start = 0;

        nCommsRes = ptrServerCert->IsExpired();
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: IsExpired() failed: 0x" << std::hex << nCommsRes << "\n";
        }

        nCommsRes = ptrServerCert->IsNotYetValid();
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: IsNotYetValid() failed: 0x" << std::hex << nCommsRes << "\n";
        }

        nCommsRes = ptrServerCert->IsValid();
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: IsValid() failed: 0x" << std::hex << nCommsRes << "\n";
        }

        nCommsRes = ptrServerCert->GetExpirationDate( expiry );
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: GetExpirationDate() failed: 0x" << std::hex << nCommsRes << "\n";
        }

        TCHAR szExpiryDate[256];
        nCommsRes = ptrServerCert->GetExpirationDateAsLocalString( szExpiryDate, sizeof(szExpiryDate) );
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: GetExpirationDateAsLocalString() failed: 0x" << std::hex << nCommsRes << "\n";
        }

        nCommsRes = ptrServerCert->GetStartDate( start );
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: GetStartDate() failed: 0x" << std::hex << nCommsRes << "\n";
        }

        TCHAR szStartDate[256];
        nCommsRes = ptrServerCert->GetStartDateAsLocalString( szStartDate, sizeof(szStartDate) );
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: GetStartDateAsLocalString() failed: 0x" << std::hex << nCommsRes << "\n";
        }

        time_t now = time(NULL);

        std::cout << "cert expires on "
                  << szExpiryDate
                  << ", in "
                  << std::dec
                  << (expiry - now) / (60*60*24) << " days, "
                  << ((expiry - now) % (60*60*24)) / (60*60) << " hours, "
                  << ((expiry - now) % (60*60)) / 60 << " minutes, "
                  << (expiry - now) % (60) << " seconds." << std::endl;
        std::cout << "cert started on "
                  << szStartDate
                  << ", has been valid for "
                  << std::dec
                  << (now - start) / (60*60*24) << " days, "
                  << ((now - start) % (60*60*24)) / (60*60) << " hours, "
                  << ((now - start) % (60*60)) / 60 << " minutes, "
                  << (now - start) % (60) << " seconds." << std::endl;

        ptrServerCert.Release();
        ptrServerKey.Release();

        // Test some key reset scenarios in ICert

        nCommsRes = ptrRootCert->SetPrivateKey( ptrRootKey );
        if (SYM_FAILED(nCommsRes))
            std::cout << "ERROR: SetPrivateKey() failed to reset key: 0x" << std::hex << nCommsRes << "\n";

        nCommsRes = ptrLoginCACert->SetPrivateKey( NULL );
        if (SYM_FAILED(nCommsRes))
            std::cout << "ERROR: SetPrivateKey() failed with NULL key: 0x" << std::hex << nCommsRes << "\n";

        ptrTempKey.Attach( ptrLoginCACert->GetPrivateKey() );
        if (ptrTempKey != NULL)
            std::cout << "ERROR: GetPrivateKey() should return NULL key\n";

        nCommsRes = ptrLoginCACert->SetPrivateKey( ptrLoginCAKey );
        if (SYM_FAILED(nCommsRes))
            std::cout << "ERROR: SetPrivateKey() failed: 0x" << std::hex << nCommsRes << "\n";

        // Cert equality tests.

        nCommsRes = ptrRootCert2->Load (g_szPathToRootCert.c_str()); // Get a separate object with the same data.
        if (SYM_FAILED(nCommsRes))
        {
            std::cout << "ERROR: Cert load failure: 0x" << std::hex << nCommsRes << "\n";
            return 1;
        }

        nCommsRes = ptrRootCert->IsEqual( ptrLoginCACert );
        if (SYM_FAILED(nCommsRes))
            std::cout << "ERROR: IsEqual() failed: 0x" << std::hex << nCommsRes << "\n";
        else if( nCommsRes == RTN_OK )
            std::cout << "ERROR: IsEqual() failed: returned true on different certs\n";

        nCommsRes = ptrRootCert->IsEqual( ptrRootCert2 );
        if (SYM_FAILED(nCommsRes))
            std::cout << "ERROR: IsEqual() failed: 0x" << std::hex << nCommsRes << "\n";
        else if( nCommsRes == RTN_FALSE )
            std::cout << "ERROR: IsEqual() failed: returned false on equivalent certs\n";

        // Issue a root cert.

        nCommsRes = ptrCertIssuer->SetValidityPeriod( SERVER_GROUP_CA_DEFAULT_LIFETIME ); // 10 years
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetValidityPeriod() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->SetValidityPreDatePeriod( 2000000 ); // 1 hour
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetValidityPreDatePeriod() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->SetRoles( NULL, 0 );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetRoles() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->SetBasicConstraints( true, -1 );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetBasicConstraints() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->SetIssuingCA( NULL );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetIssuingCA() with NULL failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->IssueCertSelfSigned( 1, ptrCertSigningRequestSGRoot, privateKeySGRoot, ptrSGRootCert.m_p );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: IssueCertSelfSigned() failed:  0x" << std::hex << nCommsRes << ".\n";
        else
        {
            nCommsRes = ptrSGRootCert->SetPrivateKey( privateKeySGRoot );
            if (SYM_FAILED(nCommsRes))
                std::cout << "ERROR: SetPrivateKey() after cert generation failed: 0x" << std::hex << nCommsRes << "\n";
        }

        nCommsRes = ptrSGRootCert->IsRoot();
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: IssueCertSelfSigned() failed:  0x" << std::hex << nCommsRes << ".\n";
        else if (nCommsRes == RTN_FALSE)
            std::cout << "ERROR: IsRoot() failed: returned false on a root certs\n";

        // Issue a user cert.

        const char* roles_user[] =
        {
            BUILTIN_ROLE_ADMIN,
            BUILTIN_ROLE_READONLY
        };

        nCommsRes = ptrCertIssuer->SetValidityPeriod( LOGIN_EE_CERT_DEFAULT_LIFETIME ); // 1 hour
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetValidityPeriod() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->SetRoles( roles_user, array_sizeof(roles_user) );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetRoles() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->SetBasicConstraints( false, 0 );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetBasicConstraints() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->SetIssuingCA( ptrLoginCACert );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetIssuingCA() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->IssueCert( 12345678, ptrCertSigningRequestUserLogin, ptrUserCert.m_p );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: IssueCert() failed:  0x" << std::hex << nCommsRes << ".\n";
        else
        {
            nCommsRes = ptrUserCert->SetPrivateKey( privateKeyUserLogin );
            if (SYM_FAILED(nCommsRes))
                std::cout << "ERROR: SetPrivateKey() after cert generation: 0x" << std::hex << nCommsRes << "\n";
        }

        // Issue a server cert.

        const char* roles_server[] =
        {
            BUILTIN_ROLE_SERVER
        };

        nCommsRes = ptrCertIssuer->SetValidityPeriod( SERVER_CERT_DEFAULT_LIFETIME ); // 10 years
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetValidityPeriod() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->SetRoles( roles_server, array_sizeof(roles_server) );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetRoles() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->SetBasicConstraints( false, 0 );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetBasicConstraints() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->SetIssuingCA( ptrSGRootCert );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetIssuingCA() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->IssueCert( 12345678, ptrCertSigningRequestServer, ptrServerCert.m_p );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: IssueCert() failed:  0x" << std::hex << nCommsRes << ".\n";
        else
        {
            nCommsRes = ptrServerCert->SetPrivateKey( privateKeyServer );
            if (SYM_FAILED(nCommsRes))
                std::cout << "ERROR: SetPrivateKey() after cert generation failed: 0x" << std::hex << nCommsRes << "\n";
        }

        // Issue a high ascii server cert.

        nCommsRes = ptrCertIssuer->SetValidityPeriod( SERVER_CERT_DEFAULT_LIFETIME ); // 10 years
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetValidityPeriod() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->SetRoles( roles_server, array_sizeof(roles_server) );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetRoles() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->SetBasicConstraints( false, 0 );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetBasicConstraints() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->SetIssuingCA( ptrSGRootCert );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: SetIssuingCA() failed:  0x" << std::hex << nCommsRes << ".\n";

        nCommsRes = ptrCertIssuer->IssueCert( 12345678, ptrCertSigningRequestServerHiAscii, ptrServerCertHiAscii.m_p );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: IssueCert() failed:  0x" << std::hex << nCommsRes << ".\n";
        else
        {
            nCommsRes = ptrServerCertHiAscii->SetPrivateKey( privateKeyServerHiAscii );
            if (SYM_FAILED(nCommsRes))
                std::cout << "ERROR: SetPrivateKey() after cert generation failed: 0x" << std::hex << nCommsRes << "\n";
        }

        // Test verification.

        nCommsRes = ptrSGRootCert->Verify( ptrServerCert );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: Verify() failed:  0x" << std::hex << nCommsRes << ".\n";
        else if (nCommsRes == RTN_FALSE )
            std::cout << "ERROR: Verify incorrectly returned false\n";

        nCommsRes = ptrLoginCACert->Verify( ptrServerCert );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: Verify() failed:  0x" << std::hex << nCommsRes << ".\n";
        else if (nCommsRes == RTN_OK )
            std::cout << "ERROR: Verify incorrectly returned true\n";

        nCommsRes = ptrSGRootCert->VerifyIssuer( ptrServerCert );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: VerifyIssuer() failed:  0x" << std::hex << nCommsRes << ".\n";
        else if (nCommsRes == RTN_FALSE )
            std::cout << "ERROR: VerifyIssuer incorrectly returned false\n";

        nCommsRes = ptrLoginCACert->VerifyIssuer( ptrServerCert );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: VerifyIssuer() failed:  0x" << std::hex << nCommsRes << ".\n";
        else if (nCommsRes == RTN_OK )
            std::cout << "ERROR: VerifyIssuer incorrectly returned true\n";

        // Now save some of them to disk.

        const char* szTempServerCertHiAsciiFileName = "c:\\tempservercert.hiascii.cer";
        const char* szTempServerCertFileName = "c:\\tempservercert.cer";
        const char* szTempServerKeyFileName = "c:\\tempserverkey.pvk";
        const char* szTempSGRootCertFileName = "c:\\tempsgrootcert.cer";

        if( privateKeyServer != NULL )
        {
            nCommsRes = privateKeyServer->Save( szTempServerKeyFileName );
            if (SYM_FAILED (nCommsRes))
                std::cout << "ERROR: Key save failure: 0x" << std::hex << nCommsRes << "\n";
        }

        if( ptrServerCert != NULL )
        {
            nCommsRes = ptrServerCert->Save( szTempServerCertFileName );
            if (SYM_FAILED (nCommsRes))
                std::cout << "ERROR: Cert save failure: 0x" << std::hex << nCommsRes << "\n";
        }

        if( ptrServerCertHiAscii != NULL )
        {
            nCommsRes = ptrServerCertHiAscii->Save( szTempServerCertHiAsciiFileName );
            if (SYM_FAILED (nCommsRes))
                std::cout << "ERROR: Cert save failure: 0x" << std::hex << nCommsRes << "\n";
        }

        if( ptrSGRootCert != NULL )
        {
            nCommsRes = ptrSGRootCert->Save( szTempSGRootCertFileName );
            if (SYM_FAILED (nCommsRes))
                std::cout << "ERROR: Cert save failure: 0x" << std::hex << nCommsRes << "\n";
        }

        remove( szTempServerKeyFileName );
        remove( szTempServerCertFileName );
        remove( szTempSGRootCertFileName );
    }

    // Do tests on the MessageManager cert store API.

    {
        IMessageManagerPtr ptrMsgMgr;

        if (SYM_SUCCEEDED (symres))
            symres = objMsgMgrLgr.CreateObject( &ptrMsgMgr );

        if (SYM_SUCCEEDED (symres))
        {
            nCommsRes = ptrMsgMgr->Initialize();
            if (SYM_FAILED (nCommsRes))
                std::cout << "ERROR: IMessageManager::Initialize() failed:  0x" << std::hex << nCommsRes << ".\n";

            if (SYM_SUCCEEDED (symres))
            {
                // Add and remove.

                nCommsRes = ptrMsgMgr->AddTrustedRootCertificate( ptrRootCert );
                if (SYM_FAILED (nCommsRes))
                    std::cout << "ERROR: IMessageManager::AddTrustedRootCertificate() failed:  0x" << std::hex << nCommsRes << ".\n";

                nCommsRes = ptrMsgMgr->AddTrustedRootCertificate( ptrLoginCACert );
                if (SYM_FAILED (nCommsRes))
                    std::cout << "ERROR: IMessageManager::AddTrustedRootCertificate() failed:  0x" << std::hex << nCommsRes << ".\n";

                nCommsRes = ptrMsgMgr->RemoveTrustedRootCertificate( ptrRootCert2 );
                if (SYM_FAILED (nCommsRes))
                    std::cout << "ERROR: IMessageManager::RemoveTrustedRootCertificate() failed:  0x" << std::hex << nCommsRes << ".\n";
                if (nCommsRes != RTN_OK)
                    std::cout << "ERROR: IMessageManager::RemoveTrustedRootCertificate() returned false when a delete should have ocurred.\n";

                // Set a list.

                ICert* trusted_roots[] =
                {
                    ptrRootCert,
                    ptrLoginCACert,
                    ptrServerCert // non-CA, this should fail
                };

                nCommsRes = ptrMsgMgr->SetTrustedRootCertificates( trusted_roots, array_sizeof(trusted_roots) );
                if (SYM_FAILED (nCommsRes) && nCommsRes != ScsSecureComms::RTNERR_CERTKEYERROR )
                    std::cout << "ERROR: IMessageManager::SetTrustedRootCertificates() failed:  0x" << std::hex << nCommsRes << ".\n";
                if (nCommsRes == RTN_OK)
                    std::cout << "ERROR: IMessageManager::SetTrustedRootCertificates() succeeded on non-CA cert.\n";

                // Try again without the server cert.

                nCommsRes = ptrMsgMgr->SetTrustedRootCertificates( trusted_roots, array_sizeof(trusted_roots) - 1 );
                if (SYM_FAILED (nCommsRes))
                    std::cout << "ERROR: IMessageManager::SetTrustedRootCertificates() failed:  0x" << std::hex << nCommsRes << ".\n";

                // Finally, clear the certs.

                nCommsRes = ptrMsgMgr->SetTrustedRootCertificates( NULL, 0 );
                if (SYM_FAILED (nCommsRes))
                    std::cout << "ERROR: IMessageManager::SetTrustedRootCertificates() failed:  0x" << std::hex << nCommsRes << ".\n";

                nCommsRes = ptrMsgMgr->RemoveTrustedRootCertificate( ptrRootCert2 );
                if (nCommsRes != RTN_FALSE)
                    std::cout << "ERROR: IMessageManager::RemoveTrustedRootCertificate() returned true when no delete should have ocurred.\n";

                // Tear down the MessageManager

                ptrMsgMgr->Deinitialize();
            }
        }
    }

    // Test cert save and load.

    ptrTempKey.Release();
    if (SYM_SUCCEEDED (symres))
        symres = objKeyLdr.CreateObject( &ptrTempKey );

    ptrTempCert.Release();
    if (SYM_SUCCEEDED (symres))
        symres = objCertLdr.CreateObject( &ptrTempCert );

    if (SYM_SUCCEEDED (symres))
    {
        const char* szTempCertFileName = "c:\\tempcert.cer";
        const char* szTempKeyFileName = "c:\\tempkey.pvk";

        nCommsRes = ptrRootCert->Save( szTempCertFileName );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: Cert save failure: 0x" << std::hex << nCommsRes << "\n";

        nCommsRes = ptrRootKey->Save( szTempKeyFileName );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: Key save failure: 0x" << std::hex << nCommsRes << "\n";

        // Load and test equality.

        nCommsRes = ptrTempCert->Load( szTempCertFileName );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: Cert load failure: 0x" << std::hex << nCommsRes << "\n";

        nCommsRes = ptrTempKey->Load( szTempKeyFileName );
        if (SYM_FAILED (nCommsRes))
            std::cout << "ERROR: Key load failure: 0x" << std::hex << nCommsRes << "\n";

        nCommsRes = ptrRootCert->IsEqual( ptrTempCert );
        if( nCommsRes != RTN_OK )
            std::cout << "ERROR: IsEqual() after save/load failed: 0x" << std::hex << nCommsRes << "\n";

        nCommsRes = ptrRootCert->SetPrivateKey( ptrTempKey );
        if (SYM_FAILED(nCommsRes))
            std::cout << "ERROR: SetPrivateKey() after save/load failed: 0x" << std::hex << nCommsRes << "\n";

        remove( szTempCertFileName );
        remove( szTempKeyFileName );
    }

    // Do some ICertVector testing.

    ICertVectorPtr ptrCertVector;

    if (SYM_SUCCEEDED (symres))
        symres = objCertVectorLdr.CreateObject( &ptrCertVector );

    if (SYM_SUCCEEDED (symres))
    {
        // Add

        nCommsRes = ptrCertVector->push_back( ptrLoginCACert );
        if( nCommsRes != RTN_OK )
            std::cout << "ERROR: ICertVector::push_back() failed: 0x" << std::hex << nCommsRes << "\n";

        nCommsRes = ptrCertVector->push_back( ptrRootCert );
        if( nCommsRes != RTN_OK )
            std::cout << "ERROR: ICertVector::push_back() failed: 0x" << std::hex << nCommsRes << "\n";

        // Get

        ptrTempCert.Attach( ptrCertVector->at(1) );
        if( ptrTempCert == NULL )
            std::cout << "ERROR: ICertVector::at() failed: no cert at index 1.\n";

        // Size

        if( ptrCertVector->size() != 2 )
            std::cout << "ERROR: ICertVector::size() failed: should be 2.\n";
    }

    return 0;
}

int main( int argc, char* argv[] )
{
    int tmpFlag   = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
    tmpFlag      |= _CRTDBG_LEAK_CHECK_DF;
//  tmpFlag      |= _CRTDBG_CHECK_CRT_DF;

    _CrtSetDbgFlag( tmpFlag );

    HMODULE scscomms = LoadLibrary( g_szPathToScsCommsDll );

    if( scscomms == NULL )
    {
        std::cout << "Failed to load ScsComms.dll from: " << g_szPathToScsCommsDll << std::endl;
        return( -1 );
    }

    int ret = app_test();

    return( ret );
}

