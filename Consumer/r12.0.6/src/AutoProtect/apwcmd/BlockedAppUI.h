#pragma once

// Events
#include "AllNAVEvents.h"
#include "AvEvents.h"
#include "ccAlertInterface.h"       // Common Client IM-style alert
#include "SimpleTimer.h"
#include <memory> // for auto_ptr

class CBlockedAppUI;

// Sink that call the client back when a dialog has closed.
//
class CBlockedAppUISink
{
public:
    CBlockedAppUISink(){};
    virtual ~CBlockedAppUISink(){};

    virtual void OnBlockedAppUIClosed () = 0;

    // User selected cancel
    //
    virtual void OnBlockedAppUICancel () = 0;
};

// Help button sink
//
class CBlockedAppUIHelp :
    public cc::INotifyCallback,
    public ISymBaseImpl<CSymThreadSafeRefCount>  
{
public:
    CBlockedAppUIHelp(DWORD dwHelpID);
    virtual ~CBlockedAppUIHelp(){};

    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(cc::IID_NotifyCallback, cc::INotifyCallback)
	SYM_INTERFACE_MAP_END() 

    virtual bool Run(HWND hWndParent, unsigned long nID, cc::INotify* pAlert);

private:
    DWORD m_dwHelpID;
};

class CBlockedAppUICloser; // forward declare
class CBlockedAppUI : public ccLib::CThread
{
public:
    // We will delete the CEventData when we exit
    //
    CBlockedAppUI( SAVRT_ROUS64 hCookie,
                   long lProcessID,
                   std::wstring strPath,
                   DWORD dwWaitTime,
                   CBlockedAppUISink* pSink = NULL /*optional*/);
    ~CBlockedAppUI(void);

    // CThread overrides
    //
    int Run ();

    // Close this UI
    //
    // bCloseNow overrides the close timer.
    // 
    void Close (bool bCloseNow = false);

    // Show the UI for at least this much time (no flickers!)
    static void SetWaitTimeMinDisplayBlocked ( DWORD dwWaitTimeMinDisplayBlocked );

    // Close the UI if an app is blocked for this long. Doesn't cancel.
    static void SetWaitTimeMaxDisplayBlocked ( DWORD dwWaitTimeMaxDisplayBlocked );

    // Action to take instead of displaying the UI
    static void SetForcedAction ( DWORD dwForcedAction );

    static long GetNumActiveUI ();

    // Time since we displayed the UI, in milliseconds
    //
    CSimpleTimer timer;

protected:
    bool getAppPath ( DWORD dwProcessID /*in*/, std::wstring& strAppFullPath /*out*/);
    bool getAppRealName ( std::wstring& strAppFullPath /*in*/, std::wstring& strAppRealName /*out*/ );

    CBlockedAppUISink* m_pSink;
    std::auto_ptr<CBlockedAppUICloser> m_pCloser; // self-terminating

    // Our copy of the data!
    SAVRT_ROUS64 m_hCookie;
    std::wstring m_strPath;
    long m_lProcessID;
    DWORD m_dwBlockedDelay;
    
    cc::INotifyPtr m_pUI;

    static long m_lNumActiveUI;
    static DWORD m_dwWaitTimeMinDisplayBlocked;
    static DWORD m_dwWaitTimeMaxDisplayBlocked;
    static DWORD m_dwForcedAction;

    // Is the UI being displayed?
    bool m_bDisplaying;
    bool m_bClosing;

    // For initializing the static stuff
    //
    void init (); 

    static std::string m_strCCPath;

    // Show this UI
    //
    void display ();

    // Help button callback
    //
    CBlockedAppUIHelp* m_pHelpCallback;

    // Have we decremented this count yet?
    //
    bool m_bDecremented;

    // Sync for data in this class
    ccLib::CCriticalSection   m_critUI;

	// Branding product name
	CString m_csProductName;
};

class CBlockedAppUICloser : public ccLib::CThread
{
public:
    CBlockedAppUICloser ( cc::INotifyPtr& pUI, DWORD dwWaitTime );
    virtual ~CBlockedAppUICloser ();

    int Run ();

protected:
    cc::INotifyPtr m_pUI;
    DWORD m_dwWaitTime;
};
