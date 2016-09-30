// AppCategory.cpp: implementation of the CAppCategory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppCategory.h"
#include "AVccLogViewerPluginId.h"
#include "..\navlogvres\resource.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAppCategory::CAppCategory()
{
	m_pFactory = NULL;
    m_dwHelpID = IDH_NAVW_CCLOGVIEW_APP_ACTIVITIES;

    CResourceHelper::LoadString ( IDS_CATEGORY_APPLICATION, m_strCategoryName, _Module.GetResourceInstance() );
    m_iCategoryID = CC_NAV_CAT_APP_ACTIVITY;
    GetFactory(NAV_AV_EVENT_OBJECT_ID, &m_pFactory);
    
    // Fill the columns
    //
    m_vecColumnTitles.resize (columnLast);

    m_vecColumnTitles[columnDate] = CResourceHelper::LoadString ( IDS_COLUMN_DATE, _Module.GetResourceInstance());
    m_vecColumnTitles[columnUserName] = CResourceHelper::LoadString (IDS_COLUMN_USER_NAME, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnComputerName] = CResourceHelper::LoadString (IDS_COLUMN_COMPUTER_NAME, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnFeature] = CResourceHelper::LoadString (IDS_COLUMN_FEATURE, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnAction] = CResourceHelper::LoadString (IDS_COLUMN_ACTION_TAKEN, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnDetails] = CResourceHelper::LoadString (IDS_COLUMN_DETAILS, _Module.GetResourceInstance() );

	CString csFormat;
	CString csBuffer;
	csFormat.LoadString(_Module.GetResourceInstance(), IDS_CATEGORY_DESC_APP);
	csBuffer.Format(csFormat, m_csProductName);
	m_strCategoryDescription = csBuffer;

    // Event types that we care about
    //
    m_nEventType = AV::Event_ID_ScanAction;
    m_nContextID = 0;
}

CAppCategory::~CAppCategory()
{

}

BOOL CAppCategory::SetEventProperties( ccEvtMgr::CEventEx* pCEvent,
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
            case AV::Event_ID_ScanAction :
            {
                pAVEvent->lEventType = lType;
                std::string strValue;
                ATL::CString strHelper;
                char szTempVal [100]= {0};
                long lBufSize = 0;
                long lTempVal = 0;
                long lAction = 0;

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
                _variant_t vUserName;
                strValue.clear ();
                if ( !pNAVEvent->props.GetData(AV::Event_Base_propUserName, strValue ) || strValue.empty())
                {
                    CResourceHelper::LoadString ( IDS_NOT_APPLICABLE, strValue, _Module.GetResourceInstance() );
                }

                vUserName = strValue.c_str();
                pAVEvent->AddData (columnUserName, vUserName);

                // Computer Name
                //
                pAVEvent->AddData (columnComputerName, g_vComputerName);

                // Feature name 
                //
                _variant_t vFeatureName;
                pNAVEvent->props.GetData(AV::Event_ScanAction_propFeature, lTempVal );

                if ( !g_bRawMode )
                {
                    //
                    // Use the feature map!
                    //
					vFeatureName = m_Features.GetFeatureName(lTempVal);

                }
                else
                {
                    TCHAR szTemp [100] = {0};
                    _ltot ( lTempVal, szTemp, 10 );
                    vFeatureName = szTemp;
                }

                pAVEvent->AddData (columnFeature, vFeatureName);

                // Action
                //
                _variant_t vScanAction;
                pNAVEvent->props.GetData(AV::Event_ScanAction_propAction, lAction );

                if ( !g_bRawMode )
                {
                    CAVScanActionMap ScanAction;
                    vScanAction = CResourceHelper::LoadString ( ScanAction.Names [lAction], _Module.GetResourceInstance() ).c_str();
                }
                else
                {
                    TCHAR szTemp [100] = {0};
                    _ltot ( lAction, szTemp, 10 );
                    vScanAction = szTemp;
                }

                pAVEvent->AddData (columnAction, vScanAction);

                // Details column
                //
                _variant_t vDetails;
                std::string strDetails;
                pNAVEvent->props.GetData(AV::Event_ScanAction_propTaskName, strDetails );
                vDetails = strDetails.c_str();
                pAVEvent->AddData (columnDetails, vDetails);
                
                // Set the bottom details to the details column. We will override this
                // later if we want to.
                //
                ::std::string strTempDetailsHTML;
                ::std::string strTempDetailsText;

                CResourceHelper::LoadString ( IDS_HTML_HEADER, strTempDetailsHTML, _Module.GetResourceInstance() );

                // Bottom details
                //
                if ( lAction == AV::Event_ScanAction_ScanCompleted )
                {
                    ::ATL::CString strTemp;
                    
                    // The " : " separator string
                    //
                    ::std::string strSeparator;
                    ::std::string strNewlineHTML;
                    CResourceHelper::LoadString ( IDS_HTML_BREAK, strNewlineHTML, _Module.GetResourceInstance() );
                    ::std::string strNewlineText;
                    CResourceHelper::LoadString ( IDS_TEXT_DELIMITER, strNewlineText, _Module.GetResourceInstance() );
                    CResourceHelper::LoadString (IDS_SCAN_SEPARATOR, strSeparator, _Module.GetResourceInstance());
                    
                    // Details title
                    //
                    strTempDetailsHTML += CResourceHelper::LoadString ( IDS_HTML_SCAN_DETAILS, _Module.GetResourceInstance() );
                    strTempDetailsHTML += CResourceHelper::LoadString ( IDS_HTML_TABLE_START, _Module.GetResourceInstance() );
                    
                    strTempDetailsText += CResourceHelper::LoadString ( IDS_TEXT_SCAN_DETAILS, _Module.GetResourceInstance() );
                    strTempDetailsText += strNewlineText;                    
                    
                    // Table begin
                    strTempDetailsHTML += CResourceHelper::LoadString ( IDS_HTML_TABLE_ENTRY_START, _Module.GetResourceInstance() );

                    // Scan Time
                    //
                    lTempVal = 0;
                    pNAVEvent->props.GetData(AV::Event_ScanAction_ScanTime, lTempVal);
                    DWORD dwMins = lTempVal / 60;
                    DWORD dwSecs = lTempVal % 60;
                    strTemp.Format ( CResourceHelper::LoadString ( IDS_SCAN_TIME, _Module.GetResourceInstance() ).c_str(),
                                     dwMins,
                                     dwSecs);
                    strTempDetailsHTML += strTemp;
                    strTempDetailsHTML += strNewlineHTML;

                    strTempDetailsText += strTemp;
                    strTempDetailsText += strNewlineText;

                    // **** MBR ****
                    //
					// Total MBRs Scanned
                    //
					lTempVal = 0;
                    pNAVEvent->props.GetData(AV::Event_ScanAction_MBRsTotalScanned, lTempVal);
                    strTemp.Format ( "%s%s%d",
                                     CResourceHelper::LoadString ( IDS_SCAN_MBR_SCANNED, _Module.GetResourceInstance() ).c_str(),
                                     strSeparator.c_str(),
                                     lTempVal );
                    
                    strTempDetailsHTML += strTemp;
                    strTempDetailsHTML += strNewlineHTML;

                    strTempDetailsText += strTemp;
                    strTempDetailsText += strNewlineText;

                    // MBR infected
                    //
					lTempVal = 0;
                    pNAVEvent->props.GetData(AV::Event_ScanAction_MBRsTotalInfected, lTempVal);
                    strTemp.Format ( "%s%s%d", CResourceHelper::LoadString ( IDS_SCAN_MBR_INFECTED, _Module.GetResourceInstance() ).c_str(),
                                               strSeparator.c_str(),
                                               lTempVal);
                    strTempDetailsHTML += strTemp;
                    strTempDetailsHTML += strNewlineHTML;

                    strTempDetailsText += strTemp;
                    strTempDetailsText += strNewlineText;


                    // MBR repaired
                    //
					lTempVal = 0;
                    pNAVEvent->props.GetData(AV::Event_ScanAction_MBRsRepaired, lTempVal);
                    strTemp.Format ( "%s%s%d", CResourceHelper::LoadString ( IDS_SCAN_MBR_REPAIRED, _Module.GetResourceInstance() ).c_str(),
                                               strSeparator.c_str(),
                                               lTempVal);
                    strTempDetailsHTML += strTemp;
                    strTempDetailsHTML += strNewlineHTML;

                    strTempDetailsText += strTemp;
                    strTempDetailsText += strNewlineText;

                    // **** BR ****
                    //
					// Total BRs Scanned
                    //
                    lTempVal = 0;
                    pNAVEvent->props.GetData(AV::Event_ScanAction_BootRecsTotalScanned, lTempVal);
                    strTemp.Format ( "%s%s%d", CResourceHelper::LoadString ( IDS_SCAN_BR_SCANNED, _Module.GetResourceInstance() ).c_str(),
                                               strSeparator.c_str(),
                                               lTempVal);
                    strTempDetailsHTML += strTemp;
                    strTempDetailsHTML += strNewlineHTML;

                    strTempDetailsText += strTemp;
                    strTempDetailsText += strNewlineText;

                    // BR infected
                    //
                    lTempVal = 0;
                    pNAVEvent->props.GetData(AV::Event_ScanAction_BootRecsTotalInfected, lTempVal);
                    strTemp.Format ( "%s%s%d", CResourceHelper::LoadString ( IDS_SCAN_BR_INFECTED, _Module.GetResourceInstance() ).c_str(),
                                               strSeparator.c_str(),
                                               lTempVal);
                    strTempDetailsHTML += strTemp;
                    strTempDetailsHTML += strNewlineHTML;

                    strTempDetailsText += strTemp;
                    strTempDetailsText += strNewlineText;


                    // BR repaired
                    //
                    lTempVal = 0;
                    pNAVEvent->props.GetData(AV::Event_ScanAction_BootRecsRepaired, lTempVal);
                    strTemp.Format ( "%s%s%d", CResourceHelper::LoadString ( IDS_SCAN_BR_REPAIRED, _Module.GetResourceInstance() ).c_str(),
                                               strSeparator.c_str(),
                                               lTempVal);
                    strTempDetailsHTML += strTemp;
                    strTempDetailsHTML += strNewlineHTML;

                    strTempDetailsText += strTemp;
                    strTempDetailsText += strNewlineText;

                    // **** Files ****
                    //
					// Total Files Scanned
                    //
                    lTempVal = 0;
                    pNAVEvent->props.GetData(AV::Event_ScanAction_FilesTotalScanned, lTempVal);
                    strTemp.Format ( "%s%s%d", CResourceHelper::LoadString ( IDS_SCAN_FILES_SCANNED, _Module.GetResourceInstance() ).c_str(),
                                               strSeparator.c_str(),
                                               lTempVal );
                    strTempDetailsHTML += strTemp;
                    strTempDetailsHTML += strNewlineHTML;

                    strTempDetailsText += strTemp;
                    strTempDetailsText += strNewlineText;

                    // Files infected
                    //
                    lTempVal = 0;
                    pNAVEvent->props.GetData(AV::Event_ScanAction_FilesTotalInfected, lTempVal);
                    strTemp.Format ( "%s%s%d", CResourceHelper::LoadString ( IDS_SCAN_FILES_INFECTED, _Module.GetResourceInstance() ).c_str(),
                                               strSeparator.c_str(),
                                               lTempVal);
                    strTempDetailsHTML += strTemp;
                    strTempDetailsHTML += strNewlineHTML;

                    strTempDetailsText += strTemp;
                    strTempDetailsText += strNewlineText;


                    // Files repaired
                    //
                    lTempVal = 0;
                    pNAVEvent->props.GetData(AV::Event_ScanAction_FilesRepaired, lTempVal);
                    strTemp.Format ( "%s%s%d", CResourceHelper::LoadString ( IDS_SCAN_FILES_REPAIRED, _Module.GetResourceInstance() ).c_str(),
                                               strSeparator.c_str(),
                                               lTempVal);
                    strTempDetailsHTML += strTemp;
                    strTempDetailsHTML += strNewlineHTML;

                    strTempDetailsText += strTemp;
                    strTempDetailsText += strNewlineText;

                    // Files quarantined
                    //
                    lTempVal = 0;
                    pNAVEvent->props.GetData(AV::Event_ScanAction_FilesQuarantined, lTempVal);
                    strTemp.Format ( "%s%s%d", CResourceHelper::LoadString ( IDS_SCAN_FILES_QUARANTINED, _Module.GetResourceInstance() ).c_str(),
                                               strSeparator.c_str(),
                                               lTempVal);
                    strTempDetailsHTML += strTemp;
                    strTempDetailsHTML += strNewlineHTML;

                    strTempDetailsText += strTemp;
                    strTempDetailsText += strNewlineText;
                
                    // Files deleted
                    //
                    lTempVal = 0;
                    pNAVEvent->props.GetData(AV::Event_ScanAction_FilesDeleted, lTempVal);
                    strTemp.Format ( "%s%s%d", CResourceHelper::LoadString ( IDS_SCAN_FILES_DELETED, _Module.GetResourceInstance() ).c_str(),
                                               strSeparator.c_str(),
                                               lTempVal);
                    strTempDetailsHTML += strTemp;
                    strTempDetailsHTML += strNewlineHTML;

                    strTempDetailsText += strTemp;
                    strTempDetailsText += strNewlineText;

                    // Files excluded
                    //
                    lTempVal = 0;
                    pNAVEvent->props.GetData(AV::Event_ScanAction_FilesExcluded, lTempVal);
                    strTemp.Format ( "%s%s%d", CResourceHelper::LoadString ( IDS_SCAN_FILES_EXCLUDED, _Module.GetResourceInstance() ).c_str(),
                                               strSeparator.c_str(),
                                               lTempVal);
                    strTempDetailsHTML += strTemp;
                    strTempDetailsText += strTemp;

                    strTempDetailsHTML += CResourceHelper::LoadString ( IDS_HTML_TABLE_ENTRY_STOP, _Module.GetResourceInstance() );
                    // Last line, don't add a newline.
                }
                else if( lAction == AV::Event_ScanAction_MemorySideEffect )
                {
                    // Get the display text passed in
                    tstring strMemDetails;
                    std::string strDisplay;
                    pNAVEvent->props.GetData(AV::Event_ScanAction_Display, strDisplay );
                    CHTMLFormat::HTMLEncode ( strDisplay.c_str(), TRUE, strMemDetails );
                    
                    strTempDetailsHTML += strMemDetails;
                    strTempDetailsText += strDisplay.c_str();
                }
                else
                {
                    // strDetails == The details column text
                    //
                    tstring strTempTaskName;
                    CHTMLFormat::HTMLEncode ( strDetails.c_str(), TRUE, strTempTaskName );

                    strTempDetailsHTML += strTempTaskName;
                    strTempDetailsText += strDetails.c_str();
                }

                strTempDetailsHTML += CResourceHelper::LoadString ( IDS_HTML_TABLE_STOP, _Module.GetResourceInstance() );
                strTempDetailsHTML += CResourceHelper::LoadString ( IDS_HTML_FOOTER, _Module.GetResourceInstance() );
                
                pAVEvent->SetDetailsHTML ( strTempDetailsHTML.c_str() );
                pAVEvent->SetDetailsText ( strTempDetailsText.c_str() );
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
