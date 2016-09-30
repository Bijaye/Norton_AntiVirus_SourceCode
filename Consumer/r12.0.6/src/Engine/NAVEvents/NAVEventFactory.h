// NAVEventFactory.h: interface for the CNAVEventFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVEVENTFACTORY_H__66A0E0FE_4603_4B67_90D0_0EF61F911C02__INCLUDED_)
#define AFX_NAVEVENTFACTORY_H__66A0E0FE_4603_4B67_90D0_0EF61F911C02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ccInstanceHelper.h" // For CCNewComObject
#include "ccEventFactoryHelper.h"

#include "AllNAVEvents.h"
#include "NAVEventCommon.h"
#include "NAVEventCommonInt.h"

#include "NAVEvents.h"

class CNAVEventFactory : public ccEvtMgr::CEventFactoryHelper
{
public:
	CNAVEventFactory();
	virtual ~CNAVEventFactory();

    virtual ccEvtMgr::CError::ErrorType NewEvent(long nEventType,
                                                 ccEvtMgr::CEventEx*& pEvent); 
    virtual ccEvtMgr::CError::ErrorType CopyEvent(const ccEvtMgr::CEventEx& Src, 
                                                  ccEvtMgr::CEventEx*& pDst); 
    virtual ccEvtMgr::CError::ErrorType CopyEvent(IEventEx* pSrc, 
                                                  ccEvtMgr::CEventEx*& pDst);
    virtual ccEvtMgr::CError::ErrorType CopyEvent(const ccEvtMgr::CEventEx& Src, 
                                                  IEventEx*& pDst);
    virtual ccEvtMgr::CError::ErrorType DeleteEvent(ccEvtMgr::CEventEx* pEvent); 
    virtual ccEvtMgr::CError::ErrorType DeleteEvent(IEventEx* pEvent);


protected:
    static const long m_EventTypeArray[];

private:
    CNAVEventFactory(const CNAVEventFactory&);
    CNAVEventFactory& operator =(const CNAVEventFactory&);
};



#endif // !defined(AFX_NAVEVENTFACTORY_H__66A0E0FE_4603_4B67_90D0_0EF61F911C02__INCLUDED_)
