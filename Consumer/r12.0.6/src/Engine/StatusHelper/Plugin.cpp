#include "StdAfx.h"
#include "plugin.h"

CPlugin::CPlugin(void)
{
	CCTRACEI("CPlugin()");
	m_shutdown = false;
}

CPlugin::~CPlugin(void)
{
}

//////////////////////////////////////////////////////////////////////
// virtual void RequestStop();

void CPlugin::RequestStop()
{
	CCTRACEI("RequestStop");
	m_shutdown = true;
    m_eventShutdown.SetEvent ();
}

//////////////////////////////////////////////////////////////////////
// virtual void CanStopNow();

bool CPlugin::CanStopNow()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
// virtual void GetObjectName();

const char * CPlugin::GetObjectName()
{
	return m_szObjectName;
}

//////////////////////////////////////////////////////////////////////
// virtual unsigned long OnMessage( int iMessage, unsigned long ulParam );

unsigned long CPlugin::OnMessage( int iMessage, unsigned long ulParam )
{
	return 0;
}