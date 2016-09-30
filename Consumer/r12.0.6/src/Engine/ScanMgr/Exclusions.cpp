
#include "Stdafx.h"
#include "Exclusions.h"
#include "DisplayNames.h"
#include "ccSymModuleLifetimeMgrHelper.h"
#include "ccEraserInterface.h"
// For exclude status defines
#include "commonuiinterface.h"
#include "ezeraserobjects.h"

using namespace std;
using namespace NavExclusions;

CExclusions::CExclusions():
    m_bReady(false), m_bDirty(false)
{}

CExclusions::~CExclusions()
{
	if (m_bReady == true)
		this->Uninitialize();
}

bool CExclusions::Initialize()
{
	if (m_bReady == true)
		return true;

    CCTRACEI(_T("CExclusions::Initialize() - Loading N32Exclu DLL."));

    SYMRESULT symRes = m_ExclusionManagerLoader.Initialize();
    if(!SYM_SUCCEEDED(symRes))
    {
        CCTRACEE(_T("CExclusions::Initialize() - Failed to load Exclusion Manager "\
                    "loader. SYMRESULT = 0x%X"), symRes);
        return false;
    }

    CCTRACEI(_T("CExclusions::Initialize() - DLL loaded, creating mgr."));
    
    if(!SYM_SUCCEEDED(symRes = m_ExclusionManagerLoader.CreateObject(m_spExclusions)))
    {
        CCTRACEE(_T("CExclusions::Initialize() - Failed to create Exclusion Manager. SYMRESULT = 0x%X"), symRes);
        return false;
    }

    CCTRACEI(_T("CExclusions::Initialize() - Object probably created."));

    if(!m_spExclusions)
    {
        CCTRACEE(_T("CExclusions::Initialize() - Exclusion Manager is invalid."));
        return false;
    }

    CCTRACEI(_T("CExclusions::Initialize() - Object creation confirmed; initializing."));

    ExResult exRes = m_spExclusions->initialize(false);
    if ( exRes != NavExclusions::Success)
    {
        CCTRACEE(_T("CExclusions::Initialize() - Failed to initialize exclusions. ExResult = %d"), exRes);
        this->m_spExclusions.Release();
		return false;
    }

    CCTRACEI(_T("CExclusions::Initialize() - Initialization OK! Done."));

    m_bDirty = false;
	return (m_bReady = true);
}

void CExclusions::Uninitialize()
{
    if(!m_bReady)
    {
        CCTRACEE(_T("CExclusions::Uninitialize - Called while uninitialized. "\
            "Automatically returning false."));
        return;
    }

    if(!m_spExclusions)
    {
        CCTRACEE(_T("CExclusions::Uninitialize() - Exclusion Manager is invalid."));
        return;
    }

    ExResult res = NavExclusions::Success;
    if(m_bDirty)
    {
        res = m_spExclusions->saveState();
        if( res != NavExclusions::Success)
        {
            CCTRACEE(_T("CExclusions::Uninitialize - Could not save "\
                        "exclusion manager state! Possible loss of data. ExResult = %d"), res);
        }
        m_bDirty = false;
    }

    res = m_spExclusions->uninitialize(false);
    if( res != NavExclusions::Success)
    {
        CCTRACEW(_T("CExclusions::Uninitialize - Error encountered "\
                    "while uninitializing exclusion manager. ExResult = %d"), res);
    }

    m_spExclusions.Release();

    m_bReady = false;
}

NavExclusions::IExclusion::ExclusionState CExclusions::IsPathMatch(
                                                      const char* pszPath,
                                                      bool bIsOem,
                                                      IExclusion::ExclusionState requiredState)
{
    if(!pszPath)  // Silly user.
    {
        CCTRACEE(_T("CExclusions::IsDirMatch - Invalid path paramater. "\
            "Automatically returning NOT_EXCLUDED."));
        return NavExclusions::IExclusion::ExclusionState::NOT_EXCLUDED;
    }

    if(!m_bReady)
    {
        CCTRACEE(_T("CExclusions::IsDirMatch - Called while uninitialized. "\
                    "Automatically returning NOT_EXCLUDED."));
        return NavExclusions::IExclusion::ExclusionState::NOT_EXCLUDED;
    }

    if(!m_spExclusions)
    {
        CCTRACEE(_T("CExclusions::IsDirMatch - Called while exclusion manager "\
                    "is null - returning NOT_EXCLUDED."));
        return NavExclusions::IExclusion::ExclusionState::NOT_EXCLUDED;
    }

    return m_spExclusions->isExcludedFS(pszPath, bIsOem, requiredState);
}

bool CExclusions::IsMatch(IScanInfection* pInfection)
{
    ULONG ulVid = 0;
    bool bViral;
    IExclusion::ExclusionState eState;
    IExclusion::ExclusionState requiredState;

    if(!pInfection)  // Silly user.
    {
        CCTRACEE(_T("CExclusions::IsMatch - Invalid infection paramater. "\
            "Automatically returning false."));
        return false;
    }

    if(!m_bReady)
    {
        CCTRACEE(_T("CExclusions::IsMatch - Called while uninitialized. "\
            "Automatically returning false."));
        return false;
    }

    if(!m_spExclusions)
    {
        CCTRACEE(_T("CExclusions::IsMatch - Called while exclusion manager "\
            "is null - returning false."));
        return false;
    }

    ulVid = pInfection->GetVirusID();
    bViral = (pInfection->IsCategoryType(SCAN_THREAT_TYPE_VIRUS) ||
        pInfection->IsCategoryType(SCAN_THREAT_TYPE_MALICIOUS) ||
        pInfection->IsCategoryType(SCAN_THREAT_TYPE_RESERVED_MALICIOUS) ||
        pInfection->IsCategoryType(SCAN_THREAT_TYPE_HEURISTIC) );
    requiredState = (bViral? IExclusion::ExclusionState::EXCLUDE_VIRAL :
                             IExclusion::ExclusionState::EXCLUDE_NONVIRAL);

    eState = m_spExclusions->isExcludedAnomaly(ulVid, NULL, requiredState);

    if(eState != IExclusion::ExclusionState::NOT_EXCLUDED)
    {   // We got a proper hit. Escape.
        CCTRACEE(_T("CExclusions::IsMatch - Matched exclusion VID %lu for scan infection "), ulVid);
        return true;
    }

    // Match path
    if(HandleISI(pInfection, requiredState, eState))
    {
        if((bViral && (eState & IExclusion::ExclusionState::EXCLUDE_VIRAL)) ||
            (!bViral && (eState & IExclusion::ExclusionState::EXCLUDE_NONVIRAL)))
        {   // We got a proper hit. Escape.
            CCTRACEE(_T("CExclusions::IsMatch - Matched exclusion path for scan infection"));
            return true;
        }
    }
    return false;
}
bool CExclusions::IsMatch(ccEraser::IAnomaly* pAnomaly)
{
    if(!pAnomaly)  // Silly user.
    {
        CCTRACEE(_T("CExclusions::IsMatch - Anomaly paramater invalid. "\
            "Automatically returning false."));
        return false;
    }

    if(!m_bReady)
    {
        CCTRACEE(_T("CExclusions::IsMatch - Called while uninitialized. "\
            "Automatically returning false."));
        return false;
    }

    if(!m_spExclusions)
    {
        CCTRACEE(_T("CExclusions::IsMatch - Called while exclusion manager "\
            "is null - returning false."));
        return false;
    }

    CEZAnomaly ezAnom;
    if( !ezAnom.Initialize(pAnomaly) )
    {
        CCTRACEE(_T("CExclusions::IsMatch - Could not initalize anomaly helper class. "\
            "Returning false."));
        return false;
    }

    cc::IStringPtr spStr = NULL;
    ULONG ulVid = 0;
    IExclusion::ExclusionState eState, requiredState;

    ulVid = ezAnom.GetVID();
    if( ulVid == 0 )
    {
        CCTRACEW(_T("CExclusions::IsMatch - Could not retrieve "\
                    "VID from Anomaly."));
    }

    // We only want to check the GUID if this is a specific anomaly
    // otherwise this was generated and the Anomaly ID is a random GUID
    if( ezAnom.IsSpecific() )
    {
        if( !ezAnom.GetID(spStr) )
        {
            CCTRACEE(_T("CExclusions::IsMatch - Could not retrieve ID from "\
                "specific anomaly."));
        }
    }
    else
    {
        CCTRACEI(_T("CExclusions::IsMatch - This anomaly is not specific, not checking the Anomaly ID."));
    }

    if(ulVid == 0 && !spStr)
    {
        CCTRACEE(_T("CExclusions::IsMatch - Could not retrieve VID or GUID "\
                    "from anomaly; no criteria to match on. Returning false."));
        return false;
    }

    requiredState = (ezAnom.IsViral() ? IExclusion::ExclusionState::EXCLUDE_VIRAL :
                                        IExclusion::ExclusionState::EXCLUDE_NONVIRAL);

    eState = m_spExclusions->isExcludedAnomaly(ulVid, spStr, requiredState);

    if(eState != IExclusion::ExclusionState::NOT_EXCLUDED)
    {   // We got a proper hit. Escape.
        CCTRACEI(_T("CExclusions::IsMatch - Matched an anomaly exclusion for anomaly with VID = %lu"), ulVid);
        return true;
    }

    if(HandleAnomalyFS(pAnomaly, requiredState))
    {
        // All paths matched.
        CCTRACEI(_T("CExclusions::IsMatch - Matched a path exclusion for anomaly with VID = %lu"), ulVid);
        return true;
    }
    else
    {
        // Something bad happened or a path didn't match
        return false;
    }
}

bool CExclusions::AddAnomaly(ccEraser::IAnomaly* pAnomaly)
{
    if(!pAnomaly)  // Silly user.
    {
        CCTRACEE(_T("CExclusions::AddAnomaly - Anomaly paramater invalid. "\
            "Automatically returning false."));
        return false;
    }

    if(!m_bReady)
    {
        CCTRACEE(_T("CExclusions::AddAnomaly - Called while uninitialized. "\
            "Automatically returning false."));
        return false;
    }

    if(!m_spExclusions)
    {
        CCTRACEE(_T("CExclusions::AddAnomaly - Called while exclusion manager "\
            "is null - returning false."));
        return false;
    }

    CEZAnomaly ezAnom;
    if( !ezAnom.Initialize(pAnomaly) )
    {
        CCTRACEE(_T("CExclusions::AddAnomaly - Could not initalize anomaly helper class. "\
            "Returning false."));
        return false;
    }

    cc::IStringPtr spStr = NULL;
    cc::IStringPtr spThreatStr = NULL;
    ULONG ulVid = 0;
    bool bViral = false;

    ulVid = ezAnom.GetVID();
    if( ulVid == 0 )
    {
        CCTRACEW(_T("CExclusions::AddAnomaly - Could not retrieve "\
            "VID from Anomaly."));
    }

    // We only want to add the GUID if this is a specific anomaly
    // otherwise this was generated and the Anomaly ID is a random GUID
    if( ezAnom.IsSpecific() )
    {
        if( !ezAnom.GetID(spStr) )
        {
            CCTRACEE(_T("CExclusions::AddAnomaly - Could not retrieve ID from "\
                "specific anomaly."));
        }
    }
    else
    {
        CCTRACEI(_T("CExclusions::AddAnomaly - This anomaly is not specific, not checking the Anomaly ID."));
    }

    if( !ezAnom.GetName(spThreatStr) || spThreatStr == NULL )
    {
        CCTRACEE(_T("CExclusions::AddAnomaly - Could not retrieve threat name "\
            "from anomaly; Returning false."));
        return false;
    }

    if(ulVid == 0 && !spStr)
    {
        CCTRACEE(_T("CExclusions::AddAnomaly - Could not retrieve VID or GUID "\
            "from anomaly %s; no criteria to add with. Returning false."), spThreatStr->GetStringA());
        return false;
    }

    // We only allow non-viral exclusion adds, don't add viral items
    if( ezAnom.IsViral() && !ezAnom.IsNonViral() )
    {
        CCTRACEE(_T("CExclusions::AddAnomaly - Not allowing a viral anomaly exclusion for %s"), spThreatStr->GetStringA());
        return false;
    }

    if(m_spExclusions->addExclusionAnomaly(ulVid, spStr, 0, spThreatStr, 
        (IExclusion::ExclusionState::EXCLUDE_NONVIRAL) )
        != _INVALID_ID)
    {
        m_bDirty = true;
        return true;
    }
    else
        return false;
}

bool CExclusions::HandleISI(IScanInfection* pInfection,
                            IExclusion::ExclusionState requiredState,
                            IExclusion::ExclusionState &eState)
{
    if (pInfection == NULL)
    {
        CCTRACEE(_T("CExclusions::ExcludeISI() - Infection parameter is null"));
        return false;
    }

    // If the exclusions could not be loaded then default to no exclusions
    if( !m_bReady )
    {
        CCTRACEE(_T("CExclusions::ExcludeNonViralThreat() - Exclusions are not initialized"));
        return false;
    }

    if(!m_spExclusions)
    {
        CCTRACEE(_T("CExclusions::ExcludeNonViralThreat() - Exclusion Manager is null"));
        return false;
    }

    try
    {
        ATL::CAtlString strPath;
        bool bOem = false;

        CSymPtr<IScanFileInfection2> pFile;
        CSymPtr<IScanCompressedFileInfection2> pCompressedFile;

        if( SYM_SUCCEEDED(pInfection->QueryInterface(IID_ScanFileInfection2, (void**)&pFile)) )
        {
            // Is the path OEM?
            bOem = pFile->AreOEMFileApis();

            // Get the path
            strPath = pFile->GetLongFileName();
        }
        else if( SYM_SUCCEEDED(pInfection->QueryInterface(IID_ScanCompressedFileInfection2, (void**)&pCompressedFile)) )
        {
            // Is the path OEM?
            bOem = pCompressedFile->AreOEMFileApis();

            // Get the container path
            string strCompressedFile;

            strPath = pCompressedFile->GetComponent(pCompressedFile->GetComponentCount()-1);
        }

        // Don't check empty paths
        if( strPath.IsEmpty() )
        {
            CCTRACEE(_T("CExclusions::HandleISI() - Scan infection path is empty."));
            return false;
        }

        // Deny Network path exclusions
        if( IsNetworkDrive(strPath) )
        {
            CCTRACEW(_T("CExclusions::HandleISI() - Ignoring UNC path: %s"), (LPCTSTR)strPath);
            return false;
        }

        eState = m_spExclusions->isExcludedFS(strPath, bOem, requiredState);

        return true;
    }
    catch(exception& Ex)
    {
        CCTRACEE(_T("Caught Exception in CExclusions::ExcludeNonViralThreat: "),Ex.what());
    }

    return false;
}

bool CExclusions::HandleAnomalyFS(ccEraser::IAnomaly* pAnomaly, 
                                  IExclusion::ExclusionState requiredState)
{
    if (pAnomaly == NULL)
    {
        CCTRACEE(_T("CExclusions::HandleAnomalyFS() - Anomaly parameter is null"));
        return false;
    }

    // If the exclusions could not be loaded then default to no exclusions
    if( !m_bReady )
    {
        CCTRACEE(_T("CExclusions::HandleAnomalyFS() - Exclusions are not initialized"));
        return false;
    }

    if(!m_spExclusions)
    {
        CCTRACEE(_T("CExclusions::HandleAnomalyFS() - Exclusion manager is null"));
        return false;
    }

    bool bRet = false;

    // Get the remediation list
    ccEraser::eResult eRes = ccEraser::Success;
    ccEraser::IRemediationActionListPtr pRemediateList;
    if( ccEraser::Succeeded(eRes = pAnomaly->GetRemediationActions(pRemediateList) ) )
    {
        size_t nCount = 0;
        if( ccEraser::Succeeded (eRes = pRemediateList->GetCount(nCount)) )
        {
            // Now we loop through each remediation action looking for scan infection items
            for(size_t nCur = 0; nCur < nCount; nCur++)
            {
                ccEraser::IRemediationActionPtr pAction;
                if( ccEraser::Succeeded (eRes = pRemediateList->GetItem(nCur, pAction)) )
                {
                    CEZRemediation ezRem(pAction);

                    // Is this a scan infection type?
                    if( ezRem.IsScanInfectionType() )
                    {
                        // Get the scan infection item and check if it is excluded
                        IScanInfectionPtr spScanInfection = ezRem.GetScanInfection();

                        if( !spScanInfection )
                        {
                            CCTRACEE(_T("CExclusions::HandleAnomalyFS() - Failed to get the scan infection object."));
                            continue;
                        }

                        IExclusion::ExclusionState eState;
                        if(HandleISI(spScanInfection, requiredState, eState))
                        {
                            if(eState != IExclusion::ExclusionState::NOT_EXCLUDED)
                            {
                                // We found at least one scan infection item where the path matches
                                // something that is in the exclusions list
                                bRet = true;
                                continue;
                            }
                            else
                                return false;
                        }
                        else
                        {
                            CCTRACEE(_T("CExclusions::HandleAnomalyFS - Failed "\
                                        "to properly handle IScanInfection."));
                            continue;
                        }
                    }
                }
                else
                {
                    CCTRACEE(_T("CExclusions::HandleAnomalyFS() - Error getting remediation action %d. eResult = %d. Skipping this remediation action."), nCur, eRes);
                    continue;
                }
            }
        }
        else
        {
            CCTRACEE(_T("CExclusions::HandleAnomalyFS() - Error getting the remediation list count."));
            return false;
        }
    }
    else
    {
        CCTRACEE(_T("CExclusions::HandleAnomalyFS() - Error getting the remediation actions list object."));
        return false;
    }

    return bRet;
}

bool CExclusions::IsNetworkDrive(const char* szItem)
{
    // Get the drive root.

    char szDrive[_MAX_PATH];
    char szDir[_MAX_DIR];
    char szFileName[_MAX_FNAME];
    char szExt[_MAX_EXT];

    _splitpath(szItem, szDrive, szDir, szFileName, szExt);

    if (PathIsUNC(szItem))
        return true;

    // Do not add files on network drives to our scan list.

    if(GetDriveType(szDrive) != DRIVE_REMOTE)
        return false;

    return true;  
}