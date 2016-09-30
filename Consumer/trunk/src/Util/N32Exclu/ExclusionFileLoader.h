////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * CExclusionFileLoader
 *  Loads IKeyValueCollections from disk,
 * examines their properties via some filters,
 * and then pushes them into CC Settings if they
 * pass examination.
 */

#ifndef __N32EXCLU_exclusionfileloader_h__
#define __N32EXCLU_exclusionfileloader_h__

// Disable stupid inheritance warning
#pragma warning( disable : 4290 )

#include "n32exclu.h"
#include "ExclusionInterface.h"
#include "ccSettingsManagerHelper.h"
#include "ccSettingsInterface.h"
#include "ccIndexValueCollectionInterface.h"
#include "ccSymFileStreamImpl.h"

namespace NavExclusions
{

    class CExclusionFileLoader: public ccSettings::CSettingsManagerHelper
    {
    public:
        CExclusionFileLoader();
        virtual ~CExclusionFileLoader();
        ExResult initialize(LPCTSTR pszFileName);
        ExResult uninitialize();

        ExResult LoadNextItem(cc::IKeyValueCollection*& pExclusion);

        // Saves to CC Settings
        ExResult saveItem(cc::IKeyValueCollection *pCollection);

        bool isInitialized();

        // Both of these must _exactly match_
        ExResult LoadAndFilter(IExclusion::ExclusionState requiredState,
            IExclusion::ExclusionType requiredType);

    private:

        ccSym::CFileStreamImplPtr m_spFileStream;

        ccSettings::ISettingsPtr m_pSettings;
        ccSettings::ISettingsManagerPtr m_pSettingsManager;

        ExResult SetValue(LPCWSTR lpszName, LPBYTE pBuf, DWORD dwSize);

        bool Load(LPCWSTR lpszKey);
        bool Commit(LPCWSTR lpszKey);
        bool SettingsReady();
        bool IsSettingsManagerActive(bool bSpinLock);

        bool m_bInitialized;

        const static DWORD m_dwLoadWaitMax;
        const static DWORD m_dwLoadWaitInterval;
    };
}
#endif