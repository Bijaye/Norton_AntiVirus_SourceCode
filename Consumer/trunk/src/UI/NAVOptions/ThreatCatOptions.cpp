////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* AVComponent Threat Category Exclusions Options Object                */
/************************************************************************/

#include "stdafx.h"
#include "ThreatCatOptions.h"
#include "NAVSettingsCache.h"

#define _MIN_NV_CATEGORY 4
#define _MAX_NV_CATEGORY 11

CThreatCatOptions::CThreatCatOptions():m_bInitialized(false), m_bDirty(false)
{
    m_CatMap.clear();
}

CThreatCatOptions::~CThreatCatOptions()
{
    if(m_spTCExclu != NULL )
        m_spTCExclu.Release();

    m_CatMap.clear();
}

HRESULT CThreatCatOptions::Initialize()
{
    SYMRESULT sr;
    bool bExcluded;

    if(m_bInitialized)
        return S_FALSE;

    if(!m_spTCExclu)
    {
        if(SYM_FAILED(sr = m_avTCExcluLoader.CreateObject(m_spTCExclu)))
        {
            CCTRCTXE1(_T("Error creating AVModule ThreatCat exclusions object. SR = 0x%08x"), sr);
            return E_FAIL;
        }

        for(unsigned long l = _MIN_NV_CATEGORY; l <= _MAX_NV_CATEGORY; l++)
        {
            m_spTCExclu->IsCategoryExcluded(l,bExcluded);
            m_CatMap[l] = bExcluded;
        }
    }

    m_bDirty = false;
    m_bInitialized = true;
    return S_OK;
}

HRESULT CThreatCatOptions::Save()
{
    HRESULT hr = S_OK;
    AVExclusions::ExResult er;

    if(!m_bInitialized || !m_bDirty)
        return S_FALSE;

	for(unsigned long l = _MIN_NV_CATEGORY; l <= _MAX_NV_CATEGORY; l++)
	{
        // Exclude or include category as necessary
        if(m_CatMap[l])
            er = m_spTCExclu->ExcludeCategory(l);
        else
            er = m_spTCExclu->IncludeCategory(l);

		if(er >= AVExclusions::Fail)
		{
			hr = E_FAIL;
			break;
		}
	}

    m_bDirty = false;
    return hr;
}

HRESULT CThreatCatOptions::Notify() { return S_FALSE; }

HRESULT CThreatCatOptions::Default()
{
    HRESULT hr;
	DWORD dwVal;
	CStringW optDefName;
	CNAVOptSettingsCache navOptDefs;

	if(FAILED(hr = Initialize()))
		return hr;

	m_bDirty = true;

	if(!navOptDefs.Init(_T("NAVOPTS.DEF"), FALSE))	// Connect to defaults
	{
		CCTRCTXE0(_T("Error connecting to ccSettings hive to retrieve defaults. Defaulting all to on."));
		// All threat categories default to on
		for(unsigned long l = _MIN_NV_CATEGORY; l <= _MAX_NV_CATEGORY; l++)
		{
			if(m_CatMap[l])
			{
				m_bDirty = true;
				m_CatMap[l] = false;
			}
		}
	}
	else
	{
		// Pull defaults from ccSettings (navopts.def key).
		for(unsigned long l = _MIN_NV_CATEGORY; l <= _MAX_NV_CATEGORY; l++)
		{
			optDefName = _T("THREAT:ThreatCat%d");
			optDefName.Format(optDefName, l);

			if(m_CatMap[l])
			{
				if(FAILED(hr = navOptDefs.GetValue(optDefName, dwVal, 1)))
				{
					CCTRCTXW2(_T("Error getting default value for Threat Category %d, hr = 0x%08x"), l, hr);
				}
				m_CatMap[l] = dwVal ? false:true;	// Flip. THREAT:ThreatCatN 1 = Not Excluded.
			}
		}
	}

    return this->Save();
}

// ThreatCatExcluded == true -> Option is OFF
HRESULT CThreatCatOptions::GetThreatCatExcluded(const unsigned long &ulCategory, bool& bExcluded)
{
    HRESULT hr;
    if(FAILED(hr = Initialize()))
        return hr;

    bExcluded = !m_CatMap[ulCategory];
    return S_OK;
}

HRESULT CThreatCatOptions::DefaultThreatCatExcluded(const unsigned long &ulCategory, bool &bExcluded)
{
    HRESULT hr;
	CNAVOptSettingsCache navOptDefs;
	DWORD dwVal;

    if(FAILED(hr = Initialize()))
        return hr;

	if(!navOptDefs.Init(_T("NAVOPTS.DEF"), FALSE))
	{
		dwVal = 1;
	}
	else
	{
		CStringW csw(_T("THREAT:Threat%d"));
		csw.Format(csw,ulCategory);

		if(FAILED(hr = navOptDefs.GetValue(csw, dwVal, 1)))
		{
			CCTRCTXW2(_T("Error getting default value for %s, hr = 0x%08x"), csw, hr);
		}
	}

	if(m_CatMap[ulCategory] != (dwVal ? false:true))
    {
        m_bDirty = true;
		bExcluded = m_CatMap[ulCategory] = dwVal ? false:true;	// Flip since THREAT:ThreatCatN 1 = not excluded
    }

    return S_OK;
}

// ThreatCatExcluded == true -> Option is OFF
HRESULT CThreatCatOptions::SetThreatCatExcluded(const unsigned long &ulCategory, const bool bExcluded)
{
    HRESULT hr;
    if(FAILED(hr = Initialize()))
        return hr;

	// bExcluded maps to !m_CatMap[ulCategory]; ergo, equality is inequality.
    if(m_CatMap[ulCategory] == bExcluded)
    {
        m_bDirty = true;
        m_CatMap[ulCategory] = !bExcluded;
    }

    return S_OK;
}
