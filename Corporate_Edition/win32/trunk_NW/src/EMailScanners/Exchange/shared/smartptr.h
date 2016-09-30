// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef __SMARTPTR_H__
#define __SMARTPTR_H__

template <class T> class SmartPtr
{
public:
	SmartPtr(T* pT = 0)							{ owner = (p = pT) != 0; }
	SmartPtr(SmartPtr<T>& srT)					{ owner = srT.owner; p = srT.Detach(); }
	~SmartPtr()									{ _cleanup(); }
	T* Detach(void)								{ owner = 0; return p; }
	void Release(void)							{ _cleanup(); Reset(); }
	void Reset(void)							{ owner = 0; p = 0; }
	operator T*()								{ return (T*) p; }
	T& operator*()								{ SAVASSERT(p != 0); return *p; }
	// operator&() is used for assignment as an OUT only parameter.  If the pointer is to be used as an IN/OUT
	// parameter, take the address of the 'p' member explicitly.  operator&() assumes that ownership is about to
	// be granted to this pointer.  Reset() or Detach() member functions can be called to relinquish ownership.
	T** operator&()								{ _cleanup(); p = 0; owner = true; return &p; }
	T* operator->()								{ SAVASSERT(p != 0); return p; }
	T* operator=(T* pT)							{ _cleanup(); owner = (p = pT) != 0; return p; }
	SmartPtr<T>& operator=(SmartPtr<T>& srT)	{ _cleanup(); owner = srT.owner; p = srT.Detach(); return *this; }
	int operator!()								{ return (p == 0); }
	T* p;
private:
	void _cleanup(void)							{ if (owner && p) delete p; }
	int owner;
};

#endif //__SMARTPTR_H__