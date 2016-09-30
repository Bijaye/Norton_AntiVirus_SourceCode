// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------


#define SAV_BUTTON_AREA_TOP    58
#define SAV_BUTTON_AREA_LEFT   40
#define SAV_BUTTON_AREA_HEIGHT 236
#define SAV_BUTTON_HEIGHT      40
#define SAV_BUTTON_WIDTH       509

#define BUTTON_TEXT_V_OFFSET  0
#define BUTTON_TEXT_H_OFFSET 20
#define BUTTON_TEXT_V_DESC_OFFSET 15
#define BUTTON_TEXT_H_DESC_OFFSET 8

// Spacing calculations macros, they are passed contants so the compiler will evaluate this at compile time.
#define SAV_EXTRA_SPACE(n)     (SAV_BUTTON_AREA_HEIGHT - (n * SAV_BUTTON_HEIGHT))
#define SAV_BUTTON_SPACING(n)  (SAV_EXTRA_SPACE(n)/(n+1))
#define SAV_BUTTON_SPACING_TOP_EXTRA(n)  ( ((SAV_EXTRA_SPACE(n)%(n+1))/2) + SAV_BUTTON_SPACING(n) )
#define SAV_BUTTON_TOP(x,n) (SAV_BUTTON_AREA_TOP+SAV_BUTTON_SPACING_TOP_EXTRA(n)+(SAV_BUTTON_SPACING(n)+SAV_BUTTON_HEIGHT)*((x)-1))

// SAV_BUTTON_POSITION defines the 4 button position parameters:
//      This macro parameters x & n are the current button, and the total button count for the menu, respectively
#define SAV_BUTTON_POSITION(x,n)  SAV_BUTTON_AREA_LEFT, SAV_BUTTON_TOP(x,n), SAV_BUTTON_WIDTH, SAV_BUTTON_HEIGHT

#define SAV_MENU_MAIN   _T("Main")
#define SAV_MENU_SAV    _T("SAV")
#define SAV_MENU_TOOLS  _T("Tools")

ExecuteMsiByOSArgs clientInstallArguments(_T("\\SAV\\setup.exe"), _T("SAV_INST.log SAVALLOWCLIENTONLY=1"),_T("\\SAVWin64\\x86\\setup.exe"), _T("SAV_INST.log SAVALLOWCLIENTONLY=1"));

// global button array
//  create one for each submenu
static BUTTONINFO biMainButtons[] = {
    SAV_BUTTON_POSITION(1,4), 
        IDS_BUTTON_MAIN_READ_ME,
        IDS_DESC_READ_ME,
        1, 
        BUTTONINFO::eEXECUTE,
        NULL,
        _T("\\Docs\\RTF.html"),
        AlwaysEnabledButton,
        IDS_ERROR_README,

    SAV_BUTTON_POSITION(2,4), 
        IDS_BUTTON_MAIN_SAV_MENU,
        IDS_DESC_SAV_MENU,
        2, 
        BUTTONINFO::eSUBMENU,
        NULL,
        SAV_MENU_SAV,
        AlwaysEnabledButton,
        0,

    SAV_BUTTON_POSITION(3,4), 
        IDS_BUTTON_MAIN_TOOLS_MENU,
        IDS_DESC_TOOLS_MENU,
        3, 
        BUTTONINFO::eSUBMENU,
        NULL,
        SAV_MENU_TOOLS,
        AlwaysEnabledButton,
        0,

    SAV_BUTTON_POSITION(4,4), 
        IDS_BUTTON_EXIT,                  
        IDS_DESC_EXIT,
        ID_BUTTON_EXIT,
        BUTTONINFO::eEXIT,
        NULL,
        NULL,
        AlwaysEnabledButton,
        0,
};

static BUTTONINFO biSavButtons[] = {
    SAV_BUTTON_POSITION(1,5),
        IDS_BUTTON_INSTALL_SSC,
        IDS_DESC_INSTALL_SSC,
        1,
        BUTTONINFO::eEXECUTE,
        NULL,
        _T("\\SSC\\setup.exe"),
        AlwaysEnabledButton,
        IDS_ERROR_SSC,

    SAV_BUTTON_POSITION(2,5),
        IDS_BUTTON_INSTALL_SAV_SERVER,
        IDS_DESC_INSTALL_SAV_SERVER,
        2, 
        BUTTONINFO::eEXECUTE,
        NULL,
        _T("\\ROLLOUT\\AVServer\\setup.exe"),
        AlwaysEnabledButton,
        IDS_ERROR_SAV,

    SAV_BUTTON_POSITION(3,5), 
        IDS_BUTTON_INSTALL_REPORT_SERVER,
        IDS_DESC_INSTALL_REPORT_SERVER,
        3,
        BUTTONINFO::eEXECUTE,
        NULL,
        _T("\\Reporting\\Install.exe"),
        AlwaysEnabledButton,
        IDS_ERROR_REPORTER,

    SAV_BUTTON_POSITION(4,5),
        IDS_BUTTON_INSTALL_SAV_CLIENT,
        IDS_DESC_INSTALL_SAV_CLIENT,
        4,
        BUTTONINFO::eEXECUTE_MSI_BY_OS,
        &clientInstallArguments,
        NULL,
        AlwaysEnabledButton,
        IDS_ERROR_SAV,

    SAV_BUTTON_POSITION(5,5),
        IDS_RETURN_TO_MAIN,                  
        IDS_DESC_RETURN_TO_MAIN,
        5,
        BUTTONINFO::eSUBMENU,
        NULL,
        SAV_MENU_MAIN,
        AlwaysEnabledButton,
        0,
};

static BUTTONINFO biToolsButtons[] = {
    SAV_BUTTON_POSITION(1,5),
        IDS_BUTTON_INSTALL_LUADMIN,
        IDS_DESC_INSTALL_LUADMIN,
        1,
        BUTTONINFO::eEXECUTE,
        NULL,
        _T("\\Tools\\LiveUpdate\\LUAU.exe"),
        AlwaysEnabledButton,
        IDS_ERROR_LUADMIN,

    SAV_BUTTON_POSITION(2,5), 
        IDS_BUTTON_INSTALL_QSERVER,
        IDS_DESC_INSTALL_QSERVER,
        3, 
        BUTTONINFO::eEXECUTE,
        NULL,
        _T("\\CentralQ\\QServer\\setup.exe"),
        AlwaysEnabledButton,
        IDS_ERROR_QSERVER,

    SAV_BUTTON_POSITION(3,5),
        IDS_BUTTON_INSTALL_QCONSOLE,
        IDS_DESC_INSTALL_QCONSOLE,
        2,
        BUTTONINFO::eEXECUTE,
        NULL,
        _T("\\CentralQ\\QConsole\\setup.exe"),
        AlwaysEnabledButton,
        IDS_ERROR_QCONSOLE,

    SAV_BUTTON_POSITION(4,5),
        IDS_BUTTON_INSTALL_REPORT_AGENT,
        IDS_DESC_INSTALL_REPORT_AGENT,
        4,
        BUTTONINFO::eEXECUTE,
        NULL,
        _T("\\Reporting\\Agents\\Setup.exe"),
        AlwaysEnabledButton,
        IDS_ERROR_REPORTER_COLLECTOR,

    SAV_BUTTON_POSITION(5,5),
        IDS_RETURN_TO_MAIN,
        IDS_DESC_RETURN_TO_MAIN,
        5,
        BUTTONINFO::eSUBMENU,
        NULL,
        SAV_MENU_MAIN,
        AlwaysEnabledButton,
        0,
};

#define array_count(x) (sizeof(x) / sizeof(x[0]))

// The list of menus must be named "miMenus".
static SYMMENUINFO miMenus[] = {
    { SAV_MENU_MAIN,   biMainButtons,  array_count(biMainButtons),  IDB_BACKDROP_SAV, IDR_DEFAULT_ACCEL,
        IDBUTTON_UP, IDBUTTON_DN, IDBUTTON_HI, IDBUTTON_DISABLED },
    { SAV_MENU_SAV,    biSavButtons,   array_count(biSavButtons),   IDB_BACKDROP_SAV, IDR_DEFAULT_ACCEL,
        IDBUTTON_UP, IDBUTTON_DN, IDBUTTON_HI, IDBUTTON_DISABLED },
    { SAV_MENU_TOOLS,  biToolsButtons, array_count(biToolsButtons), IDB_BACKDROP_SAV, IDR_DEFAULT_ACCEL,
        IDBUTTON_UP, IDBUTTON_DN, IDBUTTON_HI, IDBUTTON_DISABLED },
};

// MENU_MAIN defines name of the first menu displayed.
#define MENU_MAIN      SAV_MENU_MAIN
// APP_NAME_ID defines the resource id of string displayed in the main window title bar.
#define APP_NAME_ID    IDS_SAV_TITLE

// Define the various menu text colors used for the various menu button states.
const DWORD NORMAL_COLOR     = RGB(0, 0, 0);
const DWORD MOUSEOVER_COLOR  = RGB(0, 0, 0);
const DWORD MOUSEDOWN_COLOR  = RGB(0, 0, 0);
const DWORD DISABLED_COLOR   = RGB(192, 192, 192);
