// DefinitionsMonitor.h: interface for the CDefinitionsMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFINITIONSMONITOR_H__9893E8E7_0F0F_4C62_95EC_6626B1F8D2F1__INCLUDED_)
#define AFX_DEFINITIONSMONITOR_H__9893E8E7_0F0F_4C62_95EC_6626B1F8D2F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GlobalEvents.h"
#include "GlobalEvent.h"    // For global XP happy events

class CDefinitionsMonitor : public ccLib::CThread 
{
public:
	CDefinitionsMonitor();
	virtual ~CDefinitionsMonitor();

	void Pause();
	void Resume();

    // CThread
	int Run();

private:
	// Disallowed
	CDefinitionsMonitor( CDefinitionsMonitor& other );

	enum {
		WAIT_INTERVAL = 1000 };

	enum {
		UPDATE_INTERVAL = 30000 };

private:
	// Pause flag
	bool						m_bPause;
};

#endif // !defined(AFX_DEFINITIONSMONITOR_H__9893E8E7_0F0F_4C62_95EC_6626B1F8D2F1__INCLUDED_)