/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// ReadSample.h: interface for the CSampleRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SAMPLEREADTHREAD_H)
#define SAMPLEREADTHREAD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Afxtempl.h"
#include "mmc.h"

class CSampleRead
{
public:
	CSampleRead();
	virtual ~CSampleRead();


    static unsigned int __stdcall ThreadFunc( void* pData );
	HRESULT ReadAllSamples(CSnapInItem* pRootNode);
    
    // 
    // Pointer identity.  Need this for call to CoSetProxyBlanket();
    // 
    SEC_WINNT_AUTH_IDENTITY *m_pIdentity;

	CMapPtrToPtr *m_pDataList;

    void Abort();

    // 
    // Handle to thread
    // 
    DWORD           m_dwThread;

private:


    // 
    // Stream for interface marshalling to the QServer.
    // 
    LPSTREAM        m_pQSStream;

    // 
    // User data.
    // 
    DWORD           m_dwUserData;

	CQSConsoleData  *m_pRootNode;


    // 
    // Handle to abort event.
    // 
    HANDLE          m_hAbort;
};



#endif 
