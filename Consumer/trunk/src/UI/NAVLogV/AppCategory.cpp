////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

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

    CResourceHelper::LoadResourceString ( IDS_CATEGORY_APPLICATION, m_strCategoryName, _Module.GetResourceInstance() );
    m_iCategoryID = CC_NAV_CAT_APP_ACTIVITY;
    GetFactory(NAV_AV_EVENT_OBJECT_ID, &m_pFactory);
    
    // Fill the columns
    //
    m_vecColumnTitles.resize (columnLast);

    m_vecColumnTitles[columnDate] = CResourceHelper::LoadResourceStringW ( IDS_COLUMN_DATE, _Module.GetResourceInstance());
    m_vecColumnTitles[columnUserName] = CResourceHelper::LoadResourceStringW (IDS_COLUMN_USER_NAME, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnComputerName] = CResourceHelper::LoadResourceStringW (IDS_COLUMN_COMPUTER_NAME, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnFeature] = CResourceHelper::LoadResourceStringW (IDS_COLUMN_FEATURE, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnAction] = CResourceHelper::LoadResourceStringW (IDS_COLUMN_ACTION_TAKEN, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnDetails] = CResourceHelper::LoadResourceStringW (IDS_COLUMN_DETAILS, _Module.GetResourceInstance() );

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
    CAVEvent* pAVEvent = static_cast<CAVEvent*>(pLogViewerEvent);
    CNAVEventCommon* pNAVEvent = static_cast<CNAVEventCommon*>(pCEvent);

    long lType = 0;
    pNAVEvent->props.GetData(AV::Event_Base_propType, lType );

    switch ( lType )
    {
        case AV::Event_ID_ScanAction :
        {
            pAVEvent->lEventType = lType;
            tstring strValue;
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
                CResourceHelper::LoadResourceString ( IDS_NOT_APPLICABLE, strValue, _Module.GetResourceInstance() );
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
                vScanAction = CResourceHelper::LoadResourceString ( ScanAction.Names [lAction], _Module.GetResourceInstance() ).c_str();
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
            tstring strDetails;
            pNAVEvent->props.GetData(AV::Event_ScanAction_propTaskName, strDetails );
            vDetails = strDetails.c_str();
            pAVEvent->AddData (columnDetails, vDetails);
            
            // Set the bottom details to the details column. We will override this
            // later if we want to.
            //
            tstring strTempDetailsHTML;
            tstring strTempDetailsText;

            CResourceHelper::LoadResourceString ( IDS_HTML_HEADER, strTempDetailsHTML, _Module.GetResourceInstance() );

            // Bottom details
            //
            if ( lAction == AV::Event_ScanAction_ScanCompleted )
            {
                ::ATL::CString strTemp;
                
                // The " : " separator string
                //
                tstring strSeparator;
                tstring strNewlineHTML;
                CResourceHelper::LoadResourceString ( IDS_HTML_BREAK, strNewlineHTML, _Module.GetResourceInstance() );
                tstring strNewlineText;
                CResourceHelper::LoadResourceString ( IDS_TEXT_DELIMITER, strNewlineText, _Module.GetResourceInstance() );
                CResourceHelper::LoadResourceString (IDS_SCAN_SEPARATOR, strSeparator, _Module.GetResourceInstance());
                
                // Details title
                //
                strTempDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_SCAN_DETAILS, _Module.GetResourceInstance() );
                strTempDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_TABLE_START, _Module.GetResourceInstance() );
                
                strTempDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_SCAN_DETAILS, _Module.GetResourceInstance() );
                strTempDetailsText += strNewlineText;                    
                
                // Table begin
                strTempDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_TABLE_ENTRY_START, _Module.GetResourceInstance() );

                // Scan Time
                //
                lTempVal = 0;
                pNAVEvent->props.GetData(AV::Event_ScanAction_ScanTime, lTempVal);
                DWORD dwMins = lTempVal / 60;
                DWORD dwSecs = lTempVal % 60;
                strTemp.Format ( CResourceHelper::LoadResourceStringW ( IDS_SCAN_TIME, _Module.GetResourceInstance() ).c_str(),
                                    dwMins,
                                    dwSecs);
                strTempDetailsHTML += strTemp;
                strTempDetailsHTML += strNewlineHTML;

                strTempDetailsText += strTemp;
                strTempDetailsText += strNewlineText;

                

                
                // **** Files ****
                //
				// Total Files Scanned
                //


	            lTempVal = 0;
                pNAVEvent->props.GetData(AV::Event_ScanAction_ItemTotalScanned, lTempVal);
                strTemp.Format ( _T("%s%s%d"), CResourceHelper::LoadResourceStringW ( IDS_SCAN_ITEM_TOTAL_SCANNED, _Module.GetResourceInstance() ).c_str(),
                                            strSeparator.c_str(),
                                            lTempVal );
                strTempDetailsHTML += strTemp;
                strTempDetailsHTML += strNewlineHTML;

                strTempDetailsText += strTemp;
                strTempDetailsText += strNewlineText;

                // Virus detected //
                //
                lTempVal = 0;
                pNAVEvent->props.GetData(AV::Event_ScanAction_VirusesDetected, lTempVal);
                strTemp.Format ( _T("%s%s%d"), CResourceHelper::LoadResourceStringW ( IDS_SCAN_VIRUS_DETECTED, _Module.GetResourceInstance() ).c_str(),
                                            strSeparator.c_str(),
                                            lTempVal);
                strTempDetailsHTML += strTemp;
                strTempDetailsHTML += strNewlineHTML;

                strTempDetailsText += strTemp;
                strTempDetailsText += strNewlineText;

				//Virus removed
				//Event_ScanAction_VirusesRemoved
				//
				lTempVal = 0;
                pNAVEvent->props.GetData(AV::Event_ScanAction_VirusesRemoved, lTempVal);
                strTemp.Format ( _T("%s%s%d"), CResourceHelper::LoadResourceStringW ( IDS_SCAN_VIRUS_REMOVED, _Module.GetResourceInstance() ).c_str(),
                                            strSeparator.c_str(),
                                            lTempVal);
                strTempDetailsHTML += strTemp;
                strTempDetailsHTML += strNewlineHTML;

                strTempDetailsText += strTemp;
                strTempDetailsText += strNewlineText;
            

				//Event_ScanAction_NonVirusesDetected
				//
				lTempVal = 0;
                pNAVEvent->props.GetData(AV::Event_ScanAction_NonVirusesDetected, lTempVal);
                strTemp.Format ( _T("%s%s%d"), CResourceHelper::LoadResourceStringW ( IDS_SCAN_NONVIRUS_DETECTED, _Module.GetResourceInstance() ).c_str(),
                                            strSeparator.c_str(),
                                            lTempVal);
                strTempDetailsHTML += strTemp;
                strTempDetailsHTML += strNewlineHTML;

                strTempDetailsText += strTemp;
                strTempDetailsText += strNewlineText;

				//Event_ScanAction_NonVirusesRemoved
				//
				lTempVal = 0;
                pNAVEvent->props.GetData(AV::Event_ScanAction_NonVirusesRemoved, lTempVal);
                strTemp.Format ( _T("%s%s%d"), CResourceHelper::LoadResourceStringW ( IDS_SCAN_NONVIRUS_REMOVED, _Module.GetResourceInstance() ).c_str(),
                                            strSeparator.c_str(),
                                            lTempVal);
                strTempDetailsHTML += strTemp;
                strTempDetailsText += strTemp;

				//strTempDetailsHTML += strNewlineHTML;
                //strTempDetailsText += strNewlineText;


				/*Incident Number: 771146 
				aaron_pierce 2006/08/15 03:11:01 AM GMT
				Removed files deleted/excluded from stats.  This is not stat that is currently
				provided to users.*/

                // Files deleted
                //
                //lTempVal = 0;
                //pNAVEvent->props.GetData(AV::Event_ScanAction_FilesDeleted, lTempVal);
                //strTemp.Format ( _T("%s%s%d"), CResourceHelper::LoadResourceStringW ( IDS_SCAN_FILES_DELETED, _Module.GetResourceInstance() ).c_str(),
                //                            strSeparator.c_str(),
                //                            lTempVal);
                //strTempDetailsHTML += strTemp;
                //strTempDetailsHTML += strNewlineHTML;

                //strTempDetailsText += strTemp;
                //strTempDetailsText += strNewlineText;


                // Files excluded
                //
                //lTempVal = 0;
                //pNAVEvent->props.GetData(AV::Event_ScanAction_FilesExcluded, lTempVal);
                //strTemp.Format ( _T("%s%s%d"), CResourceHelper::LoadResourceStringW ( IDS_SCAN_FILES_EXCLUDED, _Module.GetResourceInstance() ).c_str(),
                //                            strSeparator.c_str(),
                //                            lTempVal);
                //strTempDetailsHTML += strTemp;
                //strTempDetailsText += strTemp;

                strTempDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_TABLE_ENTRY_STOP, _Module.GetResourceInstance() );
                // Last line, don't add a newline.
            }
            else if( lAction == AV::Event_ScanAction_MemorySideEffect )
            {
                // Get the display text passed in
                tstring strMemDetails;
                tstring strDisplay;
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

            strTempDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_TABLE_STOP, _Module.GetResourceInstance() );
            strTempDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_FOOTER, _Module.GetResourceInstance() );
            
            pAVEvent->SetDetailsHTML ( strTempDetailsHTML.c_str() );
            pAVEvent->SetDetailsText ( strTempDetailsText.c_str() );
        }
        break;

        default:
            return FALSE;
            break;
    }
    
    return TRUE;
}
