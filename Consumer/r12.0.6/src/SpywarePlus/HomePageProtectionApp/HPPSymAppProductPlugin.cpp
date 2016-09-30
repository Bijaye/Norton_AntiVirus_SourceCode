#include "StdAfx.h"

#include "ccAppPlugin.h"
#include "HPPSymAppProductPlugin.h"

CHPPSymAppProductPlugin::CHPPSymAppProductPlugin(void)
{
}

CHPPSymAppProductPlugin::~CHPPSymAppProductPlugin(void)
{
}

SYMRESULT CHPPSymAppProductPlugin::VerifyProduct()
{
    return SYM_OK;
}

unsigned int CHPPSymAppProductPlugin::GetPluginDLLCount()
{
    return 2;
}

const char * CHPPSymAppProductPlugin::GetPluginDLLPath( unsigned int uIndex )
{
    return NULL;
}

const char * CHPPSymAppProductPlugin::GetProductName()
{
    return NULL;
}
