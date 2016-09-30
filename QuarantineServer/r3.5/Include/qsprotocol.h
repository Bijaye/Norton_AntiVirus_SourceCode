// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
//
// This header file contains structures and constants relating to the 
// sockets based communications protocol used by the Quarantine Server and
// client components.
//
///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _QSPROTOCOL_H_
#define _QSPROTOCOL_H_

#if defined(UNIX)
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#endif

#if defined(__GNUC__) 
#define PACKED  __attribute__ ((packed))
#else
#define PACKED
#endif


///////////////////////////////////////////////////////////////////////////////
// Protocol specific structures.  Should be packed on 1 byte boundaries.
#if !defined(__GNUC__)
#pragma pack( push, 1)
#endif

// 
// This structure is sent as a single packet from clients to 
// the Quarantine server.
// 
typedef struct FILESIZESTRUCT
{
    DWORD dwSignature PACKED;          // 0xDEADBEEF
    DWORD dwFileSize PACKED;
}FILESIZESTRUCT;
#define FILE_SIZE_SIGNATURE     0xDEADBEEF


///////////////////////////////////////////////////////////////////////////////
// End Protocol specific structures.  
#if !defined(__GNUC__)
#pragma pack( pop )
#endif


///////////////////////////////////////////////////////////////////////////////
// Messages sent between clients and server.

#define BEGIN_XFER                  (DWORD) 0x00000001
#define DONE_XFER                   (DWORD) 0x00000002

#define SERVER_ERROR                (DWORD) 0xF0000000
#define SERVER_ERROR_BUSY           (DWORD) 0xF0000001
#define SERVER_ERROR_DISK_SPACE     (DWORD) 0xF0000002
#define SERVER_ERROR_SOCKET         (DWORD) 0xF0000003
#define SERVER_ERROR_BOGUS_FILE     (DWORD) 0xF0000004


///////////////////////////////////////////////////////////////////////////////
#endif
