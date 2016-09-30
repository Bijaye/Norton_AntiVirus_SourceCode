/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// NavAPICallbacks.h: interface for the CNavAPICallbacks class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVAPICALLBACKS_H__64182084_D585_11D2_8F47_3078302C2030__INCLUDED_)
#define AFX_NAVAPICALLBACKS_H__64182084_D585_11D2_8F47_3078302C2030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "navapi.h"
#include <ecomtype.h>
#include <eicore.h>
#include <eilegtyp.h>
#include <eitclist.h>
#include <eilegacy.h>
#include <ecomlodr.h>
#include <thretcat.h>

class CNavAPIMemCallbacks;

class CMemoryScanData
{
friend class CNavAPIMemCallbacks;

public:
    CMemoryScanData( DWORD dwSize, BYTE* pData )
        { 
        m_dwDataSize = dwSize;
        m_pData = NULL;
        m_dwCurrentPosition = 0;
        }

public: 
    DWORD   m_dwDataSize;    
    BYTE*   m_pData;

private:
    DWORD   m_dwCurrentPosition;

};



class CNavAPIMemCallbacks : public LEGFILEIO
{
public:
	CNavAPIMemCallbacks( );
	virtual ~CNavAPIMemCallbacks();

    operator LPLEGFILEIO() { return this; }

private:
    // 
    // Memory based callbacks
    // 
    static BOOL	   ECOM_STDCALL MemoryOpen( LPVOID lpvFileInfo, DWORD dwOpenMode, LPVOID FAR *lplpvHandle );
    static BOOL    ECOM_STDCALL MemoryClose( LPVOID lpvHandle );
    static DWORD   ECOM_STDCALL MemorySeek( LPVOID lpvHandle, LONG lOffset, int nFrom );
    static UINT    ECOM_STDCALL MemoryRead( LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToRead );
    static UINT    ECOM_STDCALL MemoryWrite( LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToWrite );
    static BOOL    ECOM_STDCALL MemoryDelete( LPVOID lpvFileInfo );
    static BOOL    ECOM_STDCALL MemoryGetDateTime( LPVOID lpvHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime );
    static BOOL    ECOM_STDCALL MemorySetDateTime( LPVOID lpvHandle, UINT uType, WORD wDate, WORD wTime );
};




#endif // !defined(AFX_NAVAPICALLBACKS_H__64182084_D585_11D2_8F47_3078302C2030__INCLUDED_)
