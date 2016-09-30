// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2004, 2005 Symantec Corporation. All rights reserved.

#include "stdafx.h"
#define INITIIDS
#include "ClientReg.h"
#include "WindowsSecurityCenter.h"
#include "WindowsICF.h"
#include "ccSettingsManagerHelper.h"
#include "ccCoInitialize.h"
#include "ccVerifyTrustLibLink.h"
#include "resource.h"

// Swiped from Norton_Internet_Security\src\include\SymSecurePort.h
#define szRegWSClegacyBackupKey   _T("InternetSecurity\\WindowsSecurityCenterBackup")
#define szRegWSClegacyBackupValue _T("LegacyDetectDisabled")
#define szRegICFbackupKey         _T("InternetSecurity\\InternetConnectionFirewallBackup")

// ** GLOBALS **
ccSym::CDebugOutput		g_DebugOutput("SP2Tool");
IMPLEMENT_CCTRACE(g_DebugOutput)

SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

int main( int argc, char* argv[] )
{
    std::string     companyName;
    std::string     productDisplayName;
    std::string     productVersion;
    bool            signatureUpToDate       = false;
    bool            onAccessEnabled         = false;
    bool            performedAction         = false;
    bool            firewallEnabled         = false;
    WindowsSecurityCenter::WMIfailPoint    failPoint    = WindowsSecurityCenter::WMIfailPoint_CreateLocator;
    bool            invalidCommandLine      = true;
	HRESULT			comInitializationStatus	= E_FAIL;
    HRESULT         returnValHR             = E_FAIL;

    // Initialize
    comInitializationStatus = CoInitialize(NULL);
	if (FAILED(comInitializationStatus))
	{
		printf("Error 0x%08x initializing COM - unable to continue!\n", comInitializationStatus);
		exit(1);
	}

    // Display banner
    printf("Symantec Client Security Windows XP SP2 Utility\n");
    printf("Copyright (c) 2004 Symantec Corporation.  Do not redistribute.\n\n");

    // Parse command line
    if (argc >= 2)
    {
		// WSC Legacy detection
        if (strcmpi(argv[1], "wsclegacydisableav") == 0)
        {
            if (argc == 2)
            {
                invalidCommandLine = false;
                returnValHR = WindowsSecurityCenter::InstallDisableLegacy(WindowsSecurityCenter::LEGACYDETECT_SAV, szReg_Key_Main "\\" szReg_Key_WindowsSecurityCenter_LegacyDetectBackup, szReg_Val_WindowsSecurityCenter_LegacyDetectDisabled);
				if (returnValHR == S_FALSE)
					printf("Returned S_FALSE.\n");
				else if (FAILED(returnValHR))
					printf("Error 0x%08x occured.\n", returnValHR);
            }
        }
        else if (strcmpi(argv[1], "wsclegacyrestoreav") == 0)
        {
            if (argc == 2)
            {
                invalidCommandLine = false;
                returnValHR = WindowsSecurityCenter::InstallRestoreLegacy(WindowsSecurityCenter::LEGACYDETECT_SAV, szReg_Key_Main "\\" szReg_Key_WindowsSecurityCenter_LegacyDetectBackup, szReg_Val_WindowsSecurityCenter_LegacyDetectDisabled);
				if (returnValHR == S_FALSE)
					printf("WSC legacy backup not found - nothing to do.\n");
				else if (FAILED(returnValHR))
					printf("Error 0x%08x occured.\n", returnValHR);
            }
        }
        else if (strcmpi(argv[1], "wsclegacydisablefw") == 0)
        {
            if (argc == 2)
            {
				ccSettings::CSettingsManagerHelper		settingsHelper;
				ccSettings::ISettingsManagerPtr			settingsManagerPtr;
				SYMRESULT								returnValSR						= SYMERR_UNKNOWN;

				invalidCommandLine = false;
				returnValSR = settingsHelper.Create(settingsManagerPtr.m_p);
				if (SYM_SUCCEEDED(returnValSR))
				{
		            returnValHR = WindowsSecurityCenter::InstallDisableLegacy(settingsManagerPtr, WindowsSecurityCenter::LEGACYDETECT_SCF, szRegWSClegacyBackupKey, szRegWSClegacyBackupValue);
					if (returnValHR == S_FALSE)
						printf("Returned S_FALSE.\n");
					else if (FAILED(returnValHR))
						printf("Error 0x%08x occured.\n", returnValHR);
				}
				else
				{
					printf("Warning:  SymError 0x%08x returned when creating settings object to re-enable ICF.\n", returnValSR);
				}
            }
        }
        else if (strcmpi(argv[1], "wsclegacyrestorefw") == 0)
        {
            if (argc == 2)
            {
				ccSettings::CSettingsManagerHelper		settingsHelper;
				ccSettings::ISettingsManagerPtr			settingsManagerPtr;
				SYMRESULT								returnValSR						= SYMERR_UNKNOWN;

				invalidCommandLine = false;
				returnValSR = settingsHelper.Create(settingsManagerPtr.m_p);
				if (SYM_SUCCEEDED(returnValSR))
				{
	                returnValHR = WindowsSecurityCenter::InstallRestoreLegacy(settingsManagerPtr, WindowsSecurityCenter::LEGACYDETECT_SCF, szRegWSClegacyBackupKey, szRegWSClegacyBackupValue);
					if (returnValHR == S_FALSE)
						printf("WSC legacy backup not found - nothing to do.\n");
					else if (FAILED(returnValHR))
						printf("Error 0x%08x occured.\n", returnValHR);
				}
				else
				{
					printf("Warning:  SymError 0x%08x returned when creating settings object to re-enable ICF.\n", returnValSR);
				}
            }
        }
        else if (strcmpi(argv[1], "wsclegacydisabledebug") == 0)
        {
            if (argc == 5)
            {
                invalidCommandLine = false;
                returnValHR = WindowsSecurityCenter::InstallDisableLegacy(argv[2], argv[3], argv[4]);
				if (returnValHR == S_FALSE)
					printf("Returned S_FALSE.\n");
				else if (FAILED(returnValHR))
					printf("Error 0x%08x occured.\n", returnValHR);
            }
        }
        else if (strcmpi(argv[1], "wsclegacyrestoredebug") == 0)
        {
            if (argc == 5)
            {
                invalidCommandLine = false;
                returnValHR = WindowsSecurityCenter::InstallRestoreLegacy(argv[2], argv[3], argv[4]);
				if (returnValHR == S_FALSE)
					printf("Returned S_FALSE.\n");
				else if (FAILED(returnValHR))
					printf("Error 0x%08x occured.\n", returnValHR);
            }
        }

		// WSC information
        else if (strcmpi(argv[1], "wscupdateav") == 0)
        {
            WindowsSecurityCenter::AntiVirusProductList                 products;
            WindowsSecurityCenter::AntiVirusProductList::iterator       currProduct;
            WindowsSecurityCenter::AntiVirusProduct                     productInfo;
            CComBSTR                                                    guidString;
            _bstr_t                                                     stringConverter;

            // Initialize
            if (argc == 4)
            {
                invalidCommandLine                          = false;
                guidString                                  = WindowsSecurityCenter::WMI_ID_SCS_Antivirus;
                stringConverter                             = guidString;
                productInfo.companyName                     = "Symantec Corporation";
                productInfo.displayName                     = "Symantec AntiVirus Corporate Edition";
                productInfo.instanceGUID                    = stringConverter;
                productInfo.onAccessScanningEnabled         = false;
                productInfo.productUpToDate                 = false;
                productInfo.versionNumber                   = "9.0.0.338";

                // Get current information from WMI
                returnValHR = WindowsSecurityCenter::EnumProducts(&products, NULL, "", &failPoint);
                // Find SavCorp info
                for(currProduct = products.begin(); currProduct != products.end(); currProduct++)
                {
                    if (currProduct->instanceGUID == (LPCTSTR) stringConverter)
                    {
                        productInfo = *currProduct;
                        break;
                    }
                }

                // Set user overrides
                if (atoi(argv[2]) != 0)
                    productInfo.onAccessScanningEnabled = true;
                else
                    productInfo.onAccessScanningEnabled = false;
                if (atoi(argv[3]) != 0)
                    productInfo.productUpToDate = true;
                else
                    productInfo.productUpToDate = false;
                // Set the new enabled status
                returnValHR = WindowsSecurityCenter::UpdateAntivirusStatus(WindowsSecurityCenter::WMI_ID_SCS_Antivirus, productInfo.companyName.c_str(), productInfo.displayName.c_str(), productInfo.versionNumber.c_str(), productInfo.productUpToDate, productInfo.onAccessScanningEnabled, &failPoint);
                if (FAILED(returnValHR))
                    printf("Error 0x%08x occured at point:  %s\n", returnValHR, WindowsSecurityCenter::WMIfailPointToString(failPoint).c_str());
            }
        }
        else if (strcmpi(argv[1], "wscupdatefw") == 0)
        {
            WindowsSecurityCenter::FirewallProductList                  products;
            WindowsSecurityCenter::FirewallProductList::iterator        currProduct;
            WindowsSecurityCenter::FirewallProduct                      productInfo;
            CComBSTR                                                    guidString;
            _bstr_t                                                     stringConverter;

            if (argc == 3)
            {
                // Initialize
                invalidCommandLine                          = false;
                guidString                                  = WindowsSecurityCenter::WMI_ID_SCS_Firewall;
                stringConverter                             = guidString;
                productInfo.companyName                     = "Symantec Corporation";
                productInfo.displayName                     = "Symantec Client Firewall";
                productInfo.instanceGUID                    = stringConverter;
                productInfo.enabled                         = false;
                productInfo.versionNumber                   = "7.1.0.98";

                // Get current information from WMI
                returnValHR = WindowsSecurityCenter::EnumProducts(NULL, &products, "", &failPoint);
                // Find SavCorp info
                for(currProduct = products.begin(); currProduct != products.end(); currProduct++)
                {
                    if (currProduct->instanceGUID == (LPCTSTR) stringConverter)
                    {
                        productInfo = *currProduct;
                        break;
                    }
                }

                // Set user overrides
                if (atoi(argv[2]) != 0)
                    productInfo.enabled = true;
                else
                    productInfo.enabled = false;
                // Set the new enabled status
                returnValHR = WindowsSecurityCenter::UpdateFirewallStatus(WindowsSecurityCenter::WMI_ID_SCS_Firewall, productInfo.companyName.c_str(), productInfo.displayName.c_str(), productInfo.versionNumber.c_str(), productInfo.enabled, &failPoint);
                if (FAILED(returnValHR))
                    printf("Error 0x%08x occured at point:  %s\n", returnValHR, WindowsSecurityCenter::WMIfailPointToString(failPoint).c_str());
            }
        }
        else if (strcmpi(argv[1], "wscshow") == 0)
        {
            WindowsSecurityCenter::AntiVirusProductList                 avProducts;
            WindowsSecurityCenter::AntiVirusProductList::iterator       currAVproduct;
            WindowsSecurityCenter::FirewallProductList                  fwProducts;
            WindowsSecurityCenter::FirewallProductList::iterator        currFWproduct;

            invalidCommandLine = false;
            returnValHR = WindowsSecurityCenter::EnumProducts(&avProducts, &fwProducts, "", &failPoint);
            if (SUCCEEDED(returnValHR))
            {
                if (avProducts.size() > 0)
                {
                    for(currAVproduct = avProducts.begin(); currAVproduct != avProducts.end(); currAVproduct++)
                    {
                        printf("Product %s\n", currAVproduct->displayName.c_str());
                        printf("   Instance GUID:  %s\n", currAVproduct->instanceGUID.c_str());
                        printf("   Company:  %s, Version:  %s\n", currAVproduct->companyName.c_str(), currAVproduct->versionNumber.c_str());
                        printf("   Product up-to-date:  %d, on-access scanning:  %d\n", currAVproduct->productUpToDate, currAVproduct->onAccessScanningEnabled);
                    }
                }
                else
                {
                    printf("(No antivirus products registered with WSC.)\n");
                }
                if (fwProducts.size() > 0)
                {
                    for(currFWproduct = fwProducts.begin(); currFWproduct != fwProducts.end(); currFWproduct++)
                    {
                        printf("Product %s\n", currFWproduct->displayName.c_str());
                        printf("   Instance GUID:  %s\n", currFWproduct->instanceGUID.c_str());
                        printf("   Company:  %s, Version:  %s\n", currFWproduct->companyName.c_str(), currFWproduct->versionNumber.c_str());
                        printf("   Enabled:  %d.\n", currFWproduct->enabled);
                    }
                }
                else
                {
                    printf("(No firewall products registered with WSC.)\n");
                }
            }
            else
            {
                printf("Error 0x%08x occured at point:  %s\n", returnValHR, WindowsSecurityCenter::WMIfailPointToString(failPoint).c_str());
            }
        }
        else if (strcmpi(argv[1], "wscremoveav") == 0)
        {
            if (argc == 2)
            {
                invalidCommandLine = false;
                returnValHR = WindowsSecurityCenter::RemoveAntivirusStatus(WindowsSecurityCenter::WMI_ID_SCS_Antivirus, &failPoint);
                if (FAILED(returnValHR))
                    printf("Error 0x%08x occured at point:  %s\n", returnValHR, WindowsSecurityCenter::WMIfailPointToString(failPoint).c_str());
            }
        }
        else if (strcmpi(argv[1], "wscremovefw") == 0)
        {
            if (argc == 2)
            {
                invalidCommandLine = false;
                returnValHR = WindowsSecurityCenter::RemoveFirewallStatus(WindowsSecurityCenter::WMI_ID_SCS_Firewall, &failPoint);
                if (FAILED(returnValHR))
                    printf("Error 0x%08x occured at point:  %s\n", returnValHR, WindowsSecurityCenter::WMIfailPointToString(failPoint).c_str());
            }
        }

		// WSC
        else if (strcmpi(argv[1], "wscdisable") == 0)
        {
            if (argc == 2)
            {
                invalidCommandLine = false;
                WindowsSecurityCenter::Disable(szReg_Key_Main "\\" szReg_Key_WindowsSecurityCenter_DisabledBackup, false);
				if (returnValHR == S_FALSE)
					printf("Nothing to do.\n");
				else if (FAILED(returnValHR))
					printf("Error 0x%08x occured.\n", returnValHR);
            }
        }
        else if (strcmpi(argv[1], "wscrestore") == 0)
        {
            if (argc == 2)
            {
                invalidCommandLine = false;
                returnValHR = WindowsSecurityCenter::Restore(szReg_Key_Main "\\" szReg_Key_WindowsSecurityCenter_DisabledBackup);
				if (returnValHR == S_FALSE)
					printf("WSC backup keys not found - nothing to do.\n");
				else if (FAILED(returnValHR))
					printf("Error 0x%08x occured.\n", returnValHR);
            }
        }

		// WSC Alerts
        else if (strcmpi(argv[1], "wscalertset") == 0)
		{
			if (argc == 3)
			{
				WindowsSecurityCenter::Alert alertCode;
				LPCTSTR backupValuePath = NULL;
				bool backupOnceOnly = true;
				if (strcmpi(argv[2], "av") == 0)
				{
					alertCode = WindowsSecurityCenter::Alert_Antivirus;
					invalidCommandLine = false;
				}
				else if (strcmpi(argv[2], "fw") == 0)
				{
					alertCode = WindowsSecurityCenter::Alert_Firewall;
					invalidCommandLine = false;
				}
				if (!invalidCommandLine)
				{
					returnValHR = WindowsSecurityCenter::SetAlert(alertCode, WindowsSecurityCenter::AlertStatusChange_Enable, szReg_Key_Main "\\" szReg_Val_WindowsSecurityCenter_AVAlertBackup, true);
					if (FAILED(returnValHR))
						printf("Error 0x%08x occured.\n", returnValHR);
				}
			}
		}
        else if (strcmpi(argv[1], "wscalertrestore") == 0)
		{
			if (argc == 3)
			{
				WindowsSecurityCenter::Alert alertCode;
				LPCTSTR backupValuePath = NULL;
				bool backupOnceOnly = true;
				if (strcmpi(argv[2], "av") == 0)
				{
					alertCode = WindowsSecurityCenter::Alert_Antivirus;
					invalidCommandLine = false;
				}
				else if (strcmpi(argv[2], "fw") == 0)
				{
					alertCode = WindowsSecurityCenter::Alert_Firewall;
					invalidCommandLine = false;
				}
				if (!invalidCommandLine)
				{
					returnValHR = WindowsSecurityCenter::RestoreAlert(alertCode, szReg_Key_Main "\\" szReg_Val_WindowsSecurityCenter_AVAlertBackup);
					if (FAILED(returnValHR))
						printf("Error 0x%08x occured.\n", returnValHR);
				}
			}
		}

		// Internet Connection Firewall
        else if (strcmpi(argv[1], "icfshowconnect") == 0)
        {
            WindowsICF::NetworkConnectionInfoList               connections;
            WindowsICF::NetworkConnectionInfoList::iterator     currConnection;
            CComBSTR                                            guidStringValue;
            _bstr_t                                             connectionIDstring;
            _bstr_t                                             connectionObjectClassIDstring;
            _bstr_t                                             uiObjectClassIDstring;
            bool                                                icf2Enabled                     = false;

            invalidCommandLine = false;
            returnValHR = WindowsICF::EnumNetworkConnectionInfo(&connections);
            if (SUCCEEDED(returnValHR))
            {
                for (currConnection = connections.begin(); currConnection != connections.end(); currConnection++)
                {
                    // Initialize
                    guidStringValue = currConnection->connectionID;
                    connectionIDstring = guidStringValue;
                    guidStringValue = currConnection->connectionObjectClassID;
                    connectionObjectClassIDstring = guidStringValue;
                    guidStringValue = currConnection->uiObjectClassID;
                    uiObjectClassIDstring = guidStringValue;

                    // Display this connection
                    printf("%s (%s)\n", currConnection->name.c_str(), (LPCTSTR) connectionIDstring);
                    printf("   Device:  %s\n", currConnection->deviceName.c_str());
                    printf("   Share type: %d, connection status: %d, characteristics: 0x%x, media type: %d.\n", (DWORD) currConnection->connectionType, (DWORD) currConnection->connectionStatus, (DWORD) currConnection->characteristics, (DWORD) currConnection->mediaType);
                    printf("   Object CLSID: %s\n", (LPCTSTR) connectionObjectClassIDstring);
                    printf("   UI CLSID: %s\n", (LPCTSTR) uiObjectClassIDstring);
                    printf("   Firewalled (ICF): %s, Shared: %s\n", currConnection->firewallEnabled?"TRUE":"FALSE", currConnection->sharingEnabled?"TRUE":"FALSE");
                }
            }

            // Show status for WinXP SP2 ICF2
            returnValHR = WindowsICF::IsWindowsICFenabledXPSP2(&icf2Enabled);
            if (SUCCEEDED(returnValHR))
            {
                if (icf2Enabled)
                    printf("Windows XP SP2 ICF2 Enabled.");
                else
                    printf("Windows XP SP2 ICF2 Disabled.");
            }
            else
            {
                printf("Windows XP SP2 ICF2 not detected.\n");
            }
        }
        else if (strcmpi(argv[1], "icfdisable") == 0)
        {
            if (argc == 2)
            {
				ccSettings::CSettingsManagerHelper		settingsHelper;
				ccSettings::ISettingsManagerPtr			settingsManagerPtr;
				SYMRESULT								returnValSR						= SYMERR_UNKNOWN;

				invalidCommandLine = false;
				returnValSR = settingsHelper.Create(settingsManagerPtr.m_p);
				if (SYM_SUCCEEDED(returnValSR))
				{
					returnValHR = WindowsICF::Disable(settingsManagerPtr, szRegICFbackupKey, IDS_ICFDISABLE_CLASSNAME, IDS_ICFDISABLE_WINDOWCAPTION, IDS_ICFDISABLE_YESBUTTONID, IDS_ICFDISABLE_MAXWAITTIME, true);
					if (FAILED(returnValHR))
						printf("Error %x disabling ICF.\n", returnValHR);
					else if (returnValHR == S_FALSE)
						printf("ICF already disabled - nothing to do.\n");
				}
				else
				{
					printf("Warning:  SymError 0x%08x returned when creating settings object to re-enable ICF.\n", returnValSR);
				}
            }
        }
        else if (strcmpi(argv[1], "icfrestore") == 0)
        {
            if (argc == 2)
            {
				ccSettings::CSettingsManagerHelper		settingsHelper;
				ccSettings::ISettingsManagerPtr			settingsManagerPtr;
				SYMRESULT								returnValSR						= SYMERR_UNKNOWN;

				invalidCommandLine = false;
				returnValSR = settingsHelper.Create(settingsManagerPtr.m_p);
				if (SYM_SUCCEEDED(returnValSR))
				{
					returnValHR = WindowsICF::Restore(settingsManagerPtr, szRegICFbackupKey, IDS_ICFDISABLE_CLASSNAME, IDS_ICFDISABLE_WINDOWCAPTION, IDS_ICFDISABLE_YESBUTTONID, IDS_ICFDISABLE_MAXWAITTIME);
					if (FAILED(returnValHR))
						printf("Error %x disabling ICF.\n", returnValHR);
					else if (returnValHR == S_FALSE)
						printf("ICF backup key/values not found - nothing to do.\n");
				}
				else
				{
					printf("Warning:  SymError 0x%08x returned when creating settings object to re-enable ICF.\n", returnValSR);
				}
            }
        }
        else if ((strcmpi(argv[1], "?") == 0) || (strcmpi(argv[1], "/?") == 0) || (strcmpi(argv[1], "help") == 0) || (strcmpi(argv[1], "/help") == 0))
        {
            invalidCommandLine = false;
            printf("Command lines:\n");
            printf("   wsclegacydisableav\n");
            printf("   wsclegacyrestoreav\n");
            printf("   wsclegacydisablefw\n");
            printf("   wsclegacyrestorefw\n\n");

            printf("   wscupdateav <OnAccessEnabled> <ProductUpToDate>\n");
            printf("   wscupdatefw <Enabled>\n");
            printf("   wscshow\n");
            printf("   wscremoveav\n");
            printf("   wscremovefw\n\n");
            
			printf("   wscdisable\n");
            printf("   wscrestore\n\n");

			printf("   wscalertset [av fw]\n");
			printf("   wscalertrestore [av fw]\n\n");

			printf("   icfshowconnect\n");
            printf("   icfdisable\n");
            printf("   icfrestore\n");
        }
    }

    if (invalidCommandLine)
        printf("Invalid command line.\n");

    CoUninitialize();
    return 0;
}