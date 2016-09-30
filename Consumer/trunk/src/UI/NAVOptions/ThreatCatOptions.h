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

#ifndef __NAVOPTIONS_TCEOPTIONS_H__
#define __NAVOPTIONS_TCEOPTIONS_H__

#include "stdafx.h"
#include "ExclusionInterface.h"
#include "AVInterfaceLoader.h"
#include <map>

class CThreatCatOptions
{
public:
    CThreatCatOptions();
    ~CThreatCatOptions();

    // CORE OPTIONS INTERFACE BEGIN
    HRESULT Initialize();
    HRESULT Save();
    HRESULT Default();
    HRESULT Notify();
    bool IsDirty();
    // CORE OPTIONS INTERFACE END

    HRESULT DefaultThreatCatExcluded(const unsigned long &ulCategory, bool &bExcluded);
    HRESULT GetThreatCatExcluded(const unsigned long &ulCategory, bool& bExcluded);
    HRESULT SetThreatCatExcluded(const unsigned long &ulCategory, const bool bExcluded);

private:
    bool m_bInitialized;
    bool m_bDirty;

    typedef ::std::map<unsigned long, bool> CategoryMap;
    CategoryMap m_CatMap;
    AVExclusions::INonViralCategoryExclusionsPtr m_spTCExclu;
	AVModule::AVLoader_IAVNVCatExclusions m_avTCExcluLoader;

};

#endif