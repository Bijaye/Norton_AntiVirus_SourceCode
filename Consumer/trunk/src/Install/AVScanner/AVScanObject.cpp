////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//****************************************************************************
//****************************************************************************
#include "stdafx.h"
#include "EraserScanSink.h"
#include "AvScannerRes.h"
#include "OpenAppsDialog.h"
#include "AnomalyRisk.h"
#include "ccSymStringImpl.h"
#include "AVScanObject.h"
#include "ccSymDebugOutput.h"
#include "ccSymMemoryStreamImpl.h"
#include "instopts.h"

#include <HRX.h>
#include <InitGuid.h>
#include <avQBackupTypes.h>


// These are the defined names for the reboot remediation handling
static LPCTSTR REMEDIATION_DATA_FOLDER = _T("NavPsRemediationData");

extern ccSym::CDebugOutput g_DeubgOutput;

using namespace ccScanw;

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
            size_t nRefCount = pUndo.Release();
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
    CString strFileName;
    bool bManuallyQuarantined;
};

#define AVS_RETURN_ON_ABORT()	\
	if ( CheckScanAbort() )		\
		return SYM_OK;


#define AVS_RETURN_FALSE_ON_ABORT()	\
	if ( CheckScanAbort() )		\
		{symResult = SYM_OK;	\
		return FALSE;			\
		}


//
// CAVScanObject class:
//

// static:
SYMRESULT CAVScanObject::Create( CAVScanObject*& pScanObj )
{
	pScanObj = new (std::nothrow) CAVScanObject();
	if ( NULL == pScanObj )
		return SYMERR_OUTOFMEMORY;

	pScanObj->AddRef();
	return SYM_OK;
}


//****************************************************************************
// PROTECTED Constructor initialize some members
//****************************************************************************
CAVScanObject::CAVScanObject(void)
{
	ccLib::CExceptionInfo exInfo;
	try
	{
		CString tmpString;   
		VERIFY( tmpString.LoadString(IDS_SCANNER_NAME) );
		VERIFY( m_strProductName.LoadString(IDS_DISPLAY_NAME) );
		m_PropertyMap[CString(PREINST_SCAN_NAME)] = tmpString;	
		m_PropertyMap[CString(PREINST_PRIORITY)] = _T("600");
		ResetMembersForNewScan();
	}
	CCCATCHMEM(exInfo);
};

//****************************************************************************
// PROTECTED Destructor: cleanup
//****************************************************************************
CAVScanObject::~CAVScanObject(void)
{
	if(m_spQBackup != NULL)
	{
		HRESULT hr = m_spQBackup->Uninitialize();
		if(FAILED(hr))
			CCTRCTXE1(_T("QBackup::Uninit == 0x%08X"), hr);
		m_spQBackup.Release();
	}
}

void CAVScanObject::ResetMembersForNewScan()
{
	m_nRebootRequired = 0;
	m_nAnomalyCount = 0;
	m_nRepairedCount = 0;
	m_nQuarantinedCount = 0;
	m_nAnomalyRepairedCount = 0;
	if(!ccLib::CStringConvert::Copy(_T(""), m_sLogDetails))
	{
		CCTRCTXE0(_T("String Copy Failes"));
	}
}

SYMRESULT CAVScanObject::InitDependentObjects( CEraserScanSinkPtr& pSink, IScannerwPtr& pScanner, cc::CSymInterfaceTrustedLoader& eraserLoader, ccEraser::IEraser4Ptr &pEraser )
{
	SYMRESULT symResult = SYM_OK;
	if ( !InitSink(pSink, symResult) )
		return symResult;

	// find the virus defs
    CString sDefsDir;
	symResult = FindDefs( sDefsDir );
	if ( SYM_FAILED(symResult) )
		return symResult;

	// get our module directory. we'll need it for a few things.
	CString strBasePath;
	if ( !GetLayoutBasePath(strBasePath) )
		return SYMERR_OUTOFMEMORY;

	if ( !InitScanner(sDefsDir, strBasePath, pScanner, symResult) )
		return symResult;

	// load eraser
	if ( !InitEraser(pSink, pScanner, sDefsDir, eraserLoader, pEraser, symResult) )
		return symResult;

	// load quarantine
	if ( !InitQuarantine(pScanner, strBasePath, symResult) )
		return symResult;

	// set eraser filters
	InitEraserFilters( pEraser, symResult );

	return symResult;
}

//****************************************************************************
//****************************************************************************

#pragma warning(push)
#pragma warning(disable:4995)
//this method is not deprecated,  ccEraser::IEraser::RequestAbort is
SYMRESULT CAVScanObject::RequestAbort()
{
	// IMPORTANT: This method will be called on a different thread than the main thread.
	// So don't do anything fancy, unless you add a thread synchronization object:
	m_bCancel = true;
	m_Status = FINISHED_SKIPPED;

	return SYM_OK;
}
#pragma warning(pop)

// Returned path looks something like this: "D:\WinNT\Support\PreScan\..\.."
bool CAVScanObject::GetLayoutBasePath( ATL::CString& str )
{
	ccLib::CExceptionInfo exInfo;
	try
	{
		LPTSTR szBuf = str.GetBuffer( MAX_PATH + 1 );
		if ( szBuf != NULL )
		{
			::GetModuleFileName( g_hModule, szBuf, MAX_PATH );
			szBuf[MAX_PATH] = NULL_CHAR;

			TCHAR* pchBackSlash = _tcsrchr( szBuf, DIRECTORY_DELIMITER_CHAR );
			if ( pchBackSlash != NULL )
				*pchBackSlash = NULL_CHAR;

			str.ReleaseBuffer();
			str += _T("\\..\\..");

			CCTRCTXI1(_T("Base Path = %s"), str);

			return true;
		}
	}
	CCCATCHMEM(exInfo);
	return false;
}

//****************************************************************************
//****************************************************************************
SYMRESULT CAVScanObject::FindDefs(CString& sDefsDir)
{
	ccLib::CExceptionInfo exInfo;
	try
	{
		CCTRCTXI0(_T("Starting."));
	    
		// figure out where we are being run from
		CString strBasePath;
		GetLayoutBasePath( strBasePath );
		
		//look for the presence of an ini file to tell us where to get the defs
		CString sIniPath = strBasePath;
		sIniPath += csRelIniPath;
		if(GetFileAttributes(sIniPath) != INVALID_FILE_ATTRIBUTES)
		{
			DWORD dwSize = 2048;
			GetPrivateProfileString(csIniSection, csIniField, _T(""), sDefsDir.GetBuffer(dwSize), dwSize, sIniPath);
			sDefsDir.ReleaseBuffer();
		}
		//if no ini file was found check if the hardcoded path is found
		else if(GetFileAttributes(csHardCodedDefsPath) == FILE_ATTRIBUTE_DIRECTORY)
		{
			sDefsDir = csHardCodedDefsPath;
		}
		else
		{
			//find the default defs on the CD
			sDefsDir = strBasePath;
			sDefsDir += cszRelativeDefsDir;
		}

		CCTRCTXI1(_T("Using VirusDefs from: %s"), sDefsDir);
		CCTRCTXI0(_T("Finished."));
	}
	CCCATCHMEM(exInfo);
	if(exInfo.IsException())
		return SYMERR_OUTOFMEMORY;

	return SYM_OK;
}

SCANWSTATUS CAVScanObject::GetProperty( const WCHAR* szProperty, int iDefault, int &riProperty  ) const
{
	riProperty = iDefault;

	if( wcscmp(szProperty, SCANW_COMPRESSED_FILES) == 0 )
		riProperty = 0; // Don't scan compressed files.

	if( wcscmp(szProperty, SCANW_DISABLE_AP) == 0 )
		riProperty = 0; // Don't disable AP because we already do it for ourselves.

	if ( wcscmp(szProperty, SCANW_NON_VIRAL_THREATS) == 0 )
		riProperty = 1; // Do scan for non-viral threats.

	return SCANW_OK;
}

SCANWSTATUS CAVScanObject::OnNewItem( const WCHAR* pszItem )
{
	ASSERT( pszItem != NULL );
	if ( NULL == pszItem )
		return SCANW_OK;

	//
	// Don't scan items inside our PSQuar (temporary Quarantine folder):
	//
	CString strPSQuarFolder;
	bool bGetPSQuarPath = GetPSQuarPath( strPSQuarFolder );

	if ( bGetPSQuarPath )
	{
		// Append a backslash to our PSQuar folder:
		ccLib::CExceptionInfo exInfo;
		try
		{
			strPSQuarFolder += DIRECTORY_DELIMITER_CHAR;
		}
		CCCATCHMEM(exInfo);
		if(exInfo.IsException())
			return ccScanw::SCANW_ERROR_MEMORY;

		// Convert pszItem to short path format:
		TCHAR szItemShortPath[MAX_PATH + 1];
		::GetShortPathName( pszItem, szItemShortPath, MAX_PATH );
		szItemShortPath[MAX_PATH] = NULL_CHAR;

		// Convert strPSQuarFolder to short path format:
		TCHAR szPSQuarFolderShortPath[MAX_PATH + 1];
		::GetShortPathName( strPSQuarFolder, szPSQuarFolderShortPath, MAX_PATH );
		szPSQuarFolderShortPath[MAX_PATH] = NULL_CHAR;

		// Convert both paths to lowercase:
		_tcslwr( szItemShortPath );
		_tcslwr( szPSQuarFolderShortPath );

		// Find the first occurrence of szPSQuarFolderShortPath in szItemShortPath:
		LPTSTR szSubString = _tcsstr( szItemShortPath, szPSQuarFolderShortPath );
		if ( szSubString == szItemShortPath )
			return SCANW_FALSE;					// This item is inside our PSQuar folder. Don't scan it.
	}

	return SCANW_OK;
}

bool CAVScanObject::HandleFileInfection (	ccEraser::IRemediationActionPtr& pRem,
                                            QBackup::IQBackupSetPtr&		 pQBSet, 
											bool&							 bCreatedNewQuarantineItem,
											bool&							 bManuallyQuarantined,
											cc::IKeyValueCollectionPtr       pAnomalyProps,
											ATL::CString&					 sLogDetails)
{
	// get the remediation properties
	cc::IKeyValueCollectionPtr pRemProps;
    ccEraser::eResult eResult = pRem->GetProperties(pRemProps);
	if(ccEraser::Failed(eResult) || !pRemProps)
	{   
        CCTRCTXW1(_T("Failed to Remediation Properties Object. Returned 0x%x."), eResult);
		return false;
	}

	ISymBasePtr pSymBase;
	IScanwInfectionQIPtr pScanInfection;
	
	// get the symbase pointer to the scaninfection object
	if(!pRemProps->GetValue(ccEraser::IRemediationAction::ScanInfection, pSymBase) || !pSymBase)
    {
		CCTRCTXW0(_T("Failed to get ScanInfection Property."));
		return false;
	}
	
	// QI for the scaninfection object
	pScanInfection = pSymBase;
	if(!pScanInfection)
	{
        CCTRCTXW1(_T("Failed to QI IScanwInfection Object. Returned 0x%x"), eResult);
		return false;
	}
	
	// for preinstall scanner, all scaninfections should be fileinfections,
	// QI for the scanfileinfection object
	IScanwFileInfectionQIPtr pScanFileInfection = pScanInfection;
	if(!pScanFileInfection)
	{
        CCTRCTXW1(_T("Failed to QI IScanwFileInfection Object. Returned 0x%x"), eResult);
		return false;
	}
	
	// we need gather some information about out scanfileinfection
	const wchar_t* pszFileName = NULL;
	ccLib::CExceptionInfo exInfo;
	try
	{
		// get the infected file name
		SCANWSTATUS status = pScanFileInfection->GetFileName(pszFileName);
		if(SCANW_OK != status || pszFileName == NULL)
		{
			CCTRCTXE1(_T("Failed to get infection file name: %d"), status);
			pszFileName = L"";
		}
		
		ATL::CString sTemp = pszFileName;
		sTemp.Replace(_T("\\\\?\\"), _T(""));

		sLogDetails.Append(_T("\r\n"));
		sLogDetails.Append(sTemp);

		CString sInfectedText;
		VERIFY( sInfectedText.LoadString(IDS_INFECTED) );

		sLogDetails.Append(sInfectedText);

		// get the VID
		DWORD dwVID = 0;
		status = pScanFileInfection->GetVirusIDW(dwVID);
		if(status != SCANW_OK)
		{
			CCTRCTXE1(_T("Failed to get VID: %d"), status);
			//fall thorugh
		}
		
		CCTRCTXI2(_T("CAVScanObject::HandleFileInfection() :: %ls is infected with VID %d"), pszFileName, dwVID);

		// get the virus name
		const wchar_t* pszVirusName = NULL;
		status = pScanFileInfection->GetVirusNameW(pszVirusName);
		if(status != SCANW_OK || pszVirusName == NULL)
		{
			CCTRCTXE1(_T("Failed to get name: %d"), status);
			pszVirusName = L"";
		}

		sLogDetails.Append(pszVirusName);
	}
	CCCATCHMEM(exInfo);
	if(exInfo.IsException())
		return false;

	// try to open a existing quarantine item if there is one
	if (!pQBSet) 
	{
		CCTRCTXI0(_T("LoadQuarantineItemFromUUID failed, Create New from file*****"));

		// if the quarantine object is still empty, then create a new one
        HRESULT symRes = m_spQBackup->CreateBackupSet(pQBSet);
        if(FAILED(symRes))
		{
			CCTRCTXE1(_T("Failed to create QB Set: 0x%08X"), symRes);
			return false;
		}
		//this wont flush to disc, just schedule to be saved
		GUID id = GUID_NULL;
		HRESULT sr = m_spQBackup->StoreBackupSet(pQBSet, &id, false);
		if(FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to store QB Set: 0x%08X"), sr);
			return false;
		}
		sr = pQBSet->SetGivenType(avQBTypes::GT_AVANOMALYSET);
		if(FAILED(sr))
		{
			CCTRCTXE1(_T("SetGivenType() == 0x%08X"), symRes);
			return false;
		}
	}
    else
    {
        CCTRCTXI1(_T("Loaded existing Quarantine object for: %ls"), pszFileName);
    }
	CEZRemediation ezRem(pRem);
	QBackup::IQBackupItemBasePtr pRemItem, pUndoItem;
	if(AddRemediationActionToSet(&ezRem, pQBSet, pRemItem.m_p, pUndoItem.m_p, NULL) == false)
	{
		CCTRCTXE0(_T("Failed to quarantine file"));
        if ( IsScanInfectionFileGone(pScanFileInfection) )
        {
            CCTRCTXW0(_T("The scan infection file we wanted to add to quarantine is already gone...treating as a repair delete and letting the remediation attempt decide the true fate."));
			bManuallyQuarantined = true;
			return true;
		}
		// we couldn't create a quarantine backup so we can't continue
        CCTRCTXW1(_T("Failed to create Quarantine object for: %ls"), pszFileName);
		return false;
	}
    else
    {
        CCTRCTXI1(_T("Created new Quarantine object for: %ls"), pszFileName);
        bCreatedNewQuarantineItem = true;
		// for this remediation we manually quarantined it, save this fact
		// so that way we dont add it to the quarantine item later
		bManuallyQuarantined = true;
    }
    return true;
}

bool CAVScanObject::RollbackAnomalyRemediations(QBackup::IQBackupSetPtr&		pQBSet,
												std::vector<CRemediationData>&	vRemData,
												bool							bCreatedNewQuarantineItem)
{
	// remove the Quarantine backup item if we created one
	if(true == bCreatedNewQuarantineItem && pQBSet != NULL)
	{
        CCTRCTXI0(_T("Deleting newly created Quarantine object."));

        GUID idSet;
        HRESULT hr = pQBSet->GetID(idSet);
		if(SUCCEEDED(hr))
		{
			hr = m_spQBackup->RemoveBackupSet(idSet);
			if(FAILED(hr))
			{
				CCTRCTXW1(_T("Failed to delete Quarantine Item. 0x%08X"), hr);
			}
		}
		else
		{
			CCTRCTXE1(_T("GetID failed: 0x%08X"), hr);
		}
        
	}

	// loop through our vector of remediation objects
	// and undo each one that needs it.
    CCTRCTXI0(_T("Performing undo actions for all completed remediations."));
	std::vector<CRemediationData>::iterator iter;
	int idx = 0;
	for(iter = vRemData.begin(); iter != vRemData.end(); iter++, idx++)
	{
		if(iter->pUndo == NULL)
		{
			CCTRCTXW1(_T("iter->pUndo == NULL #%d"), idx);
			continue;
		}

		if(iter->pRemediation == NULL)
		{
			CCTRCTXE1(_T("iter->pRemediation == NULL #%d"), idx);
			continue;
		}

		ULONGLONG ullPos = 0;
		iter->pUndo->GetPosition(ullPos);
		CCTRCTXI2(_T("Undo Data #%d position: %I64d"), idx, ullPos);

        ccEraser::eResult eResult = (*iter).pRemediation->Undo((*iter).pUndo);
        if(ccEraser::Failed(eResult))
		{
			CCTRCTXE2(_T("Failed to undo Remediation #%d. eResult = %d."), idx, eResult);
		}
		else
			CCTRCTXI2(_T("Successfully undid remediation #%d: eResult = %d"), idx, eResult);
	}

	return true;
}

bool CAVScanObject::AddRemediationActionToSet(CEZRemediation* pAction, QBackup::IQBackupSet* pSet,
                                              QBackup::IQBackupItemBase*& pItemRemAct, QBackup::IQBackupItemBase*& pItemUndo,
                                              cc::IStream* pUndoData)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        // Get the undo data
        cc::IStreamPtr spUndo;
        if(pUndoData)
		{
			CCTRCTXI0(_T("undo data supplied"));
            spUndo = pUndoData;
		}
        else
        {
			CCTRCTXI0(_T("Attempting to load undo data"));
            GetUndoData(*pAction, spUndo);
        }

		cc::IStreamPtr pRemStream;
		pRemStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
		if(pRemStream == NULL)
		{
			CCTRCTXE0(_T("pRemStream == NULL"));
			hrx << E_OUTOFMEMORY;
		}

		cc::ISerializeQIPtr pSerialize = (const ccEraser::IRemediationAction*)(*pAction);
		if(pSerialize == NULL)
		{
			CCTRCTXE0(_T("pSerialize == NULL"));
			hrx << E_NOINTERFACE;
		}

		if(pSerialize->Save(pRemStream) == false)
		{
			CCTRCTXE0(_T("pSerialize->Save() == false"));
			hrx << E_FAIL;
		}

		if(pRemStream->SetPosition(0) == false)
		{
			CCTRCTXE0(_T("SetPosition() == false"));
			hrx << E_FAIL;
		}

        // Create a new QBackup item to hold this file
        CCTRCTXI0(_T("Creating new QBackup item"));
        QBackup::IQBackupItemBasePtr spItemRemAct;
		hrx << pSet->CreateBackupItem(QBackup::itemStream, spItemRemAct);

		QBackup::IQBackupItemBasePtr spItemUndo;
		if(spUndo != NULL)
		{
			// Create a new QBackup item to hold the undo data
			CCTRCTXI0(_T("Creating new QBackup item"));
			hrx << pSet->CreateBackupItem(QBackup::itemStream, spItemUndo);
		}
		else
			CCTRCTXI0(_T("spUndo == NULL"));

        // QI For IQBackupItemSerialize
        CCTRCTXI0(_T("QI'ing for IQBackupStream"));
        QBackup::IQBackupItemStreamQIPtr spSerItem(spItemRemAct);
        if(!spSerItem)
            hrx << E_NOINTERFACE;

        // QI For IQBackupItemStream
        CCTRCTXI0(_T("QI'ing for IQBackupItemStream"));
        QBackup::IQBackupItemStreamQIPtr spStreamItem;
		if(spUndo != NULL)
		{
			spStreamItem = (spItemUndo);
			if(!spStreamItem)
				hrx << E_NOINTERFACE;
		}

        // Backup the remediation action
        CCTRCTXI0(_T("Backing up the remediation action"));
        hrx << spSerItem->Backup(pRemStream);

		GUID idStream = GUID_NULL;
		GUID idRemAct = GUID_NULL;

		CCTRCTXI0(_T("Setting the given type for rem action"));
		hrx << spItemRemAct->SetGivenType(avQBTypes::GT_AVREMEDIATIONACTION);

        CCTRCTXI0(_T("Storing the remediation action in QBackup"));
        hrx << pSet->StoreBackupItem(spItemRemAct, &idRemAct);

		if(spUndo != NULL)
		{
			// Backup the stream
			CCTRCTXI0(_T("Backing up the undo data"));
			hrx << spStreamItem->Backup(spUndo);
		
			CCTRCTXI0(_T("Setting the given type for undo data"));
			hrx << spStreamItem->SetGivenType(avQBTypes::GT_AVREMEDIATIONACTIONUNDO);

			CCTRCTXI0(_T("Storing the undo data in QBackup"));
			hrx << pSet->StoreBackupItem(spStreamItem, &idStream);

			cc::IKeyValueCollectionPtr spStreamProps;
			CCTRCTXI0(_T("Getting undo properties"));
			hrx << spStreamItem->GetProperties(spStreamProps);
			CCTRCTXI0(_T("Seting remActId"));
			hrx << spStreamProps->SetValue(avQBTypes::avRAUProp_MatchingRemActID, idRemAct);
		}

        // Get the property collects for both items
        CCTRCTXI0(_T("Getting the property collections for two items"));
        cc::IKeyValueCollectionPtr spRemActProps;
        hrx << spItemRemAct->GetProperties(spRemActProps);

        // Set properties and link the two items
        CCTRCTXI0(_T("Swapping lookup GUIDs"));
        hrx << spRemActProps->SetValue(avQBTypes::avRAProp_MatchingUndoID, idStream);

		CCTRCTXI0(_T("Flushing backup set"));
		hrx << pSet->Flush();
        

        // Return the items to the caller
        pItemRemAct = spItemRemAct.Detach();
        pItemUndo = spStreamItem.Detach();

        // Done
        hrx << SYM_OK;
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);

    if(FAILED(hr))
    {
        CCTRCTXE1(_T("Last operation failed! (hr=%08X)"), hr);
        return false;
    }

    return true;
}

bool CAVScanObject::GetUndoData(ccEraser::IRemediationAction* pAction, cc::IStream*& pStream)
{
    if(pAction == NULL)
    {
        CCTRCTXE0(_T("The remediation action is not valid.  pAction == NULL"));
        return false;
    }

    ccEraser::eResult eRes = ccEraser::Success;
    bool bUndoable = false;

    eRes = pAction->SupportsUndo(bUndoable);
    if(ccEraser::Failed(eRes))
    {
        CCTRCTXE1(_T("SupportsUndo() failed. eResult = %d"), eRes);
        return false;
    }
    else if(!bUndoable)
    {
        CCTRCTXI0(_T("The remediation action does not support undo."));
        return false;
    }
    else
    {
        ccSym::CFileStreamImplPtr pFileStream;
        pFileStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());

        if (pFileStream == NULL)
        {
            CCTRCTXE0(_T("File stream is NULL."));
            return false;
        }

        TCHAR szTempDir[MAX_PATH] = {0};
        if(GetTempPath(MAX_PATH, szTempDir) == 0)
            CCTRCTXE0(_T("Failed to get the temp path."));

		ccLib::CExceptionInfo exInfo;
		ccLib::CString strUndoFileName;
		try
		{
			if(0 == GetTempFileName(szTempDir, _T("und"), 0, strUndoFileName.GetBuffer(MAX_PATH+1)))
			{
				CCTRCTXE1(_T("Failed to get temp file: %d"), GetLastError());
				strUndoFileName.ReleaseBuffer();
				return false;
			}
		}
		CCCATCHMEM(exInfo);
		if(exInfo.IsException())
			return false;
		
		CCTRCTXI1(_T("Temp file: %s"), strUndoFileName);

        if(FALSE == pFileStream->GetFile().Open(strUndoFileName, 
                                                GENERIC_READ | GENERIC_WRITE, 0, NULL, 
                                                CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, NULL))
        {
            CCTRCTXE1(_T("pFileStream->GetFile().Open() failed for file %s"), (LPCTSTR)strUndoFileName);
            DeleteFile(strUndoFileName);
            return false;
        }

        // Get the undo information
        eRes = pAction->GetUndoInformation(pFileStream);
        if(ccEraser::Failed(eRes))
        {
            CCTRCTXE1(_T("Error getting undo information. eResult = %d"), eRes);
            pFileStream->GetFile().Close();
            return false;
        }

		ULONGLONG ullPos = 0;
		if(pFileStream->GetPosition(ullPos) && ullPos != 0)
		{
			CCTRCTXW1(_T("Resetting undo stream from %I64d to 0"), ullPos);
			if(pFileStream->SetPosition(0) == false)
				CCTRCTXE0(_T("pFileStream->SetPosition(0) == false"));
		}

        pStream = pFileStream.Detach();
    }

    return true;
}

bool CAVScanObject::SetQuarItemProperties (QBackup::IQBackupSetPtr&		    pQBSet,
										   cc::IKeyValueCollectionPtr&		pAnomalyProps,
										   std::vector<CRemediationData>&	vRemData,
										   ATL::CString&					sLogDetails,
										   bool                             bHaveAnUndoForAtLeastOneRemediation)
{
	// make sure we have a Quarantine Item
    if(pQBSet == NULL && vRemData.size()>0 && bHaveAnUndoForAtLeastOneRemediation)
    {
        CCTRCTXI0(_T("Creating empty Qurantine Item for this file-free Anomaly."));
        HRESULT hr = m_spQBackup->CreateBackupSet(pQBSet);
        if(FAILED(hr))
        {
            CCTRCTXE1(_T("Failed to create Qurantine Item for this Anomaly. Returned 0x%x."), hr);
			return false;
        }
		hr = pQBSet->SetGivenType(avQBTypes::GT_AVANOMALYSET);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("SetGivenType() == 0x%08X"), hr);
			return false;
		}
		GUID id;
		CCTRCTXI0(_T("Storing backup set"));
		hr = m_spQBackup->StoreBackupSet(pQBSet, &id, false);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to save the quarantine item: 0x%08X"), hr);
			return false;
		}
    }
    
    if(pQBSet)
	{
        cc::IKeyValueCollectionPtr spProps;
        HRESULT hr = pQBSet->GetProperties(spProps);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("GetProps failed: 0x%08X"), hr);
			return false;
		}

        // set the anomaly name
		ISymBasePtr pSymBase;
        cc::IString* pStrName;
        if( pAnomalyProps->GetValue(ccEraser::IAnomaly::Name, pSymBase) )
        {
            if( SYM_SUCCEEDED(pSymBase->QueryInterface(cc::IID_String, reinterpret_cast<void**>(&pStrName))) )
            {
                CCTRCTXI1(_T("Setting Anomaly Name: %ls."), pStrName->GetStringW());
//                spProps->SetValue(avQBTypes::avThreatProp_Name, pStrName->GetStringW());
            }

        }
        
        // set the anomaly ID
        cc::IStringPtr pStrID;
        if( pAnomalyProps->GetValue(ccEraser::IAnomaly::ID, pSymBase) )
        {
            if( SYM_SUCCEEDED(pSymBase->QueryInterface(cc::IID_String, reinterpret_cast<void**>(&pStrID))) )
            {
                CCTRCTXI1(_T("Setting Anomaly ID: %ls."), pStrID->GetStringW());
//                spProps->SetValue(avQBTypes::avASProp_AnomalyID, pStrID->GetStringW());
            }
        }
        
        // set the anomaly categories
		BOOL bNonViral = FALSE;
        cc::IIndexValueCollectionPtr pCategories;
        if( pAnomalyProps->GetValue(ccEraser::IAnomaly::Categories, pSymBase) )
        {                  
            if( SYM_SUCCEEDED(pSymBase->QueryInterface(cc::IID_IndexValueCollection, reinterpret_cast<void**>(&pCategories.m_p))) )
            {
				bNonViral = IsCategoryNonViral(pCategories);
//                spProps->SetValue(avQBTypes::avThreatProp_Categories, pCategories);
            }
        }
        
		// Set the damage flag values for non-viral items
        if( bNonViral )
        {
			DWORD dwStealth = 0, dwRemoval = 0, dwPerformance = 0, dwPrivacy = 0;
			if(pAnomalyProps->GetValue(ccEraser::IAnomaly::Stealth, dwStealth) &&
				pAnomalyProps->GetValue(ccEraser::IAnomaly::Removal, dwRemoval) &&
				pAnomalyProps->GetValue(ccEraser::IAnomaly::Performance, dwPerformance) &&
				pAnomalyProps->GetValue(ccEraser::IAnomaly::Privacy, dwPrivacy)
				)
			{
//                spProps->SetValue(avQBTypes::avThreatProp_Stealth, dwStealth);
//                spProps->SetValue(avQBTypes::avThreatProp_Removal, dwRemoval);
//                spProps->SetValue(avQBTypes::avThreatProp_Performance, dwPerformance);
//                spProps->SetValue(avQBTypes::avThreatProp_Privacy, dwPrivacy);
			}

            // Also set dependency flag
			DWORD dwDependency = 0;
			if(pAnomalyProps->GetValue(ccEraser::IAnomaly::Dependency, dwDependency))
			{
				bool bHasKnownDependencies = dwDependency != 0;
//                spProps->SetValue(avQBTypes::avThreatProp_Dependencies, bHasKnownDependencies);
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
                QBackup::IQBackupItemBasePtr spA, spB;
                CEZRemediation ezRem((*iter).pRemediation);
                AddRemediationActionToSet(&ezRem, pQBSet, spA, spB, (*iter).pUndo);
            }

            if( (*iter).bManuallyQuarantined )
            {
				ccLib::CExceptionInfo exInfo;
				try
				{
					if((*iter).eRemediationResult == ccEraser::Repaired )
					{
						//CString sRepaired;
						//VERIFY( sRepaired.LoadString(IDS_REPAIRED) );
						//sLogDetails.Append(sRepaired);
						++m_nRepairedCount;
					}
					else if((*iter).eRemediationResult == ccEraser::Success || (*iter).eRemediationResult == ccEraser::Deleted)
					{
						//CString sQuarantined;
						//VERIFY( sQuarantined.LoadString(IDS_QUARANTINED) );
						//sLogDetails.Append(sQuarantined);
						++m_nQuarantinedCount;
					}
				}
				CCCATCHMEM(exInfo);
				if(exInfo.IsException())
					return false;
            }
        } // END: for(each Remediation data)
    } // END: if(valid quarantine object)

	return true;

}

ccEraser::eResult CAVScanObject::OnAnomalyDetected( ccEraser::IAnomaly* pAnomaly, const ccEraser::IContext* pContext )
{
	// make sure our Anomaly pointer is good
    if( NULL == pAnomaly )
        return ccEraser::Continue;

	// Currently, this object is used only with SaveRebootRemediationsToDisk()
	CEZAnomaly EZAnomaly;
    if ( !EZAnomaly.Initialize(pAnomaly) )
    {
        CCTRCTXE0(_T("Failed to initialize the anomaly data."));
        return ccEraser::Continue;
    }

	// Determine the severity level before proceeding
	CAnomalyRisk anomalyRisk;
	if(anomalyRisk.ShouldRemediateAnomaly(pAnomaly) == FALSE)
	{
		CCTRCTXI0( _T("CAnomalyRisk::ShouldRemediateAnomaly() returned FALSE. Ignoring Anomaly.") );
        return ccEraser::Continue;
	}

	m_nAnomalyCount++; //increment the anomaly count

	ccLib::CExceptionInfo exInfo;
	try
	{
		CString sLogLine;
		VERIFY( sLogLine.LoadString(IDS_FIXING_ANOMALIES) );
		FireStatus(PROGRESS_TEXT, sLogLine);
	}
	CCCATCHMEM(exInfo);
	if(exInfo.IsException())
		return ccEraser::OutOfMemory;

	// get the anomaly properties, on fail we can't do anything so move on to the next anomaly
	cc::IKeyValueCollectionPtr pAnomalyProps;
	ccEraser::eResult eResult = pAnomaly->GetProperties(pAnomalyProps);
    if(ccEraser::Failed(eResult) || !pAnomalyProps)
    {
        CCTRCTXW1( _T("Failed to get Properties from current Anomaly. Returned 0x%x"), eResult);
        return ccEraser::Continue;
    }

	// get the remediation list, on fail we're stuck so move on to the next anomaly.
	ccEraser::IRemediationActionListPtr pRemediationList;
    eResult = pAnomaly->GetRemediationActions(pRemediationList);
	if(ccEraser::Failed(eResult) || !pRemediationList)
    {			
        CCTRCTXW1( _T("Failed to get Remediation List from current Anomaly. Returned 0x%x"), eResult);
        return ccEraser::Continue;
    }

	// figure out how many remediations there are, on fail move on to the next anomaly
	size_t nRemediationCount = 0;
    eResult = pRemediationList->GetCount(nRemediationCount);
	if(ccEraser::Failed(eResult))
    {
        CCTRCTXW1( _T("Failed to get Remediation Count from current Anomaly. Returned 0x%x"), eResult);
        return ccEraser::Continue;
    }

    CCTRCTXI1( _T("Current Anomaly has %d Remediations."), nRemediationCount);

	bool	bAnomalyRequestedReboot = false;
	bool	bSucceeded = true;								// this indicates if the entire remediation process succeeded.
	bool	bHaveAnUndoForAtLeastOneRemediation = false;	// do we have any undo items?
	bool	bCreatedNewQuarantineItem = false;

    QBackup::IQBackupSetPtr spQBSet;
	IScanwFileInfectionQIPtr			pScanFileInfection;	// declare our ScanFileInfection pointers.
	std::vector<CRemediationData>	vRemData;			// list of remediation actions we've taken, so we can roll back on failure


	GUID idSet;
	if (EZAnomaly.GetQBackupSetID(idSet))
	{
		CCTRCTXI0(_T("This is a filename\\vid detected item, will attempt to load an existing qurantine item."));
        HRESULT hr = m_spQBackup->RetrieveBackupSetByID(idSet, spQBSet);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to load the existing qbackup set item: 0x%08X"), hr);
		}
	}

	//////////////////////////////////////////////////
	//		process each remediation in the list	//
	//////////////////////////////////////////////////
	for( size_t nRemeditionItem = 0; nRemeditionItem < nRemediationCount; nRemeditionItem++ )
	{
        CCTRCTXI1( _T("Processing Remediation #%d."), nRemeditionItem);

		// get the current remediation action from list
		ccEraser::IRemediationActionPtr pRem;
        eResult = pRemediationList->GetItem(nRemeditionItem, pRem);
		if( ccEraser::Failed(eResult) || !pRem )
		{
			CCTRCTXE1( _T("Failed to IRemediation Object. Returned 0x%x."), eResult);
			bSucceeded = false;
			break;
		}
	    
		
		cc::IStringPtr pStrDesc;
		LPCWSTR pszRemDescription = L"[No Description]";

		if(ccEraser::Succeeded(pRem->GetDescription(pStrDesc)) && pStrDesc != NULL && pStrDesc->GetLength() > 0)
			pszRemDescription = pStrDesc->GetStringW();

		CCTRCTXI2(_T("Remediaiton #%Iu: %ls"), nRemeditionItem, pszRemDescription);

		// get the remediation type
		ccEraser::eObjectType type;
		eResult = pRem->GetType(type);
        if(ccEraser::Failed(eResult))
		{   
            CCTRCTXE1( _T("Failed to Remediation Type Object. Returned 0x%x."), eResult);
			bSucceeded = false;
			break;
		}                    
	        
		CRemediationData remData;
		remData.pRemediation = pRem;
		
		// is it a file infection?
		if( type == ccEraser::InfectionRemediationActionType )
		{
            CCTRCTXI0( _T("The current Remediation is a FileInfection.") );
			if (!HandleFileInfection(pRem, spQBSet, bCreatedNewQuarantineItem, remData.bManuallyQuarantined, pAnomalyProps, m_sLogDetails))
			{
				CCTRCTXW0( _T("HandleFileInfection() failed. Returned false.") );
				bSucceeded = false;
				break;
			}
		}
		else if( type == ccEraser::ProcessRemediationActionType || type == ccEraser::ServiceRemediationActionType )
        {
            bool bIsPresent = false;
			eResult = pRem->IsPresent(bIsPresent);

            if( ccEraser::Failed(eResult)|| !bIsPresent)
			{
				CCTRCTXW1(_T("Process/Service: res=%d or not present"), eResult);
                continue;
			}
		}
	    
		if ( GetUndoData(pRem, remData.pUndo, remData.strFileName) ) 
		{
			bHaveAnUndoForAtLeastOneRemediation = true;
		}
		
		if (!PerformRemediationAction(pRem, remData.eRemediationResult))
		{
			if (remData.eRemediationResult == ccEraser::FileStillPresent && bAnomalyRequestedReboot)
			{
				CCTRCTXI0( _T("Current Remediation has returned FileStillPresent. Not treating this as an error.") );
			}
			else if(remData.eRemediationResult == ccEraser::NotImplemented)
			{
				CCTRCTXE1(_T("Cannot remediate in preinstall mode: %ls"), pszRemDescription);
				cc::IStringPtr pDisplay;
				CEZRemediation ezRem(pRem);
				if(ezRem.GetDisplayString(pDisplay.m_p) && pDisplay != NULL && pDisplay->GetLength() > 0)
				{
					if(ccLib::CStringConvert::Append(m_sLogNotImplemented, pDisplay->GetStringW()))
						ccLib::CStringConvert::Append(m_sLogNotImplemented, _T("\r\n"));
				}
			}
			else if(IsFatalError(pRem))
			{
				CCTRCTXE3(_T("Failed to remediatie #%Iu (%ls).  Aborting remediations for this anomaly (Result = %d)"), nRemeditionItem, pszRemDescription, remData.eRemediationResult);
				bSucceeded = false;
				//we will treat this as a failure of the anomaly, and abort processing it.
				//we will try to rollback the already performed remediations below.
				break;
			}
			else
			{
				CCTRCTXE3(_T("Failed to remediatie #%Iu (%ls).  Ignoring error %d"), nRemeditionItem, pszRemDescription, remData.eRemediationResult);
			}
		}
		else if (remData.eRemediationResult == ccEraser::RebootRequired)
		{
			CCTRCTXI0(_T("RebootRequired"));

			if (!bAnomalyRequestedReboot)
				bAnomalyRequestedReboot = true;
		}

		CEZRemediation EZRem( pRem );
		if ( EZRem.ShouldSaveRemediation() )
		{
			ccLib::CExceptionInfo exInfo;
			try
			{
				vRemData.push_back(remData);
			}
			CCCATCHMEM(exInfo);
			if(exInfo.IsException())
				return ccEraser::OutOfMemory;
		}
		else
			CCTRCTXI1(_T("Not saving remediation #%Iu"), nRemeditionItem);

	} // END: for(each remediation)

	// did one of the remediaitons fail for this anomaly?
	// if so need to rollback
	if(!bSucceeded)
	{
        CCTRCTXE0( _T("There was an error during repair of this Anomaly.  Rolling back.") );
		RollbackAnomalyRemediations(spQBSet,  vRemData, bCreatedNewQuarantineItem);
		
	} // END: if(!bSucceeded)
	else
	{
		++m_nAnomalyRepairedCount;
        CCTRCTXI0( _T("All Remediations succeeded during repair of this Anomaly.") );
		SetQuarItemProperties(spQBSet, pAnomalyProps, vRemData, m_sLogDetails, bHaveAnUndoForAtLeastOneRemediation);
        
        // Set the quarantine item UUID in the user data
        GUID guid;
        if ( spQBSet && SUCCEEDED(spQBSet->GetID(guid)) )
            EZAnomaly.SetQBackupSetID(guid);
        else
            CCTRCTXE0(_T("Failed to get the quarantine item UUID."));

		if(bAnomalyRequestedReboot != false)
		{
			++m_nRebootRequired;
			CCTRCTXI1(_T("Anomalies requiring reboot: %d"), m_nRebootRequired);

			cc::IStringPtr pAnomalyName;
			if(EZAnomaly.GetName(pAnomalyName.m_p) && pAnomalyName != NULL && pAnomalyName->GetLength() > 0)
			{
				ccLib::CStringConvert::AppendFormat(m_sLogRebootRisks, _T("%ls\r\n"), pAnomalyName->GetStringW());
			}
			else
				CCTRCTXE0(_T("Failed to get anomaly name!"));
		}
	} // END: if(successful remediations)


    return ccEraser::Continue;
}


bool CAVScanObject::PerformRemediationAction(ccEraser::IRemediationActionPtr pRem, 
											 ccEraser::eResult& eRemResult)
{
    if ( pRem == NULL)
    {
        CCTRCTXE0(_T("The remediation action is not valid."));
        return false;
    }

	CEZRemediation		ezRem(pRem);

	if(ezRem.GetState() != ccEraser::IRemediationAction::NotRemediated)
	{
		CCTRCTXE1(_T("Already remediated: %d.  NothingToDo"), ezRem.GetState());
		eRemResult = ccEraser::NothingToDo;
		return true;
	}

	CCTRCTXI0(_T("calling Remediate()"));
	// Perform the remediation action
	eRemResult = ezRem->Remediate();
	CCTRCTXI1(_T("Finished calling Remediate(), %d"), eRemResult);

    // Get the type
    ccEraser::eObjectType	type = ezRem.GetType();
	IScanwInfectionPtr		spInfection = NULL;
	long					lUserData = InfectionStatus_Unhandled;
	cc::IKeyValueCollectionPtr pUserDataCollection = NULL;

	if ( type == ccEraser::InfectionRemediationActionType )
	{
		if (spInfection = ezRem.GetScanInfection())
			spInfection->GetUserDataW(pUserDataCollection);
	}

	// Did the item not exist at all?
	if ( eRemResult == ccEraser::FileNotFound || 
			eRemResult == ccEraser::NothingToDo )
	{
		CCTRCTXW0(_T("Remediation action did not exist. Nothing to remediate."));

		if ( spInfection )
			pUserDataCollection->SetValue(UIInfectionStatus, (ULONG)InfectionStatus_NotDetected);

        // This is a success
        return true;
	}

	// check if the remediation succeded
	if(ccEraser::Failed(eRemResult))
	{
		// For infection items we want to see if the infection path or 
        // container was removed already
        if ( spInfection && IsScanInfectionFileGone(spInfection) )
        {
            CCTRCTXW0(_T("The infected file could not be found. Treating as nothing to do."));

            // Update the status to not detected
			pUserDataCollection->SetValue(UIInfectionStatus, (ULONG)InfectionStatus_NotDetected);

            // Set the remediation result to nothing to do
            cc::IKeyValueCollectionPtr pProps = ezRem.GetProperties();

            if ( pProps )
            { 
                pProps->SetValue(ccEraser::IRemediationAction::RemediationResult, 
                                 static_cast<DWORD>(ccEraser::NothingToDo));
            }

            return true;
        }

		CCTRCTXE1( _T("Failed to perform Remediation action. Returned 0x%x"), eRemResult);
		
		if ( spInfection )
		{
			// Set the appropriate user data value for this failure so we show 
			// proper status
			if ( eRemResult == ccEraser::AccessDenied )
			{
				pUserDataCollection->SetValue(UIInfectionStatus, (ULONG)InfectionStatus_AccessDenied);
			}
			else if ( lUserData == InfectionStatus_Repaired )
			{
				pUserDataCollection->SetValue(UIInfectionStatus, (ULONG)InfectionStatus_Repair_Failed);
			}
			else if ( lUserData == InfectionStatus_Quarantined )
			{
				pUserDataCollection->SetValue(UIInfectionStatus, (ULONG)InfectionStatus_Quarantine_Failed);
			}
			else
			{
				pUserDataCollection->SetValue(UIInfectionStatus, (ULONG)InfectionStatus_Delete_Failed);
			}
		}
		
		return false;
	}
	else if ( ccEraser::RebootRequired == eRemResult)
	{
		if (spInfection)
			pUserDataCollection->SetValue(UIInfectionStatus, (ULONG)InfectionStatus_RebootRequired);

		// If this is a file type save off the file path so we'll know if a 
		// future remediation action needs to be saved and performed after 
		// reboot
		if( type == ccEraser::FileRemediationActionType ||
			type == ccEraser::ServiceRemediationActionType || 
			type == ccEraser::InfectionRemediationActionType || 
			type == ccEraser::ProcessRemediationActionType )
		{

			ATL::CAtlString strPath;
			cc::IKeyValueCollection* pUserData = ezRem.GetUserData();

			if (pUserData != NULL && !pUserData->GetExists(RemediationDescription))
			{
				IScanwInfectionPtr pInfection = ezRem.GetScanInfection();
				IScanwFileInfectionQIPtr pFile2 = pInfection;
				if (pFile2)
				{
					const wchar_t* pszPath = NULL;
					SCANWSTATUS status = pFile2->GetFileName(pszPath);
					if(SCANW_OK != status || pszPath == NULL)
					{
						CCTRCTXE1(_T("GetFileName failed: %d"), status);
						pszPath = NULL;
					}
					cc::IStringPtr pStr;
					pStr.Attach(ccSym::CStringImpl::CreateStringImpl());
					if( pStr && pszPath)
					{
						if( pStr->SetStringW(pszPath) && pUserData->SetValue(RemediationDescription, pStr) )
							CCTRCTXI1(_T("Scan infection remediation descriptive text set to %ls in user data."), pStr->GetStringW());
					}
				}
			}
		}
	} // END: RebootRequired

    cc::IStringPtr pStrDesc;

    if ( ccEraser::Succeeded(ezRem->GetDescription(pStrDesc)) )
        CCTRCTXI2(_T("Remediation action succeeded for %ls. eResult = %d"), pStrDesc->GetStringW(), eRemResult);
    else
        CCTRCTXI0(_T("Remediation action succeeded, but failed to get action description."));

// #if 0
// 	CCTRCTXE0(_T("Forcing failure - rollback unit testing!!!!"));
// 	eRemResult = ccEraser::Fail;
// 	return false;
// #endif

    return true;
}

ccEraser::eResult CAVScanObject::PostDetection( ccEraser::IDetectionAction* pDetectAction, ccEraser::eResult eDetectionResult, const ccEraser::IContext* pContext )
{
    // If logging is enabled log out successful detections
    if( g_DebugOutput.IsOutputEnabled(ccLib::CDebugOutput::eInformationDebug) )
    {
        cc::IKeyValueCollectionPtr pDetectProps;
        if( ccEraser::Failed(pDetectAction->GetProperties(pDetectProps)) || pDetectProps == NULL )
        {
            CCTRCTXE0(_T("Failed to get detection action properties"));
            return ccEraser::Continue;
        }

        // Get the state
        DWORD dwState = 0;
        if( !pDetectProps->GetValue(ccEraser::IDetectionAction::State, dwState) )
        {
            CCTRCTXE0(_T("Failed to get detection action state"));
            return ccEraser::Continue;
        }

        // See if this action was detected
        if( dwState == ccEraser::IDetectionAction::Detected )
        {
            cc::IStringPtr pStrDesc;
            if( ccEraser::Succeeded(pDetectAction->GetDescription(pStrDesc)) && pStrDesc != NULL )
            {
                CCTRCTXI1(_T("DETECTED: %ls"), pStrDesc->GetStringW());
            }
            else
            {
                CCTRCTXE0(_T("Failed to get generic detection action description for detected action"));
            }
        }
    }

    return ccEraser::Success;
}

SYMRESULT CAVScanObject::Scan(HWND hParentWnd)
{
	SYMRESULT symResult = SYM_OK;

	if(QueryDoScan(hParentWnd, symResult) == FALSE)
	{
		CCTRCTXW1(_T("QueryDoScan returned false: 0x%08X"), symResult);
		return symResult;
	}

	ResetMembersForNewScan();	// To make scan re-entrant (on the same thread, only)

	// setup our status flags:
	m_Status = RUNNING;
	m_nRebootRequired = 0;

    CString sLogDetails;
	VERIFY( sLogDetails.LoadString(IDS_LOG_DETAILS) );

	// Scan sink and eraser loader object smart pointers should be outside of the following block:
	CEraserScanSinkPtr pSink;
	cc::CSymInterfaceTrustedLoader ccEraserLoader;

	// IMPORTANT: Scoping the eraser object smart pointer inside a block to ensure that
	// it goes out of scope before its corresponding loader object does.
	ccLib::CExceptionInfo exInfo;
	try
	{
		ccEraser::IEraser4Ptr pEraser;
		IScannerwPtr pScanner;

		symResult = InitDependentObjects( pSink, pScanner, ccEraserLoader, pEraser );
		if ( SYM_FAILED(symResult) )
			return symResult;

		// proceed with regular scanning.....
		CString sLogLine;
		VERIFY( sLogLine.LoadString(IDS_SCANNING) );
		FireStatus(PROGRESS_TEXT, sLogLine);

		CCTRCTXI0(_T("Starting Eraser Scan."));

		ccEraser::IContextPtr pContext;
		ccEraser::eResult eResult = pEraser->Scan(pSink, pContext);
        
        CCTRCTXI1(_T("Finished Eraser Scan. Returned 0x%x"), eResult);

		// user aborted scan
		if (eResult == ccEraser::eResult::Abort)
		{
            CCTRCTXW0(_T("User Aborted Scan."));
			m_Status = FINISHED_SKIPPED;
			return SYM_OK;
		}

		// scan failed
		if (ccEraser::Failed(eResult) || pContext == NULL)
		{
			CCTRCTXE1(_T("Scan Failed. Returned 0x%x"), eResult);
			VERIFY( sLogLine.LoadString(IDS_ERR_SCANFAILED) );
			FireStatus(LOG_TEXT, sLogLine, eResult);
			m_Status = FINISHED_FAILED;
			return SYMERR_UNKNOWN;
		}

        // log a few things
        VERIFY( sLogLine.LoadString(IDS_LOG_SUMMARY) );
	    FireStatus(LOG_TEXT, sLogLine);
	    VERIFY( sLogLine.LoadString(IDS_LOG_NUMVIRS) );
	    FireStatus(LOG_TEXT, sLogLine, m_nAnomalyCount);
		VERIFY(sLogLine.LoadString(IDS_LOG_NUMRISKS_FIXED));
		FireStatus(LOG_TEXT, sLogLine, m_nAnomalyRepairedCount);
	    VERIFY( sLogLine.LoadString(IDS_LOG_NUMREP) );
	    FireStatus(LOG_TEXT, sLogLine, m_nRepairedCount);
	    VERIFY( sLogLine.LoadString(IDS_LOG_NUMQUAR) );
	    FireStatus(LOG_TEXT, sLogLine, m_nQuarantinedCount);
		if(m_nRebootRequired > 0)
		{
			VERIFY(sLogLine.LoadString(IDS_LOG_NUMRISKS_REBOOT));
			FireStatus(LOG_TEXT, sLogLine, m_nRebootRequired);
		}
	    FireStatus(LOG_TEXT, _T("\r\n\r\n"));
		
		VERIFY( sLogLine.LoadString(IDS_SCAN_COMPLETED) );
		FireStatus(PROGRESS_TEXT, sLogLine);

        FireStatus(LOG_TEXT, m_sLogDetails);

		if(m_nRebootRequired > 0 && m_sLogRebootRisks.GetLength() > 0)
		{
			FireStatus(LOG_TEXT, _T("\r\n"));
			sLogLine.LoadString(IDS_REBOOT_SUMMARY);
			FireStatus(LOG_TEXT, sLogLine);
			FireStatus(LOG_TEXT, m_sLogRebootRisks);
		}
		if(m_sLogNotImplemented.GetLength() > 0 && sLogLine.LoadString(IDS_REPAIR_NOT_IMPLEMENTED))
		{
			FireStatus(LOG_TEXT, _T("\r\n"));
			FireStatus(LOG_TEXT, sLogLine);
			FireStatus(LOG_TEXT, m_sLogNotImplemented);

			CCTRCTXW2(_T("%s %s"), sLogLine, m_sLogNotImplemented);
		}
	}
	CCCATCHMEM(exInfo);
	if(exInfo.IsException())
	{
		m_Status = FINISHED_FAILED;
		return SYMERR_OUTOFMEMORY;
	}

	//TODO:SEND COMPLETED EVENT
	m_Status = FINISHED_SUCCESS;

	return ((m_nRebootRequired > 0) ? SYM_REBOOT : SYM_OK);
}

//****************************************************************************
//	Query if scan should be done
//****************************************************************************
BOOL CAVScanObject::QueryDoScan(HWND hParentWnd, SYMRESULT &symResult)
{
	BOOL bReturn = FALSE;
	ccLib::CExceptionInfo exInfo;
	try
	{
		//check for wierdness, re-entrancy, etc
		if(m_Status == RUNNING || m_Status == STOPPING)
		{
			symResult = SYMERR_UNKNOWN;
		}

		//If we're in silent mode, we're not going to do anything
		//If the path is UNC, we don't care because we're not going to run
		//Per OEM requirements - silent install must never scan.

		else if(hParentWnd == NULL)
		{
			CCTRCTXW0(_T("hWnd == NULL, silent install, skipping"));
			symResult = SYM_OK;
			bReturn = FALSE;
			m_Status = FINISHED_SKIPPED;
		}

		//Install is not silent, proceed as normal.
		else
		{
			m_Status = STOPPED;
			FireStatus(PROGRESS_TEXT, _T(""));

			ATL::CString sBasePath;
			if(GetLayoutBasePath(sBasePath) == false)
			{
				symResult = SYMERR_OUTOFMEMORY;
				return false;
			}
			
			bool bPathAllowed = IsPathAllowed(sBasePath);
			if(bPathAllowed == false)
			{
				CCTRCTXW1(_T("Path %s not allowed"), sBasePath);
				
				//at this point, we can exit the install, or just give up on the scan.
				//Eiher way, we're not going to scan.

				m_Status = FINISHED_SKIPPED;
				symResult = SYM_OK;
				
				ATL::CString sLogText;
				if(sLogText.LoadString(IDS_UNC_SCAN_NOT_SUPPORTED))
				{
					FireStatus(LOG_TEXT, sLogText);
				}
				else
					CCTRCTXE0(_T("LoadString == false"));
				
				CUNCErrorDialog errDlg;
				if(errDlg.DoModal(hParentWnd) == IDCANCEL)
				{
					//User wants to exit the install, and move the layout locally
					CCTRCTXE0(_T("User chose to abort install"));
					symResult = SYM_ABORT;
				}
				else
				{
					//user doesn't care about the pre-install scan.
					//Install will proceed from a UNC path, this scan will not run.
					CCTRCTXW0(_T("User chose to skip prescan and continue install"));
				}

				//Skip scan due to bad path
				bReturn = FALSE;
			}
			else
			{
				INT_PTR res = IDOK;
				if(ShouldPrompt() != false)
				{
					CCTRCTXI0(_T("Prompting user for permission to scan"));
					COpenAppsDialog dlg;
					res = dlg.DoModal(hParentWnd);
				}

				if(res == IDOK)
				{
					CCTRCTXI0(_T("Running Pre-Install scan"));
					m_Status = RUNNING;
					bReturn = TRUE;
				}
				else
				{
					CCTRCTXW0(_T("User chose to skip Pre-Install scan"));
					m_Status = FINISHED_SKIPPED;
					bReturn = FALSE;
				}
			}
		}
	}
	CCCATCHMEM(exInfo);
	if(exInfo.IsException())
	{
		m_Status = FINISHED_FAILED;
		symResult = SYMERR_OUTOFMEMORY;
		bReturn = false;
	}
	return bReturn;
}

//****************************************************************************
//	Initialize the sink object
//****************************************************************************
BOOL CAVScanObject::InitSink( CEraserScanSinkPtr &pSink, SYMRESULT &symResult )
{
	ASSERT( pSink.m_ptr.m_p == NULL );				// If this fires, the pointer is already initialized

	ccLib::CExceptionInfo exInfo;
	try
	{
		CString sLogLine;
		VERIFY( sLogLine.LoadString(IDS_LOADING_SCANNER) );
		FireStatus(PROGRESS_TEXT, sLogLine);

		// Create our callback sink, registering ourselves as the client:
		symResult = CEraserScanSink::Create( pSink, this );
		if ( SYM_FAILED(symResult) )
		{
			CCTRCTXE1( _T("Failed to create EraserScanSink. Returned 0x%x"), symResult );

		
			VERIFY( sLogLine.LoadString(IDS_ERR_SCNRCREATEFAILED) );
			FireStatus(LOG_TEXT, sLogLine, 55);  //TODO:: Make this error code a real NAV error code

			m_Status = FINISHED_FAILED;
			return FALSE;
		}
	}
	CCCATCHMEM(exInfo);
	if(exInfo.IsException())
	{
		m_Status = FINISHED_FAILED;
		return FALSE;
	}

	ASSERT( pSink != NULL );

	// make sure we haven't been aborted
	AVS_RETURN_FALSE_ON_ABORT();

	return TRUE;
}

//****************************************************************************
//	Initialize the scanner object
//****************************************************************************
BOOL CAVScanObject::InitScanner( LPCTSTR							szDefsDir,
								 LPCTSTR							szBasePath,
								 IScannerwPtr&						pScanner,
								 SYMRESULT&							symResult ) throw()
{
	ASSERT( szDefsDir != NULL && szBasePath != NULL );
	ASSERT( pScanner.m_p == NULL );				// If this fires, the pointer is already initialized

	// See if the loader is already initialized by checking its object count.
	// If there are outstanding objects, don't re-initialize, since that
	// would unload and re-load the DLL (a bad thing to do while its
	// serving-up objects).
	if ( m_ccScannerLoader.GetObjectCount() <= 0 )
	{
		// find the path to ccScan and initialize our loader with it
		TCHAR szCCScanPath[MAX_PATH * 2] = {0};
		_tcscpy(szCCScanPath, szBasePath);
		PathAppend(szCCScanPath, _T("Support\\ccCommon\\ccCommon\\ccScanw.dll"));

		CCTRCTXI1(_T("ccScanPath = %s"), szCCScanPath);
		symResult = m_ccScannerLoader.Initialize( szCCScanPath );
	    if (SYM_FAILED(symResult))
		{
			CCTRCTXE2(_T("Failed to initialize the ccScanLoader. Returned 0x%x - last error = %d"), symResult, GetLastError());

			ccLib::CExceptionInfo exInfo;
			try
			{
				CString sLogLine;
				VERIFY( sLogLine.LoadString(IDS_ERR_SCNRCREATEFAILED) );
				FireStatus(LOG_TEXT, sLogLine, symResult);  
			}
			CCCATCHMEM(exInfo);

			m_Status = FINISHED_FAILED;
		    symResult = SYMERR_UNKNOWN;
			return FALSE;
		}

		// make sure we haven't been aborted
		AVS_RETURN_FALSE_ON_ABORT();

		CCTRCTXI1(_T("Using ccScan.dll from: %s"), szCCScanPath);
	}

	// get a scanner object
	symResult = m_ccScannerLoader.CreateObject(IID_Scannerw, IID_Scannerw, reinterpret_cast<void**>(&pScanner));
	if(SYM_FAILED(symResult))
	{
		CCTRCTXE1(_T("Failed to load IScannerw object. Returned 0x%x."), symResult);

		ccLib::CExceptionInfo exInfo;
		try
		{
			CString sLogLine;
			VERIFY( sLogLine.LoadString(IDS_ERR_SCNRCREATEFAILED) );
			FireStatus(LOG_TEXT, sLogLine, symResult); 
		}
		CCCATCHMEM(exInfo);

		m_Status = FINISHED_FAILED;
		symResult = SYMERR_UNKNOWN;
		return FALSE;
	}
    
	// make sure we haven't been aborted
	AVS_RETURN_FALSE_ON_ABORT();

	// initialize our scanner with the virus defs path
	TCHAR szTempPath[MAX_PATH];
	GetTempPath(MAX_PATH, szTempPath);

	SCANWSTATUS Status = pScanner->Initialize(szDefsDir, // def path, see flags below
											  szTempPath, // temp path
											  2, // bloodhound level
											  IScanner::ISCANNER_USE_PERSISTABLE_VIDS | IScanner::ISCANNER_USE_DEF_PATH);
	if (Status != SCANW_OK)
	{   
        CCTRCTXE1(_T("Failed to intialize IScannerw object. Returned 0x%x."), Status);

		ccLib::CExceptionInfo exInfo;
		try
		{
			CString sLogLine;
			VERIFY( sLogLine.LoadString(IDS_ERR_SCNRINITFAILED) );
			FireStatus(LOG_TEXT, sLogLine, Status); 
		}
		CCCATCHMEM(exInfo);

		m_Status = FINISHED_FAILED;
		symResult = SYMERR_UNKNOWN;
		return FALSE;
	}

	// make sure we haven't been aborted
	AVS_RETURN_FALSE_ON_ABORT();
	return TRUE;
}

//****************************************************************************
//	Initialize the Eraser object
//****************************************************************************
BOOL CAVScanObject::InitEraser( CEraserScanSink*				pSink,
							    IScannerw*						pScanner,
							    LPCTSTR							szDefsDir,
							    cc::CSymInterfaceTrustedLoader&	loader,
							    ccEraser::IEraser4Ptr&			pEraser,
							    SYMRESULT&						symResult ) throw()
{
	ASSERT( pSink != NULL && pScanner != NULL && szDefsDir != NULL );
	ASSERT( pEraser.m_p == NULL );				// If this fires, the pointer is already initialized

	// find the path to ccEraser.dll and initialize our
	// loader with it.
	TCHAR szEraserPath[MAX_PATH * 2] = { 0 };
	_tcscpy(szEraserPath, szDefsDir);
	PathAppend(szEraserPath, _T("ccEraser.dll"));

	symResult = loader.Initialize(szEraserPath);
	if (SYM_FAILED(symResult))
	{
        CCTRCTXE1(_T("Failed to initialize ccEraser Loader. Returned 0x%x"), symResult);

		ccLib::CExceptionInfo exInfo;
		try
		{
			CString sLogLine;
			VERIFY( sLogLine.LoadString(IDS_ERR_GSECREATEFAILED) );
			FireStatus(LOG_TEXT, sLogLine, symResult); 
		}
		CCCATCHMEM(exInfo);

		m_Status = FINISHED_FAILED;
		symResult = SYMERR_UNKNOWN;
		return FALSE;
	}    
    
    CCTRCTXI1(_T("Using ccEraser.dll from: %s"), szEraserPath);

	// make sure we haven't been aborted
	AVS_RETURN_FALSE_ON_ABORT();

	// create the IEraser object
	symResult = loader.CreateObject(ccEraser::IID_Eraser, ccEraser::IID_Eraser4, reinterpret_cast<void**>(&pEraser));
	if (SYM_FAILED(symResult))
	{
        CCTRCTXE1(_T("Failed to create IEraser object. Returned 0x%x"), symResult);

		ccLib::CExceptionInfo exInfo;
		try
		{
			CString sLogLine;
			VERIFY( sLogLine.LoadString(IDS_ERR_GSECREATEFAILED) );
			FireStatus(LOG_TEXT, sLogLine, symResult); 

		}
		CCCATCHMEM(exInfo);

		m_Status = FINISHED_FAILED;
		symResult = SYMERR_UNKNOWN;
		return FALSE;
	}

	// make sure we haven't been aborted
	AVS_RETURN_FALSE_ON_ABORT();

	ccEraser::eResult eResult;
	eResult = pEraser->Initialize(pScanner, pSink, pSink, ccEraser::CurrentVersion);
	if (ccEraser::Failed(eResult))
	{
		CCTRCTXE1(_T("Failed to Initialize IEraser object. Returned 0x%x"), eResult);

		ccLib::CExceptionInfo exInfo;
		try
		{
			CString sLogLine;
			VERIFY( sLogLine.LoadString(IDS_ERR_GSEINITFAILED) );
			FireStatus(LOG_TEXT, sLogLine, eResult); 
		}
		CCCATCHMEM(exInfo);

		m_Status = FINISHED_FAILED;
		symResult = SYMERR_UNKNOWN;
		return FALSE;
	}
	
	// make sure we haven't been aborted
	AVS_RETURN_FALSE_ON_ABORT();
	return TRUE;
}

//****************************************************************************
//	Initialize the quarantine object
//****************************************************************************
BOOL CAVScanObject::InitQuarantine( IScannerw* pScanner, LPCTSTR szBasePath, SYMRESULT &symResult ) throw()
{
	// If QBackup pointer is not NULL, then we're already initialized:
	if ( m_spQBackup.m_p != NULL )
		return TRUE;

	ccLib::CExceptionInfo exInfo;
	try
	{
		// See if the loader is already initialized by checking its object count.
		// If there are outstanding objects, don't re-initialize, since that
		// would unload and re-load the DLL (a bad thing to do while its
		// serving-up objects).
		if ( m_QBackupLoader.GetObjectCount() <= 0 )
		{
			// find the path to QBackup.dll and initialize our loader with it
			CString sQBackupPath = szBasePath;
			PathAppend(sQBackupPath.GetBuffer(sQBackupPath.GetLength() + 1024), _T("Support\\AV\\AV\\QBackup.dll"));
			sQBackupPath.ReleaseBuffer();

			symResult = m_QBackupLoader.Initialize(sQBackupPath);
			if(SYM_FAILED(symResult))
			{
				CCTRCTXE1(_T("Failed to initialize QBackup Loader. Returned 0x%x."), symResult);

				CString sLogLine;
				VERIFY( sLogLine.LoadString(IDS_ERR_QUARCREATEFAILED) );
				FireStatus(LOG_TEXT, sLogLine, symResult); 

				m_Status = FINISHED_FAILED;
				symResult = SYMERR_UNKNOWN;
				return FALSE;
			}

			// make sure we haven't been aborted
			AVS_RETURN_FALSE_ON_ABORT();

			CCTRCTXI1(_T("Using QBackup.dll from: %s"), sQBackupPath.GetBuffer());
		}

		// create the QBackup object
		symResult = m_QBackupLoader.CreateObject(QBackup::SYMOBJECT_QBACKUP, QBackup::IID_QBackup, reinterpret_cast<void**>(&m_spQBackup));
		if(SYM_FAILED(symResult))
		{
			CCTRCTXE1(_T("Failed to create IQBackup object. Returned 0x%x."), symResult);

			CString sLogLine;
			VERIFY( sLogLine.LoadString(IDS_ERR_QUARCREATEFAILED) );
			FireStatus(LOG_TEXT, sLogLine, symResult); 

			m_Status = FINISHED_FAILED;
			symResult = SYMERR_UNKNOWN;
			return FALSE;
		}

		// make sure we haven't been aborted
		AVS_RETURN_FALSE_ON_ABORT();

		// first try to inialize the QBackup without tweaks.  this will
		// only succeed when quarantine is has been left behind on the machine.
		HRESULT hr = m_spQBackup->Initialize();
		if(HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND) == hr)
		{
			CCTRCTXI0(_T("Failed attempt to initialize QBackup. Path unavailable."));
	            
			// we can't initialize the quarantine server because the directories aren't set up
			// this is expected in almost all cases.  we need to create a temp quarantine dir
			// and point the reg key to it
		    
			// NOTE: although it seems ugly, there is a good reason for creating the regkey path
			// rather than enhancing the quarantine interface to support having the path passed in.
			// if the user exits the installer after quarantining some files, the installer will be 
			// able to find the quarantine directory later so it can move the files to the real
			// quarantine directory.

			// Get/Create Quarantine dir (in TEMP directory):
			CString strQuarantine;
			bool bGetPSQuarPath = GetPSQuarPath( strQuarantine, /* bCreateFolder = */ true );
			if ( !bGetPSQuarPath )
			{
				CString sLogLine;
				VERIFY( sLogLine.LoadString(IDS_ERR_SCNRINITFAILED) );
				FireStatus( LOG_TEXT, sLogLine, (HRESULT)bGetPSQuarPath );

				m_Status = FINISHED_FAILED;
				symResult = SYMERR_UNKNOWN;
				return FALSE;
			}

			CCTRCTXI1(_T("Created Quarantine path at: %s"), strQuarantine);

			// now try to initialize again by giving it the new path...
			hr = m_spQBackup->Initialize(strQuarantine);
		}

		// make sure we haven't been aborted
		AVS_RETURN_FALSE_ON_ABORT();

		// verify that our quarantine server is initialized
		if(FAILED(hr))
		{
			// we've done everything we can at this point, if we still
			// can't initialize QBackup, we're stuck. error out.
			CCTRCTXE1(_T("Fatal Failure intializing QBackup. Returned 0x%x"), hr);

			CString sLogLine;
			VERIFY( sLogLine.LoadString(IDS_ERR_QUARINITFAILED) );
			FireStatus(LOG_TEXT, sLogLine, hr);

			m_Status = FINISHED_FAILED;
			symResult = SYMERR_UNKNOWN;
			return FALSE;
		}
	}
	CCCATCHMEM(exInfo);
	if(exInfo.IsException())
	{
		m_Status = FINISHED_FAILED;
		symResult = SYMERR_OUTOFMEMORY;
		return FALSE;
	}
	// make sure we haven't been aborted
	AVS_RETURN_FALSE_ON_ABORT();
	return TRUE;
}

BOOL CAVScanObject::InitEraserFilters( ccEraser::IEraser4* pEraser, SYMRESULT &symResult ) throw()
{
	ASSERT( pEraser != NULL );

	ccLib::CExceptionInfo exInfo;
	try
	{
		ccEraser::IFilterPtr pFilterIn = NULL;
		ccEraser::IFilterPtr pFilterOut = NULL;
		cc::IKeyValueCollectionPtr pSettings = NULL;

		ccEraser::eResult eResult = pEraser->GetSettings(pSettings);
		if (ccEraser::Failed(eResult))
		{
			CCTRCTXE1(_T("Failed to get Eraser Settings: %d"), eResult);
			
			CString sLogLine;
			VERIFY( sLogLine.LoadString(IDS_ERR_QUARINITFAILED) );
			FireStatus(LOG_TEXT, sLogLine, 56);  //TODO:: Get a real error code or a different message

			m_Status = FINISHED_FAILED;
			symResult = SYMERR_UNKNOWN;
			return FALSE;
		}

		// make sure we haven't been aborted
		AVS_RETURN_FALSE_ON_ABORT();

		// get the input filter
		eResult = pEraser->CreateObject(ccEraser::FilterType, ccEraser::IID_Filter, reinterpret_cast<void**>(&pFilterIn));
		if (ccEraser::Failed(eResult))
		{
			CCTRCTXE1(_T("Failed to get input Filter: %d"), eResult);

			CString sLogLine;
			VERIFY( sLogLine.LoadString(IDS_ERR_QUARINITFAILED) );
			FireStatus(LOG_TEXT, sLogLine, 57); //TODO:: Get a real error code or a different message

			m_Status = FINISHED_FAILED;
			symResult = SYMERR_UNKNOWN;
			return FALSE;
		}

		// make sure we haven't been aborted
		AVS_RETURN_FALSE_ON_ABORT();

		// get the output filter
		eResult = pEraser->CreateObject(ccEraser::FilterType, ccEraser::IID_Filter, reinterpret_cast<void**>(&pFilterOut));
		if (ccEraser::Failed(eResult))
		{
			CCTRCTXE1(_T("Failed to get output Filter: %d"), eResult);

			CString sLogLine;
			VERIFY( sLogLine.LoadString(IDS_ERR_QUARINITFAILED) );
			FireStatus(LOG_TEXT, sLogLine, 58); //TODO:: Get a real error code or a different message

			m_Status = FINISHED_FAILED;
			symResult = SYMERR_UNKNOWN;
			return FALSE;
		}

		// make sure we haven't been aborted
		AVS_RETURN_FALSE_ON_ABORT();

		pFilterOut->ClearAnomalyFilter();
		pFilterOut->ClearDetectionFilter();

		pFilterOut->AddAnomalyCategory(ccEraser::IAnomaly::Viral);
		pFilterOut->AddAnomalyCategory(ccEraser::IAnomaly::Malicious);
		pFilterOut->AddAnomalyCategory(ccEraser::IAnomaly::ReservedMalicious);
		pFilterOut->AddAnomalyCategory(ccEraser::IAnomaly::Heuristic);
		pFilterOut->AddAnomalyCategory(ccEraser::IAnomaly::SpyWare);
		pFilterOut->AddAnomalyCategory(ccEraser::IAnomaly::Adware);

		pFilterIn->ClearAnomalyFilter();
		pFilterIn->ClearDetectionFilter();

		pFilterIn->AddAnomalyCategory(ccEraser::IAnomaly::GenericLoadPoint);

		// Set the filter in the settings object as the input filter
		if( !pSettings->SetValue(ccEraser::IEraser::InputFilter, pFilterIn) )
		{
			CCTRCTXE0(_T("Failed to set ccEraser::IEraser::InputFilter"));

			CString sLogLine;
			VERIFY( sLogLine.LoadString(IDS_ERR_QUARINITFAILED) );
			FireStatus(LOG_TEXT, sLogLine, 58); //TODO:: Get a real error code or a different message

			m_Status = FINISHED_FAILED;
			symResult = SYMERR_UNKNOWN;
			return FALSE;
		}

		// Set the filter in the settings object as the output filter
		if( !pSettings->SetValue(ccEraser::IEraser::OutputFilter, pFilterOut) )
		{
			CCTRCTXE0(_T("Failed to set ccEraser::IEraser::OutputFilter"));

			CString sLogLine;
			VERIFY( sLogLine.LoadString(IDS_ERR_QUARINITFAILED) );
			FireStatus(LOG_TEXT, sLogLine, 59); //TODO:: Get a real error code or a different message

			m_Status = FINISHED_FAILED;
			symResult = SYMERR_UNKNOWN;
			return FALSE;
		}
	}
	CCCATCHMEM(exInfo);
	if(exInfo.IsException())
	{
		m_Status = FINISHED_FAILED;
		symResult = SYMERR_OUTOFMEMORY;
		return FALSE;
	}

	// make sure we haven't been aborted
	AVS_RETURN_FALSE_ON_ABORT();

	return TRUE;
}

bool CAVScanObject::IsScanInfectionFileGone(IScanwInfection* pInfection)
{
	ccLib::CExceptionInfo exInfo;
	try
	{
		if( NULL == pInfection )
		{
			CCTRCTXE0(_T("Scan infection object is null."));
			return false;
		}

		// Check to see if the file is actually gone already...
		ATL::CAtlString strFileName;
		const wchar_t* pszFileName = NULL;
		IScanwFileInfectionQIPtr pScanFile;
		IScanwCompressedFileQIPtr pCompressed;
		SCANWSTATUS status = ccScanw::SCANW_ERROR_UNKNOWN;
		if( pScanFile = pInfection )
		{
			// Make sure the current file APIs match the file mode for the path we are going to check
			status = pScanFile->GetFileName(pszFileName);
			if(status == SCANW_OK && pszFileName != NULL)
				strFileName = pszFileName;
			else
				CCTRCTXE1(_T("Failed to get file name: %d"), status);
		}
		else if( pCompressed = pInfection )
		{
			int nComponentCount = 0;
			status = pCompressed->GetComponentCount(nComponentCount);
			if(SCANW_OK == status && nComponentCount > 0)
			{
				status = pCompressed->GetComponent(nComponentCount-1, pszFileName);
				if(status == SCANW_OK && pszFileName != NULL)
					strFileName = pszFileName;
				else
					CCTRCTXE1(_T("Failed to get file name: %d"), status);
			}
			else
				CCTRCTXE2(_T("Failed to get component count (count=%d): %d"), nComponentCount, status);
		}
		else
		{
			unsigned long ulVirusID = 0;
			pInfection->GetVirusIDW(ulVirusID);
			const wchar_t* pszVirusName = L"";
			SCANWSTATUS status = pInfection->GetVirusNameW(pszVirusName);
			if(status != SCANW_OK)
			{
				CCTRCTXE1(_T("Failed to get vname: %d"), status);
			}
			CCTRCTXE2(_T("Scan infection item VID = %lu, Name = %ls is not a file or compressed file infection."),ulVirusID, pszVirusName);
			return false;
		}

		// Does this file exist?
		DWORD dwAttribs = INVALID_FILE_ATTRIBUTES;

		dwAttribs = GetFileAttributes(strFileName);

		if ( INVALID_FILE_ATTRIBUTES == dwAttribs &&
			ERROR_FILE_NOT_FOUND == GetLastError() )
		{
			CCTRCTXW1(_T("The path %s for this scan infection no longer exists."), strFileName);
			return true;
		}
	/*    else if ( pCompressed && m_Quarantine.AddFailedOnExtraction() )
		{
			CCTRCTXW1(_T("The infected compressed file within the %s scan infection container no longer exists."), strFileName);
			return true;
		}
	*/
	}
	CCCATCHMEM(exInfo);
    return false;
}

bool CAVScanObject::GetUndoData(ccEraser::IRemediationAction* pAction, cc::IStream*& pStream, CString& strUndoFileName)
{
    if( pAction == NULL )
    {
        CCTRCTXE0(_T("The remediation action is not valid."));
        return false;
    }

    ccEraser::eResult eRes = ccEraser::Success;
    bool bUndoable = false;

    eRes = pAction->SupportsUndo(bUndoable);
    if( ccEraser::Failed(eRes) )
    {
        CCTRCTXE1(_T("SupportsUndo() failed. eResult = %d"), eRes);
        return false;
    }
    else if( !bUndoable ) 
    {
        CCTRCTXI0(_T("The remediation action does not support undo."));
        return false;
    }
    else
    {
		CCTRCTXI0( _T("Current Remediation is undoable. Saving undo information.") );
        ccSym::CFileStreamImplPtr pFileStream;
        pFileStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());

        if (pFileStream == NULL)
        {
            CCTRCTXE0(_T("File stream is NULL."));
            return false;
        }

        TCHAR szTempDir[MAX_PATH] = {0};
        if (GetTempPath(MAX_PATH, szTempDir) == 0)
			CCTRCTXE1(_T("Failed to get the temp path: %d"), GetLastError());

		ccLib::CExceptionInfo exInfo;
		try
		{
			GetTempFileName(szTempDir, _T("und"), 0, strUndoFileName.GetBuffer(MAX_PATH));
			strUndoFileName.ReleaseBuffer();
		}
		CCCATCHMEM(exInfo);
		if(exInfo.IsException())
			return false;

        if (FALSE == pFileStream->GetFile().Open(strUndoFileName, 
            GENERIC_READ | GENERIC_WRITE,
            0, 
            NULL, 
            CREATE_ALWAYS, 
            0,
            NULL))
        {
            CCTRCTXE1(_T("pFileStream->GetFile().Open() failed for file %s"), (LPCTSTR)strUndoFileName);
            DeleteFile(strUndoFileName);
            strUndoFileName.Empty();
            return false;
        }

        // Get the undo information
        eRes = pAction->GetUndoInformation(pFileStream);
        if( ccEraser::Failed(eRes) )
        {
            CCTRCTXE1(_T("Error getting undo information. eResult = %d"), eRes);
            pFileStream->GetFile().Close();
            
            if( !pFileStream->GetFile().Delete(strUndoFileName, FALSE) )
            {
                CCTRCTXE1(_T("Failed to delete backup undo file stream %s"), (LPCTSTR)strUndoFileName);
            }

            strUndoFileName.Empty();
            return false;
        }
		ULONGLONG ullPos = 0;
		if(pFileStream->GetPosition(ullPos) && ullPos != 0)
		{
			CCTRCTXW1(_T("Resetting undo data from %I64d to 0"), ullPos);
			if(pFileStream->SetPosition(0) == false)
				CCTRCTXE0(_T("pFileStream->SetPosition(0) == false"));
		}
        pStream = pFileStream.Detach();
    }

    return true;
}

void CAVScanObject::ReleaseUndoData(cc::IStream* pUndoStream, LPCTSTR pcszUndoFileName)
{
    // Release the undo stream if we have one
    if( pUndoStream != NULL )
    {
        pUndoStream->Release();
        pUndoStream = NULL;
    }

    // Delete the undo file if we have one
    if ( pcszUndoFileName != NULL && *pcszUndoFileName != NULL_CHAR )
    {
        if( DeleteFile(pcszUndoFileName) )
            CCTRCTXI1(_T("Successfully deleted backup undo file %s"),pcszUndoFileName);
        else
            CCTRCTXE1(_T("Failed to delete backup undo file %s"),pcszUndoFileName);
    }
}

// static:
void CAVScanObject::EnsurePathExists(LPCTSTR szFullPath)
{
	ccLib::CExceptionInfo exInfo;
	try
	{
		CString cszDirectoryName = szFullPath;
		INT iCurPos = NULL, iDirNameSize = cszDirectoryName.GetLength() + 1;
		CString cszTemp, cszNewPath = cszDirectoryName.Tokenize(_T("\\/"), iCurPos);
		LPTSTR szNewPath = cszNewPath.GetBuffer(iDirNameSize);
		cszTemp = cszDirectoryName.Tokenize(_T("\\"), iCurPos);
		while(!cszTemp.IsEmpty())
		{
			PathAppend(szNewPath, cszTemp);
			if(-1 == GetFileAttributes(szNewPath))
			{
				CreateDirectory(szNewPath, NULL);
				if(-1 == GetFileAttributes(szNewPath))
					break;
			}

			cszTemp = cszDirectoryName.Tokenize(_T("\\/"), iCurPos);
		}

		szNewPath = NULL;
		cszNewPath.ReleaseBuffer();
	}
	CCCATCHMEM(exInfo);

    return;
}

// static:
//FIXME: this may throw on return....
CString CAVScanObject::GetTempFolder()
{
	// Get required buffer length for temp path:
	const DWORD kdwLen = ::GetTempPath( 0, NULL );
	CString strFolder;
	ccLib::CExceptionInfo exInfo;
	try
	{
		LPTSTR pszBuf = strFolder.GetBuffer( kdwLen );
		DWORD dwLen = ::GetTempPath( kdwLen, pszBuf );

		ASSERT( kdwLen == dwLen + 1 );
		UNUSED_ALWAYS( dwLen );								// Quiet "unreferenced variable" warning in release builds.

		strFolder.ReleaseBuffer();
	}
	CCCATCHMEM(exInfo);
	return strFolder;
}

// static:
bool CAVScanObject::DeleteDirectoryContents( LPCTSTR szDirectory, BOOL bRecursive )
{
    bool                bRet = false;
    HANDLE              hFind = INVALID_HANDLE_VALUE;
    CString             szFile;
    WIN32_FIND_DATA     fd;

    CCTRCTXW1( _T("BEGIN DeleteDirectoryContents( %s )"), szDirectory );

	ccLib::CExceptionInfo exInfo;
	try
	{
		SetFileAttributes( szDirectory, FILE_ATTRIBUTE_NORMAL );

	    
		szFile = szDirectory;
		szFile += _T("\\*.*" );

		hFind = FindFirstFile( szFile, &fd );

		if ( hFind != INVALID_HANDLE_VALUE )
		{
			do
			{
           		szFile = szDirectory;

				if ( fd.cFileName[0] != '.' &&
					fd.cAlternateFileName[0] != '.' )
				{				
						szFile.Append( fd.cFileName[0] ? fd.cFileName :
										fd.cAlternateFileName );
		
						if (bRecursive && fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
							DeleteDirectoryContents( szFile, TRUE);
						else
						{					
							SetFileAttributes( szFile, FILE_ATTRIBUTE_NORMAL );
							DeleteFile( szFile );
						}
				}
			}
			while( FindNextFile( hFind, &fd ) );

			FindClose( hFind );
			hFind = INVALID_HANDLE_VALUE;
		}

		RemoveDirectory( szDirectory );

		bRet = true;

		CCTRCTXW1( _T("END DeleteDirectoryContents( %d )"), bRet );
	}
	CCCATCHMEM(exInfo)
	if(exInfo.IsException())
	{
		if(hFind != INVALID_HANDLE_VALUE)
			FindClose(hFind);
		bRet = false;
	}
    return( bRet );
}

// static:
bool CAVScanObject::GetPSQuarPath( CString& strPath, bool bCreateFolder /* = false */ )
{
	ccLib::CExceptionInfo exInfo;
	try
	{
		CString strPSQuarPath;

		if ( strPSQuarPath.IsEmpty() )
		{
			// Our temporary Quarantine directory will be inside the Temp directory
			strPSQuarPath = GetTempFolder();
			strPSQuarPath += _T("PSQuar");
		}

		if ( bCreateFolder )
		{
			BOOL bCreateOk = ::CreateDirectory( strPSQuarPath, NULL );
			if ( !bCreateOk )
			{
				DWORD dwLastError = ::GetLastError();
				if ( dwLastError != ERROR_ALREADY_EXISTS )
				{
					CCTRCTXE1( _T(" Failed to create a Quarantine folder: %s"),strPSQuarPath );
					return false;
				}
			}
		}

		strPath = strPSQuarPath;
	}
	CCCATCHMEM(exInfo);
	if(exInfo.IsException())
		return false;

	return true;
}
// static
BOOL CAVScanObject::IsCategoryNonViral(cc::IIndexValueCollectionPtr &pCategories)
{
    // Look for viral categories
	if(pCategories)
	{
		size_t nCatCount = pCategories->GetCount();

		for ( size_t nCur = 0; nCur < nCatCount; nCur++ )
		{
			DWORD dwCategory = 0;

			if ( !pCategories->GetValue(nCur, dwCategory) )
			{
				CCTRCTXE1(_T("CScanManager::IsNonViral() - Error getting anomaly category %d."), nCur);
				continue;
			}

			// Does this category match any viral categories
			if ( static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==
												ccEraser::IAnomaly::SecurityRisk 
				||
				static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) == 
													ccEraser::IAnomaly::Hacktool 
				||
				static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) == 
														ccEraser::IAnomaly::SpyWare 
				||
				static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==
													ccEraser::IAnomaly::Trackware 
				||
				static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==
														ccEraser::IAnomaly::Dialer 
				||
				static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==
												ccEraser::IAnomaly::RemoteAccess 
				||
				static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) == 
														ccEraser::IAnomaly::Adware 
				||
				static_cast<ccEraser::IAnomaly::Anomaly_Category>(dwCategory) ==
														ccEraser::IAnomaly::Joke )
			{
				return TRUE;
			}
		}
	}

    return FALSE;
}

SYMRESULT CAVScanObject::FireStatus(PFEVENTSTATUS StatusType, LPCWSTR szStatusText, ...)
{
	if(m_pfStatusCallBack == NULL)
	{
		CCTRCTXE0(_T("m_pfStatusCallBack == NULL"));
		return SYM_OK;
	}

	ccLib::CExceptionInfo exInfo;
	try
	{
		va_list va;
		va_start(va, szStatusText);
		ccLib::CStringW sMessage;
		sMessage.FormatV(szStatusText, va);
		va_end(va);

		//working around a buffsize limit in ITB's status event handler
		//oddly, the PreflightScannerObjectBase has a 520 byte limit 
		//in this same function, so we'll avoid that one altogether
		//There are at least 3 different limits within the logging code, so we'll
		// assume the LCD.

		const int buff_size = 0x1ff; //511 bytes
		int len = sMessage.GetLength();

		CCTRCTXI1(_T("lenght = %d"), len);

		if(len < buff_size)
		{
			CCTRCTXI2(_T("%d < %d, just firing directly"), len, buff_size);
			(m_pfStatusCallBack)(m_pCBContext, StatusType, sMessage);
			CCTRCTXI1(_T("[%ls]"), sMessage);
		}
		else
		{
			//ITB will truncate our messagae if its greater than some hardcoded value (perhaps 2k, 1k, ??)
			//Need to split it up into manageable sizes.
			//It will also toss in an extra \r\n, so we have to split this string on newlines.
			//We'll throw them in our selves since there's no buffer size checking in ITB

			CCTRCTXI2(_T("%d >= %d, writing in parts"), len, buff_size);
			int pos = 0, end = 0;
			for(;pos < len;)
			{
				end = sMessage.Find(L"\r\n", pos);
				int count = 0;
				if(end == -1)
					count = len - pos;
				else
					count = end - pos;

				if(count < 1)
				{
					//got \r\n\r\n! - in between the two
					(m_pfStatusCallBack)(m_pCBContext, StatusType, L"\r\n");
					CCTRCTXI0(_T("[\r\n]"));
				}
				else
				{
				
					ccLib::CStringW sPart = sMessage.Mid(pos, count);
					if(end != -1) sPart += L"\r\n";
					(m_pfStatusCallBack)(m_pCBContext, StatusType, sPart);
					CCTRCTXI1(_T("[%ls]"), sPart);
				}

				if(end == -1)
					break;

				pos = end + 2;
			}
		}
	}
	CCCATCHMEM(exInfo);
	if(exInfo.IsException())
		return SYMERR_OUTOFMEMORY;
	return SYM_OK;
}

SYMRESULT CAVScanObject::FireStatus(PFEVENTSTATUS StatusType, LPCSTR szStatusText, ...) 
{
	CCTRCTXE0(_T("Not Implmented"));
	return SYMERR_NOTIMPLEMENTED;
}
	
bool CAVScanObject::IsPathAllowed(const CString& sPath)
{
	ccLib::CExceptionInfo exInfo;
	bool bRet = false;
	try
	{
		for(;;)
		{
			if(sPath.GetLength() < 5)
			{
				CCTRCTXE0(_T("path too short"));
				break;
			}
			//no \ in front of path
			if(sPath[0] != _T('\\'))
			{
				//TODO: Chect drive type??
				
				/*****
				UINT type = GetDriveType(sPath);
				if(type != DRIVE_FIXED || type != DRIVE_CDROM)
				{
					CCTRCTXE2(_T("Unsupported drive type - %d for %s"), type, sPath);
					break;
				}
				*****/

				CCTRCTXI1(_T("%s appears to be local"), sPath);
				bRet = true; 
				break;
			}

			//\??\ (usually used for drivers...)
			if(sPath[1] == _T('?'))
			{
				bRet = true;
				break;
			}
			if(sPath[1] != _T('\\'))
			{
				CCTRCTXW1(_T("Unexpected token in path: %s"), sPath);
				bRet = true;
				break;
			}
			// \\?\ 
			if(sPath[2] == _T('?'))
			{
				// \\?\UNC\server\share
				if(_tcsnicmp(sPath.GetString() + 3, _T("\\UNC\\"), 5) == 0)
				{
					CCTRCTXE1(_T("Unicode UNC path: %s"), sPath);
					break;
				}

				CCTRCTXI1(_T("Unicode path: %s"), sPath);
				bRet = true;
				break;
			}

			// \\.\ (odd for a file path... but we'll let it pass)
			if(sPath[2] == _T('.'))
			{
				CCTRCTXW1(_T("Allowing %s"), sPath);
				bRet = true;
				break;
			}

			// assume UNC from this point on.
			CCTRCTXE1(_T("Assuming UNC: %s"), sPath);
			break;

		}
	}
	CCCATCHMEM(exInfo);
	if(exInfo.IsException())
		bRet = false;
	return bRet;
}

bool CAVScanObject::IsFatalError(const ccEraser::IRemediationAction* pRemediation) const throw()
{
	ccEraser::eObjectType eType = static_cast<ccEraser::eObjectType>(-1);
	ccEraser::eResult eResult = ccEraser::Fail;

	if(pRemediation == NULL)
	{
		CCTRCTXE0(_T("pRemediation == NULL"));
		return false;
	}

	eResult = pRemediation->GetType(eType);
	if(ccEraser::Failed(eResult))
	{
		CCTRCTXE1(_T("GetType == %d"), eResult);
		return false;
	}

	if(eType == ccEraser::InfectionRemediationActionType)
	{
		CCTRCTXE0(_T("eType == InfectionRemediationActionType.  Error is Fatal"));
		return true;
	}

	CCTRCTXE1(_T("Type == %d, error not fatal"), eType);
	return false;
}

bool CAVScanObject::ShouldPrompt() const throw()
{
	bool bRes = true; //default to prompt
	ccLib::CExceptionInfo exInfo;
	try
	{
		InstallToolBox::CInstoptsDat instopts;
		DWORD dwSilent = 0;
		LPCTSTR pszFileName = instopts.GetFilePath();
		if(pszFileName == NULL)
		{
			CCTRCTXE0(_T("failed to load instopts file"));
			return bRes;
		}
		CCTRCTXI1(_T("Using instopts file: %s"), pszFileName);
		if(instopts.ReadDWORD(g_pszSilentScanOpt, dwSilent, 0) == FALSE)
		{
			CCTRCTXW1(_T("Failed to read %ls.  Scan will not be silent"), g_pszSilentScanOpt);
			return bRes;
		}

		CCTRCTXI2(_T("%ls == 0x%08X"), g_pszSilentScanOpt, dwSilent);
		bRes = (dwSilent == 0);
	}
	CCCATCHMEM(exInfo);
	return bRes;
}