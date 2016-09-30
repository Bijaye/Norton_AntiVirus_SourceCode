////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

class CConfigManifests
{
public:
    CConfigManifests(void);
    ~CConfigManifests(void);
    bool Initialize(void);
    HRESULT CopyManifest(const wchar_t* szSource);
protected:
    HRESULT CopyManifestFile(const wchar_t* szSourceDir, const wchar_t* szDestDir, const wchar_t* szFileName, const wchar_t* szExtension);
};
