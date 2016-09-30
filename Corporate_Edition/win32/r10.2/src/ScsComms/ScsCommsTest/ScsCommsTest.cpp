// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ScsCommsTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//#include <iostream>
#include <process.h>
#include <crtdbg.h>
#include <time.h>
#include <vector>
#include <tchar.h>
#include <sys/timeb.h>

#define INITIIDS
#include "SymInterface.h"
#include "SymInterfaceLoader.h"
#include "IMessageManager.h"
#include "Configuration.h"
#include "IByteStreamer.h"
#include "nts_legacy.h"
#include "SockAddrOps.h"
#include ".\example_cert_store\example_cert_store.h"
#include "RoleVector.h"
#include "SymSaferStrings.h"
#include "SavAssert.h"

// array_sizeof to help with element count as opposed to byte count.

#define array_sizeof(x) (sizeof(x) / sizeof(x[0]))

#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))



bool g_quit					   = false;
const char* g_path_to_scscomms = "ScsComms.dll";
bool g_useDefaultCertsAndKeys  = true;
unsigned long g_delay          = 0;
bool g_printRoles              = false;
unsigned long g_msgMaxSize     = 2048;
unsigned long g_msgMinSize     = sizeof(VDTM_HEADER);

//Configuration parameters, available only from the config file
std::string certStore;
std::string loginCACertFileName;
std::string loginCAPrivateKeyFileName;
std::string serverCertFileName;
std::string serverPrivateKeyFileName;
std::vector<std::string> rootCACertFileNames;
std::vector<std::string> rootCAPrivateKeyFileNames;
std::string userCertFileName;
std::string userPrivateKeyFileName;

unsigned long clientTimeout = 0;
std::string clientChannel;
bool clientSharedMsgMgr = false;

// These macros provide implementations for GetFactory and GetObjectCount.
SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

typedef std::basic_string<unsigned char> ustring;

ustring parseCert( const std::string& certFileName );
ustring parseKey( const std::string& keyFileName );

void print_usage()
{
    std::cout << "ScsCommsTest: Test framework for the ScsComms component." << std::endl;
    std::cout << std::endl;
    std::cout << "    Usage: specify \'s\' for sever mode or \'c' for client mode." << std::endl;
    std::cout << "           specify port number to listen on or connect to" << std::endl;
	std::cout << "           if using server mode, optionally specify a lifespan in seconds." << std::endl;
    std::cout << "           if using client mode, specify ip address to connect to" << std::endl;
    std::cout << "           if using client mode, specify number of threads to start" << std::endl;
}

BOOL WINAPI console_ctrl_handler( DWORD ctrl_type )
{
    g_quit = true;

    return( TRUE );
}

std::string     g_host_name = "127.0.0.1";
unsigned short  g_port = 0;

typedef CSymInterfaceHelper< CSymInterfaceLoader,
                             ScsSecureComms::IMessageManager,
                             &ScsSecureComms::SYMOBJECT_MessageManager,
                             &ScsSecureComms::IID_MessageManagerServer > MsgMgrLoader;

typedef CSymInterfaceHelper< CSymInterfaceLoader,
                             ScsSecureComms::IMessageManagerServer,
                             &ScsSecureComms::SYMOBJECT_MessageManager,
                             &ScsSecureComms::IID_MessageManagerServer > MsgMgrSrvLoader;

typedef CSymInterfaceHelper< CSymInterfaceLoader,
                             ScsSecureComms::IKey,
                             &ScsSecureComms::SYMOBJECT_Key,
                             &ScsSecureComms::IID_Key > KeyLoader;

typedef CSymInterfaceHelper< CSymInterfaceLoader,
                             ScsSecureComms::ICert,
                             &ScsSecureComms::SYMOBJECT_Cert,
                             &ScsSecureComms::IID_Cert > CertLoader;

typedef CSymInterfaceHelper< CSymInterfaceLoader,
                             ScsSecureComms::IAuth,
                             &ScsSecureComms::SYMOBJECT_AuthServer,
                             &ScsSecureComms::IID_Auth > AuthServerLoader;

typedef CSymInterfaceHelper< CSymInterfaceLoader,
                             ScsSecureComms::IAuth,
                             &ScsSecureComms::SYMOBJECT_AuthLogin,
                             &ScsSecureComms::IID_Auth > AuthLoginLoader;

typedef CSymInterfaceHelper< CSymInterfaceLoader,
                             ScsSecureComms::IUser,
                             &ScsSecureComms::SYMOBJECT_User,
                             &ScsSecureComms::IID_User > UserLoader;

typedef CSymInterfaceHelper< CSymInterfaceLoader,
                             ScsSecureComms::IMessageBuffer,
                             &ScsSecureComms::SYMOBJECT_MessageBuffer,
                             &ScsSecureComms::IID_MessageBuffer > MessageBufferLoader;

typedef struct tag_ClientThreadParams
{
    ScsSecureComms::IMessageManager*    m_msg_mgr;
    ScsSecureComms::IAuth*              m_credentials;
    unsigned short                      m_index;

} ClientThreadParams;

unsigned int __stdcall client_thread( void* param )
{
    ClientThreadParams* params = reinterpret_cast<ClientThreadParams*>(param);

    srand( time(NULL) | GetCurrentThreadId() );

    MessageBufferLoader msg_buf_ldr;
    
    msg_buf_ldr.Initialize( LoadLibrary( g_path_to_scscomms ) );

    unsigned long msg_count = 0;
    struct _timeb msg_start = {0};
    struct _timeb msg_end = {0};
    struct _timeb msg_total_send_time = {0};

    // Allocate buffers to hold our request and response messages.

    BYTE* data_in = NULL;
    BYTE* data_out = NULL;

    data_in = reinterpret_cast<BYTE*>(malloc( g_msgMaxSize ));

    if( data_in == NULL )
    {
        std::cout << "Client thread failed to allocated message buffers." << std::endl;
        return( -1 );
    }

    data_out = reinterpret_cast<BYTE*>(malloc( g_msgMaxSize ));

    if( data_out == NULL )
    {
        free( data_in );

        std::cout << "Client thread failed to allocated message buffers." << std::endl;
        return( -1 );
    }

    // Loop, sendig messages.

    while( g_quit == false )
    {
        sockaddr_in addr_to = {0};

        addr_to.sin_family      = AF_INET;
        addr_to.sin_addr.s_addr = inet_addr( g_host_name.c_str() );
//      addr_to.sin_port        = htons( g_port + params->m_index );
        addr_to.sin_port        = htons( g_port );

        // Generate some "random" data.

        unsigned long data_in_len = g_msgMinSize + (g_msgMaxSize == g_msgMinSize ? 0 : (rand() % (g_msgMaxSize - g_msgMinSize)));
        unsigned long data_out_len = g_msgMaxSize;

        VDTM_HEADER* hdr = reinterpret_cast<VDTM_HEADER*>(data_in);

        memset( hdr, 0, sizeof(VDTM_HEADER) );

        hdr->Version = VDTM_TRANSPORT_VERSION;
        hdr->Command = 4; // COM_ALIVE
        hdr->Product = VDTM_PROD_LDVP;
        hdr->Reply = 0;
        hdr->ID = static_cast<unsigned char>(GetCurrentThreadId() % VDTM_MAX_ID);
        hdr->Flags = VDTM_FLAG_NEEDS_CRIPT;
        hdr->RetryRate = 30;
        hdr->Retrys = 3;

        for( unsigned int i = sizeof(VDTM_HEADER); i < data_in_len; ++i )
        {
            switch( data_in_len - i )
            {
            case 4 :
                data_in[i] = 'E';
                break;
            case 3 :
                data_in[i] = 'N';
                break;
            case 2 :
                data_in[i] = 'D';
                break;
            case 1 :
                data_in[i] = 0; // NULL term for printing politeness
                break;
            default :
                data_in[i] = 0x20 + (rand() % (0x7E - 0x20)); // random printable char
                break;
            }
        }

        unsigned long hints = 0;

/*      // Use this code to test reconnects.

        if( msg_count % 10 == 0 )
        {
            hints |= HINT_KEEP_ALIVE;
        }
        else if( msg_count % 10 == 9 )
        {
            hints |= HINT_DONE;
        }
*/

/*      // Use this code to test no response functionality.

        hints |= HINT_NO_WAIT;
*/

/*      // Use this code to test exact address functionality.

        hints |= HINT_EXACT_ADDRESS;
*/
        _ftime( &msg_start );

        // Get buffers to send the data.

        ScsSecureComms::IMessageBuffer* msg_req = NULL;
        ScsSecureComms::IMessageBuffer* msg_rsp = NULL;

        msg_buf_ldr.CreateObject( &msg_req );
        msg_buf_ldr.CreateObject( &msg_rsp );

        // Send this funky message to the other side.

        ScsSecureComms::RESULT res = ScsSecureComms::RTN_OK;

        if( _tcsicmp( clientChannel.c_str(), "ipc" ) == 0 )
        {
            msg_req->Write( data_in, MIN( data_in_len, VDTM_MAX_PACKET_SIZE ) );

            res = params->m_msg_mgr->SendRequestMessageIPC( msg_req, msg_rsp );
        }
        else if( _tcsicmp( clientChannel.c_str(), "udp" ) == 0 )
        {
            msg_req->Write( data_in, MIN( data_in_len, VDTM_MAX_PACKET_SIZE ) );

            if( addr_to.sin_addr.s_addr != INADDR_NONE )
            {
                res = params->m_msg_mgr->SendRequestMessageInsecure( (sockaddr*)&addr_to,
                                                                      NULL,
                                                                      0,
                                                                      msg_req,
                                                                      msg_rsp,
                                                                      (clientTimeout == 0 ? 60 : clientTimeout) );
            }
            else
            {
                res = params->m_msg_mgr->SendRequestMessageInsecure( NULL,
                                                                     g_host_name.c_str(),
//                                                                   g_port + params->m_index,
                                                                     g_port,
                                                                     msg_req,
                                                                     msg_rsp,
                                                                     (clientTimeout == 0 ? 60 : clientTimeout) );
            }
        }
        else // _tcsicmp( clientChannel.c_str(), "ssl" ) == 0 || _tcsicmp( clientChannel.c_str(), "sslshare" ) == 0
        {
            msg_req->Write( data_in, data_in_len );

            if( addr_to.sin_addr.s_addr != INADDR_NONE )
            {
                res = params->m_msg_mgr->SendRequestMessage( (sockaddr*)&addr_to,
                                                              NULL,
                                                              0,
                                                              params->m_credentials,
                                                              msg_req,
                                                              msg_rsp,
                                                              clientTimeout*1000, // 0 forces the longest possible wait
                                                              hints );

            }
            else
            {
                res = params->m_msg_mgr->SendRequestMessage( NULL,
                                                             g_host_name.c_str(),
//                                                           g_port + params->m_index,
                                                             g_port,
                                                             params->m_credentials,
                                                             msg_req,
                                                             msg_rsp,
                                                             clientTimeout*1000, // 0 forces the longest possible wait
                                                             hints );
            }
        }

        // Get the data out and verify that it has not been corrupted.

        if( res == ScsSecureComms::RTN_OK )
        {
            msg_rsp->Read( data_out, &data_out_len );

            if( data_out_len != data_in_len )
            {
                std::cout << "Reflected response not the same length as request." << std::endl;
            }
            else
            {
                char* iter = NULL;
                for( unsigned long i = 0; i < data_in_len; ++i )
                {
                    if( data_in[i] != data_out[i] )
                    {
                        std::cout << "Reflected response corrupted at byte" << i << std::cout;
                        break;
                    }
                }
            }
        }

        _ftime( &msg_end );

        // Output success, fail and stats.

        if( res != ScsSecureComms::RTN_OK )
        {
            std::cout << "[0x" << std::hex << GetCurrentThreadId() << "] SendRequestMessage returned: 0x" << std::hex << res << std::endl;
        }
        else
        {
            ++msg_count;

            if( msg_end.millitm < msg_start.millitm )
            {
                msg_total_send_time.time    += (msg_end.time - 1) - msg_start.time;
                msg_total_send_time.millitm += (msg_end.millitm + 1000) - msg_start.millitm;
            }
            else
            {
                msg_total_send_time.time    += msg_end.time - msg_start.time;
                msg_total_send_time.millitm += msg_end.millitm - msg_start.millitm;
            }

            if( msg_total_send_time.millitm >= 1000 )
            {
                msg_total_send_time.time += 1;
                msg_total_send_time.millitm -= 1000;
            }

            if( msg_count % 1000 == 0 )
            {
                std::cout << std::dec << msg_count << " messages in " << msg_total_send_time.time << "." << msg_total_send_time.millitm << " seconds." << std::endl;
            }

        }

        msg_req->Release();        
        msg_rsp->Release();        

        // Delay if configured to do so, but be responsive to quits.

        unsigned long sleep_count = 0;

        while( g_quit == false && sleep_count * 10 < g_delay )
        {
            Sleep( 10 );
            ++sleep_count;
        }
    }

    if( clientSharedMsgMgr == false )
    {
        params->m_msg_mgr->Deinitialize();
    }

    params->m_msg_mgr->Release();
    params->m_credentials->Release();

    free( data_in );
    free( data_out );

    delete params;

    return( 0 );
}

bool add_trusted_root_cert( HMODULE scscomms, ScsSecureComms::IMessageManager* msg_mgr, const ustring& rootCert )
{
    bool ret = false;

    ScsSecureComms::IByteStreamer* stream = NULL;
    ScsSecureComms::ICert* cert = NULL;

    if( rootCert.empty() == false )
    {
        ScsSecureComms::RESULT rtn = ScsSecureComms::RTN_OK;

        CertLoader cert_ldr;

        cert_ldr.Initialize( scscomms );

        cert_ldr.CreateObject( &cert );

        // Set the cert from binary data.

        cert->QueryInterface( ScsSecureComms::IID_ByteStreamer, reinterpret_cast<void**>(&stream) );

        rtn = stream->ParseByteStream( reinterpret_cast<const unsigned char*>(rootCert.c_str()), rootCert.length() );

        if( rtn != ScsSecureComms::RTN_OK )
        {
            std::cout << "Certificate parsing error: " << std::hex << rtn << std::endl;
            goto Root_Cert_Setup_Failed;
        }

        rtn = msg_mgr->AddTrustedRootCertificate( cert );

        if( rtn != ScsSecureComms::RTN_OK )
        {
            std::cout << "AddTrustedRootCertificate failed: " << std::hex << rtn << std::endl;
            goto Root_Cert_Setup_Failed;
        }
    }

    ret = true;

Root_Cert_Setup_Failed:

    if( cert != NULL )
        cert->Release();

    if( stream != NULL )
        stream->Release();

    return( ret );
}

bool add_trusted_root_certs( HMODULE scscomms, ScsSecureComms::IMessageManager* msg_mgr )
{
    bool ret = false;

    ustring rootCert;

	if ( g_useDefaultCertsAndKeys )
	{
        rootCert = server_group_root_cert_pem;

        ret = add_trusted_root_cert( scscomms, msg_mgr, rootCert );
	}
	else
	{
        for( std::vector<std::string>::iterator it = rootCACertFileNames.begin(); it != rootCACertFileNames.end(); ++it )
        {
            if( (*it).empty() == false )
            {
			    // Parse the login ca cert.  If this fails, use the default

			    rootCert = parseCert( (*it).c_str() );
                
                if( rootCert.empty() == true )
			    {
				    std::cout << "Parsing the root ca cert (" << (*it).c_str() << ") failed.  Using the default cert." << std::endl;
                    rootCert = server_group_root_cert_pem;
			    }
            }

            ret = add_trusted_root_cert( scscomms, msg_mgr, rootCert );

            if( ret == false )
            {
                break;
            }
        }

        if( rootCACertFileNames.size() == 0 )
		{
            // This is ok -- basically we are going to have no trusted roots.

			std::cout << "Client connecting without root certs." << std::endl;
		}
    }

    return( ret );
}

class MessageHandler : public ISymBaseImpl< CSymThreadSafeRefCount >,
                       public ScsSecureComms::IMessageManagerServerCallback
{
public:
    MessageHandler( ScsSecureComms::IMessageManagerServer* msg_mgr_srv = NULL,
                    ScsSecureComms::IAuth* credentials = NULL )
     :  m_MsgMgr( msg_mgr_srv ),
        m_Credentials( credentials )
    {
        m_MsgBufLdr.Initialize( g_path_to_scscomms );
    }

    ~MessageHandler() {}

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY( ScsSecureComms::IID_MessageManagerServerCallback, ScsSecureComms::IMessageManagerServerCallback )
    SYM_INTERFACE_MAP_END()

    ScsSecureComms::IMessageManagerServerPtr    m_MsgMgr;
    ScsSecureComms::IAuthPtr                    m_Credentials;
    MessageBufferLoader                         m_MsgBufLdr;
    CriticalSection                             m_PrintSentry;

    // IMessageManagerServerCallback

    void PrintCredentials( sockaddr* addr_from, ScsSecureComms::IAuth* credentials )
    {
        ScsSecureComms::RESULT res = ScsSecureComms::RTN_OK;

        sockaddr_in* addr_from_ipv4 = reinterpret_cast<sockaddr_in*>(addr_from);

        std::cout << static_cast<int>(addr_from_ipv4->sin_addr.S_un.S_un_b.s_b1) << "."
                  << static_cast<int>(addr_from_ipv4->sin_addr.S_un.S_un_b.s_b2) << "."
                  << static_cast<int>(addr_from_ipv4->sin_addr.S_un.S_un_b.s_b3) << "."
                  << static_cast<int>(addr_from_ipv4->sin_addr.S_un.S_un_b.s_b4) << ":" << ntohs( addr_from_ipv4->sin_port );

        std::cout << " (roles: ";

        if( credentials->GetCertCount() > 0 )
        {
            ScsSecureComms::ICertVectorPtr certs;
            ScsSecureComms::IRoleVectorPtr roles;

            if( (res = credentials->GetCertChain( certs.m_p )) == ScsSecureComms::RTN_OK )
            {
                ScsSecureComms::ICertPtr cert;
            
                cert.Attach( certs->at(0) );

                if( (res = cert->GetRoles( roles.m_p )) == ScsSecureComms::RTN_OK )
                {
                    for( unsigned int i = 0; i < roles->size(); ++i )
                    {
                        if( i != 0 )
                            std::cout << ", ";

                        std::cout << roles->at(i);
                    }
                }
            }
        }
        else
        {
            std::cout << "<none>";
        }

        if( res != ScsSecureComms::RTN_OK )
        {
            std::cout << " failed to get roles 0x" << std::hex << res;
        }

        std::cout << ")";
    }

    virtual ScsSecureComms::RESULT OnReceiveRequestMessage( sockaddr* addr_from,
                                                            ScsSecureComms::IAuth* credentials,
                                                            ScsSecureComms::IMessageBuffer* msg_in,
                                                            ScsSecureComms::IMessageBuffer* msg_out )
    {
        // Verify that his permissions are allowed.  ;)

        if( credentials != NULL )
        {
            credentials->IsOperationAllowed( ScsSecureComms::COMMS_ACCESS_GET_PONG_DATA );
        }

        // Find out who sent this.

        if( g_printRoles == true )
        {
            CriticalFunction fn( m_PrintSentry );

            std::cout << "Message from: ";
            PrintCredentials( addr_from, credentials );
            std::cout << " [size: " << msg_in->GetSize() << "]" << std::endl;
        }

        // Get a copy of the request.
/*
        ScsSecureComms::IMessageBuffer* msg_in_copy = NULL;
        ScsSecureComms::IMessageBuffer* msg_out_copy = NULL;

        if( m_MsgMgr != NULL )
        {
            m_MsgBufLdr.CreateObject( &msg_in_copy );
            m_MsgBufLdr.CreateObject( &msg_out_copy );
        }
*/
        // Just reflect the request.

        while( msg_in->GetSize() > msg_in->GetReadOffset() )
        {
            unsigned char data[VDTM_MAX_PACKET_SIZE];
            unsigned long data_len = sizeof(data);

            msg_in->Read( data, &data_len );

            msg_out->Write( data, data_len );

/*
            if( msg_in_copy != NULL )
                msg_in_copy->Write( data, data_len );
*/
        }

        // Send the last message we received back to the caller.

/*
        if( m_MsgMgr != NULL )
        {
            m_MsgMgr->SendRequestMessage( (sockaddr*)addr_from,
                                          NULL,
                                          0,
                                          m_Credentials,
                                          msg_in_copy,
                                          msg_out_copy,
                                          // INIFINTE forces the longest possible wait (for debugging purposes)
                                          (clientTimeout == 0 ? INFINITE : clientTimeout*1000),
                                          HINT_EXACT_ADDRESS );

            msg_in_copy->Release();
            msg_out_copy->Release();
        }
*/
        // Add some artificial delay

        if( g_delay > 0 )
        {
            Sleep( g_delay );
        }

        return( ScsSecureComms::RTN_OK );
    }

    virtual ScsSecureComms::RESULT OnReceiveRequestMessageInsecure( sockaddr* addr_from,
                                                                    ScsSecureComms::IMessageBuffer* msg_in,
                                                                    ScsSecureComms::IMessageBuffer* msg_out )
    {
        unsigned char data_in[VDTM_MAX_PACKET_SIZE];
        unsigned long data_len = sizeof(data_in);

        msg_in->Read( data_in, &data_len );

        const VDTM_HEADER* hdr_request = reinterpret_cast<const VDTM_HEADER*>(data_in);

        VDTM_HEADER hdr_reply = {0};
        DWORD ret = 0;

        hdr_reply.Version = VDTM_TRANSPORT_VERSION;
        hdr_reply.Command = hdr_request->Command;
        hdr_reply.Product = hdr_request->Product;
        hdr_reply.Reply   = VDTM_RESPONSE;
        hdr_reply.ID      = hdr_request->ID;
        hdr_reply.Flags   = VDTM_FLAG_NEEDS_CRIPT;
        hdr_reply.ID      = hdr_request->ID;

        switch( hdr_request->Command )
        {
        case 4 : // COM_ALIVE
            ret = 0; // ERROR_SUCCESS
            break;

        default :
            ret = 0x40000011; // COM_ERROR_UNSUPPORTED_FUNCTION
            break;
        }

        msg_out->Write( reinterpret_cast<unsigned char*>(&hdr_reply), sizeof(hdr_reply) );
        msg_out->Write( reinterpret_cast<unsigned char*>(&ret), sizeof(ret) );

        return( ScsSecureComms::RTN_OK );
    }

    virtual ScsSecureComms::RESULT OnReceiveRequestMessageIPC( ScsSecureComms::IMessageBuffer* msg_in,
                                                               ScsSecureComms::IMessageBuffer* msg_out )
    {
        unsigned char data_in[VDTM_MAX_PACKET_SIZE];
        unsigned long data_len = sizeof(data_in);

        msg_in->Read( data_in, &data_len );

        const VDTM_HEADER* hdr_request = reinterpret_cast<const VDTM_HEADER*>(data_in);

        VDTM_HEADER hdr_reply = {0};
        DWORD ret = 0;

        hdr_reply.Version = VDTM_TRANSPORT_VERSION;
        hdr_reply.Command = hdr_request->Command;
        hdr_reply.Product = hdr_request->Product;
        hdr_reply.Reply   = VDTM_RESPONSE;
        hdr_reply.ID      = hdr_request->ID;
        hdr_reply.Flags   = VDTM_FLAG_NEEDS_CRIPT;
        hdr_reply.ID      = hdr_request->ID;

        switch( hdr_request->Command )
        {
        case 4 : // COM_ALIVE
            ret = 0; // ERROR_SUCCESS
            break;

        default :
            ret = 0x40000011; // COM_ERROR_UNSUPPORTED_FUNCTION
            break;
        }

        msg_out->Write( reinterpret_cast<unsigned char*>(&hdr_reply), sizeof(hdr_reply) );
        msg_out->Write( reinterpret_cast<unsigned char*>(&ret), sizeof(ret) );

        return( ScsSecureComms::RTN_OK );
    }

    virtual void OnAcceptConnection( sockaddr* addr_from,
                                     ScsSecureComms::IAuth* credentials,
                                     ScsSecureComms::RESULT disposition )
    {
        if( g_printRoles == true )
        {
            CriticalFunction fn( m_PrintSentry );

            std::cout << "Connection from: ";
            PrintCredentials( addr_from, credentials );
        }
    }

};

bool get_certs( HMODULE scscomms,                                   // handle to module implementing ICert
                const std::vector<ustring>& cert_data,              // array of text certs
                const ustring& key_data,                            // key data corresponding to cert in cert_data[0]
                std::vector<ScsSecureComms::ICert*>& certs )        // array to populate with created cert objects
{
    bool ret = false;

    KeyLoader key_ldr;
    CertLoader cert_ldr;

    key_ldr.Initialize( scscomms );
    cert_ldr.Initialize( scscomms );

    ScsSecureComms::ICert* cert = NULL;
    ScsSecureComms::IKey* key = NULL;
    ScsSecureComms::IByteStreamer* stream = NULL;

    ScsSecureComms::RESULT rtn;

    std::vector<ustring>::const_iterator it;

    // Validate params.

    if( key_data.empty() == true || cert_data.size() < 1 )
    {
        return( false );
    }

    // Create the identity cert and key.

    cert_ldr.CreateObject( &cert );

    // Set the cert from binary data.

    cert->QueryInterface( ScsSecureComms::IID_ByteStreamer, reinterpret_cast<void**>(&stream) );

    rtn = stream->ParseByteStream( cert_data.at(0).c_str(), cert_data.at(0).length() - 1 );

    if( rtn != ScsSecureComms::RTN_OK )
    {
        std::cout << "Certificate parsing error on identity cert: " << std::hex << rtn << std::endl;
        goto Cert_Setup_Failed;
    }

    stream->Release();

    // Set the key from binary data.

    key_ldr.CreateObject( &key );

    key->QueryInterface( ScsSecureComms::IID_ByteStreamer, reinterpret_cast<void**>(&stream) );

    rtn = stream->ParseByteStream( key_data.c_str(), key_data.length() - 1 );

    if( rtn != ScsSecureComms::RTN_OK )
    {
        std::cout << "Key parsing error: " << std::hex << rtn << std::endl;
        goto Cert_Setup_Failed;
    }

    stream->Release();

    // Put the key in with the cert.

    rtn = cert->SetPrivateKey( key );

    key->Release(); // cert has it now

    if( rtn != ScsSecureComms::RTN_OK )
    {
        std::cout << "Key cert mismatch error: " << std::hex << rtn << std::endl;
        goto Cert_Setup_Failed;
    }

    certs.push_back( cert );

    // Now generate the rest of the certs.

    it = cert_data.begin();

    ++it; // Move past the identity cert.

    while( it != cert_data.end() )
    {
        cert_ldr.CreateObject( &cert );

        // Set the cert from binary data.

        cert->QueryInterface( ScsSecureComms::IID_ByteStreamer, reinterpret_cast<void**>(&stream) );

        rtn = stream->ParseByteStream( (*it).c_str(), (*it).length() - 1 );

        if( rtn != ScsSecureComms::RTN_OK )
        {
            std::cout << "Certificate parsing error on cert in chain: " << std::hex << rtn << std::endl;
            goto Cert_Setup_Failed;
        }

        stream->Release();

        certs.push_back( cert );

        ++it;
    }

    ret = true;

Cert_Setup_Failed:

    return( ret );
}

void do_server( unsigned short port, char auth_type, unsigned long serverLifetime )
{
	//Start time for the purpose of shutting down the main thread after a predetermined period.
	DWORD startTime;

    SYMRESULT symres = SYM_OK;
	ScsSecureComms::RESULT scscommsres = ScsSecureComms::RTN_OK;

    MsgMgrSrvLoader mm_ldr;

    unsigned short ip_port_secure = port;
    unsigned short ip_port = port;
    unsigned short ipx_port = port;

    symres = mm_ldr.Initialize( g_path_to_scscomms );

    if( symres == SYM_OK )
    {
        // Get a second handle to the DLL so that we can ensure that it stays loaded
        // until process exit.  Otherwise, memory leaks reported by the C-run time will
        // throw exceptions, because information about where they were allocated is unmapped
        // when the DLL is unloaded.

        HMODULE scscomms = LoadLibrary( g_path_to_scscomms );

        AuthServerLoader auth_ldr;
        auth_ldr.Initialize( scscomms );

        ScsSecureComms::IMessageManagerServer* msg_mgr_srv = NULL;
        ScsSecureComms::IAuth* auth = NULL;

        MessageHandler* handler = NULL;

        symres = mm_ldr.CreateObject( &msg_mgr_srv );

        if( symres == SYM_OK )
        {
            msg_mgr_srv->Initialize();

 			//Save off the time we started listening
 			startTime = GetTickCount();

            // Add trusted root certs.

            add_trusted_root_certs( scscomms, msg_mgr_srv );

            // Create the server cert chain.

            std::vector<ustring> server_certs;
            std::vector<ScsSecureComms::ICert*> server_icerts;
            ustring server_key;

			if ( g_useDefaultCertsAndKeys )
			{
				// A server cert chain
				server_certs.push_back( server_cert_pem );
				server_certs.push_back( server_group_root_cert_pem );
				server_key = server_key_pem;
			}
			else
			{
                ustring tempCert;
                ustring tempKey;

                if( serverCertFileName.empty() == false )
                {
				    // Parse the server cert.  If this fails, use the default

                    tempCert = parseCert( serverCertFileName );
				    if ( tempCert.empty() == false )
				    {
					    server_certs.push_back( tempCert );
				    }
				    else
				    {
					    std::cout << "Parsing the server cert failed.  Using the default cert." << std::endl;
        				server_certs.push_back( server_cert_pem );
				    }

                    if( rootCACertFileNames.empty() == false )
                    {
				        // Parse the root ca cert.  If this fails, use the default

                        tempCert = parseCert( rootCACertFileNames.at( 0 ) );
				        if ( tempCert.empty() == false )
				        {
					        server_certs.push_back( tempCert );
				        }
				        else
				        {
					        std::cout << "Parsing the root ca cert failed.  Using the default cert." << std::endl;
            				server_certs.push_back( server_group_root_cert_pem );
				        }
                    }

				    // Parse the server private key.  If this fails, use the default

                    tempKey = parseKey( serverPrivateKeyFileName );
				    if ( tempKey.empty() == false )
				    {
					    server_key = tempKey;
				    }
				    else
				    {
					    std::cout << "Parsing the server private key failed.  Using the default private key." << std::endl;
					    server_key = server_key_pem;
				    }
                }
			    else
			    {
                    // This is ok -- basically we are going to act like a SAV Client.

				    std::cout << "Server running without certs." << std::endl;
			    }
			}

            if( server_certs.empty() == false )
            {
                if( get_certs( scscomms, server_certs, server_key, server_icerts ) == false )
                {
                    goto Server_Setup_Failed;
                }
            }

            // Create an auth object to serve as the credentials presented to connecting peers.

            auth_ldr.CreateObject( &auth );

            try
            {
                ScsSecureComms::ICert **icerts_array = new ScsSecureComms::ICert *[server_icerts.size()];

                std::vector<ScsSecureComms::ICert*>::iterator iter = server_icerts.begin();

                for (int cnt = 0; iter != server_icerts.end(); ++iter, ++cnt)
                    icerts_array[cnt] = *iter;
                auth->SetCertChain( icerts_array, server_icerts.size() );

                delete[] icerts_array;
            }
            catch (std::bad_alloc &)
            {
                goto Server_Setup_Failed;
            }

            // Set remote auth.

            switch( auth_type )
            {
            case 'a' :
                auth->SetAuthTypeRemote( ScsSecureComms::IAuth::NO_AUTH );
                break;

            case 'n' :
                auth->SetAuthTypeRemote( ScsSecureComms::IAuth::NAME_AUTH );
                break;

            case 'r' :
                auth->SetAuthTypeRemote( ScsSecureComms::IAuth::ROOT_AUTH );
                break;

            default :
                SAVASSERT( false );
                break;
            }

            // Create a message handler

            handler = new MessageHandler( msg_mgr_srv, auth );

            handler->AddRef();

            symres = msg_mgr_srv->ListenStart( &ip_port_secure,
                                               handler,
                                               auth );

            g_port = ip_port_secure;

            if( symres == ScsSecureComms::RTN_OK )
            {
                symres = msg_mgr_srv->ListenStartInsecure( &ip_port,
                                                           &ipx_port,
                                                           handler );
                if( SYM_FAILED(symres) )
                {
                    std::cout << "ERROR: Could not listen on legacy protocol ip port: " << ip_port << ", ipx socket: " << ipx_port << " : " << std::hex << symres << std::endl;
                }

                // Get the addrresses we're listening on.

                ScsSecureComms::ISockAddrVectorPtr addrs;

                symres = msg_mgr_srv->GetAddresses( addrs.m_p );

                // Print them out

                for( unsigned int i = 0; i < addrs->size(); ++i )
                {
                    sockaddr_storage addr = addrs->at(i);
                    sockaddr_buffer* addr_buf = reinterpret_cast<sockaddr_buffer*>(&addr);

                    char addr_str[SOCKADDR_BUFFER_STR_LEN];

                    switch( addr_buf->sockaddr_family )
                    {
                    case AF_INET :
                        sssnprintf( addr_str, sizeof(addr_str),
                                    "ip:%s:%hu", 
                                    inet_ntoa( addr_buf->u.Ipv4.sin_addr ),
                                    ip_port_secure );

                        std::cout << "Address: " << addr_str << std::endl;

                        break;

                    case AF_IPX :
                        sssnprintf( addr_str, sizeof(addr_str),
                                    "ipx:%02X%02X%02X%02X:%02X%02X%02X%02X%02X%02X:%hu",
                                    addr_buf->u.Ipx.sa_netnum[0],
                                    addr_buf->u.Ipx.sa_netnum[1],
                                    addr_buf->u.Ipx.sa_netnum[2],
                                    addr_buf->u.Ipx.sa_netnum[3],
                                    addr_buf->u.Ipx.sa_nodenum[0],
                                    addr_buf->u.Ipx.sa_nodenum[1],
                                    addr_buf->u.Ipx.sa_nodenum[2],
                                    addr_buf->u.Ipx.sa_nodenum[3],
                                    addr_buf->u.Ipx.sa_nodenum[4],
                                    addr_buf->u.Ipx.sa_nodenum[5],
                                    ipx_port );
                        break;

                    case AF_INET6 :
                        // *WORK*
                        _tcsncpy( addr_str, "ipv6:*WORK*:", array_sizeof(addr_str) );
                        addr_str[ array_sizeof(addr_str) - 1 ] = 0;
                        break;

                    default :
                        _tcsncpy( addr_str, "unknown", array_sizeof(addr_str) );
                        addr_str[ array_sizeof(addr_str) - 1 ] = 0;
                        break;
                    }
                }

                symres = msg_mgr_srv->ListenStartIPC( handler );

                if( SYM_FAILED(symres) )
                {
                    std::cout << "ERROR: Could not listen on IPC : " << std::hex << symres << std::endl;
                }

                while( g_quit == false )
                {
                    Sleep( 250 );

 					//If we have exceeded our runtime, then set the global quit flag
 					if ( ( serverLifetime > 0 ) && 
 						 ( ( GetTickCount() - startTime ) > serverLifetime ) )
 					{
 						g_quit = true;
 					}
                }
            }
            else
            {
                std::cout << "ERROR: Could not listen on port " << port << ": " << std::hex << symres << std::endl;
            }

            msg_mgr_srv->ListenStop();

            msg_mgr_srv->Deinitialize();

Server_Setup_Failed:

            std::vector<ScsSecureComms::ICert*>::iterator it = server_icerts.begin();

            while( it != server_icerts.end() )
            {
                (*it)->Release();
                ++it;
            }

            if( msg_mgr_srv != NULL )
                msg_mgr_srv->Release();

            if( handler != NULL )
                handler->Release();

            if( auth != NULL )
                auth->Release();
        }
        else
        {
            std::cout << "ERROR: Could not load IMessageManagerServer from " << g_path_to_scscomms << ": 0x" << std::hex << symres << "." << std::endl;
        }
    }
    else
    {
        std::cout << "ERROR: Could not locate ScsComms.dll at " << g_path_to_scscomms << ": 0x" << std::hex << symres << "." << std::endl;
    }
}

void do_client( const char* host_name, unsigned short port, char auth_type, unsigned long clientThreadCount, unsigned long clientThreadLifetime )
{
    SYMRESULT symres = SYM_OK;

	//Start time for the purpose of shutting down the main thread after a predetermined period.
	DWORD startTime;

    g_host_name = host_name;
    g_port = port;

    // Create an IMessageManager.

    MsgMgrLoader mm_ldr;
    CertLoader cert_ldr;
    AuthLoginLoader auth_ldr;

    symres = mm_ldr.Initialize( g_path_to_scscomms );

    if( symres == SYM_OK )
    {
        // Get a second handle to the DLL so that we can ensure that it stays loaded
        // until process exit.  Otherwise, memory leaks reported by the C-run time will
        // throw exceptions, because information about where they were allocated is unmapped
        // when the DLL is unloaded.

        HMODULE scscomms = LoadLibrary( g_path_to_scscomms );

        ScsSecureComms::RESULT rtn;

        ScsSecureComms::ICert* cert = NULL;
        ScsSecureComms::IByteStreamer* stream = NULL;
        ScsSecureComms::IAuth* auth = NULL;

        std::vector<HANDLE>::iterator it;
        std::vector<HANDLE> threads;
        HANDLE thread;

        const unsigned char* rootCert = NULL;

        std::vector<ustring> client_certs;
        std::vector<ScsSecureComms::ICert*> client_icerts;
        ustring client_key;

        // Create an IAuth object to represent the credentials we have and want on this connection.

        auth_ldr.Initialize( scscomms );

        auth_ldr.CreateObject( &auth );

        // Set remote auth.

        switch( auth_type )
        {
        case 'a' :
            auth->SetAuthTypeRemote( ScsSecureComms::IAuth::NO_AUTH );
            break;

        case 'n' :
            auth->SetAuthTypeRemote( ScsSecureComms::IAuth::NAME_AUTH );
            break;

        case 'r' :
            auth->SetAuthTypeRemote( ScsSecureComms::IAuth::ROOT_AUTH );
            break;

        default :
            SAVASSERT( false );
            break;
        }

        // Use the login credentials.

		if ( g_useDefaultCertsAndKeys )
		{
			// A login cert chain
			client_certs.push_back( user_admin_cert_pem );
			client_certs.push_back( login_ca_cert_pem );
			client_certs.push_back( server_group_root_cert_pem );
			client_key = user_admin_key_pem;
		}
		else
		{
            ustring tempCert;
			ustring tempKey;

            if( userCertFileName.empty() == false )
            {
				// Parse the user cert.  If this fails, use the default

                tempCert = parseCert( userCertFileName );
				if ( tempCert.empty() == false )
				{
					client_certs.push_back( tempCert );
				}
				else
				{
					std::cout << "Parsing the user login cert failed.  Using the default cert." << std::endl;
					client_certs.push_back( user_admin_cert_pem );
				}

                if( loginCACertFileName.empty() == false )
                {
				    // Parse the login ca cert.  If this fails, use the default

                    tempCert = parseCert( loginCACertFileName );
				    if ( tempCert.empty() == false )
				    {
					    client_certs.push_back( tempCert );
				    }
				    else
				    {
					    std::cout << "Parsing the login ca cert failed.  Using the default cert." << std::endl;
					    client_certs.push_back( login_ca_cert_pem );
				    }
                }

                if( rootCACertFileNames.empty() == false )
                {
				    // Parse the root cert.  If this fails, use the default

                    tempCert = parseCert( rootCACertFileNames.at( 0 ) );
				    if ( tempCert.empty() == false )
				    {
					    client_certs.push_back( tempCert );
				    }
				    else
				    {
					    std::cout << "Parsing the root ca cert failed.  Using the default cert." << std::endl;
					    client_certs.push_back( server_group_root_cert_pem );
				    }
                }

				// Parse the user login private key.  If this fails, use the default

                tempKey = parseKey( userPrivateKeyFileName );
				if ( tempKey.empty() == false )
				{
					client_key = tempKey;
				}
				else
				{
					std::cout << "Parsing the user login private key failed.  Using the default private key." << std::endl;
					client_key = user_admin_key_pem;
				}
            }
            else
			{
                // This is ok -- basically we are going to act like a SAV Client and present no credentials when we connect..

				std::cout << "Client connecting without certs." << std::endl;
			}
        }

        if( client_certs.empty() == false )
        {
            if( get_certs( scscomms, client_certs, client_key, client_icerts ) == false )
            {
                goto Client_Setup_Failed;
            }
        }

        try
        {
            ScsSecureComms::ICert **icerts_array = new ScsSecureComms::ICert *[client_icerts.size()];

            std::vector<ScsSecureComms::ICert*>::iterator iter = client_icerts.begin();

            for (int cnt = 0; iter != client_icerts.end(); ++iter, ++cnt)
                icerts_array[cnt] = *iter;

            rtn = auth->SetCertChain( icerts_array, client_icerts.size() );

            delete[] icerts_array;
        }
        catch (std::bad_alloc &)
        {
            goto Client_Setup_Failed;
        }

        if( rtn != ScsSecureComms::RTN_OK )
        {
            std::cout << "SetCertChain failed: " << std::hex << rtn << std::endl;
            goto Client_Setup_Failed;
        }

 		// Save off the time we started the client threads

 		startTime = GetTickCount();

        // Crank up our threads.

        ScsSecureComms::IMessageManager* msg_mgr = NULL;

        for( unsigned long n = 0; n < clientThreadCount; ++n )
        {
            ClientThreadParams* params = new ClientThreadParams();

            // Create a new MessageManager for each thread, if necessary.

            if( msg_mgr == NULL )
            {
                symres = mm_ldr.CreateObject( &msg_mgr );

                if( SYM_FAILED(symres) )
                {
                    std::cout << "ERROR: Could not load IMessageManagerServer from " << g_path_to_scscomms << ": 0x" << std::hex << symres << "." << std::endl;
                    goto Client_Setup_Failed;
                }

                rtn = msg_mgr->Initialize();

                if( rtn != ScsSecureComms::RTN_OK )
                {
                    std::cout << "IMessageManager::Initialize failed: " << std::hex << rtn << std::endl;
                    goto Client_Setup_Failed;
                }

                // Add trusted root certs.

                add_trusted_root_certs( scscomms, msg_mgr );
            }

            params->m_msg_mgr = msg_mgr;

            if( clientSharedMsgMgr == false )
            {
                msg_mgr = NULL; // client thread owns
            }
            else
            {
                msg_mgr->AddRef(); // client thread shares
            }

            auth->AddRef();
            params->m_credentials = auth;

            params->m_index = n;

            thread = reinterpret_cast<HANDLE>( _beginthreadex( NULL, 0, client_thread, params, 0, NULL ) );

            threads.push_back( thread );
        }

        while( g_quit == false )
        {
            Sleep( 250 );

 			//If we have exceeded our runtime, then set the global quit flag
 			if ( ( clientThreadLifetime > 0 ) && 
 				 ( ( GetTickCount() - startTime ) > clientThreadLifetime ) )
 			{
 				g_quit = true;
 			}
        }

        // Clean up our threads.

        try
        {
            HANDLE *threads_array = new HANDLE[threads.size()];

            it = threads.begin();

            for (int cnt = 0; it != threads.end(); ++it, ++cnt)
                threads_array[cnt] = *it;

            WaitForMultipleObjects( threads.size(), threads_array, TRUE, INFINITE );

            delete[] threads_array;
        }
        catch (std::bad_alloc &)
        {
        }

        it = threads.begin();

        while( it != threads.end() )
        {
            CloseHandle( *it );
            ++it;
        }

Client_Setup_Failed:

        if( msg_mgr != NULL )
        {
            if( clientSharedMsgMgr == true )
            {
                msg_mgr->Deinitialize();
            }

            msg_mgr->Release();
        }

        std::vector<ScsSecureComms::ICert*>::iterator it_certs = client_icerts.begin();
        while( it_certs != client_icerts.end() )
        {
            (*it_certs)->Release();
            ++it_certs;
        }

        if( cert != NULL )
            cert->Release();

        if( auth != NULL )
            auth->Release();
    }
    else
    {
        std::cout << "ERROR: Could not locate ScsComms.dll at " << g_path_to_scscomms << ": 0x" << std::hex << symres << "." << std::endl;
    }
}

/*
 * Read in a X509 pem encoded certificate from disk as a string and return it.
 */
ustring parseCert( const std::string& certFileName )
{
	ustring tempCert;

	std::ifstream certFile( certFileName.c_str() );

	//If the cert file is null, then it wasn't found
	if ( ( certFile != NULL ) )
	{
		//Check for any I/O errors
		if ( certFile.good() )
		{
            //Read in the file on char at a time.
            std::ifstream::int_type temp;
			while ( (temp = certFile.get()) != -1 )
			{
                tempCert += temp;
			}
		}
		else
		{
			std::cout << certFileName << " was found, but there was an error opening it." << std::endl;
		}
	}
	else
	{
		std::cout << certFileName << " was not found." << std::endl;
	}

	return tempCert;
}

/*
 * Read in a RSA pem encoded private key from disk as a string and return it.
 */
ustring parseKey( const std::string& keyFileName )
{
	ustring tempKey;

    std::ifstream keyFile( keyFileName.c_str() );

	//If the key file is null, then it wasn't found
	if ( ( keyFile != NULL ) )
	{
		if ( keyFile.good() )
		{
            //Read in the file on char at a time.
            std::ifstream::int_type temp;
			while ( (temp = keyFile.get()) != -1 )
			{
                tempKey += temp;
			}
		}
		else
		{
			std::cout << keyFileName << " was found, but there was an error opening it." << std::endl;
		}
	}
	else
	{
		std::cout << keyFileName << " was not found." << std::endl;
	}

	return tempKey;
}

int main( int argc, char* argv[] )
{
	//Our configuration file
	Configuration configuration;

	//Configuration parameters, read either from command line or from a config file
	unsigned long clientThreadLifetime	= 0;
	unsigned long clientThreadCount		= 0;
	unsigned long serverLifetime		= 0;
    unsigned long temp_port				= 0;
	unsigned short port					= 0;
    std::string mode;
	std::string host_name       		= "127.0.0.1";
    std::string auth_type;

    char* end_ptr						= NULL;
	int tmpFlag							= _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
	tmpFlag							   |= _CRTDBG_LEAK_CHECK_DF;
//	tmpFlag		 |= _CRTDBG_CHECK_CRT_DF;

	_CrtSetDbgFlag( tmpFlag );

    srand( time(NULL) | GetCurrentThreadId() );

	// Read from the config file, then let command line params override if specified.

	if ( configuration.read() )
	{
		std::cout << "Attempting to read from configuration file..." << std::endl;

		mode = configuration.getValue( "mode" ).c_str();

		//Unrecoverable
		if ( mode != "c" && mode != "C" && mode != "s" && mode != "S" )
		{
			std::cout << "ERROR: Mode was not specified in the configuration file.  Exiting.";
			return( -1 );
		}

		host_name			  = configuration.getValue( "host", "127.0.0.1" ).c_str();
		temp_port			  = strtoul( configuration.getValue( "port", "0" ).c_str(), &end_ptr, 10 );
		clientThreadCount	  = strtoul( configuration.getValue( "client_thread_count", "1" ).c_str(), &end_ptr, 10 );
		clientThreadLifetime  = strtoul( configuration.getValue( "client_thread_life_time", "0" ).c_str(), &end_ptr, 10 );
		serverLifetime		  = strtoul( configuration.getValue( "server_life_time", "0" ).c_str(), &end_ptr, 10 );
        clientChannel         = configuration.getValue( "client_channel", "ipc" ).c_str();
        clientTimeout         = strtoul( configuration.getValue( "client_timeout", "0" ).c_str(), &end_ptr, 10 );
        clientSharedMsgMgr    = strtoul( configuration.getValue( "client_thread_share", "0" ).c_str(), &end_ptr, 10 ) == 0 ? false : true;
        g_delay               = strtoul( configuration.getValue( "delay", "0" ).c_str(), &end_ptr, 10 );
        g_printRoles          = strtoul( configuration.getValue( "print_roles", "0" ).c_str(), &end_ptr, 10 ) == 0 ? false : true;
        g_msgMaxSize          = strtoul( configuration.getValue( "msg_size_max", "2048" ).c_str(), &end_ptr, 10 );
        g_msgMinSize          = strtoul( configuration.getValue( "msg_size_min", "0" ).c_str(), &end_ptr, 10 );

        // Range check message sizes.

        if( g_msgMinSize < sizeof(VDTM_HEADER) )
        {
            g_msgMinSize = sizeof(VDTM_HEADER);
        }

        if( g_msgMinSize > g_msgMaxSize )
        {
            g_msgMaxSize = g_msgMinSize;
        }

        // Set default auth type based on mode.

        if( mode == "s" || mode == "S" )
        {
            auth_type         = configuration.getValue( "auth", "a" ).c_str();
        }
        else
        {
            auth_type         = configuration.getValue( "auth", "r" ).c_str();
        }

        if( auth_type != "r" && auth_type != "a" && auth_type != "n" )
        {
			std::cout << "ERROR: Authorization type invalid (" << auth_type << ") in the configuration file.  Exiting.";
			return( -1 );
        }

		if ( configuration.getValue( "use_default_certs_and_keys", "false" ) == "false" )
		{
			g_useDefaultCertsAndKeys = false;
		}

		if ( ! g_useDefaultCertsAndKeys )
		{
			std::cout << "Using external certificates and keys." << std::endl;

            certStore                 = configuration.getValue( "cert_store" );
			loginCACertFileName		  = configuration.getValue( "login_ca_cert" );
            if( loginCACertFileName.empty() == false ) loginCACertFileName.insert( 0, certStore );
			loginCAPrivateKeyFileName = configuration.getValue( "login_ca_pk" );
            if( loginCAPrivateKeyFileName.empty() == false ) loginCAPrivateKeyFileName.insert( 0, certStore );
			serverCertFileName		  = configuration.getValue( "server_cert" );
            if( serverCertFileName.empty() == false ) serverCertFileName.insert( 0, certStore );
			serverPrivateKeyFileName  = configuration.getValue( "server_pk" );
            if( serverPrivateKeyFileName.empty() == false ) serverPrivateKeyFileName.insert( 0, certStore );

            std::string rootCACertFileName;
            std::string rootCAPrivateKeyFileName;
            char rootOption[MAX_PATH];
            int counter = 0;
            do
            {
                sssnprintf( rootOption, sizeof(rootOption), "root_ca_cert_%d", counter );
                rootOption[ array_sizeof(rootOption) - 1 ] = 0;
                rootCACertFileName = configuration.getValue( rootOption );
                if( rootCACertFileName.empty() == false )
                {
                    rootCACertFileName.insert( 0, certStore );
                    rootCACertFileNames.push_back( rootCACertFileName );
                }
                sssnprintf( rootOption, sizeof(rootOption), "root_ca_pk_%d", counter );
                rootOption[ array_sizeof(rootOption) - 1 ] = 0;
			    rootCAPrivateKeyFileName  = configuration.getValue( rootOption );
                if( rootCAPrivateKeyFileName.empty() == false )
                {
                    rootCAPrivateKeyFileName.insert( 0, certStore );
                    rootCAPrivateKeyFileNames.push_back( rootCAPrivateKeyFileName );
                }
                ++counter;
            }
            while( rootCACertFileName.length() > 0 );
			userCertFileName		  = configuration.getValue( "user_cert" );
            if( userCertFileName.empty() == false ) userCertFileName.insert( 0, certStore );
			userPrivateKeyFileName    = configuration.getValue( "user_pk" );
            if( userPrivateKeyFileName.empty() == false ) userPrivateKeyFileName.insert( 0, certStore );
		}

		std::cout << "Done reading configuration file." << std::endl;
	}
    else
    {
	    if ( argc < 3 )
	    {
		    print_usage();
		    return( -1 );
	    }
    }

	// Parse the command line params

    if( argc == 1 )
        goto CmdLine_Done;

	//mode is always first
	mode = argv[1];

    if( argc == 2 )
        goto CmdLine_Done;

	//port is always second
	temp_port = strtoul( argv[2], &end_ptr, 10 );
    
	if ( errno == ERANGE ) 
	{
		print_usage(); 
		return( -1 );
	}

    if( argc == 3 )
        goto CmdLine_Done;

	auth_type = "a";

	//If we're a server with a 3rd parameter, its lifetime
 	if( mode[0] == 's' || mode[0] == 'S' )
 	{
 		serverLifetime = strtoul( argv[3], &end_ptr, 10 );

		if ( errno == ERANGE ) 
		{ 
			print_usage(); 
			return( -1 );
		}
    }
	else if( mode[0] == 'c' || mode[0] == 'C' )
	{
		//If we're a client, ip address/host name is 3rd
		host_name = argv[3];

        if( argc == 4 )
            goto CmdLine_Done;

		//thread count is 4th
		clientThreadCount = strtoul( argv[4], &end_ptr, 10 );

		if ( errno == ERANGE ) 
		{ 
			print_usage(); 
			return( -1 );
		}

        if( argc == 4 )
            goto CmdLine_Done;

		//If we have a 5th argument, its lifetime
 		clientThreadLifetime = strtoul( argv[5], &end_ptr, 10 );

		if ( errno == ERANGE ) 
		{ 
			print_usage(); 
			return( -1 );
		}
	}

CmdLine_Done:

	//Input verification (both in the config file case and the command line parameter case
	if( ( temp_port >= USHRT_MAX /*|| end_ptr != argv[2] + strlen(argv[2])*/ ) ||
        ( serverLifetime >= ULONG_MAX /*|| end_ptr != argv[3] + strlen(argv[3])*/ )				 ||
		( clientThreadCount >= ULONG_MAX /*|| end_ptr != argv[4] + strlen(argv[4])*/ )			 ||
		( clientThreadLifetime >= ULONG_MAX /*|| end_ptr != argv[5] + strlen(argv[5])*/ ) )
	{
		print_usage();
		return( -1 );
	}

	port = static_cast<unsigned short>(temp_port);

    // Register a console ctrl handler so that we can gracefully exit on Ctrl-C or System Menu cmds.
    SetConsoleCtrlHandler( console_ctrl_handler, TRUE );

    // Switch on server or client mode.
    if( _strnicmp( mode.c_str(), "s", mode.length() ) == 0 )
    {
        // We are a server.
        std::cout << "Server mode listening on " << port;
 		
 		if ( serverLifetime > 0 )
 		{
 			std::cout << " for " << serverLifetime << " seconds";

			//Convert to milliseconds
			serverLifetime *= 1000;
 		}
 
 		std::cout << "." << std::endl;

        do_server( port, auth_type[0], serverLifetime );
    }
    else if( _strnicmp( mode.c_str(), "c", mode.length() ) == 0 )
    {
        // We are a client.

        std::cout << "Client mode running " << clientThreadCount << " threads on connection to " << host_name <<":" << port;
		
 		if ( clientThreadLifetime > 0 )
 		{
 			std::cout << " for " << clientThreadLifetime << " seconds";

			//Convert to milliseconds
			clientThreadLifetime *= 1000;
 		}

		std::cout << "." << std::endl;

        do_client( host_name.c_str(), port, auth_type[0], clientThreadCount, clientThreadLifetime );
    }
    else
    {
        print_usage();
        return( -1 );
    }

    return( 0 );
}

