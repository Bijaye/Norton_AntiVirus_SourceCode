// QSINST.H main header file for qsinst.dll
//

// AMS Install Flags
#define AMS_FLAGS_SERVER            0x0001
#define AMS_FLAGS_CONSOLE           0x0002
#define AMS_FLAGS_CLIENT            0x0004
#define AMS_FLAGS_CLIENTLITE        0x0008
#define AMS_PRODUCT_NAME            "LDVP"
#define AMS_INSTALL_DLL             "InstallAMS.DLL"
#define AMSCONSOLE_PATH				TEXT("AMS2\\WINNT")
#define MAX_STR_LEN					512

#define QSERVER_REGKEY              "Software\\Symantec\\Quarantine\\Server"
#define AMSINSTALLED_REGVALUE       "AMSClientInstalled"
#define AMSINSTALLPATH_REGVALUE     "AMSInstallPath"
#define INSTALLPATH_REGVALUE        "InstallPath"
#define QFOLDER_DELETE_REGVALUE     "QuarantineFolderDelete"

#define ID_INTELFILES_QSERVER	    "SYMANTEC_QSERVER"
