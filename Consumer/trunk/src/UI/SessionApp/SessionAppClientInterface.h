////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SymInterface.h>
#include <ccStringInterface.h>
#include <ccServiceInterface.h>
#include <ccAlertInterface.h>

namespace SessionApp
{
    //****************************************************************************
    //  Custom HRESULT Errors
    //****************************************************************************
    enum 
    {
        E_UnableToConnectToServer = 
            MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF, 1),   //The client was not able to make a connection to the server
        E_BufferTooSmall = 
            MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF, 2)    // The call failed because the buffer was not large enough
    };

    //****************************************************************************
    //  Session App Client interface
    //    This class will establish a connection to whichever AppName and Session
    //    is specified in the Initialize() method.
    //****************************************************************************
    class ISessionAppClient : public ISymBase
    {
    public:
        virtual HRESULT Initialize(LPCWSTR wszApplicationName, DWORD dwSessionId = -1) throw() = 0;
        virtual void Destroy() throw() = 0;

        virtual HRESULT GetCurrentAppState(DWORD &dwAppState) throw() = 0;
        virtual HRESULT SetCurrentAppState(const DWORD dwAppState) throw() = 0;

        virtual HRESULT GetCurrentNotifyState(DWORD &dwNotifyState, LPDWORD pdwRemaining, cc::IString** ppstrToolTip) throw() = 0;
        virtual HRESULT SetCurrentNotifyState(const DWORD dwNotifyState, const DWORD dwDuration, LPCWSTR wszToolTip) throw() = 0;

        virtual HRESULT CreateNotification(cc::INotify*& pNotification) throw() = 0;
        virtual HRESULT CreateAlert(cc::IAlert*& pAlert) throw() = 0;

        virtual HRESULT LaunchApplication(LPCWSTR wszCommandLine) throw() = 0;

        virtual HRESULT GetUserName(cc::IString*& pstrUserName) throw() = 0;
        virtual HRESULT IsAdministrator(bool &bIsAdministrator) throw() = 0;
        virtual HRESULT IsGuest(bool &bIsGuest) throw() = 0;

        virtual HRESULT GetOption(const DWORD dwOptionId, DWORD_PTR pOption) throw() = 0;
        virtual HRESULT SetOption(const DWORD dwOptionId, DWORD_PTR pOption) throw() = 0;

        virtual HRESULT SendCommand(const SYMGUID& guidCommandId, ISymBase *pCommand, ISymBase **ppReturn) throw() = 0;
    };
    
    // {0D532CE2-3CE5-4792-B9AD-576322DE7EAF}
    SYM_DEFINE_GUID(IID_SessionAppClient, 
        0xd532ce2, 0x3ce5, 0x4792, 0xb9, 0xad, 0x57, 0x63, 0x22, 0xde, 0x7e, 0xaf);

    typedef CSymPtr<ISessionAppClient> ISessionAppClientPtr;
    typedef CSymQIPtr<ISessionAppClientPtr, &IID_SessionAppClient> ISessionAppClientQIPtr;


    //****************************************************************************
    //  App State values
    //****************************************************************************
    namespace AppState
    {
        enum{ eInvalid = 0, eFirst, eNormal, eAbnormal, eUnknown, eLast };
    };

    //****************************************************************************
    //  Notify State values
    //****************************************************************************
    namespace NotifyState
    {
        enum{ eInvalid = AppState::eLast, eFirst, eNormal, eWorking, eInformation, eWarning, eLast };
    };

    //****************************************************************************
    //  Options values
    //****************************************************************************
    namespace Options
    {
        enum{ eInvalid, eLast };
    }

    //****************************************************************************
    //  General Detail values
    //****************************************************************************
    namespace GeneralDetail
    {
        enum
        {
            eFirst = 0xF0000000,
            eSessionId,                                   // DWORD, Windows Session Id
            eLast
        } ;
    }

    //****************************************************************************
    //  Display a notification message
    //****************************************************************************
    namespace NotifyMessage
    {
        enum
        {
            eFirst = GeneralDetail::eLast,
            eTitle,                                         // ISTRING, Message title
            eText,                                          // ISTRING, Message text
            eToolTip,                                       // ISTRING, ToolTip text
            eMoreInfo,                                      // ISTRING, More Info text
            eDuration,                                      // DWORD, how long to display
			eProperties,                                    // IKeyValueCollection, property bag
            eWidth,                                         // DWORD, message width
            eHeight,                                        // DWORD, message height
            eLast
        };
    }
    // {13D508B3-2517-490c-9AD6-D1CAB2A1F5D9}
    SYM_DEFINE_GUID(CMDID_NotifyMessage, 
        0x13d508b3, 0x2517, 0x490c, 0x9a, 0xd6, 0xd1, 0xca, 0xb2, 0xa1, 0xf5, 0xd9);

    //****************************************************************************
    //  Display an alert message
    //****************************************************************************
    namespace AlertMessage
    {
        enum
        {
            eFirst = NotifyMessage::eLast,
            eTitle,                                         // ISTRING, Message Title
            eSubtitle,                                      // ISTRING, Message sub title
            eDetail,                                        // ISTRING, Message Detail
            eLast
        };
    }
    // {521221E5-2114-4e62-A37F-EBCD4CADC8E3}
    SYM_DEFINE_GUID(CMDID_AlertMessage, 
        0x521221e5, 0x2114, 0x4e62, 0xa3, 0x7f, 0xeb, 0xcd, 0x4c, 0xad, 0xc8, 0xe3);

    //****************************************************************************
    //  Set the tray icon state
    //****************************************************************************
    namespace IconState
    {
        enum
        {
            eFirst = AlertMessage::eLast,
            eAppState,                                       // DWORD, icon app state
            eNotifyState,                                    // DWORD, icon notify state 
            eDuration,                                       // DWORD, icon notify duration
            eToolTip,                                        // ISTRING, ToolTip text
            eLast
        };
    }
    // {03CB7510-8887-400d-B6B2-21AF9947044D}
    SYM_DEFINE_GUID(CMDID_SetIconState, 
        0x3cb7510, 0x8887, 0x400d, 0xb6, 0xb2, 0x21, 0xaf, 0x99, 0x47, 0x4, 0x4d);

    // {0458C00E-ABA9-40c6-A922-39B3971E9082}
    SYM_DEFINE_GUID(CMDID_GetIconState, 
        0x458c00e, 0xaba9, 0x40c6, 0xa9, 0x22, 0x39, 0xb3, 0x97, 0x1e, 0x90, 0x82);

    //****************************************************************************
    //  Display an alert message
    //****************************************************************************
    namespace CustomCommand
    {
        enum
        {
            eFirst = IconState::eLast,
            eCommandId,                                        // GUID, Message id
            eCommandDataGuid,                                  // GUID, Serialized Command Data Object Id
            eCommandData,                                      // BINARY, Serialized Command Data
        };
    }
    
    // {584375D2-D4E4-43f1-BBA0-697CBA9D703E}
    SYM_DEFINE_GUID(CMDID_CustomCommand, 
        0x584375d2, 0xd4e4, 0x43f1, 0xbb, 0xa0, 0x69, 0x7c, 0xba, 0x9d, 0x70, 0x3e);


}