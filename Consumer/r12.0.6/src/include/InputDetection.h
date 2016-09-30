
// InputDetection.h: interface for the CInputDetection class.
//
//////////////////////////////////////////////////////////////////////
#pragma once


#define DIRECTINPUT_VERSION 0x0300

//
//  <Init the DirectX GUIDs>
#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#include <Dinput.h>
#undef INITGUID
#else
#include <guiddef.h>
#include <Dinput.h>
#endif

//  </Init the DirectX GUIDs>
//

#include "StahlSoft.h"
#pragma comment(lib, "dinput")

//direct input notifications
const TCHAR DX_KEYBOARD_EVENT[] = _T("SYM:CKA:KeyboardNotify");
const TCHAR DX_MOUSE_EVENT[]    = _T("SYM:CKA:MouseNotify");

const TCHAR CKA_REG_KEY[]       = _T("Software\\Symantec\\CKA");
const DWORD TIME_STANDBY        = 4;


#define LPInputDetectionSink CInputDetectionSink*
class CInputDetectionSink
{
public:
	virtual void OnInputDeteted()=0;
};

#define LPInputDetectionEventSource CInputDetectionEventSource*
class CInputDetectionEventSource : public StahlSoft::CEventSourceT<CInputDetectionSink*>
{
public:
    CInputDetectionEventSource(){}
    virtual ~CInputDetectionEventSource(){}

    void Fire_OnInputDetected()
    {
        StahlSoft::CSmartLock smLock(&m_lock);

        iterator it = begin();
	    while(it!=end())
	    {
		    try
		    {
			    (*it)->OnInputDeteted();
			    it++;
		    }
		    catch(...)
		    {
		    }
	    }
    }
};

#define LPInputDetection CInputDetection*
class CInputDetection : 
    public CInputDetectionEventSource,
    public StahlSoft::CSystemThreadBase
{

public:
	CInputDetection();
	virtual ~CInputDetection();

    HRESULT Initialize(HWND hwnd, HINSTANCE hInstance);
    
    HRESULT SetHwnd(HWND hwnd);
    HRESULT SetHinstance(HINSTANCE hInstance);

	STDMETHOD(IsRunning)();
	STDMETHOD(RequestExit)();
    
protected:
	//Thread functions
	STDMETHOD(InitInstance)();
	STDMETHOD(RunInstance)();
	STDMETHOD(ExitInstance)();

private:
    HRESULT InternalInitialize();

    // Event Handlers
    void OnInputDetected();

	StahlSoft::CSmartHandle m_shStop;
	StahlSoft::CSmartHandle m_shMouseEvent;
    StahlSoft::CSmartHandle m_shKeyEvent;
    StahlSoft::CSmartHandle m_shCycleEvent;

	BOOL m_bRunning; 

    CComPtr<IDirectInput>       m_pDI;         
    CComPtr<IDirectInputDevice> m_pKeyboard;
    CComPtr<IDirectInputDevice> m_pMouse;
    
    HRESULT m_hrCoInit;

    HINSTANCE    m_hInstance;
    HWND         m_hWnd;

};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

inline CInputDetection::CInputDetection() :
//30 minutes default
m_hInstance(NULL),
m_hWnd(NULL),
m_hrCoInit(E_FAIL)
{
    //
    //  This event will be signaled for the thread to stop processing
    //   and exit.
    m_shStop = ::CreateEvent(NULL,TRUE,FALSE,NULL);
    
}

inline CInputDetection::~CInputDetection()
{
    CCTRACEI("CInputDetection::~CInputDetection() - Begin");
    RequestExit();
    CCTRACEI("CInputDetection::~CInputDetection() - End");
}

inline HRESULT CInputDetection::Initialize(HWND hwnd, HINSTANCE hInstance)
{
    SetHwnd(hwnd);
    SetHinstance(hInstance);
    
    return CSystemThreadBase::CreateThread();
}

inline HRESULT CInputDetection::InternalInitialize()
{
    HRESULT hrStatus = S_OK;

    //create our notification events

    m_shKeyEvent = CreateEvent(NULL, FALSE, FALSE, DX_KEYBOARD_EVENT);

    if(m_shKeyEvent == NULL)
    {
        DWORD dwError = ::GetLastError();
        return HRESULT_FROM_WIN32(dwError);
    }

    m_shMouseEvent = CreateEvent(NULL, FALSE, FALSE, DX_MOUSE_EVENT);

    if(m_shMouseEvent == NULL)
    {
        DWORD dwError = ::GetLastError();
        return HRESULT_FROM_WIN32(dwError);
    }

    if(m_pDI!=NULL)
    {
        m_pDI.Release();
    }

    hrStatus = DirectInputCreate( m_hInstance, DIRECTINPUT_VERSION, &m_pDI, NULL );

    if(m_pMouse!=NULL)
    {
        m_pMouse.Release();
    }

    // Obtain an interface to the system mouse device.
    if( SUCCEEDED( hrStatus ))
    {
        hrStatus = m_pDI->CreateDevice( GUID_SysMouse, &m_pMouse, NULL );
    }


    if(m_pKeyboard!=NULL)
    {
        m_pKeyboard.Release();
    }

    // Obtain an interface to the system keyboard device
    if( SUCCEEDED( hrStatus ))
    {
        hrStatus = m_pDI->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, NULL );
    }

    if( SUCCEEDED( hrStatus ))
    {
        hrStatus = m_pMouse->SetDataFormat( &c_dfDIMouse );
    }

    if( SUCCEEDED( hrStatus ))
    {
        hrStatus = m_pKeyboard->SetDataFormat( &c_dfDIKeyboard );
    }

    //set our monitors up
    if( SUCCEEDED( hrStatus ))
    {
        hrStatus = m_pMouse->SetCooperativeLevel(m_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    }

    //set our monitors up
    if( SUCCEEDED( hrStatus ))
    {
        hrStatus = m_pKeyboard->SetCooperativeLevel(m_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    }

    if( SUCCEEDED( hrStatus ))
    {
        hrStatus = m_pMouse->SetEventNotification(m_shMouseEvent);

        if(hrStatus == DI_POLLEDDEVICE)
        {
            hrStatus = S_OK;
        }
    }


    if( SUCCEEDED( hrStatus ))
    {
        hrStatus = m_pKeyboard->SetEventNotification(m_shKeyEvent);

        if(hrStatus == DI_POLLEDDEVICE)
        {
            hrStatus = S_OK;
        }
    }

    //acquire the devices so we can monitor them
    if( SUCCEEDED( hrStatus ))
    {
        hrStatus =m_pMouse->Acquire();
    }

    if( SUCCEEDED( hrStatus ))
    {
        hrStatus =m_pKeyboard->Acquire();
    }

    return hrStatus;
}

inline HRESULT CInputDetection::SetHwnd(HWND hwnd)
{
    HRESULT hrStatus = S_OK;

    if(m_hWnd)
    {
        m_hWnd = hwnd;
    }
    else
    {
        hrStatus = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
    }

    return hrStatus;
}

inline HRESULT CInputDetection::SetHinstance(HINSTANCE hInstance)
{
    HRESULT hrStatus = S_OK;

    if(hInstance)
    {
        m_hInstance = hInstance;
        SetEvent(m_shCycleEvent);
    }
    else
    {
        hrStatus = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
    }

    return hrStatus;
}

inline STDMETHODIMP CInputDetection::InitInstance()
{
    m_hrCoInit = CoInitialize(NULL);
    return InternalInitialize();
}

inline STDMETHODIMP CInputDetection::RunInstance()
{
    HRESULT hrStatus = S_OK;

    DWORD dwInputStatus = WAIT_OBJECT_0;

    HANDLE hInputEvents[3] = 
    {
        m_shMouseEvent,
        m_shKeyEvent,
        m_shStop
    };

    DWORD   dwError;

    // Wait for input detection or the stop event
    dwInputStatus=WaitForMultipleObjects(3, hInputEvents, FALSE, INFINITE);

    switch(dwInputStatus)
    {
    case WAIT_OBJECT_0:
    case WAIT_OBJECT_0+1:
        CCTRACEI("CInputDetection::RunInstance() - Input detected");
        OnInputDetected();
        break;

    case WAIT_OBJECT_0+2:
        CCTRACEI("CInputDetection::RunInstance() - Received stop event");
        break;

    case WAIT_ABANDONED:
        CCTRACEE("CInputDetection::RunInstance() - Mouse Event Abandoned");
        break;
    case WAIT_ABANDONED+1:
        CCTRACEE("CInputDetection::RunInstance() - Keyboard Event Abandoned");
        break;

    case WAIT_FAILED:
        dwError = ::GetLastError();
        CCTRACEE("CInputDetection::RunInstance() - Input Handle Error %d : %d", NULL, dwError);
        break;

    default:
        break;
    }

    return hrStatus;
}

inline STDMETHODIMP CInputDetection::ExitInstance()
{
    CCTRACEI("CInputDetection::ExitInstance() - Exiting");
    HRESULT hrStatus = S_OK;

    if(SUCCEEDED(m_hrCoInit))
    {
        CoUninitialize();
        m_hrCoInit = E_FAIL;
    }

    return hrStatus;
}

inline STDMETHODIMP CInputDetection::IsRunning()
{
    return  m_bRunning ? S_OK:S_FALSE;
}

inline STDMETHODIMP CInputDetection::RequestExit()
{
    CCTRACEI("CInputDetection::RequestExit() - Begin");
    ::SetEvent(m_shStop);
    m_bRunning = false;

    DWORD dwRet = WaitForSingleObject(GetThreadHandle(), 2000);
    if(dwRet == WAIT_TIMEOUT)
    {
       CCTRACEE(_T("CInputDetection::RequestExit - thread did not exit properly. Terminating!!"));
       TerminateThread();
    }

    CCTRACEI("CInputDetection::RequestExit() - End");
    return S_OK;
}

inline void CInputDetection::OnInputDetected()
{
    Fire_OnInputDetected();
}
