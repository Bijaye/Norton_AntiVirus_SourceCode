
#pragma once

#include "ccEraserInterface.h"


void DumpRemediation(size_t nIndex, const ccEraser::IRemediationAction* pRemediation) throw();
void DumpAnomaly(const ccEraser::IAnomaly* pAnomaly) throw();

template<typename T, const SYMGUID* _IID>
HRESULT CreateEraserObject(ccEraser::IEraser4* pEraser, cc::IStream* pStream, T*& t)
{
	utils::FreeObject(t);
	ISymBasePtr pBase;
	ccEraser::eResult eRes = pEraser->CreateObjectFromStream(pStream, NULL, pBase.m_p);
	if(ccEraser::Failed(eRes))
	{
		CCTRCTXE1(_T("Failed to create eraser obj: %d"), eRes);
		return E_FAIL;
	}
	SYMRESULT sr = pBase->QueryInterface(*_IID, (void**)&t);
	if(SYM_FAILED(sr))
	{
		CCTRCTXE0(_T("Failed to QI for eraser obj"));
		return E_NOINTERFACE;
	}
	return S_OK;
}
