// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif


#ifndef NSF_OBJECT_DEFS
#define NSF_OBJECT_DEFS

#ifndef NSF_DEFS
#include "nsfdata.h"
#endif


/* Note Storage File Object Package Definitions */

/*	Available across the network */

STATUS LNPUBLIC NSFDbAllocObject (DBHANDLE hDB, DWORD dwSize,
								WORD Class,
								WORD Privileges,
								DWORD far *retObjectID);

STATUS LNPUBLIC NSFDbReallocObject (DBHANDLE hDB, DWORD ObjectID, DWORD NewSize);

STATUS LNPUBLIC NSFDbFreeObject (DBHANDLE hDB, DWORD ObjectID);

STATUS LNPUBLIC NSFDbGetObjectSize (DBHANDLE hDB, DWORD ObjectID, WORD ObjectType,
								DWORD far *retSize,
								WORD far *retClass,
								WORD far *retPrivileges);

STATUS LNPUBLIC NSFDbReadObject (DBHANDLE hDB, DWORD ObjectID,
								DWORD Offset, DWORD Length,
								HANDLE far *rethBuffer);

STATUS LNPUBLIC NSFDbWriteObject (DBHANDLE hDB, DWORD ObjectID,
								HANDLE hBuffer,
								DWORD Offset, DWORD Length);

/* End of Note Storage File Object Package */

#endif


#ifdef __cplusplus
}
#endif

