#pragma once

#include "ccServiceApp.h"
#include "ccConsoleWindow.h"

class CNPFMonitorApp : public ccLib::CServiceApp
{
public:
	CNPFMonitorApp(void);
	virtual ~CNPFMonitorApp(void);

protected:
	//////////////////////////////
	// CComService overrides.

    virtual int Start();

protected:
    BOOL CreateConsoleWindows();
    BOOL DestroyConsoleWindows();

public:
	//////////////////////////////
	// Output window for debug output 
	ccLib::CConsoleWindow m_DebugWnd;
};

extern CNPFMonitorApp _App;