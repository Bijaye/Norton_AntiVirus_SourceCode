// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// RtvControler.h - CNAV declaration

#ifndef __RtvControler_h__
#define __RtvControler_h__
#include <atlbase.h>
#include "pscan.h"

class CNAV
{
	class CEnvironment
	{
#ifdef WIN95
		HRESULT m_hr;
#else  WIN95
		SC_HANDLE m_schSCManager;
		SC_HANDLE m_schService;
		operator SC_HANDLE&(void) { return m_schService; }
		friend class CNAV;
#endif WIN95
	public:
		CEnvironment(void);
		~CEnvironment(void);
		operator bool(void);
	} m_env;

public:
	BOOL Running(void);
	BOOL Start(void);
	BOOL Stop(void);
};

#endif __RtvControler_h__
