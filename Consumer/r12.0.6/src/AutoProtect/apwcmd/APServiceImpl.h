// APServiceImpl.h: interface for the CAPServiceImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APSERVICEIMPL_H__82162077_3BD1_46BE_B510_8B4FFB686125__INCLUDED_)
#define AFX_APSERVICEIMPL_H__82162077_3BD1_46BE_B510_8B4FFB686125__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "navapsvc.h"

// Forward declarations
class CApw95Cmd;

class CAPServiceImpl : public INAVAPService
{
public:
	CAPServiceImpl( CApw95Cmd& cmdObj );
	virtual ~CAPServiceImpl();

	// IUnknown methods.
	STDMETHOD(QueryInterface)( REFIID iid, void ** ppvObject );
	STDMETHOD_(ULONG,AddRef)();
	STDMETHOD_(ULONG,Release)();

	// INAVAPService methods.
	STDMETHOD(RegisterHandler)( INAVAPEventHandler* pHandler );
	STDMETHOD(UnRegisterHandler)( INAVAPEventHandler* pHandler );
	STDMETHOD(GetAutoProtectEnabled)( BOOL* pbEnabled );
	STDMETHOD(SendCommand)( BSTR pCommand, VARIANT vArrayData );

private:
	// Disallowed.
	CAPServiceImpl();
	CAPServiceImpl( const CAPServiceImpl& );

	// Reference count
	DWORD			m_dwRef;

	// CMD object.
	CApw95Cmd&		m_cmdObj;
};

#endif // !defined(AFX_APSERVICEIMPL_H__82162077_3BD1_46BE_B510_8B4FFB686125__INCLUDED_)
