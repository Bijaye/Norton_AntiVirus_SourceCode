// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// VpshellEx.h : Declaration of the CVpshellEx

#ifndef __VPSHELLEX_H_
#define __VPSHELLEX_H_

#include "resource.h"       // main symbols
#include "scaninf.h"
#include <shlobj.h>
#include <comdef.h>

/////////////////////////////////////////////////////////////////////////////
// CVpshellEx
class ATL_NO_VTABLE CVpshellEx : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CVpshellEx, &CLSID_VpshellEx>,
    public IShellExtInit,
    public IContextMenu,
    public IShellPropSheetExt
{
public:
    CVpshellEx() : m_hDrop(NULL), m_bNT(FALSE)
    {
    }
    ~CVpshellEx()
    {
        if (m_hDrop)
            CoUninitialize();
    }

DECLARE_REGISTRY_RESOURCEID(IDR_VPSHELLEX)

BEGIN_COM_MAP(CVpshellEx)
    COM_INTERFACE_ENTRY(IShellExtInit)
    COM_INTERFACE_ENTRY(IContextMenu)
    COM_INTERFACE_ENTRY(IShellPropSheetExt)
END_COM_MAP()


public:
     STDMETHOD(Initialize)(LPCITEMIDLIST pidlFolder,
                          LPDATAOBJECT lpdobj, HKEY hkeyProgID);
     STDMETHOD(QueryContextMenu)(HMENU hmenu,
                                UINT indexMenu,
                                UINT idCmdFirst,
                                UINT idCmdLast,
                                UINT uFlags);

    STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO lpici);

    STDMETHOD(GetCommandString)(UINT        idCmd,
                                UINT        uType,
                                UINT      * pwReserved,
                                LPSTR       pszName,
                                UINT        cchMax);
    STDMETHOD(AddPages)(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam);
    STDMETHOD(ReplacePage)(UINT uPageID, LPFNADDPROPSHEETPAGE lpfnReplaceWith, LPARAM lParam);
private:
    bool IsRtvScanRunning () const;
    STDMETHOD(ActuallyScan) (LPCMINVOKECOMMANDINFO lpici);

private:
    HDROP m_hDrop;
    BOOL m_bNT;

};

#endif //__VPSHELLEX_H_
