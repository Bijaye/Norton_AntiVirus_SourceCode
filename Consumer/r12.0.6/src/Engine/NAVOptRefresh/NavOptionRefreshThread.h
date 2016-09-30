// NavOptionRefreshThread.h: interface for the CNavOptionRefreshThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVOPTIONREFRESHTHREAD_H__AB0F875B_20F0_431D_86CD_766FE3FB9AF3__INCLUDED_)
#define AFX_NAVOPTIONREFRESHTHREAD_H__AB0F875B_20F0_431D_86CD_766FE3FB9AF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////
/*

_IT_ == INSIDE THREAD
_IT_Functions(...).  Member functions called from INSIDE the THREAD.

*/
////////////////////////////////////////////
typedef enum _NAVOPTIONREFRESHTHREADEVENT
{
    NORTE_ForceRefresh = 1
}NAVOPTIONREFRESHTHREADEVENT;

class CNavOptionRefreshThreadSink
{
public:
    virtual void OnEvent(NAVOPTIONREFRESHTHREADEVENT idEvent) = 0;
};
class CNavOptionRefreshThreadSource:
    public StahlSoft::CEventSourceT2<CNavOptionRefreshThreadSink>
{
public:
    void Fire_OnEvent(NAVOPTIONREFRESHTHREADEVENT idEvent)
    {
        iterator it = begin();
        while(it != end())
        {
            (*it)->OnEvent(idEvent);
            ++it;
        }
    }
};

////////////////////////////////////////////
class CNavOptionRefreshThread  :
	public StahlSoft::CSystemThreadBase
    ,public CNavOptionRefreshThreadSource
{
public:
	CNavOptionRefreshThread();
	virtual ~CNavOptionRefreshThread();

	STDMETHOD(RunInstance)();                               //_IT_ == INSIDE THREAD
	STDMETHOD(RequestExit)();

private:
	StahlSoft::CSmartHandle m_shStop;
	StahlSoft::CSmartHandle m_shOnDemandRefresh;
	StahlSoft::CSmartHandle m_shLicenseChangeEvent;
    
    // Is there a Symantec product uninstalling?
    bool isUninstalling();
	HRESULT _IT_InternalRunInstance();			            //_IT_ == INSIDE THREAD
};

#endif // !defined(AFX_NAVOPTIONREFRESHTHREAD_H__AB0F875B_20F0_431D_86CD_766FE3FB9AF3__INCLUDED_)
