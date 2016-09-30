#pragma once

#include "ccEraserInterface.h"
#include "CommonUIInterface.h"
#include "iquaran.h"

//
// Note: These EZ classes contain smart pointers to SymInterface objects, such
//       as IKeyValueCollections for user data. When these items are queried 
//       for (i.e. GetUserData()) the object is not AddRefed before returning 
//       so that clients can assign them directly to a smart pointer. This is 
//       not how object ref counting is normally performed thus this note is 
//       being made. The correct usage for retrieving a SymInterface object 
//       from the EZ classes is:
//       cc::IKeyValueCollectionPtr spUserData = EZAnomaly.GetUserData();

enum AffectedTypes
{
    typeFirst = 0,
    typeFile = typeFirst,
    typeProcess,
    typeBR,
    typeMBR,
    typeReg,
    typeBatch,
    typeINI,
    typeService,
    typeCOM,
    typeHosts,
    typeLSP,
    typeUnkown,
    typeLast = typeUnkown
};

class CRemediationStats
{
public:
    CRemediationStats(void);
    ~CRemediationStats(void);

    //
    // Must be called before any real statistics can be retrieved,
    // If you want to reinitialize with a new anomaly you must first call 
    // Clear()
    //
    bool Initialize(ccEraser::IAnomaly* pAnomaly,       // Anomaly to retrieve 
                                                        // statistics for
                    LPCTSTR pcszUnknownText = NULL,     // Text to use for 
                                                        // unknown remediation 
                                                        // action types
                    LPCTSTR pcszDefaultRegKeyText = NULL);  // Text to use for 
                                                            // default registry
                                                            // key value names

    // Resets all of the data stored
    bool Clear();

    // Retrieve the count for the type passed in
    DWORD GetRemediationCount(AffectedTypes type);

    // The remediation description and operation/status
    typedef std::pair<ATL::CAtlString, ATL::CAtlString> REMEDIATIONPAIR;

    typedef std::vector<REMEDIATIONPAIR> REMEDIATIONSTATSVEC;

    // Retrieve the list of items for the type passed in
    REMEDIATIONSTATSVEC GetItems(AffectedTypes type);

private:
    REMEDIATIONSTATSVEC m_vFile, m_vProc, m_vReg,
        m_vCOM, m_vService, m_vBatch,
        m_vINI, m_vBR, m_vMBR, m_vUnknown,
        m_vHosts, m_vLSP;

    bool m_bInitialized;
};

class CEZAnomaly
{
public:
    CEZAnomaly(ccEraser::IAnomaly* pAnomaly);
    CEZAnomaly();
    ~CEZAnomaly(void);

    // Retrieves all of the data for an anomaly
    bool Initialize(ccEraser::IAnomaly* pAnomaly);

    // Retrieves all of the data for the anomaly currently stored
    // in the member variable m_spAnomaly
    bool Initialize();

    // Clears out all of the internal data stored about the anomaly
    void Reset();

    // Boot record, 9x memory infections and infections that ccEraser marks
    // as repair-only (i.e. don't try or offer to the user quarantine nor 
    // delete as back-up plan if repair fails) require special handling. 
    // These methods will mark and anomaly as one of these types and allow 
    // us to  check for the type later
    bool SetMemBootType(ScanInfectionTypes type);
    bool SetRepairOnly();
    bool IsBootRecordAnomaly();
    bool IsMBRAnomaly();
    bool Is9xMemAnomaly();
    bool IsRepairOnlyAnomaly();

    // Methods to determine the type of anomaly
    bool IsGeneric();
    bool IsSpecific();
    bool IsViral();
    bool IsNonViral();
    bool CantDelete();

    // Returns true if there is a memory remediation that is currently present
    // also sets a property on each memory item to explicitly mark it as present
    // or not so we only have to check for the presence one time to improve performance
    bool ProfileMemoryItems();

    // true if there is a remediation with reboot as the result of the Remediate() call
    bool RequiresReboot();
    bool SetRequiresReboot();

    // Retrieve damage flag values from the anomaly
    DWORD GetDamageFlagValue(DWORD dwFlag);
    DWORD GetDamageFlagAverage();
    
    // Retrieve dependency flag from the anomaly
    bool KnownToHaveDependencies();

    // Get and Set the single infection anomaly flag
    bool IsSingleInfectionAnomaly();
    bool SetSingleInfectionAnomaly(bool bVal);

    // Get and Set the action to take and status of the anomaly
    void SetAnomalyStatus(AnomalyAction eNextAction, CommonUIInfectionStatus eStatus);
    bool GetAnomalyStatus(AnomalyAction& eNextAction, CommonUIInfectionStatus& eStatus);

    // Retrieve the recommended action for this anomaly given its
    // current status
    bool GetRecommendedAction(AnomalyAction& action);

    // The threat name
    bool GetName(cc::IString*& pStrName);

    // The anomaly ID
    bool GetID(cc::IString*& pStrID);

    // The VID
    DWORD GetVID();

    // Retrieves the general description of all the affected areas from the user data
    // this is just a string of the number of affected areas for this anomaly
    bool GetGenericDescription(cc::IString*& pSymStrDesc);

    // Retrieve the anomaly structures we save during initialization
    cc::IKeyValueCollection* GetUserData();
    cc::IKeyValueCollection* GetProperties();
    cc::IIndexValueCollection* GetCategories();
    ccEraser::IRemediationActionList* GetRemediations();

    // Retrieve the remediation data
    size_t GetRemediationCount();
    ccEraser::IRemediationAction* GetRemediation(size_t nItem);

    // Initializes and returns a remediation status class to get all of the
    // information about remediation needed to display. Since this class doesn't have any resources
    // of it's own clients can pass in the text from there resources to use for unknown items and
    // default registry key text, if none are passed in it will use hard-coded "Unknown" and "(Default)" strings
    CRemediationStats* GetRemediationStatistics(LPCTSTR pcszUnknownText = NULL, LPCTSTR pcszDefaultRegKeyText = NULL);

    // Retrieve or Set the associated quarantine item identifier
    bool SetQuarantineItemUUID(UUID uuid);
    bool GetQuarantineItemUUID(UUID &uuid);

    // Operator overrides so this class can be used interchangably where
    // a ccEraser::IAnomaly* or ccEraser::IAnomalyPtr is used
    ccEraser::IAnomaly* operator =(int nNull) 
    {
        assert(nNull == 0);
        UNREFERENCED_PARAMETER(nNull);
        Reset();
        return m_spAnomaly.m_p;
    }
    ccEraser::IAnomaly* operator =(const ccEraser::IAnomalyPtr& p)
    {
        if (p == m_spAnomaly)
        {
            return m_spAnomaly.m_p;
        }
        return operator =(p.m_p);
    }
    ccEraser::IAnomaly* operator =(const ccEraser::IAnomaly* p)
    { 
        if (p == m_spAnomaly.m_p)
        {
            return m_spAnomaly.m_p;
        }

        // Clear out the old anomaly
        Reset();

        // Assign the new one
        m_spAnomaly = p;

        // Initialize data with the new one
        Initialize();

        return m_spAnomaly.m_p;
    }
    operator ccEraser::IAnomaly*() const
    {
        return m_spAnomaly.m_p; 
    }
    ccEraser::IAnomaly& operator *() const
    {
        return *m_spAnomaly;
    }
    ccEraser::IAnomaly* operator ->() const
    { 
        return m_spAnomaly.m_p;
    }
    bool operator !() const throw()
    {
        return (m_spAnomaly.m_p == NULL);
    }
    bool operator ==(ccEraser::IAnomaly* p) const
    {
        return m_spAnomaly.m_p == p;
    }

private:
    ccEraser::IAnomalyPtr m_spAnomaly;
    cc::IKeyValueCollectionPtr m_spAnomalyProps;
    cc::IKeyValueCollectionPtr m_spUserData;
    ccEraser::IRemediationActionListPtr m_spRemediationList;
    cc::IIndexValueCollectionPtr m_spCategories;
    CRemediationStats m_RemediationStatistics;

    // Creates the user data IKeyValueCollection for this anomaly if it
    // is not already there
    bool CreateUserData();

    // Default damage flag values, used if the damage flags are not set in the
    // defs
    DWORD GetDefaultDamageFlagValue();

    bool m_bInitialized;
    bool m_bRepairOnly;
};

class CEZRemediation
{
public:
    CEZRemediation(ccEraser::IRemediationAction*);
    ~CEZRemediation();

    ccEraser::eObjectType GetType();
    cc::IKeyValueCollection* GetProperties();

    // This will get the existing user data or create them if none exist
    cc::IKeyValueCollection* GetUserData();

    bool IsScanInfectionType();
    ScanInfectionTypes GetScanInfectionType();
    bool IsProcessType();
    bool GetProcessRemediationID(DWORD& dwProcID);
    IScanInfection* GetScanInfection();
    bool GetPathProperty(cc::IString*& pStrPath);
    bool GetDisplayString(cc::IString*& pStrDisp, 
                          LPCTSTR pcszDefaultRegKeyName = NULL);
    ccEraser::IRemediationAction::Action_State GetState();
    bool SetState(ccEraser::IRemediationAction::Action_State);
    bool GetRemediationResult(ccEraser::eResult& eRemResult);
    bool SetRemediationResult(ccEraser::eResult eRemResult);
    bool SetThreatOperation(ccEraser::IRemediationAction::Handle_Threat_Ops 
                                                                    operation); 
    bool GetRecommendedOperation(DWORD &dwOperation);
    ccEraser::IRemediationAction::Handle_Threat_Ops GetThreatOperation();
    ccEraser::IRemediationAction::Operation_Type GetOperationType();
    bool SetOperationType(ccEraser::IRemediationAction::Operation_Type);
    cc::IIndexValueCollection* GetReferencedFilePaths();

    bool SetStatusText(LPCTSTR pstrStatus);
    bool GetStatusText(cc::IString*& pstrStatus);

    bool ShouldSaveRemediation();

    bool SetManualRebootRequired();
    bool ManualRebootRequired();

    bool SetQuarantineItemUUID(UUID uuid);
    bool GetQuarantineItemUUID(UUID &uuid);

    bool IsRemediationPresent();
    bool SetRemediationPresent(bool bPresent);

    bool IsActiveInMemory();
    bool SetActiveInMemory(bool bActive);

    bool SetScanInfection(IScanInfection* pInfection);

    operator ccEraser::IRemediationAction*() const
    {
        return m_spRemediation.m_p;
    }
    ccEraser::IRemediationAction* operator ->() const
    { 
        return m_spRemediation.m_p; 
    }

private:
    ccEraser::IRemediationActionPtr m_spRemediation;
    cc::IKeyValueCollectionPtr m_spProps;
    cc::IKeyValueCollectionPtr m_spUserData;
    ccEraser::eObjectType m_Type;

    bool m_bInitialized;
};
