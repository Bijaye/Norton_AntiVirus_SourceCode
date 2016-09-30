// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include <screen.h>
// main.cpp

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

#include "winThreads.h"
#include "NTS_legacy.h" 

#ifdef NLM
extern "C" {
	#include <library.h>
	#include <netware.h>
	#include <event.h>
	#include <nks/netware.h>
}
	#include "libcnlm.h"
#endif

extern unsigned long noThreads;


// Global handle to the SSL-C library.

R_LIB_CTX*	g_SSLCContext;
int			g_SSLCContextLoadErr = 0;


// Global handle to the RSA Cert-C library.

CERTC_CTX	g_CertCContext = NULL;
int			g_CertCContextStatus = 0;


// Global cached GetFineLinearTime result at approx 100 msec resolution.

unsigned long g_ChunkLinearTime = GetFineLinearTime();


// These macros provide implementations for GetFactory and GetObjectCount.

SYM_OBJECT_MAP_BEGIN()
	 SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_MessageManager,	  MessageManager )
	 SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_Key,					  CKey )
	 SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_Cert,					  CCert )
	 SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_User,					  CUser )
	 SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_AuthLogin,			  CAuthLogin )
	 SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_AuthServer,			  CAuthServer )
	 SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_AuthRemote,			  CAuthRemote )
	 SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_CertSigningRequest, CCertSigningRequest )
	 SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_CertIssuer,			  CCertIssuer )
	 SYM_OBJECT_ENTRY( ScsSecureComms::SYMOBJECT_CertVector,			  CCertVector )

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

/*********************************************************************************************************************/

static unsigned long hUnloadEvent; 

// this block of function pointers must be provided by the loader - it is how I import symbols
// from the main program - sorry - no slick object-oriented stuff this late in the project

SYMNTS_FP_BLOCK SymNTS_FP_Block = { 0 };

extern "C" int SymScsCommsSetUnloadEvent() {

	// do an orderly shutdown if and only if I am being unloaded

	SetEvent( hUnloadEvent );
	return 0;
}

void main(int argc,char *argv[]) {

#ifdef NLM
    SYMNTS_FP_BLOCK *lpLoadBlock;

    // Initialize unload event.

	hUnloadEvent = CreateEvent(NULL,true,false,NULL);

	// Initialize Clib context broker.

	ClibContext::mk();

	// Rectify argv[0] if launched from system because of secure console.

	char *q,*p; int i;

	if (argc == 3 && ! strcmp( argv[1],"/SECURE_CONSOLE")) {
		argv[0] = argv[2]; argc -= 2;
	}
	if (argc == 4 && ! strcmp( argv[2],"/SECURE_CONSOLE")) {
		argv[0] = argv[3]; argc -= 2;
	}
	if (argc == 5 && ! strcmp( argv[3],"/SECURE_CONSOLE")) {
		argv[0] = argv[4]; argc -= 2;
	}
	if (argc == 6 && ! strcmp( argv[4],"/SECURE_CONSOLE")) {
		argv[0] = argv[5]; argc -= 2;
	}

	// check for the required command-line parameter telling us where the legacy NTS functions are in RTVSCAN/VPSTART
	
    if ( argc < 2 )
    {
		// they are required!
		
        exit( 1 );
    }
    else
    {
        int i;

		// locate the command-line argument I need
		
        for ( i = 1; i< argc; i++ )
        {
            char szLoadBlockTag[ ] = SYMNTS_COMMAND_LINE_TAG;

            if ( ! strncmp( argv[i], szLoadBlockTag, strlen( szLoadBlockTag ) ) )
            {
				// Pull out the address of the block, verify that it looks more or less like
				// I expect, and save it locally - proceeding with a pointer to something that
				// is somewhere else together, while possibly safe, is madness. It might even
				// have been on their stack!
				
                lpLoadBlock = ( SYMNTS_FP_BLOCK * ) atoi( argv[i] + strlen( szLoadBlockTag ) );

                if ( lpLoadBlock && 
                     lpLoadBlock->dwTag == SYMNTS_FP_BLOCK_TAG &&
                     lpLoadBlock->dwFPCount >= SYMNTS_FP_BLOCK_COUNT )
                {
                    // all ok

                    memcpy( &SymNTS_FP_Block, lpLoadBlock, sizeof( SymNTS_FP_Block ) );
                }
                else
                {
                    // the block is below rev or incorrect - leave!

                    exit( 1 );
                }

                break;
            }
        }
    }

	q = _tcschr (argv[0],'\\');
	p = _tcschr (argv[0],'/');
	q = q > p ? q : p;
	if (q) {
		*q = '\0'; strcpy(ScsCommsDir,argv[0]);
	} else strcpy(ScsCommsDir,NW_SYSTEM_DIR);

	q = _tcschr(ScsCommsDir,':');
	if (q) {
		if(q[1] != '\\') {
			if(q[1] == '/' )
				q[1] = '\\';
			else {
				for (i=0; i<IMAX_PATH; i++)
					if (ScsCommsDir[i] == '\0')
						break;
				memmove(q + 2,q + 1,i -(q - ScsCommsDir));
				q[1] = '\\'; i = 0;
			}
		}
	}
#endif

	// Initialize the legacy transport packet obfuscation seeds.

	SetSeedValues();

	// Do the initial load of settings.

	Config::GetInstance().Load( true );
	DebugPrinter::Instance().LoadSettings();

    DebugPrint( DEBUGFLAG_THREADS, "SCSComms starting up:\n" );
							
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
		g_SSLCContextLoadErr = SSLC_library_new(SSLC_get_default_resource_list(),
															 R_RES_FLAG_DEF,&g_SSLCContext);
#else
		g_SSLCContextLoadErr = SSLC_library_new(SSLC_get_small_resource_list(),
															 R_RES_FLAG_DEF,&g_SSLCContext);
#endif

		if( g_SSLCContextLoadErr == R_ERROR_NONE) {
			// Seed the SSL-C PRNG (random number generator).

			BYTE rand_seed[64]; GetRandomData(rand_seed, sizeof(rand_seed));
			R_rand_seed(R_rand_get_default(),rand_seed,sizeof(rand_seed));

			// Load error strings to use for profiling and debugging.

			SSL_load_error_strings();
		}
	}

    DebugPrint( DEBUGFLAG_THREADS, "SCSComms SSL init complete\n" );
														
	// Initialize the Cert-C library.

	{
		g_CertCContext = NULL;

		SERVICE_HANDLER service_providers[1];

		service_providers[0].type = SPT_CRYPTO;
		service_providers[0].name = "BSAFE Crypto-C";
		service_providers[0].Initialize = S_InitializeDefaultCSP;

		POINTER service_providers_params[1];
		service_providers_params[0] = NULL;

		g_CertCContextStatus =
			C_InitializeCertC(service_providers,service_providers_params,1,
									&g_CertCContext);
	}

    DebugPrint( DEBUGFLAG_THREADS, "SCSComms CERT init complete, waiting for close of business\n" );
															  
#ifdef NLM
    {
        ClibContext context;
        lpLoadBlock->ScsCommsIsLoaded=true;
    }
	// Wait for unload event from ScsCommsUtils' UnLoadScsComms.

    DebugPrint(DEBUGFLAG_THREADS, "SCSComms waiting for unload event to be signaled\n");
	WaitForSingleObject(hUnloadEvent,INFINITE); CloseHandle(hUnloadEvent);
    DebugPrint(DEBUGFLAG_THREADS, "SCSComms waiting for %d threads to shutdown\n", noThreads);
    while (noThreads > 0)
    {
        delay(100);
    }
    DebugPrint( DEBUGFLAG_THREADS, "SCSComms additional 5 second delay\n" );
    delay(5000);

	// Terminate ScsComms.

    DebugPrint( DEBUGFLAG_THREADS, "SCSComms close of business begins\n" );

	if( g_CertCContextStatus == 0 )
		{
         C_FinalizeCertC( &g_CertCContext );
		}

    DebugPrint( DEBUGFLAG_THREADS, "SCSComms C_FinalizeCertC - complete\n" );
	
	// Cleanup SSL-C

	if( g_SSLCContext != NULL )
		{
         DebugPrint( DEBUGFLAG_THREADS, "SCSComms calling SSLC_library_free\n" );
         int err = SSLC_library_free( g_SSLCContext );
         DebugPrint( DEBUGFLAG_THREADS, "SCSComms called SSLC_library_free\n" );

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

    DebugPrint( DEBUGFLAG_THREADS, "SCSComms SLL context release - complete\n" );
	
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

    DebugPrint( DEBUGFLAG_THREADS, "SCSComms leak checks complete\n" );
	
	// Cleanup the SSL-C locking.                                

	MessageManager::Deinitialize_SSLLocking();

    DebugPrint( DEBUGFLAG_THREADS, "SCSComms MessageManager::Deinitialize_SSLLocking complete\n" );
																 
	// Wait for user threads to terminate.

	DWORD nCount; HANDLE* pThreads;
	GetThreads(&nCount,&pThreads);
	
    DebugPrint( DEBUGFLAG_THREADS, "SCSComms %d threads remaining\n", nCount );

	if (nCount) {
		DebugPrint(DEBUGFLAG_ANY,"Waiting for user threads to end ...\n");

		switch (WaitForMultipleObjects(nCount,pThreads,true,30 * 1000)) {
		case WAIT_TIMEOUT: 
			DebugPrint(DEBUGFLAG_ANY,"Waiting for user threads to end ... timeout\n");
			break;
		case WAIT_FAILED: 
			DebugPrint(DEBUGFLAG_ANY,"Waiting for user threads to end ... failed\n");
			break;
		default:
			DebugPrint(DEBUGFLAG_ANY,"Waiting for user threads to end ... done\n");
		}
		delete[] pThreads;
	} else
		DebugPrint(DEBUGFLAG_ANY,"User threads have ended\n");
	
	// Terminate Clib context broker.

    DebugPrint( DEBUGFLAG_THREADS, "nuke ClibContext broker\n" );
	
    {
        ClibContext context;
        lpLoadBlock->ScsCommsIsUnloaded=true;
    }
	ClibContext::rm();

    Sleep( 1000 );
    DebugPrint( DEBUGFLAG_THREADS, "SCSComms close of business complete\n" );
	
#endif

	exit(0);
}

