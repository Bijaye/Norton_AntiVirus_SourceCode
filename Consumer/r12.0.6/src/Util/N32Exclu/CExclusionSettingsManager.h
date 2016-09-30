// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////

/***
 * Interface header file for the Settings manager wrapper class.
 * Has helper functions for serializing and deserializing exclusions.
 ***/

#ifndef __N32EXCLU_cexclusionsettingsmanager_h__
#define __N32EXCLU_cexclusionsettingsmanager_h__

#include "n32exclu.h"
#include "ExclusionInterface.h"
#include "CAnomalyExclusion.h"
#include "CFileSystemExclusion.h"
#include "ccSerializeInterface.h"
#include "ccSettingsInterface.h"
#include "ccSettingsManagerHelper.h"

namespace NavExclusions
{

class CExclusionSettingsManager: public ccSettings::CSettingsManagerHelper
{
public:
    CExclusionSettingsManager();
    virtual ~CExclusionSettingsManager();
    ExResult initialize();
    ExResult uninitialize();
    ExResult saveExclusion(IExclusion* pExclusion);

    ExResult enumLoadExclusionData(cc::IKeyValueCollection*& pExclusion);
    ExResult enumLoadReset();

    bool isInitialized();
    bool enumUnload();

private:
    ccSettings::ISettingsPtr m_pSettings;
    ccSettings::ISettingsManagerPtr m_pSettingsManager;
    ccSettings::IEnumValuesPtr m_pEnumValues;
    DWORD m_dwEnumCurrent;

    ExResult SetValue(LPCWSTR lpszName, LPBYTE pBuf, DWORD dwSize);

    bool Load(LPCWSTR lpszKey);
    bool Commit(LPCWSTR lpszKey);
    bool SettingsReady();
    bool IsSettingsManagerActive(bool bSpinLock);

    bool m_bInitialized;

    ExResult deleteExclusion(IExclusion* pExclusion);
    const static DWORD m_dwLoadWaitMax;
    const static DWORD m_dwLoadWaitInterval;

};

}
#endif