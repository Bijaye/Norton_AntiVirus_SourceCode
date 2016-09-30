// CErrorCategory.cpp: implementation of the ErrorCategory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVEvent.h"
#include "TechSupportURL.h"

using namespace std;
#include "ErrorCategory.h"
#include "AVccLogViewerPluginId.h"
#include "..\navlogvres\resource.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CErrorCategory::CErrorCategory()
{
	m_pFactory = NULL;
    m_dwHelpID =  IDH_NAVW_CCLOGVIEW_ERRORS;

    CResourceHelper::LoadString ( IDS_CATEGORY_NAME_ERRORS, m_strCategoryName, _Module.GetResourceInstance() );

    m_iCategoryID = CC_NAV_CAT_ERRORS;
    
    GetFactory(NAV_AV_EVENT_OBJECT_ID, &m_pFactory);
    
    // Fill the columns
    //
    m_vecColumnTitles.resize (columnLast);

    m_vecColumnTitles[columnDate] = CResourceHelper::LoadString (IDS_COLUMN_DATE, _Module.GetResourceInstance());
    m_vecColumnTitles[columnUserName] = CResourceHelper::LoadString (IDS_COLUMN_USER_NAME, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnComputerName] = CResourceHelper::LoadString (IDS_COLUMN_COMPUTER_NAME, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnFeature] = CResourceHelper::LoadString (IDS_COLUMN_FEATURE, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnErrorCode] = CResourceHelper::LoadString (IDS_COLUMN_ERROR_CODE, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnMessage] = CResourceHelper::LoadString (IDS_COLUMN_ERROR_MESSAGE, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnProductVersion] = CResourceHelper::LoadString (IDS_COLUMN_PRODUCT_VERSION, _Module.GetResourceInstance() );

	CString csFormat;
	CString csBuffer;
	csFormat.LoadString(_Module.GetResourceInstance(), IDS_CATEGORY_DESC_ERRORS);
	csBuffer.Format(csFormat, m_csProductName);
    m_strCategoryDescription = csBuffer;

    // Event types that we care about
    //
    m_nEventType = AV::Event_ID_Error;
    m_nContextID = 0;

    m_strErrorLink = CResourceHelper::LoadString ( IDS_ERROR_LINK, _Module.GetResourceInstance() );
}

CErrorCategory::~CErrorCategory()
{
}


BOOL CErrorCategory::SetEventProperties( ccEvtMgr::CEventEx* pCEvent,
                                         const ccEvtMgr::CLogFactoryEx::EventInfo& Info,
                                         cc::ILogViewerEvent* pLogViewerEvent)
{
    try
    {
        CAVEvent* pAVEvent = static_cast<CAVEvent*>(pLogViewerEvent);
        CNAVEventCommon* pNAVEvent = static_cast<CNAVEventCommon*>(pCEvent);

        long lType = 0;
        pNAVEvent->props.GetData(AV::Event_Base_propType, lType );

        switch ( lType )
        {
            case AV::Event_ID_Error :
            {
                pAVEvent->lEventType = lType;

                // First column is the date
                //
                DATE dtDate;
                _variant_t vValue;
                SYSTEMTIME sysTime;
                CopyMemory ( &sysTime, &Info.m_TimeStamp, sizeof (SYSTEMTIME));
                SystemTimeToVariantTime(&sysTime, &dtDate);

                // Log is in UTC System time.
                //
                V_VT(&vValue) = VT_DATE;
                V_DATE(&vValue) = dtDate;
                pAVEvent->AddData (columnDate, vValue, LV_DATE_FORMAT_UTC);
                
                // User Name
                //
                std::string strValue;
                char szTempVal [100]= {0};
                long lBufSize = 0;
                long lTempVal = 0;

                _variant_t vUserName;
                strValue.clear();
                if ( !pNAVEvent->props.GetData(AV::Event_Base_propUserName, strValue ) || strValue.empty())
                {
                    CResourceHelper::LoadString ( IDS_NOT_APPLICABLE, strValue, _Module.GetResourceInstance() );
                }
                
                vUserName = strValue.c_str();
                pAVEvent->AddData (columnUserName, vUserName);

                // Computer Name
                //
                pAVEvent->AddData (columnComputerName, CAVCategory::g_vComputerName);

                // Product Version
                //
                _variant_t vProductVersion;
                if ( pNAVEvent->props.GetData(AV::Event_Error_propProductVersion, strValue ))
                    vProductVersion = strValue.c_str();

                pAVEvent->AddData (columnProductVersion, vProductVersion);

                // Feature name (uses Module ID)
                //
                _variant_t vFeatureName;
                long lModuleID = 0;
                pNAVEvent->props.GetData(AV::Event_Error_propModuleID, lModuleID );

                if ( !g_bRawMode )
                {
                    //
                    // Use the feature map!
                    //
					vFeatureName = m_Features.GetFeatureName(lModuleID);
                }
                else
                {
                    TCHAR szTemp [100] = {0};
                    _ltot ( lModuleID, szTemp, 10 );
                    vFeatureName = szTemp;
                }

                pAVEvent->AddData (columnFeature, vFeatureName);

                // Error code
                //
                long lErrorID = 0;
                _variant_t vErrorCode;

                pNAVEvent->props.GetData(AV::Event_Error_propErrorID, lErrorID );

                char szErrorCode [100] = {0};
                wsprintf(szErrorCode, 
                         _T("%03u%04u"),
                         lModuleID,
                         lErrorID);
                vErrorCode = szErrorCode;
                pAVEvent->AddData (columnErrorCode, vErrorCode);

                // Error message
                //
                _variant_t vMessage;
                if ( pNAVEvent->props.GetData(AV::Event_Error_propMessage, strValue ))
                    vMessage = strValue.c_str();

                pAVEvent->AddData (columnMessage, vMessage);

                // Details = message + link
                //
                ::std::string strDetails;
                CResourceHelper::LoadString ( IDS_HTML_HEADER, strDetails, _Module.GetResourceInstance() );
                strDetails += "<small>";

                std::string strErrorMessage;
                CHTMLFormat::HTMLEncode ( strValue.c_str(), TRUE, strErrorMessage );

                strDetails += strErrorMessage;
                strDetails += "</small>";

                // Generate the error link
                //
                TCHAR szURLBuffer [512] = {0};

                wsprintf ( szURLBuffer,
                           m_strErrorLink.c_str(),
                           m_TechSupport.GetURL ( lModuleID, lErrorID ),
                           szErrorCode );
                
                strDetails += CResourceHelper::LoadString ( IDS_HTML_BREAK, _Module.GetResourceInstance() );
                strDetails += szURLBuffer;
                strDetails += CResourceHelper::LoadString ( IDS_HTML_FOOTER, _Module.GetResourceInstance() );
                pAVEvent->SetDetailsHTML ( strDetails.c_str() );
                
                // Link will not be set as text for export/print
            }
            break;

            default:
                return FALSE;
                break;
        }
    }
    catch(...)
    {
        return FALSE;
    }
    
    return TRUE;
}
