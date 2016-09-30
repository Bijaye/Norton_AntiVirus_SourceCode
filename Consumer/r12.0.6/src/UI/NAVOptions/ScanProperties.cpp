#include "StdAfx.h"
#include "scanproperties.h"

CScanProperties::CScanProperties(void)
{
}

CScanProperties::~CScanProperties(void)
{
}

int CScanProperties::GetProperty(const char* szProperty, int iDefault)
{
    return iDefault;
}

const char* CScanProperties::GetProperty(const char* szProperty, const char* szDefault)
{
    return szDefault;
}