// XError.h: interface for the XError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XERROR_H__DF5E2DD9_0A64_11D3_ADE2_00A0C9C71BBC__INCLUDED_)
#define AFX_XERROR_H__DF5E2DD9_0A64_11D3_ADE2_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class XError  
{
public:
	static const char*	Abandoned()	{ return "abandoned"; }
	static const char*	Content()	{ return "content"; }
	static const char*	Crumbled()	{ return "crumbled"; }
	static const char*	Declined()	{ return "declined"; }
	static const char*	Internal()	{ return "internal"; }
	static const char*	Lost()		{ return "lost"; }
	static const char*	Malformed()	{ return "malformed"; }
	static const char*	Missing()	{ return "missing"; }
	static const char*	Overrun()	{ return "overrun"; }
	static const char*	Sample()	{ return "sample"; }
	static const char*	Superceded(){ return "superceded"; }
	static const char*	Type()		{ return "type"; }
	static const char*	Underrun()	{ return "underrun"; }
	static const char*	Unavailable(){return "unavailable"; }

private:
	XError();
	virtual ~XError();

};

#endif // !defined(AFX_XERROR_H__DF5E2DD9_0A64_11D3_ADE2_00A0C9C71BBC__INCLUDED_)
