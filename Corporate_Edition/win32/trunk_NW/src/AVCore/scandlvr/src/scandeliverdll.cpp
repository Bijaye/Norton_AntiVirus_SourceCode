// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/scandeliverdll.cpv   1.65   10 Jul 1998 15:24:28   jtaylor  $
////////////////////////////////////////////////////////////////////////////
//
// ScanDeliverDLL.cpp - Contains implementation for CScanDeliverDLL
//
// 12 Jan 2005 KTALINKI - Enhanced for the support of Anomaly / Extended Threats
//	Modified to use IQuarantineSession, IQuarantinItem2, ISNDSubmission, ISNDSample interfaces to
//	create SARC Submission package as a ZIP File containing each threat in a seperate ZIP file
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <new>

#include "sndclassloader.h"

#include "ScanDeliverDLL.h"
#include "ScanDlvr.h"
#include "ScanWiz.h"
#include "QuarAdd.h"

// SARC Scan and Deliver APIs
#include "SDSendMe.h"
#include "SDStrip.h"
#include "SDSocket.h"
#include "SDFilter.h"

#include "iquaran.h"
#include "Quar32.h"
#include "Quaradd.h"
#include "global.h"
#include "smtpmail.h"
#include "AcceptPg.h"
#include "RejectPg.h"
#include "CorpPg.h"
#include "ReviewPg.h"
#include "WrapUpPg.h"
#include "WelcomPg.h"
#include "User1Pg.h"
#include "User2Pg.h"
#include "User3Pg.h"
#include "xapi.h"
#include "FOTDPg.h"
#include "Time.h"
#include "User4Pg.h"
#include "Review1Pg.h"
#include "ccEraserInterface.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"
#include "crc32.h"
#include "arclib.h"
#include "pkarc.h"
#include "alsimple.h"
#include "hashpack.h"
#include "psm.h"
#include "BLDNUM.H"

// Include the Scan and Deliver resource files.
#include "sdrcinit.h"

// Progress dialog
#include "ProgDlg.h"

// Global object count
extern LONG g_dwObjs;

// Global Configurations object
extern CScanDeliverConfiguration   g_ConfigInfo;

extern TCHAR g_szMainNavDir[MAX_QUARANTINE_FILENAME_LEN + 1];
extern BOOL g_bSubmitAdditionalInfo;
extern BOOL	g_bUsedByCQC;
extern BOOL	g_bCQAnomaly;
//
// This function lives in ScanDRes.
//
BOOL __declspec( dllimport ) InitScanDeliverResources();

const char STR_SYSTEM_SNAPSHOT_TEMP_FILE[] = "SnapShot";

////////////////////////////////////////////////////////////////////////////
// Function name    : UpdateProgress
//
// Description      : This function is used to update the progress control
//                    passed in as lpvProgress, to the nPercent mark.
//
////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL static UpdateProgress( LPVOID lpvProgress, int nPercent )
    {
        // Advance progress dialog
        ((CProgressDlg *)lpvProgress)->SetPos( nPercent );

        return !((CProgressDlg *)lpvProgress)->CheckCancelButton();
    };

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::CScanDeliverDLL
//
// Description      : Constructor
//
////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CScanDeliverDLL::CScanDeliverDLL()
{
    m_dwRef =0;
    InterlockedIncrement( &g_dwObjs );
    m_nAcceptedFiles = 0;
    m_nRejectedFiles = 0;
    m_lpbyBuff = NULL;
    m_szPackageFilename[0] = NULL;
    m_bInFileOfTheDayMode = FALSE;
    m_bPackageSent = FALSE;
	m_pStripper = NULL;
    ZeroMemory( &m_stDefsDate, sizeof( SYSTEMTIME ) );
	m_strDeletableFolderArray.RemoveAll();
	m_strDeletableFilesArray.RemoveAll();
	m_pISNDSubmission = NULL;
}



////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::~CScanDeliverDLL
//
// Description      : Destructor
//
////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
// 4/19/98 JTAYLOR - Added code to release memory.
// 1/12/05 KTALINKI - Updated to call CleanUp()
////////////////////////////////////////////////////////////////////////////
CScanDeliverDLL::~CScanDeliverDLL()
{	
    InterlockedDecrement( &g_dwObjs );	
	CleanUp();		
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::~CScanDeliverDLL
//
// Description      : Helper method to cleanup all the accepted and rejected 
//						files lists, delete the temporory files and folders
// Returns			: HRESULT
////////////////////////////////////////////////////////////////////////////
HRESULT CScanDeliverDLL::CleanUp()
{
	int iCnt = 0;

	try
	{
		try
		{
			if(m_pISNDSubmission)
			{
				if( NULL != m_lpbyBuff )
				{
					m_pISNDSubmission->ReleaseSubmissionData( m_lpbyBuff );
				}
				m_lpbyBuff = NULL;
				m_pISNDSubmission.Release();
				m_pISNDSubmission = NULL;
			}
		}
		catch(...)
		{
		}

		//Delete the accepted Files & String Arrays
		iCnt = m_AcceptedFileNames.GetSize();
		for(int i=0; i<iCnt; i++)
		{
			CStringArray *pStrArray = m_AcceptedFileNames.GetAt(i);

			if(pStrArray)
			{
				int iFileCnt = pStrArray->GetSize();
				for(int idx =0; idx < iFileCnt; idx++)
					DeleteFile(pStrArray->GetAt(idx));

				pStrArray->RemoveAll();
				delete pStrArray;
			}
		}
		m_AcceptedFileNames.RemoveAll();

		//Delete all the temporory file created.
		iCnt = m_strDeletableFilesArray.GetSize();
		for(int i=0;i<iCnt; i++)
		{
			DeleteFile((LPCTSTR)(m_strDeletableFilesArray.GetAt(i)));			
		}
		m_strDeletableFilesArray.RemoveAll();

		//Cleanup the temporory folders created 
		iCnt = m_strDeletableFolderArray.GetSize();
		for(int i=0;i<iCnt; i++)
		{
			RemoveDirectory((LPCTSTR)(m_strDeletableFolderArray.GetAt(i)));			
		}
		m_strDeletableFolderArray.RemoveAll();		

		//Delete the Rejection Strings and memory allocated for them
		iCnt = m_RejectedFileStrings.GetSize();
		for(int i=0;i<iCnt; i++)
		{
			CString* pStr = m_RejectedFileStrings.GetAt(i);
			if(pStr)
				delete pStr;
		}
		m_RejectedFileStrings.RemoveAll();		

		//Release the Rejected Session Interface pointers and the array
		iCnt = m_RejectedFileInterfacePointers.GetSize();
		for(int i=0; i<iCnt; i++)
		{
			IQuarantineSession *ptr = m_RejectedFileInterfacePointers.GetAt(i);
			if(ptr)
				ptr->Release();
		}
		m_RejectedFileInterfacePointers.RemoveAll();

		//Release the Accepted Session Interface pointers and the array
		iCnt = m_AcceptedFileInterfacePointers.GetSize();
		for(int i=0; i<iCnt; i++)
		{
			IQuarantineSession *ptr = m_AcceptedFileInterfacePointers.GetAt(i);
			if(ptr)
				ptr->Release();
		}
		m_AcceptedFileInterfacePointers.RemoveAll();

		// If we were stripping files Release the ContentStripper Interface pointer
		if( NULL != m_pStripper )
		{
			m_pStripper->ShutDown();
			m_pStripper->Release();
		}
		return S_OK;
	}
	catch(...)
	{
		return E_UNEXPECTED;
	}	
}

///////////////////////////////////////////////////////////////////
// IUnknown implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function name    : CScanDeliverDLL::QueryInterface
// Description      : This function will return a requested COM interface
// Return type      : STDMETHODIMP
// Argument         : REFIID riid - REFIID of interface requested
// Argument         : void** ppv - pointer to requested interface
//
////////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
// 1/12/05 KTALINKI	- Modfied to support IID_ScanDeliverDLL3 and removed support for 
//					  IID_ScanDeliverDLL & IID_ScanDeliverDLL2
// 2/22/05 KTALINKI - Renabled IID_ScanDeliverDLL2 for Central Quarantine Server,
//						to submit the legacy threats as IQuarantineItem2 interface pointers.
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::QueryInterface( REFIID riid, void** ppv )
{
    *ppv = NULL;

    if( IsEqualIID( riid, IID_IUnknown )|| IsEqualIID( riid, IID_ScanDeliverDLL2 ) ||
		IsEqualIID( riid, IID_ScanDeliverDLL3 ) )
        *ppv = this;

    if( *ppv )
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return ResultFromScode( S_OK );
        }

    return ResultFromScode( E_NOINTERFACE );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name    : CScanDeliverDLL::AddRef()
// Description      : Increments reference count for this object
// Return type      : ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CScanDeliverDLL::AddRef()
{
    return ++m_dwRef;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name    : CScanDeliverDLL::Release()
// Description      : Decrements reference count for this object
// Return type      : ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CScanDeliverDLL::Release()
{
    if( --m_dwRef )
        return m_dwRef;

    delete this;
    return 0;
}


////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::GetVersion()
//
// Description      : Returns the version number of this object
//
// Return type      : UINT
//
////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_( UINT ) CScanDeliverDLL::GetVersion()
{
    return 1;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::DeliverFilesToSarc()
//
// Description      : This function starts the file delivery process
//
// Return type      :
//
////////////////////////////////////////////////////////////////////////////
// 12/28/04 KTALINKI -	Method created to deliver files to SARC using 
//						IQuarantineSession interface to facilitate the usage
//						of Extended Threats. SARC submission package will have
//						subpackages for each threat.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::DeliverFilesToSarc( IQuarantineSession **pQSesArray, int nNumberSessions )
{
    // since this module displays the wizard panels, we need to swap module states
    // in order to access our resources
    AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

    auto int     nIndex = 0;
    auto int     i = 0;
    auto int     nStartup = 0;
    auto char    szGeneratedFilename[MAX_QUARANTINE_FILENAME_LEN]      = "";
    auto DWORD   dwErrorBits                                           = 0;
    auto CString szRejectionReason;
    auto int     nMaxRejectionReasonLength                             = 1000;
    auto IQuarantineItem2 *pIQuarItem                                   = NULL;
    auto HRESULT hr                                                    = E_FAIL;
    auto BOOL    bResult                                               = FALSE;
    auto HRESULT hrRetVal                                              = E_FAIL;
    //auto ViralSubmissionFilter *pVirFilter                             = NULL;
    // WJORDAN 2-5-03: Removing dependencies on the scan and deliver filtering dll (sdflt32i.dll)
    //auto CMaliciousCodeFilter *pFileFilter                             = NULL;
    auto CString szStatusText;

	auto SDPACKDLL_Submission_Loader	objSndSubLoader;
	auto SDPACKDLL_Sample_Loader		objSndSampleLoader;
	SYMRESULT							sr = 0;	
	CStringArray						*pStrArray = NULL;	
    HINSTANCE hResources = NULL;

	try
	{
		g_bUsedByCQC = FALSE;
		g_bCQAnomaly = FALSE;
		m_bPackageSent = FALSE;
		m_strDeletableFolderArray.RemoveAll();

		// Get the installation directory
		if( m_sStartupFolder.IsEmpty() )
		{
			bResult = GetMainNavDir( m_sStartupFolder.GetBuffer(MAX_QUARANTINE_FILENAME_LEN), 
                                     MAX_QUARANTINE_FILENAME_LEN * sizeof (TCHAR));
			m_sStartupFolder.ReleaseBuffer();

			// Make sure the get path function suceeded.
			if( FALSE == bResult )
				return E_UNABLE_TO_INITIALIZE;
		}

		// Make sure that the resource DLL is loaded
		InitScanDeliverResources( );

		// Make sure that the arguments are marginally valid
		if( ( 0 >= nNumberSessions ) || ( NULL == pQSesArray ) )
			return E_INVALIDARG;

		// Initialize some global DBCS information.
		InitDbcsVars();

		// Check to see if we are running on a trial system that has expired.
		BOOL bTrialExpired = IsTrialInstallAndExpired();

		if( bTrialExpired )
		{
			CString szTrialTitle;
			szTrialTitle.LoadString(IDS_TRIAL_EXPIRED_TITLE);
			CString szTrialText;
			szTrialText.LoadString(IDS_TRIAL_EXPIRED_TEXT);

			MessageBox(NULL, szTrialText, szTrialTitle, MB_OK);
			return E_FAIL;
		}

		// Clear the accepted and rejected lists. These should be null.
		hrRetVal = CleanUp();
		if(FAILED(hrRetVal))
			return hrRetVal;	

		// Create progress dialog
		CProgressDlg progress;
		progress.Create();
		progress.SetRange(0, nNumberSessions );
		// Load the status text string
		szStatusText.LoadString(IDS_PROGRESS_ANALYZING);
		progress.SetStatus(szStatusText);

		// get the temporary Quarantine Directory for unpackaging
		bResult = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
									REGKEY_QUARANTINE_TEMP_PATH,
									REGVAL_QUARANTINE_TEMP_PATH,
									m_sTempFolder.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ),
									MAX_QUARANTINE_FILENAME_LEN);
		m_sTempFolder.ReleaseBuffer();
		if( FALSE == bResult )
		{
			// If no temp folder specified, use windows temp path.    
			if( 0 == GetTempPath( MAX_QUARANTINE_FILENAME_LEN, m_sTempFolder.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ) ) )
			{
				m_sTempFolder.ReleaseBuffer();
				hrRetVal = E_QUARANTINE_DIRECTORY_INVALID;
				goto Exit_Function;
			}
			m_sTempFolder.ReleaseBuffer();
		}

		// Get virus defs folder.
		if( m_sDefsFolder.IsEmpty() )
		{
			bResult = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
										REGKEY_NAV_DEFINITIONS_PATH,
										REGVAL_NAV_DEFINITIONS_PATH,
										m_sDefsFolder.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ),
										MAX_QUARANTINE_FILENAME_LEN);
			m_sDefsFolder.ReleaseBuffer();
			if( FALSE == bResult )
			{
				hrRetVal = E_QUARANTINE_DIRECTORY_INVALID;
				goto Exit_Function;
			}
		}

		// get the Scan Config Options Directory
		if( m_sScanConfigPath.IsEmpty() )
		{
			bResult = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
										REGKEY_QUARANTINE_PATH,
										REGVAL_QUARANTINE_OPTIONS_PATH_STR,
										m_sScanConfigPath.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ),
										MAX_QUARANTINE_FILENAME_LEN);
			m_sScanConfigPath.ReleaseBuffer(0);
			if( FALSE == bResult )
			{
				hrRetVal = E_QUARANTINE_DIRECTORY_INVALID;
				goto Exit_Function;
			}
		}

		/*
		********************************************************************************************
		1/5/05	KTALINKI - Removing the dependency on ViralSubmissionFilter.
		ViralSubmissionFilter::AllowSubmission is checking whether the file is Non Repairable or not.
		That logic will be implemented in local AllowSubmission Method and called.
		********************************************************************************************
		// Create the SARC filter APIs
		pVirFilter = ViralSubmissionFilterFactory::CreateInstance();
		if( NULL == pVirFilter )
		{
			hrRetVal = E_UNABLE_TO_CREATE_VIRUS_FILTER;
			goto Exit_Function;
		}

		// Start the virus filter
		nStartup = pVirFilter->StartUp(m_sStartupFolder.GetBuffer(0), m_sDefsFolder, FILTER_TYPE_SELF_DETERMINATION );
		if( INIT_STATUS_NO_ERROR != nStartup )
		{
			hrRetVal = E_UNABLE_TO_START_VIRUS_FILTER;
			goto Exit_Function;
		}
		m_sStartupFolder.ReleaseBuffer();
		*/	
		// WJORDAN 2-5-03: Removing dependencies on the scan and deliver filtering dll (sdflt32i.dll)
		/* Create the SARC file filter APIs
		pFileFilter = CMaliciousCodeFilterFactory::CreateInstance();
		if( NULL == pFileFilter )
			{
			hrRetVal = E_UNABLE_TO_CREATE_FILE_MALICIOUSCODEFILTER;
			goto Exit_Function;
			}

		// Start the virus filter
		nStartup = pFileFilter->StartUp( m_sStartupFolder.GetBuffer(0) );
		if( INIT_STATUS_NO_ERROR != nStartup )
			{
			hrRetVal = E_UNABLE_TO_CREATE_FILE_MALICIOUSCODEFILTER;
			goto Exit_Function;
			}
		*/
		// Add each item to the accepted or rejected list.
		for( i = 0; i < nNumberSessions; i++ )
		{
			DWORD		dwItemCnt = 0;
			DWORD		dwItemType = -1;
			CString		strDestFile = _T("");
			CString		strUnPackSesRoot = m_sTempFolder;
			VBININFO	stVBinInfo;		
			CString *pstrRejReason = NULL;
			int			iRemedCnt = 0;
			pStrArray	= NULL;
			
			try{ pstrRejReason = new CString; }
			catch(std::bad_alloc&){ return E_OUTOFMEMORY; }
	        
			if(NULL == pstrRejReason)
			{
				hrRetVal = E_OUTOFMEMORY;
				goto Exit_Function;
			}
			//Verify whether the session is submittable to SARC or not
			//If rejected, add the session pointer to Rejected File Interfaces list and 
			//add rejection string to RejectedFileStrings array
			if(!IsSessionSubmittable(pQSesArray[i], *pstrRejReason))
			{			
				// If the file is rejected then add it to the rejected list.
				// Save the rejection reason for later, and delete the rejected file.
				nIndex = m_RejectedFileInterfacePointers.Add(pQSesArray[i]);
				pstrRejReason->LoadString(IDS_TRIVIAL_POSSIBLE_THREAT);
				try{m_RejectedFileStrings.SetAtGrow( nIndex,pstrRejReason);}
				catch(...)
				{
					delete pstrRejReason; 
					return E_UNEXPECTED;
				}
				m_nRejectedFiles++;
				continue;	//Conitnue to the next session
			}			

			IQuarantineSession *pQSes = pQSesArray[i];
			IQuarantineItem2		*pQItem = NULL;

			//Get the VBININFO of the session
			hrRetVal = pQSes->GetSessionInfo(&stVBinInfo);
			if(FAILED(hrRetVal))
			{
				hrRetVal = E_UNABLE_TO_GET_QSESSION_INFO;
				goto Exit_Function;
			}

			if(!strUnPackSesRoot.IsEmpty() && strUnPackSesRoot[strUnPackSesRoot.GetLength() - 1] != '\\')
				strUnPackSesRoot += "\\";			

            //Append the Session id to create the destination folder to unpack the items in to
			if(stVBinInfo.dwSessionID)
				strUnPackSesRoot.AppendFormat("%08lX\\", stVBinInfo.dwSessionID);			
			else
				strUnPackSesRoot.AppendFormat("%08lX\\", stVBinInfo.RecordID);	//Legacy Threat			

			//Create the folder if doesn't exist
			if(!PathFileExists(strUnPackSesRoot))
			{
                if(FALSE == CreateDirectory(strUnPackSesRoot, NULL))
				{
					hrRetVal = E_UNABLE_TO_CREATE_TEMP_FOLDER;
					goto Exit_Function;
				}
				m_strDeletableFolderArray.Add(strUnPackSesRoot);
			}
			
			//Get the number of items in the session
			hrRetVal = pQSes->GetItemCount(&dwItemCnt);
			if(FAILED(hrRetVal))
			{
				hrRetVal = E_UNABLE_TO_GET_QSESSION_INFO;
				goto Exit_Function;
			}

			//Allocate memory for CStringArray to store generated file names
			try{pStrArray = new CStringArray();}
			catch(...){return E_OUTOFMEMORY;}

			if(NULL == pStrArray)
			{
				hrRetVal = E_OUTOFMEMORY;
				goto Exit_Function;
			}
			pStrArray->RemoveAll();
			
			//Get first item from the session
			hrRetVal = pQSes->GetFirstItem(&pQItem);
			if(FAILED(hrRetVal) || NULL == pQItem)
			{
				hrRetVal = E_UNABLE_TO_GET_QITEM;
				goto Exit_Function;
			}		
			
			//Loop thru all the items in the session
			for(DWORD j=0; j < dwItemCnt && NULL != pQItem; j ++)
			{			
				// Clear the Generated Filename.  Having a non-empty value will confuse UnpackageItem
				szGeneratedFilename[0] = NULL;
				VBININFO	stItemInfo;
				
				ZeroMemory(&stItemInfo, sizeof(VBININFO));

				//Get the item type from the session
				hrRetVal = pQItem->GetItemInfo(&stItemInfo);
				if(FAILED(hrRetVal))
				{
					hrRetVal = E_UNABLE_TO_GET_QITEM;
					goto Exit_Function;
				}
				strDestFile = strUnPackSesRoot;

				//Prepare the destination file name to unpack to, depeding on the item type
				switch(stItemInfo.dwRecordType)
				{
					case VBIN_RECORD_LEGACY_INFECTION:
						{
							hrRetVal = pQItem->UnpackageItem((LPSTR)(LPCSTR)strUnPackSesRoot,
																NULL,
																szGeneratedFilename,
																MAX_QUARANTINE_FILENAME_LEN,
																FALSE,
																TRUE );
						}
						break;				
						
					case VBIN_RECORD_REMEDIATION:
						{
							//Unpack the remediation record to a file, later while 
							//creating the package,it can be split in to Action and 
							//Undo files and then get added to submission package
							strDestFile.Format("%08lX.RAU", iRemedCnt++);	
							hrRetVal = pQItem->UnpackageItem((LPSTR)(LPCSTR)strUnPackSesRoot,
													(LPSTR)(LPCSTR)strDestFile,
													szGeneratedFilename,
													MAX_QUARANTINE_FILENAME_LEN,
													FALSE,
													FALSE );						
						}
						break;
					case VBIN_RECORD_SYSTEM_SNAPSHOT:
						{
							strDestFile = STR_SYSTEM_SNAPSHOT_TEMP_FILE;	
							hrRetVal = pQItem->UnpackageItem((LPSTR)(LPCSTR)strUnPackSesRoot,
													(LPSTR)(LPCSTR)strDestFile,
													szGeneratedFilename,
													MAX_QUARANTINE_FILENAME_LEN,
													FALSE,
													FALSE );							
						}
						break;
				}			
				if( FAILED( hrRetVal ) )
				{
					//Delete the files and Release the memory allocated so far for pStrArray
					try
					{
						if(pStrArray)
						{
							for(int i=0; i<pStrArray->GetSize(); i++)
							{
								if(PathFileExists(pStrArray->GetAt(i)))
									DeleteFile(pStrArray->GetAt(i));
							}
							pStrArray->RemoveAll();
							delete pStrArray;
							pStrArray = NULL;
						}
					}
					catch(...){}
					goto Exit_Function;
				}
				//Add the destination file to list of files
				pStrArray->Add(szGeneratedFilename);
				
				hrRetVal = pQSes->GetNextItem(&pQItem);
				if(FAILED(hrRetVal) || NULL == pQItem)
				{
					break;
				}
			}	//end of For loop for Items in session
					
			nIndex = m_AcceptedFileInterfacePointers.Add(pQSesArray[i]);
			//add the list of files to accepted files list
			m_AcceptedFileNames.SetAtGrow(nIndex, pStrArray);
			m_nAcceptedFiles++;
			
			// Advance progress dialog
			progress.SetPos( i );

			if( progress.CheckCancelButton() )
			{
				hrRetVal = E_USER_CANCELLED;
				goto Exit_Function;
			}
		}	//End of Sessions For Loop 

		// Finish creating the scan config options object.
		AppendFileName( m_sScanConfigPath.GetBuffer(MAX_QUARANTINE_FILENAME_LEN), SCAN_CONFIG_DAT_FILENAME );
		m_sScanConfigPath.ReleaseBuffer();

		bResult = g_ConfigInfo.Initialize(m_sScanConfigPath.GetBuffer(0), TRUE); // TRUE means create the DAT file if it does not exist
		if( FALSE == bResult )
		{
			hrRetVal = E_UNABLE_TO_CREATE_OPTIONS_OBJECT;
			goto Exit_Function;
		}

		bResult = g_ConfigInfo.ReadConfigFile(&dwErrorBits);
		if( FALSE == bResult )
		{
			hrRetVal = E_UNABLE_TO_READ_OPTIONS;
			goto Exit_Function;
		}

		// Remove progress window
		progress.DestroyWindow();

		// display the wizard panels
		LaunchUI();

		// If the wizard panels sent a package to SARC, then update the
		// IQuarantineItems.
		if( TRUE == m_bPackageSent )
		{
			// Get the current date/time
			SYSTEMTIME stDateSubmitted;
			GetSystemTime( &stDateSubmitted );

			// Since the send to SARC was successfull, mark them as sent.
			for( i = 0; i < m_nAcceptedFiles; i++ )
			{
				IQuarantineSession	*pQSes = NULL;

				// Update the status of the file as sent to SARC.            
				pQSes = m_AcceptedFileInterfacePointers[i];
				if(pQSes)
				{
					hr = pQSes->SetStatus(QFILE_STATUS_SUBMITTED);
					
					if( FAILED( hr ) )
					{
						hrRetVal = hr;
						continue;
					}

					// Update the time the files were sent to SARC            
					hr = pQSes->SetDateSubmittedToSARC( &stDateSubmitted );
					if( FAILED( hr ) )
					{
						hrRetVal = hr;
						continue;
					}
		            
					hr = pQSes->SaveSession();
					if( FAILED( hr ) )
					{
						hrRetVal = hr;
						continue;
					}
				}
			}
		}

		hrRetVal = S_OK;

	Exit_Function:

		if( S_OK != hrRetVal &&
			E_USER_CANCELLED != hrRetVal &&
			E_OUTOFMEMORY != hrRetVal )
		{
			auto CString szError;
			auto CString szErrorCaption;
			szErrorCaption.LoadString( IDS_CAPTION_STANDARD_MESGBOX );

			if( TRUE == m_bPackageSent )
			{
				szError.LoadString( IDS_ERROR_UPDATING_ITEMS );
			}
			else
			{
				szError.LoadString( IDS_ERROR_STARTING_SUBMISSION );
			}

			MessageBox( NULL, szError, szErrorCaption, MB_OK | MB_ICONERROR );
		}

		// Remove progress window
		progress.DestroyWindow();

		// If we were in the middle of testing a file, delete that file
		if( NULL != szGeneratedFilename[0] )
		{
			WipeOutFile( szGeneratedFilename );

			// Clear the Generated Filename.
			szGeneratedFilename[0] = NULL;
		}
		/*
		// Clean up the virus filter -- note these have no return values to check
		if( NULL != pVirFilter )
		{
			pVirFilter->ShutDown();
			pVirFilter->Release();
		}
		*/
		//
		// WJORDAN 2-5-03: Removing dependencies on the scan and deliver filtering dll (sdflt32i.dll)
		//
		/* Clean up the file filter -- note these have no return values to check
		if( NULL != pFileFilter )
			{
			pFileFilter->ShutDown();
			pFileFilter->Release();
			}
		*/
		
		// Delete the package file.
		if( _tcslen(m_szPackageFilename) > 0 )
		{
			WipeOutFile( m_szPackageFilename );
		}

		hrRetVal = CleanUp();

		FreeLibrary(hResources);
		return hrRetVal;
	}
	catch(...)
	{
		return E_UNEXPECTED;
	}    
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::GetNumberRejectedFiles()
//
// Description      : This function is used to retrieve the number of rejected
//                    files in this object.
//
// Return type      : int, this is the number of files rejected.
//
////////////////////////////////////////////////////////////////////////////
// 4/14/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
int CScanDeliverDLL::GetNumberRejectedFiles()
{
    return m_nRejectedFiles;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::GetNumberAcceptedFiles()
//
// Description      : This function is used to retrieve the number of accepted
//                    files in this object.
//
// Return type      : int, this is the number of files accepted.
//
////////////////////////////////////////////////////////////////////////////
// 4/14/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
int CScanDeliverDLL::GetNumberAcceptedFiles()
{
    return m_nAcceptedFiles;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::GetRejectionReason
//
// Description      : This function is used to retrieve the string containing
//                    the reason why files where rejected.
//
// Return type      : CString *, this is the reason.  This may be NULL.
//
////////////////////////////////////////////////////////////////////////////
// 4/14/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CString * CScanDeliverDLL::GetRejectionReason(int nIndex)
{
    if( ( nIndex < 0 ) ||
        ( nIndex >= m_nRejectedFiles ))
        {
        return NULL;
        }

    return m_RejectedFileStrings.GetAt(nIndex);
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::GetAcceptedQuarantineSession
//
// Description      : This function is used to retrieve the IQuarantineSession
//                    interface for the index.
//
// Return type      : IQuarantineSession *, The item, this may be NULL
//
////////////////////////////////////////////////////////////////////////////
IQuarantineSession * CScanDeliverDLL::GetAcceptedQuarantineSession(int nIndex)
{
    if( ( nIndex < 0 ) ||
        ( nIndex >= m_nAcceptedFiles ))
        {
        return NULL;
        }

    return m_AcceptedFileInterfacePointers.GetAt(nIndex);
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::GetRejectedQuarantineSession
//
// Description      : This function is used to retrieve the IQuarantineSession
//                    interface for the index.
//
// Return type      : IQuarantineSession *, The item, this may be NULL
//
////////////////////////////////////////////////////////////////////////////
IQuarantineSession * CScanDeliverDLL::GetRejectedQuarantineSession(int nIndex)
{
    if( ( nIndex < 0 ) ||
        ( nIndex > m_nRejectedFiles ))
        {
        return NULL;
        }

    return m_RejectedFileInterfacePointers.GetAt(nIndex);
}

////////////////////////////////////////////////////////////////////////////
// Function name    : LaunchUI
//
// Description      : This function displays the series of wizard panels that
//                    walks the user through the submission process
//
// Return type      : TRUE -- if the user selected OK
//                    FALSE -- if the user hit cancel
//
////////////////////////////////////////////////////////////////////////////
// 4/15/98 SEDWAD - Function created / Header added.
// 4/19/98 JTAYLOR - Added return value
// 4/27/98 JTAYLOR - Added code to support switching to File of the Day mode.
////////////////////////////////////////////////////////////////////////////
BOOL    CScanDeliverDLL::LaunchUI(void)
{
    auto    CScanDeliverWizard*         pWizSheet;
    auto    CWelcomePropertyPage        wizWelcome;
    auto    CAcceptedFilesPropertyPage  wizAcceptedFiles(this);
    auto    CRejectedFilesPropertyPage  wizRejectedFiles(this);
    auto    CFileOfTheDayPropertyPage   wizFileOfTheDay(this);
    auto    CUserInfo1PropertyPage      wizUserInfo1;
    auto    CUserInfo2PropertyPage      wizUserInfo2;
    auto    CUserInfo3PropertyPage      wizUserInfo3;
	auto    CUserInfo4PropertyPage      wizUserInfo4;
    auto    CorpInfoPropertyPage        wizCorporateInfo;
	auto    CReview1PropertyPage        wizReview1(this);
    auto    CReviewPropertyPage         wizReview(this);
    auto    CWrapUpPropertyPage         wizWrapUp;
    auto    int                         nResult;

    // create an instance of the property sheet
	try
	{
		pWizSheet = new CScanDeliverWizard(IDS_SCAN_WIZARD_CAPTION);
	}
	catch(std::bad_alloc &)
	{
		return FALSE;
	}

    // add the property pages to the property sheet
    pWizSheet->AddPage(&wizWelcome);

	//These pages display the list of accepted or rejected files.
	//In case of new Anomalies, Central Quarantine Package receives 
	//the SARC submission package from Quarantine Client
	//and tries to forward that thru SnD. Displaying the file list 
	//is not possible unless the package is unpacked and manifest files are read.
	//so do not display these page when this is called from Central Quarantine Console.

	if(FALSE == g_bUsedByCQC)
	{

    // see if it is appropriate to show the dialog for "File of the Day" mode
    if (FOTD_IS_VALID == IsFileOfTheDayModeValid())
        {
        if (TRUE == ConvertToFileOfTheDayMode())
            {
            pWizSheet->AddPage(&wizFileOfTheDay);
            }
        else
            {
            pWizSheet->AddPage(&wizRejectedFiles);
            }
        }
    else
        {
        if (GetNumberAcceptedFiles() > 0)
            {
            pWizSheet->AddPage(&wizAcceptedFiles);
            }

        if (GetNumberRejectedFiles() > 0)
            {
            pWizSheet->AddPage(&wizRejectedFiles);
            }
        }
	}
    // finish adding property pages
    pWizSheet->AddPage(&wizUserInfo1);
    pWizSheet->AddPage(&wizUserInfo2);
    pWizSheet->AddPage(&wizUserInfo3);
	pWizSheet->AddPage(&wizUserInfo4);

    // add the corporate page only if we're running in "corporate mode"
    if (TRUE == g_ConfigInfo.IsCorporateMode())
        {
        pWizSheet->AddPage(&wizCorporateInfo);
        }
	//Review1 page displays the disclaimer to send the system snapshot.
	//Central Quarantine Package already has the System Snapshot, 
	//so displaying and getting the consent from user is not possible, so do not display this page.
	if(FALSE == g_bUsedByCQC)
		pWizSheet->AddPage(&wizReview1);

    pWizSheet->AddPage(&wizReview);
    pWizSheet->AddPage(&wizWrapUp);

    // place the sheet in "wizard" mode and display the wizard panels
    pWizSheet->SetWizardMode();
    nResult = pWizSheet->DoModal();

    // free up alloated memory
    delete  pWizSheet;

    // if the user hit OK then return TRUE
    if (ID_WIZFINISH == nResult)
        {
        return TRUE;
        }

    return FALSE;

}  // end of "CScanDeliverDLL::LaunchUI"


////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::CreatePackage()
//
// Description      : This function fills out the package info and
//                    Adds all the files to the package, creates the
//                    .zip file.
//
// Return type      : HRESULT                    
//
////////////////////////////////////////////////////////////////////////////
// 4/18/98 JTAYLOR - Function created / Header added.
// 4/18/98 JTAYLOR - Added close of file created with CreateUniqueFile.
//                   Added conversion of filenames to Short filenames.
// 4/20/98 JTAYLOR - Added more error handling
// 4/20/98 JTAYLOR - Improved error handling
// 4/22/98 JTAYLOR - Removed short filename creation
// 4/23/98 JTAYLOR - Added support for file stripping
// 4/23/98 JTAYLOR - Moved temp file creation to its own function
// 4/23/98 JTAYLOR - Fixed a bug involving redeclarations of bResult.
// 4/24/98 JTAYLOR - Added 10 MB filesize limitation.
// 4/26/98 JTAYLOR - Added code to preserve original filetimes when stripping
//                   file content.
// 4/26/98 JTAYLOR - Added specific error code for submission > max packet size.
// 5/01/98 JTAYLOR - Fixed problem with GetVersion call.
// 5/06/98 JTAYLOR - Added max buff size.
// 6/04/98 JTAYLOR - Added support for remaining package fields
// 6/08/98 JTAYLOR - Changed the NAV path to use the global function.
// 6/08/98 JTAYLOR - Added support for the platform field.
// 1/12/05 KTALINKI - Modified to support Anomaly / Extended Threats.
//					Added usage of ISNDSubmission, ISNDSample & IQuarantineSession
//					Creating Submission Package containing subpackages for each threat.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::CreatePackage( HWND hWnd )
{
    BOOL bResult = FALSE;
    IQuarantineDLL *pIQuarDll                                     = NULL;    
    
    ISNDSubmission::FIELD stField;
    CContentStripper::INIT_STATUS  stContentInitStatus;
    DWORD   dwHighSize                                            = 0;
    DWORD   dwLowSize                                             = 0;
    HANDLE  hPackage                                              = INVALID_HANDLE_VALUE;
    HRESULT hrRetVal                                              = E_FAIL;    
    BOOL    bStripFiles                                           = FALSE;
    CString szStatusText;
	
	ISNDSamplePtr			ptrISample = NULL;
	ISNDProgressPtr			ptrSNDProgress = NULL;	
	SYMRESULT							sr = 0;

	int		i,iItemIdx	=	0;
	DWORD	dwItemCnt	=	0;
	DWORD	dwTemp = 0;
	DWORD	dwFileSize, dwNumberFiles;

    CWnd *pWindow = CWnd::FromHandle( hWnd );
    if( NULL == pWindow )
    {
        return E_INVALIDARG;
    }

	try
	{
		// Make sure we have valid defs date
		if( m_stDefsDate.wYear == 0 )
		{
			// get the IQuarantineDLL
			hrRetVal = CoCreateInstance( CLSID_QuarantineDLL,
										NULL,
										CLSCTX_INPROC_SERVER,
										IID_QuarantineDLL,
										(LPVOID*)&pIQuarDll);
			if( FAILED( hrRetVal ) )
			{
				return hrRetVal;
			}

			hrRetVal = pIQuarDll->Initialize();
			if( SUCCEEDED( hrRetVal ) )
			{
				// get the date of definitions
				hrRetVal = pIQuarDll->GetCurrentDefinitionsDate( &m_stDefsDate );
			}

			// Cleanup.
			pIQuarDll->Release();
			if( FAILED( hrRetVal ) )
				return hrRetVal;
		}
	}
	catch(...)
	{
		//Continue;
	}
	
    // Create progress dialog
    CProgressDlg *ptrProgress = NULL;

	try{	ptrProgress = new CProgressDlg;	}
	catch(std::bad_alloc&){ return E_OUTOFMEMORY; }

	if(NULL == ptrProgress)
		return E_OUTOFMEMORY;

    ptrProgress->Create(pWindow);
    ptrProgress->SetRange(0, m_nAcceptedFiles );

    // Load the status text string
    szStatusText.LoadString(IDS_PROGRESS_CREATING_PACKAGE);
    ptrProgress->SetStatus(szStatusText);

	//Initialize class loaders to create the ISndSubmission and ISndSample objects.
	sr = m_objSndSubLoader.Initialize();
	if(SYM_FAILED(sr))
	{
		hrRetVal = E_UNABLE_TO_INITIALIZE;
		goto Exit_Function;
	}

	sr = m_objSndSampleLoader.Initialize();
	if(SYM_FAILED(sr))
	{
		hrRetVal = E_UNABLE_TO_INITIALIZE;
		goto Exit_Function;
	}

    // Create a unique filename in the Quarantine TEMP directory.
    hrRetVal = CreateNewFileInQuarantineTemp( _T(".zip"), m_szPackageFilename, MAX_QUARANTINE_FILENAME_LEN );
    if( FAILED( hrRetVal ) )
    {
        hrRetVal = E_FILE_CREATE_FAILED;
        goto Exit_Function;
    }

	//Instantiate ISNDSubmission interface
	sr = m_objSndSubLoader.CreateObject(&m_pISNDSubmission);
	if(SYM_FAILED(sr) || !m_pISNDSubmission)
	{
		hrRetVal = E_UNABLE_TO_START_PACKAGING_CLASS;
		goto Exit_Function;
	}	
	
    // Set the package name and prepare it for info.
    hrRetVal = m_pISNDSubmission->Reset( T2A(m_szPackageFilename));
    if( FAILED(hrRetVal) )
    {
        hrRetVal = E_UNABLE_TO_START_PACKAGING_CLASS;
        goto Exit_Function;
    }

	hrRetVal = SetSubmissionInfoFields(m_pISNDSubmission);
	if(FAILED(hrRetVal))
		goto Exit_Function;

    // Find out if the user elected to strip the content from files.
    bStripFiles = g_ConfigInfo.GetStripFileContent( );

    // If we want to strip the files, then create the strip file object
    if( TRUE == bStripFiles && NULL == m_pStripper)
    {
        // Allocate the new class
        m_pStripper = CContentStripperFactory::CreateInstance();
        if( NULL == m_pStripper)
        {
            hrRetVal = E_OUTOFMEMORY;
            goto Exit_Function;
        }

        // Initialize the content stripper
        stContentInitStatus = m_pStripper->StartUp( m_sStartupFolder.GetBuffer(0) );
        if( INIT_NO_ERROR != stContentInitStatus )
        {
            hrRetVal = E_UNABLE_TO_CREATE_FILE_STRIPPER;
            goto Exit_Function;
        }
   }

    // Add all the files to the package.
    // Loop through each accepted Session, and add items to the package
    for( i = 0; i < m_nAcceptedFiles; i++ )
    {
		IQuarantineItem2		*pQItem		= NULL;
        IQuarantineSession	*pQSession = m_AcceptedFileInterfacePointers[i];

		if(NULL == pQSession)
			continue;
		//Get the info details of the each sample and create a sample
		sr = m_objSndSampleLoader.CreateObject(&ptrISample);
		if(SYM_FAILED(sr) || !m_pISNDSubmission)
		{
			hrRetVal = E_UNABLE_TO_START_PACKAGING_CLASS;
			goto Exit_Function;
		}
        
		hrRetVal = SetSampleInfoFields(ptrISample,pQSession);
		if(FAILED(hrRetVal))
		{
			ptrISample.Release();
			continue;
		}

		hrRetVal = pQSession->GetItemCount(&dwItemCnt);
		if(FAILED(hrRetVal))
		{
			hrRetVal = E_UNABLE_TO_GET_QSESSION_INFO;
			goto Exit_Function;
		}

		CStringArray *pArray = m_AcceptedFileNames.GetAt(i);
		if(NULL == pArray)
		{
			continue;
		}
		int iRemedIdx = 0;
		//Loop through the session items and add Accepted Files to the Sample
		for(DWORD dwItemIdx = 0; dwItemIdx < dwItemCnt; dwItemIdx++)
		{			
			VBININFO	stVBINInfo;			

			hrRetVal = pQSession->GetItem(dwItemIdx, &pQItem);
			if(FAILED(hrRetVal) || NULL == pQItem)
			{
				continue;
			}

			hrRetVal = pQItem->GetItemInfo(&stVBINInfo);
			if(FAILED(hrRetVal))
			{
				continue;
			}

			switch(stVBINInfo.dwRecordType)
			{
				case VBIN_RECORD_LEGACY_INFECTION:
					{
						TCHAR szStrippedFile[MAX_QUARANTINE_FILENAME_LEN];

						CString strTmp = pArray->GetAt(dwItemIdx);
						//If the user has selected for File Content Stripping
						if( TRUE == bStripFiles )
						{
							hrRetVal = StripFileContent(strTmp,szStrippedFile,MAX_QUARANTINE_FILENAME_LEN);
							if(FAILED(hrRetVal))
							{
								hrRetVal = E_UNABLE_TO_STRIP_FILE;
								goto Exit_Function;
							}
							m_strDeletableFilesArray.Add(szStrippedFile);
							//Fix for 1-3HQ0NR - Use Description field if FullPathLFN field is empty
							if(NULL != stVBINInfo.FullPathAndLFN && NULL != stVBINInfo.FullPathAndLFN[0])
								sr = ptrISample->AddSampleFile(szStrippedFile,
															stVBINInfo.FullPathAndLFN);
							else
								sr = ptrISample->AddSampleFile(szStrippedFile,
															stVBINInfo.Description);

						}
						else
						{
							//Fix for 1-3HQ0NR - Use Description field if FullPathLFN field is empty
							if(NULL != stVBINInfo.FullPathAndLFN && NULL != stVBINInfo.FullPathAndLFN[0])
								sr = ptrISample->AddSampleFile((LPCSTR)strTmp,
															stVBINInfo.FullPathAndLFN);							
							else
								sr = ptrISample->AddSampleFile((LPCSTR)strTmp,
															stVBINInfo.Description);	
						}
						if(SYM_FAILED(sr))
						{
							hrRetVal = E_UNABLE_TO_ADD_FILE_TO_SAMPLE;
							goto Exit_Function;
						}
					}
					break;

				case VBIN_RECORD_REMEDIATION:
					{						
						//Remediation file name stored as Remediation Action\Undo in the Accepted File Names Array.
						//Seperate the 2 files names and add to sample
						CString szRemedActionUndo = pArray->GetAt(dwItemIdx);
						TCHAR szRemedAction[MAX_QUARANTINE_FILENAME_LEN],szRemedUndo[MAX_QUARANTINE_FILENAME_LEN];
						
						if(szRemedActionUndo.IsEmpty())
							continue;					

						szRemedAction[0] =szRemedUndo[0] = 0x00;
						_tcscpy(szRemedAction, (LPCTSTR)szRemedActionUndo);
						_tcscpy(szRemedUndo, (LPCTSTR)szRemedActionUndo);

						//Rename the Remediation File with Remediation Action and Undo file extensions
						PathRenameExtension(szRemedAction,_T(".RMA"));
						PathRenameExtension(szRemedUndo,_T(".URM"));

						hrRetVal = pQItem->GetRemediationAction((LPCSTR)szRemedActionUndo,T2A(szRemedAction));
						if(FAILED(hrRetVal))
							goto Exit_Function;						

						hrRetVal = pQItem->GetRemediationUndo((LPCSTR)szRemedActionUndo,T2A(szRemedUndo));						

						if(PathFileExists((LPCTSTR)szRemedAction))
						{
							//Add the Filename to the Deletable Temp Files List
							m_strDeletableFilesArray.Add(szRemedAction);

							sr = ptrISample->AddRemedAction((LPCSTR)szRemedAction, iRemedIdx);						

							if(SYM_FAILED(sr))
							{
								hrRetVal = E_UNABLE_TO_ADD_FILE_TO_SAMPLE;
								goto Exit_Function;
							}

							if(PathFileExists((LPCTSTR)szRemedUndo))
							{
								//Add the Filename to the Deletable Temp Files List
								m_strDeletableFilesArray.Add(szRemedUndo);

								//If the user has selected for File Content Stripping
								if( TRUE == bStripFiles )
								{
									if(ccEraser::FileRemediationActionType == stVBINInfo.dwRemediationType || 
										ccEraser::InfectionRemediationActionType == stVBINInfo.dwRemediationType)
									{
										//Strip the file content if the remediation undo cotains a file infection or file remediation
										//StripRemediationUndoFileContent call will strip the file content with CContentStripper and
										//will replace the Undo file with the stripped file.
										//So no need to add the stripped file to list of stripped files
										hrRetVal = StripRemediationUndoFileContent(szRemedUndo);
										if(FAILED(hrRetVal))
										{
											hrRetVal = E_UNABLE_TO_STRIP_FILE;
											continue;;
										}
									}
								}
								
								sr = ptrISample->AddRemedUndo(szRemedUndo, iRemedIdx);
								if(SYM_FAILED(sr))
								{
									hrRetVal = E_UNABLE_TO_ADD_FILE_TO_SAMPLE;
									goto Exit_Function;
								}
							}
						}
						iRemedIdx++;
					}
					break;

				case VBIN_RECORD_SYSTEM_SNAPSHOT:
					{
						//If user has chosen for sending additional information, then add snapshot.
						if(g_bSubmitAdditionalInfo)
						{
							CString strTmp = pArray->GetAt(dwItemIdx);
							sr = ptrISample->AddSnapShot((LPCSTR)strTmp);				
							if(SYM_FAILED(sr))
							{
								hrRetVal = E_UNABLE_TO_ADD_FILE_TO_SAMPLE;
								goto Exit_Function;
							}
						}
					}
					break;
			}	//End of Switch			
		}	//End of Items For Loop

		sr = m_pISNDSubmission->SubmitSample(ptrISample);
		if(SYM_FAILED(sr))
		{
			hrRetVal = E_UNABLE_TO_ADD_SAMPLE_TO_SUB;
			goto Exit_Function;
		}		

        // Advance progress dialog
        ptrProgress->SetPos( i );

        if( ptrProgress->CheckCancelButton() )
        {
            hrRetVal = E_USER_CANCELLED;
            goto Exit_Function;
        }
    }	//End of Sessions For loop

    ptrProgress->SetRange(0, 100 );

    // Load the status text string
    szStatusText.LoadString(IDS_PROGRESS_COMPRESSING);
    ptrProgress->SetStatus(szStatusText);

    // Reset the position to 0%
    ptrProgress->SetPos( 0 );
	
	ptrSNDProgress = ptrProgress;
    // Zip up the package
    sr = m_pISNDSubmission->Finish(ptrSNDProgress, 0, 0, &m_lpbyBuff,&stField);
	
	if(SYM_FAILED(sr))
	{
		hrRetVal = E_UNABLE_TO_FINISH_PACKAGE;
        goto Exit_Function;
	}    

    // Open the package file
    hPackage = CreateFile(m_szPackageFilename,
                          GENERIC_READ,
                          0,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);

    if( INVALID_HANDLE_VALUE == hPackage )
    {
        hrRetVal = E_FILE_CREATE_FAILED;
        goto Exit_Function;
    }

    // get the filesize
    dwLowSize = GetFileSize( hPackage, &dwHighSize );
    // If there are more that 10 MB of files, return failure
    if( ( dwHighSize > 0 ) ||
		( dwLowSize > 10 * 1024 * 1024 ) )
    {
        hrRetVal = E_PACKAGE_OVER_MAX_SUBMISSION_SIZE;
        goto Exit_Function;
    }

    // Close the file handle
    if( FALSE == CloseHandle(hPackage) )
    {
        hrRetVal = E_FILE_CLOSE_FAILED;
        goto Exit_Function;
    }
    else
    {
        hPackage = INVALID_HANDLE_VALUE;
    }

    // This is the code to use if SARC fixes GetTotalFileSizeCount.
    // Otherwise, we can use the code above to check the compressed package file size.
    // Now make sure that the submission size is < 10 MegaBytes
    hrRetVal = m_pISNDSubmission->GetTotalFileSizeCount( &dwFileSize, &dwNumberFiles );
    if( FAILED(hrRetVal))
    {
        hrRetVal = E_UNABLE_TO_FINISH_PACKAGE;
        goto Exit_Function;
    }

    // If there are more that 10 MB of files, return failure
    if( dwFileSize > 10 * 1024 * 1024 )
        {
        hrRetVal = E_UNABLE_TO_FINISH_PACKAGE;
        goto Exit_Function;
        }

	if(ptrSNDProgress)
		ptrSNDProgress.Release();
	else
	{
		if(ptrProgress)
		{// Remove progress window
			ptrProgress->DestroyWindow();
			delete ptrProgress;
			ptrProgress = NULL;
		}
	}

    // If we have made it here, then we can safely return S_Ok
    return S_OK;

Exit_Function:
	
	if(ptrSNDProgress)
		ptrSNDProgress.Release();
	else	//Progress Dialog ptrProgress is not yet assigned to ptrSNDProgress. So destroy the progress window
	{
		if(ptrProgress)
		{// Remove progress window
			ptrProgress->DestroyWindow();
			delete ptrProgress;
			ptrProgress = NULL;
		}
	}

    // Close the file handle if it is open.
    if( INVALID_HANDLE_VALUE != hPackage )
    {
        CloseHandle(hPackage);
    }
    // return
    return hrRetVal;
}

STDMETHODIMP CScanDeliverDLL::PingSARC(HWND hWnd)
{
    DWORD   lpExitCode;
    DWORD   dwSARCThreadId, dwSARCThreadParam = 1;
    HRESULT hrRetVal = S_OK;
    CProgressDlg progress;
    CString szStatusText;
    BOOL    bDone = FALSE;
    DWORD   dwResult = 0;

    CWnd *pWindow = CWnd::FromHandle( hWnd );
    if( NULL == pWindow )
    {
        hrRetVal = E_INVALIDARG;
    }
    else
    {
        // Create progress dialog
        progress.Create(pWindow);
        progress.SetRange(0, 100);

        // Load the status text string
        szStatusText.LoadString(IDS_PROGRESS_PING_SARC);
        progress.SetStatus(szStatusText);

        m_hSARCThread = CreateThread(NULL, 0, SARCPingThreadLinkFunc, this, 0, &dwSARCThreadId);
        if (m_hSARCThread == NULL) 
			return E_UNABLE_TO_CREATE_THREAD;

		// This loop waits for either the thread to finish or for the user
		// to abort the process by pressing a Cancel button
        while (!bDone)
        {
            GetExitCodeThread(m_hSARCThread, &lpExitCode);
            if (STILL_ACTIVE != lpExitCode)
            {
				hrRetVal = m_hResult;
                bDone = TRUE;
            }
            else
            {
                if( progress.CheckCancelButton() )
                {
                    // Terminate the thread
                    TerminateThread(m_hSARCThread, 0);

                    hrRetVal = E_USER_CANCELLED;
                    bDone = TRUE;
                }
            }
        }
    }

    // Destroy the progress dialog
    progress.DestroyWindow();
	
	CloseHandle( m_hSARCThread );

    return hrRetVal;
}


DWORD WINAPI SARCPingThreadLinkFunc(LPVOID pParam)
{
    return (((CScanDeliverDLL*)pParam)->SARCPingThreadFunc());
}


// ==== SARCThreadFunc ====================================================
//
//  This thread function is responsible for testing the connection to SARC
//
//  Input:  nothing
//
//  Output: nothing
//
// ========================================================================
//  Function created: 1/99 Mark Mavromatis
// ========================================================================

DWORD CScanDeliverDLL::SARCPingThreadFunc() 
{
    TCHAR   szInfoBuf[MAX_INI_BUF_SIZE] = "";

	// If the config file indicates to bypass HTTP, only try to connect to SARC 
    // using the SMTP protocol

    if (g_ConfigInfo.GetBypassHTTP())
    {
		m_hResult = TriggerAutoDial();
    }
    else
    {
        // Try to connect via HTTP
        m_hResult = CheckBackendStatus();

        if (E_BACKEND_UNABLE_TO_CONNECT_TO_INTERNET == m_hResult)
	    {
		    // Try to connect via SMTP
		    m_hResult = TriggerAutoDial();
	    }
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::SendToSarc()
//
// Description      : This function takes the package generated by
//                    CreatePackage and sends it to SARC.
//
//   This function is based on the SMTP delivery code from Env Newvir
//   proj sarccan.
//
// Return type      : TRUE for success
//                    FALSE otherwise
//
////////////////////////////////////////////////////////////////////////////
// 4/19/98 JTAYLOR - Function created / Header added.
// 4/20/98 JTAYLOR - Added more error handling
// 4/21/98 JTAYLOR - Made the mime encoded file be a temporary file.
//                   Added delete call for the temporary file.
// 4/23/98 JTAYLOR - Moved temp file creation to its own function
// 4/30/98 JTAYLOR - Added progress dialog and more error handling
// 4/30/98 JTAYLOR - Do not finish email if the user hits cancel.
// 5/19/98 JTAYLOR - Use the SMTP server and email address from the
//                   ScanDeliverConfig options.
// 5/19/98 JTAYLOR - Fixed an error in the error handling. Goto Exit_Function
//                   inside Exit_Function (infinite loop)  :(
// 7/01/98 JTAYLOR - Added text to the email body.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::SendToSarc( HWND hWnd )
{
    WSADATA             stWSADATA;
    int                 nErr;
    DWORD               dwError;
    SOCKET_ADDRESS_T    stSocketAddress;
    SMTP_CONNECTION_T   stSMTPConnection;
    BOOL                bResult;
    char                szBuf[513];
    DWORD               dwLength;
    DWORD               dwBytesSent;
    int                 nBytesToRead;
    TCHAR               szMailServer[MAX_QUARANTINE_FILENAME_LEN]   = "";
    TCHAR               szSender[MAX_QUARANTINE_FILENAME_LEN]       = "";
    TCHAR               szRecipient[MAX_QUARANTINE_FILENAME_LEN]    = "";
    TCHAR               szMIMEFileName[MAX_QUARANTINE_FILENAME_LEN] = "";
    HANDLE              hPackage                                    = INVALID_HANDLE_VALUE;
    HRESULT             hrRetVal                                    = E_FAIL;
    CString             szStatusText;
    CString             szEmailText;
    CString             szBoundryString;
    CFile               fInput;
    CFile               fOutput;
    CProgressDlg        progress;
    CString             szHeader;

    CWnd *pWindow = CWnd::FromHandle( hWnd );
    if( NULL == pWindow )
        {
        hrRetVal = E_INVALIDARG;
        goto Exit_Function;
        }

    // Create progress dialog
    progress.Create(pWindow);
    progress.SetRange(0, 100 );
    // Load the status text string
    szStatusText.LoadString(IDS_PROGRESS_PREPARING);
    progress.SetStatus(szStatusText);

    // Load the string for the message body
    szEmailText.LoadString(IDS_EMAIL_BODY);

    // Get the email address of the sender
    g_ConfigInfo.GetEmail( szSender, MAX_QUARANTINE_FILENAME_LEN );

    // Get the SMTP server to send to.
    g_ConfigInfo.GetSmtpServer( szMailServer, MAX_QUARANTINE_FILENAME_LEN );

    // Get the SARC email address to send to.
    g_ConfigInfo.GetSarcEmailAddress( szRecipient, MAX_QUARANTINE_FILENAME_LEN );

    // Start up winsock
    nErr = WSAStartup(0x0101,&stWSADATA);

    if (nErr != 0)
        {
        hrRetVal = E_FAIL;
        goto Exit_Function;
        }

    bResult = TRUE;

    // Start the SMTP session

    if (SocketAddressFill(&stSocketAddress,
                          szMailServer,
                          25,
                          &dwError) != SOCKET_STATUS_OK)
        bResult = FALSE;

    // Load the status text string
    szStatusText.LoadString(IDS_PROGRESS_INITIALIZING);
    progress.SetStatus(szStatusText);

    if (bResult == TRUE &&
        SMTPInitiateMailConnection(&stSMTPConnection,
                                   &stSocketAddress,
                                   szSender) != SMTP_STATUS_OK)
        bResult = FALSE;

    if (bResult == TRUE &&
        SMTPConnectionAddRecipient(&stSMTPConnection,
                                   szRecipient) != SMTP_STATUS_OK)
        bResult = FALSE;

    if (bResult == TRUE &&
        SMTPConnectionStartData(&stSMTPConnection) != SMTP_STATUS_OK)
        bResult = FALSE;

    // Send the subject
    if (bResult == TRUE &&
        SMTPConnectionSendData(&stSMTPConnection,
                               "Subject: Scan and Deliver package\r\n"
                               "MIME-Version: 1.0\r\n"
                               "Content-Type: multipart/mixed; boundary=\"") !=
        SMTP_STATUS_OK)
        bResult = FALSE;

    // Generate a unique boundry string for the message
    // seed the random number generator
    srand( (unsigned)time( NULL ) );

    // Make sure that a DWORD stays defined as 32 bits.
    SPRINTF( szBoundryString.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ),
             _T("%.8X%.8X%.8X"),
             MAKELONG(rand(),rand()) & 0xFFFFFFFF,
             MAKELONG(rand(),rand()) & 0xFFFFFFFF,
             MAKELONG(rand(),rand()) & 0xFFFFFFFF );

    // Free the buffer
    szBoundryString.ReleaseBuffer();

    // Append 12 '-' to the head of the boundry string
    szBoundryString = "------------" + szBoundryString;

    szHeader.Empty();
    szHeader += szBoundryString;
    szHeader += "\"\r\n\r\n";
    szHeader += "This is a multi-part message in MIME format.\r\n\r\n--";
    szHeader += szBoundryString;
    szHeader += "\r\nContent-Type: text/plain; charset=us-ascii\r\n";
    szHeader += "Content-Transfer-Encoding: 7bit\r\n\r\n";
    szHeader += szEmailText;
    szHeader += "\r\n\r\n--";
    szHeader += szBoundryString;
    szHeader += "\r\nContent-Transfer-Encoding: base64\r\n";
    szHeader += "Content-Description: Zip compressed file\r\n";
    szHeader += "Content-Disposition: attachment; filename=\"SUBMIT.zip\"\r\n";
    szHeader += "Content-Type: application/zip; name=\"SUBMIT.ZIP\"\r\n";

    // Send the subject
    if (bResult == TRUE &&
        SMTPConnectionSendData(&stSMTPConnection,
                            szHeader.GetBuffer(0) ) !=
        SMTP_STATUS_OK)
        bResult = FALSE;

    szHeader.ReleaseBuffer();

    // Open a file to the package.
    if (fInput.Open(m_szPackageFilename,
                    CFile::modeRead |
                    CFile::shareExclusive,
                    NULL) == 0)
        {
        hrRetVal = E_FILE_CREATE_FAILED;
        goto Exit_Function;
        }

    // Create a unique filename in the Quarantine TEMP directory.
    hrRetVal = CreateNewFileInQuarantineTemp( _T(".zip"), szMIMEFileName, MAX_QUARANTINE_FILENAME_LEN );
    if( FAILED( hrRetVal ) )
        {
        hrRetVal = E_FILE_CREATE_FAILED;
        goto Exit_Function;
        }

    // Open a the mime encoded file.
    if (fOutput.Open(szMIMEFileName,
                     CFile::modeReadWrite  |
                     CFile::shareExclusive |
                     CFile::modeCreate,
                     NULL) == 0)
        {
        hrRetVal = E_FILE_CREATE_FAILED;
        goto Exit_Function;
        }

    // Mime encode the input file.
    MIMEEncodeZip(&fInput, &fOutput);

    // Load the status text string
    szStatusText.LoadString(IDS_PROGRESS_TRANSMITTING);
    progress.SetStatus(szStatusText);

    if (bResult == TRUE)
    {
        TRY
        {
            // Get the attachment size

            dwLength = fOutput.GetLength();

            sssnprintf(szBuf,sizeof(szBuf),"Content-Length: %lu\r\n\r\n",dwLength);

            if (SMTPConnectionSendData(&stSMTPConnection,
                                       szBuf) != SMTP_STATUS_OK)
                bResult = FALSE;

            fOutput.SeekToBegin();

            nBytesToRead = 512;
            dwBytesSent = 0;
            while (dwBytesSent < dwLength)
            {
                if ((dwLength - dwBytesSent) < (DWORD)nBytesToRead)
                    nBytesToRead = (int)(dwLength - dwBytesSent);

                if (fOutput.Read(szBuf,nBytesToRead) != (DWORD)nBytesToRead)
                {
                    bResult = FALSE;
                    break;
                }

                szBuf[nBytesToRead] = 0;

                if (SMTPConnectionSendData(&stSMTPConnection,
                                           szBuf) != SMTP_STATUS_OK)
                {
                    bResult = FALSE;
                    break;
                }

                dwBytesSent += nBytesToRead;

                // Advance progress dialog
                progress.SetPos( dwBytesSent * 100 / dwLength );

                // If the user has hit cancel, then exit.
                if( progress.CheckCancelButton() )
                    {
                    hrRetVal = E_USER_CANCELLED;
                    bResult = FALSE;  // do not finish the email.
                    goto Exit_Function;
                    }
                }
        }
        CATCH(CFileException, e)
        {
            bResult = FALSE;
        }
        END_CATCH

        // terminate the attachment section.
        szHeader.Empty();
        szHeader += "--";
        szHeader += szBoundryString;
        szHeader += "--";

        // Send the subject
        if (bResult == TRUE &&
            SMTPConnectionSendData(&stSMTPConnection,
                                szHeader.GetBuffer(0) ) !=
            SMTP_STATUS_OK)
            bResult = FALSE;

        szHeader.ReleaseBuffer();
        }

    hrRetVal = S_OK;

Exit_Function:

    if (bResult == TRUE &&
        SMTPFinishMailConnection(&stSMTPConnection) != SMTP_STATUS_OK)
        bResult = FALSE;

    WSACleanup();

    // If the files are open, then close them.
    if( _tcslen(fInput.GetFileName()) != 0 )
        {
        fInput.Close();
        }

    if( _tcslen(fOutput.GetFileName()) != 0 )
        {
        fOutput.Close();
        }

    // remove the temporary mime file.
    if( _tcslen( szMIMEFileName ) > 0 )
        {
        DeleteFile( szMIMEFileName );
        }

    // Destroy the progress dialog
    progress.DestroyWindow();

    if( FALSE == bResult )
        {
        hrRetVal = E_FAIL;
        }

    // If the package was successfully sent, mark the package as sent.
    // This will be used to determine if we should update the status on the
    // files that were selected for submission or not.
    if( SUCCEEDED( hrRetVal ) )
        {
        m_bPackageSent = TRUE;
        }

    return hrRetVal;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::TriggerAutoDial()
//
// Description      : This function attempts to create a socket connection
//                    to trigger AutoDial in IE4.  This will only work once
//                    per process.
//
//   This function is based on the SMTP delivery code from Env Newvir
//   proj sarccan.
//
// Return type      : S_OK      for success
//                    E_FAIL    otherwise
//
////////////////////////////////////////////////////////////////////////////
// 6/10/98 JTAYLOR - Initial revision.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::TriggerAutoDial( )
{
    WSADATA             stWSADATA;
    int                 nErr;
    DWORD               dwError;
    SOCKET_ADDRESS_T    stSocketAddress;
    SMTP_CONNECTION_T   stSMTPConnection;
    BOOL                bResult;
    TCHAR               szMailServer[MAX_QUARANTINE_FILENAME_LEN]   = "";
    TCHAR               szSender[MAX_QUARANTINE_FILENAME_LEN]       = "";
    TCHAR               szRecipient[MAX_QUARANTINE_FILENAME_LEN]    = "";
    HRESULT             hrRetVal                                    = E_FAIL;

    // Get the email address of the sender
    g_ConfigInfo.GetEmail( szSender, MAX_QUARANTINE_FILENAME_LEN );

    // Get the SMTP server to send to.
    g_ConfigInfo.GetSmtpServer( szMailServer, MAX_QUARANTINE_FILENAME_LEN );

    // Get the SARC email address to send to.
    g_ConfigInfo.GetSarcEmailAddress( szRecipient, MAX_QUARANTINE_FILENAME_LEN );

    // Start up winsock
    nErr = WSAStartup(0x0101,&stWSADATA);
    if (nErr != 0)
        {
        hrRetVal = E_FAIL;
        goto Exit_Function;
        }

    bResult = TRUE;

    // Start the SMTP session
    if (SocketAddressFill(&stSocketAddress,
                          szMailServer,
                          25,
                          &dwError) != SOCKET_STATUS_OK)
        bResult = FALSE;

    if (bResult == TRUE &&
        SMTPInitiateMailConnection(&stSMTPConnection,
                                   &stSocketAddress,
                                   szSender) != SMTP_STATUS_OK)
        bResult = FALSE;

    if( TRUE == bResult )
        {
        hrRetVal = S_OK;
        }
    else
        {
        hrRetVal = E_BACKEND_UNABLE_TO_CONNECT_TO_INTERNET;
        }

Exit_Function:

    if (bResult == TRUE &&
        SMTPFinishMailConnection(&stSMTPConnection) != SMTP_STATUS_OK)
        bResult = FALSE;

    WSACleanup();

    return hrRetVal;
}

//*************************************************************************
//
// Function:
//  void CScanDeliverDLL::MIMEEncode64TriToQuad()
//
// Parameters:
//  lpbyTri: IN=Array of 3, 8-bit bytes to be encoded
//  lpszQuad: OUT=Array of 4, valid BASE64 characters to be outputted
//
// Description:
//  This function encodes 3 8-bit bytes into 4 valid BASE64 bytes.
//
//   This function is taken directly from Env Newvir proj sarccan.
//
// Returns:
//  Nothing
//
//************************************************************************

void CScanDeliverDLL::MIMEEncode64TriToQuad
(
    LPBYTE              lpbyTri,
    LPSTR               lpszQuad
)
{
    BYTE        byQuad[4];
    int         i;

    byQuad[0] = lpbyTri[0] >> 2;
    byQuad[1] = ((lpbyTri[0] & 0x3) << 4) | (lpbyTri[1] >> 4);
    byQuad[2] = ((lpbyTri[1] & 0xf) << 2) | (lpbyTri[2] >> 6);
    byQuad[3] = lpbyTri[2] & 0x3F;

    for (i=0;i<4;i++)
    {
        if (byQuad[i] <= 25)
            lpszQuad[i] = byQuad[i] + 'A';
        else if (byQuad[i] >= 26 && byQuad[i] <= 51)
            lpszQuad[i] = byQuad[i] + 'a' - 26;
        else if (byQuad[i] >= 52 && byQuad[i] <= 61)
            lpszQuad[i] = byQuad[i] + '0' - 52;
        else if (byQuad[i] == 62)
            lpszQuad[i] = '+';
        else if (byQuad[i] == 63)
            lpszQuad[i] = '/';
    }
}


//*************************************************************************
//
// Function:
//  BOOL CScanDeliverDLL::MIMEEncodeZip(void)
//
// Parameters:
//  None
//
// Description:
//  This function encodes the specified input stream as a BASE64 attachment
//  and writes the output to the current write pointer in the lpstOutput
//  stream.  The calling function should make sure to seek to the proper
//  location in the output stream before invoking this function.
//
//   This function is taken directly from Env Newvir proj sarccan.
//
// Returns:
//  TRUE        On successful encoding
//  FALSE       If an error occurs during encoding
//
//*************************************************************************

BOOL CScanDeliverDLL::MIMEEncodeZip(CFile *pInput, CFile *pOutput)
{
#define BASE64_LINE_LEN 72

    char                szLine[BASE64_LINE_LEN+1];
    BYTE                byTri[3];
    BYTE                byChar;
    int                 nCount, nStringIndex;
    DWORD               dwZipLen;
    DWORD               dwCount;
    BYTE                abyBuf[512];
    int                 nBufIndex;

    TRY
    {
        // Seek to start of input file

        pInput->SeekToBegin();

        // Seek to start of output file

        pOutput->SetLength(0);
        pOutput->SeekToBegin();

        nCount = nStringIndex = 0;
        byTri[0] = byTri[1] = byTri[2] = 0;

        nBufIndex = sizeof(abyBuf);
        dwZipLen = pInput->GetLength();
        for (dwCount=0;dwCount<dwZipLen;dwCount++)
        {
            if (nBufIndex == sizeof(abyBuf))
            {
                // Refill the buffer

                if (dwZipLen - dwCount < sizeof(abyBuf))
                {
                    if (pInput->Read(abyBuf,dwZipLen-dwCount) != dwZipLen - dwCount)
                        return(FALSE);
                }
                else
                {
                    if (pInput->Read(abyBuf,sizeof(abyBuf)) != sizeof(abyBuf))
                        return(FALSE);
                }

                nBufIndex = 0;
            }

            byChar = abyBuf[nBufIndex++];
            //if (pInput->Read(&byChar,1) != 1)
            //  return(FALSE);

            // create sets of 3 characters

            byTri[nCount++] = byChar/* ^ 0xCD*/;

            if (nCount == 3)
            {
                // encode the 3 into 4 valid BASE64 characters and add them
                // to our output line

                MIMEEncode64TriToQuad(byTri,szLine+nStringIndex);

                nCount = 0;
                nStringIndex += 4;
                byTri[0] = byTri[1] = byTri[2] = 0;

                if (nStringIndex == BASE64_LINE_LEN)
                {
                    // Once we have reached the proper line length, output the
                    // BASE64 line to the output file.

                    szLine[nStringIndex++] = '\r';
                    szLine[nStringIndex++] = '\n';
                    pOutput->Write(szLine,nStringIndex);

                    // time to start on the next line

                    nCount = nStringIndex = 0;

                    // Update the progress

/*                    if (m_lpfnProgressCB != NULL &&
                        m_lpfnProgressCB(m_lpvProgressCookie,
                                         eSARCSubmitStageMIME,
                                         dwCount * 100 / dwZipLen,
                                         NULL) == FALSE)
                        return(FALSE);
*/
                }
            }
        }

        // see if we have a partially full byTri (< 3 bytes).  add it to the
        // current line.

        if (nCount != 0)
        {
            MIMEEncode64TriToQuad(byTri,szLine+nStringIndex);
            if (nCount == 1)
                nStringIndex += 2;
            else if (nCount == 2)
                nStringIndex += 3;
        }

        // if there is a non-empty last line, write it out

        if (nStringIndex != 0)
        {
            while (nStringIndex % 4 != 0)
                szLine[nStringIndex++] = '=';
            szLine[nStringIndex++] = '\r';
            szLine[nStringIndex++] = '\n';
            pOutput->Write(szLine,nStringIndex);
        }
    }
    CATCH(CFileException, e)
    {
        return(FALSE);
    }
    END_CATCH

    return(TRUE);
}


////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::CreateNewFileInQuarantineTemp
//
// Description      : This function takes an extension and creates a new
//                    file in the Quarantine temp path.  The new file
//                    is then closed and the filename is returned.
//
// Arguments        : lpszFilename    -- This filename will be used to retrieve the
//                                       extension of the new file.
//                    lpszNewFilename -- [out]This is the name of the new filename
//                    uBufferSize     -- This is the size in characters of the output
//                                       buffer.
// Return type      : S_OK for success.
//
////////////////////////////////////////////////////////////////////////////
// 4/19/98 JTAYLOR - Function created / Header added.
// 4/20/98 JTAYLOR - Added more error handling
// 4/21/98 JTAYLOR - Made the mime encoded file be a temporary file.
//                   Added delete call for the temporary file.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::CreateNewFileInQuarantineTemp( LPCTSTR lpszFilename, LPTSTR lpszNewFilename, UINT uBufferSize )
{
    BOOL    bResult                                               = FALSE;
    TCHAR    szTempFile[MAX_QUARANTINE_FILENAME_LEN]               = "";
    TCHAR    szTempExtension[MAX_QUARANTINE_EXTENSION_LENGTH]      = "";
    HANDLE  hPackage                                              = INVALID_HANDLE_VALUE;


    // get the extension of the file to strip
    GetFileExtension( lpszFilename, szTempExtension, MAX_QUARANTINE_EXTENSION_LENGTH );

    // Create a unique filename in the Quarantine TEMP directory.
    hPackage = SNDCreateUniqueFile( m_sTempFolder.GetBuffer(0), szTempExtension, szTempFile);
    if( INVALID_HANDLE_VALUE == hPackage )
        {
        return E_FILE_CREATE_FAILED;
        }
    else
    {
        // Make sure the file will fit in the input buffer.
        if( _tcslen( szTempFile ) > uBufferSize )
            {
            CloseHandle(hPackage);
            WipeOutFile( szTempFile );
            return E_UNABLE_TO_MANIPULATE_STRING;
            }

        // Copy the filename to the argument
        _tcscpy( lpszNewFilename, szTempFile );

        // Close the file since the caller expects it to be closed.
        if( FALSE == CloseHandle(hPackage) )
        {
            return E_FILE_CLOSE_FAILED;
        }
        else
        {
            hPackage = INVALID_HANDLE_VALUE;
        }
    }

    return S_OK;

}


////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::CheckBackendStatus
//
// Description      : This function contacts the SARC socket server
//                    and determines the status of the SMTP server.
//
// Return type      : S_BACKEND_SEND_PACKAGE for send package.
//                    E_UNABLE_TO_GET_BACKEND_STATUS if error or undefined return
//                    E_BACKEND_UNABLE_TO_CONNECT_TO_INTERNET if unable to get to sarcscan or www.symantec.com
//                    S_BACKEND_BUSY if the server is busy
//                    S_BACKEND_FALSE_POSITIVE if the package is all false positives
//                    S_BACKEND_ALL_CLEAN if the package contains only files that do not have viruses.
////////////////////////////////////////////////////////////////////////////
// 4/24/98 JTAYLOR - Function created / Header added.
// 5/06/98 JTAYLOR - Added real socket server address.  Added correct
//                   priority.
// 5/20/98 JTAYLOR - Added support for backend all clean.
// 5/20/98 JTAYLOR - Added support for E_UNABLE_TO_CONNECT_TO_INTERNET
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::CheckBackendStatus( )
{
    int nResult = 0;
    HRESULT hrRetVal = E_FAIL;
    CString szSocketServerAddress;
    int nPriority = CUSTOMER_PRIORITY;

    // bugbug make sure that the byte buffer is initialized


    // Allocate the new class
    SocketClient *pSocket = SocketClientFactory::CreateInstance();
    if( NULL == pSocket )
        {
        return E_OUTOFMEMORY;
        }

    // Get the socket server address
    g_ConfigInfo.GetSarcSocketServer( szSocketServerAddress.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ),
                                      MAX_QUARANTINE_FILENAME_LEN );
    szSocketServerAddress.ReleaseBuffer( );

    // Set the priority to corporate if this is true
    if( g_ConfigInfo.IsCorporateMode( ) )
        {
        nPriority = CORPORATE_PRIORITY;
        }

    // Check the status of the back end.
    nResult = pSocket->GetBackEndStatus( szSocketServerAddress.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ),
                                         nPriority,
                                         m_lpbyBuff );
    szSocketServerAddress.ReleaseBuffer( );

    switch ( nResult )
        {
        case BACK_END_SEND_PACKAGE:
            hrRetVal = S_BACKEND_SEND_PACKAGE;
            break;
        case BACK_END_DATA_ERROR:
            hrRetVal = E_UNABLE_TO_GET_BACKEND_STATUS;
            break;
        case BACK_END_CONNECTION_ERROR:
            {
            // We were unable to connect to the sarc socket server, try
            // to connect to www.symantec.com to verify internet connection.
            szSocketServerAddress.LoadString( IDS_SYMANTEC_MAIN_WEB_SERVER );
            nResult = pSocket->GetBackEndStatus( szSocketServerAddress.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ),
                                                 nPriority,
                                                 m_lpbyBuff );
            szSocketServerAddress.ReleaseBuffer( );

            // If we were unable to connect to www.symantec.com, return internet connection error.
            if( BACK_END_CONNECTION_ERROR == nResult )
                {
                hrRetVal = E_BACKEND_UNABLE_TO_CONNECT_TO_INTERNET;
                }
            else
                {
                hrRetVal = E_UNABLE_TO_GET_BACKEND_STATUS;
                }
            break;
            }
        case BACK_END_BUSY_TRY_LATER:
            hrRetVal = S_BACKEND_BUSY;
            break;
        case BACK_END_FP_USE_LU:
            hrRetVal = S_BACKEND_FALSE_POSITIVE;
            break;
        case BACK_END_ALL_CLEAN:
            hrRetVal = S_BACKEND_ALL_CLEAN;
        default:
            hrRetVal = E_UNABLE_TO_GET_BACKEND_STATUS;
        }

    if( NULL != pSocket )
        {
        pSocket->Release();
        pSocket = NULL;
        }

    return hrRetVal;
}



////////////////////////////////////////////////////////////////////////////
// Function name    : IsFileOfTheDayModeValid
//
// Description      : This function checks to see if File of the Day mode is
//                    valid for the current file list.  This function should
//                    be called by a client before converting to file of the
//                    day mode.  We will not convert to FOD mode if this
//                    function does not return "FOTD_IS_VALID".
//
// Return type      : A DWORD of the possible values:
//                      -- FOTD_IS_VALID
//                      -- FOTD_UNKOWN_ERROR
//                      -- FOTD_FILE_IS_COMPRESSED
//                      -- FOTD_MORE_THAN_ONE_REJECTED_FILE
//                      -- FOTD_INVALID_TIME_LAPSE
//
////////////////////////////////////////////////////////////////////////////
// 4/23/98 SEDWARD - Function created / Header added.
// 4/26/98 JTAYLOR - Added support for corporate and returning the correct
//                   answer after the rejected file has been converted to
//                   an accepted file during file of the day mode.
// 5/19/98 SEDWARD - Function now returns a DWORD instead of BOOL to give the
//                   caller more information as to why we're not in "File
//                   Of The Day" mode
// 1/4/05  KTALINKI - Function modified for usage of Quarantine Sessions	
////////////////////////////////////////////////////////////////////////////
DWORD   CScanDeliverDLL::IsFileOfTheDayModeValid(void)
{
    auto    DWORD       dwResult = FOTD_IS_VALID;

    // are we dealing with only one rejected file?
    if ((0 < GetNumberAcceptedFiles())  ||  (1 != GetNumberRejectedFiles()))
        {
        dwResult = FOTD_MORE_THAN_ONE_REJECTED_FILE;
        goto  Exit_Function;
        }

    // is the file compressed?  get the file type...
    //auto    IQuarantineItem*        pItemPtr;
	auto    IQuarantineSession*        pItemPtr;
    //auto    HRESULT                 hResult;
    //auto    DWORD                   dwFileType;

    pItemPtr = m_RejectedFileInterfacePointers[0];
    if (NULL == pItemPtr)
    {
        dwResult = FOTD_UNKOWN_ERROR;
        goto  Exit_Function;
    }

	/* TODO - TBD - John Meade is deciding on whether to reject a item when it has atleast one compressed file 
    hResult = pItemPtr->GetFileType(&dwFileType);
    if (FAILED(hResult))
    {
        dwResult = FOTD_UNKOWN_ERROR;
        goto  Exit_Function;
    }

    // test for compressed files here
    if (dwFileType & QFILE_TYPE_COMPRESSED)
        {
        dwResult = FOTD_FILE_IS_COMPRESSED;
        goto  Exit_Function;
        }
	*/
    // if the previous tests succeed and we're running in corporate mode, always
    // return a success value (the default)
    if (TRUE == g_ConfigInfo.IsCorporateMode())
    {
        goto  Exit_Function;
    }

    // has it been at least one day since the last submission?
    if (FALSE == CheckFileOfTheDayTimeLapse())
    {
        dwResult = FOTD_INVALID_TIME_LAPSE;
        goto  Exit_Function;
    }


Exit_Function:

    return (dwResult);

}  // end of "CScanDeliverDLL::IsFileOfTheDayModeValid"

////////////////////////////////////////////////////////////////////////////
// Function name    : IsFileOfTheDayMode
//
// Description      : This function checks to see if the File of the Day mode
//                    is currently active.  This will be TRUE only after a
//                    successfull call to ConvertToFileOfTheDay.
//
// Return type      : BOOL
//
////////////////////////////////////////////////////////////////////////////
// 4/26/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL    CScanDeliverDLL::IsFileOfTheDayMode(void)
{
    return m_bInFileOfTheDayMode;
}  // end of "CScanDeliverDLL::IsFileOfTheDayMode"

////////////////////////////////////////////////////////////////////////////
// Function name    : ConvertToFileOfTheDayMode
//
// Description      : This function converts the submission process to file
//                    of the day mode.  This moves the one rejected file into
//                    the accepted file list.
//
// Return type      : BOOL
//
////////////////////////////////////////////////////////////////////////////
// 4/26/98 JTAYLOR - Function created / Header added.
// 1/03/04 KTALINKI - Modified for Quarantine Sessions ( Extended Threats)
////////////////////////////////////////////////////////////////////////////
BOOL    CScanDeliverDLL::ConvertToFileOfTheDayMode(void)
{
    auto    BOOL        bResult = TRUE;
    char szGeneratedFilename[MAX_QUARANTINE_FILENAME_LEN * 2+ 16];
    HRESULT hrRetVal = E_FAIL;
	DWORD	dwItemCnt = 0;
	CString strUnPackSesRoot = m_sTempFolder;	
	CString	strDestFile = _T("");
	VBININFO	stVBinInfo;
	CStringArray *pStrArray = NULL;
	int iRemedCnt = 0;
	int nIndex = 0;

	try{pStrArray = new CStringArray();}
	catch(std::bad_alloc&){return FALSE;}

	if(NULL == pStrArray)
		return FALSE;

    // If we are already in file of the day mode, return TRUE
    if( TRUE == m_bInFileOfTheDayMode )
    {
		delete pStrArray;
        return TRUE;
    }
	
    // If we cannot go to FOD mode return FALSE
    if (FOTD_IS_VALID != IsFileOfTheDayModeValid())
    {
		delete pStrArray;
        return FALSE;
    }
	
    //IQuarantineItem *pQuarantineItem = m_RejectedFileInterfacePointers[0];
	IQuarantineSession *pQSes = m_RejectedFileInterfacePointers[0];

	if(NULL == pQSes) {
		delete pStrArray;
		return FALSE;
	}

	// Clear the Generated Filename.  Having a non-empty value will confuse UnpackageItem
    szGeneratedFilename[0] = NULL;    
	IQuarantineItem2		*pQItem = NULL;

	//Get the VBIN id of the session
	hrRetVal = pQSes->GetSessionInfo(&stVBinInfo);
	if(FAILED(hrRetVal))
	{
		delete pStrArray;
		return FALSE;		
	}

	pStrArray->RemoveAll();

	//If there is not trailing '\' add it
	if(!strUnPackSesRoot.IsEmpty() && strUnPackSesRoot[strUnPackSesRoot.GetLength()-1]!= '\\')
		strUnPackSesRoot += "\\";

	//Append the Session id of the session to create the destination folder to unpack the items in to
	if(stVBinInfo.dwSessionID)
		strUnPackSesRoot.AppendFormat("%08lX\\", stVBinInfo.dwSessionID);
	else
		strUnPackSesRoot.AppendFormat("%08lX\\", stVBinInfo.RecordID);	//Legacy Threat

	//Create the folder if doesn't exist
	if(!PathFileExists(strUnPackSesRoot))
	{
        if(FALSE == CreateDirectory(strUnPackSesRoot, NULL))
		{
			hrRetVal = E_UNABLE_TO_CREATE_TEMP_FOLDER;
			return FALSE;
		}
		m_strDeletableFolderArray.Add(strUnPackSesRoot);
	}

	//Get the number of items in the session
	hrRetVal = pQSes->GetItemCount(&dwItemCnt);
	if(FAILED(hrRetVal))
	{
		delete pStrArray;
		return FALSE;		
	}

	//Get first item from the session
	hrRetVal = pQSes->GetFirstItem(&pQItem);
	if(FAILED(hrRetVal) || NULL == pQItem)
	{
		delete pStrArray;
		return FALSE;
	}
	
	//Loop thru all the items in the session
	for(DWORD j=0; j < dwItemCnt && NULL != pQItem; j ++)
	{
		// Clear the Generated Filename.  Having a non-empty value will confuse UnpackageItem
		szGeneratedFilename[0] = NULL;
		VBININFO	stItemInfo;
		
		ZeroMemory(&stItemInfo, sizeof(VBININFO));

		//Get the item type from the session
		hrRetVal = pQItem->GetItemInfo(&stItemInfo);
		if(FAILED(hrRetVal))
		{
			delete pStrArray;
			hrRetVal = E_UNABLE_TO_GET_QITEM;
			return FALSE;
		}
		strDestFile = strUnPackSesRoot;

		//Prepare the destination file name to unpack to, depeding on the item type
		switch(stItemInfo.dwRecordType)
		{
			case VBIN_RECORD_LEGACY_INFECTION:
				{
					hrRetVal = pQItem->UnpackageItem((LPSTR)(LPCSTR)strUnPackSesRoot,
														NULL,
														szGeneratedFilename,
														MAX_QUARANTINE_FILENAME_LEN,
														FALSE,
														TRUE );
				}
				break;				
				
			case VBIN_RECORD_REMEDIATION:
				{
					//Unpack the remediation record to a file, later while 
					//creating the package,it can be split in to Action and 
					//Undo files and then get added to submission package
					strDestFile.Format("%08lX.RAU", iRemedCnt++);	
					hrRetVal = pQItem->UnpackageItem((LPSTR)(LPCSTR)strUnPackSesRoot,
											(LPSTR)(LPCSTR)strDestFile,
											szGeneratedFilename,
											MAX_QUARANTINE_FILENAME_LEN,
											FALSE,
											FALSE );						
				}
				break;
			case VBIN_RECORD_SYSTEM_SNAPSHOT:
				{
					strDestFile = STR_SYSTEM_SNAPSHOT_TEMP_FILE;	
					hrRetVal = pQItem->UnpackageItem((LPSTR)(LPCSTR)strUnPackSesRoot,
											(LPSTR)(LPCSTR)strDestFile,
											szGeneratedFilename,
											MAX_QUARANTINE_FILENAME_LEN,
											FALSE,
											FALSE );							
				}
				break;
		}			
		if( FAILED( hrRetVal ) )
		{
			//Delete the files and Release the memory allocated so far for pStrArray
			try
			{
				if(pStrArray)
				{
					for(int i=0; i<pStrArray->GetSize(); i++)
					{
						if(PathFileExists(pStrArray->GetAt(i)))
							DeleteFile(pStrArray->GetAt(i));
					}
					pStrArray->RemoveAll();
					delete pStrArray;
					pStrArray = NULL;
				}
			}
			catch(...){}
			return FALSE;
		}
		//Add the destination file to list of files
		pStrArray->Add(szGeneratedFilename);
		
		hrRetVal = pQSes->GetNextItem(&pQItem);
		if(FAILED(hrRetVal) || NULL == pQItem)
		{
			break;
		}
	}	//end of For loop for Items in session
					
	nIndex = m_AcceptedFileInterfacePointers.Add(pQSes);
	//add the list of files to accepted files list
	m_AcceptedFileNames.SetAtGrow(nIndex, pStrArray);
	m_nAcceptedFiles++;
    // Set the FOD mode bool to TRUE.
    m_bInFileOfTheDayMode = TRUE;

    return (TRUE);

}  // end of "CScanDeliverDLL::ConvertToFileOfTheDayMode"


////////////////////////////////////////////////////////////////////////////
// Function name    : CheckFileOfTheDayTimeLapse
//
// Description      : This function checks the registry to see if we qualify
//                    to submit another "file of the day".  It basically checks
//                    if a day has lapsed since the last successful submission.
//                    (NOTE: this means "day" boundaries, not 24-hours)
//
// Return type      : BOOL -- TRUE if it's okay to submit a new file of the day,
//                            FALSE if not
//
////////////////////////////////////////////////////////////////////////////
// 4/23/98 SEDWARD - Function created / Header added.
// 5/14/98 JTAYLOR - Updated function to not use KEY_ALL_ACCESS
////////////////////////////////////////////////////////////////////////////
BOOL    CScanDeliverDLL::CheckFileOfTheDayTimeLapse(void)
{
    auto    BOOL            bResult = FALSE;
    auto    CTime           currentTime;
    auto    CTime           registryTime;
    auto    DWORD           dwRegValueType;
    auto    DWORD           dwSize;
    auto    HKEY            hKey = NULL;
    auto    int             nCurrentDay;
    auto    int             nRegistryDay;
    auto    long            lResult;
    auto    time_t          stRegistryTime;


    // open the registry key; if it doesn't exist, create one with the current
    // date and time, and exit
    lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE                       // handle of open key
                                        , REGKEY_QUARANTINE_PATH    // address of name of subkey to open
                                        , 0                         // reserved
                                        , (KEY_READ | KEY_WRITE)    // security access mask
                                        , &hKey);                   // address of handle of open key
    if (ERROR_SUCCESS != lResult)
        {
        hKey = NULL;
        CreateTimeLapseRegistryData(hKey);
        goto  Exit_Function;
        }

    // get the timestamp of the last successful file of the day submission; if it
    // does not exist, create the value with the current date and time, and exit
    dwSize = sizeof(stRegistryTime);
    lResult = SymSaferRegQueryValueEx(hKey                      // handle of key to query
                            , FILE_OF_THE_DAY_VALUE     // address of name of value to query
                            , 0                         // reserved (must be zero)
                            , &dwRegValueType           // address of buffer for value type
                            , (LPBYTE)&stRegistryTime   // address of data buffer
                            , &dwSize);                 // address of data buffer size


    if ((ERROR_SUCCESS != lResult)  ||  (REG_BINARY != dwRegValueType))
    {
		//If key value doesn't exist then user is SnD for the first time.
		//The value will be created and set when user submits first time.

		if(ERROR_FILE_NOT_FOUND == lResult)
			bResult = TRUE;
        //CreateTimeLapseRegistryData(hKey);
        goto  Exit_Function;
    }

    // initialize a CTime object with what we fetched from the registry
    registryTime = stRegistryTime;

    // get the current day
    currentTime = CTime::GetCurrentTime();

    // if the current day is at least one day since the last submission, we want
    // to return TRUE; else, return FALSE
    nRegistryDay = registryTime.GetDayOfWeek();
    nCurrentDay = currentTime.GetDayOfWeek();
    if (nCurrentDay - nRegistryDay)
        {
        bResult = TRUE;
        }
    else if (nCurrentDay == nRegistryDay)
        {
        // see if we're on the same day, but over a week
        auto    CTimeSpan       timeLapse = currentTime - registryTime;
        if (timeLapse.GetDays() > 1)
            {
            bResult = TRUE;
            }
        }


Exit_Function:

    if (NULL != hKey)
        {
        RegCloseKey(hKey);
        }

    return (bResult);

}  // end of "CScanDeliverDLL::CheckFileOfTheDayTimeLapse"




////////////////////////////////////////////////////////////////////////////
// Function name    : CreateTimeLapseRegistryData
//
// Description      : This function is used to create a "file of the day"
//                    registry value, in case it's missing.  The value is
//                    initialized with the current system timestamp.
//
// Return type      : BOOL -- TRUE means all went well, FALSE means not
//
////////////////////////////////////////////////////////////////////////////
// 4/23/98 SEDWARD - Function created / Header added.
// 5/14/98 JTAYLOR - Updated function to not use KEY_ALL_ACCESS
////////////////////////////////////////////////////////////////////////////
BOOL    CScanDeliverDLL::CreateTimeLapseRegistryData(HKEY  hRegKey)
{
    auto    BOOL            bResult = FALSE;
    auto    CTime           currentTime;
    auto    DWORD           dwDisposition;
    auto    HKEY            hTempRegKey = NULL;
    auto    long            lResult;
    auto    time_t          timeStruct;



    // if the caller wants the key created, do so
    if (NULL == hRegKey)
        {
        lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE                     // handle of an open key
                                            , REGKEY_QUARANTINE_PATH    // address of subkey name
                                            , 0                         // reserved
                                            , NULL                      // address of class string
                                            , 0                         // special options flag
                                            , (KEY_READ | KEY_WRITE)    // desired security access
                                            , NULL                      // address of key security structure
                                            , &hTempRegKey              // address of buffer for opened handle
                                            , &dwDisposition);          // address of disposition value buffer
        if (ERROR_SUCCESS != lResult)
            {
            goto  Exit_Function;
            }

        // save the new handle in the input argument (note: this does not affect
        // the caller's copy)
        hRegKey = hTempRegKey;
        }

    // get the current date/time and write it to the file of the day registry value
    currentTime = CTime::GetCurrentTime();
    timeStruct = currentTime.GetTime();
    lResult = RegSetValueEx(hRegKey                             // handle of key to set value for
                                    , FILE_OF_THE_DAY_VALUE     // address of value to set
                                    , 0                         // reserved
                                    , REG_BINARY                // flag for value type
                                    , (CONST BYTE*)&timeStruct  // address of value data
                                    , sizeof(timeStruct));      // size of value data
    if (ERROR_SUCCESS == lResult)
        {
        bResult = TRUE;
        }



Exit_Function:

    if (NULL != hTempRegKey)
        {
        RegCloseKey(hTempRegKey);
        }

    return (bResult);

}  // end of "CScanDeliverDLL::CreateTimeLapseRegistryData"



////////////////////////////////////////////////////////////////////////////
// Function name    : UpdateTimeLapseRegistryData
//
// Description      : This function is used to update the "file of the day"
//                    registry value with the current system timestamp.
//
// Return type      : BOOL -- TRUE means all went well, FALSE means not
//
////////////////////////////////////////////////////////////////////////////
// 4/23/98 SEDWARD - Function created / Header added.
// 5/14/98 JTAYLOR - Updated function to not use KEY_ALL_ACCESS
////////////////////////////////////////////////////////////////////////////
BOOL    CScanDeliverDLL::UpdateTimeLapseRegistryData(void)
{
    auto    BOOL            bResult = FALSE;
    auto    CTime           currentTime;
    auto    HKEY            hKey = NULL;
    auto    long            lResult;
    auto    time_t          timeStruct;


    // open the registry key; if it doesn't exist, create one with the current
    // date and time, and exit
    lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE                       // handle of open key
                                        , REGKEY_QUARANTINE_PATH    // address of name of subkey to open
                                        , 0                         // reserved
                                        , (KEY_READ | KEY_WRITE)    // security access mask
                                        , &hKey);                   // address of handle of open key
    if (ERROR_SUCCESS != lResult)
        {
        hKey = NULL;
        CreateTimeLapseRegistryData(hKey);
        goto  Exit_Function;
        }

    // get the current date/time and write it to the file of the day registry value
    currentTime = CTime::GetCurrentTime();
    timeStruct = currentTime.GetTime();
    lResult = RegSetValueEx(hKey                                // handle of key to set value for
                                    , FILE_OF_THE_DAY_VALUE     // address of value to set
                                    , 0                         // reserved
                                    , REG_BINARY                // flag for value type
                                    , (CONST BYTE*)&timeStruct  // address of value data
                                    , sizeof(timeStruct));      // size of value data
    if (ERROR_SUCCESS == lResult)
        {
        bResult = TRUE;
        }


Exit_Function:

    if (NULL != hKey)
        {
        RegCloseKey(hKey);
        }

    return (bResult);

}  // end of "CScanDeliverDLL::UpdateTimeLapseRegistryData"


////////////////////////////////////////////////////////////////////////////
// IScanDeliverDLL2
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Function name    : SetVirusDefinitionsDate
//
// Description      : This function is used to set the date of the virus defs
//                    used to scan the submitted samples last.
//
// Return type      : HRESULT
//
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::SetVirusDefinitionsDate( SYSTEMTIME* pstDate )
{
    //
    // Validate input
    //
    if( pstDate == NULL )
        return E_INVALIDARG;

    //
    // Save off virus defs date.
    //
    m_stDefsDate = *pstDate;

    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CScanDeliverDLL::SetDefinitionsDir
//
// Description  :
//
// Return type  : STDMETHODIMP
//
// Argument     : LPCSTR pszDefsDir
//
///////////////////////////////////////////////////////////////////////////////
// 3/25/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::SetDefinitionsDir( LPCSTR pszDefsDir )
{
    if( pszDefsDir == NULL )
        return E_INVALIDARG;

    m_sDefsFolder = pszDefsDir;

    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CScanDeliverDLL::SetStartupDir
//
// Description  :
//
// Return type  : STDMETHODIMP
//
// Argument     : LPCSTR pszStartupDir
//
///////////////////////////////////////////////////////////////////////////////
// 3/25/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::SetStartupDir( LPCSTR pszStartupDir )
{
    if( pszStartupDir == NULL )
        return E_INVALIDARG;

    m_sStartupFolder = pszStartupDir;
    _tcsncpy(g_szMainNavDir, A2T((LPSTR)pszStartupDir), MAX_QUARANTINE_FILENAME_LEN );

    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CScanDeliverDLL::SetConfigDir
//
// Description  :
//
// Return type  : STDMETHODIMP
//
// Argument     : LPCSTR  pszConfigDir
//
///////////////////////////////////////////////////////////////////////////////
// 3/25/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::SetConfigDir( LPCSTR  pszConfigDir )
{
    if( pszConfigDir == NULL )
        return E_INVALIDARG;

    m_sScanConfigPath = pszConfigDir;
    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CScanDeliverDLL::SetLanguage
//
// Description  :
//
// Return type  : STDMETHODIMP
//
// Argument     : LPCSTR  pszLanguage
//
///////////////////////////////////////////////////////////////////////////////
// 3/25/99 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::SetLanguage( LPCSTR  pszLanguage )
{
    if( pszLanguage == NULL )
        return E_INVALIDARG;

    m_sLanguage = pszLanguage;
    return S_OK;
}

BOOL	CScanDeliverDLL::AllowSubmission(BOOL bRepairable, CString& lpszReason)
{
	// if it can be repaired, forget it!
    if (bRepairable == TRUE)
    {
        if (lpszReason.LoadString(IDS_REFUSE_SUBMIT_REPAIRABLE))
		{
			lpszReason = "This virus can be safely detected and repaired by Norton AntiVirus and should not be submitted for further analysis.";
		}
		return(FALSE);
    }
	return TRUE;
}

/******************************************************************************
// Function name    : CScanDeliverDLL::IsSessionSubmittable
// Description      : This Method verifies whether the specified Session is 
						submittanble to SARC or not, if not returns the rejection reason
// Return type      : BOOL
// Argument         : IQuarantineSesion *pQSes - pointer to the Session Interface
// Argument         : CString& strRejReason - Reference for the CString to hold
						the rejection reason.
******************************************************************************/
BOOL CScanDeliverDLL::IsSessionSubmittable(IQuarantineSession *pQSes, CString& strRejReason)
{
	BOOL bWantSubmission = FALSE;
	IQuarantineItem2 *pQItem = NULL;
	DWORD	dwItemCnt = 0;
	DWORD	dwFileType = 0;	
	DWORD dwVirusID = 0;

	//Loop thru all the items in the session
	//if there is atleast one non repairable item, 
	//then we need to submit the Threat to SARC.
	//We are doing this one ahead of unpacking the items, 
	//To avoid unpacking and deleting the unpacked files 
	//if the session doesn't need to be sent.	
	
	HRESULT hrRetVal = pQSes->GetVirusID(&dwVirusID);
	if(FAILED(hrRetVal))
	{
		return FALSE;
	}

	if(0 == dwVirusID)
	{
		strRejReason.LoadString(IDS_TRIVIAL_POSSIBLE_THREAT);
		return FALSE;
	}

	hrRetVal = pQSes->GetItemCount(&dwItemCnt);
	if(FAILED(hrRetVal))
	{
		return FALSE;
	}
	for(DWORD dwItemIdx=0; dwItemIdx < dwItemCnt; dwItemIdx++)
	{
		hrRetVal = pQSes->GetItem(dwItemIdx, &pQItem);
		if(FAILED(hrRetVal) || NULL == pQItem)
			break;		
		//Get the item type from the session
		hrRetVal = pQItem->GetItemType(&dwFileType);
		
		if(FAILED(hrRetVal))
			break;
		if(VBIN_RECORD_LEGACY_INFECTION == dwFileType)
		{			
            hrRetVal = pQItem->GetFileType(&dwFileType);
			if(FAILED(hrRetVal))
				continue;
			if(!(dwFileType & QFILE_TYPE_REPAIRABLE))
			{
				bWantSubmission = true;
				break;
			}
		}
	}
	//If the session is not submittable return false and load the rejection reason string.
	if(FALSE == bWantSubmission)
		if(FALSE == strRejReason.LoadString(IDS_REFUSE_SUBMIT_REPAIRABLE))
			strRejReason = "This virus can be safely detected and repaired by Norton AntiVirus and should not be submitted for further analysis.";

	return bWantSubmission;
}

bool Is64BitOS(VOID)
{
    bool bRet = false;

    // Try to get a pointer to IsWow64Process() to determine if 64-bit Windows
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));
	::GetSystemDirectory( szPath, MAX_PATH );
	_tcscat( szPath, _T("\\KERNEL32.DLL"));
    HINSTANCE hLibrary = LoadLibrary ( szPath );

    if ( hLibrary == NULL )
    {
        return false;
    }

    static TCHAR szWow64Process[] = _T("IsWow64Process");

    typedef BOOL (WINAPI *fnIsWow64) (HANDLE,PBOOL);
    fnIsWow64 pIsWow64  = (fnIsWow64) GetProcAddress ( hLibrary, szWow64Process );
    BOOL bIsWow64 = FALSE;
    
    if ( pIsWow64 == NULL )
    {
        // Could not find the function so this is not a 64-bit OS
        bRet = false;
    }
    else if( pIsWow64(GetCurrentProcess(), &bIsWow64) != TRUE )
    {
        // Call to IsWow64Process() function call failed, assume 32-bit
        bRet = false;
    }
    else if( bIsWow64 != TRUE )
    {
        // 32-bit process
        bRet = false;
    }
    else
    {
        // 64-bit process
        bRet = true;
    }
        
    if(hLibrary != NULL)
    {
        FreeLibrary(hLibrary);
        hLibrary = NULL;
    }

    return bRet;
        
}

/******************************************************************************
// Function name    : CScanDeliverDLL::SetSubmissionInfoFields
// Description      : This method populates the info fields of Submission
// Return type      : HRESULT
// Argument         : ISNDSubmissionPtr ptrISub - pointer to the Submission interface
******************************************************************************/
HRESULT CScanDeliverDLL::SetSubmissionInfoFields(ISNDSubmissionPtr ptrISub)
{
	HRESULT hrRetVal = S_OK;
	BOOL	bResult;
	TCHAR   szInfoBuf[MAX_INI_BUF_SIZE]                           = "";

	// Fill out the info on the submission
    g_ConfigInfo.GetFirstName( szInfoBuf, MAX_INI_BUF_SIZE );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::FirstName, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    g_ConfigInfo.GetLastName( szInfoBuf, MAX_INI_BUF_SIZE );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::LastName, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    g_ConfigInfo.GetCompany( szInfoBuf, MAX_INI_BUF_SIZE );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::Company, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    g_ConfigInfo.GetAddress1( szInfoBuf, MAX_INI_BUF_SIZE );
    bResult = ptrISub->SetInfoField( ISNDSubmission::AddressLine1, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    g_ConfigInfo.GetAddress2( szInfoBuf, MAX_INI_BUF_SIZE );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::AddressLine2, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    g_ConfigInfo.GetCity( szInfoBuf, MAX_INI_BUF_SIZE );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::City, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    g_ConfigInfo.GetState( szInfoBuf, MAX_INI_BUF_SIZE );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::StateProvince, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    g_ConfigInfo.GetPhone( szInfoBuf, MAX_INI_BUF_SIZE );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::Phone, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    g_ConfigInfo.GetFax( szInfoBuf, MAX_INI_BUF_SIZE );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::Fax, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    g_ConfigInfo.GetEmail( szInfoBuf, MAX_INI_BUF_SIZE );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::Email, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    DWORD dwCountryCode;
    g_ConfigInfo.GetUserCountryCode( &dwCountryCode );
    sssnprintf( szInfoBuf, sizeof(szInfoBuf), "%d", dwCountryCode );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::Country, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    g_ConfigInfo.GetInfectionCountryCode( &dwCountryCode );
    sssnprintf( szInfoBuf, sizeof(szInfoBuf), "%d", dwCountryCode );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::InfectionCountry, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    g_ConfigInfo.GetSymptoms( szInfoBuf, MAX_INI_BUF_SIZE );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::Symptoms, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

	g_ConfigInfo.GetPreDefSymptoms( szInfoBuf, MAX_INI_BUF_SIZE );
	hrRetVal = ptrISub->SetInfoField( ISNDSubmission::PreDefSymptoms, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    // Get the zip code
    g_ConfigInfo.GetZipCode( szInfoBuf, MAX_INI_BUF_SIZE );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::ZipPostalCode, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    // Get the ATLAS ID number
    g_ConfigInfo.GetAtlasNumber( szInfoBuf, MAX_INI_BUF_SIZE );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::AtlasID, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    // Get the corporate support number
    g_ConfigInfo.GetCorporateSupportNumber( szInfoBuf, MAX_INI_BUF_SIZE );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::SupportNum, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    // Load the install language from the registry.
    if( m_sLanguage.IsEmpty() )
    {
        bResult = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
                                     REGKEY_NAV_MAIN,
                                     REGVAL_NAV_LANGUAGE,
                                     m_sLanguage.GetBuffer(MAX_INI_BUF_SIZE),
                                     MAX_INI_BUF_SIZE);
        m_sLanguage.ReleaseBuffer();
        if( FALSE == bResult )
        {
            return E_UNABLE_TO_SET_INFO_FIELD;
        }
    }

    // Set the language in the package
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::Language, m_sLanguage.GetBuffer(0) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }
	m_sLanguage.ReleaseBuffer();

    // set the date of definitions
    hrRetVal = ptrISub->SetInfoFieldDateTime( ISNDSubmission::DefinitionsDate,
                                                       m_stDefsDate.wYear,
                                                       m_stDefsDate.wMonth,
                                                       m_stDefsDate.wDay,
                                                       m_stDefsDate.wHour,
                                                       m_stDefsDate.wMinute,
                                                       m_stDefsDate.wSecond);
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

    // Set the platforms for which the user would like defs back.
    DWORD dwPlatforms;
    g_ConfigInfo.GetOperatingSystem( &dwPlatforms );
    // Convert this value to a string
    sssnprintf( szInfoBuf, sizeof(szInfoBuf), "%d", dwPlatforms );
    hrRetVal = ptrISub->SetInfoField( ISNDSubmission::ProductOS, T2A(szInfoBuf) );
    if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

	// Add the OSBits
	bool bIs64bit = Is64BitOS();
	if( bIs64bit )
		_tcscpy(szInfoBuf, _T("64"));
	else
		_tcscpy(szInfoBuf, _T("32"));

	g_ConfigInfo.SetOSBits( szInfoBuf );

	bResult = ptrISub->SetInfoField( ISNDSubmission::OSBits, szInfoBuf );
	if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

	//Set the Product Version Field Value
	sssnprintf( szInfoBuf, sizeof(szInfoBuf), _T("%u.%u.%u.%u"), MAINPRODUCTVERSION, SUBPRODUCTVERSION, INLINEPRODUCTVERSION, BUILDNUMBER);
	bResult = ptrISub->SetInfoField( ISNDSubmission::Version, szInfoBuf );
	if( FAILED(hrRetVal) )
    {
        return E_UNABLE_TO_SET_INFO_FIELD;
    }

	return hrRetVal;
}

/******************************************************************************
// Function name    : CScanDeliverDLL::SetSampleInfoFields
// Description      : This method populates the info fields of Sample
// Return type      : HRESULT
// Argument         : [IN] ISNDSamplePtr ptrSample - pointer to the Sample interface
// Argument			: [IN] IQuarantineSession *pQSession - Pointer to Quarantine Session interface.
******************************************************************************/
HRESULT CScanDeliverDLL::SetSampleInfoFields(ISNDSamplePtr ptrISample, IQuarantineSession *pQSession)
{
	HRESULT hrRetVal = S_OK;
	TCHAR   szInfoBuf[MAX_INI_BUF_SIZE]   = "";
	TCHAR   szMachineName[MAX_QUARANTINE_FILENAME_LEN]            = "";
	DWORD	dwTemp = 0;
	CString strTmp = _T("");

	if(!ptrISample || !pQSession)
		return E_INVALIDARG;

	// get the Unique ID / Sample GUID
    hrRetVal = pQSession->GetUniqueID(T2A(szInfoBuf),MAX_INI_BUF_SIZE);
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_GET_QSESSION_INFO;
	}

	hrRetVal = ptrISample->SetInfoField(ISNDSample::SampleGUID, T2A(szInfoBuf));
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}

    // Get the source machine name
    hrRetVal = pQSession->GetOriginalMachineName(T2A( szMachineName), MAX_QUARANTINE_FILENAME_LEN );
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_GET_QSESSION_INFO;
	}

	hrRetVal = ptrISample->SetInfoField(ISNDSample::MachineName, T2A(szMachineName));
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}

	//Get and set Virus ID
	hrRetVal = pQSession->GetVirusID(&dwTemp );
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_GET_QSESSION_INFO;
	}
	//Convert DWORD VirusID to String
	sssnprintf(szInfoBuf,sizeof(szInfoBuf),"%lu",dwTemp);
	hrRetVal = ptrISample->SetInfoField(ISNDSample::VirusID, T2A(szInfoBuf));
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}
	
	DWORD dwBufSz = MAX_INI_BUF_SIZE;

	//Get and set Virus Name
	hrRetVal = pQSession->GetVirusName(szInfoBuf, &dwBufSz);
	if(SUCCEEDED(hrRetVal))
	{	
		hrRetVal = ptrISample->SetInfoField(ISNDSample::VirusName, T2A(szInfoBuf));
		if(FAILED(hrRetVal))
		{
			return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
		}
	}
	else
	if(hrRetVal == ERROR_INSUFFICIENT_BUFFER)	//The buffer sent in was not of enough length
	{
		char *pstr = NULL;
		pstr = new char[dwBufSz+1];
		if(NULL == pstr)
		{
			return E_OUTOFMEMORY;
		}

		hrRetVal = pQSession->GetVirusName(pstr, &dwBufSz);
		if(FAILED(hrRetVal))
		{
			delete[] pstr;
			return E_UNABLE_TO_GET_QSESSION_INFO;
		}

		hrRetVal = ptrISample->SetInfoField(ISNDSample::VirusName, pstr);
		if(FAILED(hrRetVal))
		{
			delete[] pstr;
			return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
		}
		delete[] pstr;
	}					
	else
	{
		return E_UNABLE_TO_GET_QSESSION_INFO;
	}
	
	//Get the Virus Threat Category
	hrRetVal = pQSession->GetThreatCat(&dwTemp);
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_GET_QSESSION_INFO;
	}

	//Convert DWORD Threat Cat to String
	//SARC expects the Virus Threat Cat val from 0 to 10, Right Shift the threat cat 4 bits.
	sssnprintf(szInfoBuf,sizeof(szInfoBuf),"%lu",dwTemp>>4);
	//Set the Virus Threat Category
	hrRetVal = ptrISample->SetInfoField(ISNDSample::VirusThreatCat, T2A(szInfoBuf));
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}

	//TODO - get and set Sample Type.
	hrRetVal = pQSession->GetSampleType(&dwTemp);
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_GET_QSESSION_INFO;
	}

	//Convert DWORD SampleType to String
	sssnprintf(szInfoBuf,sizeof(szInfoBuf),"%lu",dwTemp);

	hrRetVal = ptrISample->SetInfoField(ISNDSample::SampleType, T2A(szInfoBuf));
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}
	
	hrRetVal = pQSession->GetAnomalyID(T2A(szInfoBuf), MAX_INI_BUF_SIZE);
	if(FAILED(hrRetVal))
	{	
		return E_UNABLE_TO_GET_QSESSION_INFO;
	}

	hrRetVal = ptrISample->SetInfoField(ISNDSample::AnomalyID, T2A(szInfoBuf));
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}

	//TODO - Get Item's Anomaly Category.
	/*hrRetVal = pQSession->GetAnomalyCat(&dwTemp);
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_GET_QSESSION_INFO;
	}

	//Convert DWORD Anomaly Cat to String
	sssnprintf(szInfoBuf,sizeof(szInfoBuf),"%lu",dwTemp);*/

	//Get the Multiple Anomaly Categories in the String format.
	dwTemp = MAX_INI_BUF_SIZE;
	hrRetVal = pQSession->GetAnomalyCat(szInfoBuf,&dwTemp);
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_GET_QSESSION_INFO;
	}
	
	hrRetVal = ptrISample->SetInfoField(ISNDSample::AnomalyCategory, T2A(szInfoBuf));
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}
	
	// set the date of definitions
	hrRetVal = ptrISample->SetInfoFieldDateTime( ISNDSample::DefDate,
													m_stDefsDate.wYear,
													m_stDefsDate.wMonth,
													m_stDefsDate.wDay,
													m_stDefsDate.wHour,
													m_stDefsDate.wMinute,
													m_stDefsDate.wSecond);
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}
	return S_OK;
}

/******************************************************************************
// Function name    :	CScanDeliverDLL::StripFileContent
// Description      :	Helper Method to strip the file content of the specified 
						file and returns the generated file name in the supplied 
						argument
// Return type      :	HRESULT
// Argument         :	[IN] LPCSTR strSourceFile - Source File Name
// Argument			:	[OUT]LPSTR  strGenFile	 - Destination File Name
// Argument			:	[IN] DWORD  dwGenFileBufSz - Destination File Buffer Size
******************************************************************************/
HRESULT CScanDeliverDLL::StripFileContent(LPCTSTR szSourceFile, LPTSTR  szStrippedFile, DWORD dwBufSz)
{
	CContentStripper::STRIP_STATUS stContentStripStatus;
	BOOL bResult;

	//szStrippedFile shoud be Non NULL and should be of Size more than MAX_QUARANTINE_FILENAME_LEN
	if(NULL == szStrippedFile || dwBufSz < MAX_QUARANTINE_FILENAME_LEN)
		return E_INVALIDARG;

    TCHAR szStrippedFileName[MAX_QUARANTINE_FILENAME_LEN];
	szStrippedFileName[0] = 0x00;

    HRESULT hrRetVal = CreateNewFileInQuarantineTemp(szSourceFile, 
												szStrippedFile,
                                                MAX_QUARANTINE_FILENAME_LEN );
    if( FAILED( hrRetVal ) )
    {
        return (hrRetVal = E_FILE_CREATE_FAILED);
    }
    
    FILETIME ftCreation;
    FILETIME ftAccess;
    FILETIME ftLastWrite;
    HANDLE   hTempHandle;

    stContentStripStatus = m_pStripper->Strip( szSourceFile,
                                                szStrippedFile );
    if( CContentStripper::STRIP_NO_ERROR != stContentStripStatus )
    {
        return ( hrRetVal = E_UNABLE_TO_STRIP_FILE);
    }

    // Preserve the file times for submission
    // Open the source file and read in its date/times
    hTempHandle = CreateFile( szSourceFile,                          // filename
                                GENERIC_READ,                        // access
                                0,                                   // don't share
                                NULL,                                // don't allow handle inheritance
                                OPEN_EXISTING,                       // only open if it exists
                                FILE_FLAG_SEQUENTIAL_SCAN,           // attributes and flags
                                NULL );                              // no template handle

    if (INVALID_HANDLE_VALUE == hTempHandle)
    {
        return (hrRetVal = E_FILE_CREATE_FAILED);
    }

    bResult = GetFileTime( hTempHandle, &ftCreation, &ftAccess, &ftLastWrite );
    if( FALSE == bResult )
    {
        return (hrRetVal = E_FAIL) ;
    }

    // close the file
    if ( INVALID_HANDLE_VALUE != hTempHandle )
        {
        CloseHandle( hTempHandle );
        }


    // Open the Destination file and write its date/times
    hTempHandle = CreateFile( szStrippedFile,                    // filename
                                GENERIC_WRITE,                       // access
                                0,                                   // don't share
                                NULL,                                // don't allow handle inheritance
                                OPEN_EXISTING,                       // only open if it exists
                                FILE_FLAG_SEQUENTIAL_SCAN,           // attributes and flags
                                NULL );                              // no template handle

    if (INVALID_HANDLE_VALUE == hTempHandle)
    {
        return (hrRetVal = E_FILE_CREATE_FAILED);
    }

    bResult = SetFileTime( hTempHandle, &ftCreation, &ftAccess, &ftLastWrite );
    if( FALSE == bResult )
    {
        return (hrRetVal = E_FAIL);
    }

    // close the file
    if ( INVALID_HANDLE_VALUE != hTempHandle )
    {
        CloseHandle( hTempHandle );
	}
	return S_OK;     
}

/******************************************************************************
// Function name    :	CScanDeliverDLL::StripRemediationUndoFileContent
// Description      :	Helper Method to strip the file content of a Remediation 
						Undo. Deletes source file after creating the stripped
						file and renames the stripped file to source file.
// Return type      :	HRESULT
// Argument         :	[IN] LPCSTR strSourceFile - Source File Name
******************************************************************************/
HRESULT CScanDeliverDLL::StripRemediationUndoFileContent(LPCTSTR szSourceFile)
{
	TCHAR   szStrippedFileName[MAX_QUARANTINE_FILENAME_LEN]       = "";

	if(NULL == szSourceFile)
		return E_INVALIDARG;

	HRESULT hrRetVal = StripFileContent(szSourceFile, szStrippedFileName, MAX_QUARANTINE_FILENAME_LEN);

	if(FAILED(hrRetVal))
		return hrRetVal;

	//Replace the source file with the stripped file
	if(FALSE == MoveFileEx(szStrippedFileName, szSourceFile, MOVEFILE_REPLACE_EXISTING))
	{
		DeleteFile(szStrippedFileName);
		return E_UNABLE_TO_REPLACE_SOURCE_WITH_STRIPPED_FILE;
	}

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////
// Function name    : CScanDeliverDLL::DeliverFilesToSarc()
//
// Description      : This function starts the file delivery process
//
// Return type      :
//
////////////////////////////////////////////////////////////////////////////
// 4/11/98 JTAYLOR - Function created / Header added.
// 4/15/98 JTAYLOR - Added calls to SARC APIs and file gathering/filtering
// 4/20/98 JTAYLOR - Added more error handling
// 4/20/98 JTAYLOR - Fixed a bug where a file being processed may be left
//                   on the drive.
// 4/20/98 JTAYLOR - Added and Update status on items successfully sent to SARC
// 4/25/98 JTAYLOR - Added malicious code filter.
// 5/01/98 JTAYLOR - Fixed problem with GetVersion call.
// 5/29/98 JTAYLOR - Changed trivial file filter failure to allow submission.
// 6/04/98 JTAYLOR - Removed unneccesary iQuarantineDll creation.
// 6/08/98 JTAYLOR - Changed the resource DLL to be dynamically loaded.
// 6/08/98 JTAYLOR - Changed the NAV path to use the global function.
// 6/11/98 JTAYLOR - Added dissable for expired trialware.
// 6/12/98 JTAYLOR - Added rejection reason from malicious code filter
// 6/30/98 JTAYLOR - Added E_UNABLE_TO_INITIALIZE error code for Quarantine
//                      to display a message for.
//                 - Improved the error reporting logic
// 7/08/98 JTAYLOR - Changed the date submitted to GMT
// 2/22/05 KTALINKI - Modifications needed for Cetral Quarantine Server to submit
//						legacy threats as IQuarantineItem pointers
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CScanDeliverDLL::DeliverFilesToSarc( IQuarantineItem2 **pQuarantineItemArray, int nNumberItems )
{
    // since this module displays the wizard panels, we need to swap module states
    // in order to access our resources
    AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

    auto int     nIndex = 0;
    auto int     i = 0;
    auto int     nStartup = 0;
    auto char    szGeneratedFilename[MAX_QUARANTINE_FILENAME_LEN]      = "";
    auto DWORD   dwErrorBits                                           = 0;
    auto CString strRejectionReason = _T("");
    auto int     nMaxRejectionReasonLength                             = 1000;
    auto IQuarantineItem2 *pIQuarItem                                   = NULL;
    auto HRESULT hr                                                    = E_FAIL;
    auto BOOL    bResult                                               = FALSE;
    auto CString *pCStringTempHolder                                   = NULL;
    auto HRESULT hrRetVal                                              = E_FAIL;
    auto ViralSubmissionFilter *pVirFilter                             = NULL;
    auto CString szStatusText;

    HINSTANCE hResources = NULL;

	/**************************************************************************
	Assumptions		:	Though this method can handle multiple Quarantine Items, 
						this will be called with only one item, because Anomalies 
						are already in SARC Submission package, it will be needed 
						to unpack all the items and repack. So Central Quarantine 
						will call Scan And Deliver with only one item.	
	****************************************************************************/

	if(nNumberItems > 1)
	{
		return E_INVALIDARG;
	}
    m_bPackageSent = FALSE;

	//Central Quarantine Console only calls DeliverfilesToSarc with IQurantineItem pointers.
	//It needs different UI behavior, turn on this flag.
	g_bUsedByCQC = TRUE;
	g_bCQAnomaly = FALSE;
    // Get the installation directory
    if( m_sStartupFolder.IsEmpty() )
    {
        bResult = GetMainNavDir( m_sStartupFolder.GetBuffer(MAX_QUARANTINE_FILENAME_LEN), MAX_QUARANTINE_FILENAME_LEN * sizeof (TCHAR)  );
        m_sStartupFolder.ReleaseBuffer();
        // Make sure the get path function suceeded.
        if( FALSE == bResult )
        {
            return E_UNABLE_TO_INITIALIZE;
        }
    }

    // Make sure that the resource DLL is loaded
    InitScanDeliverResources( );

    // Make sure that the arguments are marginally valid
    if( ( 0 >= nNumberItems ) ||
        ( NULL == pQuarantineItemArray ) )
    {
        return E_INVALIDARG;
    }

    // Initialize some global DBCS information.
    InitDbcsVars();

    // Check to see if we are running on a trial system that has expired.
    BOOL bTrialExpired = IsTrialInstallAndExpired();

    if( bTrialExpired )
    {
        CString szTrialTitle;
        szTrialTitle.LoadString(IDS_TRIAL_EXPIRED_TITLE);
        CString szTrialText;
        szTrialText.LoadString(IDS_TRIAL_EXPIRED_TEXT);

        MessageBox(NULL, szTrialText, szTrialTitle, MB_OK);
        return E_FAIL;
    }

    // Clear the accepted and rejected lists. These should be null.
    m_AcceptedQItemPointers.RemoveAll();
    m_QIAcceptedFileNames.RemoveAll();
    m_RejectedQItemPointers.RemoveAll();
    m_QIRejectedFileStrings.RemoveAll();

    // Create progress dialog
    CProgressDlg progress;
    progress.Create();
    progress.SetRange(0, nNumberItems );
    // Load the status text string
    szStatusText.LoadString(IDS_PROGRESS_ANALYZING);
    progress.SetStatus(szStatusText);

    // get the temporary Quarantine Directory for unpackaging
    bResult = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
                                 REGKEY_QUARANTINE_TEMP_PATH,
                                 REGVAL_QUARANTINE_TEMP_PATH,
                                 m_sTempFolder.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ),
                                 MAX_QUARANTINE_FILENAME_LEN);
    m_sTempFolder.ReleaseBuffer();
    if( FALSE == bResult )
    {
        //
        // If no temp folder specified, use windows temp path.
        //
        if( 0 == GetTempPath( MAX_QUARANTINE_FILENAME_LEN, m_sTempFolder.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ) ) )
        {
            m_sTempFolder.ReleaseBuffer();
            hrRetVal = E_QUARANTINE_DIRECTORY_INVALID;
            goto Exit_Function;
        }
        m_sTempFolder.ReleaseBuffer();
    }

    // Get virus defs folder.
    if( m_sDefsFolder.IsEmpty() )
    {
        bResult = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
                                     REGKEY_NAV_DEFINITIONS_PATH,
                                     REGVAL_NAV_DEFINITIONS_PATH,
                                     m_sDefsFolder.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ),
                                     MAX_QUARANTINE_FILENAME_LEN);
        m_sDefsFolder.ReleaseBuffer();
        if( FALSE == bResult )
        {

            hrRetVal = E_QUARANTINE_DIRECTORY_INVALID;
            goto Exit_Function;
        }
    }

    // get the Scan Config Options Directory
    if( m_sScanConfigPath.IsEmpty() )
    {
        bResult = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
                                     REGKEY_QUARANTINE_PATH,
                                     REGVAL_QUARANTINE_OPTIONS_PATH_STR,
                                     m_sScanConfigPath.GetBuffer( MAX_QUARANTINE_FILENAME_LEN ),
                                     MAX_QUARANTINE_FILENAME_LEN);
        m_sScanConfigPath.ReleaseBuffer(0);
        if( FALSE == bResult )
        {
            hrRetVal = E_QUARANTINE_DIRECTORY_INVALID;
            goto Exit_Function;
        }
    }
	
    // Add each item to the accepted or rejected list.
    for( i = 0; i < nNumberItems; i++ )
    {        
		DWORD dwItemType = -1;
        // Clear the Generated Filename.  Having a non-empty value will confuse UnpackageItem
        szGeneratedFilename[0] = NULL;		
        IQuarantineItem2 *pQuarantineItem = pQuarantineItemArray[i];
        hr = pQuarantineItem->UnpackageItem( m_sTempFolder.GetBuffer(0),
                                             NULL,
                                             szGeneratedFilename,
                                             MAX_QUARANTINE_FILENAME_LEN,
                                             FALSE,
                                             TRUE );
        if( FAILED( hr ) )
        {
            hrRetVal = hr;
            goto Exit_Function;
        }

		//Get the item type from the session
		hr = pQuarantineItem->GetItemType(&dwItemType);	
		if(SUCCEEDED(hr))
		{
			if(VBIN_RECORD_LEGACY_INFECTION != dwItemType)
			{			
				g_bCQAnomaly = TRUE;
			}			
		}

        if(IsQItemSubmittable(pQuarantineItem, strRejectionReason))
        {
            // Create a string referencing the filename.
			try
			{
				pCStringTempHolder = new CString( szGeneratedFilename );
			}
			catch(std::bad_alloc &){}
            if( NULL == pCStringTempHolder )
            {
                hrRetVal = E_OUTOFMEMORY;
                goto Exit_Function;
            }
            // Clear the Generated Filename.
            szGeneratedFilename[0] = NULL;

            // If the file is accepted, then add it to the accepted list
            // Also store the unpackaged filename for use later.
            nIndex = m_AcceptedQItemPointers.Add(pQuarantineItemArray[i]);
            m_QIAcceptedFileNames.SetAtGrow( nIndex, pCStringTempHolder );
			CString *tmp = m_QIAcceptedFileNames.GetAt(nIndex);
            m_nAcceptedFiles++;
        }
        else
        {
			try
			{
				pCStringTempHolder = new CString( strRejectionReason );
			}
			catch(std::bad_alloc &){}
            if( NULL == pCStringTempHolder )
            {
                hrRetVal = E_OUTOFMEMORY;
                goto Exit_Function;
            }

            // If the file is rejected then add it to the rejected list.
            // Save the rejection reason for later, and delete the rejected file.
            nIndex = m_RejectedQItemPointers.Add(pQuarantineItemArray[i]);
            m_QIRejectedFileStrings.SetAtGrow( nIndex, pCStringTempHolder );
            m_nRejectedFiles++;
            WipeOutFile( szGeneratedFilename );

            // Clear the Generated Filename.
            szGeneratedFilename[0] = NULL;
        }

        // Advance progress dialog
        progress.SetPos( i );

        if( progress.CheckCancelButton() )
        {
            hrRetVal = E_USER_CANCELLED;
            goto Exit_Function;
        }
    }

    // Finish creating the scan config options object.
    AppendFileName( m_sScanConfigPath.GetBuffer(MAX_QUARANTINE_FILENAME_LEN), SCAN_CONFIG_DAT_FILENAME );
    m_sScanConfigPath.ReleaseBuffer();

    bResult = g_ConfigInfo.Initialize(m_sScanConfigPath.GetBuffer(0), TRUE); // TRUE means create the DAT file if it does not exist
    if( FALSE == bResult )
    {
        hrRetVal = E_UNABLE_TO_CREATE_OPTIONS_OBJECT;
        goto Exit_Function;
    }

    bResult = g_ConfigInfo.ReadConfigFile(&dwErrorBits);
    if( FALSE == bResult )
    {
        hrRetVal = E_UNABLE_TO_READ_OPTIONS;
        goto Exit_Function;
    }

    // Remove progress window
    progress.DestroyWindow();

    // display the wizard panels
    LaunchUI();

    // If the wizard panels sent a package to SARC, then update the
    // IQuarantineItems.
    if( TRUE == m_bPackageSent )
    {
        // Get the current date/time
        SYSTEMTIME stDateSubmitted;
        GetSystemTime( &stDateSubmitted );

        // Since the send to SARC was successfull, mark them as sent.
        for( i = 0; i < m_nAcceptedFiles; i++ )
        {
            // Update the status of the file as sent to SARC.
            pIQuarItem = m_AcceptedQItemPointers[i];

            hr = pIQuarItem->SetFileStatus(QFILE_STATUS_SUBMITTED);
            if( FAILED( hr ) )
            {
                hrRetVal = hr;
                goto Exit_Function;
            }

            // Update the time the files were sent to SARC
            hr = pIQuarItem->SetDateSubmittedToSARC( &stDateSubmitted );
            if( FAILED( hr ) )
            {
                hrRetVal = hr;
                goto Exit_Function;
            }

            hr = pIQuarItem->SaveItem();
            if( FAILED( hr ) )
            {
                hrRetVal = hr;
                goto Exit_Function;
            }
        }
    }

    hrRetVal = S_OK;

Exit_Function:

    if( S_OK != hrRetVal &&
        E_USER_CANCELLED != hrRetVal &&
        E_OUTOFMEMORY != hrRetVal )
    {
        auto CString szError;
        auto CString szErrorCaption;
        szErrorCaption.LoadString( IDS_CAPTION_STANDARD_MESGBOX );

        if( TRUE == m_bPackageSent )
        {
            szError.LoadString( IDS_ERROR_UPDATING_ITEMS );
        }
        else
        {
            szError.LoadString( IDS_ERROR_STARTING_SUBMISSION );
        }

        MessageBox( NULL, szError, szErrorCaption, MB_OK | MB_ICONERROR );
    }

    // Remove progress window
    progress.DestroyWindow();

    // If we were in the middle of testing a file, delete that file
    if( NULL != szGeneratedFilename[0] )
    {
        WipeOutFile( szGeneratedFilename );

        // Clear the Generated Filename.
        szGeneratedFilename[0] = NULL;
    }

	/*
    // Clean up the virus filter -- note these have no return values to check
    if( NULL != pVirFilter )
    {
        pVirFilter->ShutDown();
        pVirFilter->Release();
    }
	*/
    // Free the memory for the strings.
    for( i = 0; i < m_nRejectedFiles; i++ )
        {
        CString *pStr = m_QIRejectedFileStrings[i];
        if( NULL != pStr )
            {
            delete pStr;
            }
        }

    // delete the accepted files and free the filenames
    for( i = 0; i < m_nAcceptedFiles; i++ )
    {
        CString *pStr = m_QIAcceptedFileNames[i];
        if( NULL != pStr )
        {
            WipeOutFile(*pStr);
            delete pStr;
        }
    }

    // Delete the package file.
    if( strlen(m_szPackageFilename) > 0 )
    {
        WipeOutFile( m_szPackageFilename );
    }

	if(g_bCQAnomaly)	//Delete the memory allocated for m_lpbyBuff
	{
		if(m_lpbyBuff)
		{
			delete[] m_lpbyBuff;
			m_lpbyBuff = NULL;
		}
	}
    // Clear the accepted and rejected lists.
    m_AcceptedQItemPointers.RemoveAll();
    m_QIAcceptedFileNames.RemoveAll();
    m_RejectedQItemPointers.RemoveAll();
    m_QIRejectedFileStrings.RemoveAll();

    FreeLibrary(hResources);

	//Initialize these flags to FALSE, so that if the user calls with IQuarantineSession these won't be effective
	g_bCQAnomaly = FALSE;
	g_bUsedByCQC = FALSE;
    return hrRetVal;
}

/******************************************************************************
Function Name	:	CreatePackageWtIQItems
Description		:	Method to create the SARC Submission Package with IQuarantine
					Items, this is useful when Central Quarantine Console calls
					ScanAndDeliver with IQuarantineItems pointers representing
					Legacy Viral Threats (Non Anomalies) and Anomalies as prepared
					SARC Submission Packages. Anomalies in prepared SARC Submission 
					Package needs to be updated with the user information,then 
					encrypt it.
Assumptions		:	Though this method can handle multiple Quarantine Items, it is 
					assumed that CQ will call this with only one item, because 
					Anomalies are already in SARC Submission package, it will be 
					needed to unpack all the items and repack.
Parameters		:	HWND - Handle to window
Return Type		:	HRESULT
******************************************************************************/
STDMETHODIMP CScanDeliverDLL::CreatePackageWtIQItems( HWND hWnd )
{
    BOOL bResult = FALSE;
    IQuarantineDLL *pIQuarDll                                     = NULL;    
    TCHAR   szOriginalName[MAX_QUARANTINE_FILENAME_LEN]           = "";
    ISNDSubmission::FIELD stField;
    CContentStripper::INIT_STATUS  stContentInitStatus;
    DWORD   dwHighSize                                            = 0;
    DWORD   dwLowSize                                             = 0;
    HANDLE  hPackage                                              = INVALID_HANDLE_VALUE;
    HRESULT hrRetVal                                              = E_FAIL;    
    BOOL    bStripFiles                                           = FALSE;
    CString szStatusText;
	
	ISNDSamplePtr			ptrISample = NULL;
	ISNDProgressPtr			ptrSNDProgress = NULL;	
	SYMRESULT							sr = 0;

	int		i,iItemIdx	=	0;
	DWORD	dwItemCnt	=	0;
	DWORD	dwTemp = 0;
	DWORD	dwFileSize, dwNumberFiles;

    CWnd *pWindow = CWnd::FromHandle( hWnd );
    if( NULL == pWindow )
    {
        return E_INVALIDARG;
    }

	//Get the Item and check the type, if it is an anomaly, call UpdateSARCTxtNEncryptPkg method to 
	//update the SARC.TXT file in the package and encrypt it.
	IQuarantineItem2 *pItem = m_AcceptedQItemPointers.GetAt(0);
	if(pItem)
	{
		DWORD dwType = -1;
		hrRetVal = pItem->GetItemType(&dwType);
		if(FAILED(hrRetVal))
			return hrRetVal;
		if(dwType != VBIN_RECORD_LEGACY_INFECTION)
		{
			CString *tmp = m_QIAcceptedFileNames.GetAt(0);
			if(tmp && !tmp->IsEmpty())
			{
				_tcscpy(m_szPackageFilename, (LPCTSTR)(*tmp));
				PathRenameExtension(m_szPackageFilename, ".zip");				
                return UpdateSARCTxtNEncryptPkg((LPCTSTR)(*tmp));
			}
			else
				return E_FAIL;
		}
	}
	else
		return E_FAIL;
	
	try
	{
		// Make sure we have valid defs date
		if( m_stDefsDate.wYear == 0 )
		{
			// get the IQuarantineDLL
			hrRetVal = CoCreateInstance( CLSID_QuarantineDLL,
										NULL,
										CLSCTX_INPROC_SERVER,
										IID_QuarantineDLL,
										(LPVOID*)&pIQuarDll);
			if( FAILED( hrRetVal ) )
			{
				return hrRetVal;
			}

			hrRetVal = pIQuarDll->Initialize();
			if( SUCCEEDED( hrRetVal ) )
			{
				// get the date of definitions
				hrRetVal = pIQuarDll->GetCurrentDefinitionsDate( &m_stDefsDate );
			}

			// Cleanup.
			pIQuarDll->Release();
			if( FAILED( hrRetVal ) )
				return hrRetVal;
		}
	}
	catch(...)
	{
		//Continue;
	}
	
    // Create progress dialog
    CProgressDlg *ptrProgress = NULL;

	try{	ptrProgress = new CProgressDlg;	}
	catch(std::bad_alloc&){ return E_OUTOFMEMORY; }

	if(NULL == ptrProgress)
		return E_OUTOFMEMORY;

    ptrProgress->Create(pWindow);
    ptrProgress->SetRange(0, m_nAcceptedFiles );

    // Load the status text string
    szStatusText.LoadString(IDS_PROGRESS_CREATING_PACKAGE);
    ptrProgress->SetStatus(szStatusText);

	//Initialize class loaders to create the ISndSubmission and ISndSample objects.
	sr = m_objSndSubLoader.Initialize();
	if(SYM_FAILED(sr))
	{
		hrRetVal = E_UNABLE_TO_INITIALIZE;
		goto Exit_Function;
	}

	sr = m_objSndSampleLoader.Initialize();
	if(SYM_FAILED(sr))
	{
		hrRetVal = E_UNABLE_TO_INITIALIZE;
		goto Exit_Function;
	}

    // Create a unique filename in the Quarantine TEMP directory.
    hrRetVal = CreateNewFileInQuarantineTemp( _T(".zip"), m_szPackageFilename, MAX_QUARANTINE_FILENAME_LEN );
    if( FAILED( hrRetVal ) )
    {
        hrRetVal = E_FILE_CREATE_FAILED;
        goto Exit_Function;
    }

	//Instantiate ISNDSubmission interface
	sr = m_objSndSubLoader.CreateObject(&m_pISNDSubmission);
	if(SYM_FAILED(sr) || !m_pISNDSubmission)
	{
		hrRetVal = E_UNABLE_TO_START_PACKAGING_CLASS;
		goto Exit_Function;
	}	
	
    // Set the package name and prepare it for info.
    hrRetVal = m_pISNDSubmission->Reset( T2A(m_szPackageFilename));
    if( FAILED(hrRetVal) )
    {
        hrRetVal = E_UNABLE_TO_START_PACKAGING_CLASS;
        goto Exit_Function;
    }

	hrRetVal = SetSubmissionInfoFields(m_pISNDSubmission);
	if(FAILED(hrRetVal))
		goto Exit_Function;

	
    // Find out if the user elected to strip the content from files.
    bStripFiles = g_ConfigInfo.GetStripFileContent( );

    // If we want to strip the files, then create the strip file object
    if( TRUE == bStripFiles && NULL == m_pStripper)
    {
        // Allocate the new class
        m_pStripper = CContentStripperFactory::CreateInstance();
        if( NULL == m_pStripper)
        {
            hrRetVal = E_OUTOFMEMORY;
            goto Exit_Function;
        }

        // Initialize the content stripper
        stContentInitStatus = m_pStripper->StartUp( m_sStartupFolder.GetBuffer(0) );
        if( INIT_NO_ERROR != stContentInitStatus )
        {
            hrRetVal = E_UNABLE_TO_CREATE_FILE_STRIPPER;
            goto Exit_Function;
        }
    }
	
    // Add all the files to the package.
    // Loop through each accepted Session, and add items to the package
    for( i = 0; i < m_nAcceptedFiles; i++ )
    {
		IQuarantineItem2		*pQItem		= m_AcceptedQItemPointers[i];        

		if(NULL == pQItem)
			continue;

		/*N30 *pN30 = NULL;
        hrRetVal = pQItem->GetN30StructPointer(&pN30);
        if( FAILED( hrRetVal ) )
        {            
            continue;
        }
		*/

		//Get the info details of the each sample and create a sample
		sr = m_objSndSampleLoader.CreateObject(&ptrISample);
		if(SYM_FAILED(sr) || !m_pISNDSubmission)
		{
			hrRetVal = E_UNABLE_TO_START_PACKAGING_CLASS;
			goto Exit_Function;
		}
        
		

		hrRetVal = SetSampleInfoFields(ptrISample,pQItem);
		if(FAILED(hrRetVal))
		{
			ptrISample.Release();
			continue;
		}		

		TCHAR szStrippedFile[MAX_QUARANTINE_FILENAME_LEN];

		// Get the original filename
        pQItem->GetOriginalAnsiFilename( szOriginalName, MAX_QUARANTINE_FILENAME_LEN );

		CString *strTmp = m_QIAcceptedFileNames[i];
		if(NULL == strTmp)
			continue;
		//If the user has selected for File Content Stripping
		if( TRUE == bStripFiles )
		{
			hrRetVal = StripFileContent(*strTmp,szStrippedFile,MAX_QUARANTINE_FILENAME_LEN);
			if(FAILED(hrRetVal))
			{
				hrRetVal = E_UNABLE_TO_STRIP_FILE;
				goto Exit_Function;
			}
			m_strDeletableFilesArray.Add(szStrippedFile);
							
			sr = ptrISample->AddSampleFile(szStrippedFile,szOriginalName);
		}
		else
		{			
			sr = ptrISample->AddSampleFile((LPCSTR)*strTmp, szOriginalName);	
		}
		if(SYM_FAILED(sr))
		{
			hrRetVal = E_UNABLE_TO_ADD_FILE_TO_SAMPLE;
			goto Exit_Function;
		}

		sr = m_pISNDSubmission->SubmitSample(ptrISample);
		if(SYM_FAILED(sr))
		{
			hrRetVal = E_UNABLE_TO_ADD_SAMPLE_TO_SUB;
			goto Exit_Function;
		}		

        // Advance progress dialog
        ptrProgress->SetPos( i );

        if( ptrProgress->CheckCancelButton() )
        {
            hrRetVal = E_USER_CANCELLED;
            goto Exit_Function;
        }
    }	//End of Items For loop

    ptrProgress->SetRange(0, 100 );

    // Load the status text string
    szStatusText.LoadString(IDS_PROGRESS_COMPRESSING);
    ptrProgress->SetStatus(szStatusText);

    // Reset the position to 0%
    ptrProgress->SetPos( 0 );
	
	ptrSNDProgress = ptrProgress;
    // Zip up the package
    sr = m_pISNDSubmission->Finish(ptrSNDProgress, 0, 0, &m_lpbyBuff,&stField);
	
	if(SYM_FAILED(sr))
	{
		hrRetVal = E_UNABLE_TO_FINISH_PACKAGE;
        goto Exit_Function;
	}    

    // Open the package file
    hPackage = CreateFile(m_szPackageFilename,
                          GENERIC_READ,
                          0,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);

    if( INVALID_HANDLE_VALUE == hPackage )
    {
        hrRetVal = E_FILE_CREATE_FAILED;
        goto Exit_Function;
    }

    // get the filesize
    dwLowSize = GetFileSize( hPackage, &dwHighSize );
    // If there are more that 10 MB of files, return failure
    if( ( dwHighSize > 0 ) ||
		( dwLowSize > 10 * 1024 * 1024 ) )
    {
        hrRetVal = E_PACKAGE_OVER_MAX_SUBMISSION_SIZE;
        goto Exit_Function;
    }

    // Close the file handle
    if( FALSE == CloseHandle(hPackage) )
    {
        hrRetVal = E_FILE_CLOSE_FAILED;
        goto Exit_Function;
    }
    else
    {
        hPackage = INVALID_HANDLE_VALUE;
    }

    // This is the code to use if SARC fixes GetTotalFileSizeCount.
    // Otherwise, we can use the code above to check the compressed package file size.
    // Now make sure that the submission size is < 10 MegaBytes
    hrRetVal = m_pISNDSubmission->GetTotalFileSizeCount( &dwFileSize, &dwNumberFiles );
    if( FAILED(hrRetVal))
    {
        hrRetVal = E_UNABLE_TO_FINISH_PACKAGE;
        goto Exit_Function;
    }

    // If there are more that 10 MB of files, return failure
    if( dwFileSize > 10 * 1024 * 1024 )
        {
        hrRetVal = E_UNABLE_TO_FINISH_PACKAGE;
        goto Exit_Function;
        }
	
	if(ptrSNDProgress)
		ptrSNDProgress.Release();
	else
	{	//ptrProgress is created and still not assigned to ptrSNDProgress, so destroy the window
		if(ptrProgress)
		{// Remove progress window
			ptrProgress->DestroyWindow();
			delete ptrProgress;
			ptrProgress = NULL;
		}
	}

    // If we have made it here, then we can safely return S_Ok
    return S_OK;

Exit_Function:

	if(ptrSNDProgress)
		ptrSNDProgress.Release();
	else
	{
		if(ptrProgress)
		{// Remove progress window
			ptrProgress->DestroyWindow();
			delete ptrProgress;
			ptrProgress = NULL;
		}
	}

    // Close the file handle if it is open.
    if( INVALID_HANDLE_VALUE != hPackage )
    {
        CloseHandle(hPackage);
    }
    // return
    return hrRetVal;
}	//End of CreatePackageWtIQItems


/******************************************************************************
// Function name    : CScanDeliverDLL::SetSampleInfoFields
// Description      : This method populates the info fields of Sample with IQuarantineItem
//						This method is needed when Scan And Deliver is been called from
//						Central Quarantine Server with IQuarantineItem pointers.
// Return type      : HRESULT
// Argument         : [IN] ISNDSamplePtr ptrSample - pointer to the Sample interface
// Argument			: [IN] IQuarantineItem *pQItem - Pointer to Quarantine Item interface.
******************************************************************************/
HRESULT CScanDeliverDLL::SetSampleInfoFields(ISNDSamplePtr ptrISample, IQuarantineItem2 *pQItem)
{
	HRESULT hrRetVal = S_OK;
	TCHAR   szInfoBuf[MAX_INI_BUF_SIZE]   = "";
	TCHAR   szMachineName[MAX_QUARANTINE_FILENAME_LEN]            = "";
	DWORD	dwTemp = 0;
	GUID    stGUID;
	DWORD	dwBufSz = MAX_INI_BUF_SIZE;

	if(!ptrISample || !pQItem)
		return E_INVALIDARG;

	// get the Unique ID / Sample GUID
    hrRetVal = pQItem->GetUniqueID(stGUID);
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_GET_QITEM_INFO;
	}
		
	sssnprintf(szInfoBuf, sizeof(szInfoBuf), "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", stGUID.Data1,stGUID.Data2,stGUID.Data3,
						stGUID.Data4[0], stGUID.Data4[1],stGUID.Data4[2],stGUID.Data4[3],stGUID.Data4[4],
						stGUID.Data4[5],stGUID.Data4[6],stGUID.Data4[7]);
	
	hrRetVal = ptrISample->SetInfoField(ISNDSample::SampleGUID, T2A(szInfoBuf));
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}
	
    // Get the source machine name
    hrRetVal = pQItem->GetOriginalMachineName(T2A( szMachineName), MAX_INI_BUF_SIZE);
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_GET_QITEM_INFO;
	}

	hrRetVal = ptrISample->SetInfoField(ISNDSample::MachineName, T2A(szMachineName));
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}

	dwTemp = 0;
	hrRetVal = pQItem->GetVirusID(&dwTemp);
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_GET_QITEM_INFO;
	}

	//Convert DWORD VirusID to String
	sssnprintf(szInfoBuf,sizeof(szInfoBuf),"%lu",dwTemp);
	hrRetVal = ptrISample->SetInfoField(ISNDSample::VirusID, T2A(szInfoBuf));
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}
	dwBufSz = MAX_INI_BUF_SIZE;	
	hrRetVal = pQItem->GetVirusName(T2A( szInfoBuf), &dwBufSz);
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_GET_QITEM_INFO;
	}

	hrRetVal = ptrISample->SetInfoField(ISNDSample::VirusName, T2A(szInfoBuf));
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}
	
	dwTemp = 0;
	//Get VirusType
	hrRetVal = pQItem->GetVirusType(&dwTemp);
	
	if(FAILED(hrRetVal))
	{
		if(E_NOTIMPL != hrRetVal)
			return E_UNABLE_TO_GET_QITEM_INFO;
	}
	//Convert DWORD Threat Cat to String
	//SARC expects the Virus Threat Cat val from 0 to 10, Right Shift the threat cat 4 bits.
	if(E_NOTIMPL == hrRetVal || 0 == dwTemp)
		_tcscpy(szInfoBuf, "Unknown");
	else
		sssnprintf(szInfoBuf,sizeof(szInfoBuf),"%lu",dwTemp>>4);

	//Set the Virus Threat Category
	hrRetVal = ptrISample->SetInfoField(ISNDSample::VirusThreatCat, T2A(szInfoBuf));
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}

	//TODO - get and set Sample Type.	
	hrRetVal = ptrISample->SetInfoField(ISNDSample::SampleType, "Unknown");
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}
		
	hrRetVal = ptrISample->SetInfoField(ISNDSample::AnomalyID, "Unknown");
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}
	
	hrRetVal = ptrISample->SetInfoField(ISNDSample::AnomalyCategory, "Unknown");
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}
	
	// set the date of definitions
	hrRetVal = ptrISample->SetInfoFieldDateTime( ISNDSample::DefDate,
													m_stDefsDate.wYear,
													m_stDefsDate.wMonth,
													m_stDefsDate.wDay,
													m_stDefsDate.wHour,
													m_stDefsDate.wMinute,
													m_stDefsDate.wSecond);
	if(FAILED(hrRetVal))
	{
		return E_UNABLE_TO_SET_SAMPLE_INFO_FLD;
	}
	return S_OK;
}

/******************************************************************************
Name		:	CreatePackageWtArchive
Description	:	Method to create the SARC submission package from the existing
				archive, which is also a non encrypted SARC Submission Package
Arguments	:	[IN] strArchiveFile - Archive File Name
Return		:	HRESULT
******************************************************************************
STDMETHODIMP CScanDeliverDLL::CreatePackageWtArchive(HWND hWnd)
{
	HRESULT hrRetVal = S_OK;
	return hrRetVal;
}
*/
/******************************************************************************
Name		:	GetRejectedQuarantineItem
Description	:	Method to get an rejected Quarantine Item
Parameter	:	[IN] index of the item
******************************************************************************/
IQuarantineItem2* CScanDeliverDLL::GetRejectedQuarantineItem( int nIndex )
{
	if( ( nIndex < 0 ) ||
        ( nIndex > m_nRejectedFiles ))
        {
        return NULL;
        }

    return m_RejectedQItemPointers.GetAt(nIndex);
}// end of GetRejectedQuarantineItem

/******************************************************************************
Name		:	GetAcceptedQuarantineItem
Description	:	Method to get an accepted Quarantine Item
Parameter	:	[IN] index of the item
******************************************************************************/
IQuarantineItem2* CScanDeliverDLL::GetAcceptedQuarantineItem( int nIndex )
{
	if( ( nIndex < 0 ) ||
        ( nIndex >= m_nAcceptedFiles ))
        {
        return NULL;
        }

    return m_AcceptedQItemPointers.GetAt(nIndex);
}// end of GetAcceptedQuarantineItem

/******************************************************************************
// Function name    : CScanDeliverDLL::IsQItemSubmittable
// Description      : This Method verifies whether the specified QItem is 
						submittanble to SARC or not, if not returns the rejection reason
// Return type      : BOOL
// Argument         : IQuarantineItem2 *pQItem - pointer to the QItem Interface
// Argument         : CString& strRejReason - Reference for the CString to hold
						the rejection reason.
******************************************************************************/
BOOL CScanDeliverDLL::IsQItemSubmittable(IQuarantineItem2 *pQItem, CString& strRejReason)
{
	BOOL bWantSubmission = FALSE;	
	DWORD	dwItemCnt = 0;
	DWORD	dwFileType = 0;	
	DWORD dwVirusID = 0;

	//if the item is non repairable, 
	//then we need to submit the Threat to SARC.
	//We are doing this one ahead of unpacking the items, 
	//To avoid unpacking and deleting the unpacked files 
	//if the item doesn't need to be sent.	
	
	HRESULT hrRetVal = pQItem->GetVirusID(&dwVirusID);
	if(FAILED(hrRetVal))
	{
		return FALSE;
	}

	if(0 == dwVirusID)
	{
		strRejReason.LoadString(IDS_TRIVIAL_POSSIBLE_THREAT);
		return FALSE;
	}

	//Get the item type from the session
	hrRetVal = pQItem->GetItemType(&dwFileType);	
	if(SUCCEEDED(hrRetVal))
	{
		if(VBIN_RECORD_LEGACY_INFECTION == dwFileType)
		{			
			hrRetVal = pQItem->GetFileType(&dwFileType);
			if(SUCCEEDED(hrRetVal))
			{			
				if(!(dwFileType & QFILE_TYPE_REPAIRABLE))
						bWantSubmission = true;			
			}
		}
		else	//for all other types of items 
			bWantSubmission = true;
	}
	//If the Item is not submittable return false and load the rejection reason string.
	if(FALSE == bWantSubmission)
		if(FALSE == strRejReason.LoadString(IDS_REFUSE_SUBMIT_REPAIRABLE))
			strRejReason = "This virus can be safely detected and repaired by Norton AntiVirus and should not be submitted for further analysis.";

	return bWantSubmission;
}	//End of IsQItemSubmittable

/******************************************************************************
Name		:	UpdateFieldVal
Description	:	Method to update a specified field with given value in the SARC.TXT
Parameters	:	[IN]	pfSrc	- file pointer to source file.
				[IN]	pfDst	- file pointer to destination file.
				[IN]	strStartTag - start tag of the field
				[IN]	strEndTag - end tag of the field.
				[IN]	strFieldVal - Field Value
				[IN/OUT]lpdwCRC - takes the in CRC and updates the CRC value.
Returns		:	DWORD
******************************************************************************/
DWORD CScanDeliverDLL::UpdateFieldVal(FILE* pfSrc, FILE* pfDst, 
									  LPCTSTR strStartTag, LPCTSTR strEndTag, 
									  LPCTSTR strFieldVal, DWORD *lpdwCRC)
{
	//This function is called when the start tag is hit

	DWORD dwRet = ERROR_SUCCESS;
	DWORD dwToWrite,dwWritten;
	TCHAR szBuf[2048+4];

	//Validation of args
	if(NULL == pfSrc || NULL == pfDst || NULL == strStartTag || NULL == strEndTag || NULL == strFieldVal || NULL == lpdwCRC)
		return E_INVALIDARG;
	
	//Write the Start tag to the dest file
	dwToWrite = _tcslen(strStartTag);
	if(dwToWrite <= 2048)
	{
		sssnprintf(szBuf, sizeof(szBuf), "%s\r\n",strStartTag);		
	}
	else
	{
		memset(szBuf, 0x00, 2052);
		memcpy(szBuf, strStartTag, 2048);
		_tcscat(szBuf,"\r\n");		
	}
	dwToWrite = _tcslen(szBuf);
	dwWritten = _fputts(szBuf, pfDst);
	if(dwWritten < 0 || dwWritten == EOF || dwWritten == WEOF)
	{
		dwRet = GetLastError();
		return dwRet;
	}
	//update the CRC
	*lpdwCRC = CRC32Compute(dwToWrite,(LPBYTE)szBuf,*lpdwCRC);

	//Write the Field Value to the dest file
	dwToWrite = _tcslen(strFieldVal);
	if(dwToWrite <= 2048)
	{
		sssnprintf(szBuf,sizeof(szBuf),"%s\r\n",strFieldVal);		
	}
	else
	{
		memset(szBuf, 0x00, 2052);
		memcpy(szBuf, strFieldVal, 2048);
		_tcscat(szBuf,"\r\n");		
	}
	dwToWrite = _tcslen(szBuf);
	dwWritten = _fputts(szBuf, pfDst);
	if(dwWritten < 0 || dwWritten == EOF || dwWritten == WEOF)
	{
		dwRet = GetLastError();
		return dwRet;
	}
	//update the CRC
	*lpdwCRC = CRC32Compute(dwToWrite,(LPBYTE)szBuf,*lpdwCRC);

	for(int i=0; i<2;i++)
	{
		//Read the next line and look for end tag
		if(NULL == _fgetts(szBuf, 2048, pfSrc))
		{
			dwRet = GetLastError();
			//Failed, either end of file reached or error occured.
			//in both cases it is an error, bcos end tag should be there
			if(feof(pfSrc))
				return -1;
			if(ferror(pfSrc))
				return dwRet;		
		}
		//look for the end tag
		if(_tcsstr(szBuf, strEndTag))
		{
			//end tag found
			//Write the end tag to the dest file
			dwToWrite = _tcslen(szBuf);
			dwWritten = _fputts(szBuf, pfDst);
			if(dwWritten < 0 || dwWritten == EOF || dwWritten == WEOF)
			{
				dwRet = GetLastError();
				return dwRet;
			}
			//update the CRC
			*lpdwCRC = CRC32Compute(dwToWrite,(LPBYTE)szBuf,*lpdwCRC);
			dwRet = ERROR_SUCCESS;
			break;
		}
		else
		{
			//End tag not found, so this should be the field value
			//Ignore and continue
		}
	}
	return dwRet;
} //end of UpdateFieldVal

/******************************************************************************
Name		:	UpdateSARCTXTContent
Description	:	Updates the SarcTxt content with the collected user information.
Arguments	:	strSARCFile - sarc File name.
******************************************************************************/
DWORD CScanDeliverDLL::UpdateSARCTXTContent(LPCSTR strSARCFile)
{
	DWORD dwRet = ERROR_SUCCESS;
	FILE* pfSrc = NULL;
	FILE* pfDst = NULL;
	TCHAR strDstFile[MAX_PATH+1];
	DWORD dwCRC = 0x02;		//SDPACK_VERSION; SDPack version def is not available
	TCHAR szBuf[2048 + 4];
	DWORD dwToWrite, dwWritten;
	TCHAR   szInfoBuf[MAX_INI_BUF_SIZE]                           = "";	

	if(NULL == strSARCFile)
		return E_INVALIDARG;

	if(_tcslen(strSARCFile) >= MAX_PATH)
		return E_INVALIDARG;

	//Prepare a destination file name for the SARC Text, with a different ext.
	_tcsncpy(strDstFile, strSARCFile, MAX_PATH);	
	PathRenameExtension(strDstFile, ".xmp");

	pfSrc = fopen(strSARCFile, "r");
	if(NULL == pfSrc)
	{
		dwRet = GetLastError();
		goto ExitFn;
	}

	pfDst = fopen(strDstFile, "w");
	if(NULL == pfDst)
	{
		dwRet = GetLastError();
		goto ExitFn;
	}

	//TODO - All the string constants for tags are hard coded, because SDPack is not
	//		exposing the string values for tags. Need to update SDPack interface header
	//		with string constants and use them here
	while(!feof(pfSrc))
	{
		if(NULL == _fgetts(szBuf, 2048, pfSrc))
		{
			dwRet = GetLastError();
			if(feof(pfSrc))
			{
				dwRet = ERROR_SUCCESS;
				break;
			}
			if(ferror(pfSrc))
				break;			
		}
		//Look for specific tokens of interest to replace the values.
		if(_tcsstr(szBuf,_T("#00000002:contact_first#")))
		{
			// Fill out the info on the submission
			g_ConfigInfo.GetFirstName( szInfoBuf, MAX_INI_BUF_SIZE );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:contact_first#", "#~00000002:contact_first#", szInfoBuf, &dwCRC);
		}
		else
		if(_tcsstr(szBuf,_T("#00000002:contact_last#")))
		{
			g_ConfigInfo.GetLastName( szInfoBuf, MAX_INI_BUF_SIZE );
            dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:contact_last#", "#~00000002:contact_last#", szInfoBuf, &dwCRC);
		}
		else
		if(_tcsstr(szBuf,_T("#00000002:company_name#")))
		{
			g_ConfigInfo.GetCompany( szInfoBuf, MAX_INI_BUF_SIZE );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:company_name#", "#~00000002:company_name#", szInfoBuf, &dwCRC);
		}
		/*else
		if(_tcsstr(szBuf,_T("#00000002:pin#")))
		{
			g_ConfigInfo.GetPin( szInfoBuf, MAX_INI_BUF_SIZE );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:pin#", "#~00000002:pin#", szInfoBuf, &dwCRC);
		}*/
		else
		if(_tcsstr(szBuf,_T("#00000002:atlas_id#")))
		{
			g_ConfigInfo.GetAtlasNumber( szInfoBuf, MAX_INI_BUF_SIZE );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:atlas_id#", "#~00000002:atlas_id#", szInfoBuf, &dwCRC);
		}
		else
		if(_tcsstr(szBuf,_T("#00000002:address1#")))
		{
			g_ConfigInfo.GetAddress1( szInfoBuf, MAX_INI_BUF_SIZE );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:address1#", "#~00000002:address1#", szInfoBuf, &dwCRC);
		}
		else
		if(_tcsstr(szBuf,_T("#00000002:address2#")))
		{
			g_ConfigInfo.GetAddress2( szInfoBuf, MAX_INI_BUF_SIZE );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:address2#", "#~00000002:address2#", szInfoBuf, &dwCRC);
		}
		else
		if(_tcsstr(szBuf,_T("#00000002:city#")))
		{
			g_ConfigInfo.GetCity( szInfoBuf, MAX_INI_BUF_SIZE );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:city#", "#~00000002:city#", szInfoBuf, &dwCRC);
		}
		else
		if(_tcsstr(szBuf,_T("#00000002:state#")))
		{
			g_ConfigInfo.GetState( szInfoBuf, MAX_INI_BUF_SIZE );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:state#", "#~00000002:state#", szInfoBuf, &dwCRC);
		}
		else
		if(_tcsstr(szBuf,_T("#00000002:zipcode#")))
		{
			g_ConfigInfo.GetZipCode( szInfoBuf, MAX_INI_BUF_SIZE );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:zipcode#", "#~00000002:zipcode#", szInfoBuf, &dwCRC);
		}
		else
		if(_tcsstr(szBuf,_T("#00000002:country#")))
		{
			DWORD dwCountryCode;
			g_ConfigInfo.GetUserCountryCode( &dwCountryCode );
			sssnprintf( szInfoBuf, sizeof(szInfoBuf), "%d", dwCountryCode );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:country#", "#~00000002:country#", szInfoBuf, &dwCRC);
		}
		else
		if(_tcsstr(szBuf,_T("#00000002:phone#")))
		{
			g_ConfigInfo.GetPhone( szInfoBuf, MAX_INI_BUF_SIZE );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:phone#", "#~00000002:phone#", szInfoBuf, &dwCRC);
		}
		else
		if(_tcsstr(szBuf,_T("#00000002:fax#")))
		{
			g_ConfigInfo.GetFax( szInfoBuf, MAX_INI_BUF_SIZE );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:fax#", "#~00000002:fax#", szInfoBuf, &dwCRC);
		}
		else
		if(_tcsstr(szBuf,_T("#00000002:email#")))
		{
			g_ConfigInfo.GetEmail( szInfoBuf, MAX_INI_BUF_SIZE );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:email#", "#~00000002:email#", szInfoBuf, &dwCRC);
		}
		/* Need not update
		else
		if(_tcsstr(szBuf,_T("#00000002:product#")))
		{
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:product#", "#~00000002:product#", "product#", &dwCRC);
		}
		else
		if(_tcsstr(szBuf,_T("#00000002:def_date#")))
		{
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:def_date#", "#~00000002:def_date#", "03032005#", &dwCRC);
		}*/
		else
		if(_tcsstr(szBuf,_T("#00000002:source_country#")))
		{
			DWORD dwCountryCode;
			g_ConfigInfo.GetInfectionCountryCode( &dwCountryCode );
			sssnprintf( szInfoBuf, sizeof(szInfoBuf), "%d", dwCountryCode );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:source_country#", "#~00000002:source_country#", szInfoBuf, &dwCRC);
		}
		else
		if(_tcsstr(szBuf,_T("#00000002:Symptoms#")))
		{
			g_ConfigInfo.GetSymptoms( szInfoBuf, MAX_INI_BUF_SIZE );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:Symptoms#", "#~00000002:Symptoms#", szInfoBuf, &dwCRC);
		}
		else
		if(_tcsstr(szBuf,_T("#00000002:PreDefSymptoms#")))
		{
			g_ConfigInfo.GetPreDefSymptoms( szInfoBuf, MAX_INI_BUF_SIZE );
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:PreDefSymptoms#", "#~00000002:PreDefSymptoms#",szInfoBuf, &dwCRC);
		}
		/*	Need not update
		else
		if(_tcsstr(szBuf,_T("#00000002:language#")))
		{
			dwRet = UpdateFieldVal(pfSrc, pfDst, "#00000002:language#", "#~00000002:language#", "English#", &dwCRC);
		}*/
		else
		if(_tcsstr(szBuf,_T("!~00000002!")))
		{
			//End of file content is reached
			dwToWrite = _tcslen(szBuf);
			dwWritten = _fputts(szBuf, pfDst);
			dwRet = GetLastError();
			if(dwWritten < 0 || dwWritten == EOF || dwWritten == WEOF)
				goto ExitFn;

			//Update the CRC
			dwCRC = CRC32Compute(dwToWrite,(LPBYTE)szBuf,dwCRC);

			//Write the CRC to the dest file
			sssnprintf(szBuf, sizeof(szBuf), "%lu", dwCRC);
			dwToWrite = _tcslen(szBuf);
			dwWritten = _fputts(szBuf, pfDst);
			dwRet = GetLastError();
			if(dwWritten < 0 || dwWritten == EOF || dwWritten == WEOF)
				goto ExitFn;
			//Discard all the content after "!~00000002!"
			while(!feof(pfSrc))	
				_fgetts(szBuf, 2048, pfSrc);

			dwRet = ERROR_SUCCESS;
		}
		else
		{
			//Write the string to dest file and continue
			dwToWrite = _tcslen(szBuf);
			dwWritten = _fputts(szBuf, pfDst);
			dwRet = GetLastError();
			if(dwWritten < 0 || dwWritten == EOF || dwWritten == WEOF)
				goto ExitFn;

			//Update the CRC
			dwCRC = CRC32Compute(dwToWrite,(LPBYTE)szBuf,dwCRC);
		}

		if(dwRet != ERROR_SUCCESS)
			break;
	} //End of while

ExitFn:
	if(pfSrc)
		fclose(pfSrc);
	if(pfDst)
		fclose(pfDst);
	//Rename the dest file to SARC text file and delete the temp dest file.
	if(ERROR_SUCCESS == dwRet)
	{
		MoveFileEx(strDstFile, strSARCFile, MOVEFILE_REPLACE_EXISTING);
		DeleteFile(strDstFile);
	}
	return dwRet;
}	// End of UpdateSARCTXTContent

/******************************************************************************
Name		:	UpdateSARCTxtNEncryptPkg
Description	:	Method to unpack the package, update SARC txt file, recpack 
				and encrypt the package
******************************************************************************/
HRESULT	CScanDeliverDLL::UpdateSARCTxtNEncryptPkg(LPCTSTR strSrcPkg)
{    
    int                 nResult = -1;
    ALPkArchive			*pArchive, *pNewArchive;
    ALEntryList			*pEntryList, *pNewEntryList;
	TCHAR strCurDir[MAX_PATH+1], strExtractFolder[MAX_PATH+1];
	TCHAR str[MAX_PATH+1], strFile[MAX_PATH+1], strDstPkg[MAX_PATH+1];
	CStringArray strDelFiles;
	int iSrcZipFileCnt = -1;
	HRESULT	hr = S_OK;
	int iAddedFiles = 0;
	ALEntry *pEntry = NULL;

	pArchive = NULL;
	pNewArchive = NULL;
	pEntryList = NULL;
	pNewEntryList = NULL;
	strDelFiles.RemoveAll();

	GetCurrentDirectory(MAX_PATH, strCurDir);		
	_tcscpy(strDstPkg, m_szPackageFilename);
	try
	{
		pArchive = new ALPkArchive(strSrcPkg);
	}
	catch(std::bad_alloc)
	{
		return E_OUTOFMEMORY;	
	}
    if (pArchive == NULL)
        return(E_FAIL);
	try
	{
		pEntryList = new ALEntryList(NULL,PkDecompressTools());
	}
	catch(std::bad_alloc)
	{
		delete pArchive; 
		return E_OUTOFMEMORY;	
	}
    if (NULL == pEntryList)
    {
        hr = E_FAIL;
		goto ExitFn;
    }
    try
	{
		nResult = pArchive->ReadDirectory(*pEntryList);
		if (nResult != AL_SUCCESS)
		{
			hr = E_FAIL;
			goto ExitFn;
		}

		iSrcZipFileCnt = pArchive->Extract(*pEntryList);
		if(iSrcZipFileCnt <=0 )
		{		
			hr = E_FAIL;
			goto ExitFn;
		}

		pEntry = pEntryList->GetFirstEntry();
		if(NULL != pEntry && NULL != pEntry->mpStorageObject)
		{
			_tcscpy(str, pEntry->mpStorageObject->mName);	
			PathCombine(strFile, strCurDir, str);
			if(!PathFileExists(strFile))
			{
				hr = E_FAIL;
				goto ExitFn;
			}
			//Update the SARC text file	
			UpdateSARCTXTContent(strFile);	
		}
		else
		{
			hr = E_FAIL;
			goto ExitFn;
		}
	}
	catch(...)
	{
		throw;
	}
	
	try
	{
		//Creaete new Entry List for destination with Compress Tools
		pNewEntryList = new ALEntryList(NULL,PkCompressTools());
	}
	catch(std::bad_alloc)
	{
		if(pEntryList)
			delete pEntryList;
		if(pArchive)
			delete pArchive;
        return(E_OUTOFMEMORY);
	}

    if (pNewEntryList == NULL)
    {
        hr = E_OUTOFMEMORY;
		goto ExitFn;
    }
	
	try
	{
		//Add the files from source entry list	
		ALEntry *pNextEntry  = pEntry;
		CString strQuotedFile = _T("");
		while(NULL != pNextEntry && iAddedFiles <= iSrcZipFileCnt)
		{			
 			if(NULL != pNextEntry->mpStorageObject)
			{
				_tcscpy(str, pNextEntry->mpStorageObject->mName);	
				PathCombine(strFile, strCurDir, str);
				if(!PathFileExists(strFile))
				{
					break;
				}				
				

				//Add the temporory extracted file to list of deletable files.
				strDelFiles.Add(strFile);
				strQuotedFile.Format("\"%s\"", strFile);
				if(pNewEntryList->AddWildCardFiles((LPCSTR)strQuotedFile) != 1)
					break;
				iAddedFiles++;	//Increment the number of files added to the destination Entry List
			}
			pNextEntry  = pEntry->GetNextEntry();
			pEntry = pNextEntry;
		}

		if(!strDelFiles.IsEmpty())
		{	//There is atleast one file extracted to this folder, delete this folder.
			_tcscpy(strExtractFolder, strDelFiles.GetAt(0));
			//Remove the file name from the path.
			PathRemoveFileSpec(strExtractFolder);
			//Add the Directory where the files are extracted.
			m_strDeletableFolderArray.Add(strExtractFolder);
		}
		
		
		if(iAddedFiles == iSrcZipFileCnt)//Number files added to dest EntryList must be equal to the number of files extracted from source package
		{
			try
			{
				pNewArchive = new ALPkArchive(strDstPkg);
			}
			catch(std::bad_alloc)
			{
				if(pArchive)
					delete pArchive;
				if(pEntryList)
					delete pEntryList;
				if(pNewEntryList)
					delete pNewEntryList;
				return E_OUTOFMEMORY;
			}
			if (pNewArchive == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto ExitFn;
			}

			// Mark all entries
			pNewEntryList->SetMarks();
			
			// Compress
			pNewArchive->ClearError();
			
			nResult = pNewArchive->Create(*pNewEntryList);
			if(nResult == iSrcZipFileCnt)
				hr = S_OK;
			else
				hr = E_FAIL;
		}

		//Delete the extracted files.
		if(!strDelFiles.IsEmpty())
		{
			int iSz = strDelFiles.GetSize();
			for ( int idx=0;idx < iSz; idx++)
				DeleteFile(strDelFiles.GetAt(idx));
		}
		
		if(FALSE == m_strDeletableFolderArray.IsEmpty())
		{
			//Cleanup the temporory folders created 
			int iCnt = m_strDeletableFolderArray.GetSize();

			for(int i=0; i<iCnt; i++)
			{
				CString	strCurPath = _T("");
				strCurPath = m_strDeletableFolderArray.GetAt(i);
				while(strCurPath.CompareNoCase(strCurDir) != 0 )
				{
					RemoveDirectory((LPCTSTR)(strCurPath));
					strCurPath = strCurPath.Left(strCurPath.ReverseFind('\\'));
				}
			}
			m_strDeletableFolderArray.RemoveAll();	
		}

	}
	catch(...)
	{
		if(pArchive)
			delete pArchive;
		if(pEntryList)
			delete pEntryList;
		if(pNewArchive)
			delete pNewArchive;
		if(pNewEntryList)
			delete pNewEntryList;
		throw;
	}

ExitFn:
	try
	{
		if(pArchive)
			delete pArchive;
		if(pEntryList)
			delete pEntryList;
		if(pNewArchive)
			delete pNewArchive;
		if(pNewEntryList)
			delete pNewEntryList;

		if(S_OK == hr && PathFileExists(strDstPkg))
		{
			//if(MoveFileEx(strDstPkg, strSrcPkg, MOVEFILE_REPLACE_EXISTING))
			{
				DWORD dwSize = 0;
				hr = EncryptPackage(strDstPkg);
				if(SUCCEEDED(hr))
					hr = CreatePreSubmissionData(strDstPkg);
				return hr;

			}
			//else
			//	return E_FAIL;
		}
	}
	catch(...)
	{ 
		throw;
	}
    return(hr);
}	//End of UpdateSARCTxtNEncryptPkg

/*************************************************************************
Name		:	EncryptPackage
Parameters	:	szFileName - Package name
Description	:   Encrypts the package by XORing each byte with SDPACK_ENCRYPT_ZIP_BYTE.
Returns		:	S_OK              On success
*************************************************************************/
#define SDPACK_ZIP_ENCRYPT_BYTE 0xCD

HRESULT CScanDeliverDLL::EncryptPackage(LPCTSTR szFileName)
{
	HANDLE              hFile;
	DWORD               dwOffset;
	DWORD               dwSize;
	DWORD               dwBytesToEncrypt;
	DWORD               dwTemp;
	BYTE                abyBuf[4096];

	// Open the file for reading/writing
	hFile = CreateFile(szFileName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	// Get the size
	dwSize = GetFileSize(hFile,NULL);
	if (dwSize == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	// Encrypt the file

	dwBytesToEncrypt = sizeof(abyBuf);
	dwOffset = 0;
	while (dwOffset < dwSize)
	{
		if (dwSize - dwOffset < dwBytesToEncrypt)
			dwBytesToEncrypt = dwSize - dwOffset;

		// Read a chunk
		dwTemp = SetFilePointer(hFile,
			dwOffset,
			NULL,
			FILE_BEGIN);

		if (dwTemp != dwOffset)
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (ReadFile(hFile,
			abyBuf,
			dwBytesToEncrypt,
			&dwTemp,
			NULL) == FALSE ||
			dwTemp != dwBytesToEncrypt)
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		// Encrypt chunk
		for (dwTemp=0;dwTemp<dwBytesToEncrypt;dwTemp++)
			abyBuf[dwTemp] ^= SDPACK_ZIP_ENCRYPT_BYTE;

		// Write the chunk
		dwTemp = SetFilePointer(hFile,
			dwOffset,
			NULL,
			FILE_BEGIN);

		if (dwTemp != dwOffset)
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (WriteFile(hFile,
			abyBuf,
			dwBytesToEncrypt,
			&dwTemp,
			NULL) == FALSE ||
			dwTemp != dwBytesToEncrypt)
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		// Update offset
		dwOffset += dwBytesToEncrypt;
	}
	CloseHandle(hFile);
	return S_OK;
}	//End of EncryptPackage

HRESULT CScanDeliverDLL::CreatePreSubmissionData
(
	LPCSTR				szPackageName    
)
{        
    LPBYTE              lpbyData = NULL;
    DWORD               dwSize;
    PPRE_SUBMISSION_HDR pstHdr;
    PPRE_PACKET         pstPacket;
    int                 i;
    int                 nHashPackCount;
      
	m_lpbyBuff = NULL;
    
	nHashPackCount = 1;

    //Allocate memory for submission data
    dwSize = sizeof(PRE_SUBMISSION_HDR_T) +
        nHashPackCount * sizeof(PRE_PACKET_T);

    lpbyData = new BYTE[dwSize];
    if (NULL == lpbyData)
        return(E_OUTOFMEMORY);

    //Initialize header
    pstHdr = (PPRE_SUBMISSION_HDR)lpbyData;
    pstHdr->dwType = PRE_SUBMISSION_HEADER;
    pstHdr->dwVersion = PRE_SUBMISSION_VERSION;
    pstHdr->dwSize = dwSize - 3 * sizeof(DWORD);
    pstHdr->dwTotalNumFiles = nHashPackCount;
    pstHdr->dwNumPackets = nHashPackCount;

    //Iterate through items
    pstPacket = (PPRE_PACKET)(pstHdr+1);    
    for (i=0;i<nHashPackCount;i++)
    {
        pstPacket->dwType = HASH_PACKET_TYPE;
        pstPacket->dwVersion = PRE_SUBMISSION_VERSION;
        pstPacket->dwSize = SUBMIT_BUFFER_SIZE;

        if (PreSubmitDataFill((LPTSTR)szPackageName,
            pstPacket->byTrojanData,
            SUBMIT_BUFFER_SIZE) == FALSE)
        {
            delete[] lpbyData;
            return(E_FAIL);
        }
        ++pstPacket;  
    }

    //Store buffer ptr
    m_lpbyBuff = lpbyData;    

    return(S_OK);
}