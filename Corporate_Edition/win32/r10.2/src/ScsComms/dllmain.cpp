// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

// dllmain.cpp

// INITIIDS defines GUIDs for SYM interfaces and objects.
// It's used here to provide a central place for defining
// GUIDs for objects defined in this module.
#define INITIIDS

#include "stdafx.h"

#pragma warning( disable : 4530 ) // try catch used when exceptions disabled
#pragma warning( disable : 4786 ) // debug identifier truncated to less than 255 chars


#include "SymInterface.h"
#include "MessageManager.h"
#include "Key.h"
#include "certobj.h"
#include "user.h"
#include "auth.h"
#include "authlogin.h"
#include "authserver.h"
#include "authremote.h"
#include "CertSigningRequest.h"
#include "CertIssuer.h"
#include "DebugPrint.h"
#include "cript.h"
#include "Random.h"
#include "CertVector.h"
#include "Config.h"

// Global handle to the SSL-C library.

R_LIB_CTX*  g_SSLCContext;
int         g_SSLCContextLoadErr = 0;


// Global handle to the RSA Cert-C library.

CERTC_CTX   g_CertCContext = NULL;
int         g_CertCContextStatus = 0;


// Global cached GetFineLinearTime result at approx 100 msec resolution.

unsigned long g_ChunkLinearTime = GetFineLinearTime();


// These macros provide implementations for GetFactory and GetObjectCount.

SYM_OBJECT_MAP_BEGIN()
    SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_MessageManager,     MessageManager )
    SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_Key,                CKey )
    SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_Cert,               CCert )
    SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_User,               CUser )
    SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_AuthLogin,          CAuthLogin )
    SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_AuthServer,         CAuthServer )
    SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_AuthRemote,         CAuthRemote )
    SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_CertSigningRequest, CCertSigningRequest )
    SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_CertIssuer,         CCertIssuer )
    SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_CertVector,         CCertVector )

	if( SymIsEqualIID( objectID, ScsSecureComms::SYMOBJECT_MessageBuffer ) )
	{
		MessageBufferFactory* pFactory = NULL;
		try
		{
			pFactory = new MessageBufferFactory;
		}
		SYMINTCATCHMEMORYEXECPTION()
		if( pFactory == NULL )
			return SYMERR_OUTOFMEMORY;
		*ppvFactory = pFactory;
		pFactory->AddRef();
		return SYM_OK;
	}

SYM_OBJECT_MAP_END()


// DllMain -- module entry point

BOOL APIENTRY DllMain( HANDLE module, DWORD reason, LPVOID reserved )
{
    UNREFERENCED_PARAMETER(reserved);
    UNREFERENCED_PARAMETER(module);

    switch( reason )
    {
    case DLL_PROCESS_DETACH :

        // Cleanup Cert-C.

        if( g_CertCContextStatus == 0 )
        {
            C_FinalizeCertC( &g_CertCContext );
        }

        // Cleanup SSL-C

        if( g_SSLCContext != NULL )
        {
            int err = SSLC_library_free( g_SSLCContext );

            if( err != R_ERROR_NONE )
            {
                // Print our the error for diagnostic purposes.

                DebugPrint( DEBUGFLAG_SSL_ERROR, "SSLC_library_free failed: %d\n", err );
            }

            // Free the error strings that we loaded for tracing.

            // The RSA docs don't explicitly say whether or not you have to do this,
            // but it seems to cut down on memory leaks.

            ERR_free_strings();

            // Deallocate all error stacks managed by the library for this process.

            ERR_remove_state( 0 );

            g_SSLCContext = NULL;
        }

        // Sanity check to make sure that we aren't leaking memory.

        if( ConnectionContext::ms_ConnectionCount != 0 )
            DebugPrint( DEBUGFLAG_ANY, "LEAK: %lu %s objects unfreed.\n", ConnectionContext::ms_ConnectionCount, "ConnectionContext" );
        if( CKey::ms_KeyCount != 0 )
            DebugPrint( DEBUGFLAG_ANY, "LEAK: %lu %s objects unfreed.\n", CKey::ms_KeyCount, "CKey" );
        if( CAuthLogin::ms_AuthLoginCount != 0 )
            DebugPrint( DEBUGFLAG_ANY, "LEAK: %lu %s objects unfreed.\n", CAuthLogin::ms_AuthLoginCount, "CAuthLogin" );
        if( CAuthRemote::ms_AuthRemoteCount != 0 )
            DebugPrint( DEBUGFLAG_ANY, "LEAK: %lu %s objects unfreed.\n", CAuthRemote::ms_AuthRemoteCount, "CAuthRemote" );
        if( CAuthServer::ms_AuthServerCount != 0 )
            DebugPrint( DEBUGFLAG_ANY, "LEAK: %lu %s objects unfreed.\n", CAuthServer::ms_AuthServerCount, "CAuthServer" );
        if( CCert::ms_CertCount != 0 )
            DebugPrint( DEBUGFLAG_ANY, "LEAK: %lu %s objects unfreed.\n", CCert::ms_CertCount, "CCert" );
        if( CCertSigningRequest::ms_CertSigningRequestCount != 0 )
            DebugPrint( DEBUGFLAG_ANY, "LEAK: %lu %s objects unfreed.\n", CCertSigningRequest::ms_CertSigningRequestCount, "CCertSigningRequest" );

        SAVASSERT( ConnectionContext::ms_ConnectionCount == 0 );
        SAVASSERT( CKey::ms_KeyCount == 0 );
        SAVASSERT( CAuthLogin::ms_AuthLoginCount == 0 );
        SAVASSERT( CAuthRemote::ms_AuthRemoteCount == 0 );
        SAVASSERT( CAuthServer::ms_AuthServerCount == 0 );
        SAVASSERT( CCert::ms_CertCount == 0 );
        SAVASSERT( CCertSigningRequest::ms_CertSigningRequestCount == 0 );

        // Cleanup the SSL-C locking.

        MessageManager::Deinitialize_SSLLocking();

        break;

    case DLL_PROCESS_ATTACH :

        // Initialize the legacy transport packet obfuscation seeds.

        SetSeedValues();

        // Do the initial load of settings.

        Config::GetInstance().Load( true );
        DebugPrinter::Instance().LoadSettings();

        // Initialize the SSL-C library.

        {
            // Set SSL-C memory management callbacks.
    
            MessageManager::Initialize_SSLMemoryMgmt();

            // Initalize the SSL-C locking structures..

            MessageManager::Initialize_SSLLocking();

            // *WORK* Initialize the library with just the cipher support we need and no more,
            // this allows us to limit the size of the code that gets linked in and might
            // be required to comply with export restrictions.

#ifndef SSLC_SMALL_CODE

            g_SSLCContextLoadErr = SSLC_library_new( SSLC_get_default_resource_list(), R_RES_FLAG_DEF, &g_SSLCContext );

#else

            g_SSLCContextLoadErr = SSLC_library_new( SSLC_get_small_resource_list(), R_RES_FLAG_DEF, &g_SSLCContext );

#endif

            if( g_SSLCContextLoadErr == R_ERROR_NONE )
            {
                // Seed the SSL-C PRNG (random number generator).

                BYTE rand_seed[64];

                GetRandomData( rand_seed, sizeof(rand_seed) );

                R_rand_seed( R_rand_get_default(), rand_seed, sizeof(rand_seed) );

                // Load error strings to use for profiling and debugging.

                SSL_load_error_strings();
            }
        }

        // Initialize the Cert-C library.

        {
            g_CertCContext = NULL;

            SERVICE_HANDLER service_providers[1];

            service_providers[0].type = SPT_CRYPTO;
            service_providers[0].name = "BSAFE Crypto-C";
            service_providers[0].Initialize = S_InitializeDefaultCSP;

            POINTER service_providers_params[1];

            service_providers_params[0] = NULL;

            g_CertCContextStatus = C_InitializeCertC( service_providers, service_providers_params, 1, &g_CertCContext );
        }

        break;
    }

    return( TRUE );
}

