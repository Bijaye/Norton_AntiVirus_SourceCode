/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


#ifndef _PROCCESSSAMPLES_H_
#define _PROCCESSSAMPLES_H_
#include "qserver.h"
#define NOITEMFETCHED		10

class CSample : public CObject
{
public:
	CSample () {m_pQSItem = NULL;};
	~CSample ()
			{
				if(m_pQSItem)
				{
					m_pQSItem->Release();
					m_pQSItem=NULL;
				}
			};

	IQuarantineServerItem* m_pQSItem;

private:

};


class CSampleList : public CObList
{
public:
	CSampleList () {dwNoItems= 0;};
	~CSampleList (){};
private:
	DWORD dwNoItems;
};

HRESULT ProcesSamples (DWORD dwModuleData, BOOL *pStopping);

#endif