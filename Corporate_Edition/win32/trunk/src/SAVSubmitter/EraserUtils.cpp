
#include "stdafx.h"
#include "EraserUtils.h"
#include "ccEraserInterface.h"
#include "ccKeyValueCollectionInterface.h"
#include "ccTrace.h"
#include "ccLib.h"

void DumpRemediation(size_t nIndex, const ccEraser::IRemediationAction* pRem) throw()
{
	if(pRem == NULL)
	{
		CCTRCTXE0(_T("pRem == NULL"));
		return;
	}
	cc::IStringPtr pDesc;
	ccEraser::eResult eRes = pRem->GetDescription(pDesc.m_p);
	if(ccEraser::Failed(eRes))
	{
		CCTRCTXE1(_T("No Description: %d"), eRes);
		return;
	}
	if(pDesc == NULL)
	{
		CCTRCTXE0(_T("pDesc == NULL"));
		return;
	}
	size_t nPropSize = 0;
	cc::IKeyValueCollectionConstPtr pProps;
	eRes = pRem->GetProperties(pProps.m_p);
	if(ccEraser::Succeeded(eRes) && pProps != NULL)
		nPropSize = pProps->GetCount();
	else
		CCTRCTXE1(_T("Failed to get props: %d"), eRes);

	CCTRCTXI3(_T("Remediation %Iu: %ls (%Iu properties)"), nIndex, pDesc->GetStringW(), nPropSize);
}

void DumpAnomaly(const ccEraser::IAnomaly* pAnomaly) throw()
{
	ISymBasePtr pBase;
	cc::IStringQIPtr pString;
	cc::IKeyValueCollectionConstPtr pProps;

	if(!g_DebugOutput.IsOutputEnabled(ccLib::CDebugOutput::eInformationDebug))
		return;

	if(pAnomaly == NULL)
	{
		CCTRCTXE0(_T("pAnomaly == NULL"));
		return;
	}
	ccEraser::eResult eRes = pAnomaly->GetProperties(pProps.m_p);
	if(ccEraser::Failed(eRes))
	{
		CCTRCTXE1(_T("Failed to get props: %d"), eRes);
		return;
	}
	pProps->GetValue(ccEraser::IAnomaly::Name, pBase.m_p);
	pString = pBase;
	if(pString)
		CCTRCTXI1(_T("\tName == %ls"), pString->GetStringW());
	else
		CCTRCTXI0(_T("\tName == NULL"));

	DWORD dw = DWORD(-1);
	pProps->GetValue(ccEraser::IAnomaly::VID, dw);
	CCTRCTXI1(_T("\tVID == %d"), dw);
}
