////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AVLOCATIONS_H
#define AVLOCATIONS_H

namespace avRegLocations
{
    static LPCTSTR szInstalledAppsKey = _T("Software\\Symantec\\InstalledApps");
    static LPCTSTR szInstalledAppsValue = _T("NAV");

    static LPCTSTR szNAVKey = _T("Software\\Symantec\\Norton AntiVirus");

    static LPCTSTR szHtmlHelpKey = _T("Software\\Microsoft\\Windows\\HTML Help");
	static LPCTSTR szSymHelpValue = _T("SymHelp.chm");								// This is a registry value name (not a help file name)
	static LPCTSTR szSymHelpFile = _T("SymHelp.chm");								// This is a help file name
}

namespace avccSettingsLocations
{
    static LPCTSTR szNAVKey = _T("Norton AntiVirus");
    static LPCTSTR szDRMDataCacheKey = _T("Norton AntiVirus\\DRMDataCache");
}

namespace avFeatureSettings
{

}

namespace avInstoptsLocations
{

}

#endif //AVLOCATIONS_H
