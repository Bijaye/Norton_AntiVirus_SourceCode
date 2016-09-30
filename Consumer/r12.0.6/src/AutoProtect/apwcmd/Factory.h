// Factory.h: interface for the CFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACTORY_H__A0922666_9CA0_49B7_AC01_B296A177DC00__INCLUDED_)
#define AFX_FACTORY_H__A0922666_9CA0_49B7_AC01_B296A177DC00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Forward declarations
class CApw95Cmd;

class CFactory : public IClassFactory 
{
public:
	CFactory( CApw95Cmd& cmdObj );
	virtual ~CFactory();

	// IUnknown methods.
	STDMETHOD(QueryInterface)( REFIID iid, void ** ppvObject );
	STDMETHOD_(ULONG,AddRef)();
	STDMETHOD_(ULONG,Release)();

	// IClassFactory methods.
	STDMETHOD(CreateInstance)( IUnknown * pUnkOuter, REFIID iid, void ** ppvObject );
	STDMETHOD(LockServer)( BOOL fLock );

private:
	// Disallowed.
	CFactory();
	CFactory( const CFactory& );

	// Reference count
	DWORD			m_dwRef;

	// Command Object
	CApw95Cmd&		m_cmdObj;
};

#endif // !defined(AFX_FACTORY_H__A0922666_9CA0_49B7_AC01_B296A177DC00__INCLUDED_)
