// NavAPICallbacks.h: interface for the CNavAPICallbacks class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVAPICALLBACKS_H__64182084_D585_11D2_8F47_3078302C2030__INCLUDED_)
#define AFX_NAVAPICALLBACKS_H__64182084_D585_11D2_8F47_3078302C2030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "navapi.h"

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



class CNavAPIMemCallbacks : public NAVFILEIO
{
public:
	CNavAPIMemCallbacks( );
	virtual ~CNavAPIMemCallbacks();

    operator LPNAVFILEIO() { return this; }

private:
    // 
    // Memory based callbacks
    // 
    static BOOL MemoryOpen( LPVOID lpvFileInfo, DWORD dwOpenMode, LPVOID FAR *lplpvHandle );
    static BOOL    MemoryClose( LPVOID lpvHandle );
    static DWORD   MemorySeek( LPVOID lpvHandle, LONG lOffset, int nFrom );
    static UINT    MemoryRead( LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToRead );
    static UINT    MemoryWrite( LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToWrite );
    static BOOL    MemoryDelete( LPVOID lpvFileInfo );
    static BOOL    MemoryGetDateTime( LPVOID lpvHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime );
    static BOOL    MemorySetDateTime( LPVOID lpvHandle, UINT uType, WORD wDate, WORD wTime );
};




#endif // !defined(AFX_NAVAPICALLBACKS_H__64182084_D585_11D2_8F47_3078302C2030__INCLUDED_)
