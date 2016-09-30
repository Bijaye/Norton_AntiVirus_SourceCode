#include "stdafx.h"

#include "CAPThreatExclusions.h"

// For threat cats
#include "NAVSettingsHelperEx.h"
#include "ccScanInterface.h"
#include "optnames.h"

// ccSym::CStringImpl
#include "ccSymStringImpl.h"

using namespace NavExclusions;

CAPThreatExclusions::CAPThreatExclusions():
    m_bInitialized(false), m_bCatsLoaded(false)
{}

CAPThreatExclusions::~CAPThreatExclusions()
{
    if(this->m_bInitialized)
        this->Uninitialize();
}

bool CAPThreatExclusions::Initialize()
{
    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);

    if(this->m_bInitialized)
        return true;

    if (!this->LoadThreatCats())
    {
        CCTRACEE(_T("CAPThreatExclusions::Initialize - Couldn't load "\
                    "threat categories."));
        return false;
    }

    if(!this->m_spExclusionMgr)
    {
        if(!SYM_SUCCEEDED(this->m_MgrLoader.CreateObject(this->m_spExclusionMgr))
                || !this->m_spExclusionMgr)
        {
            CCTRACEE(_T("CAPThreatExclusions::Initialize - Unable to create "\
                        "exclusion manager object."));
            return false;
        }
    }

    if(this->m_spExclusionMgr->initialize(false) != NavExclusions::Success)
    {
        CCTRACEE(_T("CAPThreatExclusions::Initialize - Unable to "\
                    "initialize Exclusion Manager."));
        return false;
    }

    this->m_bInitialized = true;
    return true;
}

bool CAPThreatExclusions::Uninitialize()
{
    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);

    if(!this->m_bInitialized)
        return true;

    if(!this->UnloadThreatCats())
    {
        CCTRACEW(_T("CAPThreatExclusions::Uninitialize - Error while "\
                    "unloading threat categories."));
    }

    if(this->m_spExclusionMgr->uninitialize(false) != NavExclusions::Success)
    {
        // report a warning
        CCTRACEW(_T("CAPThreatExclusions::Uninitialize - "\
                    "IExclusionManager::Uninitialize returned an error!"));
    }

    this->m_spExclusionMgr.Release();
    this->m_bInitialized = false;
    return true;
}

bool CAPThreatExclusions::isInitialized() { return this->m_bInitialized; }

bool CAPThreatExclusions::Reload()
{
    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);

    if(!this->UnloadThreatCats())
    {
        CCTRACEE(_T("CAPThreatExclusions::Reload - Error while unloading "\
                    "threat categories."));
        return false;
    }
    else if(!this->LoadThreatCats())
    {
        CCTRACEE(_T("CAPThreatExclusions::Reload - Error while loading "\
                    "threat categories."));
        return false;
    }

    if(this->m_spExclusionMgr->reloadState() != NavExclusions::Success)
    {
        CCTRACEE(_T("CAPThreatExclusions::Reload - Error while reloading "\
                    "Exclusions."));
        return false;
    }

    return true;
}

bool CAPThreatExclusions::IsExcluded(ULONG ulVid, LPCWSTR pszFullPath, 
                                     DWORD dwCatCount, LPDWORD pdwThreatCats)
{
    IExclusion::ExclusionState exState;

    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);

    if(!this->m_bInitialized)
        return false;

    if(dwCatCount > 0 && pdwThreatCats && 
        this->IsThreatCatExcluded(dwCatCount, pdwThreatCats))
    {   // True == all categories excluded, exclude this item
        return true;
    }

    exState = this->m_spExclusionMgr->isExcludedAnomaly(ulVid, 0, 
        IExclusion::ExclusionState::EXCLUDE_NONVIRAL);

    if(exState & IExclusion::ExclusionState::EXCLUDE_NONVIRAL)
    {   // True - excluded
        return true;
    }

    if(!pszFullPath)
    {
        /*CCTRACEI(_T("CAPThreatExclusions::IsExcluded - No path supplied; "\
                    "assuming not excluded."));*/
        return false;
    }

    cc::IStringPtr spFullPath = ccSym::CStringImpl::CreateStringImpl(pszFullPath);
    if(!spFullPath)
    {
        CCTRACEW(_T("CAPThreatExclusions::IsExcluded - Unable to create "\
                    "an IString from the widechar path."));
        return false;
    }

    // Look for any nonviral exclusions that match this path
    exState = this->m_spExclusionMgr->isExcludedFS(spFullPath, 
        IExclusion::ExclusionState::EXCLUDE_NONVIRAL);
    if(exState & IExclusion::ExclusionState::EXCLUDE_NONVIRAL)
    {   // True - excluded
        return true;
    }

    return false;
}

/** Protected/Internal Methods; no locking since these are internal **/

bool CAPThreatExclusions::LoadThreatCats()
{
    CNAVOptSettingsEx NavOpts;
    HRESULT status = S_OK;
    DWORD dwLastKnownThreatCategory, dwFirstNonViralThreatCategory, dwData = 0;

    if(this->m_bCatsLoaded)
        return true;

    if( !NavOpts.Init() )
    {
        CCTRACEE( _T("CAPThreatExclusions::Initialize - Could not load the "\
            "options. Using default values. Error Code: %d"), status ); 
        return false;
    }

    if (FAILED(NavOpts.GetValue(THREAT_LastKnownCategory, dwData, SCAN_THREAT_TYPE_JOKE))) // Joke is a default
        CCTRACEW(_T("CAPThreatExclusions::Initialize - Unable to read the "\
        "threat last known category option."));
    else
        dwLastKnownThreatCategory = dwData;

    if (FAILED(NavOpts.GetValue(THREAT_FirstKnownNonViral, dwData, SCAN_THREAT_TYPE_SECURITY_RISK)))
        CCTRACEW(_T("CAPThreatExclusions::Initialize - Unable to read the "\
        "threat first known category option."));
    else
        dwFirstNonViralThreatCategory = dwData;

    for( DWORD dwIndex = dwFirstNonViralThreatCategory; dwIndex <= dwLastKnownThreatCategory; dwIndex++ )
    {
        // Create the string for the current threat category option "Threat##"
        TCHAR szCurCatOpt[MAX_PATH] = _T("THREAT:Threat");
        TCHAR szCurCatNum[20] = {0};
        _tcscat(szCurCatOpt, _ultot(dwIndex, szCurCatNum, 10));

        if (FAILED(NavOpts.GetValue(szCurCatOpt, dwData, 0)))
            CCTRACEW(_T("CAPThreatExclusions::Initialize - Unable to read a "\
            "threat category enabled setting"));
        else
            m_ThreatCategoryEnabled[dwIndex] = (dwData ? true : false);
    }

    m_bCatsLoaded = true;
    return true;
}

bool CAPThreatExclusions::UnloadThreatCats()
{
    this->m_ThreatCategoryEnabled.clear();
    this->m_bCatsLoaded = false;
    return true;
}

bool CAPThreatExclusions::IsThreatCatExcluded(DWORD dwCatCount, LPDWORD pdwThreatCats)
{
    if ( !m_bCatsLoaded )
        return false;
 
    // Error, report the category
    if ( 0 == dwCatCount || 0 == pdwThreatCats[0] || 0 == m_ThreatCategoryEnabled.size())
        return false;

    // Check threat cat exclusions
    //
    for ( DWORD dwCatIndex = 0; dwCatIndex < dwCatCount; dwCatIndex++ )
    {
        if ( 1 == m_ThreatCategoryEnabled[pdwThreatCats[dwCatIndex]])
        {
            // Not excluded, report it
            return false;
        }
    }

    return true;
}
//#endif