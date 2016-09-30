// MonitorDBForSigChanges.h: interface for the MonitorDBForSigChanges class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONITORDBFORSIGCHANGES_H__E3B0CEA9_904D_11D2_ACFF_00A0C9C71BBC__INCLUDED_)
#define AFX_MONITORDBFORSIGCHANGES_H__E3B0CEA9_904D_11D2_ACFF_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AVIS.h>
#include <CMclThread.h>

class MonitorDBForSigChanges : public CMclThreadHandler  
{
public:
	MonitorDBForSigChanges(uint initialSigSeqNum) : sigSeqNum(initialSigSeqNum) {};
	virtual ~MonitorDBForSigChanges() {};

	uint	ThreadHandlerProc();

private:
	uint	sigSeqNum;
};

#endif // !defined(AFX_MONITORDBFORSIGCHANGES_H__E3B0CEA9_904D_11D2_ACFF_00A0C9C71BBC__INCLUDED_)
