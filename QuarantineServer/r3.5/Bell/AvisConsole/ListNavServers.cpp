/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


#include "stdafx.h"
#include "nsctop.h"
#include "nsctop_i.c"
#include "ListNavServers.h"


///////////////////////////////////////////////////////////////////////////////
//
// Function name: ListNavServers.
//
// Description  : Creates a list of navServers
//
///////////////////////////////////////////////////////////////////////////////
// 01-04-00 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT ListNavServers(SServerData **pSServerData, long *pdwNoRecords)
{
	HRESULT hr = S_OK;
 	IServerContainer2* pServers = 0;
	SServerData	*psdList = NULL;
	long dwNoRecords=0;

	// Check params
	if (pSServerData == NULL || pdwNoRecords == NULL)
		return E_INVALIDARG;
	
	// Setup default return Params incase something goes wrong
	*pSServerData=NULL;
	*pdwNoRecords = 0;
	
	// get topology interface to a server container
	hr = ::CoCreateInstance( CLSID_ServerContainer, NULL, CLSCTX_LOCAL_SERVER, IID_IServerContainer2, (void**) &pServers );
    // ATLASSERT( SUCCEEDED(hr) );

	if (SUCCEEDED(hr) )
	{
		// we are good to go get number if items and allocate memory for server list.
		hr = pServers->Count(&dwNoRecords);
		if (SUCCEEDED(hr) )
		{
			psdList = new SServerData[dwNoRecords];
			if (psdList == NULL)
			{
				hr = E_OUTOFMEMORY;
			}
			else
			{
				IEnumServers* pes = 0;
				ULONG cFetched = 0;

				// get server enumerator from container
				hr = pServers->_NewEnum(&pes);
				if(SUCCEEDED(hr) )
				{
					// get the server data records
					hr = pes->Next( dwNoRecords, psdList, &cFetched );
					if (SUCCEEDED(hr))
					{
						// if it worked setup return values.
						*pdwNoRecords = cFetched;
						*pSServerData = psdList;
					}
					pes->Release();
					pes= NULL;
				}
			}
		}
	}


	if (pServers)
		pServers->Release();
	return hr;
}