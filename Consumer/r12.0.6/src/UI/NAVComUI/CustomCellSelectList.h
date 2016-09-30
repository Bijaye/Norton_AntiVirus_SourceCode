#pragma once

class CCustomCellSelectList : public CWindowImpl<CCustomCellSelectList, 
                                                 CListBox>, 
                              public CMessageFilter
{
public:
    CCustomCellSelectList(void);
    virtual ~CCustomCellSelectList(void);

    virtual BOOL PreTranslateMessage(MSG* pMsg);

    BEGIN_MSG_MAP(CCustomCellSelectList)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
        MESSAGE_HANDLER(WM_NCDESTROY, OnNcDestroy)
        MESSAGE_HANDLER(WM_CHAR, OnChar)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
        REFLECTED_NOTIFY_CODE_HANDLER(CBN_SELCHANGE, OnSelChanged)
        REFLECTED_NOTIFY_CODE_HANDLER(CBN_KILLFOCUS, OnLbnKillFocus)
    END_MSG_MAP()

    LRESULT OnPaint ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                      BOOL & bHandled );
    LRESULT OnKillFocus ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                          BOOL & bHandled );
    LRESULT OnNcDestroy ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                          BOOL & bHandled );
    LRESULT OnChar ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                     BOOL & bHandled );
    LRESULT OnCreate ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                       BOOL & bHandled );
    LRESULT OnMouseMove ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                          BOOL & bHandled );
    LRESULT OnLButtonDown ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                            BOOL & bHandled );
    LRESULT OnLButtonUp ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                          BOOL & bHandled );
    
    LRESULT OnSelChanged(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

    LRESULT OnLbnKillFocus(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    // Overrides
    virtual void OnFinalMessage(HWND hWnd);

private:
    void SendComboItemChangeMsg();

public:
    int m_iItem;
    int m_iSubItem;
    int m_iCtrlId;

    BOOL m_bESC;
    BOOL m_bDragging;
    BOOL m_bLButtonReleased;
    
    LPARAM m_lParam;

    HWND m_hwndGrandParent;

};
