// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////
//
//  WastFactory.H
//	
//	Definition of CWastFactory Object
//
//	Randy Templeton
//	Copywrite(C) 1996 Intel Coporation
//
///////////////////////////////////////////////////////////////////

#ifndef _DROPTARGET_H
#define _DROPTARGET_H


//////////////////////////////////////////////////////
//	
//
//
class CDropTarget : public IDropTarget
{
public:
	CDropTarget() : m_cRefCount(0) {}
	STDMETHODIMP QueryInterface(REFIID riid,
								 LPVOID FAR *ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP DragEnter(IDataObject * pDataObject,
							DWORD grfKeyState,	
							POINTL pt,	
							DWORD * pdwEffect);
	STDMETHODIMP DragLeave(void);
	STDMETHODIMP DragOver(DWORD grfKeyState,	
							POINTL pt,
							DWORD * pdwEffect);	
    STDMETHODIMP Drop(IDataObject * pDataObject,	
						DWORD grfKeyState,
						POINTL pt,	
						DWORD * pdwEffect );	
 

private:
	ULONG m_cRefCount;
  
};

#endif