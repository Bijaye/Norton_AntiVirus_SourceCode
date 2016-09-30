// Copyright 1992 - 1994 Symantec, Peter Norton Group
// ************************************************************************
//
// $Header:   S:/NAVWINX5/VCS/NAVNODE.H_v   1.0   01 Jun 1998 16:08:38   DBuches  $
//
// Description:
//
// See Also:
//
// ************************************************************************
// $Log:   S:/NAVWINX5/VCS/NAVNODE.H_v  $
// 
//    Rev 1.0   01 Jun 1998 16:08:38   DBuches
// Initial revision.
// 
//    Rev 1.0   01 Mar 1998 18:50:00   pagbabi
// Initial revision.
// ************************************************************************

#ifndef _NAVNODE_H_
#define _NAVNODE_H_

// class declaration for the NAV extension snapin node

interface IRegister;

class CNavWinNode :
	public CNode,
    public CComCoClass<CNavWinNode,&CLSID_CNavExtension>
{
private:

    // store the registration interface to console
    IRegister*          m_pRegister;

    // store the node name
    static  CString     m_strNodeName;

    // menu items
    static  CString     m_strConfig95;
    static  CString     m_strConfig95Desc;
    static  CString     m_strConfigNT;
    static  CString     m_strConfigNTDesc;
    static  CString     m_strConfigNTS;
    static  CString     m_strConfigNTSDesc;

    enum
    {
        IDM_CONFIGURE_NAV95,
        IDM_CONFIGURE_NAVNT,
        IDM_CONFIGURE_NAVNTS,
    };

public:

DECLARE_REGISTRY(CNavWinNode, _T("NSDUConsole.NAV.1"), _T("NSDUConsole.NAV"), IDS_NAVEXTENSION_DESC, THREADFLAGS_BOTH)

    CNavWinNode();
    virtual ~CNavWinNode();

    // overrides
    virtual const CLSID & GetCoClassID(){ return CLSID_CNavExtension; }
    virtual HRESULT Init();

    // IExtendContextMenu interface
	STDMETHOD(AddMenuItems) (LPDATAOBJECT, LPCONTEXTMENUCALLBACK, long*);
	STDMETHOD(Command) (long, LPDATAOBJECT);

	// IComponentData interface
    STDMETHOD(CreateComponent) (LPCOMPONENT* ppComponent);

    // framework overrides
    STDMETHOD(OnFolder) (LPDATAOBJECT, BOOL, HSCOPEITEM);
    STDMETHOD(OnDestroy) ();
    STDMETHOD(OnCreateDataObject)( long cookie, DATA_OBJECT_TYPES type,
                NodeType nodetype, CDataObject** ppDataObject );

private:

};

#endif
