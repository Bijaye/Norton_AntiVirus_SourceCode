// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_wizard.cpv   1.8   09 Mar 1998 23:42:50   DALLEE  $
//
// Description:
//      Repair Wizard dialog.  (Much of the code is based upon the Scan
//      Results dialog in N_RESULT.CPP.)
//
//      The Repair Wizard is only one dialog; it changes panels by dynamically
//      hiding and unhiding controls.
//
// Contains:
//      LaunchCleanupWizard()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_wizard.cpv  $
// 
//    Rev 1.8   09 Mar 1998 23:42:50   DALLEE
// Changes for new (MFC-based) Repair Wizard.
// 
//    Rev 1.7   21 Jul 1997 01:40:16   BGERHAR
// Removed hacked /NORESULTS handling (now using uProgramMode=PROGRAM_AUTOMATED)
// 
//    Rev 1.6   18 Jul 1997 17:27:36   BGERHAR
// Slight change for /NORESULTS rework
// 
//    Rev 1.5   15 Jul 1997 14:42:32   DBUCHES
// Added Ctl3d support for DEC Alpha
// 
//    Rev 1.4   26 Jun 1997 16:45:08   TIVANOV
// check if the user is using the NORESULTS command line switch
//
//    Rev 1.3   29 May 1997 11:53:58   TIVANOV
// fix the internal compiler error
//
//    Rev 1.2   22 May 1997 16:21:18   BILL
// Alpha changes (removed ctl3d)
//
//    Rev 1.1   24 Apr 1997 11:03:32   JBRENNA
// Tesla port: JMILLAR: make the last string significantly longer - it is
// multi-line dialog text.
//
//    Rev 1.0   06 Feb 1997 21:02:56   RFULLER
// Initial revision
//
//    Rev 1.12   29 Oct 1996 10:22:02   JBRENNA
// STS 73413 - Dynamically move the "Ad&vice" button to just below the
// ID_SR_LISTBOX. This ensures that the two windows do not overlap on
// Win95 Korean.
//************************************************************************

#include "stdafx.h"
#include "platform.h"
#include "xapi.h"
#include "disk.h"
#include "ngui.h"
	#include "ctl3d.h"

#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#include "stdhelp.h"
#include "symcfg.h"
#pragma optimize( "", off  )   // Turns off p-code
#include "navutil.h"
#pragma optimize( "", on  )   // Turns on p-code
#include "exclude.h"
#include "options.h"
#include "navw_hlp.h"
#include "cmbstr.h"
#include "nunavdlg.h"
#include "nunavstb.h"
#include "security.h"

#include "stub.h"
#include "virscan.h"
#include "inoc.h"
#include "nonav.h"
#include "navver.h"
#include "AlignWnd.h"
#include "navcomm.h"

#include "rwizard.h"

//************************************************************************
// EXTERNS and GLOBALS
//************************************************************************

extern  HINSTANCE   hInstance;

//************************************************************************
// PROTOTYPES
//************************************************************************

BOOL LOCAL GetUseRepairWizardDefault();
VOID LOCAL SetUseRepairWizardDefault( BOOL bUseWizard );


//************************************************************************
// LaunchCleanupWizard()
//
// VOID SYM_EXPORT WINAPI LaunchCleanupWizard (
//     HWND        hDlg,                [in] Parent
//     LPNONAV     lpNav,               [in] Info on this scan
//     UINT        uPanel,              [in] unused (was initial panel to display)
//     LPDWORD     dwFlags )            [out] Result flags from repair wizard
//
// This routine launches the Repair Wizard.
//
// Now using new MFC CRepairWizard implementation.
//
// Returns:
//  Nothing.
//************************************************************************
// 06/06/1996 DALLEE, created.
//************************************************************************

VOID SYM_EXPORT WINAPI LaunchCleanupWizard (
    HWND        hDlg,
    LPNONAV     lpNav,
    UINT        uPanel,
    LPDWORD     lpdwFlags )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    
    auto    CRepairWizard   wizRepair( CWnd::FromHandle( hDlg ) );
    auto    int             nResult;

    *lpdwFlags = 0;

    wizRepair.SetNoNav( lpNav );

    nResult = wizRepair.DoModal();

    if ( IDCANCEL == nResult )
        {
        *lpdwFlags |= RWIZFLAG_CANCEL;
        }

    if ( wizRepair.GetManualRepair() )
        {
        *lpdwFlags |= RWIZFLAG_USEADVANCED;
        }
} // LaunchCleanupWizard()    


//************************************************************************
// GetUseRepairWizardDefault()
//
// This routine returns the default prompt for whether to use repair wizard.
//
// Parameters:
//  None.
//
// Returns:
//  FALSE   if RepairWizardDefault key under NAV\Clinic is == 0.
//  TRUE    if non-zero, or doesn't exist.
//************************************************************************
// 06/06/1996 DALLEE, created.
//************************************************************************

BOOL LOCAL GetUseRepairWizardDefault ( )
{
    auto    DWORD       dwValue;
    auto    char        szValueName [SYM_MAX_PATH];
    auto    BOOL        bUseWizard = TRUE;

    auto    LONG        lRegResult;
    auto    DWORD       dwType;
    auto    DWORD       dwValueSize;
    auto    CFGREGKEY   cfg;

    if ( LoadString(hInstance,
                    IDS_REPWIZ_DEFAULTREGKEY,
                    szValueName,
                    sizeof(szValueName)) )
        {
        MEMSET(&cfg, 0, sizeof(cfg));

        cfg.hkey = HKEY_LOCAL_MACHINE;
        cfg.lpszProduct = NAV_INSTALLKEY_PRODUCT;
        cfg.lpszVersion = NAV_MODULE_CLINIC;
        cfg.lpszProject = NULL;
        cfg.lpszSection = NULL;
        cfg.lpszEntry = NULL;
        cfg.lpszName = szValueName;
        cfg.lpszSwitch = NULL;

        dwValueSize = sizeof(dwValue);

        lRegResult = ConfigRegQueryValue(&cfg,
                                         NULL,
                                         &dwType,
                                         (LPBYTE)&dwValue,
                                         &dwValueSize);

        if ( (ERROR_SUCCESS == lRegResult) &&
             (REG_DWORD == dwType) )
            {
            bUseWizard = (0 != dwValue);
            }
        }

    return (bUseWizard);

} // GetUseRepairWizardDefault()


//************************************************************************
// SetUseRepairWizardDefault()
//
// This routine sets the default prompt for whether to use repair wizard.
//
// Parameters:
//  BOOL bUseWizard         Whether to suggest wizard by default or not.
//
// Returns:
//  Nothing.
//************************************************************************
// 06/06/1996 DALLEE, created.
//************************************************************************

VOID LOCAL SetUseRepairWizardDefault (BOOL bUseWizard)
{
    auto    DWORD       dwValue = (DWORD) bUseWizard;
    auto    char        szValueName [SYM_MAX_PATH];

    auto    DWORD       dwType = REG_DWORD;
    auto    CFGREGKEY   cfg;

    if ( LoadString(hInstance,
                    IDS_REPWIZ_DEFAULTREGKEY,
                    szValueName,
                    sizeof(szValueName)) )
        {
        MEMSET(&cfg, 0, sizeof(cfg));

        cfg.hkey = HKEY_LOCAL_MACHINE;
        cfg.lpszProduct = NAV_INSTALLKEY_PRODUCT;
        cfg.lpszVersion = NAV_MODULE_CLINIC;
        cfg.lpszProject = NULL;
        cfg.lpszSection = NULL;
        cfg.lpszEntry = NULL;
        cfg.lpszName = szValueName;
        cfg.lpszSwitch = NULL;

        ConfigRegSetValue(&cfg,
                          0,
                          dwType,
                          &dwValue,
                          sizeof(dwValue));
        }
} // SetUseRepairWizardDefault()

