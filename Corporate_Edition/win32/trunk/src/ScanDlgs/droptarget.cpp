// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "stdafx.h"
#include "droptarget.h"


STDMETHODIMP CDropTarget::QueryInterface(REFIID riid,
							LPVOID FAR *ppv)
{
	 *ppv = NULL;
    // Any interface on this object is the object pointer
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDropTarget))
    {
        *ppv = (LPDROPTARGET)this;
        AddRef();
        return NOERROR;
    }
    return E_NOINTERFACE;
}


STDMETHODIMP_(ULONG) CDropTarget::AddRef()
{	
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) CDropTarget::Release()
{
	ULONG res = --m_cRefCount;
	if (m_cRefCount <=0) delete this;
	return res;
}


STDMETHODIMP CDropTarget::DragEnter(IDataObject *pDataObject,
							DWORD grfKeyState,	
							POINTL pt,	
							DWORD * pdwEffect)
{
	*pdwEffect = DROPEFFECT_MOVE;
	 return NOERROR;
}

STDMETHODIMP CDropTarget::DragLeave(void)
{
	 return NOERROR;
}
	
STDMETHODIMP CDropTarget::DragOver(DWORD grfKeyState,	
							POINTL pt,
							DWORD * pdwEffect)
{
	*pdwEffect = DROPEFFECT_MOVE;
	 return NOERROR;
}

STDMETHODIMP CDropTarget::Drop(IDataObject * pDataObject,	
						DWORD grfKeyState,
						POINTL pt,	
						DWORD * pdwEffect)
{
	*pdwEffect = DROPEFFECT_MOVE;
	 return NOERROR;
}