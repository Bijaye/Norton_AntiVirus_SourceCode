// SampleStatus.h: interface for the SampleStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLESTATUS_H__CF749A19_098F_11D3_ADE1_00A0C9C71BBC__INCLUDED_)
#define AFX_SAMPLESTATUS_H__CF749A19_098F_11D3_ADE1_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SampleStatus  
{
public:
	SampleStatus();
	virtual ~SampleStatus();

	enum Status {
					quarantined	= 0,
					submitted	= 1,
					held		= 2,
					released	= 3,
					unneeded	= 4,
					needed		= 5,
					available	= 6,
					distributed	= 7,
					installed	= 8,
					attention	= 9,
					error		= 10,
					notInstalled= 11,
					restart		= 12,
					legacy		= 13,
					distribute	= 14,

					unknown		= -1
				};

	static const char*	Unknown()		{ return "unknown"; }
	static const char*	Quarantined()	{ return "quarantined"; }
	static const char*	Submitted()		{ return "submitted"; }
	static const char*	Held()			{ return "held"; }
	static const char*	Released()		{ return "released"; }
	static const char*	UnNeeded()		{ return "unneeded"; }
	static const char*	Needed()		{ return "needed"; }
	static const char*	Available()		{ return "available"; }
	static const char*	Distributed()	{ return "distributed"; }
	static const char*	Installed()		{ return "installed"; }
	static const char*	Attention()		{ return "attention"; }
	static const char*	Error()			{ return "error"; }
	static const char*	NotInstalled()	{ return "notInstalled"; }
	static const char*	Restart()		{ return "restart"; }
	static const char*	Legacy()		{ return "legacy"; }
	static const char*	Distribute()	{ return "distribute"; }

	static const char*	ToConstChar(Status status);
	static Status		ToStatus(const char* status);
	static Status		FromInt(const int asInt);
};

#endif // !defined(AFX_SAMPLESTATUS_H__CF749A19_098F_11D3_ADE1_00A0C9C71BBC__INCLUDED_)
