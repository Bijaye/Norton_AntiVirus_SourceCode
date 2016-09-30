////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScanTaskDialogDocument.cpp : implementation file
//
#include "StdAfx.h"
#include "..\\avScanTaskRes\\resource.h"
#include "ScanTaskDialogDocument.h"
#include "ScanTaskDefines.h"
#include "isVersion.h"
#include "isErrorLoader.h"
#include "isErrorIDs.h"
#include "isErrorResource.h" // error resource id's
#include "isResource.h"
#include "AVccModuleId.h"
#include "ccSymModuleLifetimeMgrHelper.h"
#include "OSInfo.h"

using namespace avScanTask;

//-------------------------------------------------------------------------
HRESULT CScanTaskDialogDocument::OnDocumentComplete() throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		__super::OnDocumentComplete(); // ALWAYS MAKE SURE YOU CALL THE BASE MEDTHOD!

		// Create quit event
		if(!m_QuitEvent.Create(NULL, FALSE, FALSE, NULL, FALSE))
		{
			hrx << E_FAIL;
		}

		// Get product name
		m_sProductName = CISVersion::GetProductName();

		// Skip checking result here since it's OK
		// for the scan panel to have an empty caption
		SetWindowText(m_sProductName);

		HICON hIcon = NULL;
		if(CISVersion::GetProductIcon(hIcon) && hIcon)
			SetIcon(hIcon, TRUE);

		SetWindowPos(0, 0, 0, 495, 320, SWP_NOZORDER|SWP_NOMOVE);
		CenterWindow();

		// Get the root html element
		symhtml::ISymHTMLElementPtr spRootElement;
		hrx << GetRootElement(spRootElement);
		
		// Display a list of scan tasks
		hrx << buildTaskList(spRootElement);

		// Refresh UI
		hrx << spRootElement->UpdateElement(TRUE);
		hrx << ShowWindow(SW_SHOW);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr; 
}
//-------------------------------------------------------------------------
HRESULT CScanTaskDialogDocument::buildTaskList(symhtml::ISymHTMLElement* lpRootElement)
{
	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		// Create scan task collection object
		hrx << (SYM_SUCCEEDED(m_ScanTaskCollectionLoader.CreateObject(&m_spScanTaskCollection))? S_OK : E_FAIL);

		// Get total of scan task items to be displayed
		DWORD dwCount = 0;
		hrx << m_spScanTaskCollection->GetCount(dwCount);
		if(!dwCount)
			return S_OK;

		// Map of all scan tasks in appropriate order
		std::map<DWORD, SCAN_TASK_ITEM> TaskListMap;

		// Index of custom scan in the map
		DWORD dwScanIndex = 0;
		DWORD dwCustomScanIndex = 0;

		// custom scan is displayed starting at 4th item in the list
		const DWORD CUSTOM_SCAN_START_INDEX = 3;		

		// Build a map of tasks in the order we want to display
		//	Scan Drives			-			-		-
		//	Scan Folders		-			-		-
		//	Scan Files			-			-		-
		//	Custom scan #1		-Schedule	-Edit	-Delete
		//	Custom scan #2		-Schedule	-Edit	-Delete
		//  ...............................................

		for(int i=0; i<dwCount; ++i)
		{
			// Get a scan task
			IScanTaskPtr spScanTask;
			hrx << m_spScanTaskCollection->GetItem(i, &spScanTask);
			if(!spScanTask.m_p)
				continue;

			// Get task name
			cc::IStringPtr pName;
			hrx << spScanTask->GetName(&pName);

			EScheduled eScheduled = EScheduled_no;
			hrx << spScanTask->GetScheduled(eScheduled);

			// Get task type
			SCANTASKTYPE type = scanUnset;
			hrx << spScanTask->GetType(type);

			ccLib::CStringW sIcon;

			// Get icon graphic for the task.
			// Determine position of the task in the list
			switch(type)
			{
			case scanSelectedDrives:
				{
					// Force scan drives to be the first item
					dwScanIndex = 0;
					sIcon = ICON_DRIVES;
				}
				break;
			case scanFolder:
				{
					// Force scan folders to be the second item
					dwScanIndex = 1;
					sIcon = ICON_FOLDERS;
				}	
				break;
			case scanFiles:
				{
					// Force scan files to the third item
					dwScanIndex = 2;
					sIcon = ICON_FILES;
				}
				break;
			case scanCustom:
				{
					// Custom scan starts after "scan files"
					dwScanIndex = CUSTOM_SCAN_START_INDEX + dwCustomScanIndex;
					++dwCustomScanIndex;
					sIcon = ICON_VIRUS;
				}
				break;
			default:
				continue;
				break;
			}

			TaskListMap[dwScanIndex].type = type;
			TaskListMap[dwScanIndex].bScheduleOn = (eScheduled == EScheduled_yes) ? true : false;
			TaskListMap[dwScanIndex].sIcon = sIcon;
			TaskListMap[dwScanIndex].sName = pName->GetStringW();
		}

		ccLib::CStringW sTaskListHtml;

		for(int i=0; i<TaskListMap.size(); ++i)
		{
			if(!TaskListMap[i].sName.IsEmpty())
			{
				CCTRCTXI1(L"Add task: %s", TaskListMap[i].sName.GetString());

				// Construct html for this task
				ccLib::CStringW sTaskItemHtml;
				hrx << buildTaskItemHtml(TaskListMap[i], i, sTaskItemHtml);
				sTaskListHtml += sTaskItemHtml;
			}
		}

		// Add all tasks to the task list
		symhtml::ISymHTMLElementPtr spTaskListElement;
		hrx << lpRootElement->GetElementByID(ELEMENT_ID_TASK_LIST, &spTaskListElement);
		hrx << spTaskListElement->AppendElementHtml(sTaskListHtml, -1);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	if(FAILED(hr))
	{
		showError(ERR_SCANTASK_CREATE_SCANTASKCOLLECTION, IDS_ISSHARED_ERROR_INTERNAL, hr);
	}
	return S_OK;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskDialogDocument::OnHtmlEvent(symhtml::ISymHTMLElement* lpElement, LPCWSTR szEventName, UINT nEventType, ISymBase* pExtra) throw()
{
	symhtml::ISymHTMLElementPtr spTaskListElement;
	symhtml::ISymHTMLElementPtr spNewTaskElement;
	bool bDlgDisabled = false;

	STAHLSOFT_HRX_TRY(hr)
	{
		// Call parent handler first
		hr = __super::OnHtmlEvent(lpElement, szEventName, nEventType, pExtra);

 		// If parent does not handle this event, we handle it.
		// Only handle hyperlink events.
		if((S_FALSE == hr) && (symhtml::event::HYPERLINK_CLICK == nEventType))
		{
			CCTRCTXI0(L"Enter");

			// Prevent user from clicking items in the dialog
			// until we finish the current request
			symhtml::ISymHTMLElementPtr spRootElement;
			hrx << GetRootElement(spRootElement);
			hrx << spRootElement->GetElementByID("oTaskList", &spTaskListElement);
			hrx << spRootElement->GetElementByID("oNewTask", &spNewTaskElement);

			// Disable the tasklist to prevent users from launching new task
			hrx << spTaskListElement->SetElementState(symhtml::state::disabled, 0, true);
			hrx << spNewTaskElement->SetElementState(symhtml::state::disabled, 0, true);
			bDlgDisabled = true;

			ccLib::CStringW sTaskName;
			HRESULT hr2 = getTaskName(lpElement, sTaskName);
			if(FAILED(hr2) || !sTaskName.GetLength())
			{
				// Do not found task name, ignore.
				return S_OK;
			}

			LPCWSTR lpszAction = NULL;
			hrx << lpElement->GetAttributeByName(ATTRIBUTE_TASK_ACTION, &lpszAction);
			hrx << (!lpszAction? E_FAIL : S_OK);

			CCTRCTXI1(L"TaskAction: %s", lpszAction);

			if(wcsstr(lpszAction, TASK_SCAN))
			{
				// Launch scan wizard
				hrx << onScan(lpElement, sTaskName);
			}
			if(wcsstr(lpszAction, TASK_SCHEDULE))
			{
				// Launch schedule wizard
				hrx << onScheduleTask(lpElement, sTaskName);
			}
			else if(wcsstr(lpszAction, TASK_EDIT))
			{
				// Launch edit wizard
				hrx << onEditTask(lpElement, sTaskName);
			}
			else if(wcsstr(lpszAction, TASK_DELETE))
			{
				// Delete the scan task
				hrx << onDeleteTask(lpElement, sTaskName);
			}

			CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);

	if(bDlgDisabled)
	{
		STAHLSOFT_HRX_TRY(hr1)
		{
			// Enable the dialog
			hrx << spTaskListElement->SetElementState(0, symhtml::state::disabled, true);
			hrx << spNewTaskElement->SetElementState(0, symhtml::state::disabled, true);
		}
		STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr1)
	}
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskDialogDocument::OnNewTask(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled) throw()
{
	CCTRCTXI0(L"Enter");

	symhtml::ISymHTMLElementPtr spTaskListElement;
	symhtml::ISymHTMLElementPtr spNewTaskElement;
	bool bDlgDisabled = false;

	STAHLSOFT_HRX_TRY(hr)
	{
		// Prevent user from clicking items in the dialog
		// until we finish the current request
		symhtml::ISymHTMLElementPtr spRootElement;
		hrx << GetRootElement(spRootElement);
		hrx << spRootElement->GetElementByID("oTaskList", &spTaskListElement);
		hrx << spRootElement->GetElementByID("oNewTask", &spNewTaskElement);

		// Disable the tasklist to prevent users from launching new task
		hrx << spTaskListElement->SetElementState(symhtml::state::disabled, 0, true);
		hrx << spNewTaskElement->SetElementState(symhtml::state::disabled, 0, true);
		bDlgDisabled = true;

		// Launch scan task wizard
		IScanTaskPtr spScanTask;
		hrx << m_spScanTaskCollection->CreateItem(&spScanTask);
	
		if(spScanTask.m_p)
		{
			// Get name of the new task
			cc::IStringPtr spName;
			hrx << spScanTask->GetName(&spName);

			// Get total number of scan items.
			// Use this number as unique ID
			// for html element of the new task.
			// Note that not all scan items are displayed 
			// in the UI. Therefore, total number of scans 
			// appear in the UI will be less than actual
			// number of scans on the disk.
			DWORD dwCount = 0;
			hrx << m_spScanTaskCollection->GetCount(dwCount);

			ccLib::CStringW sTaskItemHtml;
			SCAN_TASK_ITEM taskItem;
			taskItem.type = scanCustom;
			taskItem.bScheduleOn = false;
			taskItem.sIcon = ICON_VIRUS;
			taskItem.sName = spName->GetStringW();

			// Construct html for the task 
			hrx << buildTaskItemHtml(taskItem, dwCount, sTaskItemHtml);

			symhtml::ISymHTMLElementPtr spRootElement;
			symhtml::ISymHTMLElementPtr spTaskListElement;

			// Append the html task at the end of the task list
			hrx << GetRootElement(&spRootElement);
			hrx << spRootElement->GetElementByID(ELEMENT_ID_TASK_LIST, &spTaskListElement);
			hrx << spTaskListElement->AppendElementHtml(sTaskItemHtml, -1);
			hrx << spTaskListElement->UpdateElement(TRUE);
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	
	if(bDlgDisabled)
	{
		STAHLSOFT_HRX_TRY(hr1)
		{
			// Enable the dialog
			hrx << spTaskListElement->SetElementState(0, symhtml::state::disabled, true);
			hrx << spNewTaskElement->SetElementState(0, symhtml::state::disabled, true);
		}
		STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr1)
	}

	if(FAILED(hr))
	{
		showError(ERR_SCANTASK_UNKNOWN_SCHEDULE, IDS_ISSHARED_ERROR_INTERNAL, hr);
	}

	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskDialogDocument::onScheduleTask(symhtml::ISymHTMLElement* lpElement, LPCWSTR lpszTaskName)
{
	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((!lpElement || !lpszTaskName)? E_INVALIDARG : S_OK);

		// Get the task item
		IScanTaskPtr spScanTask;
		hrx << m_spScanTaskCollection->GetItem(lpszTaskName, &spScanTask);
		hrx << (!spScanTask.m_p ? E_FAIL : S_OK);

		// Launch schedule wizard
		hrx << spScanTask->ScheduleTask(false);

		// Check to see if a scan is scheduled
		ccLib::CStringW sScheduleText;
		EScheduled eSchedule = EScheduled_no;
		hrx << spScanTask->GetScheduled(eSchedule);

		if(EScheduled_yes == eSchedule)
		{
			sScheduleText.LoadString(IDS_SCHEDULE_ON);
		}
		else
		{
			sScheduleText.LoadString(IDS_SCHEDULE_OFF);
		}

		// Update schedule text to either "Schedule" or "Scan Scheduled"
		hrx << lpElement->SetElementInnerText(sScheduleText, sScheduleText.GetLength());

		// Refresh UI
		symhtml::ISymHTMLElementPtr spRootElement;
		hrx << GetRootElement(spRootElement);
		hrx << spRootElement->UpdateElement(TRUE);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	if(FAILED(hr))
	{
		showError(ERR_SCANTASK_UNKNOWN_SCHEDULE, IDS_ISSHARED_ERROR_INTERNAL, hr);
	}

	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskDialogDocument::onEditTask(symhtml::ISymHTMLElement* lpElement, LPCWSTR lpszTaskName)
{
	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((!lpElement || !lpszTaskName)? E_INVALIDARG : S_OK);

		// Get the task object
		IScanTaskPtr spScanTask;
		hrx << m_spScanTaskCollection->GetItem(lpszTaskName, &spScanTask);
		hrx << (!spScanTask.m_p ? E_FAIL : S_OK);

		CCTRCTXI1(L"Launch Edit wizard for %s", lpszTaskName);

		// Launch edit wizard
		hrx << spScanTask->Edit();

		CCTRCTXI0(L"Update UI");

		// Get the new task name
		cc::IStringPtr pName;
		hrx << spScanTask->GetName(&pName);

		// Get the <li> task element
		symhtml::ISymHTMLElementPtr spTaskItemElement;
		hrx << getTaskItemElement(lpElement, &spTaskItemElement);
		if(spTaskItemElement.m_p)
		{
			// Update the task-name attribute of <li> element
			hrx << spTaskItemElement->SetAttributeByName(ATTRIBUTE_TASK_NAME, pName->GetStringW());
		}

		// Update the task name
		// - Get the id of the task name anchor 
		// so that we can update it with the new task name.
		// id of the task name anchor is saved as 
		// "name-anchor-id" attribute of the edit anchor
		LPCWSTR lpszNameAnchorId = NULL;
		hrx << lpElement->GetAttributeByName(ATTRIBUTE_TASK_NAME_ANCHOR_ID, &lpszNameAnchorId);

		if(lpszNameAnchorId)
		{
			// Get root element
			symhtml::ISymHTMLElementPtr spRootElement;
			hrx << GetRootElement(spRootElement);

			// Get the scan name element
			symhtml::ISymHTMLElementPtr spNameElement;
			hrx << spRootElement->GetElementByID(CW2A(lpszNameAnchorId), &spNameElement);

			// Update task name in the UI
			hrx << spNameElement->SetElementInnerText(pName->GetStringW(), static_cast<int>(pName->GetLength()));

			// Refresh UI
			hrx << spRootElement->UpdateElement(TRUE);
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	if(FAILED(hr))
	{
		showError(ERR_SCANTASK_UNKNOWN_EDIT, IDS_ISSHARED_ERROR_EDIT_CUSTOM_SCAN, hr);
	}

	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskDialogDocument::onDeleteTask(symhtml::ISymHTMLElement* lpElement, LPCWSTR lpszTaskName)
{
	if(!lpElement || !lpszTaskName)
		return E_INVALIDARG;

	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		ccLib::CStringW sMsg;
		sMsg.LoadString(IDS_DELETE_TASK_CONFIRM);

		// Launch confirmation dialog
		if(IDYES == MessageBox(GetDesktopWindow(), sMsg, m_sProductName, MB_YESNO|MB_ICONQUESTION))
		{
			CCTRCTXI1(L"Delete task: %s", lpszTaskName);

			// Delete the scan task
			hrx << m_spScanTaskCollection->DeleteItem(lpszTaskName);

			// Get the root element
			CCTRCTXI0(L"Get root element");
			symhtml::ISymHTMLElementPtr spRootElement;
			hrx << GetRootElement(&spRootElement);

			// Get the scan task UI element 
			CCTRCTXI0(L"Get the corresponding task element");
			symhtml::ISymHTMLElementPtr spTaskItemElement;
			hrx << getTaskItemElement(lpElement, &spTaskItemElement);
			if(spTaskItemElement.m_p)
			{
				// Delete the element
				CCTRCTXI0(L"Delete the task element");
				hrx << spTaskItemElement->DeleteElement();
			}
			else
			{
				CCTRCTXE0(L"Task element not found");
			}

			// Refresh UI
			CCTRCTXI0(L"Refresh UI");
			hrx << spRootElement->UpdateElement(TRUE);

			CCTRCTXI0(L"All good");
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
		
	if(FAILED(hr))
	{
		showError(ERR_SCANTASK_FAILED_DELETE, IDS_ISSHARED_ERROR_INTERNAL, hr);
	}

	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskDialogDocument::onScan(symhtml::ISymHTMLElement* lpElement, LPCWSTR lpszTaskName)
{
	if(!lpElement || !lpszTaskName)
		return E_INVALIDARG;

	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		// Get the task with the given name
		IScanTaskPtr spScanTask;
		hrx << m_spScanTaskCollection->GetItem(lpszTaskName, &spScanTask);
		hrx << (!spScanTask.m_p ? E_FAIL : S_OK);

		// Invoke scan
        HANDLE hScanProcess = NULL;
		hrx << spScanTask->Scan(&hScanProcess, NULL);

		HANDLE aHandle[] = {m_QuitEvent, hScanProcess};
		DWORD dwCount = sizeof(aHandle)/sizeof(HANDLE);

		StahlSoft::WaitForMultipleObjectsWithMessageLoop(dwCount, aHandle, INFINITE);
		::CloseHandle(hScanProcess);
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	if(FAILED(hr))
	{
		if(ERROR_CANCELLED == HRESULT_CODE(hr))
		{
			CCTRCTXI0(L"User cancelled the scan operation");
			hr = S_OK;
		}
		else
		{
			CCTRCTXE1(L"Scan failed. Error: 0x%08X", hr);
			showError(ERR_SCANTASK_FAILED_LAUNCH_SCAN, IDS_ISSHARED_ERROR_INTERNAL, hr);
		}
	}

	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskDialogDocument::getTaskName(symhtml::ISymHTMLElement* lpElement, ccLib::CStringW& sTaskName)
{
	if(!lpElement)
		return E_INVALIDARG;

	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		symhtml::ISymHTMLElementPtr spTaskItemElement;
		hrx << getTaskItemElement(lpElement, &spTaskItemElement);
		hrx << (!spTaskItemElement.m_p? E_FAIL : S_OK);

		LPCWSTR lpszTaskName = NULL;
		hr = spTaskItemElement->GetAttributeByName(ATTRIBUTE_TASK_NAME, &lpszTaskName);

		if(SUCCEEDED(hr) || lpszTaskName)
		{
			CCTRCTXI1(L"Task found: %s", lpszTaskName);
			sTaskName = lpszTaskName;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskDialogDocument::getTaskItemElement(symhtml::ISymHTMLElement* lpElement, symhtml::ISymHTMLElement** lpTaskItemElement)
{
	if(!lpElement || !lpTaskItemElement)
		return E_INVALIDARG;

	CCTRCTXI0(L"Enter");

	STAHLSOFT_HRX_TRY(hr)
	{
		bool bFound = false;
		DWORD dwStep = 0;
		symhtml::ISymHTMLElementPtr spElement(lpElement);
		symhtml::ISymHTMLElementPtr spParentElement;

		// Wake up the parent element hierachy to find the <li> element.
		// This is where task-name attribute is stored.
		// It should take at most two steps up for a child element
		// of a task element to reach the task element.
		while(!bFound && (dwStep < 3))
		{
			spParentElement = NULL;

			hr = spElement->GetParentElement(&spParentElement);
			if(FAILED(hr) || !spParentElement.m_p)
			{
				CCTRCTXE1(L"GetParentElement failed. HR:0x%08X", hr);
				break;
			}

			LPCSTR lpszType = NULL;
			hr = spParentElement->GetElementType(&lpszType);
			if(FAILED(hr) || !lpszType)
			{
				CCTRCTXE1(L"GetElementType failed. HR:0x%08X", hr);
				break;
			}

			if(0 == stricmp(lpszType, "li"))
			{
				CCTRCTXI0(L"Found task item element");
				*lpTaskItemElement = spParentElement.Detach();
				bFound = true;
			}

			spElement = spParentElement.Detach();

			++dwStep;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskDialogDocument::buildTaskItemHtml(SCAN_TASK_ITEM taskItem,
												   DWORD dwTaskIndex, 
												   ccLib::CStringW& sHtml)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(scanCustom == taskItem.type)
		{
			// Setup unique ID's for all html elements
			ccLib::CStringW sTaskId;
			ccLib::CStringW sDivId;
			ccLib::CStringW sScanNameId, sScheduleId, sEditId, sDeleteId;
			ccLib::CStringW sScheduleText;

			// <div> ID
			sDivId.Format(L"oDiv%d", dwTaskIndex);

			// <li> scan task item ID
			sTaskId.Format(L"oTask%d", dwTaskIndex);

			// Setup ID of scan name anchor
			sScanNameId.Format(L"oName%d", dwTaskIndex);

			// Setup ID of Edit button
			sEditId.Format(L"oEdit%d", dwTaskIndex);

			// Setup ID of Delete button
			sDeleteId.Format(L"oDelete%d", dwTaskIndex);

			// Setup ID of Schedule button
			sScheduleId.Format(L"oSchedule%d", dwTaskIndex);

			if(taskItem.bScheduleOn)
			{
				sScheduleText.LoadString(IDS_SCHEDULE_ON);
			}
			else
			{
				sScheduleText.LoadString(IDS_SCHEDULE_OFF);
			}

			COSInfo OSInfo;
			if(COSInfo::IsGuest())
			{
				CCTRCTXI0(L"Guest account");
				//
				// Disable the schedule button.
				// Only admin and limited account can schedule task
				//
				// Load html for the task
				ccLib::CStringW sFormat;
				sFormat.LoadString(IDS_HTML_CUSTOM_SCAN_NO_SCHEDULE);

				// Format the element html
				// Save task name as "task-name" attribute for each html element
				// so that when the element is clicked, we can identify the 
				// task that the element is for.

				sHtml.Format(sFormat, 
					sTaskId,		// ID of <li> task item element
					taskItem.sName,	// save task name in attribute "task-name"
					sDivId,			// ID of <div> element
					taskItem.sIcon,	// Icon source
					sScanNameId,	// ID of scan name anchor <a> 
					taskItem.sName,	// name of the scan
					sEditId,		// ID of edit anchor <a>
					sScanNameId,	// scan name anchor ID is save as value of "name-anchor-id"
									// to be used for updating scan name later.
					sDeleteId		// ID of delete anchor <a> 
					);
			}
			else
			{
				CCTRCTXI0(L"Admin or limited account");

				// Load html for the task
				ccLib::CStringW sFormat;
				sFormat.LoadString(IDS_HTML_CUSTOM_SCAN);

				// Format the element html
				// Save task name as "task-name" attribute for each html element
				// so that when the element is clicked, we can identify the 
				// task that the element is for.

				sHtml.Format(sFormat, 
					sTaskId,		// ID of <li> task item element
					taskItem.sName,	// save task name in attribute "task-name"
					sDivId,			// ID of <div> element
					taskItem.sIcon,	// Icon source
					sScanNameId,	// ID of scan name anchor <a> 
					taskItem.sName,	// name of the scan
					sScheduleId,	// ID of schedule anchor <a> 
					sScheduleText,	// Schedule On or Schedule Off text
					sEditId,		// ID of edit anchor <a>
					sScanNameId,	// scan name anchor ID is save as value of "name-anchor-id"
									// to be used for updating scan name later.
					sDeleteId		// ID of delete anchor <a> 
					);
			}
		}
		else
		{
			ccLib::CStringW sFormat;
			sFormat.LoadString(IDS_HTML_BUILT_IN_SCAN);

			// Format the html string.
			// Task name is saved as "task-name=%s" attribute of each child element
			// This is needed so that we know which task this child element is for.
			ccLib::CStringW sTaskId;
			sTaskId.Format(L"oTask%d", dwTaskIndex);

			ccLib::CStringW sDivId;
			sDivId.Format(L"oDiv%d", dwTaskIndex);

			ccLib::CStringW sScanNameId;
			sScanNameId.Format(L"oTaskName%d", dwTaskIndex);
			sHtml.Format(sFormat, 
				sTaskId,		// ID of <li> task element 
				taskItem.sName,	// save task name in attribute "task-name"
				sDivId,			// ID of <div> element
				taskItem.sIcon,	// Icon source
				sScanNameId,	// ID of the task name anchor element
				taskItem.sName	// name of the scan
				);	
		}

		CCTRCTXI1(L"html: %s", sHtml.GetString());
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR: 0x%08X", hr);
	return hr;
}
//-------------------------------------------------------------------------
HRESULT CScanTaskDialogDocument::showError(DWORD dwErrorId, DWORD dwResId, HRESULT hr)
{
	HRESULT hr1 = S_OK;
	ISShared::ISShared_IError loader;
	ISShared::IErrorPtr spError;

	if(SYM_SUCCEEDED(loader.CreateObject(GETMODULEMGR(), &spError)))
	{
		hr1 = spError->Show(AV_MODULE_ID_SCAN_TASKS, dwErrorId, dwResId, NULL, NULL, hr, true, true, NULL);
	}

	return hr1;
}


//-------------------------------------------------------------------------
HRESULT CScanTaskDialogDocument::OnFinish(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
	m_QuitEvent.SetEvent();
	return __super::EndDialog(0);
}

//-------------------------------------------------------------------------
HRESULT CScanTaskDialogDocument::OnClose()
{
	m_QuitEvent.SetEvent();
	return S_OK;
}
