// VirusAlertCategory.h: interface for the CVirusAlertCategory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIRUSALERTCATEGORY_H__C6053CEF_AC36_4898_8B4F_7C53C037CBBB__INCLUDED_)
#define AFX_VIRUSALERTCATEGORY_H__C6053CEF_AC36_4898_8B4F_7C53C037CBBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ccLogViewerInterface.h"
#include "NAVLogV.h"
#include "AllNAVEvents.h"
#include "AVEvent.h"

#include "ccLogManagerHelper.h"

#include "resourceHelper.h"
#include "AVCategory.h"
#include "AVNamesMap.h"

class CVirusAlertCategory : public CAVCategory  
{
public:
	CVirusAlertCategory();
	virtual ~CVirusAlertCategory();
                            
protected:
    // override from CAVCategory
    HRESULT GetEvent        (int iEventIndex, cc::ILogViewerEvent*& pEvent);
    virtual void    deleteEvents ();

    enum // Columns
    {
        columnDate = 0,
        columnFeature,          // AV feature that found the problem
        columnVirusName,        // Virus name, if known.
        columnResult,           // Final action AV took on the object
        columnItemType,         // Type of object (file, BR, MBR)
        columnTarget,           // Offending application
        columnActionAttempted,  // Action the virus-like app tried
        columnDefsRevision,     // Revision of the defs that found the infection
        columnProductVersion,   // Product version
        columnUserName,         // User name
        columnComputerName,     // Computer name
        columnLast              // Always the last
    };

    // We don't use this function.
    //
    BOOL SetEventProperties(ccEvtMgr::CEventEx* pCEvent,
                            const ccEvtMgr::CLogFactoryEx::EventInfo& Info,
                            cc::ILogViewerEvent* pLogViewerEvent){return FALSE;}

    HRESULT GetDefaultSort (int& iColumn /* [out] */, int& iSortOrder /* [out] */);
   
    // Override this so we can pre-process our events
    HRESULT loadBatch ();
    void clearSearchContexts();

    typedef ::std::vector<cc::ILogViewerEvent*> vecLogEvents;
    vecLogEvents::iterator iterLogEvents;
    vecLogEvents m_pProcessedLogEvents;
    
    BOOL processLogEvents( ccEvtMgr::CEventEx* pCEvent,
                           const ccEvtMgr::CLogFactoryEx::EventInfo& Info);
};

#endif // !defined(AFX_VIRUSALERTCATEGORY_H__C6053CEF_AC36_4898_8B4F_7C53C037CBBB__INCLUDED_)
