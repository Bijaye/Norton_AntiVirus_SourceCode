////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//*************************************************************************
// defwatch.h - created 11/6/98 1:57:15 PM
//
// $Header$
//
// Description:
//
// Contains:
//
//*************************************************************************
// $Log$
//*************************************************************************

#ifndef _QSCOMM_H_
#define _QSCOMM_H_

//*************************************************************************

#ifdef WIN32

#ifdef BUILDING_QSCOMM
#define DECLSPEC_QSCOMM __declspec( dllexport )
#else
#define DECLSPEC_QSCOMM __declspec( dllimport )
#endif

#else

#define DECLSPEC_QSCOMM

#endif


#if defined(UNIX)
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef int BOOL;
#endif

//
// Callback function typedef.  The callback routine should return TRUE to continue
// processing, or FALSE to abort processing.
//
typedef BOOL (*LPFNXFERCALLBACK)( int iMessage, DWORD dwParam1, DWORD dwParam2 );

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
// Argument      : int iPort - port to use.
// Argument      : LPFNXFERCALLBACK pCallback - User supplied callback function.  
//                 Can be NULL.
//
///////////////////////////////////////////////////////////////////////////////
// 12/22/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus      
    extern "C"   
#endif
int DECLSPEC_QSCOMM SendFileToQServer(  const char* pszSourceFile,
                        const char* pszServerAddress,
                        int iProtocol,
                        int iPort,
                        LPFNXFERCALLBACK pCallback );

// 
// Values for iProtocol parameter of SendFileToQServer.
// 
#define QSCOMM_PROTOCOL_IP                  0
#define QSCOMM_PROTOCOL_SPX                 1

// 
// Error codes returned by SendFileToQServer.
// 
#define QSCOMM_SUCCESS                      0
#define QSCOMM_USER_ABORT                   1
#define QSCOMM_ERROR_SERVER_BUSY            2
#define QSCOMM_ERROR_SERVER_FULL            3
#define QSCOMM_ERROR_SERVER_NOT_FOUND       4 
#define QSCOMM_ERROR_SERVER_SOCKET_ERROR    5
#define QSCOMM_ERROR_SERVER_UNKNOWN_ERROR   6
#define QSCOMM_ERROR_FILE_NOT_FOUND         7
#define QSCOMM_ERROR_UNKNOWN_PROTOCOL       8
#define QSCOMM_ERROR_OUT_OF_MEMORY          9
#define QSCOMM_ERROR_BOGUS_FILE             10

// 
// Callback messages.  

//  
// 
// CALLBACK_BEGIN - dwParam1 not used
//                - dwParam2 not used
//
#define CALLBACK_BEGIN                      0
//  
// 
// CALLBACK_END   - dwParam1 not used
//                - dwParam2 not used
//
#define CALLBACK_END                        1

//  
// 
// CALLBACK_STATUS - dwParam1 = bytes sent so far.
//                 - dwParam2 = total bytes remaining.
//
#define CALLBACK_STATUS                     2

//*************************************************************************

#endif





