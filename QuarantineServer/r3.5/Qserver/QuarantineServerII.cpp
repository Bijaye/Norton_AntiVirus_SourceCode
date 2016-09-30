/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// QuarantineServerII.cpp : Implementation of CQuarantineServerII
#include "stdafx.h"
#include "Qserver.h"
#include "QuarantineServerII.h"
#include "const.h"
#include "qscommon.h"
/////////////////////////////////////////////////////////////////////////////
// CQuarantineServerII


STDMETHODIMP CQuarantineServerII::GetVersion(ULONG *pulVersion)
{
	HRESULT hr = S_OK;
	ULONG ulVer=0;

	if(pulVersion == NULL)
		hr = E_INVALIDARG;
	else
	{
		if (GetVersionFromReg(&ulVer)== S_OK)
			*pulVersion = ulVer;
		else
			*pulVersion = QSVERISON;
	}
	return hr;
}


HRESULT CQuarantineServerII::GetVersionFromReg(ULONG *pulVer)

{
	HRESULT hr = S_OK;
	CRegKey reg;
	ULONG ulVer = 0;

	if (pulVer == NULL)
		return E_INVALIDARG;

	*pulVer = 0;
	
	if( ERROR_SUCCESS != reg.Open( HKEY_LOCAL_MACHINE, REGKEY_QSERVER, KEY_ALL_ACCESS) )
	    hr = E_FAIL;
	else
	{

	    if( ERROR_SUCCESS == reg.QueryDWORDValue( REGVALUE_VERSION,ulVer ) )
		{
			*pulVer = ulVer;
		}
		else
		{
			reg.SetDWORDValue(REGVALUE_VERSION,QSVERISON);
			*pulVer = QSVERISON;
		}
	}



	return hr;

}

STDMETHODIMP CQuarantineServerII::ResetQuarantineConfigInterface()
{
    ULONG ulOld;
	// 
    // Decrement global count of config objects
    // 
     ulOld = InterlockedExchange( &_Module.m_iConfigCount,0 );

	return S_OK;
}

STDMETHODIMP CQuarantineServerII::OpenPurgeLog(ULONG *pulLogHandle)
{
	// TODO: Add your implementation code here

	return S_OK;
}
