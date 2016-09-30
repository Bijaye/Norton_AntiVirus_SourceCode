// WaitForShutdown.h: interface for the CWaitForShutdown class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAITFORSHUTDOWN_H__F18AB8AF_991D_44B2_B026_A2F769C61FA4__INCLUDED_)
#define AFX_WAITFORSHUTDOWN_H__F18AB8AF_991D_44B2_B026_A2F769C61FA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

class CWaitForShutdown  
{
public:
	bool GetShutdown();

	CWaitForShutdown();
	virtual ~CWaitForShutdown();

    static LRESULT CALLBACK CWaitForShutdown::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static unsigned int __stdcall CWaitForShutdown::threadProc( void* pThis );	
    void Terminate();

protected:
    ::std::string  m_strWindowName;

    unsigned int InternalThreadProc();
    void createWindow ();

    // Set this when we should close
    StahlSoft::CSmartHandle m_shShutdownEvent;

    // For setting the value of shutdown
    StahlSoft::CCritSec     m_shCritShutdown;

	void setShutdown( bool bShutdown );

    // Have we received the shutdown message?
    bool    m_bShutdown;    

    // Handle to our thread
	ULONG				m_ulMonitorThread;

	// Handle to window
	HWND				m_hWnd;

	// Instance handle used for window operations
	HINSTANCE			m_hInstance;
};

#endif // !defined(AFX_WAITFORSHUTDOWN_H__F18AB8AF_991D_44B2_B026_A2F769C61FA4__INCLUDED_)
