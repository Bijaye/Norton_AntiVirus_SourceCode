////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SymHTML.h>
#include <SymHTMLWindow.h>
#include <SymHTMLDocumentImpl.h>
#include <AVInterfaces.h>
#include <NAVSettingsHelperEx.h>
#include <resource.h>
#include "TabSummaryDocument.h"
#include "TabAttentionDocument.h"
#include "TabDetailsDocument.h"
#include "ProcessAllThread.h"

// {D17958C4-CAD8-4e55-8DEC-3E36E8B163C9}
SYM_DEFINE_INTERFACE_ID(IID_IFrameDocument,
                        0xd17958c4, 0xcad8, 0x4e55, 0x8d, 0xec, 0x3e, 0x36, 0xe8, 0xb1, 0x63, 0xc9);

#define FRAME_MIN_WIDTH 610
#define FRAME_MIN_HEIGHT 415

// CFrameDocumet
class CFrameDocument :
    public symhtml::CSymHTMLDocumentImpl,
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public ccLib::CThread
{
public:
    DECLARE_HTML_DOCUMENT(L"FRAME-DOCUMENT.HTML")

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument)
        SYM_INTERFACE_ENTRY(IID_IFrameDocument, symhtml::ISymHTMLDocument)
    SYM_INTERFACE_MAP_END()

    BEGIN_MSG_MAP(CFrameDocument)
    END_MSG_MAP()

    BEGIN_HTML_EVENT_MAP(CFrameDocument)
        HTML_EVENT_BUTTON_CLICK(L"pause", OnPause)
        HTML_EVENT_BUTTON_CLICK(L"stop", OnStop)
        HTML_EVENT_BUTTON_CLICK(L"next", OnNext)
        HTML_EVENT_BUTTON_CLICK(L"apply", OnApply)
        HTML_EVENT_BUTTON_CLICK(L"finish", OnFinish)
        HTML_EVENT_HYPERLINK(L"export", OnExport)
        HTML_EVENT_HYPERLINK(L"helpLink", OnHelp)
        HTML_EVENT_ELEMENT_EXPANDED(L"tabsummary", OnTabClicked)
        HTML_EVENT_ELEMENT_EXPANDED(L"tabattention", OnTabClicked)
        HTML_EVENT_ELEMENT_EXPANDED(L"tabdetails", OnTabClicked)
    END_HTML_EVENT_MAP()

public:
    CFrameDocument(AVModule::IAVScanBase* pScanner, bool bResultsMode = false);
    virtual ~CFrameDocument(void);
    virtual void FinalRelease() throw();

    // control methods
    void StartProgress();
    void StopProgress();
    void ScanDone();
    void ProcessingMode(DWORD dwCount);
    void ThreatProcessed();
    void SetStatus(LPCWSTR pszStatus);
    void SetCurFile(LPCWSTR pszFile);
    void RefreshScanStats();
    void RefreshScanStats(const avScanUI::ScanStatistics& scanStats);
    void SetThreatTracker(CThreatTracker* pThreatTracker);
    void RefreshThreatList();
    HRESULT CloseDocument();
    HRESULT SwitchToTab(LPCSTR pszTabID);
    bool GetScanAborted() { return m_bScanAborted; }
    void LastCallback();

    // Event handlers
    HRESULT OnPause(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled);
    HRESULT OnStop(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled);
    HRESULT OnNext(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled);
    HRESULT OnApply(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled);
    HRESULT OnFinish(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled);
    HRESULT OnExport(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled);
    HRESULT OnHelp(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled);
    HRESULT OnTabClicked(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled);
    virtual HRESULT OnDocumentComplete() throw();
    virtual HRESULT OnNewIFrame(symhtml::ISymHTMLElement* lpElement, symhtml::ISymHTMLWindow* lpNewFrame) throw();

    // Window message handlers
    LRESULT OnSysClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSysMinimize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSysRestore(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
    void RealRefreshThreatList();
    void ClearProgress();
	void GenerateNavLogEntry();
    HRESULT UpdateTabButtons();
    HRESULT UpdateTabButtons(const avScanUI::ScanStatistics& scanStats);
    bool PromptIfProcessing();
    bool PromptIfScanRunning();
    bool PromptIfThreatsRemain();
    bool PromptForReboot();
    bool ThreatsRemain();
    HRESULT SetElementSelected(LPCSTR pszElementID, bool bSelected);
    HRESULT UpdateStatusIconAndText(int nDetected, int nReqAttn);
    HRESULT SetElementHidden(LPCSTR szID, BOOL bDisplay, BOOL bRenderNow = FALSE);

    // CThread methods
    virtual int Run() throw();

protected:
    symhtml::ISymHTMLWindowPtr m_spSummaryIFrame;
    symhtml::ISymHTMLWindowPtr m_spAttentionIFrame;
    symhtml::ISymHTMLWindowPtr m_spDetailsIFrame;
    bool m_bReady;
    bool m_bPaused;
    bool m_bProgressRunning;
    CSymPtr<CThreatTracker> m_spThreatTracker;
    CProcessAllThread m_thdProcessAll;
    CStringA m_sCurTab;     // explicit ANSI (for now)
    CStringW m_sLastSaveFile;
    bool m_bScanDone;
    bool m_bInMinimize;
    bool m_bProcessing;
    DWORD m_dwToProcessCount;
    DWORD m_dwProcessed;
    bool m_bResultsMode;
    bool m_bRebootOnExit;
    bool m_bScanAborted;
    bool m_bExport;
    bool m_bTListRefreshNeeded;
    bool m_bExitOnComplete;

    // NAVOpts
	CNAVOptSettingsEx m_NavOpts;

    // Scanner interface
    AVModule::IAVScanBasePtr m_spScanner;
    int m_nPrevPriority;

    // Tab Documents
    CSymPtr<CTabSummaryDocument> m_spDocSummaryTab;
    CSymPtr<CTabAttentionDocument> m_spDocAttentionTab;
    CSymPtr<CTabDetailsDocument> m_spDocDetailsTab;

	// Cached state for SetCurFile() for avoiding redundant UI updates.
	// Remembers the prefix of the set filename.
	// The buffer size somewhat arbitrary.  It just needs to be big enough
	// for common cases, but the bigger it is the slower.
	WCHAR m_szCurFile[100];  

	// Cached state for RefreshScanStats() for avoiding redundant UI updates.
	// Remembers the number of items requiring attention.
	int m_nRequireAttn;

	// Cached state for UpdateStatusIconAndText() for avoiding redundant UI updates.
	// Remembers the overall scan state, which is used to select icons, text, colors, etc.
	typedef enum { None, DoneAborted, DoneClean, DoneFixed, DoneInsecure, Working, WorkingInsecure } StatusState;
	StatusState m_statusState;

	// Cached state for UpdateTabButtons() for avoiding redundant UI updates.
	// Remembers the visibility of various buttons.
	bool m_bApplyButtonVisible;
	bool m_bNextButtonVisible;
	bool m_bDoneButtonsVisible;
};
