// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

// Random.cpp

#include "stdafx.h"
#include "Random.h"
//#include "DebugPrint.h"

#ifdef _WINDOWS

// The Windows implementation calls the CryptGenRandom function in the Crypto API

void GetRandomData( BYTE* buffer, unsigned long buffer_length )
{
    BOOL good_random = FALSE;

    // The best random data available to us is through the Window cryptographic provider APIs.

    HCRYPTPROV provider = NULL;

    DWORD provider_type = PROV_RSA_FULL; // The Secure Coding 2 book uses this in its example, not sure why ...
    DWORD provider_flags = CRYPT_VERIFYCONTEXT | CRYPT_SILENT;

    if( (good_random = CryptAcquireContext( &provider,
                                            NULL,
                                            NULL,
                                            provider_type,
                                            provider_flags )) != FALSE )
    {
        if( (good_random = CryptGenRandom( provider, buffer_length, buffer )) == FALSE )
        {
            //DebugPrint( DEBUGFLAG_SSL_ERROR, "ENTROPY warn: CryptGenRandom failed: %lu\n", GetLastError() );
        }

        CryptReleaseContext( provider, 0 );
    }
    else
    {
        //DebugPrint( DEBUGFLAG_SSL_ERROR,
        //            "ENTROPY warn: CryptAcquireContext for prov type %lu with flags 0x%08X failed: %lu\n",
        //            provider_type,
        //            provider_flags,
        //            GetLastError() );
    }

    if( good_random == FALSE )
    {
        // Use poor random data instead.

        srand( time(NULL) | (GetCurrentThreadId() << 16) || GetCurrentProcessId() );

        while( buffer_length > 0 )
        {
            // C-Run Time's rand generally produces values less than 16-bits,
            // so we'll just do this one byte at a time.

            *buffer = static_cast<BYTE>( rand() & 0x000000FF );

            buffer_length--;
            buffer++;
        }
    }
}


#endif // _WINDOWS

