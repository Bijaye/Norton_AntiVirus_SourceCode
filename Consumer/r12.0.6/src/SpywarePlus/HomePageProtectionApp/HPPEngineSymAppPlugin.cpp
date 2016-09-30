#include "StdAfx.h"

#include "ccAppPlugin.h"
#include "HPPEngineSymAppPlugin.h"

CHPPEngineSymAppPlugin::CHPPEngineSymAppPlugin(void)
{
}

CHPPEngineSymAppPlugin::~CHPPEngineSymAppPlugin(void)
{
}

//****************************************************************************
//****************************************************************************
void CHPPEngineSymAppPlugin::Run( ICCApp* pCCApp )
{
    return;
}

//****************************************************************************
//****************************************************************************
bool CHPPEngineSymAppPlugin::CanStopNow()
{
    return TRUE;
}

//****************************************************************************
//****************************************************************************
void CHPPEngineSymAppPlugin::RequestStop()
{
    return;
}

//****************************************************************************
//****************************************************************************
const char * CHPPEngineSymAppPlugin::GetObjectName()
{
    return _T("Home Page Protection Engine SymApp Plugin");
}

//****************************************************************************
//****************************************************************************
unsigned long CHPPEngineSymAppPlugin::OnMessage( int iMessage, unsigned long ulParam )
{
    return 0;
}
