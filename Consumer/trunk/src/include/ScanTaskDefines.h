////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScanTaskDefines.h : header file
//
#pragma once

#include "navopt32.h"

//-------------------------------------------------------------------------
// Macro's
//-------------------------------------------------------------------------
#define NAVOPT_SUCCEEDED(res)	((res == NAVOPTS32_OK)? S_OK : E_FAIL)

//-------------------------------------------------------------------------
// Option names
//-------------------------------------------------------------------------
static LPCSTR szOptVersionMajor		= "Version:Major";
static LPCSTR szOptVersionMinor		= "Version:Minor";
static LPCSTR szOptVersionInLine	= "Version:InLine";
static LPCSTR szOptName				= "Name";
static LPCSTR szOptType				= "Type";
static LPCSTR szOptLastRun			= "LastRun";
static LPCSTR szOptDriveCount		= "DriveCount";
static LPCSTR szOptFolderCount		= "FolderCount";
static LPCSTR szOptFileCount		= "FileCount";
static LPCSTR szOptFileVIDCount		= "FileVIDCount";
static LPCSTR szOptCanEdit			= "CanEdit";
static LPCSTR szOptCanSchedule		= "CanSchedule";

static LPCWSTR TASK_FILE_EXTENSION	= L"sca";
static LPCWSTR TASK_SUBDIR			= L"Tasks";

// Maximum allowable # of characters in name of task - "My Computer"
static const UINT TASK_MAX_NAME_LENGTH  = 215; // MAX_PATH - (("Norton AntiVirus - ")x2) - '\0'

//-------------------------------------------------------------------------
// UI's
//-------------------------------------------------------------------------
// Icon for task item
static LPCWSTR ICON_DRIVES			= L"drives.png";
static LPCWSTR ICON_FOLDERS			= L"folder.png";
static LPCWSTR ICON_FILES			= L"files.png";
static LPCWSTR ICON_VIRUS			= L"virus_loupe.png";

static LPCWSTR TASK_SCAN			= L"scan";
static LPCWSTR TASK_SCHEDULE		= L"schedule";
static LPCWSTR TASK_EDIT			= L"edit";
static LPCWSTR TASK_DELETE			= L"delete";

static LPCSTR ELEMENT_ID_TASK_LIST				= "oTaskList";
static LPCSTR ATTRIBUTE_TASK_NAME				= "task-name";
static LPCSTR ATTRIBUTE_TASK_ACTION				= "task-action";
static LPCSTR ATTRIBUTE_TASK_NAME_ANCHOR_ID		= "scan-name-id";

