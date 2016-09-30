////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* NAV Instant Messaging Options Object                                 */
/************************************************************************/

#ifndef __NAVOPTIONS_IMOPTIONS_H__
#define __NAVOPTIONS_IMOPTIONS_H__

#include "stdafx.h"
#include <map>
#include "IMConfigInterface.h"

class CIMOptions
{
public:
    CIMOptions();
    ~CIMOptions();

    // CORE OPTIONS INTERFACE BEGIN
    HRESULT Initialize();
    HRESULT Save();
    HRESULT Default();
    HRESULT Notify();
    bool IsDirty();
    // CORE OPTIONS INTERFACE END

    // Specific Options
    HRESULT GetState(const ISShared::IIMConfig::IMTYPE client, DWORD& dwState);
    HRESULT SetState(const ISShared::IIMConfig::IMTYPE client, const DWORD& dwState);
    HRESULT IsInstalled(const ISShared::IIMConfig::IMTYPE client, bool& bInstalled);
    HRESULT Configure(const ISShared::IIMConfig::IMTYPE client);

private:
    bool m_bInitialized;
    bool m_bDirty;

    typedef ::std::map<ISShared::IIMConfig::IMTYPE, bool> ImTypeMap;
    ImTypeMap m_ImMap;
    ISShared::IIMConfigPtr m_spConfig;
};

#endif // __NAVOPTIONS_IMOPTIONS_H__