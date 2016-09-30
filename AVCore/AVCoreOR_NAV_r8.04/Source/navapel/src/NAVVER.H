#ifndef INCL_NAVVER
#define INCL_NAVVER

#define NAV_PRODUCT_NAME         "Norton AntiVirus"
#define NAV_VERSION_MAJOR        5
#define NAV_VERSION_MINOR        3
#define NAV_VERSION_INLINE       1
#define NAV_BUILD_NUMBER         39
#define NAV_BUILD_STRING         "39"
#define NAV_PRODUCT_VERSION      "5.3.1.39"
#define NAV_LEGAL_COPYRIGHT      "Copyright (C) Symantec Corporation 1991-1998"

/*
    The following defines are used INTERNALLY by our products for non UI
    related needs (such as creating mutex's, registry lookups, etc).
    **THIS STRINGS SHOULD NOT BE INTERNATIONALIZED**  **THESE STRINGS SHOULD
    NOT BE USED AS UI COMPONENTS (window titles, etc)**
*/

#define NAV_APP_APW             "Norton AntiVirus Auto-Protect Agent"
#define NAV_APP_APVXD           "Norton AntiVirus Auto-Protect VxD"
#define NAV_APP_CLINIC          "Norton AntiVirus Clinic"
#define NAV_APP_DOWNLOAD        "Norton AntiVirus Download Clinic"

/*
    The following two values are replacements for the values above...  This
    is so that we can use Quake's APIs to read/write registry keys.  Please
    use these if possible.
*/
#define NAV_MODULE_AUTOPROTECT  "Auto-Protect"
#define NAV_MODULE_CLINIC       "Clinic"

// Same as Tomahawk:
#define NAV_APP_RESCUE          "Norton Rescue"
#define NAV_APP_SCHEDULER       "Norton Scheduler"

// These defines are for Netscape so that we can read their registry values
#define NS_APPINFOSECT      "Software\\Netscape\\Netscape Navigator\\Main"
#define NS_MOZILLA          "Mozilla"
#define NS_INSTALLDIR       "Install Directory"
#define NS_INSTALLVER       "Good-2.0"
#define NS_INSTALLVER2      "Good-2"
#define NS_INSTALLVER3      "Good-3"
#define NS_INSTALLVER4      "Good-4"

#ifdef NAVWNT
    #define NAV_APPINFOSECT          "HKEY_LOCAL_MACHINE\\SOFTWARE\\Symantec\\Norton AntiVirus NT"
    #define NAV_INSTALLKEY           "SOFTWARE\\Symantec\\Norton AntiVirus NT\\Install\\5.31"
    #define NAV_INSTCOMPKEY          "SOFTWARE\\Symantec\\Norton AntiVirus NT\\Install\\5.31\\Components"
    //&? MOREWORK - needs run-time check. (if it's actually referenced anywhere)
    #define NAV_WINVERKEY            "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"

                                            // Used for NT scheduler, and
                                            // location of registry keys.
    #define NAV_INSTALLKEY_PRODUCT   "Norton AntiVirus NT"
    #define NAV_INSTALLKEY_FUNC      "Install"
    #define NAV_INSTALLKEY_VER       "5.31"
#else
    #ifdef NAVSCANW
        #define NAV_APPINFOSECT          "HKEY_LOCAL_MACHINE\\SOFTWARE\\Symantec\\Norton AntiVirus Scanner"
        #define NAV_INSTALLKEY           "SOFTWARE\\Symantec\\Norton AntiVirus Scanner\\Install\\5.31"
        #define NAV_INSTCOMPKEY          "SOFTWARE\\Symantec\\Norton AntiVirus Scanner\\Install\\5.31\\Components"
        //&? MOREWORK - needs run-time check. (if it's actually referenced anywhere)
        #define NAV_WINVERKEY            "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"

                                                // Used for NT scheduler, and
                                                // location of registry keys.
        #define NAV_INSTALLKEY_PRODUCT   "Norton AntiVirus Scanner"
    #else
        // These keys are for Symantec shared DLL's (e.g., SYMEVNT).
        // They are the same as Tomahawk:
        #define NAV_APPINFOSECT          "HKEY_LOCAL_MACHINE\\SOFTWARE\\Symantec\\Norton AntiVirus"
        #define NAV_INSTALLKEY           "SOFTWARE\\Symantec\\Norton AntiVirus\\Install\\5.31"
        #define NAV_INSTCOMPKEY          "SOFTWARE\\Symantec\\Norton AntiVirus\\Install\\5.31\\Components"
        #define NAV_WINVERKEY            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion"

                                                // Used for location of registry keys.
        #define NAV_INSTALLKEY_PRODUCT   "Norton AntiVirus"
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

#endif // INCL_NAVVER
