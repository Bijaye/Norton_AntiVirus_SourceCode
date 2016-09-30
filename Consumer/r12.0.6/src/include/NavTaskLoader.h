#pragma once

#include "ccSymInterfaceLoader.h"
#include "ccSymPathProvider.h"

#include "ScanTask.h"

LPCTSTR szDLLName = _T("NavTasks.dll");

typedef CSymInterfaceDLLHelper<&szDLLName,
                               ccSym::CNAVPathProvider, 
							   cc::CSymInterfaceTrustedCacheLoader,
                               IScanTask, 
                               &IID_IScanTask, 
                               &IID_IScanTask> NavScanTaskLoader;
