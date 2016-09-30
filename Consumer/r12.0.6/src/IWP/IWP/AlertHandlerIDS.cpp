#include "StdAfx.h"
#include ".\alerthandlerids.h"
#include "TrayNotifyUI.h"

#include "AlertUI.h"
#include "SymFWAgtInterface.h"
#include "ccLogViewerEventFactory.h"
#include "ccAlertInterface.h"
#include "IDSSignatureInfo.h"

// The event interfaces
#include "ccSerializableEventEx.h"
#include "SymNetDrvAlertEventEx.h" //SymNetDrvLogEventEx needs this
#include "SymNetDrvLogEventEx.h"
#include "IWPSettingsLoader.h"      // For CIWPPathProvider
#include "SymIDSI.h"
#include "IDSLoader.h"

#include "ResourceHelper.h"
#include "resource.h"

using namespace ccEvtMgr;
using namespace SymNeti;
using namespace cc;
using namespace ccSettings;

const extern LPCTSTR sz_SNLog_dll = _T("IWPLog.dll");

CAlertHandlerIDS::CAlertHandlerIDS(void)
{
}

CAlertHandlerIDS::~CAlertHandlerIDS(void)
{
}

IAlertData::ALERT_RESULT CAlertHandlerIDS::DisplayAlert(CEventEx* pEvent)
{
    // Is this a REAL IDS alert?
    //
	// Set up the IDS Alert

    // QI the event
	CSymQIPtr<CSNLogEventEx, &IID_SNLogEventEx> pLogEvent = pEvent;

    if(pLogEvent == NULL)
    {
        CCTRACEE("IWP::DisplayIDSAlert : Failed to QI for CSNLogEventEx");
        return IAlertData::NO_ACTION;
    }

	int iSubType = 0;

	// See if it is an "Alert" type of Log
	pLogEvent->GetEventSubtype(iSubType);

	if (iSubType != CSNLogEventEx::SubTypeAlert)
    {
        CCTRACEI("CAlertHandlerIDS::DisplayAlert: Not a CLogEvent::SubTypeAlert");
        return IAlertData::NO_ACTION;
    }

	int		nSelection;
	char	szMCBuffer[4096] = {0};
	char	szBuffer[256] = {0};
    std::string strAllLocalAdapters;
    DWORD   dwIDSSignature = 0;

	IAlert::ALERTTHREATLEVEL threatlevel = IAlert::THREAT_HIGH;

	DWORD dwSize = sizeof(szMCBuffer);
	DWORD dwFlag = 0;
	VARIANT varDate;
	SYSTEMTIME	sysTime = {0};
    HRESULT hr = S_OK;

	// Show what time the alert happened.
	pLogEvent->GetEventDate (dwFlag, varDate);
	VariantTimeToSystemTime(varDate.date, &sysTime); 
	
    // Load up the Log Formatting Dll and get a LogViewerEventFactory
	CSymInterfaceDLLHelper<&sz_SNLog_dll,
                            CIWPPathProvider, 
                            CSymInterfaceTrustedLoader,
                            ILogViewerEventFactory, 
                            &IID_ILogViewerEventFactory, 
                            &IID_ILogViewerEventFactory> SNLVEventLoader;

	ILogViewerEventFactoryPtr pLVEventFactory;

    if(SYM_FAILED(SNLVEventLoader.CreateObject(&pLVEventFactory)))
    {
        CCTRACEE(_T("CAlertHandlerIDS::DisplayAlert: Failed to Create IID_ILogViewerEventFactory Object from IWPLog.dll\n"));
        return IAlertData::NO_ACTION;
    }

    ILogViewerEvent *pLogViewerEvent = NULL;
    hr = pLVEventFactory->Create(pEvent, pLogViewerEvent);

    if(SUCCEEDED (hr))
    {
		// Get the IDS Log Event Information
		if (pLogViewerEvent->GetDetails(LVEDT_NORMAL, 0, szMCBuffer, dwSize) == S_OK)
		{
			pLVEventFactory->Destroy(pLogViewerEvent);
            pLVEventFactory.Release();
		}
		else
		{
			// Error, Getting Details
			CCTRACEE("CAlertHandlerIDS::DisplayAlert: Failed to pLogViewerEvent->GetDetails()");
			return IAlertData::NO_ACTION;
		} 

        // Attack Name
		if (SUCCEEDED(pLogEvent->GetAttribute(0, (BYTE*)szBuffer, sizeof(szBuffer))))
		{
			SNDATAVALUE* pSNDataValue = (SNDATAVALUE*)szBuffer;
			ASSERT(pSNDataValue->byDataType == CSNLogEventEx::eEV_T_IDS_SIG_ID);
            dwIDSSignature = *((DWORD*)pSNDataValue->abyData);

            // Get the priority
            //
            if ( dwIDSSignature )
            { // Scope so we don't hold IDS open longer than we need to.
                DWORD dwPriority = 3; // default to high

                // Waiting for 1-2VIULV to be fixed.
                CIDSLoader IDSLoader;
                CSymPtr<SymIDS::IIDSSignatureList> pIDSSigList;
                CSymPtr<SymIDS::IIDSSignatureInfo> pIDSSignature;

                SYMRESULT result = IDSLoader.GetObject ( SymIDS::IID_IDSSignatureList, (ISymBase**) &pIDSSigList );
                
                if ( SYM_SUCCEEDED (result))
                {
                    //
                    result = pIDSSigList->Initialize (SymIDS::IID_IDSSignatureInfo);
                    if ( SYM_SUCCEEDED (result ))
                    {
                        result = pIDSSigList->GetSignatureByID (dwIDSSignature, &pIDSSignature);

                        if ( SYM_SUCCEEDED (result))
                        {
                            result = pIDSSignature->GetProperty (SymIDS::PROPERTY_PRIORITY, dwPriority);
                            
                            if ( SYM_SUCCEEDED(result))
                            {
		                        // Set the Threat Level
		                        switch (dwPriority)
		                        {
			                        case 1:
				                        threatlevel = IAlert::THREAT_LOW;
				                        break;
			                        case 3:
				                        threatlevel = IAlert::THREAT_HIGH;
				                        break;
			                        default:
				                        threatlevel = IAlert::THREAT_MED;
				                        break;
		                        }
                            }
                            else
                                CCTRACEE ("CAlertHandlerIDS::DisplayAlert - failed getting priority value 0x%x", result);
                        }
                        else
                            CCTRACEE ("CAlertHandlerIDS::DisplayAlert - failed to get GetSignatureByID 0x%x", result);
                    }
                    else
                        CCTRACEE ("CAlertHandlerIDS::DisplayAlert - failed to initialize pIDSSigList 0x%x", result);
                }
                else
                {
                    CCTRACEE ("CAlertHandlerIDS::DisplayAlert - failed to get IID_IDSSignatureList 0x%x", result);
                }
            }


            // Has the user excluded this IDS signature from alerting?
            //
            if ( m_IDSExclusions.IsExcluded ( dwIDSSignature ))
            {
                CCTRACEI ("CAlertHandlerIDS::DisplayAlert - IDS sig excluded from alert : %d", dwIDSSignature);
                return IAlertData::NO_ACTION;
            }
        }
        else
            CCTRACEE ("CAlertHandlerIDS::DisplayAlert - failed to get signature ID");


        short iPort = 0;
		// Attack IP
        // If the port is 0, display "All local adapters", else display the IP
        if (SUCCEEDED(pLogEvent->GetAttribute(CSNLogEventEx::eEV_T_PORT, (BYTE*)iPort, sizeof(iPort))) && iPort == 0)
        {
            CResourceHelper::LoadString ( IDS_ALL_LOCAL_ADAPTERS, strAllLocalAdapters, g_hInstance );
        }
        else if (SUCCEEDED(pLogEvent->GetAttribute(CSNLogEventEx::eEV_T_IP, (BYTE*)szBuffer, sizeof(szBuffer))))
		{
			SNDATAVALUE* pSNDataValue = (SNDATAVALUE*)szBuffer;
			ASSERT(pSNDataValue->byDataType == CSNLogEventEx::eEV_T_IP);
		}
	}
    else
    {
		// Error, Get pLogViewerEvent Pointer
		CCTRACEE("CAlertHandlerIDS::DisplayAlert: Failed pLVEventFactory->Create");
		return IAlertData::NO_ACTION;
    }

    // First display the tray alert.
    // Then if they want more info display the whole thing.
    //
    CTrayNotifyUI trayalert;
    std::string strTrayAlertText;
    CResourceHelper::LoadString ( IDS_IDS_DESC, strTrayAlertText, g_hInstance );

    if ( !trayalert.Start (strTrayAlertText.c_str()))
        return IAlertData::NO_ACTION;

    // If the user doesn't want "More Info" then we are done.
    if (!trayalert.m_bUserWantsMoreInfo)
        return IAlertData::NO_ACTION;

    CAlertUI alert;
    std::string strTimeDate;

    // Get time/date and add those rows
    //
    {
		char szDate[40];
		char szTime[40];
        char szFormat[100] = {0};
        std::string strTemp;

		GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &sysTime, NULL, szDate, sizeof(szDate));
		GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &sysTime, NULL, szTime, sizeof(szTime));

        CResourceHelper::LoadString ( IDS_INTRUSION_TIME_DATE, strTemp, g_hInstance );
        _snprintf ( szFormat, 99, strTemp.c_str(), szTime, szDate );
	}

    std::string strDetails;
    strDetails = szMCBuffer;
    strDetails += "\r\n";
    strDetails += strTimeDate;

    alert.GetAlert()->SetDetailedDesc (strDetails.c_str());
    alert.GetAlert()->SetShowDetail(true);
    alert.GetAlert()->SetThreatLevel(threatlevel);
    alert.GetAlert()->SetAlertTitle (g_hInstance, IDS_SECURITYALERT );
    alert.GetAlert()->SetBriefDesc ( g_hInstance, IDS_IDS_DESC);
    alert.GetAlert()->SetCheckBoxText ( g_hInstance, IDS_SECDONTASKAGAINCHECK, false); 
    alert.dwHelpID = NAV_CSH_IWP_IDS_ALERT;    

	// Display the alert
    nSelection = alert.DisplayAlert (NULL, HWND_TOPMOST);

    // If the user select to not show this alert again, save off the setting
    //
    if(alert.GetAlert()->GetCheckBoxState ())
        m_IDSExclusions.AddExclusion ( dwIDSSignature );

	return IAlertData::NO_ACTION; 
}