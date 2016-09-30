////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "FrameDocument.h"
#include <resource.h>
#include "LowPriorityPromptDlg.h"
#include <SymHTMLLoader.h>
#include <ccSymModuleLifetimeMgrHelper.h>
#include <OptNames.h>
#include "ScanUIMisc.h"
#include "GetFileName.h"
#include <isSymTheme.h>

// NAV logging
#include <AvProdLoggingLoader.h>

// AvProdService async results logging
#include <AvProdSvcClientLoader.h>

// Help IDs
#include <SymHelp.h>

using namespace symhtml;

#define  TAB_SUMMARY   "tabsummary"
#define  TAB_ATTENTION "tabattention"
#define  TAB_DETAILS   "tabdetails"
#define  BTN_NEXT      "next"
#define  BTN_APPLY     "apply"
#define  BTN_PAUSE     "pause"
#define  BTN_STOP      "stop"
#define  BTN_FINISH    "finish"
#define  BTN_EXPORT    "export"

CFrameDocument::CFrameDocument(AVModule::IAVScanBase* pScanner, bool bResultsMode) :
    m_spScanner(pScanner),
    m_bResultsMode(bResultsMode),
    m_bPaused(false),
    m_bProgressRunning(false),
    m_bScanDone(false),
    m_nPrevPriority(THREAD_PRIORITY_NORMAL),
    m_bInMinimize(false),
    m_dwToProcessCount(0),
    m_bProcessing(false),
    m_bRebootOnExit(false),
    m_sCurTab(TAB_SUMMARY),
    m_bScanAborted(false),
	m_nRequireAttn(0),
	m_statusState(StatusState::None),
	m_bApplyButtonVisible(false),
	m_bNextButtonVisible(false),
	m_bDoneButtonsVisible(false),
    m_bTListRefreshNeeded(false),
    m_bExitOnComplete(false)
{
	m_szCurFile[0] = '\0';

    // Init navopts helper
	if(!m_NavOpts.Init())
	{
		LOG_FAILURE(L"Unable to init NAVOpts helper!", E_FAIL);
	}
}

CFrameDocument::~CFrameDocument(void)
{
    CCTRCTXI0(L"dtor");

    // If we still have a thread, get rid of it
    if(IsThread())
    {
        PostTerminate();
        WaitForExit(INFINITE);
    }

    TRACE_REF_COUNT(m_spSummaryIFrame);
    TRACE_REF_COUNT(m_spAttentionIFrame);
    TRACE_REF_COUNT(m_spDetailsIFrame);
    TRACE_REF_COUNT(m_spThreatTracker);
    TRACE_REF_COUNT(m_spScanner);
    TRACE_REF_COUNT(m_spDocSummaryTab);
    TRACE_REF_COUNT(m_spDocAttentionTab);
    TRACE_REF_COUNT(m_spDocDetailsTab);
}

void CFrameDocument::FinalRelease()
{
    // Initiate the reboot (if requested)
    if(m_bRebootOnExit)
    {
        // Fix our process token
        CAccessToken tokProcess;
        if(tokProcess.GetProcessToken(TOKEN_WRITE | TOKEN_READ))
        {
            if(tokProcess.EnablePrivilege(SE_SHUTDOWN_NAME))
            {
                if(!ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_MINOR_SECURITY))
                    LOG_FAILURE(L"Failed to initiate system reboot", HRESULT_FROM_WIN32(GetLastError()));
            }
            else
                LOG_FAILURE(L"Failed to enable the SE_SHUTDOWN_NAME priviledge for our process!", HRESULT_FROM_WIN32(GetLastError()));
        }
        else
            LOG_FAILURE(L"Failed to get our process token!", HRESULT_FROM_WIN32(GetLastError()));
    }
}

void CFrameDocument::StartProgress()
{
    // Only start the progress when stopped
    if(m_bProgressRunning)
        return;

    // Only if the doc is ready
    if(S_FALSE == IsDocumentComplete())
        return;

    // Start the animation
    symhtml::ISymHTMLElementPtr spProg;
    HRESULT hr = GetElement("progress", &spProg);
    if(SUCCEEDED(hr) && spProg)
    {
        spProg->SetStyleAttribute("transition", L"image");
        spProg->UpdateElement(TRUE);
    }

    // Update the status icon and text
    UpdateStatusIconAndText(0, 0);

    // Enable the pause/stop buttons
    SetElementEnabled(BTN_PAUSE, true, TRUE);
    SetElementEnabled(BTN_STOP, true, TRUE);

    m_bProgressRunning = true;
}

void CFrameDocument::StopProgress()
{
    // Only stop the progress when started
    if(!m_bProgressRunning)
        return;

    // Only if the doc is ready
    if(S_FALSE == IsDocumentComplete())
        return;

    // Stop the animation
    symhtml::ISymHTMLElementPtr spProg;
    HRESULT hr = GetElement("progress", &spProg);
    if(SUCCEEDED(hr) && spProg)
    {
        spProg->SetStyleAttribute("transition", L"none");
        spProg->UpdateElement(TRUE);
    }

    m_bProgressRunning = false;
}

void CFrameDocument::ClearProgress()
{
    if(S_FALSE == IsDocumentComplete())
        return;

    SetCurFile(L" ");
    StopProgress();
}

void CFrameDocument::SetStatus(LPCWSTR pszStatus)
{
    if(S_FALSE == IsDocumentComplete())
        return;

    SetElementText("statustext", pszStatus, -1, TRUE);
}

void CFrameDocument::SetCurFile(LPCWSTR pszFile)
{
    if(S_FALSE == IsDocumentComplete())
        return;

	// If the filename being set is the same as the previously set filename,
	// do nothing.  (If the filename exceeds the m_szCurFile buffer size, 
	// it is treated as if it is a different name.)
	//
	// This seems to be a fairly common case and thus worth testing for,
	// though I think it is happening more often than it should
	// (perhaps there's a bug elsewhere in the scan engine.)
	if (*m_szCurFile == *pszFile && wcscmp(m_szCurFile, pszFile) == 0)
		return;

	// Remember the filename being set for use on the next call.
	wcsncpy(m_szCurFile, pszFile, sizeof(m_szCurFile) / sizeof(WCHAR));
	m_szCurFile[(sizeof(m_szCurFile) / sizeof(WCHAR)) - 1] = '\0';

	// This step seems like it shouldn't be needed, but without it
	// the subsequent call to SetElementText on "currentfile"
	// doesn't always result in a paint of the new filename.
	ISymHTMLElementPtr spElement;
	HRESULT hr = GetElement("statustext", &spElement);
	if( SUCCEEDED(hr) )
		spElement->UpdateElement(TRUE);

    // If the filename has a "\\?\" prefix, remove it.
	// This prefix comes from ccEraser/ccScan.
	if (pszFile[0] == '\\' &&
		pszFile[1] == '\\' &&
		pszFile[2] == '?' &&
		pszFile[3] == '\\')
	{
		pszFile += 4;
	}

	// Update the filename in the HTML.
    SetElementText("currentfile", pszFile, -1, TRUE);
}

void CFrameDocument::RefreshScanStats()
{
    // Get the scan statistics
    avScanUI::IScanUICallbacksIntPtr spScanUI = m_spThreatTracker->GetScanUI();
    if(!spScanUI)
    {
        LOG_FAILURE(L"Can't display stats ... spScanUI = NULL", E_FAIL);
        return;
    }
    avScanUI::ScanStatistics scanStats;
    spScanUI->GetScanStats(scanStats);

	// Pass them to the method that does the work.
	RefreshScanStats(scanStats);
}

void CFrameDocument::RefreshScanStats(const avScanUI::ScanStatistics& scanStats)
{
    // If we haven't yet loaded... we are done here...
    if(S_FALSE == IsDocumentComplete())
        return;

	// Calculate the number of items needing attention.
    int nDetected = scanStats.nVirDetected + scanStats.nNonVirDetected;
    int nRepaired = scanStats.nVirRemoved + scanStats.nNonVirRemoved;
    int nRequireAttn = nDetected - nRepaired;
    CCTRACEI(L"nDetected: %d\tnRepaird: %d\tnAttn: %d", nDetected, nRepaired, nRequireAttn);

	// If the number has changed...
	if (nRequireAttn != m_nRequireAttn)
	{
		// Remember the number for next time
		m_nRequireAttn = nRequireAttn;

		// Get the HTML elements to be updated.
		ISymHTMLElementPtr spElement, spPane;
		HRESULT hr = GetElement("tabattention", &spElement);
		LOG_FAILURE(L"Failed to get the attention tab element", hr);
		hr = GetElement("AttentionBody", &spPane);
		LOG_FAILURE(L"Failed to get the attention pane element", hr);

		// Update the attention tab count/color
		if(nRequireAttn > 0)
		{
			CCTRACEI(L"Setting tab to red");
			// Make it red
			CStringW sText;
			sText.Format(L"(%d)", nRequireAttn);
			hr = ReplaceElementHtml("RAcount", sText);
			LOG_FAILURE(L"Failed to set RAcount text", hr);
			hr = spElement->SetAttributeByName("state", L"alert");
			LOG_FAILURE(L"Failed to set tabattention state attribute", hr);
			hr = spPane->SetAttributeByName("state", L"alert");
			LOG_FAILURE(L"Failed to set AttentionBody state attribute", hr);
		}
		else
		{
			CCTRACEI(L"Setting tab to normal");
			// Normal
			hr = ReplaceElementHtml("RAcount", L"");
			LOG_FAILURE(L"Failed to set RAcount text", hr);
			hr = spElement->SetAttributeByName("state", NULL);
			LOG_FAILURE(L"Failed to delete tabattention state attribute", hr);
			hr = spPane->SetAttributeByName("state", NULL);
			LOG_FAILURE(L"Failed to delete AttentionBody state attribute", hr);
		}
		hr = spElement->UpdateElement(TRUE);
		LOG_FAILURE(L"UpdateElement failed", hr);
	}

    // Update the status icon and text
    UpdateStatusIconAndText(nDetected, nRequireAttn);

    // Update the contents of the summary tab
    if(m_spDocSummaryTab)
        m_spDocSummaryTab->RefreshScanStats(scanStats);

    // Update all the buttons
    UpdateTabButtons();

    // Redraw everything
    symhtml::ISymHTMLElementPtr spRoot;
    GetRootElement(&spRoot);
    if(spRoot)
        spRoot->UpdateElement(TRUE);
}

HRESULT CFrameDocument::UpdateStatusIconAndText(int nDetected, int nReqAttn)
{
	// Compute the new status state.
	StatusState newStatusState;
	if(m_bScanDone)
	{
		if(m_bScanAborted)
			newStatusState = StatusState::DoneAborted;
		else if(nDetected == 0)
			newStatusState = StatusState::DoneClean;
		else if(nReqAttn == 0)
			newStatusState = StatusState::DoneFixed;
		else 
			newStatusState = StatusState::DoneInsecure;
	}
	else
	{
		if(nReqAttn == 0)
			newStatusState = StatusState::Working;
		else
			newStatusState = StatusState::WorkingInsecure;
	}

	// Was the old status in a "done" state?
	bool bOldScanDone = false;
	switch(m_statusState)
	{
	    case StatusState::DoneAborted:
	    case StatusState::DoneClean:
	    case StatusState::DoneFixed:
	    case StatusState::DoneInsecure:
		    bOldScanDone = true;
		    break;
	}

	// If the state has not changed, don't recalculate or paint any UI.
	// This should be a common case.
	if(newStatusState == m_statusState)
		return S_OK;

	// Remember the new state for next time.
	m_statusState = newStatusState;

	// If the "done" state has changed...
	if(m_bScanDone != bOldScanDone)
	{
		// Show/hide the proper blocks
		SetElementHidden("iconarea", !m_bScanDone);
		SetElementHidden("statustextandprogress", !m_bScanDone);
		SetElementHidden("iconareadone", m_bScanDone);
	}

	// Get the elements to update.

    ISymHTMLElementPtr spTextArea;
	HRESULT hr = GetElement(m_bScanDone ? "icontextareadone" : "icontextarea", &spTextArea);
	LOG_FAILURE_AND_RETURN(L"Failed to get the icontextarea element", hr);

    ISymHTMLElementPtr spIcon;
	hr = GetElement(m_bScanDone ? "activityicondone" : "activityicon", &spIcon);
	LOG_FAILURE_AND_RETURN(L"Failed to get the activityicon element", hr);

	LPCSTR szStatusTextArea = m_bScanDone ? "iconstatustextdone" : "iconstatustext";

	// Get the text, icon, and colors needed.
    CStringW sTextClass, sText, sIcon;
	switch(newStatusState)
	{
	    case StatusState::DoneAborted:
		    sTextClass = L"GrayText";
		    sText.LoadString(IDS_ABORTED);
		    sIcon = L"url(icon_gray_stopped_1.png)";
		    break;

	    case StatusState::DoneClean:
            sTextClass = L"GreenText";
            sText.LoadString(IDS_DONE_CLEAN);
            sIcon = L"url(icon_green_secure.png)";
		    break;

	    case StatusState::DoneFixed:
            sTextClass = L"GreenText";
            sText.LoadString(IDS_DONE_FIXED);
            sIcon = L"url(icon_green_secure.png)";
		    break;

	    case StatusState::DoneInsecure:
            sTextClass = L"RedText";
            sText.LoadString(IDS_DONE_ATTN);
            sIcon = L"url(icon_red_notsecure.png)";
		    break;

	    case StatusState::Working:
            sTextClass = L"BlueText";
            sText.LoadString(IDS_WORKING);
            sIcon = L"url(icon_blue_working_1.png)";
		    break;

	    case StatusState::WorkingInsecure:
            sTextClass = L"OrangeText";
            sText.LoadString(IDS_WORKING);
            sIcon = L"url(icon_orange_attn_req.png)";
		    break;

	    default:
		    LOG_FAILURE_AND_RETURN(L"Invalid StatusState", E_FAIL);
    }

    // Change the icon text area color
    hr = spTextArea->SetAttributeByName("class", sTextClass);
    LOG_FAILURE(L"Failed to set the icontextarea class attribute", hr);

    // Change the icon
    hr = spIcon->SetStyleAttribute("foreground-image", sIcon);
    LOG_FAILURE(L"Failed to set the activityicon foreground-image style", hr);

    // Change the text
	hr = SetElementText(szStatusTextArea, sText, -1, TRUE);
    LOG_FAILURE(L"Failed to set the iconstatustext content", hr);

    return hr;
}

void CFrameDocument::SetThreatTracker(CThreatTracker* pThreatTracker)
{
    m_spThreatTracker = pThreatTracker;

    // Pass on the threat tracker
    if(m_spDocDetailsTab)
        m_spDocDetailsTab->SetThreatTracker(pThreatTracker);
    if(m_spDocAttentionTab)
        m_spDocAttentionTab->SetThreatTracker(pThreatTracker);
    if(m_spDocSummaryTab)
        m_spDocSummaryTab->SetThreatTracker(pThreatTracker);
}

void CFrameDocument::RefreshThreatList()
{
    if(IsThread())
    {
        // Queue the refresh (picked up every second)
        m_bTListRefreshNeeded = true;
    }
    else
    {
        // Our thread is missing, refresh here
        RealRefreshThreatList();
    }
}

void CFrameDocument::RealRefreshThreatList()
{
    // Pass the control on...
    if(m_spDocDetailsTab)
        m_spDocDetailsTab->RefreshThreatList();
    if(m_spDocAttentionTab)
        m_spDocAttentionTab->RefreshThreatList();
    if(m_spDocSummaryTab)
        m_spDocSummaryTab->RefreshThreatList();
}

void CFrameDocument::ScanDone()
{
    // Always show the 100% progress bar
    SetElementHidden("progress", FALSE, TRUE);
    SetElementHidden("TPprogress", FALSE, TRUE);

    // Force the processing progress bar to 100% (just in case)
    if(m_bProcessing)
    {
        ISymHTMLElementPtr spProg;
        HRESULT hr = GetElement("TPprogress", &spProg);
        LOG_FAILURE(L"Failed to get TPprogress element", hr);
        if(spProg)
        {
            CStringW sValue;
            sValue.Format(L"%d", m_dwToProcessCount);
            spProg->SetAttributeByName("value", sValue);
            spProg->UpdateElement(TRUE);
        }
    }

    // Reset some vars
    m_bProcessing = false;
    m_bScanDone = true;
    ClearProgress();

    // Update all the buttons
    UpdateTabButtons();
}

void CFrameDocument::LastCallback()
{
    // Redraw everything
    symhtml::ISymHTMLElementPtr spRoot;
    GetRootElement(&spRoot);
    if(spRoot)
        spRoot->UpdateElement(TRUE);

    // Check if we were closed while scanning...
    if(m_bExitOnComplete)
    {
        CCTRCTXI0(L"Scan is done... exiting");
        // Close our document
        HRESULT hr = CloseDocument();
        LOG_FAILURE(L"Failed to close document", hr);
    }
}

void CFrameDocument::ProcessingMode(DWORD dwCount)
{
    m_bProcessing = true;
    m_dwProcessed = -1;     // First call to ThreatProcessed() - below - will set this to 0
    m_dwToProcessCount = dwCount;
    SetElementHidden("progress", FALSE, TRUE);
    StopProgress();
    SetElementHidden("TPprogress", TRUE, TRUE);
    ISymHTMLElementPtr spProg;
    HRESULT hr = GetElement("TPprogress", &spProg);
    LOG_FAILURE(L"Failed to get TPprogress element", hr);
    if(spProg)
    {
        CStringW sValue;
        sValue.Format(L"%d", m_dwToProcessCount);
        spProg->SetAttributeByName("maxvalue", sValue);
        spProg->UpdateElement(TRUE);
    }

    // Disable the pause/stop buttons
    SetElementEnabled("pause", FALSE, TRUE);
    SetElementEnabled("stop", FALSE, TRUE);

    // Update the text and value
    ThreatProcessed();
}

void CFrameDocument::ThreatProcessed()
{
    // Increment the count of processed threats
    ++m_dwProcessed;

    // Update the progress bar...
    ISymHTMLElementPtr spProg;
    HRESULT hr = GetElement("TPprogress", &spProg);
    LOG_FAILURE(L"Failed to get TPprogress element", hr);
    if(spProg)
    {
        CStringW sValue;
        sValue.Format(L"%d", m_dwProcessed);
        spProg->SetAttributeByName("value", sValue);
        spProg->UpdateElement(TRUE);
    }

    // Update the text
    CStringW sFmt;
    DWORD dwRemain = m_dwToProcessCount - m_dwProcessed;
    if(dwRemain == 1)
        sFmt.LoadString(IDS_THREATREMAINING);
    else
        sFmt.LoadString(IDS_THREATSREMAINING);

    CStringW sFileText;
    sFileText.Format(sFmt, dwRemain);
    SetCurFile(sFileText);
}

LRESULT CFrameDocument::OnSysClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    if(!PromptIfProcessing() || !PromptIfScanRunning() || !PromptIfThreatsRemain())
    {
        bHandled = TRUE;
        return 0;
    }

    // If the scan is done, we can exit now...
    // otherwise, we need to set a flag to exit when ScanDone() is called
    if(m_bScanDone)
	{
        // Get our thread to start exiting...
        PostTerminate();

		// Generate a log entry
		GenerateNavLogEntry();
		
		// prompt for reboot (if needed)
		PromptForReboot();

        // Don't eat this message
        bHandled = FALSE;
	}
    else
    {
        CCTRCTXI0(L"Waiting for OnScanComplete() before closing window");
        bHandled = TRUE;
        m_bExitOnComplete = true;
    }

    return 0;
}

LRESULT CFrameDocument::OnSysMinimize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    // We never block the defwindowproc from handling this
    bHandled = FALSE;

    // If the scan is done, we don't need to prompt
    if(m_bScanDone)
        return 0;

    // While we are prompting if the user clicks on the taskbar icon
    // this method gets called again (see incident# 609110)
    if(m_bInMinimize)
    {
        // On reentry, eat this message
        bHandled = TRUE;
        return 0;
    }
    
    // When we are in safe mode, we don't need to worry
    // about running in the background
    if(GetSystemMetrics(SM_CLEANBOOT))
    {
        return 0;
    }

    // Save the current priority
    m_spScanner->GetPriority(m_nPrevPriority);

    // Prompt the user
    symhtml::ISymHTMLDialogPtr spDialog;
    if(SYM_SUCCEEDED(symhtml::loader::ISymHTMLDialog::CreateObject(GETMODULEMGR(), spDialog)))
    {
        // Create our prompt dialog
        CSymPtr<CLowPriorityPromptDlg> spDlgDoc = new CLowPriorityPromptDlg();
        if(!spDlgDoc)
        {
            CCTRACEE(CCTRCTX L"new of CExportProgressDlg failed!");
            return false;
        }

        // Get our window handle
        HWND hWnd;
        GetDocHwnd(hWnd);

        // Check for a prior response...
		DWORD dwValue = 0;
		m_NavOpts.GetValue(SCANNER_BackgroundScanWhenMin, dwValue, 0);

        int nResult = 0;
        if(dwValue == 0)
        {
            // Display the prompt UI
            HRESULT hr = S_OK;
            if(!m_bInMinimize)
            {
                m_bInMinimize = true;
                hr = spDialog->DoModal(hWnd, spDlgDoc, &nResult);
                m_bInMinimize = false;
            }

            if(FAILED(hr))
            {
                CCTRACEE(CCTRCTX L"DoModal failed (hr=%08X)", hr);
                return 0;
            }
        }
        else
            nResult = dwValue;

        // If the user answered with yes, then lower the priority
        if(nResult == IDYES)
            m_spScanner->SetPriority(THREAD_PRIORITY_LOWEST);

        // If the user wanted us to save the response... do so
        if(spDlgDoc->GetRemember() && nResult != 0)
        {
    		m_NavOpts.SetValue(SCANNER_BackgroundScanWhenMin, (DWORD)nResult);
        }
    }

    return 0;
}

LRESULT CFrameDocument::OnSysRestore(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    // We never block the defwindowproc from handling this
    bHandled = FALSE;

    // Restore the scan priority
    m_spScanner->SetPriority(m_nPrevPriority);

    return 0;
}

HRESULT CFrameDocument::OnPause(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
    CAtlString sBtnText;
    if(m_bPaused)
    {
        if(SUCCEEDED(m_spScanner->Resume()))
        {
            m_bPaused = false;
            sBtnText.LoadString(IDS_PAUSE);
        }
    }
    else
    {
        if(SUCCEEDED(m_spScanner->Pause()))
        {
            m_bPaused = true;
            sBtnText.LoadString(IDS_RESUME);
        }
    }

    // Update the button text, if needed
    if(!sBtnText.IsEmpty())
    {
        SetElementText(BTN_PAUSE, sBtnText, sBtnText.GetLength(), TRUE);
        
        symhtml::ISymHTMLElementPtr spElem;
        if(SUCCEEDED(GetElement(BTN_PAUSE, &spElem)) && spElem)
            spElem->SetAttributeByName("title", sBtnText);
    }

    return S_OK;
}

HRESULT CFrameDocument::OnStop(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
    HRESULT hr = S_OK;

    // Get our HWND
    HWND hWnd;
    GetDocHwnd(hWnd);

    // Pause before the prompt
    AVModule::ManualScanner::eMSState eState;
    hr = m_spScanner->GetCurrentState(eState);
    LOG_FAILURE(L"Failed to get current scan state!", hr);
    hr = m_spScanner->Pause();
    LOG_FAILURE(L"Failed to pause scan!", hr);

    // Make sure the user wants to stop the scan
    CStringW sMsg, sTitle;
    sMsg.LoadString(IDS_USERSTOPSCAN_PROMPT);
    sTitle = CISVersion::GetProductName();
    if(IDYES == MessageBox(hWnd, sMsg, sTitle, MB_YESNO))
    {
        // Stop the active scan...
        hr = m_spScanner->Abort();
        LOG_FAILURE(L"Abort call failed", hr);
        if(SUCCEEDED(hr))
        {
            m_bScanAborted = true;

            // Disable the pause and stop buttons
            SetElementEnabled(BTN_PAUSE, false, TRUE);
            SetElementEnabled(BTN_STOP, false, TRUE);
        }

        return S_OK;
    }

    // Restart the scan if we paused it
    if(eState != AVModule::ManualScanner::eMSState_Paused
        && eState != AVModule::ManualScanner::eMSState_Pausing)
    {
        hr = m_spScanner->Resume();
        LOG_FAILURE(L"Failed to resume scan!", hr);
    }

    return S_OK;
}

HRESULT CFrameDocument::OnFinish(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
    HRESULT hr = S_OK;

    if(PromptIfThreatsRemain() && PromptForReboot())
    {
        // Close our document
        hr = CloseDocument();
    }

    return hr;
}

bool CFrameDocument::ThreatsRemain()
{
    // Get the stats
    avScanUI::IScanUICallbacksIntPtr spScanUI = m_spThreatTracker->GetScanUI();
    if(!spScanUI)
    {
        LOG_FAILURE(L"Can't get stats ... spScanUI = NULL", E_FAIL);
        return false;
    }

    avScanUI::ScanStatistics scanStats;
    spScanUI->GetScanStats(scanStats);

    // Check for items remaining
    int nDetected = scanStats.nVirDetected + scanStats.nNonVirDetected;
    int nRepaired = scanStats.nVirRemoved + scanStats.nNonVirRemoved;
    if(nRepaired < nDetected)
        return true;

    return false;
}

HRESULT CFrameDocument::OnNext(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
    HRESULT hr = S_OK;
    if(m_sCurTab == TAB_SUMMARY)
    {
        // Do we have items that need attention?
        if(ThreatsRemain())
            hr = SwitchToTab(TAB_ATTENTION);
        else
            hr = SwitchToTab(TAB_DETAILS);
    }
    else if(m_sCurTab == TAB_ATTENTION)
    {
        hr = SwitchToTab(TAB_DETAILS);
    }
    else if(m_sCurTab == TAB_DETAILS)
    {
        CCTRACEE(CCTRCTX L"No next button on the details page!");
        ATLASSERT(FALSE);
    }

    return hr;
}

HRESULT CFrameDocument::OnApply(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
    HWND hWnd;
    GetDocHwnd(hWnd);

    // Create the process all thread
    m_thdProcessAll.ApplyAllThreatActions(hWnd, m_spThreatTracker);

    return S_OK;
}

UINT_PTR CALLBACK OFNFileExistsHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR pHdr = (LPNMHDR)lParam;
    if(uiMsg == WM_NOTIFY && !IsBadReadPtr(pHdr, sizeof(NMHDR)))
    {
        LPOFNOTIFY pNotify = (LPOFNOTIFY)lParam;
        if(pHdr->code == CDN_FILEOK
            && !IsBadReadPtr(pNotify, sizeof(OFNOTIFY))
            && !IsBadReadPtr(pNotify->lpOFN, sizeof(OPENFILENAME)))
        {
            //
            // Make sure this filename + extension, doesn't already exist
            //
            CString sFilename = pNotify->lpOFN->lpstrFile;
            bool bFilenameFixed = false;

            // Ensure we have a valid extension
            if(sFilename.Right(4).CompareNoCase(L".txt") != 0
                && sFilename.Right(4).CompareNoCase(L".xml") != 0)
            {
                bFilenameFixed = true;
                switch(pNotify->lpOFN->nFilterIndex)
                {
                    case 1:
                        sFilename += L".txt";
                        break;
                    case 2:
                        sFilename += L".xml";
                        break;
                }
            }

            // Can't have more than MAX_PATH
            if(sFilename.GetLength() >= (MAX_PATH - 1))
            {
                // Notify the user that their filename is too long
                CString sPrompt;
                sPrompt.LoadString(IDS_FILENAME_TOOLONG);
                MessageBox(hdlg, sPrompt, pNotify->lpOFN->lpstrTitle, MB_OK | MB_ICONEXCLAMATION);

                // Filename is too long... stop the operation
                SetWindowLong(hdlg, DWL_MSGRESULT, 1);
                return 1;                
            }

            // Copy the full filename back
            _tcsncpy(pNotify->lpOFN->lpstrFile, sFilename, pNotify->lpOFN->nMaxFile - 1);

            // Check if the file exists
            DWORD dwParams = GetFileAttributes(sFilename);
            if(bFilenameFixed && dwParams != INVALID_FILE_ATTRIBUTES)
            {
                // prompt
                CString sPromptFmt, sPrompt;
                sPromptFmt.LoadString(IDS_EXISTS);
                sPrompt.Format(sPromptFmt, sFilename.GetBuffer());
                int nRes = MessageBox(hdlg, sPrompt, pNotify->lpOFN->lpstrTitle, MB_YESNO | MB_ICONEXCLAMATION);
                if(nRes == IDNO)
                {
                    // User doesn't want to overwrite ... we need to stop the OK operation
                    SetWindowLong(hdlg, DWL_MSGRESULT, 1);
                    return 1;
                }
                
                // Fall through if the user selected yes
            }

            // Fall through if file doesn't exist
        }
    }

    // Don't handle anything else...
    return 0;
}


HRESULT CFrameDocument::OnExport(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
    // Get the docs HWND
    HWND hwndRoot;
    HRESULT hr = GetDocHwnd(hwndRoot);
    if(FAILED(hr))
        return hr;

    // Load our two strings for the save dialog
    CStringW sTitle, sFilter;
    sTitle.LoadString(IDS_EXPORT_TITLE);
    sFilter.LoadString(IDS_EXPORT_FILTER);

    // Determine the starting directory
    CStringW sInitialDir;
    if(m_sLastSaveFile.IsEmpty())
    {
        // Try the "My Documents" folder...
        HRESULT hr = SHGetFolderPath(hwndRoot, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, sInitialDir.GetBuffer(1024));
        sInitialDir.ReleaseBuffer();
        if(FAILED(hr))
        {
            LOG_FAILURE(L"Failed to get My Documents folder ... trying desktop", hr);

            // Desktop is next best...
            hr = SHGetFolderPath(hwndRoot, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, sInitialDir.GetBuffer(1024));
            sInitialDir.ReleaseBuffer();
            LOG_FAILURE(L"Failed to get desktop folder...", hr);
        }
    }
    else
    {
        int nLastSlash = m_sLastSaveFile.ReverseFind(L'\\');
        if(nLastSlash != -1)
            sInitialDir = m_sLastSaveFile.Left(nLastSlash);
        else
            sInitialDir = m_sLastSaveFile;
    }


	// Since we can't save a string with '\0' embedded we use the hash (#)
	// as a place holder and replace it with the nils.
    sFilter.Replace(L'#', L'\0');

    // Have the user pick the export location/file
    CStringW sBuffer;
    CGetFileName gfn;
    gfn.hwndOwner = hwndRoot;
    gfn.lpstrFilter = sFilter;
    gfn.lpstrFile = sBuffer.GetBuffer(8192);
    gfn.nMaxFile = 8192;
    gfn.lpstrTitle = sTitle;
    gfn.lpstrInitialDir = sInitialDir;
    gfn.Flags = OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_ENABLEHOOK;
    gfn.lpfnHook = OFNFileExistsHookProc;

    BOOL bRes = gfn.Save(true);
    sBuffer.ReleaseBuffer();
    if(!bRes)
        return S_OK;
    else
    {
        m_sLastSaveFile = sBuffer;
        m_bExport = true;
    }

    // Export the threats
    if(m_bExport)
    {
        // Get the docs HWND
        HWND hwndRoot;
        HRESULT hr = GetDocHwnd(hwndRoot);
        if(FAILED(hr))
            return hr;

        m_spThreatTracker->ExportThreats(m_sLastSaveFile, hwndRoot, true);
    }

    return S_OK;
}

HRESULT CFrameDocument::OnHelp(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
    if(!m_spThreatTracker)
        LOG_FAILURE_AND_RETURN(L"Missing Threat Tracker!", E_NOINTERFACE);
        
    // Get the ScanUICallback object
    avScanUI::IScanUICallbacksIntPtr spScanUI = m_spThreatTracker->GetScanUI();
    if(!spScanUI)
        LOG_FAILURE_AND_RETURN(L"Failed to get pScanUI from Threat Tracker!", E_NOINTERFACE);

    // Get a context-sensitive help ID.  Be gentle with unknown tabs.
    DWORD dwHelpID;
    if(m_sCurTab == TAB_SUMMARY)
        dwHelpID = IDH_NAVW_SCAN_SUMMARY_DLG_MOREINFO;
    else if(m_sCurTab == TAB_ATTENTION)
        dwHelpID = IDH_NAVW_REPAIR_WIZARD_DLG_HELP_BTN;
    else if(m_sCurTab == TAB_DETAILS)
        dwHelpID = NAVW_SCAN_DET_RESULTS;
    else
    {
        CCTRACEI(L"Unknown m_sCurTab; assuming default help ID");
        dwHelpID = IDH_NAVW_SCAN_SUMMARY_DLG_MOREINFO;
    }

    // Get our HWND
    HWND hWnd = NULL;
    GetDocHwnd(hWnd);

    // Launch help
    return spScanUI->LaunchHelp(dwHelpID, hWnd);
}

HRESULT CFrameDocument::OnTabClicked(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
    if(lpElement)
    {
        // Get the name of the item clicked
        LPCWSTR pszVal = NULL;
        lpElement->GetAttributeByName("id", &pszVal);

        // Update our buttons
        m_sCurTab = pszVal;
        UpdateTabButtons();
    }

    return S_OK;
}

HRESULT CFrameDocument::OnDocumentComplete() throw()
{
    HRESULT hr = symhtml::CSymHTMLDocumentImpl::OnDocumentComplete();
    if(FAILED(hr))
        return hr;

    // Create our tabs
    m_spDocSummaryTab = new CTabSummaryDocument();
    m_spDocAttentionTab = new CTabAttentionDocument();
    m_spDocDetailsTab = new CTabDetailsDocument();

    // Pass on the threat tracker...
    CSymPtr<CThreatTracker> spThreatTracker = m_spThreatTracker;
    SetThreatTracker(spThreatTracker);

    // Render the tabs
    m_spSummaryIFrame->Render(m_spDocSummaryTab);
    m_spAttentionIFrame->Render(m_spDocAttentionTab);
    m_spDetailsIFrame->Render(m_spDocDetailsTab);

    // Enable message pumping since we are the main UI thread...
    m_Options.m_bPumpMessages = TRUE;

    // Create the refresh thread
    if(!Create(NULL, 0, 0))
    {
        LOG_FAILURE(L"Failed to create refresh thread... falling back to SLOW mode", E_FAIL);
    }

    if(m_bResultsMode)
    {
        CStringW sText;
        sText.LoadString(IDS_STATUS_DONE);

        // Stop the progress (switch to being done)
        ScanDone();
        SetStatus(sText);

        // Refresh our stats...
        RefreshScanStats();

        // In results mode, we need to start on the attention or details tab
        // depending on if we have threats that remain...
        if(ThreatsRemain())
            SwitchToTab(TAB_ATTENTION);
        else
            SwitchToTab(TAB_DETAILS);

        // Refresh the threat list
        RefreshThreatList();
    }

    return hr;
}

HRESULT CFrameDocument::OnNewIFrame(symhtml::ISymHTMLElement* lpElement, symhtml::ISymHTMLWindow* lpNewFrame) throw()
{
    LPCWSTR pszID;
    HRESULT hr = lpElement->GetAttributeByName(symhtml::attribute::id, &pszID);
    LOG_FAILURE_AND_RETURN(L"Failed to get new frames ID", hr);

    CStringW sID = pszID;
    if(sID.Compare(L"summaryiframe") == 0)
        m_spSummaryIFrame = lpNewFrame;
    else if(sID.Compare(L"attentioniframe") == 0)
        m_spAttentionIFrame = lpNewFrame;
    else if(sID.Compare(L"detailsiframe") == 0)
        m_spDetailsIFrame = lpNewFrame;
    else
        LOG_FAILURE_AND_RETURN(L"Unknown Frame ID!", E_INVALIDARG);

    return S_OK;
}

HRESULT CFrameDocument::SetElementSelected(LPCSTR pszElementID, bool bSelected)
{
    symhtml::ISymHTMLElementPtr spElem;
    HRESULT hr = GetElement(pszElementID, &spElem);
    LOG_FAILURE_AND_RETURN(L"Failed to get element!", hr);

    if(bSelected)
        hr = spElem->SetAttributeByName("selected", L"1");
    else
        hr = spElem->SetAttributeByName("selected", NULL);

    LOG_FAILURE_AND_RETURN(L"Failed to add/remove selected element", hr);
    return hr;
}

HRESULT CFrameDocument::SwitchToTab(LPCSTR pszTabID)
{
    m_sCurTab = pszTabID;

    // Get the tab container
    symhtml::ISymHTMLElementPtr spTabContainer;
    HRESULT hr = GetElement("tabcontainer", &spTabContainer);
    LOG_FAILURE_AND_RETURN(L"Failed to get the tabcontainer element", hr);

/*
    symhtml::ISymHTMLElementPtr spTab;
    GetElement(pszTabID, &spTab);
    return spTabContainer->PostEvent(symhtml::event::MOUSE, spTab, 0 / *EVENT_REASON::MOUSE* /);
*/

    // Get the new tab
    symhtml::ISymHTMLElementPtr spNewTab;
    hr = GetElement(pszTabID, &spNewTab);
    LOG_FAILURE_AND_RETURN(L"Failed to find the new tab", hr);

    // Check if the new tab is already selected
    UINT nState = 0;
    hr = spNewTab->GetElementState(&nState);
    LOG_FAILURE_AND_RETURN(L"Failed to get new tabs state", hr);
    if(nState & symhtml::state::current)
    {
        // nothing to do... tab is already selected
        return S_OK;
    }

    // Find the currently selected tab and panel
    symhtml::ISymHTMLElementPtr spCurTab, spCurPage;
    hr = spTabContainer->FindFirstElement("[name]:expanded", &spCurPage);
    LOG_FAILURE_AND_RETURN(L"Failed to find the currently expanded page", hr);
    hr = spTabContainer->FindFirstElement("[panel]:current", &spCurTab);
    LOG_FAILURE_AND_RETURN(L"Failed to find the current tab", hr);

    // Get the new panel name
    LPCWSTR pszPanelName;
    hr = spNewTab->GetAttributeByName("panel", &pszPanelName);

    // Find the new panel
    CStringA sSelector;
    sSelector.Format("[name=\"%S\"]", pszPanelName);
    symhtml::ISymHTMLElementPtr spNewPage;
    hr = spTabContainer->FindFirstElement(sSelector, &spNewPage);
    LOG_FAILURE_AND_RETURN(L"Failed to find the matching new page", hr);

    // "Unselect" the current tab and page
    spCurPage->SetAttributeByName("selected", NULL);                // remove selected attribute - just in case somone is using attribute selectors
    spCurPage->SetElementState(symhtml::state::collapsed, 0, TRUE); // collapse it
    spCurTab->SetAttributeByName("selected", NULL);                 // remove selected attribute
    spCurTab->SetElementState(0, symhtml::state::current, TRUE);    // reset also state flag, :current

    // "Select" the new tab and page
    spNewPage->SetAttributeByName("selected", L"");                 // set selected attribute (empty)
    spNewPage->SetElementState(symhtml::state::expanded, 0, TRUE);  // expand it
    spNewTab->SetAttributeByName("selected", L"");                  // set selected attribute (empty)
    spNewTab->SetElementState(symhtml::state::current, 0, TRUE);  // set also state flag, :current

    // Redraw
    spTabContainer->UpdateElement(TRUE);

    return UpdateTabButtons();
}

HRESULT CFrameDocument::UpdateTabButtons()
{
    // Get the scan statistics
    avScanUI::IScanUICallbacksIntPtr spScanUI = m_spThreatTracker->GetScanUI();
    if(!spScanUI)
    {
        LOG_FAILURE_AND_RETURN(L"Can't get stats ... spScanUI = NULL", E_NOINTERFACE);
    }
    avScanUI::ScanStatistics scanStats;
    spScanUI->GetScanStats(scanStats);

	// Pass them to the method that does the work.
	return UpdateTabButtons(scanStats);
}

HRESULT CFrameDocument::UpdateTabButtons(const avScanUI::ScanStatistics& scanStats)
{
    HRESULT hr = S_OK;

	// These will track which buttons should be visible.
	bool bApplyButtonVisible(false);
	bool bNextButtonVisible(false);
	bool bDoneButtonsVisible(false);

	// Determine which buttons should be visible
    int nDetected = scanStats.nVirDetected + scanStats.nNonVirDetected;
    int nResolved = scanStats.nVirRemoved + scanStats.nNonVirRemoved;
    if(m_sCurTab == TAB_SUMMARY)
    {
        // Show Next button iff the scan is done and threats are detected
        if(m_bScanDone && nDetected > 0)
            bNextButtonVisible = true;
    }
    else if(m_sCurTab == TAB_ATTENTION)
    {
        // Show Next button iff the scan is done and threats are detected
        if(m_bScanDone && nDetected > 0)
            bNextButtonVisible = true;

        // Show Apply button iff detected threats exceed resolved threats
        if((nDetected - nResolved) > 0)
            bApplyButtonVisible = true;
    }
    else if(m_sCurTab == TAB_DETAILS)
    {
        // Display none
    }

	// For now, Next button is always hidden.
	bNextButtonVisible = false;

	// Show Done button iff scan is done.
	bDoneButtonsVisible = m_bScanDone;

	// If the visibility of a button has changed, update the button in the HTML
	// and remember the new visibility state for the next call.
	if (bApplyButtonVisible != m_bApplyButtonVisible)
	{
	    SetElementHidden(BTN_APPLY, bApplyButtonVisible, TRUE);
		m_bApplyButtonVisible = bApplyButtonVisible;
	}
	if (bNextButtonVisible != m_bNextButtonVisible)
	{
	    SetElementHidden(BTN_NEXT, bNextButtonVisible, TRUE);
		m_bNextButtonVisible = bNextButtonVisible;
	}
	if (bDoneButtonsVisible != m_bDoneButtonsVisible)
	{
	    SetElementHidden(BTN_PAUSE, !bDoneButtonsVisible, TRUE);
	    SetElementHidden(BTN_STOP,  !bDoneButtonsVisible, TRUE);
	    SetElementHidden(BTN_EXPORT, bDoneButtonsVisible, TRUE);
	    SetElementHidden(BTN_FINISH, bDoneButtonsVisible, TRUE);
		m_bDoneButtonsVisible = bDoneButtonsVisible;
	}

    return hr;
}

void CFrameDocument::GenerateNavLogEntry()
{
	// Do logging
	if(m_spThreatTracker)
	{
		// Initialize logging    
		AvProd::AvModuleLogging::IManualScanClientActivitiesPtr spScanResultsLog;
		SYMRESULT sr = AvProd::AvModuleLogging::ManualScanClientActivities_Loader::CreateObject(GETMODULEMGR(), &spScanResultsLog);
		if(spScanResultsLog && SUCCEEDED(spScanResultsLog->Initialize()))
		{
			CCTRACEI( CCTRCTX _T("::--> Logging start <--::"));
			CThreatTracker::MAP_CAT_COUNT mapCounts;
			CStringW sSubStats;

			// Log resolved category counts
			m_spThreatTracker->GetAnomalyCategoryStats(sSubStats, CThreat::eTTState_Done, &mapCounts);
			if(mapCounts.GetCount())
			{
				for(DWORD dwIdx = 0; dwIdx <= ccEraser::IAnomaly::Last_Category; ++dwIdx)
				{
					CThreatTracker::MAP_CAT_COUNT::CPair* pPair = mapCounts.Lookup((ccEraser::IAnomaly::Anomaly_Category)dwIdx);
					if(pPair)
					{
						CCTRACEI( CCTRCTX _T(":::: Category: %d, Value: %d"), pPair->m_key, pPair->m_value);
						spScanResultsLog->SetScanResultByCategory(AV::Event_ScanAction_ResolvedByCategory, pPair->m_key, pPair->m_value);
					}
				}
			}

			sSubStats.Empty();
			mapCounts.RemoveAll();

			// Log unresolved category counts
			m_spThreatTracker->GetAnomalyCategoryStats(sSubStats, CThreat::eTTState_RequireAttention, &mapCounts);
			if(mapCounts.GetCount())
			{
				for(DWORD dwIdx = 0; dwIdx <= ccEraser::IAnomaly::Last_Category; ++dwIdx)
				{
					CThreatTracker::MAP_CAT_COUNT::CPair* pPair = mapCounts.Lookup((ccEraser::IAnomaly::Anomaly_Category)dwIdx);
					if(pPair)
					{
						CCTRACEI( CCTRCTX _T(":::: Category: %d, Value: %d"), pPair->m_key, pPair->m_value);
						spScanResultsLog->SetScanResultByCategory(AV::Event_ScanAction_UnResolvedByCategory, pPair->m_key, pPair->m_value);
					}
				}
			}

			if(m_spThreatTracker->GetScanUI())
			{
				// Log specific scan counts
				avScanUI::ScanStatistics scanStats;
				m_spThreatTracker->GetScanUI()->GetScanStats(scanStats, true);
				
				CString cszScanTitle;
				cc::IStringPtr spScanTitleString;
				HRESULT hr = m_spThreatTracker->GetScanUI()->GetScanTitle(spScanTitleString);
				if(SUCCEEDED(hr) && spScanTitleString)
					cszScanTitle = spScanTitleString->GetStringW();
                
                GUID guidScanInstanceId = GUID_NULL;
                hr = m_spThreatTracker->GetScanUI()->GetScanInstanceId(guidScanInstanceId);
                if(SUCCEEDED(hr))
                    spScanResultsLog->SetScanInstanceId(guidScanInstanceId);

				spScanResultsLog->SetScanType(cszScanTitle);
				spScanResultsLog->SetScanResult(scanStats.nScanResult);
				spScanResultsLog->SetScanResultByProperty(AV::Event_ScanAction_ScanTime, scanStats.nScanTime);
				spScanResultsLog->SetScanResultByProperty(AV::Event_ScanAction_ItemTotalScanned, scanStats.nTotalScanned);
				spScanResultsLog->SetScanResultByProperty(AV::Event_ScanAction_VirusesDetected, scanStats.nVirDetected);
				spScanResultsLog->SetScanResultByProperty(AV::Event_ScanAction_VirusesRemoved, scanStats.nVirRemoved);
				spScanResultsLog->SetScanResultByProperty(AV::Event_ScanAction_NonVirusesDetected, scanStats.nNonVirDetected);
				spScanResultsLog->SetScanResultByProperty(AV::Event_ScanAction_NonVirusesRemoved, scanStats.nNonVirRemoved);
				spScanResultsLog->SetEraserScanResults(ccEraser::RegistryDetectionActionType, scanStats.nRegistryItems);
				spScanResultsLog->SetEraserScanResults(ccEraser::FileDetectionActionType, scanStats.nFileItems);
				spScanResultsLog->SetEraserScanResults(ccEraser::ProcessDetectionActionType, scanStats.nProcessItems);
				spScanResultsLog->SetEraserScanResults(ccEraser::BatchDetectionActionType, scanStats.nBatchItems);
				spScanResultsLog->SetEraserScanResults(ccEraser::INIDetectionActionType, scanStats.nINIItems);
				spScanResultsLog->SetEraserScanResults(ccEraser::ServiceDetectionActionType, scanStats.nServiceItems);
				spScanResultsLog->SetEraserScanResults(ccEraser::StartupDetectionActionType, scanStats.nStartupItems);
				spScanResultsLog->SetEraserScanResults(ccEraser::COMDetectionActionType, scanStats.nCOMItems);
				spScanResultsLog->SetEraserScanResults(ccEraser::HostsDetectionActionType, scanStats.nHostsItems);
				spScanResultsLog->SetEraserScanResults(ccEraser::DirectoryDetectionActionType, scanStats.nDirectoryItems);
				spScanResultsLog->SetEraserScanResults(ccEraser::LSPDetectionActionType, scanStats.nLSPItems);
				spScanResultsLog->SetEraserScanResults(ccEraser::BrowserCacheDetectionActionType, scanStats.nBrowserCacheItems);
				spScanResultsLog->SetEraserScanResults(ccEraser::CookieDetectionActionType, scanStats.nCookieItems);
				spScanResultsLog->SetEraserScanResults(ccEraser::ApplicationHeuristicDetectionActionType, scanStats.nApplicationHeuristicItems);
			
				// Have the service do the logging of the individual threats
				AvProdSvc::AvProdSvcClient_Loader AvProdSvcClientLoader;
				AvProdSvc::IAvProdSvcClientPtr spAvProdSvcClient;
				AvProdSvcClientLoader.CreateObject(spAvProdSvcClient);
				if(spAvProdSvcClient)
				{
					spAvProdSvcClient->Initialize();
					spAvProdSvcClient->LogManualScanResults(guidScanInstanceId);
				}

			}

			CCTRACEI( CCTRCTX _T("::--> Logging end <--::"));
			spScanResultsLog->Commit();
		}
		else
		{
			CCTRACEE(CCTRCTX _T("Failed to init logging. sr=0x%08X"), sr);
			// Non-fatal error        
		}
	}
	
	return;
}
HRESULT CFrameDocument::CloseDocument()
{
    // Get our thread to start exiting...
    PostTerminate();

    // Generate a log entry
	GenerateNavLogEntry();

    // Get the docs HWND
    HWND hwndRoot;
    HRESULT hr = GetDocHwnd(hwndRoot);
    if(FAILED(hr))
        return hr;

    // Send a close message
    PostMessage(GetParent(hwndRoot), WM_CLOSE, 0, 0);

    return S_OK;
}

bool CFrameDocument::PromptIfProcessing()
{
    // If not processing, this method has no prompt
    if(!m_bProcessing)
        return true;

    // Otherwise, the scan is still running... prompt the user to cancel it
    HWND hWnd;
    GetDocHwnd(hWnd);

    CStringW sMsg, sTitle;
    sMsg.LoadString(IDS_PROCESSING_MSG);
    sTitle = CISVersion::GetProductName();
    MessageBox(hWnd, sMsg, sTitle, MB_OK);

    return false;
}

bool CFrameDocument::PromptIfScanRunning()
{
    // If the scan is done, just return true...
    if(m_bScanDone)
        return true;

    // Otherwise, the scan is still running... prompt the user to cancel it
    HWND hWnd;
    GetDocHwnd(hWnd);

    // Pause before the prompt
    AVModule::ManualScanner::eMSState eState;
    HRESULT hr = m_spScanner->GetCurrentState(eState);
    LOG_FAILURE(L"Failed to get current scan state!", hr);
    hr = m_spScanner->Pause();
    LOG_FAILURE(L"Failed to pause scan!", hr);

    CStringW sMsgB, sTitle;
    sMsgB.LoadString(IDS_USERSTOPSCAN_PROMPT);
    sTitle = CISVersion::GetProductName();
    if(IDYES == MessageBox(hWnd, sMsgB, sTitle, MB_YESNO))
    {
        // Stop the active scan...
        hr = m_spScanner->Abort();
        LOG_FAILURE(L"Abort call failed", hr);
        if(SUCCEEDED(hr))
        {
        m_bScanAborted = true;

            // Disable the pause and stop buttons
            SetElementEnabled(BTN_PAUSE, false, TRUE);
            SetElementEnabled(BTN_STOP, false, TRUE);
        }

        return true;
    }

    // Restart the scan if we paused it
    if(eState != AVModule::ManualScanner::eMSState_Paused
        && eState != AVModule::ManualScanner::eMSState_Pausing)
    {
        hr = m_spScanner->Resume();
        LOG_FAILURE(L"Failed to resume scan!", hr);
    }

    return false;
}

bool CFrameDocument::PromptForReboot()
{
    // Prompt for reboot (if needed)
    if(m_spThreatTracker->RebootRequired())
    {
        avScanUI::IScanUIMiscPtr spMiscUI = new avScanUI::CScanUIMisc();
        if(!spMiscUI)
        {
            LOG_FAILURE(L"Failed to create an instance of CScanUIMisc", E_OUTOFMEMORY);
        }
        else
        {
            // Prompt the user
            HWND hWnd = NULL;
            GetDocHwnd(hWnd);
            if(avScanUI::eReboot_Now == spMiscUI->QueryUserForReboot(hWnd))
                m_bRebootOnExit = true;
        }
    }

    // Prompt for reboot always returns true
    return true;
}

bool CFrameDocument::PromptIfThreatsRemain()
{
    // If the no threats remain, just return true...
    if(!ThreatsRemain())
        return true;

    // Otherwise, the threats remain... prompt the user to cancel it
    HWND hWnd;
    GetDocHwnd(hWnd);

    CStringW sMsg, sTitle;
    sMsg.LoadString(IDS_ITEMSREMAIN_PROMPT);
    sTitle = CISVersion::GetProductName();
    if(IDYES == MessageBox(hWnd, sMsg, sTitle, MB_YESNO))
    {
        return true;
    }

    return false;
}

HRESULT CFrameDocument::SetElementHidden(LPCSTR szID, BOOL bDisplay, BOOL bRenderNow)
{
    HRESULT hr = S_OK;

    ISymHTMLElementPtr spElement;
    hr = GetElement(szID, &spElement);
    if(FAILED(hr))
        return hr;

    LPCWSTR szwValue = bDisplay ? NULL : L"1";
    hr = spElement->SetAttributeByName("hide", szwValue);

    if(SUCCEEDED(hr))
        spElement->UpdateElement(bRenderNow);

    return hr;
}

int CFrameDocument::Run()
{
    CCTRCTXI0(L"--Refresh Threat List Thread Begin");

    // Load SymTheme on this thread (for dialogs)
	CISSymTheme isSymTheme;
	HRESULT hr = isSymTheme.Initialize(GetModuleHandle(NULL));
	if(FAILED(hr))
	{
		CCTRCTXE1(L"CISSymTheme::Initialize() failed. Error: 0x%08X", hr);
	}

    // Wait 1 second for the exit event
    while(FALSE == WaitForTerminate(1000))
    {
        // Do we need to refresh?
        if(m_bTListRefreshNeeded)
        {
            m_bTListRefreshNeeded = false;

            // Do the refresh
            RealRefreshThreatList();
        }
    }

    CCTRCTXI0(L"--Refresh Threat List Thread End");
    return 0;
}