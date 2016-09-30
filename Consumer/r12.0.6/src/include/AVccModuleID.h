#pragma once


/////////////////////////////////////////////////////////////////////////
// AV Module IDs
// Reserved range: 3000 - 3999

enum AV_MODULE_IDs
{
    AV_MODULE_ID_BASE = 3000,        // 3000
    AV_MODULE_ID_EVENT_FACTORY,      // 3001
    AV_MODULE_ID_MAIN_UI,            // 3002
    AV_MODULE_ID_NAVLNCH,            // 3003
    AV_MODULE_ID_STATUS,             // 3004
    AV_MODULE_ID_OPTIONS_UI,         // 3005
    AV_MODULE_ID_LU_CALLBACK,        // 3006
    AV_MODULE_ID_SCAN_AND_DELIVER,   // 3007
    AV_MODULE_ID_SCRIPT_BLOCKING_UI, // 3008
    AV_MODULE_ID_INFO_WIZARD,        // 3009
    AV_MODULE_ID_DEF_ALERT,          // 3010
    AV_MODULE_ID_LOG_VIEWER_PLUGIN,  // 3011
    AV_MODULE_ID_ABOUT_BOX,          // 3012
    AV_MODULE_ID_SHELL_EXT,          // 3013
    AV_MODULE_ID_SCAN_TASKS,         // 3014
    AV_MODULE_ID_SCAN_TASK_WIZARD,   // 3015
    AV_MODULE_ID_ERROR_MODULE,       // 3016
    AV_MODULE_ID_OEM_TOOL,           // 3017
    AV_MODULE_ID_BOOT_WARN,          // 3018
    AV_MODULE_ID_NAVW,               // 3019
    AV_MODULE_ID_OFFICEAV,           // 3020
    AV_MODULE_ID_IMSCAN,             // 3021
    AV_MODULE_ID_VIRUS_LIST,         // 3022
    AV_MODULE_ID_OPTIONS_UI_SCRIPT,  // 3023
    AV_MODULE_ID_NAV_LICENSE,        // 3024
    AV_MODULE_ID_NIS_LICENSE,        // 3025
    AV_MODULE_ID_QUARANTINE,         // 3026
    AV_MODULE_ID_NAV_PRODUCT_PLUGIN, // 3027
    AV_MODULE_ID_NAVUIRES,           // 3028
    AV_MODULE_ID_NAVOPTREFRESH,      // 3029
    AV_MODULE_ID_NAVAPW32,           // 3030
    AV_MODULE_ID_NAVUIHTM,           // 3031
    AV_MODULE_ID_SCANMANAGER,        // 3032
    AV_MODULE_ID_NAVSETUP,           // 3033
    AV_MODULE_ID_AVNSCPLG            // 3034      
};

/////////////////////////////////////////////////////////////////////////
// SAVRT Module IDs
// Reserved range: 4000 - 4999
enum SAVRT_MODULE_IDs
{
    SAVRT_MODULE_ID_BASE = 4000,     // 4000
    SAVRT_MODULE_ID_NAVAPSVC,        // 4001
    SAVRT_MODULE_ID_NAVAPSCR,        // 4002
    SAVRT_MODULE_ID_DRIVER           // 4003
};
