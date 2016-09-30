#pragma once

#include "IDSAlertExclusions.h"
#include "ccEventEx.h"
#include "symfwagtinterface.h"

class CAlertHandlerIDS
{
public:
	CAlertHandlerIDS(void) throw();
	virtual ~CAlertHandlerIDS(void) throw();

private:
	CAlertHandlerIDS(const CAlertHandlerIDS&) throw();
	CAlertHandlerIDS& operator=(const CAlertHandlerIDS&) throw();

public:
	IAlertData::ALERT_RESULT DisplayAlert(ccEvtMgr::CEventEx* pEvent) throw();

private:
	CIDSAlertExclusions m_IDSExclusions;
};
