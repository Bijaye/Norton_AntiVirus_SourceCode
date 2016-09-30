// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef __STORAGE_H__
#define __STORAGE_H__

#include "comm.h"

extern STORAGEINFO g_storageInfo;
extern PSSFUNCTIONS g_pSSFunctions;

extern "C" DllExport DWORD MEC_StorageInit(DWORD flags, PSTORAGEINFO *info, HANDLE *handle, PSSFUNCTIONS functions);
DWORD FormatEmailMessage (char * pMsgBuf, size_t nNumMsgBufBytes, const char * pMsgFmt, PPROCESSBLOCK pBlock);

#endif //__STORAGE_H__