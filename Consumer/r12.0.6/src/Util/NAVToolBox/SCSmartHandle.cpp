// SCSmartHandle.cpp: implementation of the CSCSmartHandle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SCSmartHandle.h"

namespace NAVToolbox
{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSCSmartHandle::CSCSmartHandle()
{
    m_handle = NULL;
}

CSCSmartHandle::CSCSmartHandle(SC_HANDLE h)
{
    m_handle = h;
}

CSCSmartHandle::~CSCSmartHandle()
{
    CloseServiceHandle();
}

void CSCSmartHandle::CloseServiceHandle()
{
	if(m_handle)
	    ::CloseServiceHandle(m_handle);
    m_handle = NULL;
};

SC_HANDLE CSCSmartHandle::Detach()
{
	SC_HANDLE h =m_handle; 
	m_handle = NULL;
	return h; 
}

const SC_HANDLE CSCSmartHandle::operator=(SC_HANDLE h)
{
    CloseServiceHandle(); 
	m_handle = h; 
	return m_handle;
};

CSCSmartHandle::operator SC_HANDLE()
{
    return (SC_HANDLE)m_handle;
};

} // namespace