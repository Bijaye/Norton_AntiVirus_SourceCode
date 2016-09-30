////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccLogFactoryHelper.h"
#include "NAVSettings.h"

// Options
//
#include "optnames.h"   // Names of options
#define LOG_FILE_MAX_SIZE 1024*1024*1024 // 1 gig

class CNAVEventLogFactory :
    public ccEvtMgr::CLogFactoryHelper 
{
public:
	CNAVEventLogFactory();
	virtual ~CNAVEventLogFactory();

    ccEvtMgr::CError::ErrorType SetMaxLogSize(long nEventType, 
                                                ULONGLONG uSize);
    ccEvtMgr::CError::ErrorType GetMaxLogSize(long nEventType, 
                                                ULONGLONG& uSize);

    ccEvtMgr::CError::ErrorType SetLogEnabled(long nEventType, 
                                                bool bEnabled);

    ccEvtMgr::CError::ErrorType GetLogEnabled(long nEventType, 
                                                bool& bEnabled);

protected:
	NAVToolbox::CCSettings m_ccSettings;

	void saveOptions();
	void loadOptions();
    static const EventLogInfo m_EventLogInfoArray[];

private:
    CNAVEventLogFactory(const CNAVEventLogFactory&);
    CNAVEventLogFactory& operator =(const CNAVEventLogFactory&);

};