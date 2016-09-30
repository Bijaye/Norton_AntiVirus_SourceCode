#ifndef __AP_CAPThreatExclusions_h__
#define __AP_CAPThreatExclusions_h__

#include "n32exclu.h"
#include "ExclusionManagerInterface.h"
#include "ExclusionManagerLoader.h"
#include "ccCriticalSection.h"
#include <map>

class CAPThreatExclusions
{
public:
    CAPThreatExclusions();
    virtual ~CAPThreatExclusions();

    bool Initialize();
    bool Uninitialize();
    bool isInitialized();

    // Reloads the exclusion list
    bool Reload ();

    // Check for an exclusion
    bool IsExcluded(ULONG ulVid, LPCWSTR pszFullPath, DWORD dwCatCount, 
                    LPDWORD pdwThreatCats);

protected:
    // Checks threat cats for exclusions
    bool IsThreatCatExcluded(DWORD dwCatCount, LPDWORD pdwThreatCats);

    bool LoadThreatCats();
    bool UnloadThreatCats();

private:
    bool m_bInitialized;
    NavExclusions::N32Exclu_IExclusionManagerFactory m_MgrLoader;
    NavExclusions::IExclusionManagerPtr m_spExclusionMgr;

    // Threat Cats
    bool m_bCatsLoaded;
    std::map <DWORD, DWORD> m_ThreatCategoryEnabled;

    ccLib::CCriticalSection m_critAccess;
};

#endif