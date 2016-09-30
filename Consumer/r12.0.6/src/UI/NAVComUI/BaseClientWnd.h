//******************************************************************************
//******************************************************************************
#pragma once
#include "ScanEvents.h"
#include "resource.h"
#include "..\NAVComUIRes\ResResource.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CBaseClientWnd : public CWindowImpl< CBaseClientWnd >
{
public:

    BEGIN_MSG_MAP( CBaseClientWnd )
        COMMAND_ID_HANDLER( ID_ALT_KEY, OnAltKey )
        MESSAGE_HANDLER( UM_GET_HELP_ID, OnGetHelpID )
        MESSAGE_HANDLER( UM_SETACTIVE, OnSetActive )
        MESSAGE_HANDLER( UM_KILLACTIVE, OnKillActive )
    END_MSG_MAP()

    virtual LRESULT OnAltKey() = 0;
    virtual LRESULT OnGetHelpID() = 0;
    virtual LRESULT OnKillActive() = 0;
    virtual LRESULT OnSetActive() = 0;

protected:

    LRESULT OnAltKey(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        return OnAltKey();
    }

    LRESULT OnGetHelpID(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return OnGetHelpID();
    }

    LRESULT OnKillActive(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return OnKillActive();
    }

    LRESULT OnSetActive(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return OnSetActive();
    }
};


