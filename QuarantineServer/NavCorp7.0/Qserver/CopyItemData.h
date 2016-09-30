// CopyItemData.h : Declaration of the CCopyItemData

#ifndef __COPYITEMDATA_H_
#define __COPYITEMDATA_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCopyItemData
class ATL_NO_VTABLE CCopyItemData : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCopyItemData, &CLSID_CopyItemData>,
	public ICopyItemData
{
public:
    CCopyItemData() :
      m_pFileData( NULL ),
      m_hMap( INVALID_HANDLE_VALUE ),
      m_dwSize(0),
      m_dwCurrentOffset(0)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_COPYITEMDATA)
DECLARE_NOT_AGGREGATABLE(CCopyItemData)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCopyItemData)
	COM_INTERFACE_ENTRY(ICopyItemData)
END_COM_MAP()

// ICopyItemData
public:
    STDMETHOD( Open ) ( /*[ in ]*/ ULONG ulFileID );
    STDMETHOD( GetSize ) ( /*[ out ]*/ ULONG* pulFileSize );
    STDMETHOD( Read) (  /*[ in ]*/ ULONG ulBufferSize,                       
               	        /*[ out ]*/ ULONG* pulBytesRead,                     
               	        /*[ out, size_is( ulBufferSize ) ]*/ BYTE* lpBuffer);
    STDMETHOD( Close ) ();


public:
    // 
    // Overrides
    // 
    void FinalRelease();


private:
    LPBYTE      m_pFileData;
    HANDLE      m_hMap;
    DWORD       m_dwSize;
    DWORD       m_dwCurrentOffset;
    

};

#endif //__COPYITEMDATA_H_
