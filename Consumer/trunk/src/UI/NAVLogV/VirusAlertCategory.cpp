////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// VirusAlertCategory.cpp: implementation of the CVirusAlertCategory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VirusAlertCategory.h"
#include "ThreatCatInfo.h"     // Toolbox
#include "NAVUIHTM_Resource.h"
#include "ccLogViewerInterface.h"
#include "AVccModuleId.h"
#include "AVccLogViewerPluginId.h"
#include "..\navlogvres\resource.h"

#include <AvInterfaceLoader.h>
#include <ccEraserInterface.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVirusAlertCategory::CVirusAlertCategory()
{
	m_pFactory = NULL;
    m_dwHelpID = IDH_NAVW_CCLOGVIEW_SECURITY_RISKS;

    CResourceHelper::LoadResourceString ( IDS_CATEGORY_NAME_VIRUS_ALERT, m_strCategoryName, _Module.GetResourceInstance() );

    m_iCategoryID = CC_NAV_CAT_VIRUS_ALERT;
    
    GetFactory(NAV_AV_EVENT_OBJECT_ID, &m_pFactory);
    
    // Fill the columns
    //
    m_vecColumnTitles.resize (columnLast);
    
    m_vecColumnTitles[columnDate] = CResourceHelper::LoadResourceStringW ( IDS_COLUMN_DATE, _Module.GetResourceInstance());
    m_vecColumnTitles[columnUserName] = CResourceHelper::LoadResourceStringW (IDS_COLUMN_USER_NAME, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnComputerName] = CResourceHelper::LoadResourceStringW (IDS_COLUMN_COMPUTER_NAME, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnVirusName] = CResourceHelper::LoadResourceStringW (IDS_COLUMN_VIRUS_NAME, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnItemType] = CResourceHelper::LoadResourceStringW (IDS_COLUMN_ITEM_TYPE, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnResult] = CResourceHelper::LoadResourceStringW (IDS_COLUMN_ACTION, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnFeature] = CResourceHelper::LoadResourceStringW (IDS_COLUMN_FEATURE, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnDefsRevision] = CResourceHelper::LoadResourceStringW (IDS_COLUMN_DEFS_REVISION, _Module.GetResourceInstance() );
    m_vecColumnTitles[columnProductVersion] = CResourceHelper::LoadResourceStringW (IDS_COLUMN_PRODUCT_VERSION, _Module.GetResourceInstance() );

    m_strCategoryDescription = CResourceHelper::LoadResourceStringW (IDS_CATEGORY_DESC_VIRUS_ALERT, _Module.GetResourceInstance() );

    // Event types that we care about
    //
    m_nEventType = AV::Event_ID_Threat;
    m_nContextID = 0;
}

CVirusAlertCategory::~CVirusAlertCategory()
{
}

BOOL CVirusAlertCategory::processLogEvents( ccEvtMgr::CEventEx* pCEvent,
                                            const ccEvtMgr::CLogFactoryEx::EventInfo& Info)
{
    CNAVEventCommon* pNAVEvent = static_cast<CNAVEventCommon*>(pCEvent);

    if (!m_pFactory)
        return FALSE;

    // Is this a supported type?
    //
    long lType = 0;
    pNAVEvent->props.GetData(AV::Event_Base_propType, lType );
    if ( lType != AV::Event_ID_Threat )
        return FALSE;

    // Get the sub type
    long lSubType = 0;

    pNAVEvent->props.GetData ( AV::Event_Threat_propEventSubType, lSubType );

    CEventData eventdataAllFiles;
    if (!pNAVEvent->props.GetNode ( AV::Event_Threat_propSubFileData, eventdataAllFiles ))
    {
        CCTRACEE (_T("CVirusAlertCategory::processLogEvents - no files!"));
        return FALSE;
    }

    long lNumSubFiles = eventdataAllFiles.GetCount ();

    // ***********************************
    // Get common info
    //
    // Date
    //
    DATE dtDate;
    _variant_t vTimeDate;
    _variant_t vUserName;
    tstring strUserName;
    _variant_t vProductVersion;
    _variant_t vDefsRevision;
    _variant_t vNotApplicable;
    _variant_t vFeatureName;
    _variant_t vActionAttempted;
    _variant_t vItemType;
	tstring strApplicationPath;
    tstring strAppPathHTML;
    long lAttemptedAction = 0;
    tstring strObject;
    tstring strMethod;

    // strHelpers
    ATL::CString strHelper;
    CAVActionTakenMap ActionTaken;
    CThreatCatInfo ThreatInfo;

    
    // Scope Commmon data work
    {
        // Date
        //
        SYSTEMTIME time;
        CopyMemory ( &time, &Info.m_TimeStamp, sizeof (SYSTEMTIME));
        SystemTimeToVariantTime(&time, &dtDate);

        // Log is in UTC System time.
        //
        V_VT(&vTimeDate) = VT_DATE;
        V_DATE(&vTimeDate) = dtDate;

        // User Name
        //
        if ( !pNAVEvent->props.GetData(AV::Event_Base_propUserName, strUserName ) || strUserName.empty())
			CResourceHelper::LoadResourceString ( IDS_NOT_APPLICABLE, strUserName, _Module.GetResourceInstance() );

        vUserName = strUserName.c_str();

        vNotApplicable = CResourceHelper::LoadResourceStringW ( IDS_NOT_APPLICABLE, _Module.GetResourceInstance() ).c_str();

        // Product Version
        //
        std::string strProdVersion;
        if ( pNAVEvent->props.GetData(AV::Event_Threat_propProductVersion, strProdVersion ))
            vProductVersion = strProdVersion.c_str();

        // Defs Revision - YYYYMMDDRRRR
        //
        std::string strDefRev;
        if ( pNAVEvent->props.GetData(AV::Event_Threat_propDefsRevision, strDefRev ))
            vDefsRevision = strDefRev.c_str();

        // Feature name
        //
        long lFeature = 0;
        pNAVEvent->props.GetData(AV::Event_Threat_propFeature, lFeature );

        if ( !g_bRawMode )
        {
            // Use the feature map!
            //
			vFeatureName = m_Features.GetFeatureName(lFeature);
        }
        else
        {
            TCHAR szTemp [100] = {0};
            _ltot ( lFeature, szTemp, 10 );
            vFeatureName = szTemp;
        }

        // Item type
        //
        long lItemType = 0;
        pNAVEvent->props.GetData(AV::Event_Threat_propObjectType, lItemType );
        if ( !g_bRawMode )
        {
            switch (lItemType)
            {
            case AV::Event_Threat_ObjectType_File :
                vItemType = CResourceHelper::LoadResourceStringW ( IDS_ITEM_TYPE_FILE, _Module.GetResourceInstance() ).c_str();
                break;
            case AV::Event_Threat_ObjectType_BootRecord :
                vItemType = CResourceHelper::LoadResourceStringW ( IDS_ITEM_TYPE_BR, _Module.GetResourceInstance() ).c_str();
                break;
            case AV::Event_Threat_ObjectType_MasterBootRecord :
                vItemType = CResourceHelper::LoadResourceStringW ( IDS_ITEM_TYPE_MBR, _Module.GetResourceInstance() ).c_str();
                break;
            case AV::Event_Threat_ObjectType_Memory :
                vItemType = CResourceHelper::LoadResourceStringW ( IDS_ITEM_TYPE_MEMORY, _Module.GetResourceInstance() ).c_str();
                break;
            default:
                vItemType = CResourceHelper::LoadResourceStringW ( IDS_ITEM_TYPE_UNKNOWN, _Module.GetResourceInstance() ).c_str();
                break;
            }
        }
        else
        {
            TCHAR szTemp [100] = {0};
            _ltot ( lItemType, szTemp, 10 );
            vItemType = szTemp;
        }
    }

    // Get a list of all remediation data
    CEventData eventAllRemediationData;
    if(pNAVEvent->props.GetNode(AV::Event_Threat_propRemediationData, eventAllRemediationData))
    {
        // enum the list, and retrieve each individual remediation action data
        long lRemediationCount = eventAllRemediationData.GetCount();
        for(long lRemediationIndex = 0; lRemediationIndex < lRemediationCount; lRemediationIndex++)
        {
            CEventData eventCurrentRemediationData;
            if(!eventAllRemediationData.GetNode(lRemediationIndex, eventCurrentRemediationData))
            {
                CCTRACEE(_T("Unable to retrieve remediation data at index %d"), lRemediationIndex);
                continue;
            }

            // determine the type of the remediation action
            LONG lRemediationType = NULL;
            if(eventCurrentRemediationData.GetData(AVModule::ThreatTracking::RemDisp_RemediationType, lRemediationType))
            {
                // use the enum'd values from ccEraserInterface.h for comparison
                if(ccEraser::FileRemediationActionType == lRemediationType)
                {
                    // found a file remediation
                    tstring strFileName;
                    eventCurrentRemediationData.GetData(AVModule::ThreatTracking::RemDisp_Target, strFileName);
                    
                    CCTRACEI(_T("Found remediation info for file: %s"), strFileName.c_str());
                }
            }
        }
    }


    // 
    // END COMMON DATA
    // **********************************************

    // Loop through the sub files and make one event for each file/VID/action
    //
    long lSubFile = 0;

    // If this is a container scan, skip the first file. We don't use
    // the parent data in the logger.
    // 
    if ( AV::Event_Threat_Container == lSubType )
        lSubFile = 1;
    
    for (; lSubFile < lNumSubFiles; lSubFile++)
    {

        CEventData eventdataSubFile;
        if ( !eventdataAllFiles.GetNode ( lSubFile, eventdataSubFile ))
        {
            CCTRACEE (_T("CVirusAlertCategory::processLogEvents - error getting subfile"));
            continue;
        }

        tstring strFilePath;
        tstring strFilePathHTML;
        tstring strFileDesc;    // for containers
        tstring strFileDescHTML;
        _variant_t vFilePath;

        // File path
        //
        if (eventdataSubFile.GetData(AV::Event_ThreatSubFile_propFileName, strFilePath ))
        {                        
            CHTMLFormat::HTMLEncode ( strFilePath.c_str(), TRUE, strFilePathHTML );
        }

        // File description (for compressed files, etc.)
        //
        if (eventdataSubFile.GetData(AV::Event_ThreatSubFile_propFileDescription, strFileDesc ))
        {
            CHTMLFormat::HTMLEncode ( strFileDesc.c_str(), TRUE, strFileDescHTML );
        }

        // Loop for each Threat Entry
        //

        CEventData eventAllThreats;
        if ( eventdataSubFile.GetNode ( AV::Event_ThreatSubFile_propThreatData, eventAllThreats ))
        {
            long lThreatCount = eventAllThreats.GetCount();

            for ( long lThreatIndex = 0; lThreatIndex < lThreatCount; lThreatIndex++)
            {
                CEventData eventThreat;

                if ( eventAllThreats.GetNode ( lThreatIndex, eventThreat ))
                {
                    long lActionCount = 0;

                    eventThreat.GetData ( AV::Event_ThreatEntry_propActionCount, lActionCount );

                    // *********************************
                    // Threat entry common data
                    //
                    _variant_t vVirusName;
                    long lItemType = 0;
                    TCHAR szCategoryList [512] = {0};
                    tstring strThreatCatsHTML;
				    TCHAR szVirusIDLink [1024] = {0};
                    tstring strVirusNameHTML;
                    tstring strVirusName;
                    // Virus name
                    //
                    if ( eventThreat.GetData(AV::Event_ThreatEntry_propVirusName, strVirusName ))
                    {
                        vVirusName = strVirusName.c_str();
					    CHTMLFormat::HTMLEncode ( strVirusName.c_str(), TRUE, strVirusNameHTML );
                    }

                    // Build the threat cat string
                    //
					tstring strThreatCats;
                    if ( eventThreat.GetData(AV::Event_ThreatEntry_propThreatCategories, strThreatCats ))
                    {
                        // Get the list of threats
                        //
                        if ( g_bRawMode )
                            _tcsncpy ( szCategoryList, strThreatCats.c_str(), 512);
                        else
                            ThreatInfo.GetCategoryText ( strThreatCats.c_str(), szCategoryList, 512 );

                        CHTMLFormat::HTMLEncode ( szCategoryList, TRUE, strThreatCatsHTML );
                    }

                    // Virus ID link
                    //
                    // VID is actually an unsigned long which the CEventData class does not support
                    // natively, but some casting should be OK here to get back to the correct unsigned value
                    long lVirusID = 0;
                    if ( eventThreat.GetData(AV::Event_ThreatEntry_propVirusID, lVirusID ) )
				    {
                        unsigned long ulVirusID = static_cast<unsigned long>(lVirusID);
					    TCHAR szBufferFormat [1024] = {0};
                        tstring strSARC;
                        CResourceHelper::LoadResourceString ( IDS_SARC_WEBSITE, strSARC, _Module.GetResourceInstance() );

					    // Set up string "Click for more information about : <a href="http://www.espn.com/vid=%d">%s</a></p>
                        swprintf ( szBufferFormat, strSARC.c_str(), m_csThreatInfoURL, strVirusNameHTML.c_str() );

					    // Replace %lu with virus id
					    swprintf ( szVirusIDLink, szBufferFormat, ulVirusID);
					    CCTRACEI(_T("CVirusAlertCategory::SetEventProperties ThreatInfoLink=%s"), szVirusIDLink);

					    // Click for more information about : <a href="http://www.espn.com/">NAVTest.Virus</a></p>
                    }
                    //
                    // End Threat Entry common data
                    // ********************************

                    // Finally, build each log entry
                    //
                    // We have to make more than one log event per NAV event
                    //
                    cc::ILogViewerEvent* pLogViewerEvent;
                    HRESULT hRes = m_pFactory->CreateInstance(cc::IID_LogViewerEvent, 
                                                            (void**)&pLogViewerEvent);
                    
                    ASSERT(SYM_SUCCEEDED(hRes));

                    CAVEvent* pAVEvent = static_cast<CAVEvent*>(pLogViewerEvent);

                    // Save common data
                    //
                    pAVEvent->AddData ( columnDate, vTimeDate, LV_DATE_FORMAT_UTC);
                    pAVEvent->AddData ( columnUserName, vUserName );
                    pAVEvent->AddData ( columnComputerName, CAVCategory::g_vComputerName );
                    pAVEvent->AddData ( columnFeature, vFeatureName );
                    pAVEvent->AddData ( columnItemType, vItemType );
                    pAVEvent->AddData ( columnVirusName, vVirusName );
                    pAVEvent->AddData (columnProductVersion, vProductVersion);


                    // Prep details text
                    tstring strDetailsHTML;
                    tstring strDetailsText;
                    strDetailsHTML = CResourceHelper::LoadResourceStringW ( IDS_HTML_HEADER, _Module.GetResourceInstance() );
                    strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_SOURCE, _Module.GetResourceInstance() );
                    strDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_SOURCE, _Module.GetResourceInstance() );

                    // Get Actions array
                    ::std::vector<BYTE> vecActions;
                    vecActions.resize (lActionCount);
                    long lSize = lActionCount;
					tstring strActionsHTML;
                    tstring strActionsText;
					tstring strFinalAction;

                    eventThreat.GetData ( AV::Event_ThreatEntry_propActionData, &vecActions[0], lSize );

                    for ( long lActionIndex = 0; lActionIndex < lActionCount; lActionIndex++)
                    {
                        // Action name
                        //
                        long lAction = vecActions[lActionIndex];

                        if ( g_bRawMode )
                        {                                    
                            TCHAR szTemp [100] = {0};
                            _ltot ( lAction, szTemp, 10 );
                            strFinalAction = szTemp;
                        }
                        else
                        {
                            // Use the ActionTaken map!
                            //
                            strFinalAction = CResourceHelper::LoadResourceStringW (ActionTaken.Names [lAction], _Module.GetResourceInstance());
                        }                               

                        strActionsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_BREAK, _Module.GetResourceInstance() );
                        strActionsHTML += CResourceHelper::LoadResourceStringW ( IDS_ACTION_HTML, _Module.GetResourceInstance() );
                        strActionsHTML += strFinalAction;

                        strActionsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_DELIMITER, _Module.GetResourceInstance() );
                        strActionsText += CResourceHelper::LoadResourceStringW ( IDS_ACTION_TEXT, _Module.GetResourceInstance() );
                        strActionsText += strFinalAction;
                    } // end action

                    // *** Columns
                    pAVEvent->AddData (columnDefsRevision, vDefsRevision );                                

                    //*** Build details
                    //
                    strDetailsText += strFilePath;
                    strDetailsHTML += strFilePathHTML;

                    // Threat categories
                    //
                    if ( !strThreatCats.empty())
                    {
                        strDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_DELIMITER, _Module.GetResourceInstance() );
                        strDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_THREAT_CAT, _Module.GetResourceInstance() );
                        strDetailsText += szCategoryList;

                        strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_BREAK, _Module.GetResourceInstance() );
                        strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_THREAT_CAT, _Module.GetResourceInstance() );
                        strDetailsHTML += strThreatCatsHTML;
                    }

                    // Threat matrix values
                    tstring strThreatMatrixHTML;
					tstring strThreatMatrix;
                    tstring strTemp;

                    // Only display if we have a threat matrix overall value
                    if( eventThreat.GetData(AV::Event_ThreatEntry_propThreatMatrixOverall, strThreatMatrix ) )
                    {
						if( _tcscmp( strThreatMatrix.c_str(),L"0")==0) //changed to 0 to make it consistant with line 876 in message.cpp
							strThreatMatrix = CResourceHelper::LoadResourceStringW ( IDS_TEXT_LOW, _Module.GetResourceInstance() );

						else if( _tcscmp( strThreatMatrix.c_str(),L"1")==0)
							strThreatMatrix = CResourceHelper::LoadResourceStringW ( IDS_TEXT_MEDIUM, _Module.GetResourceInstance() );

						else if( _tcscmp( strThreatMatrix.c_str(),L"2")==0)
							strThreatMatrix = CResourceHelper::LoadResourceStringW ( IDS_TEXT_HIGH, _Module.GetResourceInstance() );
                        else
							strThreatMatrix = CResourceHelper::LoadResourceStringW ( IDS_ITEM_TYPE_UNKNOWN, _Module.GetResourceInstance() );


                        // Overall values for the text version
                        strDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_DELIMITER, _Module.GetResourceInstance() );
                        strDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_THREAT_MATRIX_OVERALL, _Module.GetResourceInstance() );

                        strDetailsText += strThreatMatrix;

                        // Convert the overall text to HTML format
                        CHTMLFormat::HTMLEncode ( strThreatMatrix.c_str(), TRUE, strThreatMatrixHTML );

                        // Load overall header for the HTML version
                        strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_BREAK, _Module.GetResourceInstance() );
                        strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_THREAT_MATRIX_OVERALL, _Module.GetResourceInstance() );
                        strDetailsHTML += strThreatMatrixHTML;

                        // Performance
                        if( eventThreat.GetData(AV::Event_ThreatEntry_propThreatMatrixPerformance, strThreatMatrix ) )
                        {
                            strDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_DELIMITER, _Module.GetResourceInstance() );
                            strDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_THREAT_MATRIX_PERFORMANCE, _Module.GetResourceInstance() );
                            strDetailsText += strThreatMatrix;

                            // Convert for HTML
                            CHTMLFormat::HTMLEncode ( strThreatMatrix.c_str(), TRUE, strThreatMatrixHTML );

                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_BREAK, _Module.GetResourceInstance() );
                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_MARGINLEFT_SPAN_BEGIN, _Module.GetResourceInstance() );
                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_TEXT_THREAT_MATRIX_PERFORMANCE, _Module.GetResourceInstance() );
                            strDetailsHTML += strThreatMatrixHTML;
                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_SPAN_END, _Module.GetResourceInstance() );
                        }
                        
                        // Privacy
                        if( eventThreat.GetData(AV::Event_ThreatEntry_propThreatMatrixPrivacy, strThreatMatrix ) )
                        {
                            strDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_DELIMITER, _Module.GetResourceInstance() );
                            strDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_THREAT_MATRIX_PRIVACY, _Module.GetResourceInstance() );
                            strDetailsText += strThreatMatrix;

                            // Convert for HTML
                            CHTMLFormat::HTMLEncode ( strThreatMatrix.c_str(), TRUE, strThreatMatrixHTML );

                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_BREAK, _Module.GetResourceInstance() );
                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_MARGINLEFT_SPAN_BEGIN, _Module.GetResourceInstance() );
                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_TEXT_THREAT_MATRIX_PRIVACY, _Module.GetResourceInstance() );
                            strDetailsHTML += strThreatMatrixHTML;
                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_SPAN_END, _Module.GetResourceInstance() );
                        }

                        // Removal
                        if( eventThreat.GetData(AV::Event_ThreatEntry_propThreatMatrixRemoval, strThreatMatrix ) )
                        {
                            strDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_DELIMITER, _Module.GetResourceInstance() );
                            strDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_THREAT_MATRIX_REMOVAL, _Module.GetResourceInstance() );
                            strDetailsText += strThreatMatrix;

                            // Convert for HTML
                            CHTMLFormat::HTMLEncode ( strThreatMatrix.c_str(), TRUE, strThreatMatrixHTML );

                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_BREAK, _Module.GetResourceInstance() );
                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_MARGINLEFT_SPAN_BEGIN, _Module.GetResourceInstance() );
                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_TEXT_THREAT_MATRIX_REMOVAL, _Module.GetResourceInstance() );
                            strDetailsHTML += strThreatMatrixHTML;
                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_SPAN_END, _Module.GetResourceInstance() );
                        }

                        // Stealth
                        if( eventThreat.GetData(AV::Event_ThreatEntry_propThreatMatrixStealth, strThreatMatrix ) )
                        {
                            strDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_DELIMITER, _Module.GetResourceInstance() );
                            strDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_THREAT_MATRIX_STEALTH, _Module.GetResourceInstance() );
                            strDetailsText += strThreatMatrix;

                            // Convert for HTML
                            CHTMLFormat::HTMLEncode ( strThreatMatrix.c_str(), TRUE, strThreatMatrixHTML );

                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_BREAK, _Module.GetResourceInstance() );
                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_MARGINLEFT_SPAN_BEGIN, _Module.GetResourceInstance() );
                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_TEXT_THREAT_MATRIX_STEALTH, _Module.GetResourceInstance() );
                            strDetailsHTML += strThreatMatrixHTML;
                            strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_SPAN_END, _Module.GetResourceInstance() );
                        }
                    }

                    // VID link
                    if ( _T('\0') != szVirusIDLink[0] )
                    {
					    // Format in HTML for link
					    //
					    strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_BREAK, _Module.GetResourceInstance() );
					    strDetailsHTML += szVirusIDLink;
                    }

                    // Actions
                    strDetailsHTML += strActionsHTML;
                    strDetailsText += strActionsText;

                    // Compressed files - Container name
                    if ( !strFileDesc.empty())
                    {
                        strDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_DELIMITER, _Module.GetResourceInstance() );
                        strDetailsText += CResourceHelper::LoadResourceStringW ( IDS_TEXT_DESCRIPTION, _Module.GetResourceInstance() );
                        strDetailsText += strFileDesc;
                        
                        strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_BREAK, _Module.GetResourceInstance() );
                        strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_DESCRIPTION, _Module.GetResourceInstance() );
                        strDetailsHTML += strFileDescHTML;
                    }

                    _variant_t vActionName;
                    vActionName = strFinalAction.c_str();
                    pAVEvent->AddData ( columnResult, vActionName );

                    // Finish details
                    //
					strDetailsHTML += CResourceHelper::LoadResourceStringW ( IDS_HTML_FOOTER, _Module.GetResourceInstance() );

                    pAVEvent->SetDetailsHTML ( strDetailsHTML.c_str() );
                    pAVEvent->SetDetailsText ( strDetailsText.c_str() );
    
                    // Add the new log event to the array
                    //
                    m_pProcessedLogEvents.push_back (pAVEvent);

                } // end threat

            }// end all theats

        } // end subfiles
    }

    return TRUE;
}



HRESULT CVirusAlertCategory::GetDefaultSort (int& iColumn /* [out] */, int& iSortOrder /* [out] */)
{
    iColumn = columnDate;    // Date field
    iSortOrder = ILogViewerCategory::LV_SORT_DESCENDING;    // This will match the other categories
    return S_OK;
}

HRESULT CVirusAlertCategory::GetEvent(int iEventIndex, cc::ILogViewerEvent*& pEvent)
{
    // Don't bother checking pEvent for NULL because it might not be. LogViewer
    // reuses the pointer in a loop. See defect 

    // Load event
    //
    if ( iEventIndex > (int) m_pProcessedLogEvents.size ()-1)
        return E_FAIL;

    // Give them our internal event because we had to pre-generate these. 
    // The other categories don't pre-generate but generate on demand.
    // An alternative it to create a copy function for the data in an event and
    // pass LogViewer a copy of the data.
    //
    pEvent = m_pProcessedLogEvents.at(iEventIndex);

    pEvent->AddRef(); // neccessary because we are passing our event to them, not a copy

    if ( !pEvent )
        return E_FAIL;

    return S_OK;
}

HRESULT CVirusAlertCategory::loadBatch ()
{
    // Load the events
    CAVCategory::loadBatch ();

    // Now break each one out into separate threat alerts
    for ( int iEventIndex = 0; iEventIndex < m_nEventCount; iEventIndex++)
    {
        // ccLogView will call Release() on the created events        
        processLogEvents ( m_vecEventDataPtr[iEventIndex]->m_pEvent, 
                           m_vecEventDataPtr[iEventIndex]->m_EventInfo );

    }

    m_nEventCount = m_pProcessedLogEvents.size();

    // Delete these temp log events
    //
    CAVCategory::deleteEvents ();

    return SYM_OK;
}

void CVirusAlertCategory::deleteEvents ()
{
    CAVCategory::deleteEvents ();

    for ( size_t iIndex = 0; iIndex < m_pProcessedLogEvents.size(); iIndex++)
    {
        cc::ILogViewerEvent* pTemp = m_pProcessedLogEvents.at(iIndex);
        if (pTemp)
        {
            pTemp->Release();
        }
    }

    m_pProcessedLogEvents.clear();
}

void CVirusAlertCategory::clearSearchContexts()
{
    deleteEvents ();
    return CAVCategory::clearSearchContexts();
}
