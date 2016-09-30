#pragma once

#include "ccLogViewerPluginId.h"
#include "ccLogViewerInterface.h"

class CHPPLVPlugin  :
    public cc::ILogViewerPlugin2,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    CHPPLVPlugin(void);
    virtual ~CHPPLVPlugin(void);

public:
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(cc::IID_LogViewerPlugin, cc::ILogViewerPlugin)
        SYM_INTERFACE_ENTRY(cc::IID_LogViewerPlugin2, cc::ILogViewerPlugin2)
    SYM_INTERFACE_MAP_END() 

    ////////////////////////////////////////////////////
    // ILogViewerPlugin methods
    //
    virtual HRESULT GetPluginID(int& iPluginID);
    virtual HRESULT GetProductName(LPTSTR szProductName, DWORD& dwSize);
    virtual HRESULT GetStyleSheet(UINT& uID);

    HRESULT GetCategoryCount(int& iCategoryCount);
    HRESULT GetCategory(int iIndex, cc::ILogViewerCategory*& pCategory);

    ////////////////////////////////////////////////////
    // ILogViewerPlugin2 methods
    //
    virtual HRESULT LaunchHelp();

private:
    CHPPLVPlugin(const CHPPLVPlugin&);
    CHPPLVPlugin& operator =(const CHPPLVPlugin&);

protected:
    enum { PluginId = CC_HPPLV_PLUGIN_ID };
};

