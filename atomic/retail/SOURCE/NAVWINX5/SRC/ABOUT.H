// Copyright 1992 - 1997 Symantec, Peter Norton Group
// ************************************************************************
//
// $Header:   S:/NAVWINX5/VCS/ABOUT.H_v   1.0   01 Jun 1998 16:08:36   DBuches  $
//
// Description:
//
// See Also:
//
// ************************************************************************
// $Log:   S:/NAVWINX5/VCS/ABOUT.H_v  $
// 
//    Rev 1.0   01 Jun 1998 16:08:36   DBuches
// Initial revision.
//
//    Rev 1.0   19 May 1998 15:52:44   PAGBABI
// Initial revision.
// ************************************************************************

// About.h
//

// {C7AEE500-F9A1-11d1-911B-00C04FAC114A}
DEFINE_GUID(CLSID_About,
0xc7aee500, 0xf9a1, 0x11d1, 0x91, 0x1b, 0x0, 0xc0, 0x4f, 0xac, 0x11, 0x4a);

class CNavextAbout :
    public ISnapinAbout,
    public CComObjectRoot,
    public CComCoClass<CNavextAbout, &CLSID_About>
{
public:
    CNavextAbout();
    ~CNavextAbout();

public:
DECLARE_REGISTRY(CNavextAbout, _T("NAVExt.About.1"), _T("NAVExt.About"), IDS_SNAPIN_DESC, THREADFLAGS_BOTH)

BEGIN_COM_MAP(CNavextAbout)
    COM_INTERFACE_ENTRY(ISnapinAbout)
END_COM_MAP()

public:
    STDMETHOD(GetSnapinDescription)(LPOLESTR* lpDescription);
    STDMETHOD(GetProvider)(LPOLESTR* lpName);
    STDMETHOD(GetSnapinVersion)(LPOLESTR* lpVersion);
    STDMETHOD(GetSnapinImage)(HICON* hAppIcon);
    STDMETHOD(GetStaticFolderImage)(HBITMAP* hSmallImage,
                                    HBITMAP* hSmallImageOpen,
                                    HBITMAP* hLargeImage,
                                    COLORREF* cLargeMask);

// Internal functions
private:
    HRESULT AboutHelper(UINT nID, LPOLESTR* lpPtr);
};

