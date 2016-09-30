// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998, 2005 Symantec Corporation. . All rights reserved.
//*************************************************************************
// sockets.cpp - created 12/22/98 1:57:15 PM
//
// $Header$
//
// Description:
//
//*************************************************************************
// $Log$
//*************************************************************************

#include "StdAfx.h"
#include "qsprotocol.h"
#include "qscomm.h"

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#if defined(WIN32)
#include <io.h>
#include <sys\stat.h>
#include "wsipx.h"
#include <new>
#endif

#if defined(UNIX)

#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#if defined(SOLARIS)
#include <sys/filio.h>
#elif defined(LINUX)
#include <asm/ioctls.h>
#include <rpc/types.h>
#endif
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
// WinSock-compatible wrappers for portability

typedef int SOCKET;

#define INVALID_SOCKET  (-1)
#define SOCKET_ERROR    (-1)
#define IPPROTO_IP      0
#define closesocket     close
#define ioctlsocket     ioctl
//#define INADDR_ANY      0x00000000UL
#if defined(SOLARIS)
#define INADDR_NONE     0xffffffff
#endif

#endif

#include "qsendian.h"


///////////////////////////////////////////////////////////////////////////////
// Local functions.
static int SendViaIP( int iFile, const char * pszServerAddress, int iPort, LPFNXFERCALLBACK pCallback );
#if defined(WIN32)
static int SendViaSPX( int iFile, const char * pszServerAddress, int iPort, LPFNXFERCALLBACK pCallback );
#endif
static int SendChunks( SOCKET s, int iFile, LPFNXFERCALLBACK pCallback );
#if defined(WIN32)
static BOOL FillIpxAddress( SOCKADDR_IPX *psa, const char * lpsAddress, int iEndpoint );
static unsigned char BtoH( char ch );
static void AtoH( char *szDest, const char *szSource, int iCount );
#endif
static BOOL IsIpAddress( const char * pszAddress );
static int GetServerStatus( SOCKET s );

///////////////////////////////////////////////////////////////////////////////
// Local data.

///////////////////////////////////////////////////////////////////////////////
// Local Macros.



///////////////////////////////////////////////////////////////////////////////
//
// Function name : SendFileToQServer
//
// Description   : This routine will send a file to a specified Quarantine 
//                 server.
//
// Return type   : int - return value indicates success or error codes.  See
//                 qscomm.h for error codes.
//
// Argument      : const char* pszSourceFile - File to send to server.
// Argument      : const char* pszServerAddress - Address of server to send file to.
// Argument      : int iProtocol - protocol to use.
// Argument      : int iPort - port to connect to.
// Argument      : LPFNXFERCALLBACK pCallback - User supplied callback function.  
//                 Can be NULL.
//
///////////////////////////////////////////////////////////////////////////////
// 12/22/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
int SendFileToQServer(  const char* pszSourceFile,
                        const char* pszServerAddress,
                        int iProtocol,
                        int iPort,
                        LPFNXFERCALLBACK pCallback )
{
    // 
    // Open file for reading
    // 
#if defined(WIN32)
    int iFile = _open( pszSourceFile, _O_BINARY | _O_RDONLY );
#else
    int iFile = open( pszSourceFile, O_RDONLY );
#endif
    if( iFile == -1 )
        {
        return QSCOMM_ERROR_FILE_NOT_FOUND;
        }

    // 
    // Send using specified protocol
    // 
    int iRet;
    switch( iProtocol )
        {
        
        case QSCOMM_PROTOCOL_IP:
            iRet = SendViaIP( iFile, pszServerAddress, iPort, pCallback );
            break;
        
#if defined(WIN32)
        case QSCOMM_PROTOCOL_SPX:
            iRet = SendViaSPX( iFile, pszServerAddress, iPort, pCallback );
            break;
#endif

        default:
            iRet = QSCOMM_ERROR_UNKNOWN_PROTOCOL;
        }

    // 
    // Cleanup.
    // 
#if defined(WIN32)
    _close( iFile );
#else
    close( iFile );
#endif
    
    return iRet;
}




///////////////////////////////////////////////////////////////////////////////
//
// Function name : SendViaIP
//
// Description   : This routine sets up an IP connection and sends a file to 
//                 the quarantine server.
//
// Return type   : int 
//
// Argument      : FILE* file - Open file to send to 
// Argument      : const char * pszServerAddress
// Argument      : int iPort
//
///////////////////////////////////////////////////////////////////////////////
// 12/22/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
int SendViaIP( int iFile, const char * pszServerAddress, int iPort,  LPFNXFERCALLBACK pCallback )
{

    // 
    // Create a socket.
    // 
    SOCKET s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( INVALID_SOCKET == s )
        return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
//_asm int 3;
    // 
    // Create and populate server address structure.
    // 
    sockaddr server;
    sockaddr_in* pServer = (sockaddr_in*) &server;
    memset( &server, 0, sizeof( sockaddr ) );
    pServer->sin_family = PF_INET;

    // This should be "pServer->sin_port = htons(iPort)" on all
    // platforms, but for legacy reasons we must always store a
    // little-endian port value into the structure instead.
    // Unfortunately this means the port number configured in the
    // UI is NOT the port that is actually used.  If the user 
    // enters port 4455, the actual port used is 26385
    // (4455 == 0x1167h, but 0x6711 == 26385).
    pServer->sin_port = WENDIAN(iPort);

    // 
    // Translate address if neccessary.
    // 
    if( IsIpAddress( pszServerAddress ) )
        {
        // 
        // Convert x.x.x.x address to IP address
        // 
        pServer->sin_addr.S_un.S_addr = inet_addr( pszServerAddress );
        if( pServer->sin_addr.S_un.S_addr == INADDR_NONE )
            {
            closesocket( s );
            return QSCOMM_ERROR_SERVER_NOT_FOUND;
            }
        }
    else
        {
        // 
        // Try looking up server using DNS.
        // 
	// Modified to use gethostbyname_r() on Solaris for
	// thread safety - C. Halpern 2/2/01

#if defined(SOLARIS)
#define HOST_BUF_LEN  2048

        struct hostent host_entry;
	char host_buffer[HOST_BUF_LEN];
	int errnum = 0; 

        struct hostent *hp = 
	    gethostbyname_r( pszServerAddress, &host_entry, host_buffer, 
			     sizeof(host_buffer), &errnum );

        if( hp == NULL || errnum != 0)
#else
        struct hostent *hp = gethostbyname( pszServerAddress );
        if( hp == NULL )
#endif
            {
            closesocket( s );
            return QSCOMM_ERROR_SERVER_NOT_FOUND;
            }
        else
            {
            // 
            // Copy resolved host address to server address block
            // 
            memcpy( &pServer->sin_addr, hp->h_addr, hp->h_length );
            }
        }
    
 
    // 
    // Connect to server
    // 
    errno = 0;
    int err = connect( s, &server, sizeof( sockaddr ) );
    if (err != 0)
        {
        err = errno;
        closesocket( s );
        return QSCOMM_ERROR_SERVER_NOT_FOUND;
        }

    // 
    // Send file to server
    // 
    int iRet = SendChunks( s, iFile, pCallback );

    // 
    // Cleanup.
    // 
    shutdown( s, 0 );
    closesocket( s );
    return iRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : SendViaSPX
//
// Description   : Send a file to a Quarantine Server via the SPX protocol over
//                 winsock.
//
// Return type   : int 
//
// Argument      : int iFile
// Argument      : const char * pszServerAddress
// Argument      : int iPort
// Argument      : LPFNXFERCALLBACK pCallback
//
///////////////////////////////////////////////////////////////////////////////
// 12/23/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
#if defined(WIN32)
int SendViaSPX( int iFile, const char * pszServerAddress, int iPort, LPFNXFERCALLBACK pCallback )
{
    // 
    // Create a socket.
    // 
    SOCKET s = socket( AF_IPX, SOCK_STREAM, NSPROTO_SPXII );
    if( INVALID_SOCKET == s )
        return QSCOMM_ERROR_SERVER_SOCKET_ERROR;

    // 
    // Create and populate server address structure.
    // 
    SOCKADDR_IPX server;
    if( FALSE == FillIpxAddress( &server, pszServerAddress, iPort ) ) 
        {
        closesocket( s );
        return QSCOMM_ERROR_SERVER_NOT_FOUND;
        }

    // 
    // Connect to server
    // 
    if( connect( s, (sockaddr*)&server, sizeof( SOCKADDR_IPX ) ) != 0)
        {
        closesocket( s );
        return QSCOMM_ERROR_SERVER_NOT_FOUND;
        }

    // 
    // Send file to server
    // 
    int iRet = SendChunks( s, iFile, pCallback );

    // 
    // Cleanup.
    // 
    shutdown( s, 0 );
    closesocket( s );
    return iRet;
}
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Function name : SendChunks
//
// Description   : This routine does the real work of sending chunks of file to
//                 the quarantine server.
//
// Return type   : int 
//
// Argument      :  SOCKET s
// Argument      : int iFile
// Argument      : LPFNXFERCALLBACK pCallback
//
///////////////////////////////////////////////////////////////////////////////
// 12/22/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
int SendChunks( SOCKET s, int iFile, LPFNXFERCALLBACK pCallback )
{

    FILESIZESTRUCT fs;
#if defined(WIN32)
    struct _stat stats;
#else
    struct stat stats;
#endif
    DWORD dwTotalSent = 0, dwBytesRemaining;
    int iBytesRead;
    char * pBuffer;
    int iRet = QSCOMM_SUCCESS;

    // 
    // Get file size.
    // 
#if defined(WIN32)
    if( -1 == _fstat( iFile, &stats ) )
#else
    if( -1 == fstat( iFile, &stats ) )
#endif
        {
        return QSCOMM_ERROR_FILE_NOT_FOUND;
        }
    
    fs.dwFileSize = (DWORD) DWENDIAN( stats.st_size );
    fs.dwSignature = DWENDIAN( FILE_SIZE_SIGNATURE );

    // 
    // Tell server how many bytes we'll be sending.
    // 
    if( send( s, (char*)&fs, sizeof( FILESIZESTRUCT ), 0 ) == SOCKET_ERROR )
        {
        return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
        }

    // 
    // Wait here for server to tell us to begin.
    // 
    DWORD dwServerStatus;
    if( recv( s, (char*)&dwServerStatus, sizeof( DWORD ), 0 ) == SOCKET_ERROR )
        {
        return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
        }

    // 
    // Make sure the server is ready for us.
    //
    dwServerStatus = DWENDIAN( dwServerStatus );
    switch( dwServerStatus )
        {
        case BEGIN_XFER: break;
        case SERVER_ERROR_BUSY: return QSCOMM_ERROR_SERVER_BUSY;
        case SERVER_ERROR_DISK_SPACE: return QSCOMM_ERROR_SERVER_FULL;
        default:
            return QSCOMM_ERROR_SERVER_UNKNOWN_ERROR;
        }

    // 
    // Tell client that we are ready to begin
    // 
    if( pCallback != NULL && pCallback( CALLBACK_BEGIN, 0, 0 ) == FALSE ) 
        return QSCOMM_USER_ABORT;

    // 
    // Allocate 32k transfer buffer.
    // 
    pBuffer = new char[ 0x7FFF ];
    if( pBuffer == NULL )
        {
        return QSCOMM_ERROR_OUT_OF_MEMORY;    
        }

    // 
    // Send data.
    // 
    dwBytesRemaining = stats.st_size;
    while( dwBytesRemaining )
        {
        // 
        // Read a chunk.
        // 
#if defined(WIN32)
        iBytesRead = _read( iFile, pBuffer, 0x7FFF );
#else
        iBytesRead = read( iFile, pBuffer, 0x7FFF );
#endif
        if( iBytesRead == -1 )
            {
            iRet = QSCOMM_ERROR_SERVER_UNKNOWN_ERROR;
            break;
            }

        // 
        // Check to see if the server is trying to tell us something
        // 
        iRet = GetServerStatus( s );
        if( iRet != QSCOMM_SUCCESS )
            break;

        // 
        // Send a chunk.
        // 
        if( send( s, pBuffer, iBytesRead, 0 ) == SOCKET_ERROR )
            {
            // 
            // See if there is an error waiting for us to get.
            // 
            iRet = GetServerStatus( s );
            
            // 
            // If there is no error status waiting for us, we still need to 
            // report an error.
            // 
            if( iRet == QSCOMM_SUCCESS )
                iRet = QSCOMM_ERROR_SERVER_SOCKET_ERROR;

            break;
            }

        // 
        // Adjust counters.
        // 
        dwBytesRemaining -= (DWORD) iBytesRead;
        dwTotalSent += (DWORD) iBytesRead;

        // 
        // Tell client what happened
        // 
        if( pCallback != NULL && pCallback( CALLBACK_STATUS, dwTotalSent, dwBytesRemaining ) == FALSE ) 
            {
            iRet = QSCOMM_USER_ABORT;
            break;
            }
        }

    // 
    // Cleanup
    // 
    delete [] pBuffer;

    // 
    // If something weird happened, or the user aborted, bail out now.
    // 
    if( iRet != QSCOMM_SUCCESS )
        return iRet;

    // 
    // Retrieve status from server.
    // 
    if( recv( s, (char*)&dwServerStatus, sizeof( DWORD ), 0 ) == SOCKET_ERROR )
        {
        iRet = QSCOMM_ERROR_SERVER_SOCKET_ERROR;
        }
    else
        {
        // 
        // Translate status values
        // 
        dwServerStatus = DWENDIAN( dwServerStatus );
        switch( dwServerStatus )
            {
            case DONE_XFER: 
                // 
                // Tell client what happened
                // 
                if( pCallback != NULL )
                    pCallback( CALLBACK_END, 0, 0 );
                break;
            case SERVER_ERROR: iRet = QSCOMM_ERROR_SERVER_UNKNOWN_ERROR; break;
            case SERVER_ERROR_DISK_SPACE: iRet = QSCOMM_ERROR_SERVER_FULL; break;
            case SERVER_ERROR_SOCKET: iRet = QSCOMM_ERROR_SERVER_SOCKET_ERROR; break;
            default:
                iRet = QSCOMM_ERROR_SERVER_UNKNOWN_ERROR;
            }
        }

    // 
    // Return final status.
    // 
    return iRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : FillIpxAddress
//
// Description   : Initialize an IPX socket address structure.
//
// Return type   : BOOL 
//
// Argument      : SOCKADDR_IPX *psa
// Argument      : LPSTR lpsAddress
// Argument      : int iEndpoint
//
///////////////////////////////////////////////////////////////////////////////
// 12/23/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
#if defined(WIN32)
BOOL FillIpxAddress( SOCKADDR_IPX *psa,  
                     const char* lpsAddress,
                     int iEndpoint ) 
{ 
    // Location of the separator 
    char* pszPoint;                  
    
    memset ( psa, 0, sizeof ( SOCKADDR_IPX ) ); 
    
    psa->sa_family = AF_IPX; 
    
    // 
    // Check if an address is specified 
    // 
    if ( NULL != lpsAddress ) 
        { 
        // 
        // Get the offset for node number/network number separator 
        // 
        pszPoint = strchr ( lpsAddress, '.' ); 
        
        if ( NULL == pszPoint ) 
            {
            return FALSE;
            } 
        
        // 
        // covert the address in the  string format to binary format 
        // 
        AtoH ( (CHAR *) psa->sa_netnum, lpsAddress, 4 ); 
        AtoH ( (CHAR *) psa->sa_nodenum, pszPoint + 1, 6 ); 
        
        } 
    // 
    // Save off endpoint
    // 
    psa->sa_socket = iEndpoint; 
    return TRUE;
} 
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Function name : AtoH
//
// Description   : AtoH () coverts the IPX address specified in the string(ascii) 
//                 format to the binary(hexadecimal) format.                                           
//
// Return type   : void 
//
// Argument      :  char *szDest
// Argument      : char *szSource
// Argument      : int iCount
//
///////////////////////////////////////////////////////////////////////////////
// 12/23/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
#if defined(WIN32)
void AtoH( char *szDest, const char *szSource, int iCount ) 
{ 
    while (iCount--) 
        { 
        *szDest++ = ( BtoH ( *szSource++ ) << 4 ) + BtoH ( *szSource++ ); 
        } 
} 
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Function name : BtoH
//
// Description   : BtoH () returns the equivalent binary value for an individual 
//                 character specified in the ascii format.                      
//
// Return type   : unsigned char 
//
// Argument      : char ch
//
///////////////////////////////////////////////////////////////////////////////
// 12/23/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
#if defined(WIN32)
unsigned char BtoH( char ch ) 
{ 
 
    if ( ch >= '0' && ch <= '9' ) 
        { 
        return ( ch - '0' ); 
        } 
 
    if ( ch >= 'A' && ch <= 'F' ) 
        { 
        return ( ch - 'A' + 0xA ); 
        } 
 
    if (ch >= 'a' && ch <= 'f' ) 
        { 
        return ( ch - 'a' + 0xA ); 
        } 

    // 
    // Should never get here.
    // 
    return 0;
}
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Function name : IsIpAddress
//
// Description   : Returns TRUE if the specified address looks like a valid IP 
//                 address. (xxx.xxx.xxx.xxx).
//
// Return type   : BOOL 
//
// Argument      : const char * pszAddress
//
///////////////////////////////////////////////////////////////////////////////
// 12/28/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL IsIpAddress( const char * pszAddress )
{
    char szAddress[ 17 ];
    int iAddressComponents = 4;
    int iLength, i;

    // 
    // Make sure no one is messing with us.
    // 
    if( strstr( pszAddress, ".." ) != NULL ||
        strlen( pszAddress ) > 15 )
        return FALSE;
    
    // 
    // Parse string
    //
    // Modification 2/1/2001 By C. Halpern
    // Use strtok_r() for Thread Safety instead of strtok() on Solaris
    // New Variable tok_ptr introduced.
    // 


    strncpy( szAddress, pszAddress, 17 );

#if defined(SOLARIS)
    char *tok_ptr;
    char *p = strtok_r( szAddress, ".", &tok_ptr );
#else
    char *p = strtok( szAddress, "." );
#endif

    while( p != NULL && iAddressComponents )
        {
        // 
        // Check length.
        // 
        iLength = strlen( p );
        if( iLength > 3 )
            return FALSE;
        
        // 
        // Make sure all characters are digits
        // 
        for( i = 0; i < iLength; i++ )
            {
            if( !isdigit( p[i] ) )
                return FALSE;
            }
        
        // 
        // Make sure ranges are correct.
        // 
        if( atoi( p ) > 255 )
            return FALSE;
        
        // 
        // Get next token.
        // 
        iAddressComponents --;        
#if defined(SOLARIS)
        p = strtok_r( NULL, ".", &tok_ptr );
#else
        p = strtok( NULL, "." );
#endif
        }

    // 
    // If we have found 4 address components, and there are no more
    // we have a valid IP address.
    // 
    if( iAddressComponents == 0 && p == NULL )
        return TRUE;

    // 
    // Valid IP address not found.
    // 
    return FALSE;
    
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : GetServerStatus
//
// Description   : This routine will examine a give socket looking for an
//                 error status code from the quarantine server.
//
// Return type   : int QSCOMM_SUCCESS if no error code found
//                     QSCOMM_ERROR_XXXX on error
//
// Argument      : SOCKET s
//
///////////////////////////////////////////////////////////////////////////////
// 1/19/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
int GetServerStatus( SOCKET s )
{
    fd_set  fdset;
    struct timeval stTimeout = { 0, 0 };
    DWORD dwServerStatus = 0;

    // 
    // Query status of this socket.
    // 
    FD_ZERO( &fdset );
    FD_SET( s, &fdset );
    int iSelectRet = select( 0, &fdset, NULL, NULL, &stTimeout );
    if( iSelectRet == SOCKET_ERROR )
        {
        return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
        }
    else if( 1 == iSelectRet )
        {
        // 
        // Retrieve status from server.
        // 
        if( recv( s, (char*)&dwServerStatus, sizeof( DWORD ), 0 ) == SOCKET_ERROR )
            {
            return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
            }
        else
            {
            // 
            // Translate status values
            // 
            dwServerStatus = DWENDIAN( dwServerStatus );
            switch( dwServerStatus )
                {
                case SERVER_ERROR: return QSCOMM_ERROR_SERVER_UNKNOWN_ERROR;
                case SERVER_ERROR_DISK_SPACE: return QSCOMM_ERROR_SERVER_FULL; 
                case SERVER_ERROR_SOCKET: return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
                case SERVER_ERROR_BOGUS_FILE: return QSCOMM_ERROR_BOGUS_FILE;
                default:
                    return QSCOMM_ERROR_SERVER_UNKNOWN_ERROR;
                }
            }
        }
        
    // 
    // Nothing to read, so return success.
    //     
    return QSCOMM_SUCCESS;        
}
