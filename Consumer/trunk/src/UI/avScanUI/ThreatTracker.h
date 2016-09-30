////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <AVInterfaceLoader.h>
#include <ScanUIInterfaces.h>
#include "avScanUIInternalInterfaces.h"
#include "ExportProgressDlg.h"
#include <ccEraserInterface.h>

//////////////////////////////////////////////////////////////////////////
// CThreat
#define  GETDATAMEMBER(xNAME, xTYPE, xMEMBER)                       xTYPE Get##xNAME() { return xMEMBER; }
#define  SETDATAMEMBER(xNAME, xTYPE, xMEMBER, xCOUNTFORDIRTY)       void Set##xNAME(const xTYPE inVal) { xMEMBER = inVal; if(xCOUNTFORDIRTY) { SetDirty(true); } }
#define  DATAMEMBER(xNAME, xTYPE, xMEMBER, xCOUNTFORDIRTY)          GETDATAMEMBER(xNAME, xTYPE, xMEMBER); \
                                                                    SETDATAMEMBER(xNAME, xTYPE, xMEMBER, xCOUNTFORDIRTY);
class CThreat
{
public:
    enum eIntThreatAction
    {
        eIntAct_None = 0,
        eIntAct_Fix,
        eIntAct_Exclude,
        eIntAct_Review,
        eIntAct_Ignore,
    };

    enum eIntThreatState
    {
        eIntState_CAV = 0,  // Use the state that CAV reports
        eIntState_Ignored,
        eIntState_Pending,
        eIntState_Reviewed,
    };

    enum eTTState
    {
        eTTState_All,
        eTTState_RequireAttention,
        eTTState_Done
    };

public:
    CThreat(AVModule::IAVMapDwordData* pThreatMap)
    {
        m_eIntState = eIntState_CAV;
        m_spThreatMap = pThreatMap;
        m_eCurAction = eIntAct_None;
        m_bDirty = true;    // start out dirty

        // Get this threats ID
        HRESULT hr = pThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_THREATID, m_idThreat);
        if(FAILED(hr))
        {
            CCTRACEE(CCTRCTX L"Failed to retrieve threat ID! hr=%08X", hr);
            return;
        }

        // Get this threats viral flag
        hr = pThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_HASVIRALCATEGORY, m_bViral);
        if(FAILED(hr))
        {
            LOG_FAILURE(L"Failed to get viral flag", hr);
            m_bViral = false;
        }
    }

    CThreat(const CThreat& threat)
    {
        Set(threat);
    }

    // Operators
    CThreat& operator=(const CThreat& threat)
    {
        Set(threat);
        return *this;
    }

    // Gets/Sets
    DATAMEMBER(ID, GUID&, m_idThreat, true);
    DATAMEMBER(ThreatMap, AVModule::IAVMapDwordData*, m_spThreatMap, true);
    DATAMEMBER(Action, eIntThreatAction, m_eCurAction, false);
    DATAMEMBER(IntState, eIntThreatState, m_eIntState, true);
    DATAMEMBER(Dirty, bool, m_bDirty, false)

    bool IsViral() { return m_bViral; }

    // Helpers
    bool IsThreatIncluded(eTTState eState)
    {
        // If caller wants all... we match
        if(eState == eTTState_All)
            return true;

        // Internal state of "Pending" can only match "Attention Required"
        if(m_eIntState == eIntState_Pending)
        {
            if(eState == eTTState_RequireAttention)
                return true;
            else
                return false;
        }


        // Internal state of Ignored or Reviewed can only match "Done"
        if(m_eIntState == eIntState_Ignored || m_eIntState == eIntState_Reviewed)
        {
            if(eState == eTTState_Done)
                return true;
            else
                return false;
        }

        //
        // eIntState wants us to look at the CAV state
        //

        // If we don't have a threat map ... don't include
        if(!m_spThreatMap)
            return false;

        // Get this threats CAV state
        DWORD dwState = AVModule::ThreatTracking::ThreatState_RemoveNotAttempted;
        HRESULT hr = m_spThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_CURRENT_STATE, dwState);
        LOG_FAILURE(L"Failed to get state from CAV ... defaulting to Not Attempted", hr);
        bool bCAVReqAttention = AVModule::ThreatTracking::ThreatNotRemoved(dwState);

        if(eState == eTTState_Done && !bCAVReqAttention)
            return true;
        else if(eState == eTTState_RequireAttention && bCAVReqAttention)
            return true;

        // We didn't match ... don't include
        return false;
    }

    // Changes this items selected action to the "default"
    void DefaultAction()
    {
        // Make sure we are valid
        if(!m_spThreatMap)
            return;

        // Get this threats state
        DWORD dwState = 0;
        HRESULT hr = m_spThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_CURRENT_STATE, dwState);
        LOG_FAILURE(L"Failed to get threat state!", hr);

        // If Do-Not-Delete, default to review
        if(dwState == AVModule::ThreatTracking::ThreatState_DoNotDelete)
        {
            m_eCurAction = eIntAct_Review;
            return;
        }

        // If Viral, default to fix
        if(m_bViral)
        {
            m_eCurAction = eIntAct_Fix;
            return;
        }

        // Overall threat level
        AVModule::ThreatTracking::eThreatLevels eLevel;
        hr = m_spThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, (DWORD&)eLevel);
        LOG_FAILURE(L"Failed to get global threat level!", hr);

        // If Medium or High, default to fix
        if(eLevel == AVModule::ThreatTracking::ThreatLevel_Med
            || eLevel == AVModule::ThreatTracking::ThreatLevel_High)
        {
            m_eCurAction = eIntAct_Fix;
            return;
        }

        // All that is a remaining is a low/non-viral threat, which is "ignore once"
        m_eCurAction = eIntAct_Ignore;
    }

protected:
    void Set(const CThreat& threat)
    {
        m_idThreat = threat.m_idThreat;
        m_spThreatMap = threat.m_spThreatMap;
        m_eCurAction = threat.m_eCurAction;
        m_eIntState = threat.m_eIntState;
        m_bViral = threat.m_bViral;
        m_bDirty = true;    // always dirty after a copy
    }

protected:
    GUID m_idThreat;
    AVModule::IAVMapDwordDataPtr m_spThreatMap;
    eIntThreatAction m_eCurAction;
    eIntThreatState m_eIntState;
    bool m_bViral;
    bool m_bDirty;
};

//////////////////////////////////////////////////////////////////////////
// CThreatTracker
class CThreatTracker :
    public ccLib::CThread,
    public AVModule::IAVThreatProcessingSink,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(AVModule::IID_AVThreatProcessingSink, AVModule::IAVThreatProcessingSink)
    SYM_INTERFACE_MAP_END()

    typedef CAtlMap<ccEraser::eObjectType, int> MAP_TYPE_COUNT;
    typedef CAtlMap<ccEraser::IAnomaly::Anomaly_Category, int> MAP_CAT_COUNT;
    typedef CAtlArray<size_t> ARRAY_INDEXES;

public:
    CThreatTracker(AVModule::IAVScanBase* pScanner, avScanUI::IScanUICallbacksInt* pScanUI);
    CThreatTracker(AVModule::IAVThreatInfo* pThreatInfo, bool bRegisterSink);
    virtual ~CThreatTracker(void);

    // Must be called before Release(), otherwise Release() will do nothing!
    void CleanupCircularRefs();

    // Can the IScanUICallbacksInt object
    avScanUI::IScanUICallbacksInt* GetScanUI();

    // Called during OnScanComplete() to get a ref to the scan results
    void SetScanResults(AVModule::IAVMapStrData* pResults);

    // Add a threat to the threat tracker...
    bool AddThreat(AVModule::IAVMapDwordData* pThreatMap, bool bReplaceOnly = false, size_t* piNewIndex = NULL);

    // Get the count of threats
    size_t GetThreatCount();

    // Get the count of threats
    void GetThreatCountEx(int& nViral, int& nNonViral, CThreat::eTTState eState = CThreat::eTTState_All);

    // Returns true if the threat in question should be included based on the input state
    bool IsThreatIncluded(size_t iThreatIndex, CThreat::eTTState eState);

    // Find a threat index by threat guid (returns false if not found)
    bool FindThreatIndexByID(const GUID& idThreat, size_t& iIndex);

    // Display the details of a threat (anomaly dump)
    HRESULT DisplayThreatDetails(HWND hParentWnd, size_t iThreatIndex, bool bStartOnFirstPage = true);

    // Open the users browser to the SARC write up on this threat
    HRESULT LaunchResponsePage(size_t iThreatIndex);

    // Set a threats currently selected action
    void SetThreatAction(size_t iThreatIndex, CThreat::eIntThreatAction eAction);

    // Gets a threats currently selected action
    CThreat::eIntThreatAction GetThreatAction(size_t iThreatIndex);

    // Takes action on a threat (based on currently selected action)
    HRESULT TakeThreatAction(size_t iThreatIndex, HWND hParentWnd, bool bSkipReview = false);

    // Applies all defined threat actions
    // returns S_OK if one or more succeeded, S_FALSE if no actions were defined
    HRESULT ApplyAllThreatActions(HWND hParentWnd);

    // Threat dirty flag
    bool GetThreatDirty(size_t iThreatIndex);
    void SetThreatDirty(size_t iThreatIndex, bool bDirty);

    // Returns counts for each anomaly category
    HRESULT GetAnomalyCategoryStats(CStringW& sCategoryStats, CThreat::eTTState eState, MAP_CAT_COUNT* pRawMap = NULL);

    // Fills in a _map_guid_data with the currently selected action for each threat
    HRESULT FillActionsMap(AVModule::IAVMapGuidData* pActionMap);

    // Fills in the MAP_TYPE_COUNT with a count of each type found in the threat
    //  ... Quicker version available if the caller has all the required inputs
    HRESULT GetTypeCountsForThreat(size_t iThreatIndex, MAP_TYPE_COUNT& mapCounts);
    HRESULT GetTypeCountsForThreat(CThreat& cThreat, MAP_TYPE_COUNT& mapCounts);
    HRESULT GetTypeCountsForThreat(AVModule::IAVMapDwordData* pThreatMap, AVModule::ThreatTracking::eThreatTypes eType,
                                   MAP_TYPE_COUNT& mapCounts, bool bFilterForUser = false);

    // Fills in the array with each target of the given type
    //  ... Quicker version available if the caller has all the required inputs
    HRESULT GetTargetsForThreat(size_t iThreatIndex, ccEraser::eObjectType eDispType, CAtlArray<CStringW>& arrTargets);
    HRESULT GetTargetsForThreat(CThreat& cThreat, ccEraser::eObjectType eDispType, CAtlArray<CStringW>& arrTargets);
    HRESULT GetTargetsForThreat(AVModule::IAVArrayData* pDisplayData, AVModule::ThreatTracking::eThreatTypes eType, AVModule::IAVMapDwordData* pThreatMap,
                                ccEraser::eObjectType eDispType, CAtlArray<CStringW>& arrTargets, bool bFilterForUser = false, CAtlArray<CStringW>* pActions = NULL);

    // ccEraser type grouping helper
    ccEraser::eObjectType GroupType(ccEraser::eObjectType eRemType);
    ccEraser::IAnomaly::Anomaly_Category GroupCategory(ccEraser::IAnomaly::Anomaly_Category eCat);

    // Returns true if any tracked threats require a reboot
    bool RebootRequired();

    // Get array of indexes for specified type
    bool GetIndexArrayForState(CThreat::eTTState eState, ARRAY_INDEXES& arrIndexes);

    // Takes the template passed in and duplicates it (filling it in) for
    // each threat found in the threat tracker.  Template keys are:
    // %{IDX}%                        - This items index
    // %{TID}%                        - Threat ID (GUID)
    // %{TYPE}%                       - Threat Type (Virus, Trojan, Joke, etc)
    // %{STATE}%                      - State (fixed/error/etc)
    // %{STATEDESC}%                  - Description string that includes the state ... should be "%{NAME}% %{STATEDESC}%"
    // %{DEPENDS}%                    - Dependency text (yes/no)
    // %{VID}%                        - Virus ID
    // %{NAME}%                       - Threat name
    // %{TCATS}%                      - Comma separated list of threat categories
    // %{DESCTCATS}%                  - Paragraph with once sentence per threat category
    // %{DETTIME}%                    - Detection time
    // %{REMTIME}%                    - Removal time
    // %{INFOURL}%                    - Info URL
    // %{TLEVEL}%                     - Overall threat level
    // %{TLEVEL[PROP=NNN]}%           - Individual threat level (NNN = ccEraser::IAnomaly::Property)
    // %{TLIMG}%                      - Overall threat level graphic (just the "bla.png" portion)
    // %{TLINT}%                      - Overall threat level as a number (1=high, 2=med, 3=low)
    // %{DESCTLEVEL}%                 - Overall threat level description
    // %{DESCTLEVEL[PROP=NNN]}%       - Individual threat level description (NNN = ccEraser::IAnomaly::Property)
    // %{ACTIONDROPDOWN[PREID=BLA]}%  - <select> statement for valid user choices ... Optional PREID can be
    //                                 specify a string that is prepended to the ID attributes
    // %{REMDETAILS[FMT=X;CLASS=Y]%}  - Fills in remediation details (counts for each type of remediation)
    //                                  FMT=What type of formatting - T=table rows; D=divs; X=XML; P=Plain text
    //                                  Y=CSS class name for rows or divs
    //                                  TYPE=F - adds targets to the output for each remediation type
    //  ... Quicker version available if the caller has all the required inputs
    CStringW GenerateOutput(LPCWSTR pszTemplate, CThreat::eTTState eState);
    CStringW GenerateOutputForItem(LPCWSTR pszTemplate, size_t iIndex);
    CStringW GenerateOutputForItem(CThreat& cThreat, LPCWSTR pszTemplate, size_t iIndex);
    CStringW GenerateOutputForItem(LPCWSTR pszTemplate, size_t iIndex, AVModule::IAVMapDwordData* pThreatMap,
                                   AVModule::ThreatTracking::eThreatTypes eType, AVModule::IAVArrayData* pDisplayData = NULL);

    // Export the results to a text file...
    bool ExportThreats(LPCWSTR pszOutputFile, HWND hParentWnd, bool bShowProgress);

    // IAVThreatProcessingSink
    virtual void OnThreatProcessingProgress(DWORD dwRemainingItems, AVModule::IAVMapDwordData* pCurItemResults, AVModule::IAVMapDwordData* pRetryParams) throw();
    virtual void OnThreatProcessingComplete(AVModule::IAVMapDwordData* pProcessingResults) throw();

protected:
    HRESULT GetReplacementValue(LPCWSTR pszKey, AVModule::IAVMapDwordData* pThreatMap, AVModule::ThreatTracking::eThreatTypes eType, 
                                AVModule::IAVArrayData* pDisplayData, size_t iIndex, CStringW& sRet);
    CStringW GetParamValue(LPCWSTR pszKey, LPCWSTR pszParamName);
    bool GotoURL(LPCWSTR pszUrl, int nShowcmd);
    bool DoesFileExist(LPCWSTR pszFile);
    CStringW GetXMLTagForEraserType(ccEraser::eObjectType eType, bool bOuter, bool bClose);
    HRESULT GetAnomalyDisplayData(AVModule::IAVMapDwordData* pThreatMap, const GUID& idThreat, bool bCheckPresence, AVModule::IAVArrayData*& pDisplayData);
    bool AccessCheck(AVModule::IAVMapDwordData* pMapInfo, AVModule::IAVMapDwordData* pThreatMap, HANDLE hClientTotken);
    CStringW TranslateRemediationActionTaken(ccEraser::eObjectType eRemType, AVModule::IAVMapDwordData* pMapRemInfo, AVModule::ThreatTracking::eThreatState eThState);

    // ccLib::CThread (used for exporting)
    virtual int Run() throw();

    typedef struct tagEraserStats
    {
        int nViral;
        int nMalicious;
        int nReservedMalicious;
        int nHeuristic;
        int nSecurityRisk;
        int nHacktool;
        int nSpyWare;
        int nTrackware;
        int nDialer;
        int nRemoteAccess;
        int nAdware;
        int nJoke;
        int nClientCompliancy;
        int nGenericLoadPoint;
        int nApplicationHeuristic;
    } ERASER_STATS;

protected:
    AVModule::IAVScanBasePtr m_spScanner;
    AVModule::IAVThreatInfoPtr m_spThreatInfo;
    avScanUI::IScanUICallbacksIntPtr m_spScanUI;
    bool m_bSinkRegistered;
    avScanUI::eProcTermResult m_eProcTermRes;
    AVModule::AVLoader_IAVMapDwordData m_loaderMapDwordData;
    AVModule::IAVMapStrDataPtr m_spScanResults;

    // Export params
    CStringW m_sExportFile;
    CSymPtr<CExportProgressDlg> m_spDlgDoc;

    // Threat tracking data
    typedef CAtlArray<CThreat> THREAT_ARRAY;
    THREAT_ARRAY m_arrThreats;

    // Syncronization
    ccLib::CMutex m_mtxData;
};
