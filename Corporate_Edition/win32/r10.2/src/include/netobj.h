// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef  _NETOBJ_H
#define  _NETOBJ_H

//#if defined(SYM_WIN) || defined(SYM_VXD)
    #include "symnet.h"
//#endif

#define NETOBJECT_NEXTRECORD    0xFFFF
#define MAX_NET_OBJECTS         10
#define SIZE_NET_USER_LIST      250

                                        // Size of 2 * OBJECT_NAME_LEN + "[/] "
#define NAVD_SERVER_USER_LENGTH (2 * 47 + 4)

#define NETUSER_FLAG_ISUSER     0x0001
#define NETUSER_FLAG_SHOWSERVER 0x0002

typedef struct tagNETUSERITEM
    {
    char    szServer[NETMAXSERVERNAME + 1];
    char    szObject[NETMAXUSERNAME   + 1];
    WORD    wFlags;
    } NETUSERITEM, FAR *LPNETUSERITEM;


typedef struct tagNETUSER
    {
    UINT                uEntries;       // Number of NETUSERITEM entries
    HGLOBAL             hNetUserItem;   // Handle to NETUSERITEM entries
    LPNETUSERITEM       lpNetUserItem;  // Ptr to NETUSERITEM entries
    } NETUSER, FAR *LPNETUSER;


#ifdef __cplusplus
extern "C" {
#endif

STATUS SYM_EXPORT WINAPI LoadNetUserList (LPNETUSER lpNetUser);
STATUS SYM_EXPORT WINAPI LoadNetUserListSpecifyDir ( LPNETUSER lpNetUser,
						LPSTR lpszDir );
STATUS SYM_EXPORT WINAPI UpdateNetUserList (LPNETUSER lpNetUser, BOOL bFreeMemory);
STATUS SYM_EXPORT WINAPI UpdateNetUserListSpecifyDir (LPNETUSER lpNetUser,
                                                LPSTR lpszDir,
                                                BOOL bFreeMemory);

#if 0
LPSTR SYM_EXPORT WINAPI GetNetObjects (BOOL bMax);    // Loads net objects

LPNETOBJ SYM_EXPORT WINAPI OpenNetObjectFile (VOID);

LPNETOBJ SYM_EXPORT WINAPI CreateNetObjectFile (VOID);

BOOL SYM_EXPORT WINAPI CloseNetObjectFile (
                     LPNETOBJ lpNet);           // Close Net Obj File

BOOL SYM_EXPORT WINAPI ReadNetObjectFile (
                     LPNETOBJ lpNet,            // Net Obj token
                     UINT uRecNo,               // Rec.No to read
                     LPNETSTRUCT lpNetStruct);  // Buffer where to read

BOOL SYM_EXPORT WINAPI WriteNetObjectFile (
                     LPNETOBJ lpNet,            // Net Obj token
                     UINT uRecNo,               // Rec.No to write
                     LPNETSTRUCT lpNetStruct);  // Buffer to write

BOOL SYM_EXPORT WINAPI TruncateNetObjectFile (
                     LPNETOBJ lpNet,            // Net Obj token
                     UINT uEntries);            // # of entries to leave

UINT SYM_EXPORT WINAPI  NumNetObjectEntries (
                     LPNETOBJ lpNet);           // Number of NETSTRUCTs
#endif

#ifdef __cplusplus
}
#endif

#endif
