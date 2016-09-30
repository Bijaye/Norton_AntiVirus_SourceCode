// AVCategory.h: interface for the CAVCategory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVCATEGORY_H__8BA674CE_5A97_4ECC_B89C_DA921E6CEE3F__INCLUDED_)
#define AFX_AVCATEGORY_H__8BA674CE_5A97_4ECC_B89C_DA921E6CEE3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

#include "ccLogViewerInterface.h"
#include "ccLogViewerPluginId.h"
#include "NAVLogV.h"
#include "AllNAVEvents.h"
#include "AVEvent.h"
#include "NAVEventFactoryLoader.h"
#include "NAVInfo.h"        // toolbox
#include "HTMLFormat.h"

#include "ccLogManagerHelper.h"
#include "ccEventManagerHelper.h"

#include <vector>
#include <string>
#include <map>
#include <list>
#include "tchar.h"

#include "resourcehelper.h"
#include "AVNamesMap.h"

// Helper class for handing loaded data from the log
//
class CLogEventData
{
    public :
        CLogEventData (const ccEvtMgr::CEventEx* pEvent,
                    ccEvtMgr::CLogFactoryEx::EventInfo EventInfo)
        {
            if  ( pEvent )
            {
                m_pEvent = pEvent;
                m_EventInfo.m_uIndex = EventInfo.m_uIndex;
                m_EventInfo.m_dwIndexCheck = EventInfo.m_dwIndexCheck;
                m_EventInfo.m_nEventType = EventInfo.m_nEventType;
                m_EventInfo.m_TimeStamp = EventInfo.m_TimeStamp;
                m_EventInfo.m_dwSequenceNumber = EventInfo.m_dwSequenceNumber;
            }
        }
        ~CLogEventData (){};

        ccEvtMgr::CEventExPtr m_pEvent;
        ccEvtMgr::CLogFactoryEx::EventInfo m_EventInfo;

    private:
        // Disallowed
        CLogEventData ();
};


typedef ::std::vector <::std::string> vecColumnTitles;

class CAVCategory :
    public cc::ILogViewerCategory,
	public ISymBaseImpl<CSymThreadSafeRefCount>   
{
public:
    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY(cc::IID_LogViewerCategory, cc::ILogViewerCategory)
	SYM_INTERFACE_MAP_END() 

	CAVCategory();
	virtual ~CAVCategory();

// ** ILogViewerCategory
	HRESULT GetCategoryID   (int& iCategoryID);
	HRESULT GetImageList    (HIMAGELIST& hImageList);
	HRESULT GetColumnCount  (int& iColumnCount);
	HRESULT GetEventCount   (int& iEventCount);
    HRESULT GetEvent        (int iEventIndex, cc::ILogViewerEvent*& pEvent);

	HRESULT GetCategoryName (LPTSTR szCategoryName,/* [in/out] */ DWORD& dwSize);
    HRESULT GetCategoryDescription  ( LPTSTR szCategoryDesc, /* [in/out] */ DWORD& dwSize);
                            
	HRESULT GetColumnName   (int iColumn,
                            LPTSTR szColumnName,
	   					    /* [in/out] */ DWORD& dwSize);

    HRESULT SetLogFileSize  (int iLogFileSize);
	HRESULT GetLogFileSize  (int& iLogFileSize);

    HRESULT LaunchHelp      ();

	HRESULT ClearCategory   ();
    HRESULT GetLoggingState (BOOL& bEnabled);
    HRESULT SetLoggingState (BOOL bEnabled);

    virtual HRESULT GetDefaultSort (int& iColumn /* [out] */, int& iSortOrder /* [out] */);
// ** end ILogViewerCategory

	static CAVFeatureNameMap m_Features;

protected:
	virtual void    clearSearchContexts();
    virtual HRESULT loadBatch ();
    virtual void    deleteEvents ();

    long m_nEventCount;     // # events
    long m_nContextID;      // search context
    bool m_bViewerInit;     // has the viewer asked us about #events yet?

    ccEvtMgr::CLogManagerHelper::EventInfoVector m_EventInfoVector;
    typedef ::std::vector <CLogEventData*> vecEventDataPtr;
    vecEventDataPtr m_vecEventDataPtr;

	int m_iCategoryID;
    DWORD m_dwHelpID;

    ::std::string m_strCategoryName;
    ::std::string m_strCategoryDescription;
    vecColumnTitles m_vecColumnTitles;
    HIMAGELIST m_hEventIcons;

	// Pointer to event manager object
    ccEvtMgr::CEventFactoryEx2Ptr m_pEventFactory;
    ccEvtMgr::CProxyFactoryExPtr m_pProxyFactory;

    CSymStaticRefCount<ccEvtMgr::CEventManagerHelper> m_EventManager;

    ccEvtMgr::CLogManagerHelper m_LogManagerHelper;

    // The factory for this category
    ISymFactoryPtr m_pFactory;

    SYMOBJECT_ID m_ObjectID;

    // Implemented by each category
    //
    virtual BOOL SetEventProperties(ccEvtMgr::CEventEx* pCEvent,
                                    const ccEvtMgr::CLogFactoryEx::EventInfo& Info,
                                    cc::ILogViewerEvent* pLogViewerEvent) = 0;
    long m_nEventType;  // Set by derived classes

    static _variant_t g_vComputerName; // it's all the same machine so... why store it?
	static CString m_csProductName;
	static CString m_csThreatInfoURL;
};

#endif // !defined(AFX_AVCATEGORY_H__8BA674CE_5A97_4ECC_B89C_DA921E6CEE3F__INCLUDED_)
