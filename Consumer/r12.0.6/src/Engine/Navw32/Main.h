
#ifndef MAIN_H
#define MAIN_H

bool RunIntegrator();
HWND ShowHelp();
bool HasUserAgreedToEULA();
bool IsCfgwizFinished();

HRESULT PerformPepCall(DJSMAR_LicenseType &licType, DJSMAR00_LicenseState &licState);

HMODULE GetAlertDll();
bool IsLicenseValid(bool bVerbose, DJSMAR00_LicenseState &licState, HMODULE hModule);
HRESULT CheckVirusDefsAndSubscriptionStatus(DJSMAR_LicenseType &licType, DJSMAR00_LicenseState &licState, HMODULE hModule);

#endif

