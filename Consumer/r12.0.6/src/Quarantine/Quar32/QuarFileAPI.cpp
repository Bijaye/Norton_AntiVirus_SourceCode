
#include "Stdafx.h"
#include "QuarFileAPI.h"

CQuarFileAPI::CQuarFileAPI()
{
	m_bOriginalANSI = (AreFileApisANSI() ? true : false);
}
	
CQuarFileAPI::~CQuarFileAPI()
{
	RestoreOriginalAPI();
}

void CQuarFileAPI::SwitchAPIToANSI()
{
	SetFileApisToANSI();
}

void CQuarFileAPI::SwitchAPIToOEM()
{
	SetFileApisToOEM();
}

void CQuarFileAPI::RestoreOriginalAPI()
{
	if (m_bOriginalANSI == true)
	{
		SetFileApisToANSI();
	}
	else
	{
		SetFileApisToOEM();
	}
}

bool CQuarFileAPI::IsOriginalANSI() const
{
	return m_bOriginalANSI;
}	