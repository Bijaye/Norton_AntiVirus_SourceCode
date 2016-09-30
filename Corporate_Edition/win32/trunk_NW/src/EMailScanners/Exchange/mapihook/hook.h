// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef __HOOK_H__
#define __HOOK_H__

#include "smartptr.h"
#include "comm.h"

class CHook;

/*---------------------------------------------------------------------------*/

class RTActionData
{
public :
	ACTIONDATA action;
	RTActionData *pNext;
	RTActionData() : pNext(0) { action.Initialize(); }
	~RTActionData() { if (pNext) delete pNext; }
};

/*---------------------------------------------------------------------------*/

class RTScanData
{
public :
	enum Constants { SIGNATURE = 'DSTR' };

	const enum Constants signature;
	CHook *pCHook;
	CMessage *pCMessage;
	CMsgStore *pCMsgStore;
	LPEXCHEXTCALLBACK peecb;
	int messageCommitRequired;
	int virusFound;
	EXTINFO extInfo;
	RTActionData *pLast;

	RTScanData() : signature(SIGNATURE), pCHook(0), pCMessage(0), messageCommitRequired(false), virusFound(false), pLast(0) { extInfo.Initialize(); };
	void AddActionData(RTActionData *pAdd) { pAdd->pNext = pLast; pLast = pAdd; }
	~RTScanData() { if (pLast) delete pLast; }

private:
    // copy constructor removed from interface
    RTScanData(const RTScanData &source);
 
    // assignment op removed from interface
    RTScanData & operator = (const RTScanData &source);

};

/*---------------------------------------------------------------------------*/

class CHook : public IExchExtMessageEvents
{
public :
	enum Constants { SIGNATURE = 'HCDL' };
	MAPI_IUNKNOWN_METHODS(IMPL);

	STDMETHOD(OnRead)(LPEXCHEXTCALLBACK	lpeecb);
	STDMETHOD(OnReadComplete)(LPEXCHEXTCALLBACK	lpeecb, ULONG ulFlags);
	STDMETHOD(OnWrite)(LPEXCHEXTCALLBACK lpeecb);
	STDMETHOD(OnWriteComplete)(LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags);
	STDMETHOD(OnCheckNames)(LPEXCHEXTCALLBACK lpeecb);
	STDMETHOD(OnCheckNamesComplete)(LPEXCHEXTCALLBACK lpeecb, ULONG	ulFlags);
	STDMETHOD(OnSubmit)(LPEXCHEXTCALLBACK lpeecb);
	STDMETHOD_(void, OnSubmitComplete)(LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags);

	STDMETHOD_(ULONG, NonDelegatingRelease)(void);

	CHook(IUnknown *pParent, MAPIAllocators& alloc) :
		m_signature(SIGNATURE),
		m_allocators(alloc),
		m_cRef(1) { m_pParent = pParent; }

	~CHook();

	HRESULT Initialize(void);

private :
	const DWORD	m_signature;	// Identifying signature (checked by operator delete)

	MAPIAllocators& m_allocators;

	DWORD m_cRef;

	HRESULT m_hrOnReadComplete;

	IUnknown *m_pParent;

	int ScanThisAttachment(CAttach *pAttach, RTScanData *pData);
	HRESULT ModifyMessageBody(RTScanData *pData);
	HRESULT SendNotificationMessages(RTScanData *pData);

	static DWORD m_cInstances;
	static HRESULT RealTimeScanAttachment(CAttach *pAttach, void *pvData);
	static HRESULT MoveAttachmentRendering(CAttach *pAttach, void *pvData);


private:
    // default constructor removed from interface
    CHook();

    // copy constructor removed from interface
    CHook(const CHook &source);
 
    // assignment op removed from interface
    CHook & operator = (const CHook &source);
};

/*---------------------------------------------------------------------------*/

class CExchExt : public IExchExt
{
public :
	enum Constants { SIGNATURE = 'PVDL' };

	// COM interface methods

	MAPI_IUNKNOWN_METHODS(IMPL);
	STDMETHOD(Install)(LPEXCHEXTCALLBACK pmecb, ULONG mecontext, ULONG ulFlags);

	// CExchExt special member functions

	CExchExt() : 
		m_signature(SIGNATURE),
		m_context(0),
		m_bMAPILoaded(false),
		m_cRef(1) { DebugOut(SM_GENERAL, "CExchExt object created [%x]", this); }

	~CExchExt() { DebugOut(SM_GENERAL, "CExchExt object destroyed [%x]", this);
					if (m_pCHook) m_pCHook->NonDelegatingRelease();
					if (m_bMAPILoaded) UnloadMAPI(); }

	// CHook Methods

	HRESULT Initialize(void);

private :
	const DWORD	m_signature;	// Identifying signature (checked by operator delete)

	UINT m_context;				// Client Context
	int m_bMAPILoaded;			// MAPI DLLs loaded
   
	DWORD m_cRef;				// This objects reference count

	MAPIAllocators m_allocators;
	// MAPI HEAP allocators.  All memory passed to MAPI must be allocated by these functions.
	// These are set by the constructor (also used by operators new and delete)

    CHook *m_pCHook;

    // copy constructor removed from interface
    CExchExt(const CExchExt &source);
 
    // assignment op removed from interface
    CExchExt & operator = (const CExchExt &source);

};

#endif //__HOOK_H__