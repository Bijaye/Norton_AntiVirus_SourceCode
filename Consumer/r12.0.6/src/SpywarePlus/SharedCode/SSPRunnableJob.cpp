#include "StdAfx.h"
#include "SSPRunnableJob.h"

CSSPRunnableJob::CSSPRunnableJob()
{
	m_hRunning = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CSSPRunnableJob::~CSSPRunnableJob()
{
}

HRESULT CSSPRunnableJob::IsRunning()
{
	HRESULT hrReturn = S_OK;

	DWORD dwRet = WaitForSingleObject(m_hRunning, 0);
	if(WAIT_TIMEOUT == dwRet)
	{
		hrReturn = S_OK;
	}
	else if(WAIT_OBJECT_0 == dwRet)
	{
		hrReturn = S_FALSE;
	}
	else
	{
		hrReturn = E_FAIL;
	}

	return hrReturn;
}

HRESULT CSSPRunnableJob::RequestExit()
{
	BOOL bFailed = SetEvent(m_hRunning);

	return bFailed ? E_FAIL : S_OK;
}
