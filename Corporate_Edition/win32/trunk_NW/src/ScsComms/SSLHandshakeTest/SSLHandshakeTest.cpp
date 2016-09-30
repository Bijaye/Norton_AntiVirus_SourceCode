// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SSLHandshakeTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <conio.h>
#include <iostream>

#include "random.h"
#include "vpstrutils.h"

BOOL initializeSocketsLib();
BOOL initializeSSLLib();
BOOL initializeSSLContext();
void infoCallback( SSL*, int, int );
void alertInfoCallback( SSL*, int, int, char* );
void wait();

//Number of times the SSL object has called us back
int callbackCount = 0;
//The step in the SSL handshake in which to intentionally fail
int failStep = 0;
//Handle is set in initializeSSLLib()
R_LIB_CTX* sslcLibrary = NULL;
//Conext is set in initializeSSLContext()
SSL_CTX* sslContext = NULL;

int main( int argc, char* argv[] )
{
	//host is of the form "<ip address>:<port>"
	char host[22];
	int ret = 0;
	BIO* socketBIO		    = NULL;
	SSL* ssl			    = NULL;
	unsigned long sleepTime = 0;

	//If we have 2 command line arguments, assume they are host and port
	if ( argc > 2 )
	{
		if ( ( strlen( argv[1] ) < 16 ) &&
			 ( strlen( argv[2] ) < 6 ) )
		{
			vpstrncpy    ( host, argv[1], sizeof (host) );
			vpstrnappend ( host, ":",     sizeof (host) );
			vpstrnappend ( host, argv[2], sizeof (host) );
		}
	}
	else
	{
		vpstrncpy ( host, "127.0.0.1:5100", sizeof (host) );
	}

	//If we have 3 command line arguments, assume the third is sleep time
	if ( argc > 3 )
	{
		sleepTime = strtoul( argv[3], NULL, 10 ) * 1000;
	}
	else
	{
		sleepTime = 1000;
	}

	//If we have 4 command line arguments, assume the fourth is the ssl step we want to fail in
	if ( argc > 4 )
	{
		failStep = static_cast<int>( strtoul( argv[4], NULL, 10 ) );

		//If the step we want to fail in isn't a valid step, set it to 0 (no fail)
		if ( 0 > failStep )
		{
			failStep = 0;
		}
	}

	//Initialize the sockets library, in this case Winsock
	if ( initializeSocketsLib() )
	{
		std::cout << "Initialized Sockets library." << std::endl;
	}
	else
	{
		std::cout << "ERROR: Failed to initialize the Sockets library." << std::endl;
		return 1;
	}

	//Initialize the SSL-C library
	if ( initializeSSLLib() )
	{
		std::cout << "Initialized SSL-C library." << std::endl;
	}
	else
	{
		std::cout << "ERROR: Failed to initialize the SSL-C library." << std::endl;
		return 1;
	}

	//Initialize the SSL context
	if ( initializeSSLContext() )
	{
		std::cout << "Initialized SSL context." << std::endl;
	}
	else
	{
		std::cout << "ERROR: Failed to initialize the SSL context." << std::endl;
		return 1;
	}

	while ( _kbhit() == 0 )
	{
		//Create the SSL connection structure
		if ( ( ssl = SSL_new( sslContext ) ) != NULL )
		{
			std::cout << "**************************" << std::endl;
			std::cout << "Created new SSL structure." << std::endl;
		}
		else
		{
			std::cout << "ERROR: Failed to create ssl structure." << std::endl;
			return 1;
		}

		//Set us as the connector
		SSL_set_connect_state( ssl );

		if ( ( socketBIO = BIO_new_connect( host ) ) != NULL )
		{
			std::cout << "Created new BIO connection." << std::endl << std::endl;
		}
		else
		{
			std::cout << "ERROR: Failed to create BIO connection." << std::endl;
			return 1;
		}

		//Associate the bio and the ssl structure
		SSL_set_bio( ssl, socketBIO, socketBIO );

		std::cout << "Starting SSL handshake..." << std::endl;
		//Let  'er rip
		ret = SSL_do_handshake( ssl );

		switch ( SSL_get_error( ssl, ret ) )
		{
			case SSL_ERROR_NONE:
				/* Handshake has finished successfully */
				std::cout << "SSL handshake complete." << std::endl;
				break;
			case SSL_ERROR_SSL:
				/* Handshake error - report and exit */
				std::cout << "Error during handshake: SSL_ERROR_SSL" << std::endl;
				break;
			case SSL_ERROR_SYSCALL:
				/* System call error. This error is different from
				 * the SSL_ERROR_SSL in that errno (under unix)
				 * has the numeric error value, and it is not
				 * converted into text.  If doing an SSL_read() or
				 * SSL_write() there is no recorded error in the error
				 * logging. This is because the error could be a
				 * 'retry' error of which the library is unaware.
				 */
				std::cout << "Error during handshake: SSL_ERROR_SYSCALL" << std::endl;
				break;
			case SSL_ERROR_WANT_READ:
				std::cout << "Error during handshake: SSL_ERROR_WANT_READ" << std::endl;
				break;
			case SSL_ERROR_WANT_WRITE:
				std::cout << "Error during handshake: SSL_ERROR_WANT_WRITE" << std::endl;
				break;
			case SSL_ERROR_WANT_CONNECT:
				/* Perform the handshake again. These errors are normally
				 * only reported when doing non-blocking I/O.
				 */
				std::cout << "Error during handshake: SSL_ERROR_WANT_CONNECT" << std::endl;
				break;
			case SSL_ERROR_ZERO_RETURN:
				/* A read(2)/write(2) system call returned 0 (usually
				 * because the socket was closed). If the socket is
				 * closed, the protocol has failed.
				 */
				std::cout << "Error during handshake: SSL_ERROR_ZERO_RETURN" << std::endl;
		}

		//Print out the chain
		std::cout << "Length of Certificate Chain: " << SSL_get_peer_cert_chain_count(ssl) << std::endl;
		std::cout << "Shutting down SSL connection..." << std::endl << std::endl;

		if ( SSL_shutdown( ssl ) < 1 )
		{
			std::cout << "ERROR: Failed to shutdown SSL connection." << std::endl << std::endl;
		}
		else
		{
			std::cout << "Shutdown SSL connection successfully." << std::endl << std::endl;
		}

		if ( ssl != NULL )
		{
			SSL_free( ssl );
			ssl = NULL;
		}

		//Reset the callback counter
		callbackCount = 0;
		std::cout << "Sleeping for " << sleepTime/1000 << " seconds..." << std::endl;
		Sleep( sleepTime );
	}

	if ( sslContext != NULL )
	{
		SSL_CTX_free( sslContext );
		sslContext = NULL;
	}

	if ( sslcLibrary != NULL )
	{
		SSLC_library_free( sslcLibrary );
		sslcLibrary = NULL;
	}

	std::cout << "Exiting...";

	return 0;
}

/**
 * Initialies the SSL-C Library
 */
BOOL initializeSSLLib()
{
    //Memory management callbacks.
/*    R_MEM_MALLOC_FUNCTION_T* ptr1  = NULL;
    R_MEM_REALLOC_FUNCTION_T* ptr2 = NULL;
    R_MEM_FREE_FUNCTION_T* ptr3    = NULL;

    R_get_mem_functions( &ptr1, &ptr2, &ptr3 );

    R_set_mem_functions( SSLCallback_Malloc,
                         SSLCallback_Realloc,
                         SSLCallback_Free );
*/
	
	//Load the library
	if( SSLC_library_new( SSLC_get_default_resource_list(), R_RES_FLAG_DEF, &sslcLibrary ) != R_ERROR_NONE )
	{
		return FALSE;
	}

    //Seed the SSL-C PRNG (random number generator).
    BYTE randomSeed[64];
    GetRandomData( randomSeed, sizeof( randomSeed ) );
    R_rand_seed( R_rand_get_default(), randomSeed, sizeof( randomSeed ) );

    //Load error strings to use for profiling and debugging.
    SSL_load_error_strings();

	return TRUE;
}

/**
 * Initialies the SSL Context with some defaults
 */
BOOL initializeSSLContext()
{
	SSL_METHOD* sslMethod = NULL;

	//Create the SSL method
	if ( ( sslMethod = SSLv3_client_method() ) == NULL )
	{
		std::cout << "Failed to create SSL Method." << std::endl;
		return FALSE;
	}

	//Create the SSL context, a factory for SSL connection structures
	if ( ( sslContext = SSL_CTX_new( sslMethod ) ) == NULL )
	{
		return FALSE;
	}

	//Set the cipher list to the default
	if ( ( SSL_CTX_set_cipher_list( sslContext, "RC4-SHA:EXP-RC4-56-SHA" ) ) == 0 )
	{
		std::cout << "Failed to set default cipher list." << std::endl;

		SSL_CTX_free( sslContext);
		sslContext = NULL;

		return FALSE;
	}

    //Set the context to do anytime shutdown.  This lets us initiate the SSL close
    //handshake even if the init handshake hasn't completed yet.
    SSL_CTX_set_anytime_shutdown( sslContext, 1 );

	//Set the blinding mode to full with update
	SSL_CTX_set_rsa_blinding_mode( sslContext, 0x02 );

	//Setup the info and alert info callbacks
    SSL_CTX_set_info_cb( sslContext, infoCallback );
    SSL_CTX_set_alert_info_cb( sslContext, alertInfoCallback, NULL );

	return TRUE;
}

/**
 * Initialies the Windows Socket library
 */
BOOL initializeSocketsLib()
{
	WSADATA wsa_data = {0};

	if ( ( WSAStartup( MAKEWORD( 1 /* major */, 1 /* minor */ ), &wsa_data ) ) != 0 )
	{
		return FALSE;
	}

	return TRUE;
}

/**
 * Callback that is called at each stage in the SSL handshake
 */
void infoCallback( SSL* ssl, int where, int ret )
{
	std::cout << "INFO: Handshake info callback:" << std::endl;
	std::cout << "\twhere        = " << where						 << std::endl;
	std::cout << "\tret          = " << ret						     << std::endl;
	std::cout << "\tstate string = " << SSL_state_string_long( ssl ) << std::endl;
	std::cout << "\tstate int    = " << SSL_state( ssl )			 << std::endl << std::endl;

	//If failStep is non-zero, we are going to fail intentionally at a step
	if ( failStep != 0 )
	{
		++callbackCount;

		//If we are at the failstep, then fail
		if ( callbackCount == failStep )
		{
			throw 'f';
		}
	}
}

/**
 * Callback that is called when an SSL alert occurs
 */
void alertInfoCallback( SSL* ssl, int where, int ret, char* arg )
{
	std::cout << "ALERT: Handshake alert info callback:" << std::endl;
	std::cout << "\twhere        = " << where						 << std::endl;
	std::cout << "\tret          = " << ret						     << std::endl;
	std::cout << "\tstate string = " << SSL_state_string_long( ssl ) << std::endl;
	std::cout << "\tstate int    = " << SSL_state( ssl )		     << std::endl << std::endl;
}

void wait()
{
	char input = getch();
}