////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QBackupInterfaces.h>
#include <EZEraserObjectsw.h>
#include <ccScanwLoader.h>

const TCHAR cszRelativeDefsDir[] = _T("\\Support\\AV\\AV\\VirusDef");
const TCHAR csRelIniPath[] = _T("\\VirusDefs.ini");
const TCHAR csIniSection[] = _T("VirusDefs");
const TCHAR csIniField[] = _T("Directory");
const TCHAR csHardCodedDefsPath[] = _T("C:\\VirusDef");

static const WCHAR* const g_pszSilentScanOpt = L"AVPRESCAN:Silent";


class CEraserScanSink;
class CEraserScanSinkPtr;
class CRemediationData;
class CEZAnomaly;

class CAVScanObject : public CPreFlightScanObjectBase
{
	friend class CSymFactoryImpl;				// Allow CSymFactoryImpl to call our protected constructor

protected:
    CAVScanObject(void) throw();
    virtual ~CAVScanObject(void) throw();


public:

	enum UserDataProperties // IKeyValueCollection properties for keeping our data
    {
        UIInfectionStatus
    };

	// IPreFlightScanner:
	#pragma warning(push)
	#pragma warning(disable:4995)
	//this method is not deprecated,  ccEraser::IEraser::RequestAbort is
	SYMRESULT RequestAbort();
	#pragma warning(pop)
	SYMRESULT Scan( HWND hParentWnd );


	// CAVScanObject:
	static SYMRESULT	Create( CAVScanObject*& pScanObj );			// Create an instance of CAVScanObject. Call Release() when done.

	bool					CheckScanAbort() const { return m_bCancel; }
	ccScanw::SCANWSTATUS	GetProperty( const WCHAR* szProperty, int iDefault, int &riProperty  ) const;
	ccScanw::SCANWSTATUS	OnNewItem( const WCHAR* pszItem );
	ccEraser::eResult		OnAnomalyDetected( ccEraser::IAnomaly* pAnomaly, const ccEraser::IContext* pContext );
	ccEraser::eResult		PostDetection( ccEraser::IDetectionAction* pDetectAction, ccEraser::eResult eDetectionResult, const ccEraser::IContext* pContext );
	// Called after reboot to process remediation actions that are waiting for reboot to be processed:


protected:
	bool				GetLayoutBasePath( ATL::CString& str );
    SYMRESULT			FindDefs(ATL::CString& sDefsDir);

	// This function is added to make scan re-entrant (on the same thread, only)
	void				ResetMembersForNewScan();
	SYMRESULT			InitDependentObjects( CEraserScanSinkPtr& pSink, ccScanw::IScannerwPtr& pScanner, cc::CSymInterfaceTrustedLoader& eraserLoader, ccEraser::IEraser4Ptr &pEraser );

	// the following functions were added to break down scan function
	BOOL				QueryDoScan(HWND hParentWnd, SYMRESULT &symResult);
	BOOL				InitSink( OUT CEraserScanSinkPtr &pSink, OUT SYMRESULT &symResult );
	BOOL				InitScanner( IN LPCTSTR szDefsDir, IN LPCTSTR szBasePath, OUT ccScanw::IScannerwPtr &pScanner, OUT SYMRESULT &symResult ) throw();
	BOOL				InitEraser( IN CEraserScanSink* pSink, IN ccScanw::IScannerw* pScanner, IN LPCTSTR szDefsDir, OUT cc::CSymInterfaceTrustedLoader& loader, OUT ccEraser::IEraser4Ptr &pEraser, OUT SYMRESULT &symResult ) throw();
	BOOL				InitQuarantine( IN ccScanw::IScannerw* pScanner, LPCTSTR szBasePath, OUT SYMRESULT &symResult ) throw();
	BOOL				InitEraserFilters( IN ccEraser::IEraser4* pEraser, OUT SYMRESULT &symResult ) throw();

	// the following functions were added to break down OnAnomalyDetected()
	bool				HandleFileInfection (ccEraser::IRemediationActionPtr&	pRem,
                                             QBackup::IQBackupSetPtr&			pQBSet, 
											  bool&								bCreatedNewQuarantineItem,
											  bool&								bManuallyQuarantined,
											  cc::IKeyValueCollectionPtr        pAnomalyProps,
											  ATL::CString&						sLogDetails);
	
	bool				IsScanInfectionFileGone(ccScanw::IScanwInfection* pInfection);

	bool				PerformRemediationAction(ccEraser::IRemediationActionPtr pRem, 
												 ccEraser::eResult& eRemResult);

	bool				RollbackAnomalyRemediations(QBackup::IQBackupSetPtr&		pQBSet,
													std::vector<CRemediationData>&	vRemData,
													bool							bCreatedNewQuarantineItem);

	bool				SetQuarItemProperties ( QBackup::IQBackupSetPtr&		pQBSet,
												cc::IKeyValueCollectionPtr&		pAnomalyProps,
												std::vector<CRemediationData>&	vRemData,
												ATL::CString&					sLogDetails,
												bool                            bHaveAnUndoForAtLeastOneRemediation);

    // Adds a remediation action and the associated undo data to QBackup
    bool AddRemediationActionToSet(CEZRemediation* pAction, QBackup::IQBackupSet* pSet,
                                   QBackup::IQBackupItemBase*& pItemRemAct, QBackup::IQBackupItemBase*& pItemUndo,
                                   cc::IStream* pUndoData = NULL);

    bool GetUndoData(ccEraser::IRemediationAction* pAction, cc::IStream*& pStream);

    // Files that are waiting for reboot to be deleted
    void				SaveRebootRemediationsToDisk(CEZAnomaly* pEZAnomaly);
    bool				SaveRemediationAction(ccEraser::IRemediationAction* pAction);
    SYMRESULT			PerformPostRebootRemediations(HWND hWnd, bool bShowUI);
	ccEraser::eResult	ProcessRemediationFile(LPCTSTR pcszFileName, ccEraser::IEraser4* pEraser);
	bool				GetUndoData(ccEraser::IRemediationAction* pAction, cc::IStream*& pStream, CString& strUndoFileName);
	void				ReleaseUndoData(cc::IStream* pUndoStream, LPCTSTR pcszUndoFileName);

	// Utility functions
	static void			EnsurePathExists(LPCTSTR szFullPath);
	static CString		GetTempFolder();
	static bool			DeleteDirectoryContents( LPCTSTR szDirectory, BOOL bRecursive );
	static bool			GetPSQuarPath( CString& strPath, bool bCreateFolder = false );
	static BOOL			IsCategoryNonViral(cc::IIndexValueCollectionPtr &pCategories);
	static bool			IsPostRebootRemediationsExist();
	static bool         IsPathAllowed(const CString& sPath);

	SYMRESULT			FireStatus(PFEVENTSTATUS StatusType, LPCWSTR szStatusText, ...);
	SYMRESULT			FireStatus(PFEVENTSTATUS StatusType, LPCSTR szStatusText, ...);

	bool IsFatalError(const ccEraser::IRemediationAction* pRemediation) const throw();
	bool ShouldPrompt() const throw();

protected:
	// m_pQuarServer keeps a reference to m_pScanner, so m_ccScannerLoader
	// MUST remain in-scope until AFTER we release m_pQuarServer, else BOOM!
	cc::CSymInterfaceTrustedLoader	m_ccScannerLoader;

	cc::CSymInterfaceTrustedLoader	m_QBackupLoader;
    QBackup::IQBackupPtr            m_spQBackup;

	size_t							m_nRebootRequired;
	ATL::CString					m_sLogDetails;
	ATL::CString					m_sLogNotImplemented;
	ATL::CString					m_sLogRebootRisks;
	size_t							m_nAnomalyCount;
    size_t							m_nRepairedCount;
	size_t							m_nQuarantinedCount;
	size_t							m_nAnomalyRepairedCount;
	CString							m_strProductName;
	


public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY( IID_PreFlightScanner2, IPreFlightScanner2 )
    SYM_INTERFACE_MAP_END()
};

// {0E6384BB-B8BF-4639-B415-EE716AFCF746}
SYM_DEFINE_OBJECT_ID(CLSID_AVScanObject, 0xE6384BB, 0xB8BF, 0x4639, 0xB4, 0x15, 0xEE, 0x71, 0x6A, 0xFC, 0xF7, 0x46);

typedef CSymPtr<CAVScanObject> CAVScanObjectPtr;
