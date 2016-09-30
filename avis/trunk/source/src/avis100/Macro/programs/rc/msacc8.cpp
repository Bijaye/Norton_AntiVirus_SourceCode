// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "stdafx.h"
#include "msacc8.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// _ACCApplication properties

/////////////////////////////////////////////////////////////////////////////
// _ACCApplication operations


void _ACCApplication::NewCurrentDatabase(LPCTSTR filepath)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x85e, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 filepath);
}

void _ACCApplication::OpenCurrentDatabase(LPCTSTR filepath, BOOL Exclusive)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BOOL;
	InvokeHelper(0x85c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 filepath, Exclusive);
}


CString _ACCApplication::GetCurrentObjectName()
{
	CString result;
	InvokeHelper(0x7dc, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void _ACCApplication::CloseCurrentDatabase()
{
	InvokeHelper(0x85d, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _ACCApplication::Quit(long Option)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x7e0, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Option);
}


BOOL _ACCApplication::GetVisible()
{
	BOOL result;
	InvokeHelper(0x864, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void _ACCApplication::SetVisible(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x864, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

LPDISPATCH _ACCApplication::CurrentDb()
{
	LPDISPATCH result;
	InvokeHelper(0x801, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, NULL);
	return result;
}


