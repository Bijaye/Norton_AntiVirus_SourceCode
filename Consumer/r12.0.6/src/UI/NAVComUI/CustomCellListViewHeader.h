#pragma once

class CCustomCellListViewHeader : public CWindowImpl<CCustomCellListViewHeader,
                                                     CHeaderCtrl>, 
                                  public CCustomDraw<CCustomCellListViewHeader>

{
public:
    typedef CCustomDraw<CCustomCellListViewHeader> baseCustomDraw;

    enum 
    {   
        CUSTCELLLISTVIEWHDR_EXSTYLE_NOBORDER = 0x01,
        CUSTCELLLISTVIEWHDR_EXSTYLE_NOEDGE
    };

    CCustomCellListViewHeader(void);
    virtual ~CCustomCellListViewHeader(void);

    BEGIN_MSG_MAP(CCustomCellListViewHeader)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
        MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
        MESSAGE_HANDLER(WM_SYSCOLORCHANGE, OnSysColorChange)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
        MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
        NOTIFY_CODE_HANDLER(HDN_BEGINTRACK , OnBeginTrack)
        CHAIN_MSG_MAP_ALT(baseCustomDraw, 1)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

public:
    LRESULT OnPaint ( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
    LRESULT OnCtlColor ( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );    
    LRESULT OnSettingChange ( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
    LRESULT OnSysColorChange ( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
    LRESULT OnMouseMove ( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled );
    LRESULT OnSetCursor ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                          BOOL & bHandled );
    LRESULT OnNcHitTest ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                          BOOL & bHandled );
    LRESULT OnEraseBkgnd ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                           BOOL & bHandled );

    LRESULT OnBeginTrack(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

    // Overrides
    DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/);
    DWORD OnPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/);
    DWORD OnPreErase(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/);
    DWORD OnPostErase(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/);
    DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/);
    DWORD OnItemPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/);
    DWORD OnItemPreErase(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/);
    DWORD OnItemPostErase(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/);
    DWORD OnSubItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/);

    void Init();
    void SetBackgroundColor(COLORREF crBackground);

protected:
    void PaintHeaderItem(NMCUSTOMDRAW *pnmc);
    void PaintHeaderRect(NMCUSTOMDRAW *pnmc);
    
    void InitControlColor();

protected:
    INT m_iPaintExStyle;

    COLORREF m_crBackground;
    COLORREF m_crText;
    COLORREF m_crDisableText;
    
    CFont m_fontHeader;

    CBrush m_brushWindow;

    bool m_bCustomDraw;
    bool m_bAllowResize;
};
