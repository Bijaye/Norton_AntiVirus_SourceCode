// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "HPPEventsInterface.h"
#include "BBSystemMonitorEventsInterface.h"
#include "BBEventID.h"
#include "SymProtectEventsLoader.h"

#include "HPPEventHelper.h"
#include "EventLogHelper.h"

#include "INITGUIDS.h"

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };
    
    
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
        COMMAND_HANDLER(IDC_BTN_SEND_EVENT_1, BN_CLICKED, OnBnClickedBtnSendEvent1)
        COMMAND_HANDLER(IDC_BTN_CHG_HOME_PAGE, BN_CLICKED, OnBnClickedBtnChgHomePage)
        COMMAND_HANDLER(IDC_BTN_CREATE_LOG, BN_CLICKED, OnBnClickedBtnCreateLog)
        COMMAND_HANDLER(IDC_BTN_READ_LOG, BN_CLICKED, OnBnClickedBtnReadLog)
        COMMAND_HANDLER(IDC_BTN_IE_OPENED, BN_CLICKED, OnBnClickedBtnIeOpened)
        COMMAND_HANDLER(IDC_BTN_CNNCT_BB, BN_CLICKED, OnBnClickedBtnCnnctBb)
        COMMAND_HANDLER(IDC_BTN_DISCNNCT_BB, BN_CLICKED, OnBnClickedBtnDiscnnctBb)
        COMMAND_HANDLER(IDC_BTN_PREP_FOR_UNINSTALL, BN_CLICKED, OnBnClickedBtnPrepForUninstall)
    END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void CloseDialog(int nVal);
    LRESULT OnBnClickedBtnSendEvent1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    
    CHPPEventHelperPtr m_spEvtHelper;

    CRegKeyMonitorWin32 m_regKeyMonitorHKLM;
    CRegKeyMonitorWin32 m_regKeyMonitorHKCU;

    LRESULT OnBnClickedBtnChgHomePage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedBtnCreateLog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedBtnReadLog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    void SetPaneWidths(int* arrWidths, int nPanes);
    void OnFinalInitDialog();


protected:
    HPP::CEventLogHelperT<HPP::HPPEventFactory_CEventFactoryEx> m_EventLog;
    
    CHyperLink m_wndHyperLink;
    CMultiPaneStatusBarCtrl m_wndStatusBar;

public:
    LRESULT OnBnClickedBtnIeOpened(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedBtnCnnctBb(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedBtnDiscnnctBb(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedBtnPrepForUninstall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
