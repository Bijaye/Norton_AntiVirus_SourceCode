/******************************************************************************/
// Copyright © 2004 Symantec Corporation.
// ---------------------------------------------------------------------------
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// All rights reserved.
//
// Author:  Chirstopher Brown 11/08/2004
//******************************************************************************/
#pragma once

#include "ScanProgressWnd.h"
#include "ScanRepairRemoveWnd.h"
#include "ManualRemovalWnd.h"
#include "ScanRepairProgressWnd.h"
#include "BaseClientWnd.h"
#include "ScanResultsWnd.h"
#include "ScanEvents.h"

#include "CommonUIInterface.h"

#include "ScriptSafeVerifyTrustCached.h"		// For scripting security

#include "ScanMgrInterface.h"

typedef CWinTraits< WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 
                    WS_EX_APPWINDOW | WS_EX_WINDOWEDGE > CAltFrameWinTraits;

class CMainScanFrame;
typedef CFrameWindowImpl<CMainScanFrame, ATL::CWindow, CAltFrameWinTraits> CSymFrameWindowImpl;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CMainScanFrame : 
    public CSymFrameWindowImpl, 
    public CUpdateUI<CMainScanFrame>,
    public CMessageFilter, 
    public CIdleHandler,
    public ICommonUI,
    public CScanEventsSink,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:

    CMainScanFrame();

    DECLARE_FRAME_WND_CLASS( "Sym_Common_Scan_Window", IDR_MAINFRAME)

    SYM_INTERFACE_MAP_BEGIN()                
        SYM_INTERFACE_ENTRY( IID_ICommonUI, ICommonUI )
    SYM_INTERFACE_MAP_END()  

    //
    // ICommonUI Implementation...
    //
    HRESULT ShowCUIDlg( CommonUIType UIType, 
                        ccEraser::IAnomalyList* pAnomalyList, 
                        ICommonUISink* pSink, 
                        const long lDamageControlItemsRemaining );
    HRESULT SetParentWindow( HWND hWndParent );
    HRESULT SetScanComplete(ccEraser::IAnomalyList* pAnomalyList, 
                            const long lDamageControlItemsRemaining );
    HRESULT SetEmailInfo(LPCTSTR szSubject, LPCTSTR szSender, 
                         LPCTSTR szRecipient);
    HRESULT SetMemoryScanStatus(MemoryScanStatus MemScanStatus);
    HRESULT SetScanCompressedFiles(bool bScanCompressedFiles);
    HRESULT SetScanTime(unsigned long ulScanTime);
    HRESULT SetRepairProgress(bool bRepairProgress);
    HRESULT SetScanNonViralThreats(bool bScanNonViralThreats);
    HRESULT SetSideEffectsEnabled(bool bScanSideEffects);
    HRESULT SetQuickScan(bool bQuickScan);
    HRESULT UpdateRepairProgress(unsigned long nItemsRemaining, 
                                 bool bFailuresOccured, bool& bAbort);
    HRESULT ShowMemTerminationDlg(bool &bTerminate, bool bEnableTimeout);
    HRESULT ShowDetailsDlg(ccEraser::IAnomaly* pAnomaly, HWND hWndParent);

    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnIdle();

    //
    // CScanEventsSink Implementation...
    //
    virtual HRESULT OnAbortScan();
    virtual HRESULT OnResumeScan();
    virtual HRESULT OnPauseScan();
    virtual HRESULT OnHelp( const long lHelpID );
    virtual HRESULT OnReady();
    virtual HRESULT OnFinished();
    virtual HRESULT OnDetails();
    virtual HRESULT OnManualRemoval();
    virtual HRESULT OnRepairRemoveCancel();
    virtual HRESULT OnRepairRemoveApply();
    virtual HRESULT OnGetCurrentFolder(LPTSTR szFolder, long nLength);
    virtual HRESULT OnGetScanFileCount(long& nScanFileCount);
    virtual HRESULT OnGetVirusCount(long& nVirusCount);
    virtual HRESULT OnGetRemovedVirusCount(long& nRemovedVirusCount);
    virtual HRESULT OnGetNonViralCount(long& nNonViralCount);
    virtual HRESULT OnGetRemovedNonViralCount(long& nRemovedNonViralCount);
    virtual HRESULT OnGetAdditionalScan(bool& bAdditionalScan);
    virtual HRESULT OnGetQuarantineCount(long& nQurantineCount);
    virtual HRESULT OnGetAnomalyList( ccEraser::IAnomalyListPtr& pAnomalyList );
    virtual HRESULT OnRepairProgressComplete();
    virtual HRESULT OnGetVirusDefSubscriptionExpired(bool& bVal);
    virtual HRESULT OnGetAppLauncher( IAppLauncher** obj );
    virtual HRESULT OnGetNAVLicenseInfo( INAVLicenseInfo** obj );
    virtual HRESULT OnRiskDetailsDialog( CEZAnomaly & ezAnomaly,
                                         bool bRiskAssessment );
    virtual HRESULT OnIsEmailScan( bool& bVal );
    virtual HRESULT OnGetEmailValues( LPCTSTR* szEmailInfoSubject, LPCTSTR* szEmailInfoSender, LPCTSTR* szEmailInfoRecipient );
    virtual HRESULT OnIsQuickScan( bool& bVal );

    BEGIN_UPDATE_UI_MAP(CMainFrame)
    END_UPDATE_UI_MAP()

    BEGIN_MSG_MAP(CMainFrame)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestory)
        MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo )
        COMMAND_ID_HANDLER( ID_F1_HELP, OnF1Help)
        COMMAND_ID_HANDLER( ID_ALT_KEY, OnAlyKey)
        COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
        COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
        COMMAND_ID_HANDLER(ID_CTRLTAB, OnCtrlTab)
        COMMAND_ID_HANDLER(ID_CTRLSHIFTTAB, OnCtrlTab)
        CHAIN_MSG_MAP(CUpdateUI<CMainScanFrame>)
        CHAIN_MSG_MAP(CSymFrameWindowImpl)
    END_MSG_MAP()

protected:

    CComPtr< IAppLauncher > m_spAppLauncher;
    CComPtr< INAVLicenseInfo > m_spNAVLicenseInfo;
    CComPtr <ISymScriptSafe> m_spSymScriptSafeAppLauncher;
    CComPtr <ISymScriptSafe> m_spSymScriptSafeLicenseInfo;

    CWindow m_wndParent;
    CommonUIType m_UIType;
    MemoryScanStatus m_eMemoryScanStatus;
    bool m_bScanCompressedFile;
    unsigned long m_ulScanTime;
    bool m_bRepairProgress;
    bool m_bScanNonViralThreats;
    bool m_bScanSideEffects;
    bool m_bFailureOccured;
    bool m_bAlertForDefaultExcludeItems;

    SMRESPONSE m_smrResponseMode;

    ccEraser::IAnomalyListPtr m_spAnomalyList;
    CSymPtr< ICommonUISink > m_spCommUISink;

    // Strings used for e-mail type scans...
    bool m_bEmail;
    CString m_strEmailInfoSubject;
    CString m_strEmailInfoSender;
    CString m_strEmailInfoRecipient;

    CScanProgressWnd  m_wndProgressView;
    CScanRepairRemoveWnd  m_wndRepairRemoveView;
    CScanRepairProgressWnd  m_wndScanRepairProgressView;
    CScanResultsWnd  m_wndScanResultsView;
    CManualRemovalWnd  m_wndManualRemovalView;

    long m_lIgnoreOnceCount;

    bool m_bQuickScan;

    // Utility funcs...
    void SetActiveView( const HWND hWnd );

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnDestory(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnCtrlTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnF1Help(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnAlyKey(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

