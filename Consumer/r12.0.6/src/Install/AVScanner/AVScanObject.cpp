//****************************************************************************
//****************************************************************************
#include "stdafx.h"
#include "AVScanObject.h"
#include "resource.h"
#include "OpenAppsDialog.h"

// timer val
const int TIMERVAL_GetFocusTimer = 0x54; // arbitrary Mongo #54!!

#ifndef _S
#define _S(x)(CString)(LPCTSTR)(x)
#endif

class CRemediationData
{
public:
    CRemediationData() : bManuallyQuarantined(false),
                         eRemediationResult(ccEraser::Success)
    {
    }

    ~CRemediationData()
    {
        if( pUndo != NULL )
        {
            int nRefCount = pUndo.Release();
            if( nRefCount == 0 )
            {
                // We also need to delete the file backing this undo data
                if( strFileName.GetLength() != 0 )
                {
                    DeleteFile(strFileName);
                }
            }
        }
    }

    ccEraser::IRemediationActionPtr pRemediation;
    cc::IStreamPtr pUndo;
    ccEraser::eResult eRemediationResult;
    ATL::CAtlString strFileName;
    bool bManuallyQuarantined;
};


//****************************************************************************
//Constructor initialize some members
//****************************************************************************
CAVScanObject::CAVScanObject(void) 
{	
	m_pSink = NULL;
	m_bAbort = false;

	CString tmpString;   
	tmpString.LoadString(IDS_SCANNER_NAME);
	m_PropertyMap[CString(PREINST_SCAN_NAME)] = tmpString;	
	m_PropertyMap[CString(PREINST_PRIORITY)] = _T("600");
};

//****************************************************************************
//Destructor: cleanup
//****************************************************************************
CAVScanObject::~CAVScanObject(void)
{
	// release the sink
	if(m_pSink)
	{
		m_pSink->Release();
		m_pSink = NULL;
	}
};

//****************************************************************************
//****************************************************************************
SYMRESULT CAVScanObject::RequestAbort()
{
	// set the abort flag in the sink to stop the scan
	if(m_pSink)
		m_pSink->SetAbortFlag();
	
    m_bAbort = false;

	return SYM_OK;
}

//****************************************************************************
//****************************************************************************
SYMRESULT CAVScanObject::FindDefs(LPTSTR szDefsDir, DWORD& dwSize)
{
	CCTRACEI("CAVScanObject::FindDefs() :: Starting.");
    
    TCHAR szIniPath[MAX_PATH+1] = {0};
	TCHAR szModuleDir[MAX_PATH+1] = {0};
	TCHAR *pchBackSlash = NULL;

	// figure out where we are being run fro
	GetModuleFileName(NULL, szModuleDir, MAX_PATH+1);
	pchBackSlash = _tcsrchr(szModuleDir, '\\');
	*pchBackSlash = '\0';
	
	//look for the presence of an ini file to tell us where to get the defs
	_tcscpy(szIniPath, szModuleDir);
	_tcscat(szIniPath, csRelIniPath);
	if(GetFileAttributes(szIniPath) != INVALID_FILE_ATTRIBUTES)
	{
		GetPrivateProfileString(csIniSection, csIniField, _T(""), szDefsDir, dwSize, szIniPath);
	}
	//if no ini file was found check if the hardcoded path is found
	else if(GetFileAttributes(csHardCodedDefsPath) == FILE_ATTRIBUTE_DIRECTORY)
	{
		_tcsncpy(szDefsDir, csHardCodedDefsPath, dwSize);
	}
	else
	{
		//find the default defs on the CD
		_tcsncpy(szDefsDir, szModuleDir, dwSize);
		_tcsncat(szDefsDir, cszRelativeDefsDir, dwSize - _tcslen(szDefsDir));
	}

    CCTRACEI("CAVScanObject::FindDefs() :: Using VirusDefs from: %s", szDefsDir);

	// set the dwSize parameter to the number of chars we copied
	dwSize = _tcslen(szDefsDir);

    CCTRACEI("CAVScanObject::FindDefs() :: Finished.");
	
	return SYM_OK;
}

//****************************************************************************
//****************************************************************************
SYMRESULT CAVScanObject::Scan(HWND m_hParentWnd)
{
	COpenAppsDialog openAppsDialog;
	
    m_Status = STOPPED;
    FireStatus(PROGRESS_TEXT, _T(""));

    DWORD ret = openAppsDialog.DoModal(m_hParentWnd);

	if(ret == IDCANCEL)
	{
        m_Status = FINISHED_SKIPPED;
		return SYM_OK;
	}

    m_Status = RUNNING;
    
    CCTRACEI("CAVScanObject::Scan() :: Starting.");
	
	ATL::CString sLogLine;
    ATL::CString sLogDetails;
    sLogDetails.LoadString(IDS_LOG_DETAILS);

	sLogLine.LoadString(IDS_LOADING_SCANNER);
	FireStatus(PROGRESS_TEXT, sLogLine);

	m_Status = RUNNING;

	// we need to make a new sync
	if(m_pSink)
	{
		m_pSink->Release();
		m_pSink = NULL;
	}
	
	// Create our callback sink with the number of threads needed.
	m_pSink = new CEraserScanSink();
	if(!m_pSink)
    {
        CCTRACEE("CAVScanObject::Scan() :: Failed to create EraserScanSink");
        
		sLogLine.LoadString(IDS_ERR_SCNRCREATEFAILED);
		FireStatus(LOG_TEXT, sLogLine, 55);  //TODO:: Make this error code a real NAV error code

		m_Status = FINISHED_FAILED;
		return SYMERR_OUTOFMEMORY;
    }
	
	m_pSink->AddRef();

    // make sure we haven't been aborted
    if(m_bAbort)
    {
        m_Status = FINISHED_SKIPPED;
		return SYM_OK;
    }

	// the handle to ccScan, eraser, quarantine loader object
	// these need to get released last or crashes could result
	cc::CSymInterfaceTrustedLoader ccEraserLoader;
	cc::CSymInterfaceTrustedLoader ccScanLoader;
    cc::CSymInterfaceTrustedLoader ccQuaratineLoader;

	try
	{	
		// find the defs        		
		TCHAR szDefsDir[MAX_PATH];
		DWORD dwSize = sizeof(szDefsDir);
		FindDefs(szDefsDir, dwSize);

		// get our module directory. we'll need it for a few things.
		TCHAR szModuleDir[MAX_PATH] = {0};
		GetModuleFileName(NULL, szModuleDir, MAX_PATH+1);
		TCHAR* pchBackSlash = _tcsrchr(szModuleDir, '\\');
		*pchBackSlash = '\0';

		// find the path to ccScan and initialize out loader with it
		TCHAR szCCScanPath[MAX_PATH] = {0};
		_tcscpy(szCCScanPath, szModuleDir);
		PathAppend(szCCScanPath, _T("Support\\ccCommon\\ccCommon\\ccScan.dll"));
		SYMRESULT srReturn = ccScanLoader.Initialize(szCCScanPath);
        if (SYM_FAILED(srReturn))
		{
            CCTRACEE("CAVScanObject::Scan() :: Failed to initialize the ccScanLoader. Returned 0x%x", srReturn);
			sLogLine.LoadString(IDS_ERR_SCNRCREATEFAILED);
			FireStatus(LOG_TEXT, sLogLine, srReturn);  
			
			m_Status = FINISHED_FAILED;
            return SYMERR_UNKNOWN;
		}

        // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
	    	return SYM_OK;
        }
		
        CCTRACEI("CAVScanObject::Scan() :: Using ccScan.dll from: %s", szCCScanPath);

		// get a scanner object
		CSymPtr<IScanner> pScanner;
		srReturn = ccScanLoader.CreateObject(IID_Scanner, IID_Scanner, reinterpret_cast<void**>(&pScanner));
		if(SYM_FAILED(srReturn))
		{
			CCTRACEE("CAVScanObject::Scan() :: Failed to load IScanner object. Returned 0x%x.", srReturn);
			sLogLine.LoadString(IDS_ERR_SCNRCREATEFAILED);
			FireStatus(LOG_TEXT, sLogLine, srReturn); 

			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}
        
         // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
	    	return SYM_OK;
        }

		// initialize our scanner with the virus defs path
		TCHAR szTempPath[MAX_PATH];
		GetTempPath(MAX_PATH, szTempPath);
		SCANSTATUS Status = pScanner->Initialize(szDefsDir, "", szTempPath, 2, IScanner::ISCANNER_USE_DEF_PATH );
		if (Status != SCAN_OK)
		{   
            CCTRACEE("CAVScanObject::Scan() :: Failed to intialize IScanner object. Returned 0x%x.", srReturn);
			sLogLine.LoadString(IDS_ERR_SCNRINITFAILED);
			FireStatus(LOG_TEXT, sLogLine, srReturn); 

			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}
		
        // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
	    	return SYM_OK;
        }

		// find the path to ccEraser.dll and initialize our
		// loader with it.
		TCHAR szEraserPath[MAX_PATH];
		_tcscpy(szEraserPath, szDefsDir);
		PathAppend(szEraserPath, _T("ccEraser.dll"));
		srReturn = ccEraserLoader.Initialize(szEraserPath);
		if (SYM_FAILED(srReturn))
		{
            CCTRACEE("CAVScanObject::Scan() :: Failed to initialize ccEraser Loader. Returned 0x%x", srReturn);
			sLogLine.LoadString(IDS_ERR_GSECREATEFAILED);
			FireStatus(LOG_TEXT, sLogLine, srReturn); 

			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}    
        
        CCTRACEI("CAVScanObject::Scan() :: Using ccEraser.dll from: %s", szEraserPath);
        
        // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
	    	return SYM_OK;
        }

		// create the IEraser object
		ccEraser::IEraserPtr pEraser;
		srReturn = ccEraserLoader.CreateObject(ccEraser::IID_Eraser, ccEraser::IID_Eraser, reinterpret_cast<void**>(&pEraser));
		if (SYM_FAILED(srReturn))
		{
            CCTRACEE("CAVScanObject::Scan() :: Failed to create IEraser object. Returned 0x%x", srReturn);
			sLogLine.LoadString(IDS_ERR_GSECREATEFAILED);
			FireStatus(LOG_TEXT, sLogLine, srReturn); 

			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}
	    
        // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
		    return SYM_OK;
        }

        // Initialize the ccEraser scanner.
		ccEraser::eResult eResult;
		eResult = pEraser->Initialize(pScanner, m_pSink, m_pSink);
		if (ccEraser::Failed(eResult))
		{
			CCTRACEE("CAVScanObject::Scan() :: Failed to Initialize IEraser object. Returned 0x%x", eResult);
			sLogLine.LoadString(IDS_ERR_GSEINITFAILED);
			FireStatus(LOG_TEXT, sLogLine, srReturn); 

			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}

        // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
	    	return SYM_OK;
        }

		// find the path to quar32.dll and initialize our loader with it
		TCHAR szQuar32Path[MAX_PATH] = {0};
		_tcscpy(szQuar32Path, szModuleDir);
		PathAppend(szQuar32Path, _T("NAV\\External\\NORTON\\APP\\Quar32.dll"));
		srReturn = ccQuaratineLoader.Initialize(szQuar32Path);
        if(SYM_FAILED(srReturn))
        {
            CCTRACEE("CAVScanObject::Scan() :: Failed to initialize Quarantine Loader. Returned 0x%x.", srReturn);
			sLogLine.LoadString(IDS_ERR_QUARCREATEFAILED);
			FireStatus(LOG_TEXT, sLogLine, srReturn); 

			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
        }
        
        CCTRACEI("CAVScanObject::Scan() :: Using Quar32.dll from: %s", szQuar32Path);

        // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
	    	return SYM_OK;
        }

		// create the quaratine server    
		IQuarantineDLLPtr pQuarServer;
		srReturn = ccQuaratineLoader.CreateObject(IID_QuarantineDLL, IID_QuarantineDLL, reinterpret_cast<void**>(&pQuarServer));
		if(SYM_FAILED(srReturn))
		{
			CCTRACEE("CAVScanObject::Scan() :: Failed to create IQuarantineDLL object. Returned 0x%x.", srReturn);
			sLogLine.LoadString(IDS_ERR_QUARCREATEFAILED);
			FireStatus(LOG_TEXT, sLogLine, srReturn); 

			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}
	   
        // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
		    return SYM_OK;
        }

		IQuarantineDLL2QIPtr pQuarServer2;
		pQuarServer2 = pQuarServer;
		if(!pQuarServer)
		{
            CCTRACEE("CAVScanObject::Scan() :: Failed to QI IQuarantineDLL2 object.");
			sLogLine.LoadString(IDS_ERR_QUARINITFAILED);
			FireStatus(LOG_TEXT, sLogLine, srReturn); 
			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}

		// give our Quarantine Server a pointer to our scanner
		HRESULT hQInit = pQuarServer2->SetScanner(pScanner);
		if(FAILED(hQInit))
		{
			CCTRACEE("CAVScanObject::Scan() :: Failed to set Quarantine scanner pointer. Returned 0x%x.", hQInit);
			sLogLine.LoadString(IDS_ERR_SCNRINITFAILED);
			FireStatus(LOG_TEXT, sLogLine, srReturn); 

			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}

        // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
	    	return SYM_OK;
        }
	    
		// first try to inialize the quarantine server without tweaks.  this will
		// only succeed when quarantine is has been left behind on the machine.
		hQInit = pQuarServer->Initialize();
		if(E_UNABLE_TO_GET_QUARANTINE_PATH == hQInit || E_QUARANTINE_DIRECTORY_INVALID == hQInit)
		{
            CCTRACEI("CAVScanObject::Scan() :: Failed attempt to initialize Quarantine Server. Path unavailable.");
                
            // we can't initialize the quarantine server because the directories aren't set up
			// this is expected in almost all cases.  we need to create a temp quarantine dir
			// and point the reg key to it
	        
			// NOTE: although it seems ugly, there is a good reason for creating the regkey path
			// rather than enhancing the quarantine interface to support having the path passed in.
			// if the user exits the installer after quarantining some files, the installer will be 
			// able to find the quarantine directory later so it can move the files to the real
			// quarantine directory.
        
			// create the quarantine directory in the Temp directory
			TCHAR szQuarantineDir[MAX_PATH] = {0};
            TCHAR szTempDir[MAX_PATH] = {0};
			GetTempPath(sizeof(szQuarantineDir)/sizeof(szQuarantineDir[0]), szQuarantineDir);
			PathAppend(szQuarantineDir, _T("PSQuar"));
            _tcscpy(szTempDir, szQuarantineDir);
			CreateDirectory(szQuarantineDir, NULL);
            CreateDirectory(szTempDir, NULL);
	        
			// create the quarantinepath key and point it to the directory we just created
			ATL::CRegKey rkQuarantine;
			rkQuarantine.Create(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus\\Quarantine"));
			rkQuarantine.SetStringValue(_T("QuarantinePath"), szQuarantineDir);
            rkQuarantine.SetStringValue(_T("TempPath"), szTempDir);

            CCTRACEI("CAVScanObject::Scan() :: Creating Quarantine path at: %s", szQuarantineDir);

			// now try to initialize again
			hQInit = pQuarServer->Initialize();
		}

        // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
	    	return SYM_OK;
        }
	    
		if(E_UNABLE_TO_READ_OPTIONS == hQInit)
		{
            CCTRACEI("CAVScanObject::Scan() :: Failed attempt to initialize Quarantine Server. Options unavailable.");

			// we can't initialize the quarantine object here because we can't
			// find the quaropts.dat file.  we'll create the key to use
			// ours from the CD layout.

			// find the path to our QuarOpts.dat file on the CD
			TCHAR szQuarOptsDat[MAX_PATH] = {0};
			_tcscpy(szQuarOptsDat, szModuleDir);
			PathAppend(szQuarOptsDat, _T("NAV\\External\\NORTON\\APP"));

			ATL::CRegKey rkQuarantine;
			rkQuarantine.Create(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus\\Quarantine"));
			rkQuarantine.SetStringValue(_T("OptionsPath"), szQuarOptsDat);
            
            CCTRACEI("CAVScanObject::Scan() :: Creating options path at: %s", szQuarOptsDat);

			// now try to initialize again
			hQInit = pQuarServer->Initialize();
		}

        // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
	    	return SYM_OK;
        }

		// verify that our quarantine server is initialized
		if(FAILED(hQInit))
		{
			// we've done everything we can at this point, if we still
			// can't initialize Quarantine, we're stuck. error out.

            CCTRACEE("CAVScanObject::Scan() :: Fatal Failure intializing Quarantine Server. Returned 0x%x", hQInit);
			sLogLine.LoadString(IDS_ERR_QUARINITFAILED);
			FireStatus(LOG_TEXT, sLogLine, srReturn);

			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}

		CCTRACEI("CAVScanObject::Scan() :: Starting Eraser Scan.");

        // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
		    return SYM_OK;
        }
        
        // Perform the Eraser scan.
		ccEraser::IContextPtr pContext = NULL;
		ccEraser::IFilterPtr pFilterIn = NULL;
        ccEraser::IFilterPtr pFilterOut = NULL;
        cc::IKeyValueCollectionPtr pSettings = NULL;

        eResult = pEraser->GetSettings(pSettings);
        if (ccEraser::Failed(eResult))
		{
            CCTRACEE("CAVScanObject::Scan() :: Failed to get Eraser Settings.");
			
			sLogLine.LoadString(IDS_ERR_QUARINITFAILED);
			FireStatus(LOG_TEXT, sLogLine, 56);  //TODO:: Get a real error code or a different message

			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}

        // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
	    	return SYM_OK;
        }

        // get the input filter
        eResult = pEraser->CreateObject(ccEraser::FilterType, ccEraser::IID_Filter, reinterpret_cast<void**>(&pFilterIn));
        if (ccEraser::Failed(eResult))
		{
            CCTRACEE("CAVScanObject::Scan() :: Failed to get input Filter.");
			sLogLine.LoadString(IDS_ERR_QUARINITFAILED);
			FireStatus(LOG_TEXT, sLogLine, 57); //TODO:: Get a real error code or a different message
			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}

        // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
	    	return SYM_OK;
        }
        
        // get the output filter
        eResult = pEraser->CreateObject(ccEraser::FilterType, ccEraser::IID_Filter, reinterpret_cast<void**>(&pFilterOut));
        if (ccEraser::Failed(eResult))
		{
            CCTRACEE("CAVScanObject::Scan() :: Failed to get output Filter.");
			sLogLine.LoadString(IDS_ERR_QUARINITFAILED);
			FireStatus(LOG_TEXT, sLogLine, 58); //TODO:: Get a real error code or a different message
			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}

        // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
	    	return SYM_OK;
        }

        pFilterOut->ClearAnomalyFilter();
        pFilterOut->ClearDetectionFilter();
        
        pFilterOut->AddAnomalyCategory(ccEraser::IAnomaly::Viral);
        pFilterOut->AddAnomalyCategory(ccEraser::IAnomaly::Malicious);
        pFilterOut->AddAnomalyCategory(ccEraser::IAnomaly::ReservedMalicious);
        pFilterOut->AddAnomalyCategory(ccEraser::IAnomaly::Heuristic);

        pFilterIn->ClearAnomalyFilter();
        pFilterIn->ClearDetectionFilter();
        
        pFilterIn->AddAnomalyCategory(ccEraser::IAnomaly::GenericLoadPoint);

         // Set the filter in the settings object as the input filter
        if( !pSettings->SetValue(ccEraser::IEraser::InputFilter, pFilterIn) )
        {
            CCTRACEE(_T("CScanManager::SetEraserSettings() - Failed to set ccEraser::IEraser::InputFilter"));
			sLogLine.LoadString(IDS_ERR_QUARINITFAILED);
			FireStatus(LOG_TEXT, sLogLine, 58); //TODO:: Get a real error code or a different message
			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
        }

        // Set the filter in the settings object as the output filter
        if( !pSettings->SetValue(ccEraser::IEraser::OutputFilter, pFilterOut) )
        {
            CCTRACEE(_T("CScanManager::SetEraserSettings() - Failed to set ccEraser::IEraser::OutputFilter"));
			sLogLine.LoadString(IDS_ERR_QUARINITFAILED);
			FireStatus(LOG_TEXT, sLogLine, 59); //TODO:: Get a real error code or a different message
			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
        }

        // make sure we haven't been aborted
        if(m_bAbort)
        {
            m_Status = FINISHED_SKIPPED;
	    	return SYM_OK;
        }

		sLogLine.LoadString(IDS_SCANNING);
		FireStatus(PROGRESS_TEXT, sLogLine);

		eResult = pEraser->Scan(m_pSink, pContext);
        
        CCTRACEI("CAVScanObject::Scan() :: Finished Eraser Scan. Returned 0x%x", eResult);

		// user aborted scan
		if (eResult == ccEraser::eResult::Abort)
		{
            CCTRACEI("CAVScanObject::Scan() :: User Aborted Scan.");
			m_Status = FINISHED_SKIPPED;
			return SYM_OK;
		}

		// scan failed
		if (ccEraser::Failed(eResult) || pContext == NULL)
		{
			CCTRACEE("CAVScanObject::Scan() :: Scan Failed. Returned 0x%x", eResult);
			sLogLine.LoadString(IDS_ERR_SCANFAILED);
			FireStatus(LOG_TEXT, sLogLine, eResult);
			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}
	
		sLogLine.LoadString(IDS_FIXING_ANOMALIES);
		FireStatus(PROGRESS_TEXT, sLogLine);

		// get the anomaly List fromt he context pointer
		ccEraser::IAnomalyListPtr pAnomalyList = NULL;
		pAnomalyList.Attach(pContext->GetDetectedAnomalyList());
		if (!pAnomalyList)
		{
            CCTRACEE("CAVScanObject::Scan() :: Failed to get Anomaly List.");
			sLogLine.LoadString(IDS_ERR_SCANFAILED);
			FireStatus(LOG_TEXT, sLogLine, 60); //TODO:: Get a real error code or a different message
			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}
	    
		// get the anomaly count from the list
		size_t nAnomCount = 0;        
        size_t nRepairedCount = 0;
        size_t nQuarantinedCount = 0;
		if(ccEraser::Failed(pAnomalyList->GetCount(nAnomCount)))
		{
			CCTRACEE("CAVScanObject::Scan() :: Failed to get Anomaly Count.");
			sLogLine.LoadString(IDS_ERR_SCANFAILED);
			FireStatus(LOG_TEXT, sLogLine, 61); //TODO:: Get a real error code or a different message
			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}

        CCTRACEI("CAVScanObject::Scan() :: Scan detected %d Anomalies.", nAnomCount);
	    
		// process each anomaly in the list
		for(size_t nAnomIndex = 0; nAnomIndex < nAnomCount; ++nAnomIndex)
		{	
            CCTRACEI("CAVScanObject::Scan() :: Procesing Anomaly #%d.", nAnomIndex + 1);
            
            // before we being processing the anomaly, gather information
			// we'll need from it to make sure we can continue:

			// get the anomaly, on fail we can't do anything so move on to the next anomaly
			ccEraser::IAnomalyPtr pAnomaly = NULL;
			eResult = pAnomalyList->GetItem(nAnomIndex, pAnomaly);
            if(ccEraser::Failed(eResult)  || !pAnomaly)
            {
                CCTRACEW("CAVScanObject::Scan() :: Failed to get current Anomaly object from the list. Returned 0x%x", eResult);
                continue;
            }
	        
			// get the anomaly properties, on fail we can't do anything so move on to the next anomaly
			cc::IKeyValueCollectionPtr pAnomalyProps;
			eResult = pAnomaly->GetProperties(pAnomalyProps);
            if(ccEraser::Failed(eResult) || !pAnomalyProps)
            {
                CCTRACEW("CAVScanObject::Scan() :: Failed to get Properties from current Anomaly. Returned 0x%x", eResult);
                continue;
            }
	        
			// get the remediation list, on fail we're stuck so move on to the next anomaly.
			ccEraser::IRemediationActionListPtr pRemediationList;
            eResult = pAnomaly->GetRemediationActions(pRemediationList);
			if(ccEraser::Failed(eResult) || !pRemediationList)
            {			
                CCTRACEW("CAVScanObject::Scan() :: Failed to get Remediation List from current Anomaly. Returned 0x%x", eResult);
                continue;
            }
	        
			// figure out how many remediations there are, on fail move on to the next anomaly
			size_t nRemediationCount = 0;
            eResult = pRemediationList->GetCount(nRemediationCount);
			if(ccEraser::Failed(eResult))
            {
                CCTRACEW("CAVScanObject::Scan() :: Failed to get Remediation Count from current Anomaly. Returned 0x%x", eResult);
                continue;
            }

            CCTRACEI("CAVScanObject::Scan() :: Current Anomaly has %d Remediations.", nRemediationCount);
	        
			// need to declare some things we'll need for
			// processing this remeditation list:
	        
			// this indicates if the entire remediation process succeeded.
			bool bSucceeded = true;
	        
			// do we have any undo items?
			bool bHaveAnUndoForAtLeastOneRemediation = false;

			bool bCreatedNewQuarantineItem = false;

			// quarantine item for storing backing up or quarantining files.
			CSymPtr<IQuarantineItem> pQuarItem = NULL;
	        
			// declare our ScanFileInfection pointers.
			IScanFileInfectionQIPtr pScanFileInfection;

			// list of remediation actions we've taken, so we can roll back on failure
			std::vector<CRemediationData> vRemData;

			// process each remediation in the list
			for( size_t nRemeditionItem = 0; nRemeditionItem < nRemediationCount; nRemeditionItem++ )
			{
                CCTRACEI("CAVScanObject::Scan() :: Processing Remediation #%d.", nRemeditionItem + 1);

				// get the current remediation action from list
				ccEraser::IRemediationActionPtr pRem;
                eResult = pRemediationList->GetItem(nRemeditionItem, pRem);
				if( ccEraser::Failed(eResult) || !pRem )
				{
                    CCTRACEW("CAVScanObject::Scan() :: Failed to IRemediation Object. Returned 0x%x.", eResult);
					bSucceeded = false;
					break;
				}

				// get the remediation properties
				cc::IKeyValueCollectionPtr pRemProps;
                eResult = pRem->GetProperties(pRemProps);
				if(ccEraser::Failed(eResult) || !pRemProps)
				{   
                    CCTRACEW("CAVScanObject::Scan() :: Failed to Remediation Properties Object. Returned 0x%x.", eResult);
					bSucceeded = false;
					break;
				}
	            
				// get the remediation type
				ccEraser::eObjectType type;
				eResult = pRem->GetType(type);
                if(ccEraser::Failed(eResult))
				{   
                    CCTRACEW("CAVScanObject::Scan() :: Failed to Remediation Type Object. Returned 0x%x.", eResult);
					bSucceeded = false;
					break;
				}                    
	                
				CRemediationData remData;
				remData.pRemediation = pRem;

				// is it a file infection?
				if( type == ccEraser::InfectionRemediationActionType )
				{
                    CCTRACEI("CAVScanObject::Scan() :: The current Remediation is a FileInfection.");

					ISymBasePtr pSymBase;
					IScanInfectionQIPtr pScanInfection;
	                
					// get the symbase pointer to the scaninfection object
					if(!pRemProps->GetValue(ccEraser::IRemediationAction::ScanInfection, pSymBase) || !pSymBase)
                    {
						CCTRACEW("CAVScanObject::Scan() :: Failed to get ScanInfection Property.");
                        bSucceeded = false;
						break;
					}
	                
					// QI for the scaninfection object
					pScanInfection = pSymBase;
					if(!pScanInfection)
					{
                        CCTRACEW("CAVScanObject::Scan() :: Failed to QI IScanInfection Object. Returned 0x%x", eResult);
						bSucceeded = false;
						break;                        
					}
	                
					// for preinstall scanner, all scaninfections should be fileinfections,
					// QI for the scanfileinfection object
					pScanFileInfection = pScanInfection;
					if(!pScanFileInfection)
					{
                        CCTRACEW("CAVScanObject::Scan() :: Failed to QI IScanFileInfection Object. Returned 0x%x", eResult);
						bSucceeded = false;
						break;
					}
	                
					// we need gather some information about out scanfileinfection
	                
					// get the infected file name
					TCHAR pszFileName[MAX_PATH] = {0};
					_tcsncpy(pszFileName, pScanFileInfection->GetShortFileName(), sizeof(pszFileName));
	                
                    sLogDetails.Append(_T("\r\n"));
                    sLogDetails.Append(pszFileName);

                    CString sInfectedText;
                    sInfectedText.LoadString(IDS_INFECTED);

                    sLogDetails.Append(sInfectedText);

					// get the VID
					DWORD dwVID = pScanFileInfection->GetVirusID();
	                
                    CCTRACEI("CAVScanObject::Scan() :: %s is infected with VID %d", pszFileName, dwVID);

                    // get the virus name
					TCHAR pszVirusName[MAX_PATH] = {0};
					_tcsncpy(pszVirusName, pScanFileInfection->GetVirusName(), sizeof(pszVirusName));

                    sLogDetails.Append(pszVirusName);
                    
					// try to open a existing quarantine item if there is one
					if(FAILED(pQuarServer->LoadQuarantineItemFromFilenameVID(pszFileName, dwVID, &pQuarItem)) || 
						!pQuarItem )
					{
						// if the quarantine object is still empty, then create a new one
						HRESULT hRes = pQuarServer->CreateNewQuarantineItemFromFile(pszFileName,
																				QFILE_TYPE_NORMAL,
																				QFILE_STATUS_BACKUP_FILE,
																				&pQuarItem);
                        if(FAILED(hRes))
						{
							// we couldn't create a quarantine backup so we can't continue
                            CCTRACEW("CAVScanObject::Scan() :: Failed to create Quarantine object for: %s. Returned 0x%x", pszFileName, hRes);
							bSucceeded = false;
							break;
						}
                        else
                        {
                            CCTRACEI("CAVScanObject::Scan() :: Created new Quarantine object for: %s", pszFileName);
        					bCreatedNewQuarantineItem = true;
                        }
					}
                    else
                    {
                        CCTRACEI("CAVScanObject::Scan() :: Loaded existing Quarantine object for: %s", pszFileName);
                    }
	                
					// for this remediation we manually quarantined it, save this fact
					// so that way we dont add it to the quarantine item later
					remData.bManuallyQuarantined = true;
				}
	            
				// check if the current remdiation is undoable
				bool bUndoable = false;
				if(ccEraser::Succeeded(pRem->SupportsUndo(bUndoable)) && bUndoable)
				{
                    CCTRACEI("CAVScanObject::Scan() :: Current Remediation is undoable. Saving undo information.");
					// if its undoable we'll need to create file stream
					// and stream out the undo information into a file in the temp
					// directory.  we don't want to keep all this information
					// in memory because sometimes it gets very large.

					// get a filestream pointer
					ccSym::CFileStreamImplPtr pFileStream;
					pFileStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());

					if (pFileStream == NULL)
					{
						bSucceeded = false;
						break;
					}
	                
					// find the temp directory and create a temp file name to store
					// information in it.
					TCHAR szTempDir[MAX_PATH] = {0};
					GetTempPath(MAX_PATH, szTempDir);
					GetTempFileName(szTempDir, _T("und"), 0, remData.strFileName.GetBuffer(MAX_PATH));
	                
					// hook up our stream object to the file
					if (!pFileStream->GetFile().Open(remData.strFileName, GENERIC_READ | GENERIC_WRITE,
															0, NULL,  CREATE_ALWAYS, 0, NULL))
					{
						// cleanup on failure
                        CCTRACEW("CAVScanObject::Scan() :: Failed to create temporarty storage file for undo information.");
						DeleteFile(remData.strFileName);
						remData.strFileName.Empty();
						bSucceeded = false;
						break;
					}
	                
					// stream the undo information into the file
                    eResult = pRem->GetUndoInformation(pFileStream);
					if(ccEraser::Failed(eResult))
					{
						// cleanup on failure
                        CCTRACEW("CAVScanObject::Scan() :: Failed to store undo information. Returned 0x%x", eResult);
						pFileStream->GetFile().Close();
						pFileStream->GetFile().Delete(remData.strFileName, FALSE);
						remData.strFileName.Empty();
						bSucceeded = false;
						break;
					}
	                
					// save the undo informatio in our remediation object
					// and set the flag to indicate that we have at least
					// one thing to undo in the case of failure
					remData.pUndo = pFileStream.Detach();
					bHaveAnUndoForAtLeastOneRemediation = true;
				}
				
				vRemData.push_back(remData);
				remData.eRemediationResult = pRem->Remediate();
				
				// check if the remediation succeded
				if(ccEraser::Failed(remData.eRemediationResult))
				{
                    CCTRACEW("CAVScanObject::Scan() :: Failed to perform Remediation action. Returned 0x%x", remData.eRemediationResult);
					bSucceeded = false;
                    break;
				}
			} // END: for(each remediation)

			// did one of the remediaitons fail for this anomaly?
			// if so need to rollback
			if(!bSucceeded)
			{
                CCTRACEW("CAVScanObject::Scan() :: There was an error during repair of this Anomaly.  Rolling back.");

				// remove the Quarantine backup item if we created one
				if(true == bCreatedNewQuarantineItem && pQuarItem != NULL)
				{
                    CCTRACEI("CAVScanObject::Scan() :: Deleting newly created Quarantine object.");

                    HRESULT hRes = pQuarItem->DeleteItem();
					if(FAILED(hRes))
					{
					    CCTRACEW("CAVScanObject::Scan() :: Failed to delete Quarantine Item. Returned 0x%x", hRes);
					}
				}

				// loop through our vector of remediation objects
				// and undo each one that needs it.
                CCTRACEI("CAVScanObject::Scan() :: Performing undo actions for all completed remediations.");
				std::vector<CRemediationData>::iterator iter;
				for(iter = vRemData.begin(); iter != vRemData.end(); iter++)
				{
                    eResult = (*iter).pRemediation->Undo((*iter).pUndo);
                    if(ccEraser::Failed(eResult))
					{
					    CCTRACEW("CAVScanObject::Scan() :: Failed to undo a Remediation. Returned 0x%x.", eResult);
					}
				}
			} // END: if(!bSucceeded)
			else
			{
                CCTRACEI("CAVScanObject::Scan() :: All Remediations succeded during repair of this Anomaly.");

				// all remediations succeeded, so add the extra remediation data
				// to the quarantine object
				
                // make sure we have a Quarantine Item
                if(!pQuarItem)
                {
                    HRESULT hRes = S_OK;
                    CCTRACEI("CAVScanObject::Scan() :: Creating empty Qurantine Item for this file-free Anomaly.");
                    hRes = pQuarServer->CreateNewQuarantineItemWithoutFile(QFILE_STATUS_QUARANTINED, &pQuarItem);
                    if(FAILED(hRes))
                    {
                        CCTRACEW("CAVScanObject::Scan() :: Failed to create Qurantine Item for this Anomaly. Returned 0x%x.", hRes);
                    }
                }
                
                if(pQuarItem)
				{
                    // set the anomaly name
					ISymBasePtr pSymBase;
                    cc::IString* pStrName;
                    if( pAnomalyProps->GetValue(ccEraser::IAnomaly::Name, pSymBase) )
                    {
                        if( SYM_SUCCEEDED(pSymBase->QueryInterface(cc::IID_String, reinterpret_cast<void**>(&pStrName))) )
                        {
                            CCTRACEI("CAVScanObject::Scan() :: Setting Anomaly Name: %s.", pStrName->GetStringA);
                            pQuarItem->SetAnomalyName(pStrName->GetStringA());
                        }

                    }
                    
                    // set the anomaly ID
                    cc::IString* pStrID;
                    if( pAnomalyProps->GetValue(ccEraser::IAnomaly::ID, pSymBase) )
                    {
                        if( SYM_SUCCEEDED(pSymBase->QueryInterface(cc::IID_String, reinterpret_cast<void**>(&pStrID))) )
                        {
                            CCTRACEI("CAVScanObject::Scan() :: Setting Anomaly ID: %s.", pStrID->GetStringA);
                            pQuarItem->SetAnomalyID(pStrID->GetStringA());        
                        }
                    }
                    
                    // set the anomaly categories
                    cc::IIndexValueCollectionPtr pCategories;
                    if( pAnomalyProps->GetValue(ccEraser::IAnomaly::Categories, pSymBase) )
                    {                  
                        if( SYM_SUCCEEDED(pSymBase->QueryInterface(cc::IID_IndexValueCollection, reinterpret_cast<void**>(&pCategories.m_p))) )
                        {
                            pQuarItem->SetAnomalyCategories(pCategories);
                        }
                    }
                    
                    // add all the undo information
                    std::vector<CRemediationData>::iterator iter;
                    for(iter = vRemData.begin(); iter != vRemData.end(); iter++)
                    {
                        // Don't add the manually quarantined remediation actions and make sure
                        // the remediation action pointer is valid
                        if( (*iter).pRemediation != NULL && (*iter).bManuallyQuarantined == false )
                        {
                            if( FAILED(pQuarItem->AddRemediationData((*iter).pRemediation, (*iter).pUndo)) )
                                CCTRACEE(_T("CScanManager::HandleAnomaly() - Failed to set the quarantine item remediation undo data"));
                        }

                        if( (*iter).bManuallyQuarantined )
                        {
                            if((*iter).eRemediationResult == ccEraser::Repaired )
                            {
                                pQuarItem->SetFileStatus(QFILE_STATUS_BACKUP_FILE);
                                CString sRepaired;
                                sRepaired.LoadString(IDS_REPAIRED);
                                sLogDetails.Append(sRepaired);
                                ++nRepairedCount;
                            }
                            else if((*iter).eRemediationResult == ccEraser::Success || (*iter).eRemediationResult == ccEraser::Deleted)
                            {
                                pQuarItem->SetFileStatus(QFILE_STATUS_QUARANTINED);
                                CString sQuarantined;
                                sQuarantined.LoadString(IDS_QUARANTINED);
                                sLogDetails.Append(sQuarantined);
                                ++nQuarantinedCount;
                            }
                        }
                    } // END: for(each Remediation data)
                } // END: if(valid quarantine object)
			} // END: if(successful remediations)
		} // END: for(each anomaly)
		

        // log a few things
        sLogLine.LoadString(IDS_LOG_SUMMARY);
	    FireStatus(LOG_TEXT, sLogLine);
	    sLogLine.LoadString(IDS_LOG_NUMVIRS);
	    FireStatus(LOG_TEXT, sLogLine, nAnomCount);
	    sLogLine.LoadString(IDS_LOG_NUMREP);
	    FireStatus(LOG_TEXT, sLogLine, nRepairedCount);
	    sLogLine.LoadString(IDS_LOG_NUMQUAR);
	    FireStatus(LOG_TEXT, sLogLine, nQuarantinedCount);
	    FireStatus(LOG_TEXT, _T("\r\n\r\n"));
		
		sLogLine.LoadString(IDS_SCAN_COMPLETED);
		FireStatus(PROGRESS_TEXT, sLogLine);

        FireStatus(LOG_TEXT, sLogDetails);
                    

	}	// END: try/catch block
	catch(...)
	{
	}

	//TODO:SEND COMPLETED EVENT
	m_Status = FINISHED_SUCCESS;

    return SYM_OK;
}