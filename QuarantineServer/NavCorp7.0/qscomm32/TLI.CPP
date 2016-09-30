// Copyright 1999 Symantec Corporation. 
//*************************************************************************
// tli.cpp - created 06/10/1999
//
// $Header$
//
// Description:
//
//*************************************************************************
// $Log$
//*************************************************************************

/////////////////added for NW os identification///////////////////////////
#ifdef NLM


/*typedef bool BOOL;
typedef unsigned long       DWORD;	
typedef char CHAR;
typedef unsigned short      WORD;
typedef unsigned char       BYTE;
*/
//typedef unsigned char       BYTE;
#define FALSE               0
#define TRUE  				1

#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))


#include <iostream.h> 
#include <nwconio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stat.h>
#include <nwstring.h>
#include <ws2nlm.h>
#include <advanced.h>
#include <errno.h>
#include "qsfwd.h"
//#include <netinet\in.h> 
//#include <arpa\inet.h>
#define SOCKET int
#define INVALID_SOCKET -1
#ifdef __cplusplus      
    extern "C" {
#endif
#include <tiuser.h> 
#ifdef __cplusplus
    }
#endif

#include "qsprotocol.h"
#include "qscomm.h"

#ifdef __cplusplus      
    extern "C" {
#endif
void EnterDebugger(void);
#ifdef __cplusplus
    }
#endif

void* GetFunctionPointer(char* szFunctionName);
/*#ifdef __cplusplus      
    extern "C" {
#endif   

unsigned short	htons( unsigned short value );
unsigned long inet_addr  (char   *cp);
#ifdef __cplusplus
    }
#endif*/
///////////////////////////////////////////////////////////////////////////////
// Local functions.
static int SendViaSPX( int iFile, const char * pszServerAddress, int iPort, LPFNXFERCALLBACK pCallback );
static int SendViaIP( int iFile, const char * pszServerAddress, int iPort, LPFNXFERCALLBACK pCallback );

static int SendChunks_SPX( int iEndPoint, int iFile, LPFNXFERCALLBACK pCallback );
static int SendChunks_IP( SOCKET s, int iFile, LPFNXFERCALLBACK pCallback );
static BOOL FillIpxAddress( IPX_ADDR *psa, const char * lpsAddress, unsigned int iSocket );


static unsigned char BtoH( char ch );
static void AtoH( char *szDest, const char *szSource, int iCount );

BOOL IsIpAddress( const char * pszAddress );

static int GetServerStatus_IP( SOCKET s );
static int GetServerStatus_SPX( int iEndPoint );


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
	int iFile = open( pszSourceFile, O_BINARY | O_RDONLY );
    
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
        case QSCOMM_PROTOCOL_SPX:
            iRet = SendViaSPX( iFile, pszServerAddress, iPort, pCallback );
            break;

            //
            //  For NLM, we support only SPX for now.
            //
          case QSCOMM_PROTOCOL_IP:
        	if( majorServerVersion < 4 ) //EA changes for IP for 4.11 onwards
				iRet = QSCOMM_ERROR_UNKNOWN_PROTOCOL;//EA 12/08/199
			else if(  (majorServerVersion == 4 ) && (minorServerVersion != 11 ) && ( minorServerVersion != 2 ) && ( minorServerVersion != 20 ) )//EA 12/08/199
			{//EA 12/08/199
				  //EA 12/08/199 //winsock 2 support from 4.11 onwards and 4.2 
				  iRet = QSCOMM_ERROR_UNKNOWN_PROTOCOL;//EA 12/08/199
			}	//EA 12/08/199
			else
	        {
	         	iRet = SendViaIP( iFile, pszServerAddress, iPort, pCallback );
	        }
            break;
            
        default:
            iRet = QSCOMM_ERROR_UNKNOWN_PROTOCOL;
        }

    // 
    // Cleanup.
    // 

    close( iFile );
    return iRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : SendViaSPX
//
// Description   : Send a file to a Quarantine Server via the SPX protocol.
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
int SendViaSPX( int iFile, const char * pszServerAddress, int iPort, LPFNXFERCALLBACK pCallback )
{
  	/* Open an endpoint*/
  	int  iEndPoint;
   	if ((iEndPoint = t_open("/dev/nspx", O_RDWR, NULL)) == -1)
   		return QSCOMM_ERROR_SERVER_SOCKET_ERROR;

  	//bind the endpoint no need to bind to a specific socket hence ask the provider to give the address to bind
	if( t_bind(iEndPoint, (struct t_bind*)NULL, (struct t_bind*)NULL) == -1)
		{
		//bind failed
		t_close( iEndPoint );	
		return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
		}


    // 
    // Create and populate server address structure.
    // 
    
    IPX_ADDR server;
    
    if( FALSE == FillIpxAddress( &server, pszServerAddress, iPort ) ) 
        {
        //unbind the socket
        t_unbind( iEndPoint );
        t_close( iEndPoint );
        return QSCOMM_ERROR_SERVER_NOT_FOUND;
        }

	
	//prepare t_call sturcture for connect
	struct t_call sndcall;
	//reset all contents of struct t_call to 0
	memset( &sndcall, 0, sizeof(struct t_call) );
	//get sndcall.addr.buf to point to the address got from pszServerAddress
	sndcall.addr.buf = (char*)&server;
	sndcall.addr.len = sizeof(server);
	sndcall.addr.maxlen = sizeof(server);
	
	// 
    // Connect to server
	if(t_connect( iEndPoint, &sndcall, NULL) == -1)
		{
		t_unbind( iEndPoint );
		t_close( iEndPoint );
		return QSCOMM_ERROR_SERVER_NOT_FOUND;
		}
		
    // 
    // Send file to server
    // 
    int iRet = SendChunks_SPX( iEndPoint, iFile, pCallback );

    // 
    // Cleanup.
    // 
    t_snddis( iEndPoint, NULL );
    t_unbind( iEndPoint );
	t_close( iEndPoint );
   
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

   	WORD	version; //EA 12/08/1999
  	WSADATA	wsaData; //EA 12/08/1999	
  	version = MAKEWORD (2,2); //EA 12/08/1999
  	//EA called WSAStartup since it is neccessary for 4.x and 5.x machines
  	if(sym_startup != NULL) //EA 12/08/1999
  	{
  		if (sym_startup (version, &wsaData) == SOCKET_ERROR) //EA 12/08/1999	
   		{ //EA 12/08/1999
   			return QSCOMM_ERROR_SERVER_SOCKET_ERROR; //EA 12/08/1999
		}//EA 12/08/1999
	}//EA 12/08/1999
	
	// 
    // Create a socket.
    // 
  	if(sym_socket == NULL )
    	return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
    SOCKET s = sym_socket( PF_INET, SOCK_STREAM, 0 );
    if( INVALID_SOCKET == s )
    {
		if(sym_cleanup != NULL)//EA 12/08/1999
			sym_cleanup();//EA 12/08/1999
   		
        return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
    } 
    // 
    // Create and populate server address structure.
    // 
    sockaddr server;
    sockaddr_in* pServer = (sockaddr_in*) &server;
    memset( &server, 0, sizeof( sockaddr ) );
    pServer->sin_family = PF_INET;
    pServer->sin_port = iPort;

    // 
    // Translate address if neccessary.
    // 
    if( IsIpAddress( pszServerAddress ) )
    	{
        // 
        // Convert x.x.x.x address to IP address
        // 
        
    	if(sym_inet_addr == NULL )
    		{
            if(sym_closesocket != NULL)
            	sym_closesocket( s );
           	if(sym_cleanup != NULL)//EA 12/08/1999
   				sym_cleanup();//EA 12/08/1999
   			
            return QSCOMM_ERROR_SERVER_NOT_FOUND;
        	}
        pServer->sin_addr.S_un.S_addr = sym_inet_addr( pszServerAddress );
        if( pServer->sin_addr.S_un.S_addr == INADDR_NONE )
            {
            if(sym_closesocket != NULL)		
            	sym_closesocket( s );
          	if(sym_cleanup != NULL) //EA 12/08/1999
   				sym_cleanup();//EA 12/08/1999
   		
            return QSCOMM_ERROR_SERVER_NOT_FOUND;
            }
           
    	}
    else
        {
        // 
        // Try looking up server using DNS.
        // 
        if(sym_gethostbyname == NULL )
    		{
            if(sym_closesocket != NULL)
            	sym_closesocket( s );
           	if(sym_cleanup != NULL)//EA 12/08/1999
   				sym_cleanup();//EA 12/08/1999
   			
            return QSCOMM_ERROR_SERVER_NOT_FOUND;
        	}
        struct hostent *hp = sym_gethostbyname( pszServerAddress );
        if( hp == NULL )
            {
            if(sym_closesocket != NULL)	
            	sym_closesocket( s );
           	if(sym_cleanup != NULL)//EA 12/08/1999
   				sym_cleanup();//EA 12/08/1999
   			
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
    
    if(sym_connect == NULL )
    {
        if(sym_closesocket != NULL)
        	sym_closesocket( s );
       	if(sym_cleanup != NULL) //EA 12/08/1999
   			sym_cleanup();//EA 12/08/1999
   	
        return QSCOMM_ERROR_SERVER_NOT_FOUND;
    }
    if( sym_connect( s, &server, sizeof( sockaddr ) ) != 0)
    {
    	if(sym_closesocket != NULL)	
	       	sym_closesocket( s );
	    if(sym_cleanup != NULL)//EA 12/08/1999
   			sym_cleanup();//EA 12/08/1999
   		
       return QSCOMM_ERROR_SERVER_NOT_FOUND;
    }
    // 
    // Send file to server
    // 
    int iRet = SendChunks_IP( s, iFile, pCallback );

    // 
    // Cleanup.
    // 

    if(sym_shutdown == NULL )
    {
        if(sym_closesocket != NULL)
        	sym_closesocket( s );
     	if(sym_cleanup != NULL)//EA 12/08/1999
   			sym_cleanup();//EA 12/08/1999
   	
        return iRet;
    }
    if(sym_shutdown != NULL)
    	sym_shutdown( s, 0 );
    if(sym_closesocket != NULL)	
    	sym_closesocket( s );
  	if(sym_cleanup != NULL)//EA 12/08/1999
  		sym_cleanup();//EA 12/08/1999
  
    return iRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : SendChunks
//
// Description   : This routine does the real work of sending chunks of file to
//                 the quarantine server.
//
// Return type   : int 
//
// Argument      :  int s
// Argument      : int iFile
// Argument      : LPFNXFERCALLBACK pCallback
//
///////////////////////////////////////////////////////////////////////////////
// 12/22/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
int SendChunks_SPX( int iEndPoint, int iFile, LPFNXFERCALLBACK pCallback )
{

    FILESIZESTRUCT fs;
    struct stat stats;
    DWORD dwTotalSent = 0, dwBytesRemaining;
    int iBytesRead;
    int nRcvFlags;
    char * pBuffer;
    int iRet = QSCOMM_SUCCESS;

    // 
    // Get file size.
    // 
    if( -1 == fstat( iFile, &stats ) )
        {
        return QSCOMM_ERROR_FILE_NOT_FOUND;
        }


    fs.dwFileSize = (DWORD) stats.st_size;
    fs.dwSignature = FILE_SIZE_SIGNATURE;
    
    // 
    // Tell server how many bytes we'll be sending.
    //
    int nSent = 0;
    nSent = t_snd (iEndPoint, (char*)&fs, sizeof( FILESIZESTRUCT ), 0);
    
    if (nSent == -1)
	    {
	    return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
	    }

	// 
    // Wait here for server to tell us to begin.
    // 
    DWORD dwServerStatus;
    nRcvFlags = 0;
	if(t_rcv( iEndPoint, (char*)&dwServerStatus, sizeof( DWORD ), &nRcvFlags) == -1 )
		{
		return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
        }
    // 
    // Make sure the server is ready for us.
    // 
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
    dwBytesRemaining = fs.dwFileSize;
    
    while( dwBytesRemaining )
        {
        // 
        // Read a chunk.
        // 
        iBytesRead = read( iFile, pBuffer, 0x7FFF );
        if( iBytesRead == -1 )
            {
            iRet = QSCOMM_ERROR_SERVER_UNKNOWN_ERROR;
            break;
            }

        // 
        // Check to see if the server is trying to tell us something
        // 
        iRet = GetServerStatus_SPX( iEndPoint );
        if( iRet != QSCOMM_SUCCESS )
            break;

        // 
        // Send a chunk.
        // 
        if( t_snd( iEndPoint, pBuffer, iBytesRead, 0 ) == -1 )
            {
            // 
            // See if there is an error waiting for us to get.
            // 
            iRet = GetServerStatus_SPX( iEndPoint );
            
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
    nRcvFlags = 0;
    if( t_rcv( iEndPoint, (char*)&dwServerStatus, sizeof( DWORD ), &nRcvFlags) == -1 )
        {
        iRet = QSCOMM_ERROR_SERVER_SOCKET_ERROR;
        }
    else
        {
        // 
        // Translate status values
        // 
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
int SendChunks_IP( SOCKET s, int iFile, LPFNXFERCALLBACK pCallback )
{

    FILESIZESTRUCT fs;
    struct stat stats;
    DWORD dwTotalSent = 0, dwBytesRemaining;
    int iBytesRead;
    char * pBuffer;
    int iRet = QSCOMM_SUCCESS;

    // 
    // Get file size.
    // 
    if( -1 == fstat( iFile, &stats ) )
        {
        return QSCOMM_ERROR_FILE_NOT_FOUND;
        }
    
    fs.dwFileSize = (DWORD) stats.st_size;
    fs.dwSignature = FILE_SIZE_SIGNATURE;

    // 
    // Tell server how many bytes we'll be sending.
    // 
    
    if( sym_send( s, (char*)&fs, sizeof( FILESIZESTRUCT ), 0 ) == SOCKET_ERROR )
        {
        return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
        }

    // 
    // Wait here for server to tell us to begin.
    // 
    DWORD dwServerStatus;
    if( sym_recv( s, (char*)&dwServerStatus, sizeof( DWORD ), 0 ) == SOCKET_ERROR )
        {
        return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
        }

    // 
    // Make sure the server is ready for us.
    // 
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
    dwBytesRemaining = fs.dwFileSize;
    while( dwBytesRemaining )
        {
        // 
        // Read a chunk.
        // 
        iBytesRead = read( iFile, pBuffer, 0x7FFF );
        if( iBytesRead == -1 )
            {
            iRet = QSCOMM_ERROR_SERVER_UNKNOWN_ERROR;
            break;
            }

        // 
        // Check to see if the server is trying to tell us something
        // 
        iRet = GetServerStatus_IP( s );
        if( iRet != QSCOMM_SUCCESS )
            break;

        // 
        // Send a chunk.
        // 
        if( sym_send( s, pBuffer, iBytesRead, 0 ) == SOCKET_ERROR )
            {
            // 
            // See if there is an error waiting for us to get.
            // 
            iRet = GetServerStatus_IP( s );
            
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
    if( sym_recv( s, (char*)&dwServerStatus, sizeof( DWORD ), 0 ) == SOCKET_ERROR )
        {
        iRet = QSCOMM_ERROR_SERVER_SOCKET_ERROR;
        }
    else
        {
        // 
        // Translate status values
        // 
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
BOOL FillIpxAddress( IPX_ADDR *psa,  
                     const char* lpsAddress,
                     unsigned int iSocket ) 
{ 
    // Location of the separator 
    char* pszPoint;                  
    memset ( psa, 0, sizeof ( IPX_ADDR ) ); 
    
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
    
        AtoH ( (char *) psa->ipxa_net, lpsAddress, 4 ); 
        AtoH ( (char *) psa->ipxa_node, pszPoint + 1, 6 ); 
        
        } 
    // 
    // Save off endpoint
    // 
		psa->ipxa_socket[0] = LOBYTE(iSocket); //EA - 08/10
		psa->ipxa_socket[1] = HIBYTE(iSocket); //EA - 08/10
	//*(WORD *)psa->ipxa_socket = IntSwap(iSocket); //EA - 08/10
    return TRUE;
} 

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
void AtoH( char *szDest, const char *szSource, int iCount ) 
{ 
    while (iCount--) 
        { 
        *szDest++ = ( BtoH ( *szSource++ ) << 4 ) + BtoH ( *szSource++ ); 
        } 
} 
 

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
int GetServerStatus_SPX( int iEndPoint )
{
    DWORD dwServerStatus = 0;
    int   nRcvFlags;
    // 
    // Query status of this socket.
    // 
    int iSelectRet = t_look( iEndPoint );
    if( iSelectRet == -1 )
        {
        return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
        }
	else if ( iSelectRet == T_DATA )
		{
		//there is some data at the endpoint 
		// 
        // Retrieve status from server.
        //
        nRcvFlags = 0;
        if(  t_rcv( iEndPoint, (char*)&dwServerStatus, sizeof( DWORD ), &nRcvFlags ) == -1 )
            {
            return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
            }
       else
            {
            // 
            // Translate status values
            // 
            if( dwServerStatus == SERVER_ERROR )
            	return QSCOMM_ERROR_SERVER_UNKNOWN_ERROR;
            else if( dwServerStatus == SERVER_ERROR_DISK_SPACE )
            	return QSCOMM_ERROR_SERVER_FULL;
            else if( dwServerStatus == SERVER_ERROR_SOCKET)
            	return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
            else if( dwServerStatus == 	SERVER_ERROR_BOGUS_FILE)
            	return QSCOMM_ERROR_BOGUS_FILE;
            else
            	return 	QSCOMM_ERROR_SERVER_UNKNOWN_ERROR;
            
           
            }
        }


    // 
    // Nothing to read, so return success.
    //     
    return QSCOMM_SUCCESS;        
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
int GetServerStatus_IP( SOCKET s )
{
    fd_set  fdset;
    struct timeval stTimeout = { 0, 0 };
    DWORD dwServerStatus = 0;

    // 
    // Query status of this socket.
    // 
    FD_ZERO( &fdset );
    FD_SET( s, &fdset );
    if(sym_select == NULL)
    {
       return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
    }
    int iSelectRet = sym_select( 0, &fdset, NULL, NULL, &stTimeout );
    if( iSelectRet == SOCKET_ERROR )
        {
        return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
        }
    else if( 1 == iSelectRet )
        {
        // 
        // Retrieve status from server.
        // 
        if( sym_recv( s, (char*)&dwServerStatus, sizeof( DWORD ), 0 ) == SOCKET_ERROR )
            {
            return QSCOMM_ERROR_SERVER_SOCKET_ERROR;
            }
        else
            {
            // 
            // Translate status values
            // 
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
    strncpy( szAddress, pszAddress, 17 );
    char *p = strtok( szAddress, "." );

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
        p = strtok( NULL, "." );
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



#endif //.. ifdef NLM
