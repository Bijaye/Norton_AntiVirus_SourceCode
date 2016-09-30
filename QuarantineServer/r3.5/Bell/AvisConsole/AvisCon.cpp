/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


#include "stdafx.h"
#include "AvisConsole.h"
#include "AvisCon.h"
//#include "ScopeExtData.h"



/////////////////////////////////////////////////////////////////////////////
//// CAvisConComponentData
//static const GUID CAvisConExtGUID_NODETYPE = 
//{ 0x1dae69de, 0xb621, 0x11d2, { 0x8f, 0x45, 0x30, 0x78, 0x30, 0x2c, 0x20, 0x30 } };
//const GUID*  CAvisConExtData::m_NODETYPE = &CAvisConExtGUID_NODETYPE;
//const OLECHAR* CAvisConExtData::m_SZNODETYPE = OLESTR("1DAE69DE-B621-11d2-8F45-3078302C2030");
//const OLECHAR* CAvisConExtData::m_SZDISPLAY_NAME = OLESTR("AvisCon");
//const CLSID* CAvisConExtData::m_SNAPIN_CLASSID = &CLSID_AvisCon;


STDMETHODIMP CAvisCon::CreatePropertyPages(LPPROPERTYSHEETCALLBACK lpProvider,
        long handle,
        LPDATAOBJECT pDataObject)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	
    TCHAR *pszText    = _T("");

    //_ASSERTE(0 == "CAvisCon::CreatePropertyPages");

    {   // TEST
        CSnapInItem* pItem = NULL;;
        DATA_OBJECT_TYPES   type;
		HRESULT hr = GetDataClass( pDataObject, &pItem, &type);  
        if( type == CCT_SCOPE )
            pszText = _T("type= CCT_SCOPE");
        else if( type == CCT_RESULT )
            pszText = _T("type= CCT_RESULT");
        else
            pszText = _T("type= CCT_UNINITIALIZED");
    }

   	fWidePrintString("CAvisCon::CreatePropertyPages ObjectID= %d  %s", m_iExtObjectID, pszText);


    // THIS CALL MAKES MMC CALL CAvisConExtData::CreatePropertyPages or CScopeExtData::CreatePropertyPages
    // Return base class implementation.
    return IExtendPropertySheetImpl<CAvisCon>::CreatePropertyPages( lpProvider, handle, pDataObject );
}


 
/*--------------------------------------------------------------------
   GetHelpTopic()
   Pointer to path of compiled HTMLHelp file
   Written by: Jim Hill                 
--------------------------------------------------------------------*/
STDMETHODIMP CAvisCon::GetHelpTopic( LPOLESTR* lpCompiledHelpFile )   
{
USES_CONVERSION;
return E_NOTIMPL;  // with the merged helpfiles this causes duplicat entries.
    TCHAR *lpszFullPathName = GetHelpfileName();

    // ALLOCATE BUFFER FOR MMC
    *lpCompiledHelpFile = reinterpret_cast<LPOLESTR>
            (CoTaskMemAlloc((lstrlen( lpszFullPathName ) + 1)* sizeof(wchar_t)) );

    if( *lpCompiledHelpFile == NULL )
        return E_OUTOFMEMORY;

    // COPY DATA
    wcscpy( *lpCompiledHelpFile, T2OLE( lpszFullPathName ) );


    //fPrintString("CAvisCon::GetHelpTopic ------------------");
	return S_OK;  //   // E_NOTIMPL
}








