////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ccAlertInterface.h>
#include <ccServiceLoader.h>
#include <AvProdWidgets.h>

namespace SessionApp
{
    class CNotifyClientImpl :
        public ISymBaseImpl< CSymThreadSafeRefCount >, 
        public AvProdWidgets::CSymObjectCreator<&cc::IID_Notify, cc::INotify, CNotifyClientImpl>,
        public cc::INotify3
    {
    public:
        CNotifyClientImpl(void);
        virtual ~CNotifyClientImpl(void);

    public:
        SYM_INTERFACE_MAP_BEGIN()                
            SYM_INTERFACE_ENTRY( cc::IID_Notify,  cc::INotify  ) 
            SYM_INTERFACE_ENTRY( cc::IID_Notify2, cc::INotify2 ) 
            SYM_INTERFACE_ENTRY( cc::IID_Notify3, cc::INotify3 ) 
        SYM_INTERFACE_MAP_END()                  

    public:
        virtual HRESULT Initialize(LPCWSTR wszApplicationName, DWORD dwSessionId);
        virtual void Destroy();

    public: // INotify interface
        // Displays notification.
        virtual RESULT Display(HWND hParentWnd);

        // Hides notification.
        virtual RESULT Hide();

        // Sets the icons for the notification.
        virtual bool SetIcon(HINSTANCE hInstance, UINT nBigIconID, UINT nSmallIconID);

        // Sets the bitmap use for the title banner.
        virtual bool SetTitleBitmap(HINSTANCE hInstance, UINT nBitmapID);

        // Sets the banner text.
        virtual bool SetTitle(LPCSTR szTitle);
        virtual bool SetTitle(LPCWSTR szTitle);
        virtual bool SetTitle(HINSTANCE hInst, UINT nID);

        // Sets the text in Windows' task bar.
        virtual bool SetTaskBarText(LPCSTR szText);
        virtual bool SetTaskBarText(LPCWSTR szText);
        virtual bool SetTaskBarText(HINSTANCE hInst, UINT nID);

        // Sets different properties (ie. show progress bar, topmost ... etc.)
        virtual bool SetProperty(PROPERTIES Property, bool bValue);

        // Sets the timeout for the notification.
        virtual bool SetTimeout(DWORD dwMillisec);

        // Sets the notification text.
        virtual bool SetText(LPCSTR szText);
        virtual bool SetText(LPCWSTR szText);
        virtual bool SetText(HINSTANCE hInst, UINT nID);

        // Sets the tooltip (if client wants to force different text).
        virtual bool SetTooltip(LPCSTR szTooltip);
        virtual bool SetTooltip(LPCWSTR szTooltip);
        virtual bool SetTooltip(HINSTANCE hInst, UINT nID);

        // Sets the progress bar percentage.
        virtual bool SetPercent(UINT nPercentage);

        // Sets the button text and callback.
        virtual bool SetButton(BUTTONTYPE Type, LPCSTR szButtonText);
        virtual bool SetButton(BUTTONTYPE Type, LPCWSTR szButtonText);
        virtual bool SetButton(BUTTONTYPE Type, HINSTANCE hInst, UINT nID);
        virtual bool SetButton(BUTTONTYPE Type, LPCSTR szButtonText, 
            cc::INotifyCallback* pCallback);
        virtual bool SetButton(BUTTONTYPE Type, LPCWSTR szButtonText, 
            cc::INotifyCallback* pCallback);
        virtual bool SetButton(BUTTONTYPE Type, HINSTANCE hInst, UINT nID, 
            cc::INotifyCallback* pCallback);

        virtual bool Reset();

    public: // INotify2
        virtual bool AddCheckbox(LPCSTR szButtonText);
        virtual bool AddCheckbox(LPCWSTR szButtonText);
        virtual bool SetCheckboxState(long lState);
        virtual long GetCheckboxState();

        // Hyperlink support
        virtual bool SetHyperlinkCallback(cc::INotifyCallback* pCallback);

        // Size change support
        virtual bool GetSize(long& cx, long& cy);
        virtual bool SetSize(long cx, long cy);

        // Accessor methods for INotify2 Set methods.
        virtual DWORD GetTimeout();
        virtual UINT GetPercent();

        // The text values for the following methods should be long enough to store the data.
        // the dwSize parameter should indicate the amount of space available in the passed in 
        // pointer in character units not bytes.  It should in
        // If the specified size is not large enough, the dwSize parameter will indicate the number of
        // characters required for the requested text, the string will not be populated and the return value
        // will be false.
        virtual bool GetTitle(LPSTR szTitle, DWORD& dwSize);
        virtual bool GetTitle(LPWSTR szTitle, DWORD& dwSize);
        virtual bool GetTaskBarText(LPSTR szText, DWORD& dwSize);
        virtual bool GetTaskBarText(LPWSTR szText, DWORD& dwSize);
        virtual bool GetProperty(PROPERTIES Property);
        virtual bool GetText(LPSTR szText, DWORD& dwSize);
        virtual bool GetText(LPWSTR szText, DWORD& dwSize);
        virtual bool GetTooltip(LPSTR szTooltip, DWORD& dwSize);
        virtual bool GetTooltip(LPWSTR szTooltip, DWORD& dwSize);
        virtual bool GetButton(BUTTONTYPE Type, LPSTR szButtonText, DWORD& dwSize, cc::INotifyCallback** ppCallback);
        virtual bool GetButton(BUTTONTYPE Type, LPWSTR szButtonText, DWORD& dwSize, cc::INotifyCallback** ppCallback);	
        virtual bool GetIcon(HINSTANCE& hInstance, UINT& nBigIconID, UINT& nSmallIconID);
        virtual bool GetTitleBitmap(HINSTANCE& hInstance, UINT& nBitmapID);

    public: // INotify3
        // Displays notification using eventing to signal that Display() has been called.
        virtual RESULT Display(HWND hParentWnd, HANDLE hEvent);

    protected:
        ccService::IClientPtr m_spCommClient;
        ccService::IComLibPtr m_spCommLib;

        CString m_cszAppName;
        DWORD m_dwSessionId;

        cc::IKeyValueCollectionQIPtr m_spCommand;

    };

}; // SessionApp
