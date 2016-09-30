// QuarantineMonitor.h: interface for the QuarantineMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUARANTINEMONITOR_H__E4E7A0A4_C228_11D2_AD54_00A0C9C71BBC__INCLUDED_)
#define AFX_QUARANTINEMONITOR_H__E4E7A0A4_C228_11D2_AD54_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

#include <AVIS.h>

#include <CMclThread.h>
#include <CMclEvent.h>

#include "VQuarantineServer.h"

class QuarantineMonitor : public CMclThreadHandler  
{
public:
	QuarantineMonitor();
	virtual ~QuarantineMonitor();

	unsigned ThreadHandlerProc(void);

	bool	Pause(void);
	bool	Resume(void);


private:
	//
	//	Used to control the quarantine monitor.
	//	To stop the quarantine monitor:
	//		1)  unsignal monitorQuarantine
	//		2)	set stopMonitoringQuarantine to true
	//		3)  wait for quarantineMonitorNotActive to become signaled
	//
	//	To restart the quarantine monitor:
	//		1)	set stopMonitoringQuarantine to flase;
	//		2)	signal monitorQuarantine

	// manual event, true when it's okay to monitor
	CMclEvent	monitorQuarantine;

	// manual event, true while actually monitoring.
	CMclEvent	quarantineMonitorNotActive;

	// set to true when you want to stop the quarantine monitor.
	bool		stopMonitoringQuarantine;

  //	QSThreadLife		qsThreadLife;		// initializes com for this thread
  //    VQuarantineServer	quarantineServer;

	bool	LookForChanges(VQuarantineServer& quarantineServer);
	void	PrepNewSample(Sample& sample);
	void	FirstTime();

  void  preserveAttributes(Sample &sample);
};

class QMLock
{
public:
	QMLock(QuarantineMonitor& m) : qm(m) { qm.Pause(); }
	~QMLock() { qm.Resume(); }
private:
	QuarantineMonitor&	qm;
};

#endif // !defined(AFX_QUARANTINEMONITOR_H__E4E7A0A4_C228_11D2_AD54_00A0C9C71BBC__INCLUDED_)
