// StdAfx.h

#pragma once

#define _WIN32_WINNT 0x0400

#include "windows.h"
#include "tchar.h"
#include <crtdbg.h>
#include <atlbase.h>
#include <atlstr.h>
#include <atlcom.h>
#include "StahlSoft.h"

#include "ccLib.h"
#include "ccCoInitialize.h"
#include "ccEvent.h"

extern ccLib::CEvent g_eventTrayIcon;
extern CString g_csProductName;
