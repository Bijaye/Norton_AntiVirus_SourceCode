// NAVOptionRefreshEngine.h: interface for the CNAVOptionRefreshEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVOPTIONREFRESHENGINE_H__3B4CCA13_5AD6_4532_9358_E4448E15C1D8__INCLUDED_)
#define AFX_NAVOPTIONREFRESHENGINE_H__3B4CCA13_5AD6_4532_9358_E4448E15C1D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <simon.h>
#include "NAVOptRefresh.h"
#include "NavOptionRefreshThread.h"

class CNAVOptionRefreshEngine  :
     public SIMON::CSimonClass<CNAVOptionRefreshEngine,&CLSID_CNAVOptionRefreshEngine>  
    ,public SIMON::IInterfaceManagementImpl<false>
    ,public INAVOptionRefreshEngine
    ,public CNavOptionRefreshThreadSink
{
public:
	CNAVOptionRefreshEngine();
	virtual ~CNAVOptionRefreshEngine();

    SIMON_INTERFACE_MAP_BEGIN()
    	SIMON_INTERFACE_ENTRY( IID_INAVOptionRefreshEngine ,INAVOptionRefreshEngine  )
    SIMON_INTERFACE_MAP_END()
    //INAVOptionRefreshEngine
    SIMON_STDMETHOD(Init)();
    SIMON_STDMETHOD(Start)();
    SIMON_STDMETHOD(Stop)();

    void OnEvent(NAVOPTIONREFRESHTHREADEVENT idEvent)
    {

    }

private:
    StahlSoft::CSmartHandle m_mutex;
    StahlSoft::CMutexLock   m_mutexLock;

    CNavOptionRefreshThread m_thread;
    StahlSoft::CRunnableHostThread m_rhtAboveNormalThread;

};

#endif // !defined(AFX_NAVOPTIONREFRESHENGINE_H__3B4CCA13_5AD6_4532_9358_E4448E15C1D8__INCLUDED_)
