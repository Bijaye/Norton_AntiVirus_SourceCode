////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScanTaskDialogDocument.h : header file
//
#pragma once

// SymHTML
#include "SymHTML.h"
#include "SymHTMLElement.h"
#include "SymHTMLWindow.h"
#include "SymHTMLLoader.h"
#include "SymHTMLDocument.h"
#include "SymHTMLDocumentImpl.h"
#include "SymHTMLDialogDocumentImpl.h"
#include "ScanTaskLoader.h"
#include "ScanTaskInterface.h"
#include "ccEvent.h"

//-------------------------------------------------------------------------
class CScanTaskDialogDocument :
	public symhtml::CSymHTMLDocumentImpl,
	public symhtml::CSymHTMLDialogDocumentImpl,
	public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
	DECLARE_HTML_DOCUMENT(L"SCAN.HTM")

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDocument, symhtml::ISymHTMLDocument)
		SYM_INTERFACE_ENTRY(symhtml::IID_ISymHTMLDialogDocument, symhtml::ISymHTMLDialogDocument)
	SYM_INTERFACE_MAP_END()

	BEGIN_MSG_MAP(CScanTaskDialogDocument)
	END_MSG_MAP()

	BEGIN_HTML_EVENT_MAP(CScanTaskDialogDocument)
		HTML_EVENT_BUTTON_CLICK(L"oNewTask", OnNewTask)
		HTML_EVENT_BUTTON_CLICK(L"oFinishBtn", OnFinish)
	END_HTML_EVENT_MAP()

	// ISymHTMLDocument
	virtual HRESULT OnHtmlEvent(symhtml::ISymHTMLElement* lpElement, LPCWSTR szEventName, UINT nEventType, ISymBase* pExtra ) throw();
	virtual HRESULT OnDocumentComplete() throw();
	virtual HRESULT OnClose() throw() ;

	// Event handlers
	HRESULT OnNewTask(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled) throw();
	HRESULT OnFinish(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled) throw();

protected:
	// Find a task item <li> element 
	HRESULT getTaskItemElement(symhtml::ISymHTMLElement* lpElement, symhtml::ISymHTMLElement** lpTaskItemElement);

	// Get a task name that the given element belongs to
	HRESULT getTaskName(symhtml::ISymHTMLElement* lpElement, ccLib::CStringW& sTaskName);

	// Display all scan tasks 
	HRESULT buildTaskList(symhtml::ISymHTMLElement* lpRootElement);

	typedef struct SCAN_TASK_ITEM
	{
		avScanTask::SCANTASKTYPE type;
		bool bScheduleOn;
		ccLib::CStringW sIcon;		
		ccLib::CStringW sName;		
	} SCAN_TASK_ITEM;

	// Construct html for each scan task
	HRESULT buildTaskItemHtml(SCAN_TASK_ITEM taskItem, DWORD dwTaskIndex, ccLib::CStringW& sHtml);

	// Launch scan wizard for a task
	HRESULT onScan(symhtml::ISymHTMLElement* lpElement, LPCWSTR lpszTaskName);

	// Delete a scan task 
	HRESULT onDeleteTask(symhtml::ISymHTMLElement* lpElement, LPCWSTR lpszTaskName);

	// Launch schedule wizard for a scan task
	HRESULT onScheduleTask(symhtml::ISymHTMLElement* lpElement, LPCWSTR lpszTaskName);

	// Launch edit wizard for a scan task
	HRESULT onEditTask(symhtml::ISymHTMLElement* lpElement, LPCWSTR lpszTaskName);

	// Launch a common error display dialog
	HRESULT showError(DWORD dwErrorId, DWORD dwErrorResId, HRESULT hr);

protected:
	// Helper object to create ScanTaskCollection object
	avScanTask::ScanTaskCollection_Loader m_ScanTaskCollectionLoader;

	// Collection of all tasks
	avScanTask::IScanTaskCollectionPtr m_spScanTaskCollection;

	// Branding product name
	ccLib::CStringW m_sProductName;

	// Quit event when user closes the dialog
	ccLib::CEvent m_QuitEvent;
};
