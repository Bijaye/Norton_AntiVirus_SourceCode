// SymSyncEvent.cpp : Implementation of CSymSyncEvent
#include "stdafx.h"
#include "SymObjectSync.h"
#include "SymSyncEvent.h"
#include "SyncHelper.cpp"
/////////////////////////////////////////////////////////////////////////////
// CSymSyncEvent

STDMETHODIMP CSymSyncEvent::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ISymSyncEvent
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
        if (ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


STDMETHODIMP CSymSyncEvent::InitializeEvent(/*[in]*/ BOOL bManualReset,/*[in]*/BOOL bInitialState,/*[in]*/BSTR name)
{
    USES_CONVERSION;
    // TODO: Add your implementation code here
    m_smEvent = ::CreateEvent(NULL,bManualReset,bInitialState,OLE2T(name));
	return S_OK;
}

STDMETHODIMP CSymSyncEvent::SetEvent(BOOL *pReturn)
{
	// TODO: Add your implementation code here
    if(m_smEvent)
    {
        *pReturn = ::SetEvent(m_smEvent);
    }
    else
        *pReturn = FALSE;

	return S_OK;
}

STDMETHODIMP CSymSyncEvent::ResetEvent(BOOL *pReturn)
{
	// TODO: Add your implementation code here
    if(m_smEvent)
    {
        *pReturn = ::ResetEvent(m_smEvent);
    }
    else
        *pReturn = FALSE;

	return S_OK;
}

STDMETHODIMP CSymSyncEvent::Wait(long timeoutInMilliSeconds,/*[in]*/BOOL bWaitWithMessageLoop,long* pReturn)
{
	// TODO: Add your implementation code here
    if(bWaitWithMessageLoop)
        *pReturn = (long)StahlSoft::WaitForSingleObjectWithMessageLoop(m_smEvent,DWORD(timeoutInMilliSeconds));
    else
        *pReturn = (long)::WaitForSingleObject(m_smEvent,DWORD(timeoutInMilliSeconds));
	return S_OK;
}
