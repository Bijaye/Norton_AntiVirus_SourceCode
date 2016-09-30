////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccEventUtil.h"
#include "ccEventFactoryHelper.h"


class CEventFactory : public ccEvtMgr::CEventFactoryHelper
{
public:
    CEventFactory();
    virtual ~CEventFactory();

private:
    CEventFactory(const CEventFactory&);
    CEventFactory& operator =(const CEventFactory&);

public:
    DECLARE_EVENT_FACTORY();

protected:
    static const long m_EventTypeArray[];
};
