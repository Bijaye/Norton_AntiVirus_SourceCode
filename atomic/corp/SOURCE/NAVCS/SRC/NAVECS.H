// ----------------------------------------------------------------------------
// Norton AntiVirus for Microsoft Exchange
// Copyright (c) 1999
// ----------------------------------------------------------------------------
//
// FILE:     navecs.h
//  
// MODULE:   navecs.dll (Nighthawk Setup Helper DLL)
//
// DESCRIPTION:
//      Contains Nighthawk functions to assist the installshield script.
//
// ----------------------------------------------------------------------------

#define EXPORT
#define EXPORT32 __declspec(dllexport)
#define CDECLEXPORTAPI EXPORT32 int __cdecl EXPORT

#define NAVE_SETUP_SUCCESS 0
// Unknown failure.
#define NAVE_SETUP_FAILURE 2

#if 1
#define NAVE_SETUP_LOGON_FAILED 3

#define NAVE_SETUP_IMPERSONATE_FAILED 4

#define NAVE_SETUP_ACCOUNT_NOT_ADMIN 5

#define NAVE_SETUP_BUFFER_TOO_SMALL 6

#define NAVE_SETUP_NET_INIT_FAILED 7

#define NAVE_SETUP_NET_UNSUPPORTED 8 

#define NAVE_SETUP_NET_PORT_IN_USE 9 

#define NAVE_SETUP_NET_FAILURE 10

#define NAVE_SETUP_MAPI_INIT_FAILURE 11

#define NAVE_SETUP_MAPI_ADMIN_FAILURE 12

#define NAVE_SETUP_MAPI_PROFILE_EXISTS 13

#define NAVE_SETUP_MAPI_PROFILE_CREATION_FAILED 14

#define NAVE_SETUP_MAPI_CONFIG_FAILED 15

#define NAVE_SETUP_MAPI_BAD_MAILBOX 16

#define NAVE_SETUP_DAPI_FAILURE 17

#define NAVE_SETUP_DAPI_MAILBOX_EXISTS 18

#define NAVE_SETUP_USER_CANCEL 19

#define NAVE_SETUP_MAPI_NO_SERVICE_PROVIDER 20

#define NAVE_SETUP_DAPI_NO_PRIVATE_STORE 21

#endif
//
// Exported functions
//

extern "C" 
{

CDECLEXPORTAPI fnEnableDebugMessages( BOOL bEnableContext = FALSE );

CDECLEXPORTAPI fnDisableDebugMessages();

}

//
// Internal functions
//
DWORD CopyW2A( LPSTR strDest,  DWORD dwDestSize, LPCWSTR strSrc);
DWORD CopyA2W( LPWSTR strDest, DWORD dwDestSize, LPCSTR strSrc);
DWORD CopyW2W( LPWSTR strDest, DWORD dwDestSize, LPCWSTR strSrc);
DWORD CatW2W( LPWSTR wcsDest, DWORD dwDestSize, LPCWSTR wcsSrc );


#define SAFE_STRING(x)  ( (x) ? (x) : ("") )

#if 1
    void fnDisplayWinError( LPCSTR strContext, DWORD dwError, LPCSTR strFile, const int iLine);
    void fnDisplayHRESULT( LPCSTR strContext, HRESULT hr, LPCSTR strFile, const int iLine);
    void fnDisplayLastError(LPCSTR strContext, LPCSTR strFile, const int iLine );
    void fnDisplayDebugMessageA(LPCSTR strMsg, LPCSTR strFile, const int iLine );
    void fnDisplayDebugMessageW(LPCWSTR strMsg,  LPCSTR strFile, const int iLine );

    #define TRACE_LAST_ERROR(x) fnDisplayLastError(x, __FILE__, __LINE__)
    #define TRACE_ERROR(x, err) fnDisplayWinError(x, err, __FILE__, __LINE__)
    #define TRACE_HRESULT(x, err) fnDisplayHRESULT(x, err, __FILE__, __LINE__)

    #define DEBUG_MSG_W( x )    fnDisplayDebugMessageW(x, __FILE__, __LINE__)
    #define DEBUG_MSG_A( x )    fnDisplayDebugMessageA(x, __FILE__, __LINE__)
    #define DEBUG_MSG( x )      DEBUG_MSG_A( x )
#else
    #define TRACE_LAST_ERROR(x)
    #define TRACE_ERROR(x, err)

    #define DEBUG_MSG_W( x )  
    #define DEBUG_MSG_A( x )
    #define DEBUG_MSG( x )
#endif


#ifdef _DEBUG
    #define TRACE_ENTER( x )    DEBUG_MSG_A( "Enter function: " #x );
    #define TRACE_EXIT( x )     DEBUG_MSG_A( "Exit function: " #x );
    #define TRACE_VALUE( x )    { char szMsg[1024]; wsprintf(szMsg, #x " = %d\n", (x) ); OutputDebugString(szMsg); }
    #define TRACE_STRING( x )   { char szMsg[1024]; wsprintf(szMsg, #x " = %s\n", (x) ); OutputDebugString(szMsg); }
    #define TRACE_STRING_W( x ) { WCHAR szMsg[1024]; wsprintfW(szMsg, L#x L" = %s\n", (x) ); OutputDebugStringW(szMsg); }
#else
    #define TRACE_ENTER( x )
    #define TRACE_EXIT( x )
    #define TRACE_VALUE( x )
    #define TRACE_STRING( x )
    #define TRACE_STRING_W( x )
#endif

