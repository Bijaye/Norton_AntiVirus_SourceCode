////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of the CNotifyIconData class and the CTrayIconImpl template.
#pragma once

#include <atlmisc.h>

// Wrapper class for the Win32 NOTIFYICONDATA structure
class CNotifyIconData : public NOTIFYICONDATA
{
public:	
	CNotifyIconData()
	{
		memset(this, 0, sizeof(NOTIFYICONDATA));
		cbSize = sizeof(NOTIFYICONDATA);
	}
};

// Template used to support adding an icon to the taskbar.
// This class will maintain a taskbar icon and associated context menu.
template <class T>
class CTrayIconImpl
{
private:
	UINT WM_TRAYICON;
	CNotifyIconData m_nid;
	UINT m_nDefault;
protected:
    bool m_bInstalled;
public:	
	CTrayIconImpl() : m_bInstalled(false), m_nDefault(0)
	{
		WM_TRAYICON = ::RegisterWindowMessage(_T("WM_TRAYICON"));
	}
	
	~CTrayIconImpl()
	{
		// Remove the icon
		RemoveIcon();
	}

	// Install a taskbar icon
	// 	lpszToolTip 	- The tooltip to display
	//	hIcon 		- The icon to display
	// 	nID		- The resource ID of the context menu
	/// returns true on success
	bool InstallIcon(LPCTSTR lpszToolTip, HICON hIcon)
	{
		T* pT = static_cast<T*>(this);
		// Fill in the data		
		m_nid.hWnd = pT->m_hWnd;
		m_nid.uID = 0x4907;
		m_nid.hIcon = hIcon;
		m_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		m_nid.uCallbackMessage = WM_TRAYICON;
		_tcscpy(m_nid.szTip, lpszToolTip);
		// Install
		m_bInstalled = Shell_NotifyIcon(NIM_ADD, &m_nid) ? true : false;
		// Done
		return m_bInstalled;
	}

    // Change the taskbar icon
    // 	lpszToolTip 	- The tooltip to display
    //	hIcon 		- The icon to display
    // 	nID		- The resource ID of the context menu
    /// returns true on success
    bool ChangeIcon(HICON hIcon)
    {
        T* pT = static_cast<T*>(this);
        // Fill in the data		
        m_nid.hWnd = pT->m_hWnd;
        m_nid.hIcon = hIcon;
        m_nid.uFlags = NIF_ICON;
        m_nid.uCallbackMessage = WM_TRAYICON;

        // Change
        return Shell_NotifyIcon(NIM_MODIFY, &m_nid) ? true : false;
    }

    // Remove taskbar icon
	// returns true on success
	bool RemoveIcon()
	{
		if (!m_bInstalled)
			return false;

        m_bInstalled = false;

		// Remove
		m_nid.uFlags = 0;
		return Shell_NotifyIcon(NIM_DELETE, &m_nid) ? true : false;
	}

	// Set the icon tooltip text
	// returns true on success
	bool SetTooltipText(LPCTSTR pszTooltipText)
	{
		if (pszTooltipText == NULL)
			return FALSE;
		// Fill the structure
		m_nid.uFlags = NIF_TIP;
		_tcscpy(m_nid.szTip, pszTooltipText);
		// Set
		return Shell_NotifyIcon(NIM_MODIFY, &m_nid) ? true : false;
	}

	BEGIN_MSG_MAP(CTrayIcon)
		MESSAGE_HANDLER(WM_TRAYICON, OnTrayIcon)
	END_MSG_MAP()

	LRESULT OnTrayIcon(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		// Is this the ID we want?
		if (wParam != m_nid.uID)
			return 0;
		T* pT = static_cast<T*>(this);
		// Was the right-button clicked?
		if (LOWORD(lParam) == WM_RBUTTONUP)
		{
			// Load the menu
			HMENU hMenu = NULL;
            // Prepare
			pT->PrepareMenu(hMenu);
            if(hMenu)
            {
                CMenuHandle oPopup(GetSubMenu(hMenu, 0));
                // Get the menu position
                CPoint pos;
                GetCursorPos(&pos);
                // Make app the foreground
                SetForegroundWindow(pT->m_hWnd);
                // Track
                oPopup.TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, pT->m_hWnd);
                // BUGFIX: See "PRB: Menus for Notification Icons Don't Work Correctly"
                pT->PostMessage(WM_NULL);
                // Done
                oPopup.DestroyMenu();
            }
		}
        else if (LOWORD(lParam) == WM_LBUTTONUP)
        {
            // Make app the foreground
            SetForegroundWindow(pT->m_hWnd);
            // Call double click handler
            OnTrayItemClick();
        }
		else if (LOWORD(lParam) == WM_LBUTTONDBLCLK)
		{
			// Make app the foreground
			SetForegroundWindow(pT->m_hWnd);
            // Call double click handler
            OnTrayItemDblClick();
		}
		return 0;
	}

	// Allow the menu items to be enabled/checked/etc.
	virtual void PrepareMenu(HMENU &hMenu)
	{
		// Stub
	}

    // Handle single click
    virtual void OnTrayItemClick()
    {
        // Stub
    }

    // Handle double click
    virtual void OnTrayItemDblClick()
    {
        // Stub
    }


};
