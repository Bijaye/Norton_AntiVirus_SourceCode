////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TestProviderPro.cpp: implementation of the CTestProviderPro class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#define INITIIDS
#include "SymInterface.h"
SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

#include "TestProviderPro.h"
//#include "EventManagerCom.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTestProviderPro::CTestProviderPro()
{
        m_nProviderId = 0;
        m_pEventFactory = NULL;
        m_piProvider = NULL;
        m_hWnd = NULL;
}

CTestProviderPro::~CTestProviderPro()
{
        Disconnect();
}

BOOL CTestProviderPro::Connect()
{
    // Create the event manager
    HRESULT hRes;
    hRes = m_piEventManager.CreateInstance(__uuidof(EVENTMANAGERLib::EventManager));

    if (FAILED(hRes))
    {
        return FALSE;
    }
    return TRUE;
}

BOOL CTestProviderPro::Register(HWND hWnd, 
                                long& nProviderId)
{
    m_hWnd = hWnd;
    nProviderId = 0;

    // Create a class factory
    if (CreateNAVEventFactory(m_pEventFactory) == FALSE ||
        m_pEventFactory == NULL)
    {
        AfxMessageBox ( "Failed CreateNAVEventFactory" );
        Unregister();
        return FALSE;
    }

    // Create a provider
    if (CreateNAVEventProvider(m_pEventFactory,
                              this,
                              m_piProvider) == FALSE ||
        m_piProvider == NULL)
    {
        AfxMessageBox ( "Failed CreateNAVEventProvider" );
        Unregister();
        return FALSE;
    }
    
    long error = CError::eNoError;
    try
    {
        // Register the provider
        _variant_t vEventStates;
        EVENTMANAGERLib::IProviderExPtr piProvider;
        piProvider = m_piProvider;
        error = m_piEventManager->RegisterProvider(piProvider, &m_nProviderId);
    }
    catch (...)
    {
        m_nProviderId = 0;
    }

    if (m_nProviderId == 0)
    {
        AfxMessageBox ( "Failed m_nProviderId == 0" );
        Unregister();
        return FALSE;
    }
    nProviderId = m_nProviderId;

    return TRUE;
}

BOOL CTestProviderPro::Disconnect()
{
    Unregister();

    if (m_piEventManager != NULL)
    {
        m_piEventManager.Release();
    }

    return TRUE;
}

BOOL CTestProviderPro::Unregister()
{
    if (m_nProviderId != 0 &&
        m_piEventManager != NULL)
    {
        try
        {
            m_piEventManager->UnregisterProvider(m_nProviderId);
        }
        catch(...)
        {
        }
        m_nProviderId = 0;
    }

    if (m_piProvider != NULL)
    {
        DeleteNAVEventProvider(m_piProvider);
        m_piProvider = NULL;
    }

    if (m_pEventFactory != NULL)
    {
        DeleteNAVEventFactory(m_pEventFactory);
        m_pEventFactory = NULL;
    }

    m_hWnd = NULL;

    return TRUE;
}

void CTestProviderPro::DeleteEvent(CEventEx* pEvent)
{
    if (m_pEventFactory != NULL)
    {
        m_pEventFactory->DeleteEvent(pEvent);
        pEvent = NULL;
    }
}

CError::ErrorType CTestProviderPro::OnEvent(long nProviderId, 
                                              const CEventEx& Event)
{
    if (m_hWnd != NULL &&
        m_pEventFactory != NULL)
    {
        // Copy event
        CEventEx* pEvent = NULL;
        if (m_pEventFactory->CopyEvent(Event, pEvent) == CError::eNoError)
        {
            ::PostMessage(m_hWnd, UM_EVENT, 0, reinterpret_cast<LPARAM>(pEvent));
        }
    }
    return CError::eNoError;
}

CError::ErrorType CTestProviderPro::OnShutdown()
{
    // We may not directly unregister
    // Posting a message to unregister is safe
    if (m_hWnd != NULL &&
        m_pEventFactory != NULL)
    {
        ::PostMessage(m_hWnd, UM_SHUTDOWN, 0, 0);
    }
    return CError::eNoError;
}

BOOL CTestProviderPro::CreateTestEvent(BOOL bBroadcast, 
                                       long nValue,
                                       long& nEventId,
                                       long lEventType, CString strName )
{
    nEventId = 0;
    HRESULT hr = E_FAIL;

    CComObject<CNAVEventCommonInt>* pNAVEvent;

    if (::CreateNAVEvent(pNAVEvent) == FALSE ||
        pNAVEvent == NULL)
    {
        return FALSE;
    }
    
    pNAVEvent->m_C.SetType ( lEventType );

    switch ( lEventType )
    {
    case AV_Event_ID_Error :
        {
        pNAVEvent->m_C.props.SetData ( AV_Event_Error_propErrorID, 101 );
        pNAVEvent->m_C.props.SetData ( AV_Event_Error_propModuleID, AV_MODULE_ID_NAVLNCH );
        pNAVEvent->m_C.props.SetData ( AV_Event_Error_propResult, E_FAIL );
        //string strTemp = "Test error message";        
        //pNAVEvent->m_C.props.SetData ( AV_Event_Error_propMessage, strTemp.c_str() );
        pNAVEvent->m_C.props.SetData ( AV_Event_Error_propMessage, strName );
        }
        break;
    case AV_Event_ID_VirusAlert :
        {
        pNAVEvent->m_C.props.SetData ( AV_Event_VirusAlert_propVirusID, 101 );
        string strTemp = "NAV.TESTING LOG Virus Name";        
        pNAVEvent->m_C.props.SetData ( AV_Event_VirusAlert_propVirusName, strTemp.c_str() );
        pNAVEvent->m_C.props.SetData ( AV_Event_VirusAlert_propAction, AV_Event_Action_Repaired );
        //strTemp = "C:\\test\\dumbvir.exe";        
        pNAVEvent->m_C.props.SetData ( AV_Event_VirusAlert_propFilePath, strName );
        pNAVEvent->m_C.props.SetData ( AV_Event_VirusAlert_propFeature, AV_MODULE_ID_NAVW );
        pNAVEvent->m_C.props.SetData ( AV_Event_VirusAlert_propType, AV_Event_VirusAlert_Type_File );
        }
        break;
    case AV_Event_ID_VirusLike :
        {
        pNAVEvent->m_C.props.SetData ( AV_Event_VirusLike_propAction, AV_Event_Action_Access_Denied );
        //string strTemp = "C:";        
        pNAVEvent->m_C.props.SetData ( AV_Event_VirusLike_propFilePath, strName );
	    //pNAVEvent->m_C.props.SetData ( AV_Event_VirusLike_propFilePath, strTemp.c_str() );
        //strTemp = "C:\\test\\malware.exe";        
        pNAVEvent->m_C.props.SetData ( AV_Event_VirusLike_propApplicationPath, strName );
        pNAVEvent->m_C.props.SetData ( AV_Event_VirusLike_propFeature, AV_MODULE_ID_MAIN_UI );
        pNAVEvent->m_C.props.SetData ( AV_Event_VirusLike_Attempted_Action, AV_Event_VirusLike_Attempted_Action_HD_Boot_Write );
        }
        break;
    case AV_Event_ID_SBAction :
        {
        pNAVEvent->m_C.props.SetData ( AV_Event_SBAction_propAction, AV_Event_Action_Authorized );
        string strTemp = "C:\\test\\malscript.js";        
        //pNAVEvent->m_C.props.SetData ( AV_Event_SBAction_propFilePath, strTemp.c_str() );
		pNAVEvent->m_C.props.SetData ( AV_Event_SBAction_propFilePath, strName );
        strTemp = "NAV TESTING LOG FileObject";
        pNAVEvent->m_C.props.SetData ( AV_Event_SBAction_propObject, strTemp.c_str() );
        strTemp = "NAV TESTING LOG DeleteFile";
        pNAVEvent->m_C.props.SetData ( AV_Event_SBAction_propMethod, strTemp.c_str() );
        }
        break;
    case AV_Event_ID_ScanAction :
        {

        pNAVEvent->m_C.props.SetData ( AV_Event_ScanAction_propAction, AV_Event_ScanAction_ScanCompleted );
        string strTemp;        
        pNAVEvent->m_C.props.SetData ( AV_Event_ScanAction_propFeature, AV_MODULE_ID_NAVW );
        //strTemp = "My Computer";
        //pNAVEvent->m_C.props.SetData ( AV_Event_ScanAction_propTaskName, strTemp.c_str() );
        pNAVEvent->m_C.props.SetData ( AV_Event_ScanAction_propTaskName, strName );
        strTemp = "NAV TESTING LOG DeleteFile";
        pNAVEvent->m_C.props.SetData ( AV_Event_SBAction_propMethod, strTemp.c_str() );
        pNAVEvent->m_C.props.SetData ( AV_Event_ScanAction_FilesTotalInfected, 10 );
        pNAVEvent->m_C.props.SetData ( AV_Event_ScanAction_FilesRepaired, 5 );
        pNAVEvent->m_C.props.SetData ( AV_Event_ScanAction_FilesQuarantined, 3 );
        pNAVEvent->m_C.props.SetData ( AV_Event_ScanAction_FilesDeleted, 1 );
        }
        break;
    }
    

    long error = CError::eNoError;
    try
    {
        EVENTMANAGERLib::IEventExPtr piEvent (pNAVEvent);

        error = m_piEventManager->CreateEvent(m_nProviderId, 
                                              piEvent, 
                                              bBroadcast == FALSE ? VARIANT_FALSE : VARIANT_TRUE,
                                              &nEventId);
    }
    catch (...)
    {
        error = CError::eFatalError;
    }


    ::DeleteNAVEvent(pNAVEvent);
    pNAVEvent = NULL;

    return (error == CError::eNoError);
}
