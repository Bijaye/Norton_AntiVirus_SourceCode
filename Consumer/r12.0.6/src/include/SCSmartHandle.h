// SCSmartHandle.h: interface for the CSCSmartHandle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCSMARTHANDLE_H__98A3E6D6_30A6_4CEA_9650_EB1147F76D54__INCLUDED_)
#define AFX_SCSMARTHANDLE_H__98A3E6D6_30A6_4CEA_9650_EB1147F76D54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace NAVToolbox
{

// Class for Service Control handles. Closes when un-scoped.
//
class CSCSmartHandle  
{
public:
	CSCSmartHandle();
    CSCSmartHandle(SC_HANDLE h);
	virtual ~CSCSmartHandle();

    void CloseServiceHandle();
    SC_HANDLE CSCSmartHandle::Detach();
    const SC_HANDLE operator=(SC_HANDLE h);
    operator SC_HANDLE();

protected:
    SC_HANDLE m_handle;

};

} // namespace NAVToolbox

#endif // !defined(AFX_SCSMARTHANDLE_H__98A3E6D6_30A6_4CEA_9650_EB1147F76D54__INCLUDED_)
