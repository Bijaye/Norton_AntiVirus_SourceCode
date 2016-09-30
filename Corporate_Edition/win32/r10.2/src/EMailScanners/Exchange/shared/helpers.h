// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef _HELPERS_H_
#define _HELPERS_H_

#include <string.h>
#include <crtdbg.h>

IUnknown* AtlComPtrAssign(IUnknown** pp, IUnknown* lp);
IUnknown* AtlComQIPtrAssign(IUnknown** pp, IUnknown* lp, REFIID riid);

template <class T> class CComPtr
{
public:
	typedef T _PtrClass;
	CComPtr()							{ p = 0; }
	CComPtr(T* lp)						{ if ((p = lp) != 0) p->AddRef(); }
	CComPtr(const CComPtr<T>& lp)		{ if ((p = lp.p) != 0) p->AddRef(); }
	~CComPtr()							{ if (p) p->Release(); }
	void Release()						{ if (p) p->Release(); p = 0; }
	operator T*()						{ return (T*) p; }
	T& operator*()						{ SAVASSERT(p != 0); return *p; }
		//The assert on operator& usually indicates a bug.  If this is really
		//what is needed, however, take the address of the p member explicitly.
	T** operator&()						{ SAVASSERT(p == 0); return &p; }
	T* operator->()						{ SAVASSERT(p != 0); return p; }
	T* operator=(T* lp)					{ return (T*) AtlComPtrAssign((IUnknown**) &p, lp); }
	T* operator=(const CComPtr<T>& lp)	{ return (T*) AtlComPtrAssign((IUnknown**) &p, lp.p); }
	bool operator!()					{ return (p == 0); }
	T* p;
};

//Note: CComQIPtr<IUnknown, &IID_IUnknown> is not meaningful
//      Use CComPtr<IUnknown>
template <class T, const IID* piid> class CComQIPtr
{
public:
	typedef T _PtrClass;
	CComQIPtr()									{ p = 0; }
	CComQIPtr(T* lp)							{ if ((p = lp) != 0) p->AddRef(); }
	CComQIPtr(const CComQIPtr<T,piid>& lp)		{ if ((p = lp.p) != 0) p->AddRef(); }
		// If you get an error that this member is already defined, you are probably
		// using a CComQIPtr<IUnknown, &IID_IUnknown>.  This is not necessary.
		// Use CComPtr<IUnknown>
	CComQIPtr(IUnknown* lp)						{ p = 0; if (lp != 0) lp->QueryInterface(*piid, (void **)&p); }
	~CComQIPtr()								{ if (p) p->Release(); }
	void Release()								{ if (p) p->Release(); p = 0; }
	operator T*()								{ return p; }
	T& operator*()								{ SAVASSERT(p != 0); return *p; }
		//The assert on operator& usually indicates a bug.  If this is really
		//what is needed, however, take the address of the p member explicitly.
	T** operator&()								{ SAVASSERT(p == 0); return &p; }
	T* operator->()								{ SAVASSERT(p != 0); return p; }
	T* operator=(T* lp)							{ return (T*) AtlComPtrAssign((IUnknown**)&p, lp); }
	T* operator=(const CComQIPtr<T,piid>& lp)	{ return (T*) AtlComPtrAssign((IUnknown**)&p, lp.p); }
	T* operator=(IUnknown* lp)					{ return (T*) AtlComQIPtrAssign((IUnknown**)&p, lp, *piid); }
	bool operator!()							{ return (p == 0); }
	T* p;
};

#endif // _HELPERS_H_