/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#include "stdafx.h"
#include "qserver.h"
#include "ThreadPool.h"
#include "proccessSamples.h"
#include "AvisEventDetection.h"   // in QuarantineServer\Include 
#include "EventObject.h"
#include "EventData.h"
#include "Resource.h"
#include "QServerSavInfo.h"
extern void GetStatusToken(CString& sStatusToken, CString& sStatusID );

///////////////////////////////////////////////////////////////////////////////
//
// Function name: ProcessSamples
//
// Description: 
//
// Return type: HRESULT
//
//
///////////////////////////////////////////////////////////////////////////////
// 01-18-00 - TMARLES	: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT ProcesSamples (DWORD dwModuleData, BOOL *pStopping)
{
	HRESULT hr = S_OK;
	USES_CONVERSION;
    CComPtr<IEnumQuarantineServerItems> pEnum; 
    CComPtr<IQuarantineServer> pQServer; 
	IQuarantineServerItem *paQServerItem[NOITEMFETCHED]={0};
	ULONG ulNoItems = 0;
	BOOL bRc = FALSE;
	CEventDetect  EventDetect;
	CEvent  *pData= NULL; 
	int	iStopRC;
    // AVIS_ALERT_INBOUND_EVENT  stNewEvent;
	CAlertEvent stNewEvent;
	
	*pStopping = FALSE;
	
    CRegKey reg;		// used only for testing events
	DWORD	dwfTestingEvents=FALSE;

    if( ERROR_SUCCESS == reg.Create( HKEY_LOCAL_MACHINE, REGKEY_QSERVER, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS) )
		reg.QueryDWORDValue( REGVALUE_TEST_EVENTS, (DWORD&)dwfTestingEvents);

	CObList *pSampleList = new CSampleList;
	EventDetect.DetectGeneralErrors( pSampleList, dwModuleData );
		
	
	// get com pointer to Qserver
	hr = pQServer.CoCreateInstance(__uuidof( QuarantineServer ),
									NULL,
									CLSCTX_ALL);

	
	// return on fail
	if (FAILED(hr))
		return E_FAIL;
	
	// how many items?
	hr = pQServer->GetItemCount(&ulNoItems);

	// if we get a failure something wrong check to see if no items 
	// if no items then no work to be done return ok
	if (FAILED(hr))
		return E_FAIL;

	// get enumerator
	hr = pQServer->EnumItems (&pEnum);


//	if (dwfTestingEvents & QS_TEST_GENERAL_EVENTS == QS_TEST_GENERAL_EVENTS)
//		EventDetect.TestGeneralErrors( pSampleList, dwModuleData );

	ULONG fetched = 0;
	// start looping and getting items
	while( pEnum->Next( NOITEMFETCHED, paQServerItem, &fetched ) == S_OK )
	{
		for( ULONG i = 0; i < fetched; i++ )
		{
			// are we going to purge stuff? tm 5-8-00
			// we have to be in purge enabled also see if there has been a quota error (either space or number of files)
			if (_Module.GetPurge() && _Module.GetQuotaError())
			{
				// Okay now we want to purge and there has been a quota error
				HRESULT hr = S_OK;
				VARIANT v;
				DWORD dwSampleStatus = 0;
				VariantInit( &v );
				try
				{
					// get the sample status state and see if we can delete this sample
					hr = paQServerItem[i]->GetValue( QSITEMINFO_X_SAMPLE_STATUS, &v );
					if( SUCCEEDED( hr ) )
					{
						if( v.vt != VT_EMPTY )
						{
							if( v.vt == VT_UI4 )
							{
								dwSampleStatus = v.ulVal ;
								// if the defs have been installed we will want to delete this sample.  proccessing has been done.
								// we may want to add STATUS_UNNEEDED and STATUS_DISTRIBUTED as conditions for delete later.
								if (dwSampleStatus == STATUS_INSTALLED)
								{
									DWORD dwFileID;
									VariantClear( &v );
									// get the sample id
									hr = paQServerItem[i]->GetValue( QSITEMINFO_X_SAMPLE_FILEID, &v );

									// The file ID is requred to remove the quarantined sample
									if( v.vt != VT_EMPTY && SUCCEEDED(hr)) // error here every sample should have a file id
									{
										TCHAR szMsgFormat[MAX_PATH]; 
										CString sVirusName,sFileName,sStatus,sStatusID, sMsg, sMsg1, sMsg2, sStatusToken,sUser,sDef;
										
										dwFileID = v.ulVal;
										//remove the sample
										_Module.RemoveOpenItem( dwFileID, paQServerItem[i]);
										hr = pQServer->RemoveItem(dwFileID);
										
										// get addational information for logging purposes
										VariantClear(&v);
										// Get the orginal filename
										hr = paQServerItem[i]->GetValue( QSITEMINFO_X_SAMPLE_FILE, &v );
										if(v.vt != VT_EMPTY && SUCCEEDED(hr))
										{
											if(v.vt == VT_BSTR)
												sFileName= v.bstrVal;
										}
										VariantClear(&v);
										// get the virus name
										hr = paQServerItem[i]->GetValue(QSITEMINFO_X_SCAN_VIRUS_NAME,&v);
										if(v.vt != VT_EMPTY && SUCCEEDED(hr))
										{
											if(v.vt == VT_BSTR)
												sVirusName= v.bstrVal;
										}
										
										VariantClear(&v);
										// get the user name
										hr = paQServerItem[i]->GetValue(QSITEMINFO_X_PLATFORM_USER,&v);
										if(v.vt != VT_EMPTY && SUCCEEDED(hr))
										{
											if(v.vt == VT_BSTR)
												sUser= v.bstrVal;
										}

										VariantClear(&v);
										// get the Defination used 
										hr = paQServerItem[i]->GetValue(QSITEMINFO_X_SIGNATURES_SEQUENCE,&v);
										if(v.vt != VT_EMPTY && SUCCEEDED(hr))
										{
											if(v.vt == VT_BSTR)
												sDef= v.bstrVal;
										}

										// determin the sample status string
										sStatusID.Format(_T("%d"),dwSampleStatus);
										GetStatusToken(sStatusToken, sStatusID );
										sStatus = LookUpIcePackTokenString(sStatusToken.GetBuffer(0),ICEPACK_TOKEN_STATUS_TABLE);
										
										// format the logging message
										LoadString( _Module.GetResourceInstance(), IDS_PURGE_MSG1, szMsgFormat, sizeof(szMsgFormat));
										sMsg1.Format(szMsgFormat,dwFileID,sFileName,sStatus,sVirusName);
										LoadString( _Module.GetResourceInstance(), IDS_PURGE_MSG2, szMsgFormat, sizeof(szMsgFormat));
										sMsg2.Format(szMsgFormat,sUser,sDef);
										sMsg= sMsg1+sMsg2;
										// write out the message to both NT Event log and QServer Log.
										fWidePrintString(T2A(sMsg.GetBuffer(0)));
										_Module.LogEvent(IDM_GENERAL_INFO_PURGE_SAMPLE,EVENTLOG_INFORMATION_TYPE,sMsg.GetBuffer(0));

										VariantClear(&v);
									}
									// releas this one since we are done with it.
									paQServerItem[i]->Release ();
									// this sample has been removed so we don't 
									//want to do anything but move on to the next one
									continue;
								}
							}

						}
					}
				}
				catch(...) 
				{
				}

			}
			// 
			// Does this sample need attention?
			// 
			bRc = EventDetect.DetectSampleError( paQServerItem[i], &stNewEvent, dwModuleData );

//			if (dwfTestingEvents & QS_TEST_SAMPLE_EVENTS == QS_TEST_SAMPLE_EVENTS && i == 0)
//				EventDetect.TestSampleError( paQServerItem[i], &stNewEvent, dwModuleData );

			// if not delete the data object and move on
			if (bRc == TRUE)
			{
				// 
				// Create sample object.
				// 
				pData = new CEvent (&stNewEvent);
				
				// set the interface pointer
				pData->m_pQSItem = paQServerItem[i];

				// add object to the Event List.
				pSampleList->AddTail(pData);
 
			}
			else
				paQServerItem[i]->Release ();
			
		}
		// reset
		fetched = 0;
		memset(paQServerItem, 0,sizeof(paQServerItem));

		// Check to see if we are stopping
		iStopRC = WaitForSingleObject(_Module.m_hStopping, 0);
		if (iStopRC == WAIT_OBJECT_0)
		{
#ifdef _DEBUG
			fWidePrintString("Alert Scanning thread detected service stopping shutting down");
#endif
			*pStopping = TRUE;
			break;
		}

		Sleep(200);
	} // end while getting data

	if (_Module.GetQuotaError())
	{
		_Module.ClearQuotaError();
	}

	if (!pSampleList->IsEmpty())
	{
		CTime CurrentTime;
		CurrentTime.GetCurrentTime();
		CTimeSpan dtTime=CurrentTime - _Module.m_tTimeOfAMSReg;

		if (dtTime.GetHours() > 24 )
		{
			_Module.m_bQserver2ChanceAMSReg = TRUE;
			_Module.m_tTimeOfAMSReg = CurrentTime;
			if (!StartAMSEx())
			{
				_Module.LogEvent (IDM_ERR_INITIALIZING_AMS_EVENTS);
			}

		}
		
	if(_Module.m_bQserver2ChanceAMSReg == FALSE)
	{
		_Module.m_bQserver2ChanceAMSReg = TRUE;
		_Module.m_tTimeOfAMSReg = CurrentTime;
		if (!StartAMSEx())
		{
			_Module.LogEvent (IDM_ERR_INITIALIZING_AMS_EVENTS);
		}
	}
		hr = EventDetect.ProccesEventQueue(pSampleList);
	}

	POSITION pos = pSampleList->GetHeadPosition();
	while (pos)
	{
		pData = static_cast <CEvent *>(pSampleList->GetAt(pos));
		if (pData)
			delete pData;
		pSampleList->SetAt(pos, NULL);
		pSampleList->GetNext(pos);
	}

	delete pSampleList;
	

	return hr;
}
