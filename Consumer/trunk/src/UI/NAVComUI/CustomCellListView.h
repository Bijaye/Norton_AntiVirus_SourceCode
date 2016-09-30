////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//***************************************************************************
// CustomCellListView.h: interface for the CCustomCellListView class.
//***************************************************************************
#pragma once

#include "CustomCellSelectList.h"
#include "CustomCellSelectListItem.h"
#include "CustomCellListViewHeader.h"
#include <vector>

class CCustomCellListView : public CWindowImpl<CCustomCellListView, 
                                               CListViewCtrl>, 
                            public CCustomDraw<CCustomCellListView>, 
                            public CMessageFilter
{
public:
    CCustomCellListView();
    virtual ~CCustomCellListView();
    
    typedef struct _LVCOMBOITEMCHANGE
    {
        NMHDR hdr; 
        LVITEM LVItem;
        LVITEM LBItem;
    } LVCOMBOITEMCHANGE, *LPLVCOMBOITEMCHANGE;

    typedef struct _LVCOMBOITEMACTIVATE
    {
        NMHDR hdr; 
        NMITEMACTIVATE itemActivate;
        std::vector<CCustomCellSelectListItem *> listItems;
        UINT nCurrentSelection;
    } LVCOMBOITEMACTIVATE, *LPLVCOMBOITEMACTIVATE;

    //
    //  Enumerations
    //
    enum
    {
        CUSTOM_CELL_LISTVIEW_STYLE_NORMAL = 0,
        CUSTOM_CELL_LISTVIEW_STYLE_DOTTED = 1
    };
    enum 
    {   
        CUSTOM_CELL_LISTVIEW_EXSTYLE_NOBORDER = 0x01,
        CUSTOM_CELL_LISTVIEW_EXSTYLE_HEADER_NOEDGE
    };
    enum
    {
        CELLTYPE_NORMAL = 1,
        CELLTYPE_HYPERLINK,
        CELLTYPE_COMBOBOX
    };
    enum
    {
        LVN_HYPERLINKACTIVATE = (LVN_FIRST-90),
        LVN_COMBOITEMACTIVATE  = (LVN_FIRST-91),
        LVN_COMBOITEMCHANGE  = (LVN_FIRST-92),
    };

    virtual BOOL PreTranslateMessage(MSG* pMsg);

    BEGIN_MSG_MAP(CCustomCellListView)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
        MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
        MESSAGE_HANDLER(WM_SYSCOLORCHANGE, OnSysColorChange)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
        REFLECTED_NOTIFY_CODE_HANDLER(HDN_BEGINTRACK , OnHeaderBeginTrack)
        REFLECTED_NOTIFY_CODE_HANDLER(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
        REFLECTED_NOTIFY_CODE_HANDLER(LVN_KEYDOWN, OnLvnKeyDown)
        CHAIN_MSG_MAP_ALT(CCustomDraw<CCustomCellListView>, 1)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()
    
    LRESULT OnCreate( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                      BOOL & bHandled );
    LRESULT OnPaint ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                      BOOL & bHandled );
    LRESULT OnLButtonDown ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                      BOOL & bHandled );
    LRESULT OnSettingChange ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                      BOOL & bHandled );
    LRESULT OnSysColorChange ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                      BOOL & bHandled );
    LRESULT OnCtlColor ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                      BOOL & bHandled );    
    LRESULT OnMouseMove ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                      BOOL & bHandled );    
    LRESULT OnSetCursor ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                      BOOL & bHandled );    
    
    LRESULT OnBeginLabelEdit(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnHeaderBeginTrack(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnLvnKeyDown(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
    
    // Overrides
    virtual void OnFinalMessage(HWND hWnd);

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

    INT m_iPaintStyle;
    INT m_iPaintExStyle;

    COLORREF m_crText;
    COLORREF m_crDisableText;
    COLORREF m_crBackground;
    COLORREF m_crLightRows;
    COLORREF m_crDarkRows;

    virtual BOOL GetCellType(INT iRow, INT iCol, DWORD *dwType) = 0;
    virtual BOOL GetCellTextColor(INT iRow, INT iCol, COLORREF *pcrColor) = 0;

    void SetAlternating(BOOL bAlternate);
    void SetNormalFont(UINT fontName);
    void SetHeaderFont();

    CCustomCellListViewHeader & GetCustomHeader();

protected:
    LRESULT OnLButtonDown_ComboBox(INT iItem, INT iSubItem, POINT &pt);
    LRESULT OnLButtonDown_Hyperlink(INT iItem, INT iSubItem, POINT &pt);

    void CDPaintNormal(LPNMCUSTOMDRAW lpNMCustomDraw);
    void CDPaintComboBoxImage(LPNMCUSTOMDRAW lpNMCustomDraw);
    void CDPaintHyperlink(LPNMCUSTOMDRAW lpNMCustomDraw);
    void CDPaintRowColors(HDC hdc);

protected:
    int HitTestEx(const POINT &pt, int &iCol);

    void CalcIdealTextRect(INT iItem, INT iSubItem, RECT &rectText, 
                           CFont &font);

    COLORREF m_clrLink;
    CFont m_fontHyperLink;
    HCURSOR m_hHyperlinkCursor;
    HCURSOR m_hPrevCursor;

    bool m_bHandcursor;
    INT m_iNavColumn;

    CFont m_fontComboBox;
    CFont m_fontHeader;
    CFont m_fontNormal;

    CBrush m_brushWindow;
    
    void InitControlColor();
    
    CCustomCellSelectList m_wndSelectList;
    CCustomCellListViewHeader m_wndHeader;
     
    BOOL m_bAlternate;
};

