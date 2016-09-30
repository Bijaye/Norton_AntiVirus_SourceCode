// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/apwcfg.h_v   1.1   10 Jun 1997 14:26:26   DALLEE  $
//
// Description:
//      Header for NAVAPW configuration.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/apwcfg.h_v  $
// 
//    Rev 1.1   10 Jun 1997 14:26:26   DALLEE
// Added member variable to track registered window message for NAVW comm.
// 
//    Rev 1.0   02 Jun 1997 02:46:50   DALLEE
// Initial revision.
//************************************************************************

#ifndef __APWCFG_H
#define __APWCFG_H

#ifndef __APWUTIL_H
#include "apwutil.h"
#endif

#ifndef __APWCMD_H
#include "apwcmd.h"
#endif


//***********************************************
// CApwConfig class. Global configuration object.

class CApwConfig {
// Constructors
public:
    CApwConfig();

// Operations
public:
    virtual BOOL ReadNavopts();

// Attributes
public:
    virtual CApwCmd *GetCmd();
    virtual VOID SetCmd( CApwCmd *pCmd );

    virtual HWND GetMainWnd();
    virtual VOID SetMainWnd( HWND hMainWnd );

    virtual HINSTANCE GetInstance();
    virtual VOID SetInstance( HINSTANCE hInst );

    virtual BOOL GetCanDisable();
    virtual VOID SetCanDisable( BOOL bCanDisable );

    virtual BOOL GetShowIcon();
    virtual VOID SetShowIcon( BOOL bShowIcon );

    virtual BOOL GetDoingShutdown();
    virtual VOID SetDoingShutdown( BOOL bDoingShutdown );

    virtual UINT GetCustomMessageID();

    virtual AP_STATE GetState();
    virtual VOID SetState( AP_STATE nState );

    virtual HICON GetIcon();
    virtual VOID SetIcon( HICON hIcon );

    virtual HICON GetSmallIcon();
    virtual VOID SetSmallIcon( HICON hSmallIcon );

    virtual BOOL GetInTray();
    virtual VOID SetInTray( BOOL bInTray );

    virtual AP_STATE GetTrayState();
    virtual VOID SetTrayState( AP_STATE nTrayState );

	virtual BOOL GetScanFloppyOnShutdown();
// Implementation
public:
    virtual ~CApwConfig();

protected:
    HWND        m_hMainWnd;       
    HINSTANCE   m_hInst;

    BOOL        m_bCanDisable;
    BOOL        m_bShowIcon;
    BOOL        m_bDoingShutdown;
    UINT        m_uCustomMessageID;
	BOOL		m_bScanFloppyOnShutdown;

    AP_STATE    m_nState;

    HICON       m_hIcon;
    HICON       m_hSmallIcon;

    // For tracking icon in tray.

    BOOL        m_bInTray;
    AP_STATE    m_nTrayState;

    // For sending commands to NAVAP service and driver.

    CApwCmd     *m_pCmd;
};

#ifdef __cplusplus
extern "C" {
#endif

APWUTILAPI CApwConfig * WINAPI ApwGetConfig ( VOID );

#ifdef __cplusplus
}
#endif

#endif // __APWCFG_H
