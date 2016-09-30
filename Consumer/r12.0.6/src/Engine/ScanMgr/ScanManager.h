
#ifndef SCAN_MANAGER_H
#define SCAN_MANAGER_H

#include <vector>
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>
#include "ccScanInterface.h"
#include "ccDecInterface.h"
#include "ccDecLoader.h"
#include "ScanMgrInterface.h"
#include "CommonUIInterface.h"
#include "Exclusions.h"
#include "SmartScan.h"
#include "ActivityLog.h"
#include "Quarantine.h"
#include "DisplayNames.h"
#include "OEHScan.h"
#include "ccSymInterfaceLoader.h"
#include "ccEraserInterface.h"
#include "AutoProtectWrapper.h"

#include "ccScanLoader.h"
#include "CommonUIThread.h"
#include "Infectionmerge.h"

#include "EZEraserObjects.h"

#include "EraserScanSink.h"
#include "EraserCallbacks.h"
#include "ScanSink.h"
#include "ScanProperties.h"
#include "ScanItems.h"
#include "ScanManagerCounts.h"
#include "NotifyAPSpywareDetections.h"

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
                    if( DeleteFile(strFileName) )
                        CCTRACEI(_T("CRemediationData::~CRemediationData() - Successfully deleted backup undo file %s"),(LPCTSTR)strFileName);
                    else
                        CCTRACEE(_T("CRemediationData::~CRemediationData() - Failed to delete backup undo file %s"),(LPCTSTR)strFileName);
                }
            }
            else
            {
                cc::IStringPtr psymstrDesc;
                pRemediation->GetDescription(psymstrDesc);
            }
        }
    }

    ccEraser::IRemediationActionPtr pRemediation;
    cc::IStreamPtr pUndo;
    ccEraser::eResult eRemediationResult;
    ATL::CAtlString strFileName;
    bool bManuallyQuarantined;
};

class CScanManager :
    public IScanManager,
    public ICommonUISink,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(IID_IScanManager, IScanManager)
        SYM_INTERFACE_ENTRY(IID_ICommonUISink, ICommonUISink)
    SYM_INTERFACE_MAP_END()

    // Constructor.
    CScanManager();

    // Destructor
    virtual ~CScanManager();

    // IScanManager overrides.
    virtual SMRESULT ScanAndRepair(const char* szAppID);
    virtual bool AddItemToScanList(const char* szItem);
    virtual bool AddItemToScanList(const char* szItem, unsigned long ulVID, bool bCompressed);
    virtual void ResetScanList();
    virtual int GetScanListItemCount();
    virtual void OverrideSetting(SMSETTING Setting, bool bEnable);
    virtual void OverrideResponse(SMRESPONSE Response);
    virtual void OverrideNonViralResponse(SMNONVIRALRESPONSE Response);
    virtual void OverrideBloundhoundLevel(int iLevel);
    virtual void OverrideMaxDecomposerExtractSize(unsigned long uBytes);
    virtual void OverrideMaxDecomposerExtractDepth(int iLevels);
    virtual void SetUserInterface(SMUSERINTERFACE UserInterface);
    virtual void SetEmailInfo(const char* szSubject, const char* szSender,
        const char* szRecipient);
    virtual void SetScanName(const char* szScanName);
    virtual void SetOEHInfo(DWORD dwClientPID, const char* szClientImage);
    virtual void SetParentWindow(HWND hWnd);
    virtual int GetTotalInfectionsFound();
    virtual int GetTotalInfectionsFixed();
    virtual void DeleteObject();

    // IScanSink2 implementation, the CScanSink and CEraserScanSink classes will call into
    // these for the actually sink functionality.
    SCANSTATUS OnBusy();
    SCANSTATUS OnError(IScanError* pError);
    SCANSTATUS OnNewItem(const char* pszItem);
    SCANSTATUS OnNewFile(const char* pszLongName, const char* pszShortName);
    SCANSTATUS OnNewDirectory(const char* pszLongName, const char* pszShortName);
    SCANSTATUS OnInfectionFound(IScanInfection* pInfection);
    SCANSTATUS OnRepairFailed(IScanInfection* pInfection);
    SCANSTATUS OnRemoveMimeComponent(IScanInfection* pInfection, char* pszMessage,
        int iMessageBufferSize);
    SCANSTATUS OnCleanFile(const char* pszFileName, const char* pszTempFileName);
    SCANSTATUS OnBeginThread();
    SCANSTATUS OnEndThread();

    // Called by IScanSink3 sink when a container is done processing so we can deal with
    // any compressed items detected
    void OnPostProcessContainer(const char * pszLongName, const char * pszShortName);
    SCANSTATUS OnDeleteFailed( IScanInfection* pInfection );

    SCANSTATUS OnCompressedInfectionFound(IScanCompressedFileInfection2* pCompressed, bool bNonViral );

    // Called by IScanSink3 sink before a decomposer item is scanned
    SCANSTATUS OnNewCompressedChild(::cc::IDeccomposerContainerObject * pIDecomposerContainerObject, const char * pszChildName);

    // IScanProperties implementation, the CScanProperties class will call into these methods
    // for the real functionality.
    int GetProperty(const char* szProperty, int iDefault);
    const char* GetProperty(const char* szProperty, const char* szDefault);

    // IScanItems implementation, the CScanItems will call these methods.
    int GetScanItemCount();
    const char* GetScanItemPath(int iIndex);

    // ccEraser::ICallback implementation, the CEraserCallbacks will call these methods
    ccEraser::eResult PreDetection(ccEraser::IDetectionAction* pDetectAction, const ccEraser::IContext* pContext) throw();
    ccEraser::eResult PostDetection(ccEraser::IDetectionAction* pDetectAction, ccEraser::eResult eDetectionResult, const ccEraser::IContext* pContext) throw();
    ccEraser::eResult OnAnomalyDetected(ccEraser::IAnomaly* pAnomaly, const ccEraser::IContext* pContext) throw();

    // Notify Auto-Protect of non-viral infections encountered so it does not double alert
    void NotifyAP(IScanInfection* pInfection);

    // ICommonUISink overrides.
    virtual HRESULT OnCUIReady();
    virtual HRESULT OnCUIAbort();
    virtual HRESULT OnCUIPause();
    virtual HRESULT OnCUIResume();
    virtual HRESULT OnCUIHandleAnomalies();
    virtual HRESULT OnCUIGetCurrentFolder(LPSTR szFolder, long nLength);
    virtual HRESULT OnCUIScanFileCount(long& nScanFileCount);
    virtual HRESULT OnCUIVirusCount(long& nVirusCount);
    virtual HRESULT OnCUIRemovedVirusCount(long& nRemovedVirusCount);
    virtual HRESULT OnCUINonViralCount(long& nNonViralCount);
    virtual HRESULT OnCUIRemovedNonViralCount(long& nRemovedNonViralCount);
    virtual HRESULT OnCUIGetQuarantinedCount(long& nQuarantineCount);
    virtual HRESULT OnCUIAdditionalScan(bool& bAdditionalScan);

protected:
    typedef std::map<unsigned long, size_t> ANOMALYVIDINDEXMAP;

    // Interface sinks
    CEraserScanSink* m_pEraserScanSink;
    CEraserCallbacks* m_pEraserCallback;
    CScanSink* m_pScanSink;
    CScanProperties* m_pScanProperties;
    CScanItems* m_pScanItems;

    // Activity logger
    CActivityLog* m_pActivityLog;

    // Scanner Loader and object
    cc::ccScan_IScanner4 m_ccScanLoader;
    IScanner4* m_pScanner;

    // SymInterface loader for ccEraser and IEraser object
    cc::CSymInterfaceTrustedLoader m_ccEraserLoader;
    ccEraser::IAnomalyListPtr m_pAnomalyList;
    ccEraser::IEraserPtr m_pEraser;
    ccEraser::IFilterPtr m_pInputFilter;
    ccEraser::IFilterPtr m_pOutputFilter;

    ANOMALYVIDINDEXMAP m_mapNonRelevantAnomalyIndex;
    ccEraser::IAnomalyListPtr m_pNonRelevantAnomalyList;

    // Flag indicating an eraser scan is in progress
    bool m_bEraserScanning;

    // Flag indicating additional scan (beyohnd what was originally
    // requested) is occurring or has occurred.
    bool m_bAdditionalScan;

    // Flag indicating that the scan in progress is a full system scan
    bool m_bFullScan;

    // Items to scan.
    std::vector<std::string> m_ItemsToScan;

    typedef std::map<unsigned long, cc::IIndexValueCollectionPtr> FILEVIDMAP;
    FILEVIDMAP m_mapFileVidsToScan;

    // Settings.
    std::map<int, bool> m_Settings;

    // First and last known threat categories
    DWORD m_dwLastKnownThreatCategory;
    DWORD m_dwFirstNonViralThreatCategory;

	// Bloodhound sensitivity level.
	int m_iBloodhoundLevel;

	// Maximum decomposer extraction size (in bytes).
	DWORD m_dwMaxDecomposerExtractSize;

	// Maximum extraction levels for decomposer.
	int m_iMaxDecomposerExtractDepth;

    // ScanDll depth for ccEraser
    DWORD m_dwScanDllDepth;

	// Type of response to infections (manual, auto-repair, auto-repair-quarantine).
	SMRESPONSE m_Response;

    // Type of response to non-viral threats (manual, auto-delete).
	SMNONVIRALRESPONSE m_NonViralResponse;

	// Type of user-interface.
	SMUSERINTERFACE m_UserInterface;

	// Email information.
	std::string m_strEmailSubject, m_strEmailSender, m_strEmailRecipient;

	// Pointer to the Common UI thread object.
	CCommonUIThread* m_pCommonUI;

	// Flag used to indicate/request an aborted scan.
	bool m_bAbortScan;

	// Flag used to indiciate a paused scan.
	bool m_bPauseScan;

	// SmartScan.
	CSmartScan m_SmartScan;
    const CSmartScan& m_cSmartScan; // This const reference ensures multi-threaded scans behave.

    // AutoProtect toggle.
    CAutoProtectWrapper m_AutoProtectToggle;

    // Exclusions
    CExclusions m_Exclusions;

    // map to keep track of the thread IDs we have successfully initialized COM on
    // for multi-threaded scans
    std::map<DWORD, bool> m_ComThreadInitMap;

	// Quarantine wrapper.
	CQuarantine m_Quarantine;
	
	// Number of seconds it took to perform the scan.
	DWORD m_dwTimeToScan;

    // Number of seconds the scan is paused.
    long m_lCurTimePaused;
	long m_lTotalTimePaused;

	// Parent window handle for the Common UI.
	HWND m_hWndParent;

	// Event used to signal the scan thread when the Common UI is ready.
	// The event is only used for scans that display progress.
	HANDLE m_hUIReadyEvent;

	// Event used to signal the scan thread to continue after being paused.
	HANDLE m_hUIResumeEvent;

	// Class that gives us full-length descriptions of infections.
	CDisplayNames m_DisplayNames;

	// Name of the current Scan
	std::string m_strScanName;

	// OEH wrapper.
	COEHScan m_OEHScan;

	// Client process ID (used for OEH).
	DWORD m_dwClientPID;
	
	// Client process image file (used for OEH).
	std::string m_strClientImage;

    // Enabled status of non-viral threat categories
    std::map<DWORD, bool> m_CategoryEnabled;

    // Number of threads to use for scanning if multi-threading is enabled
    DWORD m_dwScanThreads;

    // Should each file scanned be logged before the scanner scans it ?
    bool m_bLogFiles;

	CString m_strProductName;

	// Converts scan items to OEM.
	void ConvertItemsToOEM();

	// Helper function to determine if it is a network drive
	bool IsNetworkDrive(const char* szItem);

	// Helper called by ScanAndRepair() for scans with progress UI.
	SMRESULT ScanAndRepairWithProgress();

	// Helper called by ScanAndRepair() for scans without progress UI.
	SMRESULT ScanAndRepairWithoutProgress();

	// Helper called to configure our Decomposer instance from GetScanner()
	SMRESULT ConfiureDecomposerManager();

	// Returns the memory infection status for the Common UI.
	MemoryScanStatus GetMemoryScanStatus();

	// Our Decomposer Manager that we will pass will pass to CC
    ::cc::IDecomposerMgr2Ptr m_pDecomposerMgr;

	// Common scan function.
	SMRESULT Scan();

    // CommonUI repair anomalies thread.
    static unsigned __stdcall OnCUIHandleAnomalyProc(void* pArgument);

    // Determines if the infection is a non-viral threat
    bool IsNonViral(IScanInfection* pInfection);

    // Determines if the infection is a virus
    bool IsViral(IScanInfection* pInfection);

    static ccLib::CCriticalSection m_critScanSink;
    ccLib::CEvent m_eventScanError;

    CScanManagerCounts m_ScanMgrCounts;

    // Initializes the class.
	void Initialize();

    // Get the scan engine
    SMRESULT GetScanner(const char* szAppID,
			   int iBloodhoundLevel,
			   long lFlags);

    // Internal ccEraser helper functions
    SMRESULT GetEraser();
    SMRESULT SetEraserSettings();
    void SetEraserFilter();

    ATL::CAtlString strMimeReplacement;

    std::string m_strAppID;
    TCHAR m_szTestDefs[MAX_PATH+1];

    //
    // Anomaly helper functions
    //

    // Are all of the threat categories for this item disabled
    bool AreCategoriesDisabled(IScanInfection* pInfection);

    // Scan each of the file VID items in our scan list
    void DoFileVidReverseScan();
    bool FindCorrespondingQuarantineItem(CEZAnomaly* pEZAnomaly, DWORD dwVID, UUID &uuid);

    // Scan the generic load points only
    void DoSingleGenericLoadPointScan();

    class CAnomalyContext
    {
    public:
        CAnomalyContext();

        std::vector<DWORD> m_vdwActions;
        bool m_bQuarantineBackup;
        unsigned long m_ulQuarItemStatus;
        CEZAnomaly* m_pEZAnomaly;
    };

    // Processes an anomaly
    bool HandleAnomaly(CEZAnomaly* pEZAnomaly);

    // This is used by the anomaly handling method to get the anomaly context 
    // information we need before attempting to remediate an anomaly, if this
    // returns false then the anomaly should not be processed
    bool PreProcessAnomaly(CAnomalyContext& context);

    // This flag will be true when we are processing anomalies during the scan and false when
    // we are processing items from CommonUI so that our HandleAnomaly routine can tell the difference
    bool m_bAutomaticMode;

    bool GetUndoData(ccEraser::IRemediationAction* pAction, cc::IStream*& pStream, ATL::CAtlString& strUndoFileName);
    void ReleaseUndoData(cc::IStream* pUndoStream, LPCTSTR pcszUndoFileName);
    bool PerformRemediationAction(CEZRemediation* pAction, ccEraser::eResult& eRemResult);
    SCANSTATUS DealWithBootScanInfection(IScanInfection* pInfection);

    // This class handles filtering out identical VID\file detections
    CInfectionMerge m_InfectionMerge;
    void MergeScanInfectionItems(CEZAnomaly* pEZAnomaly);

    // Map of VID's for specific anomalies we've actually detected and their index into the anomaly list.
    ANOMALYVIDINDEXMAP m_mapSpecificAnomalyIndex;
    bool AddInfectionToExistingSpecificAnomaly(IScanInfection* pInfection);
    bool AddInfectionToExistingNonRelevantAnomaly(IScanInfection* pInfection);

    // Check for the presence of each remediation action in an anomaly
    bool CheckPresenceOfRemediationActions(CEZAnomaly* pEZAnomaly);

    // Memory termination prompting and result
    bool m_bTerminateProcessPrompted;
    bool m_bTerminateProcessAccepted;

    long m_lDamageControlItems;

    // Compressed infections waiting for the container scan to complete to be handled
    typedef std::vector<IScanCompressedFileInfection2Ptr> COMPRESSED_ITEM_VECTOR;
    typedef std::map<ATL::CAtlString,COMPRESSED_ITEM_VECTOR> COMPRESSED_ITEM_MAP;
    COMPRESSED_ITEM_MAP m_mapCompressedItems;
    void RemoveFromCompressedMap(IScanInfection* pInfection);

    // Anomalies being processed automagically during the scan will need to be looked
    // up by their IScanInfection pointer.
    typedef std::map<IScanInfection*, ccEraser::IAnomalyPtr> SCANINFECTIONTOANOMALYMAP;
    SCANINFECTIONTOANOMALYMAP m_mapInfectionsToAnomalies;

    bool SetAnomalyValuesFromInfection(ccEraser::IAnomaly* pAnomaly, IScanInfection* pInfection);

    bool GenerateAnomalyFromInfection(IScanInfection* pInfection,
                                      ccEraser::IAnomaly*& pAnomaly,
                                      CommonUIInfectionStatus eStatus,
                                      AnomalyAction eNextAction,
                                      bool bCheckforSpecifics = true,
                                      bool bIncrementFindCounts = true);

    bool GenerateInfectionRemediationAction(IScanInfection* pInfection, ccEraser::IRemediationAction*& pRemediation);

    // Files that are waiting for reboot to be deleted
    std::set<ATL::CAtlString> m_vRebootFiles;
    void SaveRebootRemediationsToDisk(CEZAnomaly* pEZAnomaly);
    bool SaveRemediationAction(ccEraser::IRemediationAction* pAction);
    void PerformPostRebootRemediations();
    ccEraser::eResult ProcessRemediationFile(LPCTSTR pcszFileName);

    // Do the non-viral threat damage flag values indicate we should auto-delete?
    bool ShouldAutoDeleteBasedOnDamageFlags(CEZAnomaly* pEZAnomaly, bool bUseDependencyFlag = true);

    bool IsScanInfectionFileGone(IScanInfection* pInfection);
    
    // Used to build a path
    void CScanManager::EnsurePathExists(LPCTSTR szFullPath);

    // Used for AP notification
    CNotifyAPSpywareDetections m_NotifyAPSpywareDetection;

    SCANSTATUS HandlePauseAbort(LPCTSTR pcszFuncName);

	void HandleInvalidEraserVersion(DWORD dwVerMost, DWORD dwVerLeast);

    bool m_bIgnoreOnAnomalyDetected;
};

#endif
