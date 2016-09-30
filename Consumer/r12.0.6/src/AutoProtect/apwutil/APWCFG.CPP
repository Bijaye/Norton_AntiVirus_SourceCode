// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/APWUTIL/VCS/apwcfg.cpv   1.3   27 Aug 1997 15:49:58   DALLEE  $
//
// Description:
//      Source for NAVAPW configuration.
//
// Contains:
//      ApwGetConfig()
//
//      CApwConfig:: CApwConfig()
//      CApwConfig:: ~CApwConfig()
//
//      CApwConfig:: ReadNavopts()
//
//      CApwConfig:: GetCanDisable()
//      CApwConfig:: GetCmd()
//      CApwConfig:: GetCustomMessageID()
//      CApwConfig:: GetDoingShutdown()
//      CApwConfig:: GetIcon()
//      CApwConfig:: GetInTray()
//      CApwConfig:: GetInstance()
//      CApwConfig:: GetMainWnd();
//      CApwConfig:: GetShowIcon()
//      CApwConfig:: GetSmallIcon()
//      CApwConfig:: GetState()
//      CApwConfig:: GetTrayState()
//
//      CApwConfig:: SetCanDisable()
//      CApwConfig:: SetCmd()
//      CApwConfig:: SetDoingShutdown()
//      CApwConfig:: SetIcon()
//      CApwConfig:: SetInTray()
//      CApwConfig:: SetInstance()
//      CApwConfig:: SetMainWnd()
//      CApwConfig:: SetShowIcon()
//      CApwConfig:: SetSmallIcon()
//      CApwConfig:: SetState()
//      CApwConfig:: SetTrayState()
//
//
// See Also:
//************************************************************************
// $Log:   S:/APWUTIL/VCS/apwcfg.cpv  $
// 
//    Rev 1.3   27 Aug 1997 15:49:58   DALLEE
// Added new loading and error states to SetTrayState().
// Was failing to track correct tray status.
// 
//    Rev 1.2   10 Jun 1997 14:27:56   DALLEE
// Now register and save custom window message for NAVW communication.
// 
//    Rev 1.1   04 Jun 1997 16:02:26   DALLEE
// Initialize new m_bDoingShutdown member.
// 
//    Rev 1.0   02 Jun 1997 02:47:46   DALLEE
// Initial revision.
//************************************************************************

#include "platform.h"
#include <windowsx.h>
#include <tchar.h>

#include "apw.h"
#include "apwutil.h"
#include "apwcfg.h"
#include "resource.h"
#include "savrt32.h"
#include "APOptNames.h"

#include "navwcomm.h"
#include "cctrace.h"
#include "AutoProtectWrapper.h"


//************************************************************************
// Statics
//************************************************************************

// NAVAP's internal configuration object. Other modules can get a pointer
// by calling ApwGetConfig();

static CApwConfig s_ApwConfig;


//************************************************************************
// External data
//************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

extern HINSTANCE g_hUtilInst;

#ifdef __cplusplus
}
#endif


//************************************************************************
// CApwConfig
//************************************************************************

//************************************************************************
// CApwConfig::CApwConfig()
//
// Constructor for config object.
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

CApwConfig::CApwConfig ()
{
    m_hMainWnd = NULL;
    m_hInst    = NULL;
                  
    m_bCanDisable   = TRUE;
    m_bShowIcon     = TRUE;
    m_bDoingShutdown= FALSE;

	m_uCustomMessageID = RegisterWindowMessage( NAVW_NAVAPW_COMM );

    m_nState      = AP_STATE_DISABLED;
    m_hIcon       = NULL;
    m_hSmallIcon  = NULL;
                  
    m_bInTray     = FALSE;
    m_nTrayState  = AP_STATE_DISABLED;

    m_pCmd        = NULL;
} // CApwConfig::CApwConfig()


//************************************************************************
// CApwConfig::~CApwConfig()
//
// Destructor for config object.
//
// Parameters:
//
// Returns:
//
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

CApwConfig::~CApwConfig ()
{
} // CApwConfig::~CApwConfig()


//************************************************************************
// CApwConfig::GetMainWnd();
//
// This routine gets the handle of NAVAPW's main dialog/window.
// 
// Returns:
//  HWND    main dialog/window handle.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

HWND CApwConfig::GetMainWnd ()
{
    return ( m_hMainWnd );
} // CApwConfig::GetMainWnd()

//************************************************************************
// CApwConfig::SetMainWnd()
//
// This routine saves the handle of NAVAPW's main dialog/window.
// 
// Returns:
//  Nothing.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

VOID CApwConfig::SetMainWnd ( HWND hMainWnd )
{
    m_hMainWnd = hMainWnd;
} // CApwConfig::SetMainWnd()


//************************************************************************
// CApwConfig::GetInstance()
//
// This routine gets NAVAPW's instance handle.
// 
// Returns:
//  HINSTANCE   NAVAPW's instance handle.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

HINSTANCE CApwConfig::GetInstance ()
{
    return ( m_hInst );
} // CApwConfig::GetInstance()

//************************************************************************
// CApwConfig::SetInstance()
//
// This routine saves NAVAPW's instance handle.
// 
// Returns:
//  Nothing.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

VOID CApwConfig::SetInstance ( HINSTANCE hInst )
{
    m_hInst = hInst;
} // CApwConfig::SetInstance()


//************************************************************************
// CApwConfig::GetCanDisable()
//
// This routine returns the setting for whether NAVAPW can disable NAVAP.
//
// Returns:
//  TRUE if NAVAPW should be allowed to disable NAVAP.
//  FALSE otherwise.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

BOOL CApwConfig::GetCanDisable ()
{
    return ( m_bCanDisable );
} // CApwConfig::GetCanDisable()

//************************************************************************
// CApwConfig::SetCanDisable()
//
// This routine saves the setting for whether NAVAPW can disable NAVAP.
//
// Returns:
//  Nothing.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

VOID CApwConfig::SetCanDisable ( BOOL bCanDisable )
{
    m_bCanDisable = bCanDisable;
} // CApwConfig::SetCanDisable()


//************************************************************************
// CApwConfig::GetShowIcon()
//
// This routine gets the setting for whether or not to show the NAVAPW icon
// in the tray or when minimized.
// 
// Returns:
//  TRUE    if NAVAPW should show up in the tray and when minimized.
//  FALSE   otherwise.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

BOOL CApwConfig::GetShowIcon()
{
    return ( m_bShowIcon );
} // CApwConfig::GetShowIcon()

//************************************************************************
// CApwConfig::SetShowIcon()
//
// This routine saves the setting for whether or not to show the NAVAPW icon
// in the tray or when minimized.
// 
// Returns:
//  Nothing.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

VOID CApwConfig::SetShowIcon ( BOOL bShowIcon )
{
    m_bShowIcon = bShowIcon;
} // CApwConfig::SetShowIcon()


//************************************************************************
// CApwConfig::GetDoingShutdown()
//
// This routine returns whether NAVAPW is currently initiating a 
// shutdown of Windows.
// 
// Returns:
//  TRUE    if NAVAPW is shutting down Windows.
//  FALSE   otherwise.
//************************************************************************
// 06/01/1997 DALLEE, created.
//************************************************************************

BOOL CApwConfig::GetDoingShutdown()
{
    return ( m_bDoingShutdown );
} // CApwConfig::GetDoingShutdown()

//************************************************************************
// CApwConfig::SetDoingShutdown()
//
// This routine should be called when initiating a shutdown of
// Windows.
// 
// Returns:
//  Nothing.
//************************************************************************
// 06/01/1997 DALLEE, created.
//************************************************************************

VOID CApwConfig::SetDoingShutdown ( BOOL bDoingShutdown )
{
    m_bDoingShutdown = bDoingShutdown;
} // CApwConfig::SetDoingShutdown()


//************************************************************************
// CApwConfig::GetCustomMessageID()
//
// This routine returns the custom message ID used for communication
// between NAVW and NAVAPW.
// Note that there is no SetCustomMessageID() fuction. The custom
// message is registered as part of CApwConfig() initialization.
// 
// Returns:
//      UINT    ID of custom message for communication with NAVW.
//************************************************************************
// 06/09/1997 DALLEE, created.
//************************************************************************

UINT CApwConfig::GetCustomMessageID()
{
    return ( m_uCustomMessageID );
} // CApwConfig::GetCustomMessageID()


//************************************************************************
// CApwConfig::GetState()
//
// This routine gets the state of the NAVAPW.
// 
// Returns:
//  AP_STATE_ENABLED       if enabled.
//  AP_STATE_DISABLED      if disabled.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

AP_STATE CApwConfig::GetState()
{
    return ( m_nState );
} // CApwConfig::GetState()

//************************************************************************
// CApwConfig::SetState()
//
// VOID SetState (
//      AP_STATE nState );              Current state - must be one of:
//                                       AP_STATE_ENABLED
//                                       AP_STATE_DISABLED
//                                       AP_STATE_LOADING
//                                       AP_STATE_ERROR
//
// This routine saves the state of NAVAPW.
// 
// Returns:
//  Nothing.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

VOID CApwConfig::SetState ( AP_STATE nState )
{
    switch ( nState )
    {
    case AP_STATE_ENABLED:
    case AP_STATE_DISABLED:
    case AP_STATE_LOADING:
    case AP_STATE_ERROR:
        m_nState = nState;
        break;

    default:
        SYM_ASSERT( FALSE );
        break;
    }
} // CApwConfig::SetState()


//************************************************************************
// CApwConfig::GetIcon()
//
// This routine gets the current large icon for NAVAPW.
// 
// Returns:
//  HICON   Current icon for NAVAPW, either enabled or disabled.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

HICON CApwConfig::GetIcon()
{
    return ( m_hIcon );
} // CApwConfig::GetIcon()

//************************************************************************
// CApwConfig::SetIcon()
//
// This routine saves the current large icon for NAVAPW.
// 
// Returns:
//  Nothing.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

VOID CApwConfig::SetIcon ( HICON hIcon )
{
    m_hIcon = hIcon;
} // CApwConfig::SetIcon()


//************************************************************************
// CApwConfig::GetSmallIcon()
//
// This routine gets the current small icon for NAVAPW.
// 
// Returns:
//  HICON   Current small icon for NAVAPW, either enabled or disabled.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

HICON CApwConfig::GetSmallIcon()
{
    return ( m_hSmallIcon );
} // CApwConfig::GetSmallIcon()

//************************************************************************
// CApwConfig::SetSmallIcon()
//
// This routine saves the current small icon for NAVAPW.
// 
// Returns:
//  Nothing.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

VOID CApwConfig::SetSmallIcon ( HICON hSmallIcon )
{
    m_hSmallIcon = hSmallIcon;
} // CApwConfig::SetSmallIcon()


//************************************************************************
// CApwConfig::GetInTray()
//
// This routine gets whether or not NAVAPW's icon is currently
// in the tray.
// 
// Returns:
//  TRUE    if NAVAPW's is in the tray.
//  FALSE   otherwise.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

BOOL CApwConfig::GetInTray()
{
    return ( m_bInTray );
} // CApwConfig::GetInTray()

//************************************************************************
// CApwConfig::SetInTray()
//
// This routine saves whether or not NAVAPW's icon is in the tray.
// 
// Returns:
//  Nothing.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

VOID CApwConfig::SetInTray ( BOOL bInTray )
{
    m_bInTray = bInTray;
} // CApwConfig::SetInTray()


//************************************************************************
// CApwConfig::GetTrayState()
//
// This routine gets the state of the NAVAPW icon in the tray.
// 
// Returns:
//  AP_STATE_ENABLED       if enabled.
//  AP_STATE_DISABLED      if disabled.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

AP_STATE CApwConfig::GetTrayState()
{
    return ( m_nTrayState );
} // CApwConfig::GetTrayState()

//************************************************************************
// CApwConfig::SetTrayState()
//
// VOID SetTrayState (
//      AP_STATE nTrayState );           Tray state - must be one of:
//                                        AP_STATE_ENABLED:
//                                        AP_STATE_DISABLED
//                                        AP_STATE_LOADING:
//                                        AP_STATE_ERROR:
//
// This routine saves the state of the NAVAPW icon in the tray.
// 
// Returns:
//  Nothing.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

VOID CApwConfig::SetTrayState ( AP_STATE nTrayState )
{
    switch ( nTrayState )
    {
    case AP_STATE_ENABLED:
    case AP_STATE_DISABLED:
    case AP_STATE_LOADING:
    case AP_STATE_ERROR:
        m_nTrayState = nTrayState;
        break;

    default:
        SYM_ASSERT( FALSE );
        break;
    }
} // CApwConfig::SetTrayState()


//************************************************************************
// CApwConfig::GetCmd()
//
// This routine gets the AP command class used for issuing 
// commands to the driver.
// NAVAPW is responsible for first obtaining the class from
// APWCMD.DLL ApwCmdNew() and saving it with CApwConfig::SetCmd().
// 
// Returns:
//  CApwCmd   Current command class.
//************************************************************************
// 05/31/1997 DALLEE, created.
//************************************************************************

CApwCmd * CApwConfig::GetCmd()
{
    return ( m_pCmd );
} // CApwConfig::GetCmd()

//************************************************************************
// CApwConfig::SetCmd()
//
// This routine saves the current command class implementation obtained
// from APWCMD.DLL ApwCmdNew().
//
// Note that when destroyed, CApwConfig will not delete the saved command
// class. Whoever sets the command class is responsible for its cleanup.
// 
// Returns:
//  Nothing.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

VOID CApwConfig::SetCmd ( CApwCmd *pCmd )
{
    m_pCmd = pCmd;
} // CApwConfig::SetCmd()


//************************************************************************
// CApwConfig::ReadNavopts()
//
// This routine loads the relevent auto protect options.
//
// Returns:
//  TRUE    on success.
//  FALSE   otherwise.
//************************************************************************
// 05/31/1997 DALLEE, created.
// 05/06/2002 mmendon, updated to use savrt options
//************************************************************************

BOOL CApwConfig::ReadNavopts()
{
	auto	DWORD	   dwValue;
	auto SAVRT_OPTS_STATUS status;

	// Load the SAVRT specific options 
    CAutoProtectOptions SAVRTOptions;
    status = SAVRTOptions.Load ();
	if ( status != SAVRT_OPTS_OK )
	{
		// set default options and return ok
		SetCanDisable( TRUE );
		SetShowIcon  ( TRUE );
		m_bScanFloppyOnShutdown = TRUE;
        CCTRACEE(_T("CApwConfig::ReadNavopts() - Failed to load SAVRT options. Error = %d"), status);
	}
	else
	{
	    // Store options relevent to NAVAPW's UI.
		SAVRTOptions.GetDwordValue( TSR_Unloadable ,&dwValue, TRUE );
		SetCanDisable( dwValue != 0 );

		SAVRTOptions.GetDwordValue( TSR_HideIcon, &dwValue, FALSE );
		SetShowIcon  ( !dwValue );
	
		SAVRTOptions.GetDwordValue( TSR_ChkFlopOnBoot, &dwValue, TRUE );
		m_bScanFloppyOnShutdown = dwValue != 0;
	}
	return ( TRUE );
} // CApwConfig::ReadNavopts()


//************************************************************************
// CApwConfig::GetScanFloppyOnShutdown()
//
// Returns TRUE if we are to scan floppies on shutdown.
//
// Returns:
//  TRUE    
//  FALSE   
//************************************************************************
// 01/10/2001 DBUCHES, created.
//************************************************************************
BOOL CApwConfig::GetScanFloppyOnShutdown()
{
	return m_bScanFloppyOnShutdown;
}

//************************************************************************
// ApwGetConfig()
//
// APWUTILAPI CApwConfig * ApwGetConfig ( VOID );
//
// This routine returns a pointer to NAVAPW's internal configuration object.
//
// Returns:
//  Pointer to global configuration object CApwConfig.
//************************************************************************
// 05/19/1997 DALLEE, created.
//************************************************************************

APWUTILAPI CApwConfig * WINAPI ApwGetConfig ( VOID )
{
    return ( &s_ApwConfig );
} // ApwGetConfig()
