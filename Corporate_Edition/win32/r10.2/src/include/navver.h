/////////////////////////////////////////////////////////////////////////////
//
// NAVVER.H -- Symantec AntiVirus product version information
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1995 - 2004, 2005 Symantec Corporation. All rights reserved.
//
// The #defines's in this file are used by NAV.VER to create the VERSIONINFO
// resource, and can also be used in the standard about box.
//
// When incrementing the build number, be sure to change it in all places!
// You must update NAV_BUILD_NUMBER, NAV_BUILD_STRING, and NAV_PRODUCT_VERSION.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef INCL_NAVVER
#define INCL_NAVVER

#ifdef _DEBUG
	#define NAV_PRODUCT_NAME         "Symantec AntiVirus * DEBUG BUILD *"
	#define NAV_LEGAL_COPYRIGHT      "Copyright 1991 - 2006 Symantec Corporation. All rights reserved. * DEBUG BUILD not for external release. *"
#else
	#define NAV_PRODUCT_NAME         "Symantec AntiVirus"
	#define NAV_LEGAL_COPYRIGHT      "Copyright 1991 - 2006 Symantec Corporation. All rights reserved."
#endif

#define NAV_VERSION_MAJOR        10
#define NAV_VERSION_MINOR        2
#define NAV_VERSION_INLINE       0
#define NAV_BUILD_NUMBER         224
#define NAV_BUILD_STRING         "224"
#define NAVFILEPRODUCTVERSION    "10.2.0.224"
#define NAV_PRODUCT_VERSION      NAVFILEPRODUCTVERSION

/*
    The following defines are used INTERNALLY by our products for non UI
    related needs (such as creating mutex's, registry lookups, etc).
    **THIS STRINGS SHOULD NOT BE INTERNATIONALIZED**  **THESE STRINGS SHOULD
    NOT BE USED AS UI COMPONENTS (window titles, etc)**
*/

#define NAV_APP_APW             "Symantec AntiVirus Auto-Protect Agent"
#define NAV_APP_APVXD           "Symantec AntiVirus Auto-Protect VxD"
#define NAV_APP_CLINIC          "Symantec AntiVirus Clinic"
#define NAV_APP_DOWNLOAD        "Symantec AntiVirus Download Clinic"

/*
    The following two values are replacements for the values above...  This
    is so that we can use Quake's APIs to read/write registry keys.  Please
    use these if possible.
*/
#define NAV_MODULE_AUTOPROTECT  "Auto-Protect"
#define NAV_MODULE_CLINIC       "Clinic"

// Same as Tomahawk:
#define NAV_APP_RESCUE          "Symantec Rescue"
#define NAV_APP_SCHEDULER       "Symantec Scheduler"

// These defines are for Netscape so that we can read their registry values
#define NS_APPINFOSECT      "Software\\Netscape\\Netscape Navigator\\Main"
#define NS_MOZILLA          "Mozilla"
#define NS_INSTALLDIR       "Install Directory"
#define NS_INSTALLVER       "Good-2.0"
#define NS_INSTALLVER2      "Good-2"
#define NS_INSTALLVER3      "Good-3"
#define NS_INSTALLVER4      "Good-4"

#ifdef NAVWNT
    #define NAV_APPINFOSECT          "HKEY_LOCAL_MACHINE\\SOFTWARE\\Symantec\\Symantec AntiVirus NT"
    #define NAV_INSTALLKEY           "SOFTWARE\\Symantec\\Symantec AntiVirus NT\\Install\\5.31"
    #define NAV_INSTCOMPKEY          "SOFTWARE\\Symantec\\Symantec AntiVirus NT\\Install\\5.31\\Components"
    //&? MOREWORK - needs run-time check. (if it's actually referenced anywhere)
    #define NAV_WINVERKEY            "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"

                                            // Used for NT scheduler, and
                                            // location of registry keys.
    #define NAV_INSTALLKEY_PRODUCT   "Symantec AntiVirus NT"
    #define NAV_INSTALLKEY_FUNC      "Install"
    #define NAV_INSTALLKEY_VER       "5.31"
#else
    #ifdef NAVSCANW
        #define NAV_APPINFOSECT          "HKEY_LOCAL_MACHINE\\SOFTWARE\\Symantec\\Symantec AntiVirus Scanner"
        #define NAV_INSTALLKEY           "SOFTWARE\\Symantec\\Symantec AntiVirus Scanner\\Install\\5.31"
        #define NAV_INSTCOMPKEY          "SOFTWARE\\Symantec\\Symantec AntiVirus Scanner\\Install\\5.31\\Components"
        //&? MOREWORK - needs run-time check. (if it's actually referenced anywhere)
        #define NAV_WINVERKEY            "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"

                                                // Used for NT scheduler, and
                                                // location of registry keys.
        #define NAV_INSTALLKEY_PRODUCT   "Symantec AntiVirus Scanner"
    #else
        // These keys are for Symantec shared DLL's (e.g., SYMEVNT).
        // They are the same as Tomahawk:
        #define NAV_APPINFOSECT          "HKEY_LOCAL_MACHINE\\SOFTWARE\\Symantec\\Symantec AntiVirus"
        #define NAV_INSTALLKEY           "SOFTWARE\\Symantec\\Symantec AntiVirus\\Install\\5.31"
        #define NAV_INSTCOMPKEY          "SOFTWARE\\Symantec\\Symantec AntiVirus\\Install\\5.31\\Components"
        #define NAV_WINVERKEY            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion"

                                                // Used for location of registry keys.
        #define NAV_INSTALLKEY_PRODUCT   "Symantec AntiVirus"
    #endif

    #define NAV_INSTALLKEY_FUNC      "Install"
    #define NAV_INSTALLKEY_VER       "5.31"
#endif

// Keys for customizing a version (only Netscape at present)
#define NAV_INSTALLKEY_CUSTOMVERSION "CustomVersion"
#define NAV_INSTALLKEY_NETSCAPE  "Netscape"

// These keys are for Symantec shared DLL's (e.g., SYMEVNT).
// They are the same as Tomahawk:
#define NAV_SHAREDCOMPS          "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDLLs"
#define NAV_SHAREDCOMPSSUB       "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDLLs"

#define NAV_SHAREDCOMPS_NT       "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SharedDLLs"
#define NAV_SHAREDCOMPSSUB_NT    "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SharedDLLs"

// MOREWORK: Why SharedUsage and not SharedDLLs?
#define SYMC_SHAREDUSAGE         "SOFTWARE\\Symantec\\SharedUsage"
                                        // These 3 are same keyword as Tomahawk:
#define NAV_INSTALLEDVAL         "Installed"
#define NAV_INSTALLDIR           "InstallDir"
#define NAV_INSTALL              "Install"

                                        // The "LiveUpdate" install key
                                        // should be deleted and replaced
                                        // with "LiveUpdate1" in future
                                        // versions of NAV95 and NAVNT
#define NAV_ONRAMP_INSTALLEDVAL  "LiveUpdate"
#define NAV_ONRAMP_INSTALLEDVAL1 "LiveUpdate1"

// Key for disabling the splash screen.  This is not global for NAV, but
// for each NAV module (AP, Clinic)

#define NAV_DISABLESPLASHSCREEN  "DisableSplashScreen"

// Key to allow NAVAPW agent to close. Only valid for Win NT Agent.
// Default is 0 (FALSE). Non-zero allows users to close Agent (instead of minimize).

#define NAV_ALLOWCLOSEAGENT      "AllowCloseAgent"

#ifdef NAVSCANW
#define NAV_ENABLEAUTOMATION     "EnableAutomation"
#endif
#define IDS_OSMIGRATEDMESS95TO2K        50002
#define IDS_OSMIGRATEDMESS98TO2K        50003
#define IDS_PROG_NAME					50004
#endif // INCL_NAVVER

// Many product codes will change between builds.  When inlining/branching, the old code should be preserved in a new constant
// (for example, MSI_PRODUCTCODE_SCS_10_0_0 for the original PC, MSI_PRODUCTCODE_SCS_10_0_1 for the PC from inline #1, etc.)
#define MSI_PRODUCTCODE_SCS             "{15DC6090-6909-4C13-93D1-9A4C58880021}"
#define MSI_PRODUCTCODE_SAV             "{6C28BDA4-6D99-4DD0-9F22-6A90A445E982}"
#define MSI_PRODUCTCODE_SAV64AMD        "{99CFBF0F-12D6-4E3E-9F77-42DE19849D8D}"
#define MSI_PRODUCTCODE_SAV64IA         "{99CFBF0F-12D6-4E3E-9F77-42DE19849D8D}"
#define MSI_PRODUCTCODE_SSC             "{CE8401A5-DDA2-4376-A5B5-2210DBBA7DBA}"
#define MSI_PRODUCTCODE_SCFA            "{C54B8A8D-4AD8-46E3-A992-A9BDD4030C4B}"
#define MSI_PRODUCTCODE_AMS             "{D1E5AC9D-DA9D-4167-8130-30B59D01F52D}"
// Upgrade codes will rarely, if ever, change
#define MSI_UPGRADECODE_SCS             "{C037167E-B804-4CBD-8D52-7D2A84BE543F}"
#define MSI_UPGRADECODE_SAV             "{24BF7A02-B60A-494B-843A-793BBC77DED4}"
#define MSI_UPGRADECODE_SAV64AMD        "{F7BE9C8A-C2E6-470D-B703-0A1845E6FF8C}"
#define MSI_UPGRADECODE_SAV64IA         "{F7BE9C8A-C2E6-470D-B703-0A1845E6FF8C}"
#define MSI_UPGRADECODE_SSC             "{6B5769C8-7ACA-4343-8003-231095CF912B}"
#define MSI_UPGRADECODE_SCFA            "{364F509D-350E-464A-96D9-85A18AF50862}"
#define MSI_UPGRADECODE_AMS             "{674DBAB8-A12E-4DAE-86BC-1702C5A7E919}"
