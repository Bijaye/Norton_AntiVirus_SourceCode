// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "stdafx.h"
#include "msppt8.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// _PPTApplication properties

/////////////////////////////////////////////////////////////////////////////
// _PPTApplication operations

LPDISPATCH _PPTApplication::GetPresentations()
{
	LPDISPATCH result;
	InvokeHelper(0x7d1, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH _PPTApplication::GetWindows()
{
	LPDISPATCH result;
	InvokeHelper(0x7d2, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH _PPTApplication::GetActiveWindow()
{
	LPDISPATCH result;
	InvokeHelper(0x7d4, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH _PPTApplication::GetActivePresentation()
{
	LPDISPATCH result;
	InvokeHelper(0x7d5, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

CString _PPTApplication::GetPath()
{
	CString result;
	InvokeHelper(0x7d8, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString _PPTApplication::GetName()
{
	CString result;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString _PPTApplication::GetCaption()
{
	CString result;
	InvokeHelper(0x7d9, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void _PPTApplication::SetCaption(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x7d9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}



void _PPTApplication::Quit()
{
	InvokeHelper(0x7e5, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}


long _PPTApplication::GetVisible()
{
	long result;
	InvokeHelper(0x7ee, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void _PPTApplication::SetVisible(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x7ee, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}



/////////////////////////////////////////////////////////////////////////////
// _Presentation properties

/////////////////////////////////////////////////////////////////////////////
// _Presentation operations



CString _Presentation::GetName()
{
	CString result;
	InvokeHelper(0x7e9, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString _Presentation::GetPath()
{
	CString result;
	InvokeHelper(0x7ea, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString _Presentation::GetFullName()
{
	CString result;
	InvokeHelper(0x7e8, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

long _Presentation::GetSaved()
{
	long result;
	InvokeHelper(0x7eb, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void _Presentation::SetSaved(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x7eb, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}


void _Presentation::Save()
{
	InvokeHelper(0x7f3, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void _Presentation::SaveAs(LPCTSTR FileName, long FileFormat, long EmbedTrueTypeFonts)
{
	static BYTE parms[] =
		VTS_BSTR VTS_I4 VTS_I4;
	InvokeHelper(0x7f4, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 FileName, FileFormat, EmbedTrueTypeFonts);
}

void _Presentation::SaveCopyAs(LPCTSTR FileName, long FileFormat, long EmbedTrueTypeFonts)
{
	static BYTE parms[] =
		VTS_BSTR VTS_I4 VTS_I4;
	InvokeHelper(0x7f5, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 FileName, FileFormat, EmbedTrueTypeFonts);
}

LPDISPATCH _Presentation::GetSlides()
{
	LPDISPATCH result;
	InvokeHelper(0x7db, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

void _Presentation::Close()
{
	InvokeHelper(0x7f7, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

LPDISPATCH _Presentation::GetSlideShowWindow()
{
	LPDISPATCH result;
	InvokeHelper(0x7ff, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// Presentations properties

/////////////////////////////////////////////////////////////////////////////
// Presentations operations

long Presentations::GetCount()
{
	long result;
	InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH Presentations::GetApplication()
{
	LPDISPATCH result;
	InvokeHelper(0x7d1, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}



LPDISPATCH Presentations::Item(const VARIANT& index)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		&index);
	return result;
}

LPDISPATCH Presentations::Add(long WithWindow)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x7d3, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		WithWindow);
	return result;
}

LPDISPATCH Presentations::Open(LPCTSTR FileName, long ReadOnly, long Untitled, long WithWindow)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_I4 VTS_I4 VTS_I4;
	InvokeHelper(0x7d4, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		FileName, ReadOnly, Untitled, WithWindow);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// Slides properties

/////////////////////////////////////////////////////////////////////////////
// Slides operations

long Slides::GetCount()
{
	long result;
	InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH Slides::Add(long index, long Layout)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x7d4, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		index, Layout);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// _Slide properties

/////////////////////////////////////////////////////////////////////////////
// _Slide operations

LPDISPATCH _Slide::GetApplication()
{
	LPDISPATCH result;
	InvokeHelper(0x7d1, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}



CString _Slide::GetName()
{
	CString result;
	InvokeHelper(0x7d8, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void _Slide::SetName(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x7d8, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

