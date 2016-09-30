#include "StdAfx.h"

#include "ccEventEx.h"
#include "ccEventUtil.h"
//#include "SNLog.h"
#include "ccEventManager_h.h"

#include "ccInstanceHelper.h"

#include "SNLogEventFactory.h"

using namespace ccLib;
using namespace ccEvtMgr;

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

const long CEventFactory::m_EventTypeArray[] = 
{
	0
};

CEventFactory::CEventFactory()
    : CEventFactoryHelper(m_EventTypeArray,
							CCDIMOF(m_EventTypeArray)) 
{
}

CEventFactory::~CEventFactory()
{
}

BEGIN_NEW_EVENT(CEventFactory)
END_NEW_EVENT()

BEGIN_COPY_EVENT_CPP_TO_CPP(CEventFactory)
END_COPY_EVENT_CPP_TO_CPP()

BEGIN_COPY_EVENT_COM_TO_CPP(CEventFactory)
END_COPY_EVENT_COM_TO_CPP()

BEGIN_COPY_EVENT_CPP_TO_COM(CEventFactory)
END_COPY_EVENT_CPP_TO_COM()

DELETE_EVENT(CEventFactory)