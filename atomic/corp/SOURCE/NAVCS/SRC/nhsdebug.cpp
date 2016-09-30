#define STRICT 1

//stop L4 warnings from Windows include files
#pragma warning(disable:4201) //nonstandard extension used
#pragma warning(disable:4514) //unreferenced inline function
#pragma warning(disable:4214) //nonstandard extension used

#pragma warning(disable:4710) // function 'x' not expanded
#pragma warning(disable:4711) // function 'x' selected for inline expansion

#include <windows.h>
#include <tchar.h>
#include <mapicode.h>

#include "nhsetup.h"

#ifndef _DEBUG
BOOL g_bEnableDebugMessages = FALSE;
#else
BOOL g_bEnableDebugMessages = TRUE;
#endif

BOOL g_bEnableDebugMessageContext = FALSE;

CDECLEXPORTAPI fnEnableDebugMessages( BOOL bEnableContext )
{
    g_bEnableDebugMessages = TRUE;
    g_bEnableDebugMessageContext = bEnableContext;
    return TRUE;
}

CDECLEXPORTAPI fnDisableDebugMessages()
{
    g_bEnableDebugMessages = FALSE;
    return TRUE;
}

#define MAX_DEBUG_MSG 1024

void fnDisplayDebugMessageA( LPCSTR strMsg, LPCSTR strFile, int iLine )
{
    if (g_bEnableDebugMessages)
    {
        DWORD dwLastError = GetLastError();
        
        char strText[MAX_DEBUG_MSG];

        lstrcpyn( strText, strMsg, MAX_DEBUG_MSG );

        if (g_bEnableDebugMessageContext)
        {
            // append context information
        }

        MessageBox(NULL, strText, "DEBUG", MB_OK | MB_ICONINFORMATION );

        SetLastError( dwLastError );
    }
}

void fnDisplayDebugMessageW( LPCWSTR strMsg, LPCSTR strFile, int iLine )
{
    if (g_bEnableDebugMessages)
    {
        DWORD dwLastError = GetLastError();
        
        DWORD dwResult = ERROR_SUCCESS;
        if (dwResult)
        {
            char strMsgA[MAX_DEBUG_MSG];
            dwResult = CopyW2A( strMsgA, MAX_DEBUG_MSG, strMsg );
            if (dwResult != ERROR_SUCCESS)
            {
                lstrcpyn( strMsgA, "Error", MAX_DEBUG_MSG );
            }
            fnDisplayDebugMessageA( strMsgA, strFile, iLine );
        }

        SetLastError( dwLastError );
    }
}



void fnDisplayLastError( LPCSTR strContext, LPCSTR strFile, const int iLine)
{
    DWORD dwError = GetLastError();
    fnDisplayWinError( strContext, dwError, strFile, iLine );
}

void fnDisplayWinError( LPCSTR strContext, DWORD dwError, LPCSTR strFile, const int iLine)
{
    DWORD dwLastError = GetLastError();

    LPTSTR lpMsgBuf = NULL;
    DWORD nByteCount = FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL 
    );

    if (nByteCount == 0)
    {
        lpMsgBuf = NULL;
    }

    // Process any inserts in lpMsgBuf.
    // ...
    // Display the string.
    

    char strMsg[MAX_DEBUG_MSG * 2];
    
    wsprintf( strMsg, "%s (0x%x): %s", strContext, dwError, SAFE_STRING(lpMsgBuf) );

    fnDisplayDebugMessageA( strMsg, strFile, iLine );
    
    
    // Free the buffer.
    LocalFree( lpMsgBuf );
    
    SetLastError(dwLastError);
}


/*
void DisplayWinError( 
    LPSTR szAPI,
    DWORD WinError
    )
{ 
    LPSTR MessageBuffer;
    DWORD dwBufferLength;

	dwBufferLength = FormatMessageA(
                        FORMAT_MESSAGE_ALLOCATE_BUFFER |
                        FORMAT_MESSAGE_FROM_SYSTEM,
                        NULL,
                        WinError,
                        GetUserDefaultLangID(),
                        (LPSTR) &MessageBuffer,
                        0,
                        NULL
                        );
    if(dwBufferLength)
    {
		MessageBox(NULL,MessageBuffer,szAPI,MB_OK);

        //
        // Free the buffer allocated by the system.
        //
        LocalFree(MessageBuffer);
    }
}


LPSTR GetLastErrorText( LPSTR lpszBuf, DWORD dwSize ) 
{
    LPTSTR lpszTemp = 0;

    DWORD dwRet =	::FormatMessage(
						FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
						0,
						GetLastError(),
						LANG_NEUTRAL,
						(LPTSTR)&lpszTemp,
						0,
						0
					);

    if( !dwRet || (dwSize < dwRet+14) )
        lpszBuf[0] = TEXT('\0');
    else {
        lpszTemp[_tcsclen(lpszTemp)-2] = TEXT('\0');  //remove cr/nl characters
        _tcscpy(lpszBuf, lpszTemp);
    }

    if( lpszTemp )
        LocalFree(HLOCAL(lpszTemp));

    return lpszBuf;
}
*/

void fnDisplayHRESULT( LPCSTR strContext, HRESULT hr, LPCSTR strFile, const int iLine)
{
    DWORD dwLastError = GetLastError();

    LPCSTR  szErr;

    switch(hr)
    {        
        case E_FAIL                             : szErr = "E_FAIL";                         break;
        case E_INVALIDARG                       : szErr = "E_INVALIDARG";                   break;
        case E_NOINTERFACE                      : szErr = "E_NOINTERFACE";                  break;
        case E_OUTOFMEMORY                      : szErr = "E_OUTOFMEMORY";                  break;
        case E_ACCESSDENIED                     : szErr = "E_ACCESSDENIED";                 break;

#ifndef TRACES_NO_MAPI
        case MAPI_E_NO_SUPPORT                  : szErr = "MAPI_E_NO_SUPPORT";              break;
        case MAPI_E_BAD_CHARWIDTH               : szErr = "MAPI_E_BAD_CHARWIDTH";           break;
        case MAPI_E_STRING_TOO_LONG             : szErr = "MAPI_E_STRING_TOO_LONG";         break;
        case MAPI_E_UNKNOWN_FLAGS               : szErr = "MAPI_E_UNKNOWN_FLAGS";           break;
        case MAPI_E_INVALID_ENTRYID             : szErr = "MAPI_E_INVALID_ENTRYID";         break;
        case MAPI_E_INVALID_OBJECT              : szErr = "MAPI_E_INVALID_OBJECT";          break;
        case MAPI_E_OBJECT_CHANGED              : szErr = "MAPI_E_OBJECT_CHANGED";          break;
        case MAPI_E_OBJECT_DELETED              : szErr = "MAPI_E_OBJECT_DELETED";          break;
        case MAPI_E_BUSY                        : szErr = "MAPI_E_BUSY";                    break;
        case MAPI_E_NOT_ENOUGH_DISK             : szErr = "MAPI_E_NOT_ENOUGH_DISK";         break;
        case MAPI_E_NOT_ENOUGH_RESOURCES        : szErr = "MAPI_E_NOT_ENOUGH_RESOURCES";    break;
        case MAPI_E_NOT_FOUND                   : szErr = "MAPI_E_NOT_FOUND";               break;
        case MAPI_E_VERSION                     : szErr = "MAPI_E_VERSION";                 break;
        case MAPI_E_LOGON_FAILED                : szErr = "MAPI_E_LOGON_FAILED";            break;
        case MAPI_E_SESSION_LIMIT               : szErr = "MAPI_E_SESSION_LIMIT";           break;
        case MAPI_E_USER_CANCEL                 : szErr = "MAPI_E_USER_CANCEL";             break;
        case MAPI_E_UNABLE_TO_ABORT             : szErr = "MAPI_E_UNABLE_TO_ABORT";         break;
        case MAPI_E_NETWORK_ERROR               : szErr = "MAPI_E_NETWORK_ERROR";           break;
        case MAPI_E_DISK_ERROR                  : szErr = "MAPI_E_DISK_ERROR";              break;
        case MAPI_E_TOO_COMPLEX                 : szErr = "MAPI_E_TOO_COMPLEX";             break;
        case MAPI_E_BAD_COLUMN                  : szErr = "MAPI_E_BAD_COLUMN";              break;
        case MAPI_E_EXTENDED_ERROR              : szErr = "MAPI_E_EXTENDED_ERROR";          break;
        case MAPI_E_COMPUTED                    : szErr = "MAPI_E_COMPUTED";                break;
        case MAPI_E_CORRUPT_DATA                : szErr = "MAPI_E_CORRUPT_DATA";            break;
        case MAPI_E_UNCONFIGURED                : szErr = "MAPI_E_UNCONFIGURED";            break;
        case MAPI_E_FAILONEPROVIDER             : szErr = "MAPI_E_FAILONEPROVIDER";         break;
        case MAPI_E_END_OF_SESSION              : szErr = "MAPI_E_END_OF_SESSION";          break;
        case MAPI_E_UNKNOWN_ENTRYID             : szErr = "MAPI_E_UNKNOWN_ENTRYID";         break;
        case MAPI_E_MISSING_REQUIRED_COLUMN     : szErr = "MAPI_E_MISSING_REQUIRED_COLUMN"; break;
        case MAPI_W_NO_SERVICE                  : szErr = "MAPI_W_NO_SERVICE";              break;
        case MAPI_E_BAD_VALUE                   : szErr = "MAPI_E_BAD_VALUE";               break;
        case MAPI_E_INVALID_TYPE                : szErr = "MAPI_E_INVALID_TYPE";            break;
        case MAPI_E_TYPE_NO_SUPPORT             : szErr = "MAPI_E_TYPE_NO_SUPPORT";         break;
        case MAPI_E_UNEXPECTED_TYPE             : szErr = "MAPI_E_UNEXPECTED_TYPE";         break;
        case MAPI_E_TOO_BIG                     : szErr = "MAPI_E_TOO_BIG";                 break;
        case MAPI_E_DECLINE_COPY                : szErr = "MAPI_E_DECLINE_COPY";            break;
        case MAPI_E_UNEXPECTED_ID               : szErr = "MAPI_E_UNEXPECTED_ID";           break;
        case MAPI_W_ERRORS_RETURNED             : szErr = "MAPI_W_ERRORS_RETURNED";         break;
        case MAPI_E_UNABLE_TO_COMPLETE          : szErr = "MAPI_E_UNABLE_TO_COMPLETE";      break;
        case MAPI_E_TIMEOUT                     : szErr = "MAPI_E_TIMEOUT";                 break;
        case MAPI_E_TABLE_EMPTY                 : szErr = "MAPI_E_TABLE_EMPTY";             break;
        case MAPI_E_TABLE_TOO_BIG               : szErr = "MAPI_E_TABLE_TOO_BIG";           break;
        case MAPI_E_INVALID_BOOKMARK            : szErr = "MAPI_E_INVALID_BOOKMARK";        break;
        case MAPI_W_POSITION_CHANGED            : szErr = "MAPI_W_POSITION_CHANGED";        break;
        case MAPI_W_APPROX_COUNT                : szErr = "MAPI_W_APPROX_COUNT";            break;
        case MAPI_E_WAIT                        : szErr = "MAPI_E_WAIT";                    break;
        case MAPI_E_CANCEL                      : szErr = "MAPI_E_CANCEL";                  break;
        case MAPI_E_NOT_ME                      : szErr = "MAPI_E_NOT_ME";                  break;
        case MAPI_W_CANCEL_MESSAGE              : szErr = "MAPI_W_CANCEL_MESSAGE";          break;
        case MAPI_E_CORRUPT_STORE               : szErr = "MAPI_E_CORRUPT_STORE";           break;
        case MAPI_E_NOT_IN_QUEUE                : szErr = "MAPI_E_NOT_IN_QUEUE";            break;
        case MAPI_E_NO_SUPPRESS                 : szErr = "MAPI_E_NO_SUPPRESS";             break;
        case MAPI_E_COLLISION                   : szErr = "MAPI_E_COLLISION";               break;
        case MAPI_E_NOT_INITIALIZED             : szErr = "MAPI_E_NOT_INITIALIZED";         break;
        case MAPI_E_NON_STANDARD                : szErr = "MAPI_E_NON_STANDARD";            break;
        case MAPI_E_NO_RECIPIENTS               : szErr = "MAPI_E_NO_RECIPIENTS";           break;
        case MAPI_E_SUBMITTED                   : szErr = "MAPI_E_SUBMITTED";               break;
        case MAPI_E_HAS_FOLDERS                 : szErr = "MAPI_E_HAS_FOLDERS";             break;
        case MAPI_E_HAS_MESSAGES                : szErr = "MAPI_E_HAS_MESSAGES";            break;
        case MAPI_E_FOLDER_CYCLE                : szErr = "MAPI_E_FOLDER_CYCLE";            break;
        case MAPI_W_PARTIAL_COMPLETION          : szErr = "MAPI_W_PARTIAL_COMPLETION";      break;
        case MAPI_E_AMBIGUOUS_RECIP             : szErr = "MAPI_E_AMBIGUOUS_RECIP";         break;
        
        #ifndef MAPI_E_UNKNOWN_CPID
        #define MAPI_E_UNKNOWN_CPID             MAKE_MAPI_E( 0x11E )
        #define MAPI_E_UNKNOWN_LCID             MAKE_MAPI_E( 0x11F )
        #endif
        case MAPI_E_UNKNOWN_CPID                : szErr = "MAPI_E_UNKNOWN_CPID";            break;
        case MAPI_E_UNKNOWN_LCID                : szErr = "MAPI_E_UNKNOWN_LCID";            break;
#endif // TRACES_NO_MAPI

        case STG_E_INVALIDFUNCTION              : szErr = "STG_E_INVALIDFUNCTION";          break;
        case STG_E_FILENOTFOUND                 : szErr = "STG_E_FILENOTFOUND";             break;
        case STG_E_PATHNOTFOUND                 : szErr = "STG_E_PATHNOTFOUND";             break;
        case STG_E_TOOMANYOPENFILES             : szErr = "STG_E_TOOMANYOPENFILES";         break;
        case STG_E_ACCESSDENIED                 : szErr = "STG_E_ACCESSDENIED";             break;
        case STG_E_INVALIDHANDLE                : szErr = "STG_E_INVALIDHANDLE";            break;
        case STG_E_INSUFFICIENTMEMORY           : szErr = "STG_E_INSUFFICIENTMEMORY";       break;
        case STG_E_INVALIDPOINTER               : szErr = "STG_E_INVALIDPOINTER";           break;
        case STG_E_NOMOREFILES                  : szErr = "STG_E_NOMOREFILES";              break;
        case STG_E_DISKISWRITEPROTECTED         : szErr = "STG_E_DISKISWRITEPROTECTED";     break;
        case STG_E_SEEKERROR                    : szErr = "STG_E_SEEKERROR";                break;
        case STG_E_WRITEFAULT                   : szErr = "STG_E_WRITEFAULT";               break;
        case STG_E_READFAULT                    : szErr = "STG_E_READFAULT";                break;
        case STG_E_SHAREVIOLATION               : szErr = "STG_E_SHAREVIOLATION";           break;
        case STG_E_LOCKVIOLATION                : szErr = "STG_E_LOCKVIOLATION";            break;
        case STG_E_FILEALREADYEXISTS            : szErr = "STG_E_FILEALREADYEXISTS";        break;
        case STG_E_INVALIDPARAMETER             : szErr = "STG_E_INVALIDPARAMETER";         break;
        case STG_E_MEDIUMFULL                   : szErr = "STG_E_MEDIUMFULL";               break;
        case STG_E_ABNORMALAPIEXIT              : szErr = "STG_E_ABNORMALAPIEXIT";          break;
        case STG_E_INVALIDHEADER                : szErr = "STG_E_INVALIDHEADER";            break;
        case STG_E_INVALIDNAME                  : szErr = "STG_E_INVALIDNAME";              break;
        case STG_E_UNKNOWN                      : szErr = "STG_E_UNKNOWN";                  break;
        case STG_E_UNIMPLEMENTEDFUNCTION        : szErr = "STG_E_UNIMPLEMENTEDFUNCTION";    break;
        case STG_E_INVALIDFLAG                  : szErr = "STG_E_INVALIDFLAG";              break;
        case STG_E_INUSE                        : szErr = "STG_E_INUSE";                    break;
        case STG_E_NOTCURRENT                   : szErr = "STG_E_NOTCURRENT";               break;
        case STG_E_REVERTED                     : szErr = "STG_E_REVERTED";                 break;
        case STG_E_CANTSAVE                     : szErr = "STG_E_CANTSAVE";                 break;
        case STG_E_OLDFORMAT                    : szErr = "STG_E_OLDFORMAT";                break;
        case STG_E_OLDDLL                       : szErr = "STG_E_OLDDLL";                   break;
        case STG_E_SHAREREQUIRED                : szErr = "STG_E_SHAREREQUIRED";            break;
        case STG_E_NOTFILEBASEDSTORAGE          : szErr = "STG_E_NOTFILEBASEDSTORAGE";      break;
        case STG_S_CONVERTED                    : szErr = "STG_S_CONVERTED";                break;


        case HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) :         szErr = "WIN32 - ERROR_FILE_NOT_FOUND";         break;
        case HRESULT_FROM_WIN32(ERROR_INVALID_DRIVE) :          szErr = "WIN32 - ERROR_INVALID_DRIVE";          break;
        case HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE) :         szErr = "WIN32 - ERROR_INVALID_HANDLE";         break;
        case HRESULT_FROM_WIN32(ERROR_SEEK) :                   szErr = "WIN32 - ERROR_SEEK";                   break;
        case HRESULT_FROM_WIN32(ERROR_SECTOR_NOT_FOUND) :       szErr = "WIN32 - ERROR_SECTOR_NOT_FOUND";       break;
        case HRESULT_FROM_WIN32(ERROR_WRITE_FAULT) :            szErr = "WIN32 - ERROR_WRITE_FAULT";            break;
        case HRESULT_FROM_WIN32(ERROR_READ_FAULT) :             szErr = "WIN32 - ERROR_READ_FAULT";             break;
        case HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION) :      szErr = "WIN32 - ERROR_SHARING_VIOLATION";      break;
        case HRESULT_FROM_WIN32(ERROR_LOCK_VIOLATION) :         szErr = "WIN32 - ERROR_LOCK_VIOLATION";         break;
        case HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED) :          szErr = "WIN32 - ERROR_NOT_SUPPORTED";          break;
        case HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER) :           szErr = "WIN32 - ERROR_NO_SUCH_USER";           break;
        case HRESULT_FROM_WIN32(ERROR_NO_SUCH_GROUP) :          szErr = "WIN32 - ERROR_NO_SUCH_GROUP";          break;
        case HRESULT_FROM_WIN32(ERROR_WRONG_PASSWORD) :         szErr = "WIN32 - ERROR_WRONG_PASSWORD";         break;
        case HRESULT_FROM_WIN32(ERROR_INVALID_PASSWORD) :       szErr = "WIN32 - ERROR_INVALID_PASSWORD";       break;
        case HRESULT_FROM_WIN32(ERROR_INVALID_FLAGS) :          szErr = "WIN32 - ERROR_INVALID_FLAGS";          break;
        case HRESULT_FROM_WIN32(ERROR_BAD_USERNAME) :           szErr = "WIN32 - ERROR_BAD_USERNAME";           break;
        case HRESULT_FROM_WIN32(ERROR_BROKEN_PIPE) :            szErr = "WIN32 - ERROR_BROKEN_PIPE";            break;
        case HRESULT_FROM_WIN32(ERROR_PIPE_BUSY) :              szErr = "WIN32 - ERROR_PIPE_BUSY";              break;
        case HRESULT_FROM_WIN32(ERROR_PIPE_NOT_CONNECTED):      szErr = "WIN32 - ERROR_PIPE_NOT_CONNECTED";     break;
        case HRESULT_FROM_WIN32(ERROR_PIPE_CONNECTED):          szErr = "WIN32 - ERROR_PIPE_CONNECTED";         break;
        case HRESULT_FROM_WIN32(ERROR_STATIC_INIT) :            szErr = "WIN32 - ERROR_STATIC_INIT";            break;
        case HRESULT_FROM_WIN32(ERROR_INVALID_FUNCTION) :       szErr = "WIN32 - ERROR_INVALID_FUNCTION";       break;
        case HRESULT_FROM_WIN32(ERROR_EXCEPTION_IN_SERVICE):    szErr = "WIN32 - ERROR_EXCEPTION_IN_SERVICE";   break;
        case HRESULT_FROM_WIN32(ERROR_CANCELLED):               szErr = "WIN32 - ERROR_CANCELLED";              break;
        case HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY):            szErr = "WIN32 - ERROR_PARTIAL_COPY";           break;
        case HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER):     szErr = "WIN32 - ERROR_INSUFFICIENT_BUFFER";    break;
        case HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION):  szErr = "WIN32 - ERROR_NO_UNICODE_TRANSLATION"; break;
        case HRESULT_FROM_WIN32(ERROR_INVALID_ACCOUNT_NAME):    szErr = "WIN32 - ERROR_INVALID_ACCOUNT_NAME";   break;
        case HRESULT_FROM_WIN32(ERROR_PRIVILEGE_NOT_HELD):      szErr = "WIN32 - ERROR_INVALID_ACCOUNT_NAME";   break;
        case HRESULT_FROM_WIN32(ERROR_BAD_NET_NAME):            szErr = "WIN32 - ERROR_BAD_NET_NAME";           break;
        case HRESULT_FROM_WIN32(ERROR_SERVICE_NO_THREAD):       szErr = "WIN32 - ERROR_SERVICE_NO_THREAD";      break;
        case HRESULT_FROM_WIN32(ERROR_SHUTDOWN_IN_PROGRESS):    szErr = "WIN32 - ERROR_SHUTDOWN_IN_PROGRESS";   break;
        case HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID):      szErr = "WIN32 - ERROR_CONNECTION_INVALID";     break;
        case HRESULT_FROM_WIN32(ERROR_HANDLE_EOF):              szErr = "WIN32 - ERROR_HANDLE_EOF";             break;
        case HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS):          szErr = "WIN32 - ERROR_ALREADY_EXISTS";         break;
        case HRESULT_FROM_WIN32(ERROR_HOST_UNREACHABLE):        szErr = "WIN32 - ERROR_HOST_UNREACHABLE";       break;
        case HRESULT_FROM_WIN32(ERROR_FILE_EXISTS):             szErr = "WIN32 - ERROR_FILE_EXISTS";            break;
        case HRESULT_FROM_WIN32(ERROR_IO_PENDING):              szErr = "WIN32 - ERROR_IO_PENDING";             break;
        case HRESULT_FROM_WIN32(ERROR_ILL_FORMED_PASSWORD):     szErr = "WIN32 - ERROR_ILL_FORMED_PASSWORD";    break;
        case HRESULT_FROM_WIN32(ERROR_PASSWORD_RESTRICTION):    szErr = "WIN32 - ERROR_PASSWORD_RESTRICTION";   break;
        case HRESULT_FROM_WIN32(ERROR_LOGON_FAILURE):           szErr = "WIN32 - ERROR_LOGON_FAILURE";          break;
        case HRESULT_FROM_WIN32(ERROR_ACCOUNT_RESTRICTION):     szErr = "WIN32 - ERROR_ACCOUNT_RESTRICTION";    break;
        case HRESULT_FROM_WIN32(ERROR_INVALID_LOGON_HOURS):     szErr = "WIN32 - ERROR_INVALID_LOGON_HOURS";    break;
        case HRESULT_FROM_WIN32(ERROR_INVALID_WORKSTATION):     szErr = "WIN32 - ERROR_INVALID_WORKSTATION";    break;
        case HRESULT_FROM_WIN32(ERROR_PASSWORD_EXPIRED):        szErr = "WIN32 - ERROR_PASSWORD_EXPIRED";       break;
        case HRESULT_FROM_WIN32(ERROR_ACCOUNT_DISABLED):        szErr = "WIN32 - ERROR_ACCOUNT_DISABLED";       break;
        case HRESULT_FROM_WIN32(ERROR_NONE_MAPPED):             szErr = "WIN32 - ERROR_NONE_MAPPED";            break;
        case HRESULT_FROM_WIN32(ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) : szErr = "WIN32 - ERROR_FAILED_SERVICE_CONTROLLER_CONNECT"; break;

        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_CALL_FAILED):              szErr = "RPC Error - RPC_S_CALL_FAILED";            break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_CALL_FAILED_DNE):          szErr = "RPC Error - RPC_S_CALL_FAILED_DNE";        break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_PROTOCOL_ERROR):           szErr = "RPC Error - RPC_S_PROTOCOL_ERROR";         break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_UNSUPPORTED_TRANS_SYN):    szErr = "RPC Error - RPC_S_UNSUPPORTED_TRANS_SYN";  break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_UNSUPPORTED_TYPE):         szErr = "RPC Error - RPC_S_UNSUPPORTED_TYPE";       break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_INVALID_TAG):              szErr = "RPC Error - RPC_S_INVALID_TAG";            break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_INVALID_BOUND):            szErr = "RPC Error - RPC_S_INVALID_BOUND";          break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_NO_ENTRY_NAME):            szErr = "RPC Error - RPC_S_NO_ENTRY_NAME";          break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_INVALID_NAME_SYNTAX):      szErr = "RPC Error - RPC_S_INVALID_NAME_SYNTAX";    break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_UNSUPPORTED_NAME_SYNTAX):  szErr = "RPC Error - RPC_S_UNSUPPORTED_NAME_SYNTAX";break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_DUPLICATE_ENDPOINT):       szErr = "RPC Error - RPC_S_DUPLICATE_ENDPOINT";     break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_INVALID_STRING_BINDING):   szErr = "RPC Error - RPC_S_INVALID_STRING_BINDING"; break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_WRONG_KIND_OF_BINDING):    szErr = "RPC Error - RPC_S_WRONG_KIND_OF_BINDING";  break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_INVALID_BINDING):          szErr = "RPC Error - RPC_S_INVALID_BINDING";        break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_PROTSEQ_NOT_SUPPORTED):    szErr = "RPC Error - RPC_S_PROTSEQ_NOT_SUPPORTED";  break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_INVALID_RPC_PROTSEQ):      szErr = "RPC Error - RPC_S_INVALID_RPC_PROTSEQ";    break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_INVALID_STRING_UUID):      szErr = "RPC Error - RPC_S_INVALID_STRING_UUID";    break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_INVALID_ENDPOINT_FORMAT):  szErr = "RPC Error - RPC_S_INVALID_ENDPOINT_FORMAT";break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_INVALID_NET_ADDR):         szErr = "RPC Error - RPC_S_NO_ENDPOINT_FOUND";      break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_NO_ENDPOINT_FOUND):        szErr = "RPC Error - RPC_S_DUPLICATE_ENDPOINT";     break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_INVALID_TIMEOUT):          szErr = "RPC Error - RPC_S_INVALID_TIMEOUT";        break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_OBJECT_NOT_FOUND):         szErr = "RPC Error - RPC_S_OBJECT_NOT_FOUND";       break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_ALREADY_REGISTERED):       szErr = "RPC Error - RPC_S_ALREADY_REGISTERED";     break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_TYPE_ALREADY_REGISTERED):  szErr = "RPC Error - RPC_S_TYPE_ALREADY_REGISTERED";break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_ALREADY_LISTENING):        szErr = "RPC Error - RPC_S_ALREADY_LISTENING";      break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_NO_PROTSEQS_REGISTERED):   szErr = "RPC Error - RPC_S_NO_PROTSEQS_REGISTERED"; break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_NOT_LISTENING):            szErr = "RPC Error - RPC_S_NOT_LISTENING";          break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_UNKNOWN_MGR_TYPE):         szErr = "RPC Error - RPC_S_UNKNOWN_MGR_TYPE";       break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_UNKNOWN_IF):               szErr = "RPC Error - RPC_S_UNKNOWN_IF";             break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_NO_BINDINGS):              szErr = "RPC Error - RPC_S_NO_BINDINGS";            break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_NO_PROTSEQS):              szErr = "RPC Error - RPC_S_NO_PROTSEQS";            break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_CANT_CREATE_ENDPOINT):     szErr = "RPC Error - RPC_S_CANT_CREATE_ENDPOINT";   break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_OUT_OF_RESOURCES):         szErr = "RPC Error - RPC_S_OUT_OF_RESOURCES";       break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_SERVER_UNAVAILABLE):       szErr = "RPC Error - RPC_S_SERVER_UNAVAILABLE";     break;
        case MAKE_HRESULT(1, FACILITY_RPC, RPC_S_SERVER_TOO_BUSY):          szErr = "RPC Error - RPC_S_SERVER_TOO_BUSY";        break;

        default : szErr = "UNKNOWN"; break;
    }   

    char strMsg[MAX_DEBUG_MSG * 2];
    wsprintf( strMsg, "%s (0x%x): %s", strContext, hr, SAFE_STRING(szErr) );
    fnDisplayDebugMessageA( strMsg, strFile, iLine );
    
    SetLastError(dwLastError);
}

#define DEFAULT_ERROR   ERROR_INVALID_PARAMETER;


DWORD CopyA2W( LPWSTR wcsWide, DWORD dwBufferLength, LPCSTR csAnsi )
{
    int iAnsiLength = ( strlen(csAnsi) + 1);

	int iNewLength = MultiByteToWideChar(
		  CP_ACP,           // code page
		  0,                // character-type options
		  csAnsi,           // address of string to map
		  iAnsiLength,      // number of bytes in string
		  wcsWide,          // address of wide-character buffer
		  dwBufferLength ); // size of buffer 

    if (iNewLength == 0)
    {
        DWORD dwResult = GetLastError();
        DEBUG_MSG("CopyA2W failed.");
        return dwResult;
    }

    return ERROR_SUCCESS;
}

DWORD CopyW2W( LPWSTR wcsDest, DWORD dwDestSize, LPCWSTR wcsSrc )
{
    DWORD dwResult = DEFAULT_ERROR;

    if (dwDestSize > wcslen(wcsSrc))
    {
        wcscpy( wcsDest, wcsSrc );

        dwResult = ERROR_SUCCESS;
    }
    else
    {
        DEBUG_MSG("buffer too small!");
        dwResult = ERROR_INSUFFICIENT_BUFFER;
    }

    return dwResult;
}

DWORD CatW2W( LPWSTR wcsDest, DWORD dwDestSize, LPCWSTR wcsSrc )
{
    DWORD dwResult = DEFAULT_ERROR;

    if (dwDestSize > ( wcslen(wcsSrc) + wcslen(wcsDest) ) )
    {
        wcscat( wcsDest, wcsSrc );

        dwResult = ERROR_SUCCESS;
    }
    else
    {
        DEBUG_MSG("buffer too small!");
        dwResult = ERROR_INSUFFICIENT_BUFFER;
    }

    return dwResult;
}

DWORD CopyW2A( LPSTR csAnsi, DWORD dwBufferLength, LPCWSTR wcsWide )
{
    static const LPCSTR csDefault = "?";
    BOOL bUnmappableChars = FALSE;

    int iWideLength = ( wcslen(wcsWide) + 1);

    int iNewLength = WideCharToMultiByte(
        CP_ACP,
        0,
        wcsWide,
        iWideLength,
        csAnsi,
        dwBufferLength,
        csDefault,
        &bUnmappableChars );
    if (iNewLength == 0)
    {
        DWORD dwResult = GetLastError();
        DEBUG_MSG("CopyW2A failed.");
        return dwResult;
    }

    if (bUnmappableChars)
    {
        DEBUG_MSG("CopyW2A had unmappable characters");
    }

    return ERROR_SUCCESS;
}