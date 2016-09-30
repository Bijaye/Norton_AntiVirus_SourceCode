// VirusListPlugin.h: interface for the CVirusListPlugin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIRUSLISTPLUGIN_H__D66D91C1_5CD7_434B_A3DB_7B181F3A988C__INCLUDED_)
#define AFX_VIRUSLISTPLUGIN_H__D66D91C1_5CD7_434B_A3DB_7B181F3A988C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ccLogViewerInterface.h"
#include "ccLogViewerPluginId.h"

class CNAVLogViewerPlugin :
    public cc::ILogViewerPlugin2,
	public ISymBaseImpl<CSymThreadSafeRefCount>  
{
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(cc::IID_LogViewerPlugin, cc::ILogViewerPlugin)
        SYM_INTERFACE_ENTRY(cc::IID_LogViewerPlugin2, cc::ILogViewerPlugin)
    SYM_INTERFACE_MAP_END() 

public:
	CNAVLogViewerPlugin();
	virtual ~CNAVLogViewerPlugin();
    
    HRESULT GetProductName(LPTSTR szProductName, 
	 					   /* [in/out] */ DWORD& dwSize);
	

    HRESULT GetPluginID(int& iPluginID);
	HRESULT GetCategoryCount(int& iCategoryCount);
    HRESULT GetCategory(int iIndex, cc::ILogViewerCategory*& pCategory);
    HRESULT GetStyleSheet(UINT& uID);
    HRESULT LaunchHelp();
};

#endif // !defined(AFX_VIRUSLISTPLUGIN_H__D66D91C1_5CD7_434B_A3DB_7B181F3A988C__INCLUDED_)
