#pragma once

#include "ThreatCatInfo.h"
#include "SpywarePopup.h"
#include "SideEffectsPopup.h"
#include "VirusAlertPopup.h"
#include "SimpleTimer.h"

#include "AVTaskLoader.h" // for IScanTask
#include "ScanTask.h"     // for IScanTask

// Events
#include "AllNAVEvents.h"
#include "AvEvents.h"

#include "ScanMonitor.h"

#include "ExclusionManagerInterface.h"
#include "ExclusionManagerLoader.h"

// Class used to manage non-viral files being manually scanned
// so user is not alerted while the manual scan is already
// dealing with the file
class CManualScannedFiles
{
public:
    CManualScannedFiles();
    ~CManualScannedFiles();

    bool AddFile(long lCookie, LPCWSTR pcwszFileName);
    bool FileBeingScanned(LPCWSTR pcwszFileName);
    void ScanCompleted(long lCookie);

protected:
    typedef std::set<ATL::CAtlStringW> setFiles;
    typedef setFiles::iterator iterSetFiles;
    typedef std::map<long,setFiles> mapScannedFilesByCookie;
    typedef mapScannedFilesByCookie::iterator iterMapScannedFiles;

    // This will keep track of the files being manually scanned based
    // on a unique cookie identifier for which scan is handling the file
    mapScannedFilesByCookie m_mapFilesBeingScanned;

    // Maintain thread safety for our STL objects
    ccLib::CCriticalSection m_critSec;
};

class CVirusAlertQueue : public ccLib::CThread, public CScanMonitorSink
{
public:
    CVirusAlertQueue(void);
    virtual ~CVirusAlertQueue(void);

    // CThread
    int Run (void);

    void AddVirusAlert ( CVirusAlertPopup* pVirusPopup );
    void AddSpywareAlert ( unsigned long ulVirusID,
                           std::wstring& strFileName,
                           std::wstring& strThreatName,
                           std::string& strThreatCats,
                           bool bCompressed);

    bool manualScanFileEvent (long lScanCookie,
                              unsigned long ulVirusID,
                              LPCWSTR cpszwStrFile);

    void manualScanCompleteEvent (long lScanCookie);

    // For turning off displaying alerts (stress testing)
    static void SetDisplayAlert ( bool bDisplayAlert );
    static void SetAutoScanSideEffects ( bool bAutoScanSideEffects );

    // Scan monitor
    void ScanFinished ( HANDLE hScan );

protected:
    static bool m_bAutoScanSideEffects;
    static bool m_bDisplayAlert;

    std::queue<CVirusAlertPopup*>   m_qpVirusAlerts;
    ccLib::CCriticalSection         m_critVirusAlerts;
    ccLib::CEvent                   m_eventNewWork;

	CString m_csProductName;

    bool displayVirusAlerts();
    void doSideEffectsScan(bool bSilent);
    void doSpywareAlert();
    void doSideEffectsAlert();
    bool isVIDIgnored ( unsigned long ulVirusID );

    // Scan stuff
    bool launchScan (IScanTask* pTask/*in*/, HANDLE& hScan/*out*/, bool bScanNonVirals/*in*/, bool bSilent = false);
    DWORD generateTempScanFile( LPSTR szOut, int nBufferSize );
    HRESULT ErrorMessageBox(long nErrorId);
    bool wideStrToOemStr ( LPCWSTR lpWide /*in*/, std::string& strOem /*out*/ );
    bool wideStrToAnsiStr ( LPCWSTR lpWide /*in*/, std::string& strAnsi /*out*/ );
    std::auto_ptr<CScanMonitor> m_pScanMonitor;

    // Timers
    // 0 = side-effects, 1 = spyware
    void resetTimer(long lTimerID, int iTimeLength);
    HANDLE m_hTimers [2];
    
    // Other system event stuff
    bool isCfgWizRunning();
    bool isOtherScanRunning();

// FILE/VID stuff
    struct DELAYED_ALERT_DATA // unique index
    {
        std::wstring strFileName;
        unsigned long ulVirusID;
        std::string strThreatCats;
        std::wstring strThreatName;
        HANDLE hScan;
        bool bCompressed;
    };

    std::list<DELAYED_ALERT_DATA> m_listDelayedAlertData;
    typedef std::list<DELAYED_ALERT_DATA>::iterator iterDelayedAlertData;
    typedef std::list<iterDelayedAlertData> listScanned;
    typedef listScanned::iterator iterScanned;
    ccLib::CCriticalSection m_critDelayedAlertData;

//    typedef std::map <long, CSimpleTimer*> mapIgnoredVIDs;
//    typedef mapIgnoredVIDs::iterator iterIgnoredVIDs;
//    mapIgnoredVIDs m_mapIgnoredVIDs;
    ccLib::CCriticalSection m_critIgnoredVIDs;

    // true == new, added to list
    bool addDelayedAlert (unsigned long ulVirusID,
                          std::wstring& strFileName,
                          std::wstring& strThreatName,
                          std::string& strThreatCats,
                          bool bCompressed = false);

    bool m_bSpywareScanRunning;

    // Manual scan data
    CManualScannedFiles m_ManualScanFiles;

    // New exclusions handlers
    NavExclusions::N32Exclu_IExclusionManagerFactory m_ExclusionMgrFac;
    NavExclusions::IExclusionManagerPtr m_spExclusionMgr;

};
